#include <QtGui>
#include <QtSql>

#include "basedataform.h"


BaseDataForm::BaseDataForm(int id, QTableView *tableView, QWidget *parent)
	: QDialog(parent), m_tableView(tableView), m_rowCountChanged(false) {
	setObjectName(tr("BaseDataForm_%1")
					  .arg(qobject_cast<MdTabView *>(tableView)->sqlTableName()));
	QSETTINGS;

	/* ======================================================================== */
	/*                          Create button objects                           */
	/* ======================================================================== */
	firstButton = new QPushButton(tr("<<"));
	previousButton = new QPushButton(tr("< &zurück"));
	nextButton = new QPushButton(tr("&vor >"));
	lastButton = new QPushButton(tr(">>"));

	addButton = new QPushButton(tr("&Hinzufügen"));
	deleteButton = new QPushButton(tr("&Löschen"));
	saveButton = new QPushButton(tr("&Anwenden"));
	closeButton = new QPushButton(tr("A&bbruch"));

	firstButton->setFixedWidth(30);
	lastButton->setFixedWidth(30);

	buttonBox = new QDialogButtonBox;
	buttonBox->addButton(addButton, QDialogButtonBox::ResetRole);
	buttonBox->addButton(deleteButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(saveButton, QDialogButtonBox::ApplyRole);
	buttonBox->addButton(closeButton, QDialogButtonBox::RejectRole);

	/* ======================================================================== */
	/*                             Set table model                              */
	/* ======================================================================== */
	tableModel = qobject_cast<SqlRtm *>(tableView->model());
	tableModel->select();

	mapper = new QDataWidgetMapper(this);
	mapper->setModel(tableModel);
	//	mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
	//		mapper->setItemDelegate(new QSqlRelationalDelegate(this));
	mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
	mapper->setItemDelegate(new NullDelegate(this));

	/* ======================================================================== */
	/*                        Query a ColumnSchema list                         */
	/* ======================================================================== */
	QList<MdTableInfo::TableInfo_column_t> columnSchemas;
	columnSchemas = MdTableInfo::queryInfo(qobject_cast<MdTabView *>(m_tableView)
								->sqlTableName(), DbController::db());

	/* ======================================================================== */
	/*                      Create button and main layout                       */
	/* ======================================================================== */
	QHBoxLayout *topButtonLayout = new QHBoxLayout;
	topButtonLayout->setContentsMargins(20, 0, 20, 5);
	topButtonLayout->addStretch();
	topButtonLayout->addWidget(firstButton);
	topButtonLayout->addWidget(previousButton);
	topButtonLayout->addWidget(nextButton);
	topButtonLayout->addWidget(lastButton);
	topButtonLayout->addStretch();

	QGridLayout *mainLayout = new QGridLayout;
	int gridRow = 0;
	mainLayout->addLayout(topButtonLayout, gridRow++, 0, 1, 2);

	int k = 0;
	foreach (MdTableInfo::TableInfo_column_t cs, columnSchemas) {
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
			lblEditors << new QLabel(tableModel->headerData(k, Qt::Horizontal).toString());
			cbEditors << new QComboBox(this);
			allEditors << qobject_cast<QWidget *>(cbEditors.last());
			cbEditors.last()->setModel(rtm);
			cbEditors.last()->setEditable(true);

			//			cbEditors.last()

//			if (lblEditors.last()->text().contains(tr("Name")))
//				cbEditors.last()->setProperty("modelColumn", rtm->fieldIndex("Name"));
//			else {
//				if (lblEditors.last()->text().contains(tr("Einstufung")))
//					cbEditors.last()->setProperty("modelColumn", rtm->fieldIndex("Einstufung"));
//				else
//					cbEditors.last()->setProperty("modelColumn", 0);
//			}

//			if (lblEditors.last()->text().contains(tr("Name"))) {
//				INFO << tableModel->headerData(k, Qt::Horizontal, Qt::DisplayRole).toString();
//				INFO << cs.name;
//				cbEditors.last()->setProperty("modelColumn", rtm->fieldIndex(
//															lblEditors.last()->text()));
//			}

			if (cs.name.contains(tr("ID"))) {
				INFO << tableModel->headerData(k, Qt::Horizontal, Qt::DisplayRole).toString();
				INFO << cs.name;
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
			lblEditors << new QLabel(cs.name);

			if (lblEditors.last()->text().contains("beschreibung", Qt::CaseInsensitive) ||
				 lblEditors.last()->text().contains("anschrift", Qt::CaseInsensitive) ||
				 lblEditors.last()->text().contains("adresse", Qt::CaseInsensitive)) {
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

				/*!
			 * Check if current ColumnSchema holds the primary key.
			 */
				if (cs.pk) {
					leEditors.last()->setEnabled(false);
					leEditors.last()->setProperty("primaryKey", true);
					lePrimaryKey = leEditors.last();
				}
				else {
					if (cs.type.contains(tr("INT"))) {
						/*!
				 * Check if a config value has been stored in settings ini.
				 */
						int upperValue = config.value(objectName() + tr("/%1").arg(cs.name),
																INT_VALIDATOR_UPPER).toInt();

						if (upperValue != INT_VALIDATOR_UPPER)
							INFO << config.fileName() << tr("has validator INT value for: %1 = %2")
									  .arg(objectName() + tr("/%1").arg(cs.name))
									  .arg(upperValue);

						leEditors.last()->setValidator(new QIntValidator(0, upperValue, this));
					}

					if (cs.type.contains(tr("REAL"))) {
						/*!
				 * Check if a config value has been stored in settings ini.
				 */
						qreal upperValue = config.value(objectName() + tr("/%1").arg(cs.name),
																  REAL_VALIDATOR_UPPER).toReal();

						if (upperValue != REAL_VALIDATOR_UPPER)
							INFO << config.fileName() << tr("has validator REAL value for: %1 = %2")
									  .arg(objectName() + tr("/%1").arg(cs.name))
									  .arg(upperValue);

						QDoubleValidator *dv = new QDoubleValidator(0, upperValue, 2, this);
						dv->setNotation(QDoubleValidator::StandardNotation);
						leEditors.last()->setValidator(dv);
					}
				}
			}
		}

		lblEditors.last()->setBuddy(allEditors.last());
		mapper->addMapping(allEditors.last(), k);
		//		dataMaps << new DataMap(allEditors.last(), k);
		mainLayout->addWidget(lblEditors.last(), gridRow, 0);
		mainLayout->addWidget(allEditors.last(), gridRow++, 1, 1, 1);

		if (cs.notnull || cs.pk) {
			lblEditors.last()->setText(lblEditors.last()->text() +
												tr("<span style='color:#ff0000;'>*</span>"));
			lblEditors.last()->setTextFormat(Qt::RichText);
			allEditors.last()->setProperty("notnull", true);
		}
		k++;
	}

	//	refreshMapper();

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

	topButtonGroup = new QButtonGroup(this);
	topButtonGroup->addButton(firstButton);
	topButtonGroup->addButton(previousButton);
	topButtonGroup->addButton(nextButton);
	topButtonGroup->addButton(lastButton);

	connect(topButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
			  this,				SLOT(onTopButtonClicked(QAbstractButton*)));

	connect(firstButton,		&QPushButton::clicked, mapper, &QDataWidgetMapper::toFirst);
	connect(previousButton, &QPushButton::clicked, mapper, &QDataWidgetMapper::toPrevious);
	connect(nextButton,		&QPushButton::clicked, mapper, &QDataWidgetMapper::toNext);
	connect(lastButton,		&QPushButton::clicked, mapper, &QDataWidgetMapper::toLast);

	connect(addButton,		&QPushButton::clicked, this, &BaseDataForm::addNew);
	connect(deleteButton,	&QPushButton::clicked, this, &BaseDataForm::deleteCurrent);
	connect(saveButton,		&QPushButton::clicked, this, &BaseDataForm::saveCurrent);
	connect(closeButton,		&QPushButton::clicked, this, &BaseDataForm::reject);

	stateBar = new QStatusBar(this);
	mainLayout->addWidget(buttonBox, gridRow++, 0, 1, 2);
	mainLayout->addWidget(stateBar, gridRow++, 0, 1, 2);

	//	mainLayout->setRowMinimumHeight(6, 10);
	//		mainLayout->setRowStretch(6, 1);
	//		mainLayout->setColumnStretch(2, 1);
	setLayout(mainLayout);

	(id == -1)
			? nextButton->setFocus()
			: leEditors.first()->setFocus();

	setWindowTitle(tr("Stammdaten bearbeiten: %1")
						.arg(Md::tableAlias[ qobject_cast<MdTabView *>(
							tableView)->sqlTableName() ]));

	setFont(qobject_cast<MdTabView *>(m_tableView)->font());
	setStyleSheet(STYLESHEET);
	show();
}
void BaseDataForm::populateComboBoxes() {
	foreach (QComboBox *cb, cbEditors)
		cb->setModelColumn(cb->property("modelColumn").toInt());
}
void BaseDataForm::onCyclic() {
	//	qobject_cast<SqlRtm *>(tableModel)->select();
}
void BaseDataForm::addNew() {
	qobject_cast<SqlRtm *>(m_tableView->model())->revertAll();

	/*!
	 * Reset all editor values.
	 */
	foreach(QWidget *w, allEditors) {
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
	}

	foreach(QComboBox *cb, cbEditors)
		cb->setCurrentIndex(0);

	/*!
	 * Insert new table row before current index.
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

	m_rowCountChanged = true;
	//	Browser *browser = Browser::instance();
	//	emit browser->sqlTableDataChanged();
}
void BaseDataForm::deleteCurrent() {
	int row = mapper->currentIndex();
	tableModel->removeRow(row);
	mapper->submit();
	m_rowCountChanged = true;
	//	mapper->setCurrentIndex(qMin(row, tableModel->rowCount() - 1));
}
void BaseDataForm::saveCurrent() {
	int mapperIndex = mapper->currentIndex();

	mapper->submit();
	QList<QWidget *> allEditors = QList<QWidget *>()
											<< listCast<QWidget *>(leEditors)
											<< listCast<QWidget *>(cbEditors);
	/*!
	 * Check all "notnull" marked editors to be NOT NULL. If some editor has invalied
	 * data, highlight it and do not invoke model()->submitALL;
	 */
	foreach (QWidget *w, allEditors) {
		if (w->property("notnull").toBool()) {
			QString editorText;

			if (qobject_cast<QLineEdit *>(w))
				editorText = qobject_cast<QLineEdit *>(w)->text();
			else {
				if (qobject_cast<QComboBox *>(w))
					editorText = qobject_cast<QComboBox *>(w)->currentText();
				else
					Q_ASSERT(0);
			}

			if (editorText.isEmpty()) {
				stateBar->showMessage(MSG_MANDATORY_EDITOR);
				Globals::setStylesheetProperty(w, "orangeBg", true);
				return;
			}
		}
	}
	static_cast<SqlRtm *>(m_tableView->model())->submitAll();
	//	refreshMapper();
	populateComboBoxes();

	if (! m_rowCountChanged)
		mapper->setCurrentIndex(mapperIndex);
	else {
		m_rowCountChanged = false;
		mapper->toLast();
	}
}
void BaseDataForm::done(int result) {
	QDialog::done(result);
}
void BaseDataForm::reject() {
	mapper->revert();
	static_cast<SqlRtm *>(m_tableView->model())->revertAll();
	QDialog::reject();
}
void BaseDataForm::clearEditorBackgrounds() {
	QList<QWidget *> allEditors = QList<QWidget *>()
											<< listCast<QWidget *>(leEditors)
											<< listCast<QWidget *>(cbEditors);
	foreach (QWidget *w, allEditors)
		Globals::setStylesheetProperty(w, "orangeBg", false);
}
void BaseDataForm::onTopButtonClicked(QAbstractButton *button) {
	Q_UNUSED(button);
	qobject_cast<SqlRtm *>(m_tableView->model())->revertAll();
}
/* ======================================================================== */
/*                              Event handler                               */
/* ======================================================================== */
void BaseDataForm::keyPressEvent(QKeyEvent *e) {
	if (e->key() == Qt::Key_F5) {
		static_cast<SqlRtm *>(m_tableView->model())->select();
		static_cast<SqlRtm *>(mapper->model())->select();
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
const QString BaseDataForm::STYLESHEET =
		QString("/* ----------------------------------------------------------------------------------------------- */"
				  "QComboBox, QLineEdit, QSpinBox, QDateEdit {	"
				  "	border:					1px solid gray;"
				  //				  "	background-color: rgb(255, 255, 255);"
				  "	color:					rgb(70, 70, 70);"
				  "}  [orangeBg=true] {"
				  "	background-color:		rgb(255, 217, 137);"
				  "}	[enabled=""false""] {"
				  "	color:					rgb(120, 120, 120);"
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
