#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFlag>
#include <QFlags>
#include <QMainWindow>
#include <QtSql>
#include <QtWidgets>

#include "ui_dbconndlg.h"
#include "browser.h"
#include "connectionwidget.h"
#include "dbconndlg.h"
#include "form.h"
#include "globals.h"
#include "inpfrm.h"
#include "locals.h"
#include "mdstatebar.h"
#include "tabledelegate.h"
#include "tablewindow.h"
#include "dbcontroller.h"

namespace Ui {
class MainWindow;
}

class ConnectionWidget;
QT_FORWARD_DECLARE_CLASS(QTableView)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QTextEdit)
QT_FORWARD_DECLARE_CLASS(QSqlError)

class MainWindow : public QMainWindow {
   Q_OBJECT

public:
   explicit MainWindow(QWidget *parent = 0);
//   static MainWindow *getObjectPtr() {
//      return inst;
//   }

   ~MainWindow();

signals:
    void mainwindowCloses();

public slots:
   void onBrowseSqlTrig(bool b);
	bool addConnectionsByCmdline(QVariant args,
												  Browser &browser);
   bool eventFilter(QObject *obj, QEvent *event);
   void about();
   void onMenuStyleShtInpFrmTrig(bool b);
   void onMenuStyleShtATrig(bool b);
   void onActExportTrig();
   void onUnderConstrTrig();
   void onSetFont();
   void closeEvent(QCloseEvent *e);
   void initDocks();
   void showEvent();
   void showEvent(QShowEvent *e);
   void onResizerDlgTrig();
   void onActHideSqlQueryTrig();
   void onSetAlterRowColTrig();
   void initializeMdl(QSqlQueryModel *model);
   void connectActions();
   void onCyclic();

protected slots:
   void makeMenuBar();
   void onActCfgInpFrmTabOrdTrig();

private slots:
   void onInputFormTrig(bool b);
   void onTblOpen(bool b);
   void onActCloseTrig();
   void restoreMainWindowGeometry() {
      QSETTINGS;
      restoreGeometry( config.value(this->objectName() +
                                    "/Geometry", " ").toByteArray() );
   }
   void saveMainWindowGeometry() {
      QSETTINGS;
      config.setValue(this->objectName() +
                      "/Geometry", this->saveGeometry());
   }
   void restoreMainWindowState() {
      QSETTINGS;
      restoreState( config.value(this->objectName() +
                                    "/State", " ").toByteArray() );
   }

private:
   Ui::MainWindow *ui;
   static int   fuse;
   Browser      *browser;
   InpFrm       *inpFrm;
   TableWindow  *tblWin;
   QMenu        *cfgMenu;
   QWidget      *wid;
   MDStateBar   *stateBar;
   SortWindow   *sortwindow;
   QMenu        *mBar;


//   static MainWindow *inst;
};

#endif // MAINWINDOW_H
