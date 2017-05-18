#include <QtGui>
#include <QtSql>

#include "basedataform.h"


BaseDataForm::BaseDataForm(int id, QTableView *tableView, QWidget *parent)
	: QDialog(parent), m_tableView(tableView), m_rowCountChanged(false),
	  m_activeTask(TaskNone) {

	setObjectName(tr("BaseDataForm_%1")
					  .arg(qobject_cast<MdTabView *>(tableView)->sqlTableName()));
	QSETTINGS;
	tableView->sortByColumn(0,Qt::DescendingOrder);

	/* ======================================================================== */
	/*                          Query table Schema                              */
	/* ======================================================================== */
	tableSchema = MdTableInfo::querySchema(qobject_cast<MdTabView *>(m_tableView)
														->sqlTableName(), DbController::db());

	/* ======================================================================== */
	/*                             Set table model                              */
	/* ======================================================================== */
	tableModel = qobject_cast<SqlRtm *>(tableView->model());
	tableModel->select();

	mapper = new QDataWidgetMapper(this);
	mapper->setModel(tableModel);
	mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
	mapper->setItemDelegate(new NullDelegate(this));

	/*!
	 * Set mapper index
	 */
	if (id != -1)
		for (int row = 0; row < tableModel->rowCount(); ++row) {
			QSqlRecord record = tableModel->record(row);
			if (record.value(ID_COLUMN).toInt() == id) {
				mapper->setCurrentIndex(row);
				break;
			}
		}
	else
		mapper->toFirst();

	/* ======================================================================== */
	/*                             Init ui elements                             */
	/* ======================================================================== */
	createUi();

	/* ======================================================================== */
	/*                          Connect button signals                          */
	/* ======================================================================== */
	connect(topBtnGrp,	SIGNAL(buttonClicked(QAbstractButton*)),
			  this,			SLOT(onTopButtonClicked(QAbstractButton*)));

	connect(btnFirst,	&QPushButton::clicked, mapper, &QDataWidgetMapper::toFirst);
	connect(btnPrev,	&QPushButton::clicked, mapper, &QDataWidgetMapper::toPrevious);
	connect(btnNext,	&QPushButton::clicked, mapper, &QDataWidgetMapper::toNext);
	connect(btnLast,	&QPushButton::clicked, mapper, &QDataWidgetMapper::toLast);

	connect(btnAdd,	&QPushButton::clicked, this, &BaseDataForm::addNew);
	connect(btnChange,&QPushButton::clicked, this, &BaseDataForm::modify);
	connect(btnDelete,&QPushButton::clicked, this, &BaseDataForm::deleteCurrent);
	connect(btnOk,		&QPushButton::clicked, this, &BaseDataForm::saveCurrent);
	connect(btnCancel,&QPushButton::clicked, this, &BaseDataForm::reject);

	(id == -1) ? btnNext->setFocus() : leEditors.first()->setFocus();

	setWindowTitle(tr("Stammdaten bearbeiten: %1")
						.arg(Md::tableAlias[ qobject_cast<MdTabView *>(
							tableView)->sqlTableName() ]));

	setEditorStates(false);
	setButtonStates(true);
	btnOk->setEnabled(false);

	setFont(qobject_cast<MdTabView *>(m_tableView)->font());
	setStyleSheet(STYLESHEET_SMALL);
	setAttribute(Qt::WA_DeleteOnClose);
	show();
}
void BaseDataForm::onTextEdited(const QString &text) {
	if (text.isEmpty())
		return;

	/*!
	 * Check if all mandatory (e.g. NotNull) editors has non-empty values
	 */
	foreach (QWidget *w, allEditors) {
		if (! w->property(Md::sqlConstraints.notNull).toBool())
			continue;
		if (getEditorText(w).isEmpty())
			return;
	}

	btnOk->setEnabled(true);
}
void BaseDataForm::populateComboBoxes() {
	foreach (QComboBox *cb, cbEditors)
		cb->setModelColumn(cb->property("modelColumn").toInt());
}
void BaseDataForm::addNew() {
	m_activeTask = TaskNew;

	/*!
	 * Revert all uncommited changes of active table.
	 */
	qobject_cast<SqlRtm *>(m_tableView->model())->revertAll();

	/*!
	 * Disable some buttons
	 */
	disableButtons();
	enableButtons(QList<QPushButton *>() << btnOk << btnCancel);
	setEditorStates(true);
	clearResetEditors();

	/*!
	 * Insert new table row after current index.
	 */
	int row = mapper->currentIndex() + 1;
	tableModel->insertRow(row);

	/*!
	 * Set data mapper index to the new row.
	 */
	mapper->setCurrentIndex(row);

	/*!
	 * Query all records of the active tables model to get the last AUTOINCREMENTed
	 * primary key value, increment by on and write the determined pk into its disabled
	 * editor.
	 */
	QSqlQuery q(tr("SELECT * from %1")
					.arg(qobject_cast<MdTabView *>(m_tableView)->sqlTableName()));
	QSqlRecord rec = q.record();
	q.last();

	if (rec.count() > 0)
		lePrimaryKey->setText(QString::number(q.value(0).toInt() + 1));
	else
		lePrimaryKey->setText("0");

	qobject_cast<QGridLayout *>(
				layout())->itemAtPosition(layout()->indexOf(lePrimaryKey), 1)
			->widget()->setFocus();

	mapper->submit();
	q.finish();

	m_rowCountChanged = true;
}
void BaseDataForm::modify() {
	m_activeTask = TaskModify;

	/*!
	 * Revert all uncommited changes of active table.
	 */
	qobject_cast<SqlRtm *>(m_tableView->model())->revertAll();

	/*!
	 * Disable some buttons
	 */
	disableButtons();
	enableButtons(QList<QPushButton *>() << btnOk << btnCancel);
	setEditorStates(true);
}
void BaseDataForm::deleteCurrent() {
	m_activeTask = TaskDelete;

	int row = mapper->currentIndex();
	tableModel->removeRow(row);

	if (mapper->submit()) {
		btnOk->setEnabled(true);
	}
	m_rowCountChanged = true;
	mapper->setCurrentIndex(qMin(row, tableModel->rowCount() - 1));
}
void BaseDataForm::saveCurrent() {
	foreach (QWidget *w, allEditors) {
		QString editorText = getEditorText(w);

		/*!
		 * Check all "notnull" marked editors to be NOT NULL. If some editor has invalied
		 * data, highlight it and return before invokation of model()->submitALL;
		 */
		if (w->property(Md::sqlConstraints.notNull).toBool()) {
			/*!
			 * Highlight, break and return if editor text is empty
			 */
			if (editorText.isEmpty()) {
				stateBar->showMessage(MSG_MANDATORY_EDITOR);
				Globals::setStylesheetProperty(w, "orangeBg", true);
				return;
			}
		}

		/*!
		 * Check editor value uniqueness.
		 */
		if (w->property(Md::sqlConstraints.unique).toBool()) {
			QSqlQuery q(tr("SELECT %2 from %1")
					.arg(qobject_cast<MdTabView *>(m_tableView)->sqlTableName())
					.arg(w->objectName()));

			QList<int> intValues;
			QList<QString> strValues;
			bool ok;

			while (q.next()) {
				if (q.value(0).canConvert<int>()) {
					intValues << q.value(0).toInt(&ok);
					if (! ok)
						intValues.removeLast();
					continue;
				}

				if (q.value(0).canConvert<QString>()) {
					strValues << q.value(0).toString();
					continue;
				}

				/*!
				 * Error handling
				 */
				INFO << RED_BG("QVariant conversion to \"int\" failed!");
				foreach (QWidget *w, allEditors)
					Globals::setStylesheetProperty(w, "orangeBg", true);
				return;
			}

			if (! intValues.isEmpty())	{
				editorText.toInt(&ok);
				if (ok) {
					if (intValues.contains(editorText.toInt())) {
						Globals::setStylesheetProperty(w, "orangeBg", true);
						return;
					}
				}
				else {
					INFO << RED_BG("intValues list is not empty but editorText.toInt(&ok) "
										"returns invalied conversion");
					return;
				}
			}

			if (! strValues.isEmpty()){
				if (strValues.contains(editorText)) {
					Globals::setStylesheetProperty(w, "orangeBg", true);
					return;
				}
			}
		}
	}

	/*!
	 * Submit all since this branch could only be reached if all editor values
	 * seems to be valid.
	 */
	if (! (mapper->submit() &&
			 static_cast<SqlRtm *>(m_tableView->model())->submitAll())) {
		/*!
		 * Error handling
		 */
		INFO << RED_BG("\"mapper->submit()\" returns false!");
		foreach (QWidget *w, allEditors)
			Globals::setStylesheetProperty(w, "orangeBg", true);
		return;
	}

	enableButtons();
	btnOk->setEnabled(false);
	populateComboBoxes();
	m_activeTask = TaskNone;
}
void BaseDataForm::done(int result) {
	QDialog::done(result);
}
void BaseDataForm::reject() {
	mapper->revert();
	static_cast<SqlRtm *>(m_tableView->model())->revertAll();

	/*!
	 * Enable all buttons
	 */
	enableButtons();

	QDialog::reject();
}
void BaseDataForm::onTopButtonClicked(QAbstractButton *button) {
	Q_UNUSED(button);
	qobject_cast<SqlRtm *>(m_tableView->model())->revertAll();
}
void BaseDataForm::onCyclic() {
	//	qobject_cast<SqlRtm *>(tableModel)->select();
}
/* ======================================================================== */
/*                                 Helpers                                  */
/* ======================================================================== */
void BaseDataForm::clearEditorBackgrounds() {
	foreach (QWidget *w, allEditors)
		Globals::setStylesheetProperty(w, "orangeBg", false);
}
void BaseDataForm::clearResetEditors(QList<QWidget *> editors) {
	if (editors.isEmpty())
		editors = allEditors;
	/*!
	 * Reset all editor values.
	 */
	foreach(QWidget *w, editors) {
		if (qobject_cast<QLineEdit *>(w)) {
			qobject_cast<QLineEdit *>(w)->clear();
			continue;
		}
		if (qobject_cast<QTextEdit *>(w)) {
			qobject_cast<QTextEdit *>(w)->clear();
			continue;
		}
		if (qobject_cast<QPlainTextEdit *>(w)) {
			qobject_cast<QPlainTextEdit *>(w)->clear();
			continue;
		}
		if (qobject_cast<QComboBox *>(w)) {
			qobject_cast<QComboBox *>(w)->setCurrentIndex(0);
			continue;
		}
	}
}
QString BaseDataForm::getEditorText(QWidget *editor) {
	if (qobject_cast<QLineEdit *>(editor))
		return qobject_cast<QLineEdit *>(editor)->text();
	else {
		if (qobject_cast<QComboBox *>(editor))
			return qobject_cast<QComboBox *>(editor)->currentText();
		else {
			if (qobject_cast<MdPlainTextEdit *>(editor))
				return qobject_cast<MdPlainTextEdit *>(editor)->toPlainText();
			else {
				INFO << tr("Class:") << editor->metaObject()->className();
				Q_ASSERT(0);
			}
		}
	}
}
/* ======================================================================== */
/*                               Button state                               */
/* ======================================================================== */
void BaseDataForm::createUi() {
	QSETTINGS;

	/* ======================================================================== */
	/*                          Create button objects                           */
	/* ======================================================================== */
	btnFirst = new QPushButton(tr("&<<"), this);
	btnPrev = new QPushButton(tr("< &zurück"), this);
	btnNext = new QPushButton(tr("&vor >"), this);
	btnLast = new QPushButton(tr("&>>"), this);

	btnAdd = new QPushButton(tr("&Neu"), this);
	btnChange = new QPushButton(tr("&Ändern"), this);
	btnDelete = new QPushButton(tr("&Löschen"), this);
	btnOk = new QPushButton(tr("&Ok"), this);
	btnCancel = new QPushButton(tr("A&bbruch"), this);

	btnFirst->setFixedWidth(30);
	btnLast->setFixedWidth(30);

	taskBtnBox = new QDialogButtonBox();
	taskBtnBox->addButton(btnAdd, QDialogButtonBox::ActionRole);
	taskBtnBox->addButton(btnChange, QDialogButtonBox::ActionRole);
	taskBtnBox->addButton(btnDelete, QDialogButtonBox::ActionRole);
	taskBtnBox->addButton(btnOk, QDialogButtonBox::ApplyRole);
	taskBtnBox->addButton(btnCancel, QDialogButtonBox::RejectRole);

	/* ======================================================================== */
	/*                      Create button and main layout                       */
	/* ======================================================================== */
	QHBoxLayout *topButtonLayout = new QHBoxLayout;
	topButtonLayout->setContentsMargins(20, 0, 20, 5);
	topButtonLayout->addStretch();
	topButtonLayout->addWidget(btnFirst);
	topButtonLayout->addWidget(btnPrev);
	topButtonLayout->addWidget(btnNext);
	topButtonLayout->addWidget(btnLast);
	topButtonLayout->addStretch();

	QGridLayout *mainLayout = new QGridLayout;
	int gridRow = 0;
	mainLayout->addLayout(topButtonLayout, gridRow++, 0, 1, 2);

	topBtnGrp = new QButtonGroup(this);
	topBtnGrp->addButton(btnFirst);
	topBtnGrp->addButton(btnPrev);
	topBtnGrp->addButton(btnNext);
	topBtnGrp->addButton(btnLast);

	botBtnGrp = new QButtonGroup(this);
	botBtnGrp->addButton(btnAdd);
	botBtnGrp->addButton(btnChange);
	botBtnGrp->addButton(btnDelete);
	botBtnGrp->addButton(btnOk);
	botBtnGrp->addButton(btnCancel);

	/* ======================================================================== */
	/*                          Create editor widgets                           */
	/* ======================================================================== */
	int k = 0;
	foreach (MdTableInfo::Column_t colSchema, tableSchema) {
		INFO << tr("tableModel->headerData(k, Qt::Horizontal):")
			  << tableModel->headerData(k, Qt::Horizontal).toString();

		/*!
		 * Check current table for some foreign key and create a QComboBox.
		 */
		if (tableModel->relationModel(k)) {
			QSqlTableModel *rtm = tableModel->relationModel(k);
#ifdef DEBUG_FOREIGN_KEY
			INFO << tr("Column %1 has foreign key for: %2/%3")
					  .arg(k)
					  .arg(rtm->tableName())
					  .arg(tableModel->headerData(k, Qt::Horizontal).toString());
#endif
			/*!
			 * Create foreign combobox editor and a label widget
			 */
			lblEditors << new QLabel(tableModel->headerData(k, Qt::Horizontal).toString());
			cbEditors << new QComboBox(this);
			allEditors << qobject_cast<QWidget *>(cbEditors.last());
			cbEditors.last()->setModel(rtm);
			cbEditors.last()->setEditable(false);

			if (colSchema.name.contains(tr("ID"))) {
				if (rtm->fieldIndex("Name") < 0) {
					/*!
					 * No field named  "Name" found.
					 */
					cbEditors.last()->setProperty("modelColumn", 1);
				}
				else
					cbEditors.last()->setProperty("modelColumn", rtm->fieldIndex("Name"));
			}
			populateComboBoxes();
		}
		else {
			lblEditors << new QLabel(colSchema.name);

			if (lblEditors.last()->text().contains("beschreibung", Qt::CaseInsensitive) ||
				 lblEditors.last()->text().contains("anschrift", Qt::CaseInsensitive) ||
				 lblEditors.last()->text().contains("adresse", Qt::CaseInsensitive)) {

				/*!
				 * Editor for "beschreibung", "anschrift" or "adresse"
				 */
				teEditors << new MdPlainTextEdit(this);
				allEditors << qobject_cast<QWidget *>(teEditors.last());
				teEditors.last()->setWordWrapMode(QTextOption::WordWrap);
				teEditors.last()->setTabChangesFocus(true);
				teEditors.last()->setUndoRedoEnabled(true);

				QSize size = teEditors.last()->document()->size().toSize();
				size.setHeight(allEditors.first()->height() * 3);
				size.setWidth(QWIDGETSIZE_MAX);
				teEditors.last()->setMaximumSize(size);
			}
			else {
				leEditors << new QLineEdit(this);
				allEditors << qobject_cast<QWidget *>(leEditors.last());

				connect(leEditors.last(),	&QLineEdit::textEdited,
						  this,					&BaseDataForm::onTextEdited);

				/*!
				 * Check if current ColumnSchema holds the primary key.
				 */
				if (colSchema.pk) {
					leEditors.last()->setEnabled(false);
					leEditors.last()->setProperty("primaryKey", true);
					lePrimaryKey = leEditors.last();
				}
				else {
					if (colSchema.type.contains(tr("INT"))) {
						/*!
						 * Check if a config value has been stored in settings ini.
						 */
						int upperValue = config.value(objectName() + tr("/%1").arg(colSchema.name),
																INT_VALIDATOR_UPPER).toInt();

						if (upperValue != INT_VALIDATOR_UPPER)
							INFO << config.fileName() << tr("has validator INT value for: %1 = %2")
									  .arg(objectName() + tr("/%1").arg(colSchema.name))
									  .arg(upperValue);

						leEditors.last()->setValidator(new QIntValidator(0, upperValue, this));
					}

					if (colSchema.type.contains(tr("REAL"))) {
						/*!
						 * Check if a config value has been stored in settings ini.
						 */
						qreal upperValue = config.value(objectName() + tr("/%1").arg(colSchema.name),
																  REAL_VALIDATOR_UPPER).toReal();

						if (upperValue != REAL_VALIDATOR_UPPER)
							INFO << config.fileName() << tr("has validator REAL value for: %1 = %2")
									  .arg(objectName() + tr("/%1").arg(colSchema.name))
									  .arg(upperValue);

						QDoubleValidator *dv = new QDoubleValidator(0, upperValue, 2, this);
						dv->setNotation(QDoubleValidator::StandardNotation);
						leEditors.last()->setValidator(dv);
					}
				}
			}
		}

		/*!
		 * Set editor buddy
		 */
		lblEditors.last()->setBuddy(allEditors.last());

		/*!
		 * Set editor buddys object name equal to it's label text
		 */
		allEditors.last()->setObjectName(lblEditors.last()->text());
		mapper->addMapping(allEditors.last(), k);

		mainLayout->addWidget(lblEditors.last(), gridRow, 0);
		mainLayout->addWidget(allEditors.last(), gridRow++, 1, 1, 1);

		/*!
		 * Place a "red star" if mandatory editor
		 */
		if (colSchema.notnull || colSchema.pk) {
			lblEditors.last()->setText(lblEditors.last()->text() +
												tr("<span style='color:#ff0000;'>*</span>"));
			lblEditors.last()->setTextFormat(Qt::RichText);
		}

		/*!
		 * Set SQL constraints as editor propertys
		 */
		allEditors.last()->setProperty(Md::sqlConstraints.notNull, colSchema.notnull);
		allEditors.last()->setProperty(Md::sqlConstraints.unique, colSchema.unique);

		k++;
	}

	stateBar = new QStatusBar(this);
	mainLayout->addWidget(taskBtnBox, gridRow++, 0, 1, 2);
	mainLayout->addWidget(stateBar, gridRow++, 0, 1, 2);

	setLayout(mainLayout);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}
void BaseDataForm::disableButtons(QList<QPushButton *> btnList) {
	if (btnList.isEmpty())
		btnList = this->findChildren<QPushButton *>();
	setButtonStates(false, btnList);
}
void BaseDataForm::disableButtons(QButtonGroup *btnGroup) {
	setButtonStates(false, listCast<QPushButton *>(btnGroup->buttons()));
}
void BaseDataForm::enableButtons(QList<QPushButton *> btnList) {
	if (btnList.isEmpty())
		btnList = this->findChildren<QPushButton *>();
	setButtonStates(true, btnList);
}
void BaseDataForm::enableButtons(QButtonGroup *btnGroup) {
	setButtonStates(true, listCast<QPushButton *>(btnGroup->buttons()));
}
void BaseDataForm::setButtonStates(bool state, QList<QPushButton *> btnList) {
	if (btnList.isEmpty())
		btnList = findChildren<QPushButton *>();

	foreach (QPushButton *btn, btnList)
		btn->setEnabled(state);
}
void BaseDataForm::setEditorStates(bool state, QList<QWidget *> editors) {
	if (editors.isEmpty())
		editors = allEditors;

	foreach (QWidget *w, editors)
		w->setEnabled(state);
}
/* ======================================================================== */
/*                              Event handler                               */
/* ======================================================================== */
void BaseDataForm::keyPressEvent(QKeyEvent *e) {
	if (e->key() == Qt::Key_F5) {
		static_cast<SqlRtm *>(m_tableView->model())->select();
		static_cast<SqlRtm *>(mapper->model())->select();
		return;
	}
	if (e->key() == Qt::Key_Escape) {
		QDialog::reject();
	}
	if (e->key() == Qt::Key_F4) {
		INFO << allEditors.at(0)->isEnabled();
		if (allEditors.at(0)->isEnabled())
			setEditorStates(false);
		else {
			setEditorStates(true);
			allEditors.at(2)->repaint();
		}
	}
}
void BaseDataForm::showEvent(QShowEvent *) {
	/*!
	 * Check if the selected table is one of the "base data tables" and warn
	 * if it isn't!
	 */
	MdTabView *mdtv = qobject_cast<MdTabView *>(m_tableView);
	if (! Locals::BASE_DATA_TABLE_NAMES.join(',').contains(mdtv->sqlTableName())) {
		mdtv->msgBoxSure.setWindowTitle(tr("Stammdaten editieren!"));
		mdtv->msgBoxSure.setText(
					tr("Die Tabelle ""%1"" gehört <b>nicht</b> zu den Stammdaten der Datenbank.\n"
						"Möchten Sie diese dennoch mit dem <b>Stammdaten-Editor</b> bearbeiten?")
					.arg(Md::tableAlias[mdtv->sqlTableName()]));
		if (mdtv->msgBoxSure.exec() != QMessageBox::Yes)
			QTimer::singleShot(100, Qt::CoarseTimer, this, &BaseDataForm::reject);
	}
}

#define QFOLDINGSTART {
const QString BaseDataForm::STYLESHEET_SMALL =
		QString("QComboBox,"
				  "QDateEdit"
				  "QLineEdit,"
				  "QSpinBox,"
				  "{"
				  "    border:         1px solid gray;"
				  "    border-radius:  3px;"
				  "    padding:        0px 3px 0px 3px;"
				  "    min-width:      3em;"
				  "    color:          rgb(70, 70, 70);"
				  "    background-color:     rgb(255, 255, 255);"
				  "}"
				  "[orangeBg=true] { background-color: rgb(255, 150, 150) }"
				  "[hasFocus=true] { border: 2px solid red }"
				  ""
				  ".QLineEdit:!enabled"
				  "{"
				  "    color:          rgb(120, 120, 120);"
				  "    background:     qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E1E1E1, stop: 0.4 #DDDDDD, stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);"
				  "}"
				  ""
				  "QComboBox:editable,"
				  "QDateEdit:editable"
				  "{ background: white }"
				  ""
				  "QComboBox:!editable,"
				  "QComboBox::drop-down:editable"
				  "{ background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E1E1E1, stop: 0.4 #DDDDDD, stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3); }"
				  "");

const QString BaseDataForm::STYLESHEET =
		QString("/* ----------------------------------------------------------------------------------------------- */"
				  "QComboBox, QLineEdit, QSpinBox, QDateEdit {	"
				  "	border:					1px solid gray;"
				  "	background-color:		rgb(255, 255, 255);"
				  "	color:					rgb(70, 70, 70);"
				  "}  [orangeBg=true] {"
				  "	background-color:		rgb(255, 100, 100);"
				  "}  [enabled=true] {"
				  "	background-color:		rgb(255, 255, 255);"
				  "}  [enabled=false] {"
				  "	background-color:		rgb(214, 214, 214);"
				  "}"
				  "/* ----------------------------------------------------------------------------------------------- */"
				  "QComboBox, QLineEdit, QSpinBox, QDateEdit {"
				  "	border:					1px solid gray;"
				  "	border-radius:			3px;"
				  "	padding:					0px 3px 0px 3px;"
				  "	min-width:				3em;"
				  "} "
				  "QComboBox:editable, QDateEdit:editable	{ "
				  "	background: white;"
				  "}"
				  "QComboBox:!editable, QComboBox::drop-down:!editable {"
				  "	background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
				  "	stop: 0 #E1E1E1, stop: 0.4 #DDDDDD,"
				  "	stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);"
				  "}"
				  "/* QComboBox gets the on state when the popup is open */"
				  "QComboBox:!editable:on, QComboBox::drop-down:editable:on {"
				  "	background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
				  "	stop: 0 #D3D3D3, stop: 0.4 #D8D8D8,"
				  "	stop: 0.5 #DDDDDD, stop: 1.0 #E1E1E1);"
				  "}"
				  "/* shift the text when the popup opens */"
				  "QComboBox:on, QDateEdit:on {"
				  "	padding-top: 3px;"
				  "	padding-left: 4px;"
				  "}"
				  "QComboBox::drop-down, QDateEdit::drop-down {"
				  "	subcontrol-origin: padding;"
				  "	subcontrol-position: top right;"
				  "	width: 15px;"
				  "	border-left-width: 1px;"
				  "	border-left-color: darkgray;"
				  "	border-left-style: solid; /* just a single line */"
				  "	border-top-right-radius: 3px; /* same radius as the QDateEdit */"
				  "	border-bottom-right-radius: 3px;"
				  "	color: rgb(70, 70, 70);"
				  "}"
				  "QComboBox::down-arrow, QDateEdit::down-arrow {"
				  "	image: url(""://sortdown"");"
				  "}"
				  "/* shift the arrow when popup is open */"
				  "QComboBox::down-arrow:on, QDateEdit::down-arrow:on {	"
				  "	top: 1px;"
				  "	left: 1px;"
				  "}");

const QString BaseDataForm::MSG_MANDATORY_EDITOR =
		QString("*Pflichtfeld!");
#define QFOLDINGEND }
