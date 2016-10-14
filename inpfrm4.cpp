#include "inpfrm4.h"


InpFrm4 *InpFrm4::mInst = 0;

/* ======================================================================== */
/*                             InpFrm4::InpFrm4                             */
/* ======================================================================== */
InpFrm4::InpFrm4(const QList<MdTable *> tables, QWidget *parent)
	: QWidget(parent), mHasSrcTables(false) {
	mInst = this;
	browser = Browser::instance(this);

	/*!
	 * Instantiate multiple InpBoxWdg objects.
	 */
	for (int k = 0; k < 3; k++)
		inpBoxes << new InpBoxWdg(tr("Box #%1").arg(k), this);

	/*!
	 * Create relational table model instance and set a sql source table.
	 */
	sourceRtm = new SqlRtm(DbController::db(), this);

	prjProxy = new SfiltMdl();
	prjProxy->setDynamicSortFilter(true);
	prjProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

	if (! tables.isEmpty())
		setSourceTables(tables);

	connect(inpBoxes.at(0)->cbx(), &MdComboBox::editTextChanged,
			  this, &InpFrm4::onTextChanged);

	vbl = new QGridLayout(this);
	vbl->addWidget(inpBoxes.at(0), 0, 0);
	vbl->addWidget(inpBoxes.at(1), 0, 1);
	vbl->addWidget(inpBoxes.at(2), 0, 2);

	foreach (InpBoxWdg *w, inpBoxes)
		emit w->doAdjustSize();

	setLayout(vbl);
	hide();

	WIN_RESTORE(this);

}
void InpFrm4::onTextChanged(const QString &text) {
	MdComboBox *senderCb = qobject_cast<MdComboBox *>(sender());

	if (! senderCb->lineEdit()->selectedText().isEmpty())
		INFO << tr("selected:") << senderCb->lineEdit()->selectedText();

	if (! senderCb->currentText().isEmpty()) {
#ifdef USE_COMPLETER
		prjProxy->setFilterRegExp(text);
#else
		Q_UNUSED(text);
		prjProxy->setFilterRegExp(senderCb->getCompleter()->completionPrefix());
#endif
	}
	else
		prjProxy->setFilterRegExp(QString());

	inpBoxes.at(1)->cbx()->refreshView();
}
void InpFrm4::setSourceTables(const QList<MdTable *> tables) {
	QStringList allTabNames;

	foreach (MdTable *mdt, tables)
		allTabNames << mdt->sqlTableName();

	if (! (allTabNames.join(',').contains("client") &&
			 allTabNames.join(',').contains("prj") &&
			 allTabNames.join(',').contains("worker")) ) {
		CRIT << tr("One or more sql tables can't be found: ")
			  << allTabNames;
		return;
	}

	foreach (MdTable *mdt, tables) {
		if (mdt->sqlTableName() == tr("client"))
			inpBoxes.at(0)->setTable(mdt);

		if (mdt->sqlTableName() == tr("prj")) {
			prjProxy->setSourceModel(mdt->tv()->model());
			prjProxy->invalidate();
			inpBoxes.at(1)->cbx()->setModel(prjProxy);
			inpBoxes.at(1)->tv()->setModel(prjProxy);
		}

		if (mdt->sqlTableName() == tr("worker"))
			inpBoxes.at(2)->setTable(mdt);
	}

	setHasSrcTables(true);
	while (prjProxy->canFetchMore(QModelIndex()))
		prjProxy->fetchMore(QModelIndex());

	inpBoxes.at(0)->cbx()->setModelColumns(QList<short>() << 2 << 3 << 1);
	inpBoxes.at(1)->cbx()->setModelColumns(QList<short>() << 1 << 3);
	inpBoxes.at(2)->cbx()->setModelColumns(QList<short>() << 1 << 3 << 4);

	QComboBox *cbxa = new QComboBox(this);
	cbxa->setEditable(true);
	cbxa->setModel(prjProxy);
	cbxa->setModelColumn(2);

	vbl->addWidget(cbxa, 1, 0);

}
/* ======================================================================== */
/*                              Event handler                               */
/* ======================================================================== */
void InpFrm4::showEvent(QShowEvent *e) {
	QSETTINGS;
	restoreGeometry(config.value(objectName() + Md::k.windowGeometry).toByteArray());

	//	if (Browser::instance()->mTabs.currentSelected(true) != NULL)
	//		setSourceTable(Browser::instance()->mTabs.currentSelected());

	QWidget::showEvent(e);
}
void InpFrm4::hideEvent(QHideEvent *e) {
	QSETTINGS;
	config.setValue(objectName() + Md::k.windowGeometry, saveGeometry());
	INFO << objectName() + Md::k.windowGeometry << tr("close");
	QWidget::hideEvent(e);
}
void InpFrm4::closeEvent(QCloseEvent *e) {
	QSETTINGS;
	config.setValue(objectName() + Md::k.windowGeometry, saveGeometry());
	QWidget::closeEvent(e);
}
void InpFrm4::keyPressEvent(QKeyEvent *e) {
	/*!
		 * This event implements a second action key (Esc) to close the filterForm.
		 * The action MainWindow::actFilterForm is configured to show/hide the FilterWidget::
		 * instance without invocation of FilterWidget::keyPressEvent(). This could simply be
		 * reached by calling the FilterForms memberfunction filterForm::addAction(actFilterForm)
		 * and add the action object to the widgets action list.
		 * To manage propper QAction QShortcut:: operation for a shortcut that differs from
		 * the actFilterForm configured one, it is necessary to trigger the mainwindows QAction
		 * in the keyPressEvent instead of simply invoke this->close().
		 */
	if (e->key() == Qt::Key_Escape) {
		if (! listObjectNames<QAction *>(actions())->join(',').contains("actFilterForm")) {
			WARN << tr("this->actions() does not contain actFilterForm ");
			return QWidget::keyPressEvent(e);
		}
		else
			foreach (QAction *act, actions())
				if (act->objectName().contains(tr("actFilterForm")))
					act->setChecked(false);

		e->accept();
		return;
	}

	QWidget::keyPressEvent(e);
}



#define QFOLDINGSTART {
const QString InpBoxWdg::mStylesheetInp4 = QString(
			"/* ----------------------------------------------------------------------------------------------- */"
			"QComboBox, QLineEdit, QSpinBox, QDateEdit {	"
			"	border: 1px solid gray;"
			"	font: italic 10pt ""Arial"";"
			"	font-weight: bold;"
			"	color: rgb(70, 70, 70);"
			"}  [hasFocus=true] {"
			"	border: 2px solid red;"
			"}"
			"QRadioButton {"
			"	/*boarder: 2px solid red;*/"
			"}"
			"/* ----------------------------------------------------------------------------------------------- */"
			"QComboBox, QLineEdit, QSpinBox, QDateEdit {"
			"	border: 1px solid gray;"
			"	border-radius: 3px;"
			"	padding: 0px 3px 0px 3px;"
			"	min-width: 3em;"
			"} "
			"QComboBox:editable, QDateEdit:editable	{ "
			"	background: white;"
			"}"
			"QComboBox:!editable, QComboBox::drop-down:editable {"
			"	background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
			"	stop: 0 #E1E1E1, stop: 0.4 #DDDDDD,"
			"	stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);"
			"}"
			"/* QComboBox gets the ""on"" state when the popup is open */"
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
			"/*	image: url(://images/1downarrow13.png);*/"
			"	image: url(""://sortdown"");"
			"}"
			"QComboBox::down-arrow:on, QDateEdit::down-arrow:on { /* shift the arrow when popup is open */"
			"	top: 1px;"
			"	left: 1px;"
			"}"
			"/* ----------------------------------------------------------------------------------------------- */"
			"QGroupBox, QTextEdit {"
			"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E0E0E0, stop: 1 #FFFFFF);"
			"	border: 2px solid rgb(140,140,140);"
			"	border-radius: 5px;"
			"	margin-top: 2ex; "
			"}"
			"QGroupBox {"
			"	font: italic 10pt ""Arial"";"
			"	color: rgb(55,55,55);"
			"	font-weight: bold;"
			"}"
			"QGroupBox::title {"
			"	subcontrol-origin: margin; "
			"	subcontrol-position: top center; "
			"	top: 1.2ex;"
			"	padding: 0 8px"
			"}"
			""
			"/* ----------------------------------------------------------------------------------------------- */"
			"QLabel {"
			"	color: rgb(70, 70, 70);"
			"}"
			"/* ----------------------------------------------------------------------------------------------- */"
			"QDateEdit:!editable, QDateEdit::drop-down:editable {"
			"	background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
			"	stop: 0 #E1E1E1, stop: 0.4 #DDDDDD,"
			"	stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);"
			"} "
			"/* QDateEdit gets the ""on"" state when the popup is open */"
			"QDateEdit:!editable:on, QDateEdit::drop-down:editable:on {"
			"	background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
			"	stop: 0 #D3D3D3, stop: 0.4 #D8D8D8,"
			"	stop: 0.5 #DDDDDD, stop: 1.0 #E1E1E1);"
			"}"
			"QDockWidget {"
			"	border: 1px solid black;"
			"	titlebar-close-icon: url(close.png);"
			"	titlebar-normal-icon: url(undock.png);"
			"}"
			"QDockWidget::title {"
			"	text-align: left; /* align the text to the left */"
			"	background: rgb(148, 148, 148);"
			"	padding-left: 5px;"
			"}"
			"QDockWidget::close-button, QDockWidget::float-button {"
			"	border: 1px solid transparent;"
			"	background: darkgray;"
			"	padding: 0px;"
			"}"
			"QDockWidget::close-button:hover, QDockWidget::float-button:hover {"
			"	background: gray;"
			"}"
			"QDockWidget::close-button:pressed, QDockWidget::float-button:pressed {"
			"	padding: 1px -1px -1px 1px;"
			"}");

#define QFOLDINGEND }
