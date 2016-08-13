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
	connectActions(connectThis);

	QSETTINGS_INIT; QSETTINGS;
//	initDocks();


	stateBar		= new MDStateBar( this );
	browser		= new Browser( parent );
	mDbc			= new DbController(this);
	inpFrm		= new InpFrm( this );
	sortwindow  = new SortWindow( parent );

	setStyleSheet(browser->browserStyleSheet);

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


	/**** Restore splitter sizes to config
	\*/
	SPLT_RESTORE(this);

	/**** Restore mainWindow geometry and window state
	\*/
	WIN_RESTORE(this);

	//	installEventFilter(this);

	setWindowTitle( MAINWINDOW_TITLE );
	config.fileName();
	connectActions(connectOthers);

	QTimer::singleShot(50, this, SLOT(restoreActionObjects()));
}
MainWindow::~MainWindow() {
	delete ui;
}
void MainWindow::onInpFrmButtonClick(bool ) {

}
void MainWindow::onBrowseSqlTrig(bool doBrowse) {
	if (doBrowse) {
		browser->show();

		if ((! mDbc->getDb().isOpen()) ||
			 (mDbc->getDb().isOpenError()))
			mDbc->getConnectionState();
	}
	else {
		//		QSETTINGS;
		//		saveMainWindowGeometry();
		//		QMetaObject::invokeMethod(browser, "close");
		browser->hide();
	}
}
void MainWindow::onOpenCloseInpFrm(bool onOff) {
	QSETTINGS;
	inpFrm->setVisible(onOff);
	//   this->addDockWidget(Qt::BottomDockWidgetArea, inpForm);

	//	if (b) {
	////		InpFrm::instance()->restoreInpFrmGeometry();
	//		//      addDockWidget( static_cast<Qt::DockWidgetArea>(
	//		//                        config.value("inpForm/DockWidgetArea", "8").toInt) );
	//		inpFrm->show();
	//	}
	//	else {

	//		//      config.setValue("inpForm/DockWidgetArea", (uint)dockWidgetArea(inpForm));
	//		inpFrm->saveInpFrmGeometry();
	//		inpFrm->hide();
	//	}
}
void MainWindow::onTblOpen(bool) {
	INFO << tr("I am an empty slot, use me!!!");
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
	bool dohide = ui->actHideSqlQuery->isChecked();
	QSETTINGS;
	config.setValue(objectName() + "/HideSqlQuery", dohide);
	inpFrm->setSqlQueryTextboxVisible( !dohide );
}
void MainWindow::onMenuStyleShtATrig(bool b) {
	if (!b) return;

	ui->actGbSShtInpFrm->setChecked( !b );
	ui->actGbStyleShtA->setChecked( b );

	foreach (TabView *tv, *browser->tvs()) {
		tv->setStyleSheet( Globals::gbStyleShtCenterPROPERTYS);
	}
}
void MainWindow::onMenuStyleShtInpFrmTrig(bool b) {
	if (!b) return;

	//   ui->actStyleShtA->setChecked( !b );
	//   ui->actStyleShtInpFrm->setChecked( b );

	//   *browser->tvs()[0]->getGrBox();
	//   foreach (QGroupBox *gb, browser->getGBoxs()) {
	//      gb->setStyleSheet( Globals::gbStyleShtInpFrm);
	//   }
}
void MainWindow::onUnderConstrTrig() {

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

}
void MainWindow::onActCloseTrig() {
	//    qApp->closeAllWindows();
	browser->close();
	inpFrm->close();
	close();
}
void MainWindow::onActSaveTrig() {
	//	QSETTINGS;
	//	config.setValue(tr("testgeometry/") + objectName(), saveGeometry());
	//	config.setValue(tr("teststate/") + objectName(), saveState());
	//	config.sync();
	WIN_STORE(this);
}
void MainWindow::onActOpenTrig() {
	WIN_RESTORE(this);
	//	QSETTINGS;
	//	restoreGeometry(config.value(tr("testgeometry/") + objectName()).toByteArray());
	//	restoreState(config.value(tr("teststate/") + objectName()).toByteArray());
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
	config.setValue("inpForm/DockWidgetArea", (uint)dockWidgetArea(inpFrm));

	/**** Write splitter sizes to config
	\*/
	SPLT_STORE(this);

	/**** Write mainWindow geometry and window state
	\*/
	WIN_STORE(this);

	/**** Safe all action states from MainWindow
	\*/
	ACTION_STORE(this);

//	QWidget::closeEvent( e );
}
/* ======================================================================== */
/*									    Init methodes										    */
/* ======================================================================== */
bool MainWindow::restoreActionObjects () {
	QSETTINGS;
	bool returnState = true;

	/**** Restore all action states from MainWindow
	\*/
	ACTION_RESTORE(this);

	//	QList<QAction *> acts = findChildren<QAction *>(QString(), Qt::FindDirectChildrenOnly);
	//	foreach (QAction *ac, acts) {
	//		if ( config.value("MainWindow/" + ac->objectName(), false).toBool() ) {
	//			if (ac != ui->actFilterForm) {
	//				emit ac->trigger();
	//				INFO << tr("QAction") << ac->text() << tr("triggered");
	//			}
	//		}
	//	}

	/**** Restore visibility flag for the SQL command interface
	\*/
	try {
		if (!config.value("MainWindow/actHideSqlQuery", true).toBool()) {
			inpFrm->setSqlQueryTextboxVisible( true );
			ui->actHideSqlQuery->setChecked( false );
		}
		else {
			inpFrm->setSqlQueryTextboxVisible( false );
			ui->actHideSqlQuery->setChecked( true );
		}
	}
	catch (...) {
		CRIT << tr("!");
	}

	/**** Restore tabview <-> SQL table assignements, but only if valid keys are stored
	 \*/
	foreach (TabView *tv, *browser->tvs()) {
		QString settingsKey = QString(browser->objectName() + "/" + tv->objectName());

		if (! config.allKeys().contains(settingsKey)) {
			returnState = false;
			continue;
		}

		tv->restoreView();
		QString tabNam = config.value(settingsKey).toString();
		browser->showRelatTable( tabNam, tv );
	}

	/**** Recall dock states
	 \*/
	if (ui->actInpForm->isChecked()) {
		inpFrm->show();
		Qt::DockWidgetArea dwa = static_cast<Qt::DockWidgetArea>(
											 config.value("inpForm/DockWidgetArea",
															  Qt::BottomDockWidgetArea).toUInt());
		/*!
		 * Produces unexpected behavior
		 * this->addDockWidget(dwa, inpFrm, Qt::Vertical);
		 */
		INFO << dwa;
	}
	else inpFrm->hide();
	//		this->addDockWidget(dwa, inpFrm, Qt::Vertical);

	return returnState;
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
	/*                        Inherited menu components                         */
	/* ======================================================================== */
	QMenu *browserMenu = Browser::instance()->menuBarElement();
	menuBar()->addMenu( browserMenu );

//	browserMenu->addActions(Browser::instance()->menuBarElement());
}
void MainWindow::connectActions(ConnectReceiver receivers) {
	ui->actHideSqlQuery->setChecked( true );

	if ((receivers == connectThis) ||
		 (receivers == connectAll)) {
		connect(ui->actBrowseSQL, &QAction::triggered, this, &MainWindow::onBrowseSqlTrig);
		connect(ui->actInpForm, &QAction::triggered, this, &MainWindow::onOpenCloseInpFrm);
		connect(ui->actShowTbl, &QAction::triggered, this, &MainWindow::onTblOpen);
		connect(ui->actClose, &QAction::triggered, this, &MainWindow::onActCloseTrig);
		connect(ui->actGbStyleShtA, &QAction::triggered, this, &MainWindow::onMenuStyleShtATrig);
		connect(ui->actGbSShtInpFrm, &QAction::triggered, this, &MainWindow::onMenuStyleShtInpFrmTrig);
		connect(ui->actExport, &QAction::triggered, this, &MainWindow::onActExportTrig);
		connect(ui->actUnderConstr, &QAction::triggered, this, &MainWindow::onUnderConstrTrig);
		connect(ui->actSelFont, &QAction::triggered, this, &MainWindow::onSetFont);
		connect(ui->actResizerDlg, &QAction::triggered, this, &MainWindow::onResizerDlgTrig);
		connect(ui->actHideSqlQuery, &QAction::triggered, this, &MainWindow::onActHideSqlQueryTrig);
		connect(ui->actSetAlterRowCol, &QAction::triggered, this, &MainWindow::onSetAlterRowColTrig);
		connect(ui->actCfgInpFrmTabOrd,  &QAction::triggered, this, &MainWindow::onActCfgInpFrmTabOrdTrig);
		connect(ui->actSave,  &QAction::triggered, this, &MainWindow::onActSaveTrig);
		connect(ui->actOpen,  &QAction::triggered, this, &MainWindow::onActOpenTrig);
	}

	if ((receivers == connectOthers) ||
		 (receivers == connectAll)) {
		connect(ui->actCyclicObjInfo, &QAction::triggered, browser, &Browser::onCyclicObjInfoTrig);
		connect(ui->actAutoFitTables,	&QAction::triggered, browser, &Browser::autofitRowCol);
		connect(ui->actFilterForm, &QAction::triggered, browser, &Browser::onActFilterForm);
		connect(browser, &Browser::stateMsg, stateBar, &MDStateBar::showMessage);
		connect(inpFrm, &InpFrm::visibilityChanged, ui->actInpForm, &QAction::setChecked);
		connect(browser, &Browser::visibilityChanged, ui->actBrowseSQL, &QAction::setChecked);
	}
	//    connect(	ui->actFilterTableWindow,	SIGNAL(triggered(bool)),
	//                browser,					SLOT(onActFilterWindowTable(bool)));
	//    connect(	this,						SIGNAL(mainwindowCloses()),
	//                browser->getSortwindow0(),	SLOT(close()));
	//    connect(	this,						SIGNAL(mainwindowCloses()),
	//                browser->getSortwindow1(),	SLOT(close()));
	//    connect(	browser->getSortwindow0(),	SIGNAL(closesUncheck(bool)),
	//                ui->actFilterTableWindow,	SLOT(setChecked(bool)));
	//    connect(	browser->getSortwindow1(),	SIGNAL(closesUncheck(bool)),
	//                ui->actFilterTableWindow,	SLOT(setChecked(bool)));
	//    connect(ui->actFilterTable, SIGNAL(triggered()),
	//            browser,        SLOT( SORTIT()));
}
void MainWindow::initDocks() {
	QSETTINGS;
	inpFrm = InpFrm::instance( this );
	inpFrm->hide();

	inpFrm->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

	/*!
	  * Restore inpFrms last doc position.
	  */
	QVariant v = QVariant::fromValue(config.value("inpForm/DockWidgetArea",
																 Qt::BottomDockWidgetArea) );
	Qt::DockWidgetArea dockArea = static_cast<Qt::DockWidgetArea>(v.toInt());

	addDockWidget(dockArea, inpFrm);
}




