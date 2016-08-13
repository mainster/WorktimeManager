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
	t2->start();
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

	/**** Recall dock states
	 \*/
	if (ui->actInpForm->isChecked()) {
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




