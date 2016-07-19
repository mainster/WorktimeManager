#include "mainwindow.h"
#include "ui_mainwindow.h"

#define  RESTORE_SQL_DATABASE_PATH

int MainWindow::fuse = 5;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
	ui(new Ui::MainWindow) {
	ui->setupUi(this);
	QSETTINGS_INIT; QSETTINGS;

	stateBar		= new MDStateBar( this );
	browser		= new Browser( parent );
	mDbc			= new DbController(this);
	inpFrm		= new InpFrm( this );
	sortwindow  = new SortWindow();
	stateBar->setClockVisible(true);
	setStatusBar( stateBar );

	/*!
	 * Connect the inpFrm's statusMessage signal to the statebar message slot
	 */
	connect(inpFrm, &InpFrm::stateMessage, stateBar, &MDStateBar::showMessage);


	makeMenuBar();
	installEventFilter(this);

	this->setWindowTitle( MAINWINDOW_TITLE );
	config.fileName();
	connectActions();
}
MainWindow::~MainWindow() {
	//   saveMainWindowUiSettings();
	delete ui;
}
void MainWindow::onInpFrmButtonClick(bool b) {

}
void MainWindow::makeMenuBar() {
	QMenu *fileMenu = menuBar()->addMenu(QObject::tr("&File"));
	fileMenu->addAction(QObject::tr("Add &Connection..."),
							  browser, SLOT(addConnection()));
	fileMenu->addSeparator();
	fileMenu->addAction(QObject::tr("&Quit"), qApp, SLOT(quit()));

	//   cfgMenu->addMenu(tr("StyleSheets"));
	//   cfgMenu->addAction(QObject::tr("gbStyleShtA"), this,
	//                      SLOT(onMenuStyleShtATrig(bool)));
	//   cfgMenu->addAction(QObject::tr("gbStyleShtInpFrm"), this,
	//                      SLOT(onMenuStyleShtInpFrmTrig(bool)));

	QMenu *helpMenu = menuBar()->addMenu(QObject::tr("&Help"));
	helpMenu->addAction(QObject::tr("About"), this, SLOT(about()));
	helpMenu->addAction(QObject::tr("About Qt"), qApp, SLOT(aboutQt()));

}
void MainWindow::connectActions() {
	ui->actHideSqlQuery->setChecked( true );


	connect(	ui->actButBrowseSQL,	SIGNAL(triggered(bool)),
				this,					SLOT(onBrowseSqlTrig(bool)));
	connect(	ui->actButInpForm,		SIGNAL(triggered(bool)),
				this,					SLOT(onOpenCloseInpFrm(bool)));
	connect(	ui->actButShowTbl,		SIGNAL(triggered(bool)),
				this,					SLOT(onTblOpen(bool)));
	connect(	ui->actButClose,		SIGNAL(triggered()),
				this,					SLOT(onActCloseTrig()));
	connect(	ui->actButGbStyleShtA,	SIGNAL(triggered(bool)),
				this,					SLOT(onMenuStyleShtATrig(bool)));
	connect(	ui->actButGbStyleShtInpFrm,	SIGNAL(triggered(bool)),
				this,					SLOT(onMenuStyleShtInpFrmTrig(bool)));
	connect(	ui->actButExport,		SIGNAL(triggered()),
				this,					SLOT(onActExportTrig()));
	connect(	ui->actButUnderConstr,	SIGNAL(triggered()),
				this,					SLOT(onUnderConstrTrig()));
	connect(	ui->actButSelFont,		SIGNAL(triggered()),
				this,					SLOT(onSetFont()));
	connect(	ui->actResizerDlg,		SIGNAL(triggered()),
				this,					SLOT(onResizerDlgTrig()));
	connect(	ui->actHideSqlQuery,	SIGNAL(triggered()),
				this,					SLOT(onActHideSqlQueryTrig()));
	connect(	ui->actSetAlterRowCol,	SIGNAL(triggered()),
				this,					SLOT(onSetAlterRowColTrig()));
	connect(	ui->actCfgInpFrmTabOrd, SIGNAL(triggered()),
				this,					SLOT(onActCfgInpFrmTabOrdTrig()));
	connect(	ui->actButCyclicObjInfo,SIGNAL(triggered(bool)),
				browser,SLOT(onCyclicObjInfoTrig(bool)));
	connect(	ui->actAutoFitTables,	SIGNAL(triggered(bool)),
				browser,SLOT(autofitRowCol()));
	connect(    ui->actFilterForm,          SIGNAL(triggered(bool)),
					browser,                    SLOT(onActFilterForm(bool)));
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
	connect( browser,               SIGNAL(stateMsg(QString)),
				stateBar,              SLOT(showMessage(QString)));
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
void MainWindow::onBrowseSqlTrig(bool doBrowse) {

	if (doBrowse) {
		this->setCentralWidget(browser);
		browser->show();

		if ((! mDbc->getDb().isOpen()) ||
			 (mDbc->getDb().isOpenError()))
			mDbc->getConnectionState();
	}
	else {
		QSETTINGS;
		saveMainWindowGeometry();
		QMetaObject::invokeMethod(browser, "close");

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
void MainWindow::onTblOpen(bool b) {
	INFO << tr("I am an empty slot, use me!!!");
}
void MainWindow::onSetAlterRowColTrig() {
	QPalette pal = browser->tvs.first()->tv()->palette();

	QColor color = QColorDialog::getColor(
							pal.background().color(), this,
							"Farbe für alternierenden Zeilenhintergrund",
							QColorDialog::DontUseNativeDialog);

	if (color.isValid()) {
		foreach (TabView *tv, browser->tvs) {
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
	inpFrm->gbSqlQuerySetVisible( !dohide );
}
void MainWindow::onMenuStyleShtATrig(bool b) {
	if (!b) return;

	ui->actButGbStyleShtInpFrm->setChecked( !b );
	ui->actButGbStyleShtA->setChecked( b );

	foreach (TabView *tv, browser->tvs) {
		tv->setStyleSheet( Globals::gbStyleShtCenterPROPERTYS);
	}
}
void MainWindow::onMenuStyleShtInpFrmTrig(bool b) {
	if (!b) return;

	//   ui->actButStyleShtA->setChecked( !b );
	//   ui->actButStyleShtInpFrm->setChecked( b );

	//   browser->tvs[0]->getGrBox();
	//   foreach (QGroupBox *gb, browser->getGBoxs()) {
	//      gb->setStyleSheet( Globals::gbStyleShtInpFrm);
	//   }
}
void MainWindow::onActExportTrig() {
	INFO << "!";
}
void MainWindow::onActCfgInpFrmTabOrdTrig() {
	emit inpFrm->changeInpFrmTabOrder(InpFrm::state_init_changeTabOrder);
}
void MainWindow::initializeMdl(QSqlQueryModel *model) {
	QSETTINGS;
	QSqlDatabase pDb = browser->getCurrentDatabase();
	QString queryStr =
			config.value("InpFrm/default_worktime_query_plain").toString();

	model->setQuery(QSqlQuery(queryStr, pDb));
	model->setHeaderData(0, Qt::Horizontal, QObject::tr("workerID"));
	model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nachname"));
	model->setHeaderData(2, Qt::Horizontal, QObject::tr("Vorname"));
	INFO << config.value("InpFrm/default_worktime_query_plain").toString();


	////   mdl->setQuery("SELECT prjID, clientID, SubID, Beschreibung, Kurzform "
	////                 "FROM prj", pDb);
	//   mdl->setQuery("SELECT * FROM prj");
	//   mdl->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
	//   mdl->setHeaderData(1, Qt::Horizontal, QObject::tr("clId"));
	//   mdl->setHeaderData(2, Qt::Horizontal, QObject::tr("subId"));


	{
		//QTableView* MainWindow::createView(QSqlQueryModel *model,
		//                                const QString &title) {
		//   QTableView *view = new QTableView(0);
		//   view->setModel(model);
		//   static int offset = 0;

		//   view->setWindowTitle(title);
		//   view->move(100 + offset, 100 + offset);
		//   offset += 20;
		//   view->show();

		//   return view;
		//
	}
}
void MainWindow::onUnderConstrTrig() {
	QSqlQueryModel plainMdl;

	//   EditableSqlMdl editableMdl;
	initializeMdl( &plainMdl );
	//   createView( &plainMdl, QObject::tr("plainMdl Query Model"));
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
	//   browser->resetStyleSheet(browser->tvs[0]);
	//   browser->resetStyleSheet(browser->tvs[1]);
	//   browser->resetStyleSheet(browser->tvs[3]);
	//   browser->clearStyleSheet(browser->tvs[0]);
	//   browser->clearStyleSheet(browser->tvs[2]);
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
void MainWindow::about() {
	QMessageBox::about(this, tr("About"), tr("The SQL Browser demonstration "
														  "shows how a data browser can be used to visualize the results of SQL"
														  "statements on a live database"));
}
void MainWindow::onCyclic() {

}
void MainWindow::onActCloseTrig() {
	//    qApp->closeAllWindows();
	this->close();
}
/* ======================================================================== */
/*                     Event handler implementation                         */
/* ======================================================================== */
void MainWindow::showEvent() {
	QShowEvent *ee = NULL;
	emit showEvent(ee);
}
void MainWindow::showEvent(QShowEvent *e) {
	Q_UNUSED(e);
	QSETTINGS;
	INFO << tr("SHOW!");

	/**** Recall all action states for MainWindow
	 \*/
	QList<QAction *> acts = findChildren<QAction *>();

	foreach (QAction *ac, acts) {
		if ( config.value("MainWindow/" + ac->objectName(), false).toBool() )
			emit ac->trigger();
	}

	try {
		/**** Recall visibility flag for the SQL command interface
	 \*/
		if (!config.value("MainWindow/actHideSqlQuery", true).toBool()) {
			inpFrm->gbSqlQuerySetVisible( true );
			ui->actHideSqlQuery->setChecked( false );
		} else {
			inpFrm->gbSqlQuerySetVisible( false );
			ui->actHideSqlQuery->setChecked( true );
		}
	} catch (...) {
		CRIT << tr("!");
	}

	/**** Recall mainWindow geometry and window state
	 \*/
	this->restoreGeometry(config.value(objectName() + "/Geometry", "").toByteArray());
	this->restoreState(config.value(objectName() + "/State", "").toByteArray());

	/**** Recall dock states
	 \*/
	Qt::DockWidgetArea dwa = static_cast<Qt::DockWidgetArea>(
										 config.value("inpForm/DockWidgetArea",
														  Qt::BottomDockWidgetArea).toUInt());
	this->addDockWidget(dwa, inpFrm, Qt::Vertical);

	/**** Restore custom TabView instance settings
	 \*/
	foreach (TabView *tv, browser->tvs) {
		tv->restoreView();
	}

	/**** Restore tabview <-> SQL table assignements
	 \*/
	foreach (TabView *tv, browser->tvs) {
		QString tabNam = config.value(
								  browser->objectName() + "/" + tv->objectName()).toString();
		browser->showRelatTable( tabNam, tv );
	}

	(ui->actButInpForm->isChecked())  ?  inpFrm->show()   :  inpFrm->hide();

	/**** Trigger the connectionWidget treeview
	 \*/

}
bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
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

	// standard event processing
//	return false;

	return QObject::eventFilter(obj, event);
}
void MainWindow::closeEvent(QCloseEvent *e) {
	QSETTINGS;


	/**** Write splitter sizes to config
	  \*/
	QList<QSplitter *> spls = browser->findChildren<QSplitter *>();

	foreach (QSplitter *sp, spls) {
		config.setValue("Browser/" + sp->objectName(), sp->saveState());
	}

	/**** Write tabView <-> SQL table relations
	  \*/
	foreach (TabView *tv, browser->tvs) {
		config.setValue(browser->objectName() + "/" +
							 tv->objectName(), tv->grBox()->title());
	}

	/**** Write browser widget geometrys
	  \*/
	config.setValue(browser->objectName() + "/Geometry", browser->saveGeometry());

	/**** Write mainWindow geometry and window state
	  \*/
	config.setValue(this->objectName() + "/Geometry", this->saveGeometry());
	config.setValue(this->objectName() + "/State", this->saveState());

	/**** Safe all action states from MainWindow
	  \*/
	QList<QAction *> acts;

	foreach (QAction *ac, findChildren<QAction *>())
		if (ac->objectName().contains("act", Qt::CaseSensitive)) {
			config.setValue("MainWindow/" + ac->objectName(), ac->isChecked());
			acts << ac;
		}
	config.setValue("MainWindow/actionCtr", acts.length());

	/**** Safe current docking area of dock widgets
	  \*/
	config.setValue("inpForm/DockWidgetArea", (uint)dockWidgetArea(inpFrm));
	config.sync();

	/**** Safe content of sql query input field to config
	  \*/
	emit mainwindowCloses();
	QWidget::closeEvent(e);
}
/* ======================================================================== */
/*									    Init methodes										    */
/* ======================================================================== */
void MainWindow::initial() {

}
