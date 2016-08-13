#include "mainwindow.h"
#include "ui_mainwindow.h"

#define  RESTORE_SQL_DATABASE_PATH

#ifdef STATIC_SIGNAL_CLASS
StaticSignal *StaticSignal::inst = 0;
#endif

int MainWindow::fuse = 5;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
	ui(new Ui::MainWindow) {
	ui->setupUi(this);

	QSETTINGS_INIT; QSETTINGS;
	//	initDocks();

	createActions();
	connectActions(connectThis);

	stateBar		= new MDStateBar( this );
	browser		= new Browser( parent );
	mDbc			= new DbController(this);

	/* ======================================================================== */
	/*                                new InpFrm                                */
	/* ======================================================================== */
	inpFrm		= new InpFrm( this );

	QVariant var;
	inpFrm->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	var.setValue (config.value(inpFrm->objectName() + tr("/DockWidgetArea"), Qt::BottomDockWidgetArea) );
	INFO << var << var.value<Qt::DockWidgetArea>() << var.value<Qt::DockWidgetAreas>();

	Qt::DockWidgetArea dockArea = static_cast<Qt::DockWidgetArea>(var.toInt());
	addDockWidget(dockArea, inpFrm);

	sortwindow  = new SortWindow( parent );

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

	/**** Restore splitter sizes to config
	\*/
	SPLT_RESTORE(this);

	//	installEventFilter(this);

	setWindowTitle( MAINWINDOW_TITLE );
	config.fileName();
	connectActions(connectOthers);


	QTimer::singleShot(50, this, SLOT(restoreActionObjects()));
	QTimer *t2 = new QTimer(this);
	t2->setInterval(100);
	t2->connect(t2, SIGNAL(timeout()), this, SLOT(onCyclic()));
	//	t2->start();
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
void MainWindow::onActHideSqlQueryTrig() {
	bool dohide = actHideSqlQuery->isChecked();
	QSETTINGS;
	config.setValue(objectName() + "/HideSqlQuery", dohide);
	inpFrm->setQueryBoxVisible( !dohide );
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

	INFO << actHideSqlQuery->isCheckable() << actHideSqlQuery->isChecked();
	actHideSqlQuery->setCheckable(true);
	actHideSqlQuery->setChecked(true);

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
	QList<QAction *> acts = findChildren<QAction *>(QRegularExpression("act*"));
	QList<int> ba;
	foreach (QAction *act, acts)
		ba << (int)act->isChecked();

	INFO << ba;
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
/* ======================================================================== */
/*                              Event handler                               */
/* ======================================================================== */
bool MainWindow::eventFilter(QObject *obj, QEvent *event) {

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
	setCentralWidget(browser);
	QWidget::showEvent( e );
}
void MainWindow::hideEvent(QHideEvent *e) {
	QWidget::hideEvent( e );
}
void MainWindow::closeEvent(QCloseEvent *e) {
	QSETTINGS;

	/**** Safe current docking area of dock widgets
	\*/
	config.setValue(inpFrm->objectName() + tr("/DockWidgetArea"),
						 (uint)dockWidgetArea(inpFrm));

	/**** Write splitter sizes to config
	\*/
	SPLT_STORE(this);

	/**** Write mainWindow geometry and window state
	\*/
	WIN_STORE(this);

	/**** Safe all action states from MainWindow
	\*/
	ACTION_STORE(this, tr("act*"));
}
/* ======================================================================== */
/*									    Init methodes										    */
/* ======================================================================== */
bool MainWindow::restoreActionObjects () {
	QSETTINGS;

	QList<QAction *> acts = findChildren<QAction *>(QRegularExpression("act*"));

	foreach (QAction *act, acts)
		if (act->objectName().isEmpty())
			acts.removeOne(act);
	qSort(acts.begin(), acts.end());

	foreach (QAction *act, acts) {
		if (! config.allKeys().join(',').contains( act->objectName() )) {
			WARN << tr("act->objectName() %1 not found!").arg(act->objectName());
			continue;
		}

		bool test = config.value(objectName() + tr("/") + act->objectName()).toBool();
		act->setChecked( test );
		INFO << act->objectName() << tr("set checked: ") << test;
	}
	/**** Restore visibility flag for the SQL command interface
	\*/
	try {
		if (!config.value(objectName() + tr("/actHideSqlQuery"), true).toBool()) {
			inpFrm->setQueryBoxVisible( true );
			actHideSqlQuery->setChecked( false );
		}
		else {
			inpFrm->setQueryBoxVisible( false );
			actHideSqlQuery->setChecked( true );
		}
	}
	catch (...) {
		CRIT << tr("!");
	}

	/**** Recall dock states
	 \*/
	if (actInpForm->isChecked()) {
		inpFrm->show();
		Qt::DockWidgetArea dwa = static_cast<Qt::DockWidgetArea>(
											 config.value(inpFrm->objectName() + tr("/DockWidgetArea"),
															  Qt::BottomDockWidgetArea).toUInt());
		/*!
		 * Produces unexpected behavior
		 * this->addDockWidget(dwa, inpFrm, Qt::Vertical);
		 */
		INFO << dwa;
	}
	else inpFrm->hide();
	//		this->addDockWidget(dwa, inpFrm, Qt::Vertical);

	return true;
}
void MainWindow::createActions() {
	actNew = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
	actOpen = new QAction(QIcon(":/images/open.png"), tr("Open"), this);
	actSave = new QAction(QIcon(":/images/save.png"), tr("Save"), this);
	actExport = new QAction(QIcon(":/images/export-3.png"), tr("Export"), this);
	actBrowseSQL = new QAction(QIcon(":/images/database.png"), tr("BrowseSQL"), this);
	actInpForm = new QAction(QIcon(":/images/databaseSubmit.png"), tr("InpForm"), this);
	actShowTbl = new QAction(QIcon(":/images/databaseDelegate.png"), tr("ShowTbl"), this);
	actDbModMaster = new QAction(QIcon(":/images/databaseConf.png"), tr("DbModMaster"), this);
	actClose = new QAction(QIcon(":/icoDelete"), tr("Close"), this);

	actGbStyleShtA = new QAction("actGbStyleShtA", this);
	actGbSShtInpFrm = new QAction("actGbSShtInpFrm", this);
	actUnderConstr = new QAction("actUnderConstr", this);
	actSelFont = new QAction("actSelFont", this);
	actCyclicObjInfo = new QAction("actCyclicObjInfo", this);
	actResizerDlg = new QAction("actResizerDlg", this);
	actHideSqlQuery = new QAction("actHideSqlQuery", this);
	actSetAlterRowCol = new QAction("actSetAlterRowCol", this);
	actAutoFitTables = new QAction("actAutoFitTables", this);
	actFilterTable = new QAction("actFilterTable", this);
	actFilterTableWindow = new QAction("actFilterTableWindow", this);
	actFilterForm = new QAction("actFilterForm", this);
	actCfgInpFrmTabOrd = new QAction("actCfgInpFrmTabOrd", this);

	actBrowseSQL->setCheckable(true);
	actInpForm->setCheckable(true);
	actShowTbl->setCheckable(true);
	actDbModMaster->setCheckable(true);
	actGbStyleShtA->setCheckable(true);
	actGbSShtInpFrm->setCheckable(true);
	actCyclicObjInfo->setCheckable(true);
	actHideSqlQuery->setCheckable(true);
	actFilterTableWindow->setCheckable(true);
	actFilterForm->setCheckable(true);

	actSelFont->setToolTip(tr("Font"));
	actNew->setToolTip(tr("Create a new file"));
	actOpen->setToolTip(tr("Open an existing file"));
	actSave->setToolTip(tr("Save the document"));
	actExport->setToolTip(tr("Export dataset to document"));
	actBrowseSQL->setToolTip(tr("Browser for SQL database"));
	actShowTbl->setToolTip(tr("Open work time database table"));
	actDbModMaster->setToolTip(tr("Stammdaten erweitern"));
	actClose->setToolTip(tr("Alle Fenster schließen "));
	actResizerDlg->setToolTip(tr("Resize MainWindow to x"));
	actHideSqlQuery->setToolTip(tr("Hide or show the input field to submit manuall "
											 "SQL querys thru database driver backend"));
	actSetAlterRowCol->setToolTip(tr("Farbe für alternierenden Zeilenhintergund"));
	actInpForm->setToolTip(tr("<html><head/><body><p>Öffnet die <span style="" "
									  "font-weight:600;"">Eingabeform</span> um neue Einträge in "
									  "die in die Arbeitszeitentabelle einzutragen</p></body></html>"));
}
void MainWindow::makeMenuBar() {
	QMenu *fileMenu = menuBar()->addMenu(QObject::tr("&File"));
	fileMenu->addAction(tr("Add &Connection..."), mDbc, SLOT(addConnection()));
	fileMenu->addSeparator();
	fileMenu->addAction(tr("&Quit"), qApp, SLOT(quit()));

	QMenu *helpMenu = menuBar()->addMenu(QObject::tr("&Help"));
	helpMenu->addAction(tr("About"), this, SLOT(about()));
	helpMenu->addAction(tr("About Qt"), qApp, SLOT(aboutQt()));

	/* ======================================================================== */

	INFO << menuBar()->children();

	QMenu *muSup = static_cast<QMenu *>(menuBar()->findChild<QMenu *>(tr("menu_Setup")));
	QMenu *muView = static_cast<QMenu *>(menuBar()->findChild<QMenu *>(tr("muView)")));

	QAction *muSetStyleSh, *muSetFont, *muSetRowCol, *muSetObjInfo, *muSetResize,
			*muSetHidQuery, *muSetTabOrd, *muSep_1, *muSetAutofit, *muSetfilt, *muSetClose;

	muSetStyleSh	= muSup->addAction("St&yleSheets",			this, &MainWindow::onStyleSheetTrig);
	muSetFont		= muSup->addAction("Schriftfont",			this, &MainWindow::onSetFont);
	muSetRowCol		= muSup->addAction("Zeilenhintergrund",	this, &MainWindow::onSetAlterRowColTrig);
	muSetObjInfo	= muSup->addAction("Cyclic object info",	browser, &Browser::setCyclicObjInfo);
	muSetResize		= muSup->addAction("Fenstergröße ändern", this, &MainWindow::onResizerDlgTrig);
	muSetHidQuery	= muSup->addAction("Eingabefeld SQL querys", this, &MainWindow::onActHideSqlQueryTrig);
	muSetTabOrd		= muSup->addAction("Tabulator Reihenfolge", this, &MainWindow::onActCfgInpFrmTabOrdTrig);
	muSep_1			= muSup->addSeparator();
	muSetAutofit	= muSup->addAction("Autofit Zeile/Spalte", browser, &Browser::autofitRowCol);
	muSetfilt		= muSup->addAction("Suchfenster anzeigen", browser, &Browser::onActFilterForm);
	muSetClose		= muSup->addAction("Close", this, &MainWindow::close);

	muSetClose->setShortcut(QKeySequence("Ctrl+Q"));
	muSetAutofit->setShortcut(QKeySequence("Ctrl+Alt+A"));
	muSetHidQuery->setShortcuts(QList<QKeySequence>()
										 << QKeySequence("Ctrl+Shifr+Q")
										 << QKeySequence("Ctrl+H"));

	/* ======================================================================== */
	ui->mainToolBar->addActions(QList<QAction*>()
										 << actNew << actOpen << actSave << actExport << actBrowseSQL
										 << actInpForm << actShowTbl << actDbModMaster << actClose);

	/* ======================================================================== */
	/*                        Inherited menu components                         */
	/* ======================================================================== */
	QMenu *browserMenu = Browser::instance()->menuBarElement();
	menuBar()->addMenu( browserMenu );

	//	browserMenu->addActions(Browser::instance()->menuBarElement());
}
void MainWindow::onStyleSheetTrig() {

}
void MainWindow::connectActions(ConnectReceiver receivers) {
	actHideSqlQuery->setChecked( true );

	if ((receivers == connectThis) ||
		 (receivers == connectAll)) {
		connect(actBrowseSQL, &QAction::triggered, this, &MainWindow::onBrowseSqlTrig);
		connect(actInpForm, &QAction::triggered, this, &MainWindow::onOpenCloseInpFrm);
		connect(actShowTbl, &QAction::triggered, this, &MainWindow::onTblOpen);
		connect(actClose, &QAction::triggered, this, &MainWindow::onActCloseTrig);
		connect(actGbStyleShtA, &QAction::triggered, this, &MainWindow::onMenuStyleShtATrig);
		connect(actGbSShtInpFrm, &QAction::triggered, this, &MainWindow::onMenuStyleShtInpFrmTrig);
		connect(actExport, &QAction::triggered, this, &MainWindow::onActExportTrig);
		connect(actUnderConstr, &QAction::triggered, this, &MainWindow::onUnderConstrTrig);
		connect(actSelFont, &QAction::triggered, this, &MainWindow::onSetFont);
		connect(actResizerDlg, &QAction::triggered, this, &MainWindow::onResizerDlgTrig);
		connect(actHideSqlQuery, &QAction::triggered, this, &MainWindow::onActHideSqlQueryTrig);
		connect(actSetAlterRowCol, &QAction::triggered, this, &MainWindow::onSetAlterRowColTrig);
		connect(actCfgInpFrmTabOrd,  &QAction::triggered, this, &MainWindow::onActCfgInpFrmTabOrdTrig);
		connect(actSave,  &QAction::triggered, this, &MainWindow::onActSaveTrig);
		connect(actOpen,  &QAction::triggered, this, &MainWindow::onActOpenTrig);
	}

	if ((receivers == connectOthers) ||
		 (receivers == connectAll)) {
		connect(actAutoFitTables,	&QAction::triggered, browser, &Browser::autofitRowCol);
		connect(actFilterForm, &QAction::triggered, browser, &Browser::onActFilterForm);
		connect(browser, &Browser::stateMsg, stateBar, &MDStateBar::showMessage);
		connect(inpFrm, &InpFrm::visibilityChanged, actInpForm, &QAction::setChecked);
		connect(browser, &Browser::visibilityChanged, actBrowseSQL, &QAction::setChecked);
	}
	//    connect(	actFilterTableWindow,	SIGNAL(triggered(bool)),
	//                browser,					SLOT(onActFilterWindowTable(bool)));
	//    connect(	this,						SIGNAL(mainwindowCloses()),
	//                browser->getSortwindow0(),	SLOT(close()));
	//    connect(	this,						SIGNAL(mainwindowCloses()),
	//                browser->getSortwindow1(),	SLOT(close()));
	//    connect(	browser->getSortwindow0(),	SIGNAL(closesUncheck(bool)),
	//                actFilterTableWindow,	SLOT(setChecked(bool)));
	//    connect(	browser->getSortwindow1(),	SIGNAL(closesUncheck(bool)),
	//                actFilterTableWindow,	SLOT(setChecked(bool)));
	//    connect(actFilterTable, SIGNAL(triggered()),
	//            browser,        SLOT( SORTIT()));
}
void MainWindow::initDocks() {

}
void MainWindow::ACTION_STORE(QObject *obj, QString regex) {
	QSETTINGS;
	QList<QAction *> acts = findChildren<QAction *>(QRegularExpression("act*"));
	foreach (QAction *act, acts) {
		config.setValue(obj->objectName() + QString("/") + act->objectName(), act->isChecked());
		INFO << obj->objectName() + QString("/") + act->objectName() << act->isChecked();
	}

	config.setValue(obj->objectName() + QString("/actionCtr"), acts.length());
	config.sync();
}
void MainWindow::ACTION_RESTORE(QObject *obj, QString regex) {
	QSETTINGS;
	QList<QAction *> acts = findChildren<QAction *>(QRegularExpression("act*"));

	foreach (QAction *act, acts)
		act->setChecked(config.value(obj->objectName() + QString("/") + act->objectName()).toBool());
}




