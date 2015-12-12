#define QFOLDINGSTART {
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "browser.h"
#include "inpfrm.h"
#include "globals.h"
//#include "conf.h"
#include "dbconndlg.h"
#include "ui_dbconndlg.h"
#include "connectionwidget.h"
#include "form.h"

#include <QtCore>
#include <QtWidgets>
#include <QtSql>
#include <QFlags>
#include <QFlag>

#define  UNIX
#undef   UNIX
#define  RESTORE_SQL_DATABASE_PATH

#define QFOLDINGEND }
class Browser;
class Conf;
class ConnectionWidget;

int MainWindow::fuse = 5;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
   ui(new Ui::MainWindow) {
   ui->setupUi(this);

   browser = Browser::getInstance( parent );
   inpForm = InpFrm::getInstance( );

   makeMenuBar();
   connectActions();
   browser->onCyclicObjInfoTrig(false);
   installEventFilter(this);

   this->setWindowTitle( MAINWINDOW_TITLE );
}
MainWindow::~MainWindow() {
   //   saveMainWindowUiSettings();
   delete ui;
}

void MainWindow::addConnectionsByCmdline(const QStringList &args,
                                               Browser &browser) {
   QSqlError err;

   for (int i = 1; i < args.count(); ++i) {
#ifdef UNIX
      QUrl url(args.at(i), QUrl::TolerantMode);
#elif defined RESTORE_SQL_DATABASE_PATH
      QUrl url(Globals::SQL_DB_PATH, QUrl::TolerantMode);
      url.setScheme("QSQLITE");
#endif

      if (!url.isValid()) {
         qWarning("Invalid URL: %s", qPrintable(args.at(i)));
         continue;
      }

      err = browser.addConnection(
               url.scheme().toUpper(), url.path(), url.host(),
               url.userName(), url.password(), url.port(-1));

      if (err.type() != QSqlError::NoError)
         qDebug() << "Unable to open connection:" << err;
   }
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

   connect(ui->actButBrowseSQL,    SIGNAL(triggered(bool)),
           this,                   SLOT(onBrowseSqlTrig(bool)));
   connect(ui->actButInpForm,       SIGNAL(triggered(bool)),
           this,                 SLOT(onInputFormTrig(bool)));
   connect(ui->actButShowTbl,       SIGNAL(triggered(bool)),
           this,                 SLOT(onTblOpen(bool)));
   connect( browser,                SIGNAL(statusMessage(QString)),
            statusBar(),            SLOT(showMessage(QString)));
   connect( ui->actButClose,           SIGNAL(triggered()),
            this,                   SLOT(onActCloseTrig()));
   connect( ui->actButGbStyleShtA,     SIGNAL(triggered(bool)),
            this,                   SLOT(onMenuStyleShtATrig(bool)));
   connect( ui->actButGbStyleShtInpFrm, SIGNAL(triggered(bool)),
            this,                   SLOT(onMenuStyleShtInpFrmTrig(bool)));
   connect( ui->actButExport,          SIGNAL(triggered()),
            this,                   SLOT(onActExportTrig()));
   connect( ui->actButUnderConstr,     SIGNAL(triggered()),
            this,                   SLOT(onUnderConstrTrig()));
   connect( ui->actButSelFont,         SIGNAL(triggered()),
            this,                   SLOT(onSetFont()));
   connect( ui->actButCyclicObjInfo,   SIGNAL(triggered(bool)),
            browser,                SLOT(onCyclicObjInfoTrig(bool)));
   connect( ui->actResizerDlg,      SIGNAL(triggered()),
            this,                   SLOT(onResizerDlgTrig()));
   connect( ui->actHideSqlQuery,    SIGNAL(triggered()),
            this,                   SLOT(onActHideSqlQueryTrig()));
   connect( ui->actSetAlterRowCol,  SIGNAL(triggered()),
            this,                   SLOT(onSetAlterRowColTrig()));
}
void MainWindow::initDocks() {
   QSETTINGS
         inpForm = InpFrm::getInstance( this );
   inpForm->setAllowedAreas(Qt::TopDockWidgetArea |
                            Qt::BottomDockWidgetArea);

   QVariant v =
         QVariant::fromValue(config.value("inpForm/DockWidgetArea",
                                          Qt::BottomDockWidgetArea) );

   //   Qt::DockWidgetAreas dar = static_cast<Qt::DockWidgetAreas>(var.toInt());

   addDockWidget(Qt::BottomDockWidgetArea, inpForm);
}

void MainWindow::onBrowseSqlTrig(bool doBrowse) {

   if (doBrowse) {
      this->setCentralWidget(browser);
      browser->show();

      if (QSqlDatabase::connectionNames().length() == 0+1) // default + ...
         addConnectionsByCmdline(
                  QStringList(Globals::SQL_DB_PATH), *browser);

      if (QSqlDatabase::connectionNames().isEmpty())
         QMetaObject::invokeMethod(
                  browser, "addConnection", Qt::QueuedConnection);
   } else {
      QSETTINGS
            saveMainWindowGeometry();
      QMetaObject::invokeMethod(browser, "close");

      browser->hide();
   }
}
void MainWindow::onInputFormTrig(bool b) {
   /**
      * Fetch pointer if a InpForm instance exists, instantiate and get pointer
      * if not
      */
   inpForm = InpFrm::getInstance();
   QSETTINGS;

   //   this->addDockWidget(Qt::BottomDockWidgetArea, inpForm);

   if (b) {
      inpForm->restoreInpFrmGeometry();
      //      addDockWidget( static_cast<Qt::DockWidgetArea>(
      //                        config.value("inpForm/DockWidgetArea", "8").toInt) );
      inpForm->show();
   } else {

      //      config.setValue("inpForm/DockWidgetArea", (uint)dockWidgetArea(inpForm));
      inpForm->saveInpFrmGeometry();
      inpForm->hide();
   }
}
void MainWindow::onTblOpen(bool b) {
   //   if (tblWin == 0x00)
   tblWin = TableWindow::getInstance();

   if (b) {
      tblWin->restoreTableWinGeometry();
      tblWin->show();
   } else {
      tblWin->saveTableWinGeometry();
      tblWin->hide();
   }
}
void MainWindow::onActCloseTrig() {
   //    qApp->closeAllWindows();
   this->close();
}
void MainWindow::onSetAlterRowColTrig() {
   QPalette pal = browser->tvs.first()->tv()->palette();

   QColor color = QColorDialog::getColor(
            pal.background().color(), this,
            "Farbe für alternierenden Zeilenhintergrund",
            QColorDialog::DontUseNativeDialog);

   if(color.isValid()) {
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
             .size() * text.toDouble()/100);
}
void MainWindow::onActHideSqlQueryTrig() {
   bool dohide = ui->actHideSqlQuery->isChecked();
   QSETTINGS;
   config.setValue(objectName() + "/HideSqlQuery", dohide);
   inpForm->gbSqlQuerySetVisible( !dohide );
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
   Q_INFO << "!";
}

///** Testing (relational) editableSqlModel    19-11-2015 */
void MainWindow::initializeMdl(QSqlQueryModel *model) {
   QSETTINGS;
   QSqlDatabase pDb = browser->getCurrentDatabase();
   QString queryStr =
         config.value("InpFrm/default_worktime_query_plain").toString();

   model->setQuery(QSqlQuery(queryStr, pDb));
   model->setHeaderData(0, Qt::Horizontal, QObject::tr("workerID"));
   model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nachname"));
   model->setHeaderData(2, Qt::Horizontal, QObject::tr("Vorname"));
   Q_INFO << config.value("InpFrm/default_worktime_query_plain").toString();


////   mdl->setQuery("SELECT prjID, clientID, SubID, Beschreibung, Kurzform "
////                 "FROM prj", pDb);
//   mdl->setQuery("SELECT * FROM prj");
//   mdl->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
//   mdl->setHeaderData(1, Qt::Horizontal, QObject::tr("clId"));
//   mdl->setHeaderData(2, Qt::Horizontal, QObject::tr("subId"));
}
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
//}
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

   for (int i=0; i <5; i++) {
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
bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
   if ((event->type() == QEvent::MouseButtonDblClick) ||
       (event->type() == QEvent::MouseButtonPress) ||
       (event->type() == QEvent::MouseButtonRelease) ||
       (event->type() == QEvent::MouseMove) ||
       (event->type() == QEvent::MouseTrackingChange)) {
      Q_INFO << event->type() << "    FROM MOUSE!";
      return QObject::eventFilter(obj, event);
   }

   if (event->type() == QEvent::KeyPress) {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
      qDebug("Ate key press %d", keyEvent->key());
      return QObject::eventFilter(obj, event);
   }

   // standard event processing
   return QObject::eventFilter(obj, event);
}
void MainWindow::showEvent() {
   QShowEvent *ee = NULL;
   emit showEvent(ee);
}
void MainWindow::showEvent(QShowEvent *e) {
   QSETTINGS;  Q_UNUSED(e);

   /// Recall all action states for MainWindow
   ///
   QList<QAction *> acts = findChildren<QAction *>();
   foreach (QAction *ac, acts) {
      if( config.value("MainWindow/"+ac->objectName(), false).toBool() )
         emit ac->trigger();
   }

   if (!config.value("MainWindow/HideSqlQuery", true).toBool()) {
      inpForm->gbSqlQuerySetVisible( true );
      ui->actHideSqlQuery->setChecked( false );
   }
   else {
      inpForm->gbSqlQuerySetVisible( false );
      ui->actHideSqlQuery->setChecked( true );
   }

   /// Recall mainWindow geometry and window state
   ///
   this->restoreGeometry(
            config.value(objectName() + "/Geometry", "").toByteArray());
   this->restoreState(
            config.value(objectName() + "/State", "").toByteArray());

   /// Recall dock states
   ///
   Qt::DockWidgetArea dwa =
         static_cast<Qt::DockWidgetArea>(
            config.value("inpForm/DockWidgetArea",
                         Qt::BottomDockWidgetArea).toUInt());

   /// Restore custom TabView instance settings
   ///
   foreach (TabView *tv, browser->tvs) {
      tv->restoreView();
   }

   this->addDockWidget(dwa, inpForm, Qt::Vertical);

   (ui->actButInpForm->isChecked())
         ?  inpForm->show()   :  inpForm->hide();

   /// Trigger the connectionWidget treeview
   ///

}
/*!
 \brief MainWindow closeEvent() is used to refresh the gui settings rfile
 before the program exits.

 \param e
*/
void MainWindow::closeEvent(QCloseEvent *e) {
   QSETTINGS;

   /**** Write splitter sizes to config
    \*/
   QList<QSplitter *> spls = browser->findChildren<QSplitter *>();
   foreach (QSplitter *sp, spls)
      config.setValue("Browser/" + sp->objectName(), sp->saveState());

   config.setValue(browser->objectName() + "/Geometry", browser->saveGeometry());

   /**** Write mainWindow geometry and window state
    \*/
   config.setValue(this->objectName() + "/Geometry", this->saveGeometry());
   config.setValue(this->objectName() + "/State", this->saveState());

   /**** Safe all action states from MainWindow
    \*/
   int nr = 0;
   QList<QAction *> acts = findChildren<QAction *>();
   foreach (QAction *ac, acts)
      if (ac->objectName().contains("actBut",Qt::CaseSensitive)) {
         config.setValue("MainWindow/" + ac->objectName(), ac->isChecked());
         nr++;
      }
   config.setValue("MainWindow/actionCtr", nr);

   /**** Safe current docking area of dock widgets
    \*/
   config.setValue("inpForm/DockWidgetArea", (uint)dockWidgetArea(inpForm));
   config.sync();

   /**** Safe content of sql query input field to config
    \*/

   QWidget::closeEvent(e);
}
