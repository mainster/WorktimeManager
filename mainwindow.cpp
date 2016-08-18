#include "mainwindow.h"
#include "ui_mainwindow.h"

#define	ON_CYCLIC_INTERVAL_MS	250

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
	ui(new Ui::MainWindow) {
	ui->setupUi(this);

	QSETTINGS_INIT; QSETTINGS;
	//	initDocks();

	createActions();
	connectActions(connectThis);

	stateBar		= new MDStateBar( this );
	browser		= new Browser(parent);
	mDbc			= new DbController(this);
	sortwindow  = new SortWindow(parent);
	connect(sortwindow,	&SortWindow::sourceTableChanged,
			  browser,		&Browser::onSourceTableChanged);

	inpFrm		= new InpFrm(this);
	notes.toDo	= new MDNotes(tr("toDo"), parent);

	connect(browser->getConnectionWidget(),	SIGNAL(detailesChanged(QString&)),
			  stateBar,									SLOT(showInfo(QString&)));
	connect(notes.toDo,								SIGNAL(settingsSaved(const QString&)),
			  stateBar,									SLOT(showMessage2sec(const QString&)));


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
	makeMenuBar();

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
void MainWindow::onSetAlterRowColTrig() {
	QPalette pal = browser->tvs()->first()->tv()->palette();

	QColor color = QColorDialog::getColor(
							pal.background().color(), this,
							"Farbe für alternierenden Zeilenhintergrund",
							QColorDialog::DontUseNativeDialog);

	if (color.isValid()) {
		foreach (TabView *tv, *browser->tvs()) {
			tv->setAlternateRowCol( color, true );
		}
	}
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
void MainWindow::onSetFont() {
	bool ok;
	QFont font = QFontDialog::getFont(
						 &ok, QFont("Helvetica [Cronyx]", 10), this);

	if (ok) {
		browser->setFontAcc(font);
	} else {
		browser->setFontAcc(font);
	}
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
void MainWindow::onBrowseSqlToggd(bool b) {
	browser->setVisible(b);
}
void MainWindow::onActNotesToggd(bool b) {
	notes.toDo->setVisible(b);
}
void MainWindow::onActFilterWindowSource(bool) {
	QAction *act = qobject_cast<QAction *>(sender());
	sortwindow->setSourceTableFlg(act->data().value<SortWindow::SourceTable>());
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
void MainWindow::closeEvent(QCloseEvent *) {
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

	INFO << sortwindow->sourceTableFlg();
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
	actClose = new QAction(QIcon(":/icoDelete"), tr("Schließen"), this);

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
	actFilterForm = new QAction("Suchfenster anzeigen", this);
	actCfgInpFrmTabOrd = new QAction("Tabulator Reihenfolge", this);
	actFiltSelectedTbl = new QAction("Auf ausgewählte Tabelle anwenden", this);
	actFiltWorktimeTbl = new QAction("Auf Arbeitszeiten-Tabelle anwenden", this);

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
		  << PONAM(actNotes) << PONAM(actClose);

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
			  /*<< PONAM(actFilterTableWindow)*/ << PONAM(actFilterForm) << PONAM(actCfgInpFrmTabOrd);

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
	acts << PONAM(actFiltSelectedTbl) << PONAM(actFiltWorktimeTbl);

	actFiltSelectedTbl->setData(QVariant::fromValue(SortWindow::useSelected));
	actFiltWorktimeTbl->setData(QVariant::fromValue(SortWindow::useWorktime));

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
	actInpForm->setCheckable(true);
	actShowTbl->setCheckable(true);
	actDbModMaster->setCheckable(true);
	actGbStyleShtA->setCheckable(true);
	actGbSShtInpFrm->setCheckable(true);
	actCyclicObjInfo->setCheckable(true);
	actShowSqlQuery->setCheckable(true);
	//	actFilterTableWindow->setCheckable(true);
	actFilterForm->setCheckable(true);
	actFiltSelectedTbl->setCheckable(true);
	actFiltWorktimeTbl->setCheckable(true);

	actClose->setShortcut(QKeySequence("Ctrl+Q"));
	actAutoFitTables->setShortcut(QKeySequence("Ctrl+Alt+A"));
	actShowSqlQuery->setShortcuts(QList<QKeySequence>()
											<< QKeySequence("Ctrl+Shifr+Q")
											<< QKeySequence("Ctrl+H"));
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
									"Programmierer \nsollten hier notiert werden!"));
	actResizerDlg->setToolTip(tr("Resize MainWindow to x"));
	actShowSqlQuery->setToolTip(tr("Hide or show the input field to submit manuall "
											 "SQL querys thru database driver backend"));
	actSetAlterRowCol->setToolTip(tr("Farbe für alternierenden Zeilenhintergund auswählen."));
	actInpForm->setToolTip(tr("Öffnet die <b>Eingabeform</b> um neue Einträge in die "
									  "Arbeitszeitentabelle einzutragen."));
	actFiltSelectedTbl->setToolTip(tr("Das Filter/Suchfenster wird auf die <b>aktuell "
												 "ausgewählte</b> Tabelle angewendet."));
	actFiltWorktimeTbl->setToolTip(tr("Das Filter/Suchfenster <b>immer</b> auf die "
												 "<b>Arbeitszeit</b>-Tabelle angewendet."));


	foreach (QAction *act, actGrTbMain->actions())
		act->setToolTip(tr("<p style='white-space:pre'>") +
							 //							 tr("<div style=\"width: 600px;\">") +
							 act->toolTip() /*+ tr("</div>")*/);
}
void MainWindow::makeMenuBar() {
	/* ======================================================================== */
	/*                               Main Toolbar                               */
	/* ======================================================================== */
	//	ui->mainToolBar->addActions(QList<QAction*>()
	//										 << actNew << actOpen << actSave << actExport
	//										 << actBrowseSQL << actInpForm << actShowTbl
	//										 << actDbModMaster << actUnderConstr << actClose);
	ui->mainToolBar->addActions( actGrTbMain->actions() );

	/* ======================================================================== */
	/*                                 fileMenu                                 */
	/* ======================================================================== */
	MdMenu *fileMenu = qobject_cast<MdMenu *>(menuBar()->addMenu(QObject::tr("&Datei")));
	fileMenu->addAction(tr("Add &Connection..."), mDbc, SLOT(addConnection()));
	fileMenu->addSeparator();
	//	fileMenu->addAction(tr("&Quit"), qApp, SLOT(quit()));
	fileMenu->addActions(QList<QAction *>()
								<< actClose);

	/* ======================================================================== */
	/*                                 helpMenu                                 */
	/* ======================================================================== */
	MdMenu *helpMenu = qobject_cast<MdMenu *>(menuBar()->addMenu(QObject::tr("&Hilfe")));
	helpMenu->addAction(tr("About"), this, SLOT(about()));
	helpMenu->addAction(tr("About Qt"), qApp, SLOT(aboutQt()));

	/* ======================================================================== */
	/*                                setupMenu                                 */
	/* ======================================================================== */
	MdMenu *setupMenu = qobject_cast<MdMenu *>(menuBar()->addMenu(QObject::tr("Se&tup")));
	setupMenu->addActions(QList<QAction *>()
								 << actSelFont << actSetAlterRowCol << actCyclicObjInfo
								 << actResizerDlg << actShowSqlQuery << actCfgInpFrmTabOrd);

	MdMenu *filterForm = qobject_cast<MdMenu *>(setupMenu->addMenu(tr("Filter Fenster")));
	filterForm->addActions(QList<QAction *>()
								  << actFiltSelectedTbl << actFiltWorktimeTbl);

	QAction *muSetStyleSh =
			setupMenu->addAction("St&yleSheets", this, &MainWindow::onStyleSheetTrig);
	Q_UNUSED(muSetStyleSh);

	setupMenu->addSeparator();

	/* ======================================================================== */
	/*                                 viewMenu                                 */
	/* ======================================================================== */
	MdMenu *viewMenu = qobject_cast<MdMenu *>(menuBar()->addMenu(QObject::tr("&Anzeige")));
	viewMenu->addActions(QList<QAction *>()
								<< actAutoFitTables << actFilterForm);

	/* ======================================================================== */
	/*                        Inherited menu components                         */
	/* ======================================================================== */
	MdMenu *browserMenu = Browser::instance()->menuBarElement();
	menuBar()->addMenu( browserMenu );

	//	browserMenu->addActions(Browser::instance()->menuBarElement());
}
void MainWindow::onStyleSheetTrig() {

}
void MainWindow::connectActions(ConnectReceiver receivers) {
	if ((receivers == connectThis) ||
		 (receivers == connectAll)) {
		connect(actBrowseSQL, &QAction::toggled, this, &MainWindow::onBrowseSqlToggd);
		connect(actNotes, &QAction::toggled, this, &MainWindow::onActNotesToggd);
		connect(actInpForm, &QAction::triggered, this, &MainWindow::onOpenCloseInpFrm);
		connect(actShowTbl, &QAction::triggered, this, &MainWindow::onTblOpen);
		connect(actClose, &QAction::triggered, this, &MainWindow::onActCloseTrig);
		connect(actGbStyleShtA, &QAction::triggered, this, &MainWindow::onMenuStyleShtATrig);
		connect(actGbSShtInpFrm, &QAction::triggered, this, &MainWindow::onMenuStyleShtInpFrmTrig);
		connect(actExport, &QAction::triggered, this, &MainWindow::onActExportTrig);
		connect(actUnderConstr, &QAction::triggered, this, &MainWindow::onUnderConstrTrig);
		connect(actSelFont, &QAction::triggered, this, &MainWindow::onSetFont);
		connect(actResizerDlg, &QAction::triggered, this, &MainWindow::onResizerDlgTrig);
		//		connect(actShowSqlQuery, &QAction::toggled, this, &MainWindow::onShowSqlQueryTogd);
		connect(actSetAlterRowCol, &QAction::triggered, this, &MainWindow::onSetAlterRowColTrig);
		connect(actCfgInpFrmTabOrd,  &QAction::triggered, this, &MainWindow::onActCfgInpFrmTabOrdTrig);
		connect(actSave,  &QAction::triggered, this, &MainWindow::onActSaveTrig);
		connect(actOpen,  &QAction::triggered, this, &MainWindow::onActOpenTrig);
		connect(actFiltSelectedTbl, &QAction::triggered, this, &MainWindow::onActFilterWindowSource);
		connect(actFiltWorktimeTbl, &QAction::triggered, this, &MainWindow::onActFilterWindowSource);
	}

	if ((receivers == connectOthers) ||
		 (receivers == connectAll)) {
		connect(actAutoFitTables,	&QAction::triggered, browser, &Browser::autofitRowCol);
		connect(actFilterForm, &QAction::triggered, browser, &Browser::onActFilterForm);
		connect(browser, &Browser::stateMsg, stateBar, &MDStateBar::showMessage);
		connect(inpFrm, &InpFrm::visibilityChanged, actInpForm, &QAction::setChecked);
		connect(browser, &Browser::visibilityChanged, actBrowseSQL, &QAction::setChecked);
		connect(actShowSqlQuery, &QAction::toggled, inpFrm, &InpFrm::setQueryBoxVisible);
	}
}
void MainWindow::initDocks() {

}
