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

	mCb = new QCheckBox(this);
	mCb->setChecked(false);

	connect(inpBoxes.at(0)->cbx(), &MdComboBox::editTextChanged,
			  this, &InpFrm4::onTextChanged);

	vbl = new QGridLayout(this);
	vbl->addWidget(inpBoxes.at(0), 0, 0);
	vbl->addWidget(inpBoxes.at(1), 0, 1);
	vbl->addWidget(inpBoxes.at(2), 0, 2);
	vbl->addWidget(mCb, 1, 2, 1, 1);

	foreach (InpBoxWdg *w, inpBoxes)
		emit w->doAdjustSize();

	setLayout(vbl);
	hide();

	WIN_RESTORE(this);


}
void InpFrm4::onTextChanged(const QString &text) {
	MdComboBox *senderCb = qobject_cast<MdComboBox *>(sender());
	if (! senderCb) {
		CRIT << tr("Cast failed!");
		return;
	}

	if (! senderCb->lineEdit()->selectedText().isEmpty())
		INFO << tr("selected:") << senderCb->lineEdit()->selectedText();

	INFO << tr("QCompleter::currentCompleteion(): %1")
			  .arg(senderCb->completer()->currentCompletion());

	/**
	 * If textChange signal emitted with a completer string filled into the lineedit,
	 * or if user has entered a comma, the "text" argument couldn't be used as valid
	 * filter String...
	 */
	if (! senderCb->currentText().isEmpty()) {
#ifdef USE_COMPLETER
		prjProxy->setFilterFixedString(text);

#else
		Q_UNUSED(text);
		INFO << tr("changed text: ") << GN_BG(text)
			  << tr("completion prefix: ") << GN_BG(senderCb->getCompleter()->completionPrefix());
		if (mCb->isChecked())
			prjProxy->setFilterFixedString(senderCb->getCompleter()->completionPrefix());
		else
			prjProxy->setFilterFixedString(text);
		prjProxy->setFilterRegExp(text);
#endif
	}
	else
		prjProxy->setFilterFixedString(QString());

	/**
	 * TODO:	Maybe emit a doRefresh() for all InpBoxWdg?!
	 */
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
		if (mdt->sqlTableName() == tr("prj")) {
			prjProxy->setSourceModel(mdt->tv()->model());
			prjProxy->invalidate();
			inpBoxes.at(0)->cbx()->setModel(prjProxy);
			inpBoxes.at(0)->tv()->setModel(prjProxy);
			inpBoxes.at(0)->setTitle(mdt->sqlTableName());
		}

		if (mdt->sqlTableName() == tr("client")) {
			inpBoxes.at(1)->setTable(mdt);
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

//	foreach (InpBoxWdg *ibw, inpBoxes)
//		ibw->cbx()->completer()->setCompletionMode(QCompleter::UnfilteredPopupCompletion);

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


/* ======================================================================== */
/*                           InpBoxWdg::InpBoxWdg                           */
/* ======================================================================== */
InpBoxWdg::InpBoxWdg(const QString &title, QWidget *parent)
	: QGroupBox(parent) {
	mCbx = new MdComboBox();
	mLbl = new QLabel(tr("spacer"));
	mGl = new QGridLayout();
	mTv = new MdTabView(QString());

	mGl->addWidget(mCbx, 0, 1, 1, 1);
	mGl->addWidget(mLbl, 1, 1, 1, 1);
	mGl->addWidget(mTv, 2, 1, -1, 1);

	mLbl->setAlignment(Qt::AlignCenter);
	mLbl->setFrameShape(QFrame::Box);
	mLbl->setMaximumHeight(30);
	setLayout(mGl);
	setTitle(title);

	QCompleter *completer = new QCompleter(this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setFilterMode(Qt::MatchContains);
	mCbx->setCompleter(completer);
	connect(this, &InpBoxWdg::doAdjustSize, this, &InpBoxWdg::onDoAdjustSize);

	adjustSize();
	setStyleSheet(mStylesheetInp4);
}
void InpBoxWdg::setTable(MdTable *table) {
	INFO << table->sqlTableName();
	setTitle(table->sqlTableName());
	mCbx->setModel(table->tv()->model());
	mTv->setModel(table->tv()->model());
	mTv->resizeRowsColsToContents();

	/**
	 * TODO: Is writeable a must have?
	 */
	connect(mTv->model(), &QAbstractItemModel::dataChanged,
			  this, &InpBoxWdg::onTvModelDataChanged);
}
void InpBoxWdg::onTvModelDataChanged(const QModelIndex, const QModelIndex,
												 const QVector<int>) {
	mCbx->setModel(mTv->model());
}
void InpBoxWdg::onDoAdjustSize() {
	mCbx->adjustSize();
}
