#include "mainwindow.h"
#include "ui_mainwindow.h"

#define	ON_CYCLIC_INTERVAL_MS	250

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
	ui(new Ui::MainWindow) {
	ui->setupUi(this);

	QSETTINGS;
	//	initDocks();


	stateBar		= new MDStateBar(this);
	browser		= new Browser(parent);
	mDbc			= new DbController(this);
	inpFrm		= new InpFrm(this);
	notes.toDo	= new MDNotes(tr("toDo"), parent);
	richEditor	= new TextEdit(this);
	richEditor->hide();
	filterForm	= new FilterForm();
	filterForm->hide();

	createActions();
	connectActions(connectThis);

	connect(browser->connectionWidget(),	SIGNAL(detailesChanged(QString&)),
			  stateBar,								SLOT(showInfo(QString&)));
	connect(notes.toDo,							SIGNAL(changed(const QString&)),
			  stateBar,								SLOT(showMessage2sec(const QString&)));
	connect(browser->connectionWidget(),	&ConnectionWidget::metaDataRequested,
			  browser,								&Browser::onConnectWdgMetaDataReq);


	QVBoxLayout *centralLayout = new QVBoxLayout(parent);
	centralLayout->addWidget(browser);
	centralLayout->addWidget(notes.toDo);

	mCentralWidget = new QWidget(parent);
	mCentralWidget->setLayout(centralLayout);

	browser->show();
	notes.toDo->hide();
	setCentralWidget(mCentralWidget);

	/* ======================================================================== */
	/*              InpFrm:: Dock location/window geometry restore              */
	/* ======================================================================== */
	inpFrm->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	Qt::DockWidgetArea dockArea =
			static_cast<Qt::DockWidgetArea>(
				config.value(inpFrm->objectName() + tr("/") +
								 KEY_DOCKWIDGETAREA, Qt::TopDockWidgetArea).toInt());
	addDockWidget(dockArea, inpFrm);

	/* ======================================================================== */
	/*                             stateBar config                              */
	/* ======================================================================== */
	stateBar->setSlInfoAlign(Qt::AlignCenter);
	stateBar->setClockAlign(Qt::AlignCenter);
	stateBar->setClockVisible(true);

	setStatusBar( stateBar );
	makeMenu();

	/*!
	 * Connect the inpFrm's statusMessage signal to the statebar message slot
	 */
	connect(inpFrm, &InpFrm::stateMessage, stateBar, &MDStateBar::showMessage);

	/*!
	  * Connect InpFrm::newWorktimeRecord() to TabView::update()
	  */
	foreach (TabView *tv, browser->mTabs.tvsNoPtr())
		connect(inpFrm, &InpFrm::newWorktimeRecord, tv, &TabView::refreshView);

	/**** Restore mainWindow geometry and window state
	\*/
	WIN_RESTORE(this);
	WIN_STATE_RESTORE(this);

	/**** Restore splitter sizes to config
	\*/
	SPLT_RESTORE(this);

	//	installEventFilter(this);

	setWindowTitle( MAINWINDOW_TITLE );
	connectActions(connectOthers);

	timCyc = new QTimer(this);
	timCyc->setInterval(ON_CYCLIC_INTERVAL_MS);
	timCyc->connect(timCyc, SIGNAL(timeout()), this, SLOT(onCyclic()));
	timCyc->start();

	QTimer::singleShot(50, this, SLOT(restoreActionObjects()));
}
MainWindow::~MainWindow() {
	delete ui;
}
void MainWindow::onInpFrmButtonClick(bool ) {

}
void MainWindow::onResizerDlgTrig() {
	bool ok;
	QString text =  QInputDialog::
						 getText(this, tr("QInputDialog::getText()"),
									tr("Resizer value in %:"), QLineEdit::Normal,
									tr("ss"), &ok);

	if (ok && !text.isEmpty())
		resize(QDesktopWidget().availableGeometry(this)
				 .size() * text.toDouble() / 100);
}
void MainWindow::onMenuStyleShtATrig(bool b) {
	if (!b) return;

	actGbSShtInpFrm->setChecked( !b );
	actGbStyleShtA->setChecked( b );

	foreach (TabView *tv, *browser->tvs()) {
		tv->setStyleSheet( Globals::gbStyleShtCenterPROPERTYS);
	}
}
void MainWindow::onUnderConstrTrig() {

	browser->mTabs.tva->setColumnHidden(0, true);
	browser->mTabs.tva->setColumnHidden(2, false);
	browser->mTabs.tva->setColumnHidden(3, true);
	return;
	INFO << actShowSqlQuery->isCheckable() << actShowSqlQuery->isChecked();
	actShowSqlQuery->setCheckable(true);
	actShowSqlQuery->setChecked(true);

	return;
#define QFOLDINGSTART {

	wid = new QWidget();
	wid->hide();

	QList<QDockWidget *> docks;

	for (int i = 0; i < 5; i++) {
		docks.append( new QDockWidget(wid) );
	}

	//   QDockAreaLayout
	//   return;
	//   form = new Form();
	//   form->show();
	//   browser->resetStyleSheet(*browser->tvs()[0]);
	//   browser->resetStyleSheet(*browser->tvs()[1]);
	//   browser->resetStyleSheet(*browser->tvs()[3]);
	//   browser->clearStyleSheet(*browser->tvs()[0]);
	//   browser->clearStyleSheet(*browser->tvs()[2]);
	//
#define QFOLDINGEND }
}
void MainWindow::onCyclic() {
	if (false) {
		QList<QAction *> acts = findChildren<QAction *>(QRegularExpression("act*"));
		QList<int> ba;
		foreach (QAction *act, acts)
			ba << (int)act->isChecked();
		INFO << ba;
	}
	/* ---------------------------------------------------------------- */
	/*         Get the name of object under the mouse pointer           */
	/* ----------------------------------------------------------------*/
	if (actGrTbMenu->actions().at(
			 actGrTbMenu->actions().indexOf(actCyclicObjInfo))->isChecked()) {
		QWidget *widget = qApp->widgetAt(QCursor::pos());

		if (widget != 0x00) {
			QString widName = widget->objectName();
			if (!widName.isEmpty())
				INFO << widName << widget->children();
		}
	}
}
void MainWindow::onActCloseTrig() {
	//    qApp->closeAllWindows();
	browser->close();
	inpFrm->close();
	close();
}
void MainWindow::onActSaveTrig() {

}
void MainWindow::onActOpenTrig() {

}
void MainWindow::onActionGroupTrigd(QAction *sender) {
	QSETTINGS;

	if (sender->isCheckable()) {
		/*!
		 * If action group is exclusive, clear all action config flags.
		 */
		if (sender->actionGroup()->isExclusive())
			foreach(QAction *act, sender->actionGroup()->actions())
				config.setValue(objectName() + tr("/") + act->objectName(), false);

		config.setValue(objectName() + tr("/") + sender->objectName(), sender->isChecked());
		config.sync();
	}
}
void MainWindow::onActRichTextToggd(bool) {
	//	QCommandLineParser parser;
	//	parser.setApplicationDescription(QCoreApplication::applicationName());
	//	parser.addHelpOption();
	//	parser.addVersionOption();
	//	parser.addPositionalArgument("file", "The file to open.");
	//	parser.process(a);

	const QRect availableGeometry = QApplication::desktop()->availableGeometry(richEditor);
	richEditor->resize(availableGeometry.width() / 2, (availableGeometry.height() * 2) / 3);
	richEditor->move((availableGeometry.width() - richEditor->width()) / 2,
						  (availableGeometry.height() - richEditor->height()) / 2);

	//	if (!richEditor->load(parser.positionalArguments().value(0, QLatin1String(":/example.html"))))
	richEditor->fileNew();

	richEditor->show();
}
void MainWindow::onActFilterWindowSource(bool) {
	QAction *act = qobject_cast<QAction *>(sender());
	filterForm->setSourceTableFlg(act->data().value<FilterForm::SourceTable>());
}
void MainWindow::onActFilterForm(bool b) {
	filterForm->setVisible(b);

	return;
	filterForm->show();

//	if (filterForm->sourceTableFlg() == FilterForm::useSelected) {
//		if (.currentSelected(true) == NULL)
//			MDStateBar::instance()->showMessage2sec(tr("Keine Tabelle zum Filtern ausgewählt!"));
//		else
//			browser->mTabs.currentSelected(true)->modelCast();

//		filterForm->raise();
//		filterForm->activateWindow();
//	}
}
/* ======================================================================== */
/*                              Event handler                               */
/* ======================================================================== */
bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
	QMouseEvent *mouseEv = static_cast<QMouseEvent *>(event);
	if (mouseEv) {
		INFO << tr("mouseEv << mouseEv->type()") << mouseEv << mouseEv->type();
		if (obj->objectName().contains(tr("manuBar")))
			if (mouseEv->type() == QEvent::MouseButtonPress)
				INFO << tr("Sender:") << obj;
	}
#ifdef ENABLE_MAINWINDOW_EVEN_PROCESSING
	if ((event->type() == QEvent::MouseButtonDblClick) ||
		 (event->type() == QEvent::MouseButtonPress) ||
		 (event->type() == QEvent::MouseButtonRelease) ||
		 (event->type() == QEvent::MouseMove) ||
		 (event->type() == QEvent::MouseTrackingChange)) {
		INFO << event->type() << "    FROM MOUSE!";
		return QObject::eventFilter(obj, event);
	}

	if (event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		INFO << tr("Ate key press %1").arg(keyEvent->key());
		return QObject::eventFilter(obj, event);
	}
#endif
	return QObject::eventFilter(obj, event);
}
void MainWindow::showEvent(QShowEvent *e) {
	QWidget::showEvent( e );
}
void MainWindow::hideEvent(QHideEvent *e) {
	QWidget::hideEvent( e );
}
void MainWindow::closeEvent(QCloseEvent *e) {
	QSETTINGS;

	/**** Safe current docking area of dock widgets
	\*/
	config.setValue(inpFrm->objectName() + tr("/") + KEY_DOCKWIDGETAREA,
						 (uint)dockWidgetArea(inpFrm));

	/**** Write splitter sizes to config
	\*/
	SPLT_STORE(this);

	/**** Write mainWindow geometry and window state
	\*/
	WIN_STORE(this);
	WIN_STATE_STORE(this);
	QWidget::closeEvent(e);
}
void MainWindow::keyPressEvent(QKeyEvent *e) {
	if (e->key() == Qt::Key_Escape) {
		emit browser->clearSelections();
		e->accept();
		return;
	}
	QWidget::keyPressEvent(e);
}
/* ======================================================================== */
/*									    Init methodes										    */
/* ======================================================================== */
bool MainWindow::restoreActionObjects() {
	QSETTINGS;

	actGrTbMain->blockSignals(true);
	actGrTbMenu->blockSignals(true);
	actGrFilterWidg->blockSignals(true);

	foreach (QAction *a, QList<QAction *>()
				<< actGrTbMain->actions()
				<< actGrTbMenu->actions()
				<< actGrFilterWidg->actions()) {
		if (! config.allKeys().join(',').contains(a->objectName()))
			continue;
		if (config.value(objectName() + tr("/") + a->objectName(), false).toBool()) {
			if (! a->isChecked())
				a->trigger();
		}
	}

	actGrTbMain->blockSignals(false);
	actGrTbMenu->blockSignals(false);
	actGrFilterWidg->blockSignals(false);

	INFO << filterForm->sourceTableFlg();
	return true;

	//	QList<QAction *> acts = findChildren<QAction *>(QRegularExpression("act*"));

	//	foreach (QAction *act, acts)
	//		if (act->objectName().isEmpty())
	//			acts.removeOne(act);
	//	qSort(acts.begin(), acts.end());

	//	foreach (QAction *act, acts) {
	//		if (! config.allKeys().join(',').contains( act->objectName() )) {
	//			WARN << tr("act->objectName() %1 not found!").arg(act->objectName());
	//			continue;
	//		}

	//		bool test = config.value(objectName() + tr("/") + act->objectName()).toBool();
	//		act->setChecked( test );
	//		INFO << act->objectName() << tr("set checked: ") << test;
	//	}
	//	/**** Restore visibility flag for the SQL command interface
	//	\*/
	//	try {
	//		if (!config.value(objectName() + tr("/actShowSqlQuery"), true).toBool()) {
	//			inpFrm->setQueryBoxVisible( true );
	//			actShowSqlQuery->setChecked( false );
	//		}
	//		else {
	//			inpFrm->setQueryBoxVisible( false );
	//			actShowSqlQuery->setChecked( true );
	//		}
	//	}
	//	catch (...) {
	//		CRIT << tr("!");
	//	}

	//	/**** Recall dock states
	//	 \*/
	//	if (actInpForm->isChecked()) {
	//		inpFrm->show();
	//		Qt::DockWidgetArea dwa = static_cast<Qt::DockWidgetArea>(
	//											 config.value(inpFrm->objectName() + tr("/DockWidgetArea"),
	//															  Qt::BottomDockWidgetArea).toUInt());
	//		/*!
	//		 * Produces unexpected behavior
	//		 * this->addDockWidget(dwa, inpFrm, Qt::Vertical);
	//		 */
	//		INFO << dwa;
	//	}
	//	else inpFrm->hide();
	//	//		this->addDockWidget(dwa, inpFrm, Qt::Vertical);

	//	return true;
}
void MainWindow::createActions() {
	/* ======================================================================== */
	/*                          Create QAction objects                          */
	/* ======================================================================== */
	/*!
	 * Create main toolbar actions.
	 */
	actNew = new QAction(QIcon(":/images/new.png"), tr("Neu"), this);
	actOpen = new QAction(QIcon(":/images/open.png"), tr("Öffnen"), this);
	actSave = new QAction(QIcon(":/images/save.png"), tr("Speichern"), this);
	actExport = new QAction(QIcon(":/images/export-3.png"), tr("Exportieren"), this);
	actBrowseSQL = new QAction(QIcon(":/images/database.png"), tr("Zeige SQL Tabellen"), this);
	actInpForm = new QAction(QIcon(":/images/databaseSubmit.png"), tr("Eingabeform"), this);
	actShowTbl = new QAction(QIcon(":/images/databaseDelegate.png"), tr("ShowTbl"), this);
	actDbModMaster = new QAction(QIcon(":/images/databaseConf.png"), tr("Stammdaten bearbeiten"), this);
	actUnderConstr = new QAction(QIcon(":/images/underConstruct.svg"), tr("Under construction"), this);
	actNotes = new QAction(QIcon(":/images/notes.png"), tr("Notizen"), this);
	actRichEdit = new QAction(QIcon(":/images/editor.png"), tr("Rich Text Editor"), this);
	actClose = new QAction(QIcon(":/icoDelete"), tr("Schließen"), this);
	actFilterForm = new QAction(QIcon(":/images/search.png"), tr("Suchfenster anzeigen"), this);

	/*!
	 * Create menu toolbar actions.
	 */
	actGbStyleShtA = new QAction("actGbStyleShtA", this);
	actGbSShtInpFrm = new QAction("actGbSShtInpFrm", this);
	actFilterTable = new QAction("actFilterTable", this);
	//	actFilterTableWindow = new QAction("actFilterTableWindow", this);

	actSelFont = new QAction("Schriftfont", this);
	actCyclicObjInfo = new QAction("Cyclic object info", this);
	actResizerDlg = new QAction("Fenstergröße ändern", this);
	actShowSqlQuery = new QAction("Eingabefeld SQL querys", this);
	actSetAlterRowCol = new QAction("Zeilenhintergrund", this);
	actAutoFitTables = new QAction("Autofit Zeile/Spalte", this);
	actCfgInpFrmTabOrd = new QAction("Tabulator Reihenfolge", this);
	actDoFiltSelectedTbl = new QAction("Auf ausgewählte Tabelle anwenden", this);
	actDoFiltWorktimeTbl = new QAction("Auf Arbeitszeiten-Tabelle anwenden", this);
	actResetConfig = new QAction("Spalten Einstellungen zurücksetzen", this);

	/* ======================================================================== */
	/*                             Action grouping                              */
	/* ======================================================================== */
	/*!
	 * Create action group for main toolbar actions.
	 */
	actGrTbMain = new QActionGroup(this);
	PONAM(actGrTbMain)->setExclusive(false);

	QList<QAction *> acts;
	acts << PONAM(actNew) << PONAM(actOpen) << PONAM(actSave) << PONAM(actExport)
		  << PONAM(actBrowseSQL) << PONAM(actInpForm) << PONAM(actShowTbl)
		  << PONAM(actDbModMaster) <<  PONAM(actUnderConstr) << PONAM(actNotes)
		  << PONAM(actNotes) << PONAM(actRichEdit) << PONAM(actClose);

	foreach (QAction *act, acts)
		actGrTbMain->addAction(act);

	/*!
	 * Connect action group triggered signal to a save-action-state slot.
	 */
	connect(actGrTbMain, &QActionGroup::triggered, this, &MainWindow::onActionGroupTrigd);

	/* ======================================================================== */

	/*!
	 * Create action group for toolbar menu actions.
	 */
	actGrTbMenu = new QActionGroup(this);
	PONAM(actGrTbMenu)->setExclusive(false);

	acts.clear();
	acts << PONAM(actGbStyleShtA) << PONAM(actGbSShtInpFrm) << PONAM(actSelFont)
		  << PONAM(actCyclicObjInfo) << PONAM(actResizerDlg) << PONAM(actShowSqlQuery)
		  << PONAM(actSetAlterRowCol) << PONAM(actAutoFitTables) << PONAM(actFilterTable)
			  /*<< PONAM(actFilterTableWindow)*/ << PONAM(actFilterForm) << PONAM(actCfgInpFrmTabOrd)
		  << PONAM(actResetConfig);

	foreach (QAction *act, acts)
		actGrTbMenu->addAction(act);

	/*!
	 * Connect action group triggered signal to a save-action-state slot.
	 */
	connect(actGrTbMenu, &QActionGroup::triggered, this, &MainWindow::onActionGroupTrigd);

	/* ======================================================================== */

	/*!
	 * Create action group for toolbar filter window submenu.
	 */
	actGrFilterWidg = new QActionGroup(this);
	PONAM(actGrFilterWidg)->setExclusive(true);

	acts.clear();
	acts << PONAM(actDoFiltSelectedTbl) << PONAM(actDoFiltWorktimeTbl);

	actDoFiltSelectedTbl->setData(QVariant::fromValue(FilterForm::useSelected));
	actDoFiltWorktimeTbl->setData(QVariant::fromValue(FilterForm::useWorktime));

	foreach (QAction *act, acts)
		actGrFilterWidg->addAction(act);

	/*!
	 * Connect action group triggered signal to a save-action-state slot.
	 */
	connect(actGrFilterWidg, &QActionGroup::triggered, this, &MainWindow::onActionGroupTrigd);

	/* ======================================================================== */
	/*                       Action object configurations                       */
	/* ======================================================================== */
	/*!
	 * Config some action objects to be checkable.
	 */
	actBrowseSQL->setCheckable(true);
	actNotes->setCheckable(true);
	actRichEdit->setCheckable(true);
	actInpForm->setCheckable(true);
	actShowTbl->setCheckable(true);
	actDbModMaster->setCheckable(true);
	actGbStyleShtA->setCheckable(true);
	actGbSShtInpFrm->setCheckable(true);
	actShowSqlQuery->setCheckable(true);
	actFilterForm->setCheckable(true);
	actDoFiltSelectedTbl->setCheckable(true);
	actDoFiltWorktimeTbl->setCheckable(true);
	actCyclicObjInfo->setCheckable(true);

	actCyclicObjInfo->setShortcut	(QKeySequence("F2"));
	actFilterForm->setShortcut		(QKeySequence("F3"));
	actInpForm->setShortcut			(QKeySequence("F4"));
	actNotes->setShortcut			(QKeySequence("F5"));

	actClose->setShortcut(			QKeySequence("Ctrl+Q"));
	actAutoFitTables->setShortcut(QKeySequence("Ctrl+Alt+A"));

	actRichEdit->setShortcuts(		QList<QKeySequence>()
											<< QKeySequence("Ctrl+E")
											<< QKeySequence("E"));

	actShowSqlQuery->setShortcuts(QList<QKeySequence>()
											<< QKeySequence("Ctrl+E")
											<< QKeySequence("H"));

	actResetConfig->setShortcuts(	QList<QKeySequence>()
											<< QKeySequence("Ctrl+E")
											<< QKeySequence("R"));
#ifdef MULTIKEY_SEQ
	actNotes->setShortcuts(			QList<QKeySequence>()
											<< QKeySequence("Ctrl+E")
											<< QKeySequence("N"));
#endif

	/*!
	 * Set action object tooltips.
	 */
	actSelFont->setToolTip(tr("Schriftart der aktuell ausgewählten Tabelle ändern."));
	actNew->setToolTip(tr("Create a new file"));
	actOpen->setToolTip(tr("Open an existing file"));
	actSave->setToolTip(tr("Save the document"));
	actExport->setToolTip(tr("Export dataset to document"));
	actBrowseSQL->setToolTip(tr("Öffnet alle Tabellen der SQL Datenbank mit "
										 "<b>Lesezugriff</b>."));
	actShowTbl->setToolTip(tr("Open work time database table"));
	actDbModMaster->setToolTip(tr("<b>Bearbeiten</b> der Datenbank-<b>Stammdaten</b>.\n"
											"Zum Anlegen/ändern von Projekten, Mitarbeitern oder "
											"Kunden."));
	actClose->setToolTip(tr("Alle Fenster schließen und Programm beenden."));
	actNotes->setToolTip(tr("Öffnet den <b>Notiz</b> Block. Programmfehler, "
									"änderungswünsche\noder sonstige Kommentare an den "
									"Programmierer, \nsollten hier notiert werden!"));
	actResizerDlg->setToolTip(tr("Resize MainWindow to x"));
	actShowSqlQuery->setToolTip(tr("Hide or show the input field to submit manuall "
											 "SQL querys thru database driver backend"));
	actSetAlterRowCol->setToolTip(tr("Farbe für alternierenden Zeilenhintergund auswählen."));
	actInpForm->setToolTip(tr("Öffnet die <b>Eingabeform</b> um neue Einträge in die "
									  "Arbeitszeitentabelle einzutragen."));
	actDoFiltSelectedTbl->setToolTip(tr("Das Filter/Suchfenster wird auf die <b>aktuell "
												 "ausgewählte</b> Tabelle angewendet."));
	actDoFiltWorktimeTbl->setToolTip(tr("Das Filter/Suchfenster wird <b>immer</b> auf die "
												 "<b>Arbeitszeit</b>-Tabelle angewendet."));
	actRichEdit->setToolTip(tr("Öffnet einen Rich Text Editor."));


	foreach (QAction *act, QList<QAction *>()
				<< actGrTbMain->actions()
				<< actGrTbMenu->actions()
				<< actGrFilterWidg->actions())
		act->setToolTip(tr("<p style='white-space:pre'>") + act->toolTip());
}
void MainWindow::connectActions(ConnectReceiver receivers) {
	if ((receivers == connectThis) ||
		 (receivers == connectAll)) {
		connect(actBrowseSQL, &QAction::toggled, this, &MainWindow::onBrowseSqlToggd);
		connect(actFilterForm, &QAction::toggled, this, &MainWindow::onActFilterForm);
		connect(actNotes, &QAction::toggled, this, &MainWindow::onActNotesToggd);
		connect(actRichEdit, &QAction::toggled, this, &MainWindow::onActRichTextToggd);
		connect(actInpForm, &QAction::triggered, this, &MainWindow::onOpenCloseInpFrm);
		connect(actShowTbl, &QAction::triggered, this, &MainWindow::onTblOpen);
		connect(actClose, &QAction::triggered, this, &MainWindow::onActCloseTrig);
		connect(actGbStyleShtA, &QAction::triggered, this, &MainWindow::onMenuStyleShtATrig);
		connect(actGbSShtInpFrm, &QAction::triggered, this, &MainWindow::onMenuStyleShtInpFrmTrig);
		connect(actExport, &QAction::triggered, this, &MainWindow::onActExportTrig);
		connect(actUnderConstr, &QAction::triggered, this, &MainWindow::onUnderConstrTrig);
		connect(actResizerDlg, &QAction::triggered, this, &MainWindow::onResizerDlgTrig);
		//		connect(actShowSqlQuery, &QAction::toggled, this, &MainWindow::onShowSqlQueryTogd);
		connect(actCfgInpFrmTabOrd,  &QAction::triggered, this, &MainWindow::onActCfgInpFrmTabOrdTrig);
		connect(actSave,  &QAction::triggered, this, &MainWindow::onActSaveTrig);
		connect(actOpen,  &QAction::triggered, this, &MainWindow::onActOpenTrig);
		connect(actDoFiltSelectedTbl, &QAction::triggered, this, &MainWindow::onActFilterWindowSource);
		connect(actDoFiltWorktimeTbl, &QAction::triggered, this, &MainWindow::onActFilterWindowSource);

		filterForm->addAction(actFilterForm);
	}

	if ((receivers == connectOthers) ||
		 (receivers == connectAll)) {
		connect(actAutoFitTables,	&QAction::triggered, browser, &Browser::autofitRowCol);
		connect(browser, &Browser::stateMsg, stateBar, &MDStateBar::showMessage);
		connect(inpFrm, &InpFrm::visibilityChanged, actInpForm, &QAction::setChecked);
		connect(browser, &Browser::visibilityChanged, actBrowseSQL, &QAction::setChecked);
		connect(actShowSqlQuery, &QAction::toggled, inpFrm, &InpFrm::setQueryBoxVisible);
		connect(actSelFont, &QAction::triggered, browser, &Browser::selectAndSetFont);
		connect(actSetAlterRowCol, &QAction::triggered, browser, &Browser::selectAndSetRowColor);
		connect(actResetConfig, &QAction::triggered, browser, &Browser::resetColumnConfig);
	}
}
void MainWindow::makeMenu() {
	/* ======================================================================== */
	/*                               Main Toolbar                               */
	/* ======================================================================== */
	ui->mainToolBar->addActions( actGrTbMain->actions() );
	ui->mainToolBar->addAction( actFilterForm );

	/* ======================================================================== */
	/*                                 fileMenu                                 */
	/* ======================================================================== */
	MdMenu *fileMenu = new MdMenu(this);
	fileMenu->setTitle(tr("&Datei"));
	fileMenu->addAction(tr("Add &Connection..."), mDbc, SLOT(addConnection()));
	fileMenu->actions().last()->
			setToolTip(tr("Neue SQLITE Datenbank Verbindung aufbauen."));
	fileMenu->addSeparator();
	fileMenu->addActions(actGrTbMain->actions());

	/* ======================================================================== */
	/*                                 helpMenu                                 */
	/* ======================================================================== */
	MdMenu *helpMenu = new MdMenu(this);
	helpMenu->setTitle(tr("&Hilfe"));
	helpMenu->addAction(tr("About"), this, SLOT(about()));
	helpMenu->addAction(tr("About Qt"), qApp, SLOT(aboutQt()));

	/* ======================================================================== */
	/*                                setupMenu                                 */
	/* ======================================================================== */
	MdMenu *setupMenu = new MdMenu(this);
	setupMenu->setTitle(tr("Se&tup"));

	QAction *sep3 = new QAction(this);
	sep3->setSeparator(true);

	setupMenu->addActions(QList<QAction *>() << actSelFont << actSetAlterRowCol
								 << sep3 << actCyclicObjInfo << actResizerDlg << actShowSqlQuery
								 << actCfgInpFrmTabOrd << actResetConfig);
	setupMenu->addSeparator();

	MdMenu *muFilterForm = new MdMenu(this);
	muFilterForm->setTitle(tr("Filter Fenster"));
	muFilterForm->addActions(QList<QAction *>() << actDoFiltSelectedTbl << actDoFiltWorktimeTbl);

	QAction *muSetStyleSh =
			setupMenu->addAction("St&yleSheets", this, &MainWindow::onStyleSheetTrig);
	Q_UNUSED(muSetStyleSh);

	setupMenu->addSeparator();

	/* ======================================================================== */
	/*                                 viewMenu                                 */
	/* ======================================================================== */
	MdMenu *viewMenu = new MdMenu(this);
	viewMenu->setTitle(tr("&Anzeige"));
	viewMenu->addActions(QList<QAction *>() << actAutoFitTables << actFilterForm);

	/* ======================================================================== */
	/*                        Inherited menu components                         */
	/* ======================================================================== */
	MdMenu *browserMenu = Browser::instance()->menuBarElement();

	/* ======================================================================== */
	/*                    Add MdMenus to mainwindows toolBar                    */
	/* ======================================================================== */
	foreach (MdMenu *mdm, QList<MdMenu *>()
				<< findChildren<MdMenu *>()
				<< browserMenu) {
		menuBar()->addMenu(mdm);
		mdm->show();
	}

}
void MainWindow::onStyleSheetTrig() {

}
void MainWindow::setVisibleFake(QObject *) {
	actFilterForm->trigger();
}
void MainWindow::initDocks() {

}
