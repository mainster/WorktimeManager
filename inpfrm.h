#ifndef INPFRM_H
#define INPFRM_H

#include <QtGui>
#include <QDockWidget>
#include <QDate>
#include <QtSql>
#include <QTextEdit>
#include <QListWidget>
#include <QInputDialog>
#include <QDataWidgetMapper>
#include <QWidget>
#include <stdio.h>
#include <QDateEdit>
#include <typeinfo>

#include "ui_tablewindow.h"
#include "globals.h"
#include "browser.h"
//#include "QSortFilterSqlQueryModel.h"
#include <QRadioButton>


namespace Ui {
class InpFrm;
}

class InpFrm : public QDockWidget {
   Q_OBJECT

public:
   enum state {
      state_none,
      state_init_changeTabOrder,
      state_running_changeTabOrder,
      state_done_changeTabOrder,
      state_rejected_changeTabOrder,
   };
   Q_DECLARE_FLAGS(states, state)

   explicit InpFrm(QWidget *parent = 0);
   static InpFrm *getInstance(QWidget *parent = 0x00) {
      if(inst == 0)
         inst = new InpFrm(parent);
      return inst;
   }
   static InpFrm *getObjectPtr() {
      return inst;
   }
   ~InpFrm();

   void restoreInpFrmGeometry() {
      QSETTINGS;
      this->restoreGeometry( config.value("InpFrm/Geometry", " ")
                             .toByteArray() );
   }
   void saveInpFrmGeometry() {
      QSETTINGS;
      config.setValue("InpFrm/Geometry", this->saveGeometry());
   }

   QSqlError addConnection(const QString &driver,
                           const QString &dbName,
                           const QString &host,
                           const QString &user,
                           const QString &passwd, int port);
   bool gbSqlQueryIsVisible() const;
   void gbSqlQuerySetVisible(bool vis);

signals:
    void changeInpFrmTabOrder(InpFrm::states state);

public slots:
   void showEvent(QShowEvent *);
   QString getQueryText() const;
   void saveSqlQueryInputText();
   void restoreSqlQueryInputText();
   void refreshCbDropDownLists();
   void onBtnOkClicked();
   void onBtnUndoClicked();
   void onBtnRedoClicked();
   void onBtnClearClicked();
   void onSqlQuerysTextChanged();
   void onCbQueryIdentIndexChaned(int idx);
   bool eventFilter(QObject *obj, QEvent *ev);
   void initComboboxes();
protected:
   void connectActions();

protected slots:
   void onInpFormChanges(int idx = -1);
   void onInpFormChanges(QDate date);
   void showEventDelay();
   void onChangeTabOrderSlot(InpFrm::states state);
   QList<QWidget *> objLstToWidLst(QList<QObject *> lst);

private:
   Ui::InpFrm *ui;
   static InpFrm *inst;
   void showError(const QSqlError &err);
   QSqlRelationalTableModel *model;
   int workerIdx, projIdx;
   QDialog *dlg;
   QVector<QRadioButton*> rbv;
   QStringList tblLst;
   QTextEdit *gbSqlQuery;
   Browser *browser;
   QDataWidgetMapper *prjm, *clm, *wkm;
   QSqlTableModel *prjmd, *clmd, *wkmd;
   QList<QComboBox *> cbs;
   QDateEdit *de;

//   QWidgetList wsTabable, objTabOrder;
//   QList<QWidget *> wsTabAble, objTabOrder;
//   QList<QWidget> wsNoP;
   QList<QObject *> objTabAble, objTabOrder;
//   QObjectList objTabable;

   InpFrm::states changeTabOrder;
//   void buildTable__worktime();
//   void buildTable__worktimeTst();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(InpFrm::states)
#endif // INPFRM_H
