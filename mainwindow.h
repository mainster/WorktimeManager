#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "globals.h"
#include "browser.h"
#include "inpfrm.h"
//#include "tablewindow.h"
#include "tabledelegate.h"
#include "dbconndlg.h"
#include "form.h"
#include "mdstatebar.h"

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
   ~MainWindow();

signals:
    void mainwindowCloses();

public slots:
   void onBrowseSqlTrig(bool b);
   void addConnectionsByCmdline(const QStringList &args,
                                      Browser &browser);
   void about();
   void onMenuStyleShtInpFrmTrig(bool b);
   void onMenuStyleShtATrig(bool b);
   void onActExportTrig();
   void onUnderConstrTrig();
   void onSetFont();
   void initDocks();
   void showEvent();
   void onResizerDlgTrig();
   void onActHideSqlQueryTrig(bool b);
   void onSetAlterRowColTrig();
   void initializeMdl(QSqlQueryModel *model);
   void connectActions();
   void onCyclic();
   void showEvent(QShowEvent *e);
   void closeEvent(QCloseEvent *e);
   bool eventFilter(QObject *obj, QEvent *event);

protected slots:
   void makeMenuBar();
   void onActCfgInpFrmTabOrdTrig();

private slots:
   void onInputFormTrig(bool b);
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
   Ui::MainWindow   *ui;
   static int       fuse;
   Browser          *browser;
   InpFrm           *inpFrm;
   QMenu            *cfgMenu;
   Form             *form;
   QWidget          *wid;
   MDStateBar       *stateBar;
   FiltForm       *filtform;
   QMenu            *mBar;


//   static MainWindow *inst;
};

#endif // MAINWINDOW_H
