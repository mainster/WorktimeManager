#ifndef TABVIEW_H
#define TABVIEW_H

#include <QWidget>
#include <QGroupBox>

namespace Ui {
class TabView;
}

class TabView : public QGroupBox /*public QWidget*/ {
   Q_OBJECT

public:
   explicit TabView(QWidget *parent = 0);
   static TabView *getInstance(QWidget *parent = 0x00) {
      if (inst == 0)
         inst = new TabView(parent);

      return inst;
   }
   static TabView *getObjectPtr() {
      return inst;
   }
   ~TabView();

public slots:
   void on_insertRowAction_triggered()
   { insertRow(); }
   void on_deleteRowAction_triggered()
   { deleteRow(); }
   void on_fieldStrategyAction_triggered();
   void on_rowStrategyAction_triggered();
   void on_manualStrategyAction_triggered();
   void on_submitAction_triggered();
   void on_revertAction_triggered();
   void on_selectAction_triggered();

   void insertRow();
   void updateActions();
   void deleteRow();
   void on_connectionWidget_tableActivated(const QString &table);
protected slots:
//   void qSetBold(QObject *item, bool bold);

private:
   Ui::TabView *ui;
   static TabView   *inst;
   QList<QAction *>  tblActions;
};

#endif // TABVIEW_H
