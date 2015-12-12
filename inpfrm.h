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

#include "ui_tablewindow.h"
#include "globals.h"
#include "browser.h"
#include "QSortFilterSqlQueryModel.h"
#include <QRadioButton>


namespace Ui {
class InpFrm;
}

class InpFrm : public QDockWidget {
   Q_OBJECT

public:
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

public slots:
   void on_clearButton_clicked();
   void on_submitButton_clicked();
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

//   void buildTable__worktime();
//   void buildTable__worktimeTst();
};

#endif // INPFRM_H
