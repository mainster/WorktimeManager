#include "tabview.h"
#include "ui_tabview.h"

TabView *TabView::inst = 0;


TabView::TabView(QWidget *parent) :
   QGroupBox(parent),
   ui(new Ui::TabView) {
   ui->setupUi(this);

   tblActions.append(insertRowAction);
   tblActions.append(deleteRowAction);
   tblActions.append(fieldStrategyAction);
   tblActions.append(rowStrategyAction);
   tblActions.append(manualStrategyAction);
   tblActions.append(submitAction);
   tblActions.append(revertAction);
   tblActions.append(selectAction);
}

TabView::~TabView() {
   delete ui;
}

void TabView::insertRow(/*QTableView &tv*/) {
   /**
    * Loop through every child of your QSplitter and call hasFocus() for
    * each one so you can know which of them has the focus (keyboard focus)
    */
   //   QObjectList objs(this->spHorzPrim->children());

   for (int i=0; i < tabVs.length(); i++) {
      //      QTableView *childa = qobject_cast<QTableView *>(objs.at(i));

      if ( tabVs.at(i)->hasFocus() )
         Q_INFO << "table" << i << "has focus";
   }

   QTableView *tvloc = qobject_cast<QTableView *>( tabVs.first() );

   QSqlTableModel *model = qobject_cast<QSqlTableModel *>(tvloc->model());

   if (!model)
      return;

   QModelIndex insertIndex = tvloc->currentIndex();
   int row = insertIndex.row() == -1 ? 0 : insertIndex.row();
   model->insertRow(row);
   insertIndex = model->index(row, 0);
   tvloc->setCurrentIndex(insertIndex);
   tvloc->edit(insertIndex);

   QWidget *wid = QWidget::focusWidget();
   Q_INFO << wid->accessibleName();

   wid = QApplication::focusWidget(),
         Q_INFO << wid->accessibleName();
}
void TabView::deleteRow(/*QTableView &tv*/) {
   //   QSqlTableModel *model = qobject_cast<QSqlTableModel *>(tvs.first()->model());
   QSqlTableModel *model = qobject_cast<QSqlTableModel *>(tabVs.first()->model());
   if (!model)
      return;

   QModelIndexList currentSelection =
         tabVs.first()->selectionModel()->selectedIndexes();
   for (int i = 0; i < currentSelection.count(); ++i) {
      if (currentSelection.at(i).column() != 0)
         continue;
      model->removeRow(currentSelection.at(i).row());
   }

   updateActions();
}
void TabView::updateActions(/*QTableView &tv*/) {
   QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(tabVs.first()->model());
   bool enableIns = tm;
   bool enableDel = enableIns && tabVs.first()->currentIndex().isValid();

   insertRowAction->setEnabled(enableIns);
   deleteRowAction->setEnabled(enableDel);

   fieldStrategyAction->setEnabled(tm);
   rowStrategyAction->setEnabled(tm);
   manualStrategyAction->setEnabled(tm);
   submitAction->setEnabled(tm);
   revertAction->setEnabled(tm);
   selectAction->setEnabled(tm);

   if (tm) {
      QSqlTableModel::EditStrategy es = tm->editStrategy();
      fieldStrategyAction->setChecked(es == QSqlTableModel::OnFieldChange);
      rowStrategyAction->setChecked(es == QSqlTableModel::OnRowChange);
      manualStrategyAction->setChecked(es == QSqlTableModel::OnManualSubmit);
   }
}
void TabView::on_fieldStrategyAction_triggered() {
   QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(tabVs.first()->model());
   if (tm)
      tm->setEditStrategy(QSqlTableModel::OnFieldChange);
}
void TabView::on_rowStrategyAction_triggered() {
   QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(tabVs.first()->model());
   if (tm)
      tm->setEditStrategy(QSqlTableModel::OnRowChange);

   QWidget *wid = focusWidget();
   Q_INFO << wid->objectName();
}
void TabView::on_manualStrategyAction_triggered() {
   QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(tabVs.first()->model());
   if (tm)
      tm->setEditStrategy(QSqlTableModel::OnManualSubmit);
}
void TabView::on_submitAction_triggered() {
   QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(tabVs.first()->model());
   if (tm)
      tm->submitAll();
}
void TabView::on_revertAction_triggered() {
   QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(tabVs.first()->model());
   if (tm)
      tm->revertAll();
}
void TabView::on_selectAction_triggered() {
   QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(tabVs.first()->model());
   if (tm)
      tm->select();
}
void TabView::on_connectionWidget_tableActivated(const QString &table) {
   //   QAbstractItemModel * im;
   QTableView *tvl;

   foreach (tvl, tabVs) {
      if (tvl->model() == 0) {
         showTable(table, tvl);
         return;
      }
   }
}

//static void TabView::qSetBold(QObject *item, bool bold) {
//   QFont font = item->font(0);
//   font.setBold(bold);
//   item->setFont(0, font);
//}
