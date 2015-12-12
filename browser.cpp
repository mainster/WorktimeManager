#include "browser.h"
#include "dbconndlg.h"
#include "globals.h"
#include "tabview.h"

#include <QtWidgets>
#include <QFrame>
#include <QWidget>
#include <QtSql>
#include <QGroupBox>
#include <QtEvents>
#include <QCloseEvent>
#include "inpfrm.h"

#define T_CYCLIC 250e-3


#define TVA    "TableLeft"
#define TVB    "TableCenter"
#define TVC    "TableRight"
#define TVM    "TableMain"

Browser *Browser::inst = 0;

Browser::Browser(QWidget *parent)  :
   QWidget(parent), ui(new Ui::Browser) {
   setupUi(this);

   inst = this;

   QStringList accNam;
   accNam << TVA << TVB << TVC << TVM;
   initTableView( inst, accNam);

   //   connect (ui->btnA,      SIGNAL(clicked()),
   //            this,          SLOT(onTestBtnClicked()));
   //   connect (ui->btnB,      SIGNAL(clicked()),
   //            this,          SLOT(onTestBtnClicked()));
   connect(this,     SIGNAL(tabViewSelChanged(TabView *)),
           tvs[0],   SLOT(onTabViewSelChanged(TabView *)));
   connect(this,     SIGNAL(tabViewSelChanged(TabView *)),
           tvs[1],   SLOT(onTabViewSelChanged(TabView *)));
   connect(this,     SIGNAL(tabViewSelChanged(TabView *)),
           tvs[2],   SLOT(onTabViewSelChanged(TabView *)));
   connect(this,     SIGNAL(tabViewSelChanged(TabView *)),
           tvs[3],   SLOT(onTabViewSelChanged(TabView *)));

   //   restoreBrowserUiSettings();

   if (QSqlDatabase::drivers().isEmpty())
      QMessageBox::information
            (this, tr("No database drivers found"),
             tr("This demo requires at least one Qt database driver. "
                "Please check the documentation how to build the "
                "Qt SQL plugins."));
   else {
      timCyc = new QTimer();
      timCyc->setInterval(T_CYCLIC * 1e3);
      connect( timCyc,    SIGNAL(timeout()),
               this,      SLOT(onCyclic()));
      timCyc->start();
   }

   emit statusMessage(tr("Browser Ready."));

   CustomMdl *cm = new CustomMdl();
   cm->setCCol(QVector<int>(0, 1));
}

Browser::~Browser() {
   //   saveBrowserUiSettings();
   delete ui;
}
QSqlError Browser::
addConnection(const QString &driver, const QString &dbName,
              const QString &host, const QString &user,
              const QString &passwd, int port) {
   static int cCount = 0;

   QSqlError err;
   QSqlDatabase db = QSqlDatabase::addDatabase(driver, QString("Browser%1")
                                               .arg(++cCount));
   db.setDatabaseName(dbName);
   db.setHostName(host);
   db.setPort(port);

   if (!db.open(user, passwd)) {
      err = db.lastError();
      db = QSqlDatabase();
      QSqlDatabase::removeDatabase(QString("Browser%1").arg(cCount));
   }

   connectionWidget->refresh();

   return err;
}
void Browser::initTableView(QWidget *parent,
                            QStringList &accNam) {
   Q_UNUSED(parent)
   QList<QAction *> tblActs;

   tblActs.append(insertRowAction);
   tblActs.append(deleteRowAction);
   tblActs.append(fieldStrategyAction);
   tblActs.append(rowStrategyAction);
   tblActs.append(manualStrategyAction);
   tblActs.append(submitAction);
   tblActs.append(revertAction);
   tblActs.append(selectAction);

   tvs.append( tva );
   tvs.append( tvb );
   tvs.append( tvc );
   tvs.append( tvm );

   int i = 0;

   foreach (TabView *tv, tvs) {
      tv->addActions( tblActs );
      tv->setAccessibleName( accNam[i] );
      tv->setObjectName( accNam[i] );

      tv->setToolTip( tr("Tooltip: ") + accNam[i++]);
      tv->setContextMenuPolicy(Qt::ActionsContextMenu);
      tv->m_gb->installEventFilter( this );
      tv->m_gb->setObjectName("gb");
      //      tv->m_gb->setStyleSheet( Globals::gbStyleShtCenterPROPERTYS );
   }
}
void Browser::requeryWorktimeTableView(QString nonDefaulQuery) {
   QSETTINGS_QUERYS;
   /** Get pointer to current active database connection */
   QSqlDatabase pdb = connectionWidget->currentDatabase();

   /** Select "tableMain" as target table view widget */
   QTableView *tvResp = tvm->tv();
   QSqlQueryModel *model = new QSqlQueryModel(tvResp);
   QSqlQuery query;

   if (! nonDefaulQuery.isEmpty())
      query = QSqlQuery( nonDefaulQuery, pdb);
   else {
      /**
       * Use a QTextEdit to transform a query saved as html into
       * corresponding plain text query
       */
      QTextEdit *te = new QTextEdit();

      foreach (QString str, configQ.allKeys()) {
         if (str.contains("default_worktime_query")) {
            QString s = configQ.value(str, "").toString();
            te->setHtml( configQ.value("InpFrm/" + s, "").toString() );
         }
      }
      query = QSqlQuery(te->toPlainText(), pdb);
   }

   model->setQuery( query );
   tvResp->setModel( model );

   if (model->lastError().type() != QSqlError::NoError)
      emit statusMessage(model->lastError().text());
   else if (model->query().isSelect())
      emit statusMessage(tr("Query OK."));
   else
      emit statusMessage(tr("Query OK, number of affected rows: %1").arg(
                            model->query().numRowsAffected()));

   tvResp->setVisible( false );
   tvResp->resizeColumnsToContents();
   tvResp->resizeRowsToContents();
   tvResp->setVisible( true );
   updateActions();

}
/*
void Browser::showTable(const QString &sqlTab, TabView *tvc) {
   //
    * Get pointer to current active database connection
    //
   QSqlDatabase pDb = connectionWidget->currentDatabase();

   QSqlTableModel *mod = new CustomMdl(tvc->tv(), pDb);
   mod->setEditStrategy(QSqlTableModel::OnRowChange);
   mod->setTable( pDb.driver()->escapeIdentifier(sqlTab, QSqlDriver::TableName));
   mod->select();

   if (mod->lastError().type() != QSqlError::NoError)
      emit statusMessage(mod->lastError().text());

   tvc->tv()->setModel(mod);
   tvc->tv()->setEditTriggers( QAbstractItemView::DoubleClicked |
                               QAbstractItemView::EditKeyPressed );

   if (! (bool) connect( tvc->tv()->selectionModel(),
                         SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
                         this, SLOT(currentChanged()) ) )
      emit statusMessage(tr("Slot connection returns false"));

   tvc->tv()->setVisible( false );
   tvc->tv()->resizeColumnsToContents();
   tvc->tv()->resizeRowsToContents();
   tvc->tv()->setVisible( true );

   updateActions();
}
*/
void Browser::showRelatTable(const QString &sqlTbl, TabView *tvc) {
   /**
    * Get active database pointer
    */
   QSqlDatabase pDb = connectionWidget->currentDatabase();
   QTableView *tv = tvc->tv();
   QSqlRelationalTableModel *rmod = new CustomRelatMdl(tvc->tv(), pDb);
   rmod->setEditStrategy(QSqlTableModel::OnRowChange);
   rmod->setTable(pDb.driver()->
                  escapeIdentifier(sqlTbl, QSqlDriver::TableName));
   /**
    * Because the target database contains more than one table related on
    * foreign keys, varying process is necessary to set the different
    * relations correctly. Which processing branch must be used depends on
    * the table name passed by sqlTbl
    */

   /** Processing branch for table ... */
   if (sqlTbl.contains("worktime", Qt::CaseInsensitive)) {
      /** Remember the indexes of the columns */
      int colEmp = rmod->fieldIndex("workerID");
      int colPrj = rmod->fieldIndex("prjID");

      /** Set the relations to the other database tables */
      rmod->setRelation(colEmp, QSqlRelation(
                           "worker", "workerID", "Nachname"));
      rmod->setRelation(colPrj, QSqlRelation(
                           "prj", "prjID", "Beschreibung"));

      // Set the localized header captions
      rmod->setHeaderData(rmod->fieldIndex("worktimID"), Qt::Horizontal, tr("ID"));
      rmod->setHeaderData(rmod->fieldIndex("dat"), Qt::Horizontal, tr("Datum"));
      rmod->setHeaderData(colEmp, Qt::Horizontal, tr("Mitarb"));
      rmod->setHeaderData(colPrj, Qt::Horizontal, tr("Beschreibung"));
      rmod->setHeaderData(rmod->fieldIndex("hours"), Qt::Horizontal, tr("Stunden"));
   } else {
      if (sqlTbl.contains("prj", Qt::CaseInsensitive)) {
         /** Remember the indexes of the columns */
         int colClient = rmod->fieldIndex("clientID");

         /** Set the relations to the other database tables */
         rmod->setRelation(colClient, QSqlRelation(
                              "client", "clientID", "Name"));

         // Set the localized header captions
         rmod->setHeaderData(rmod->fieldIndex("prjID"), Qt::Horizontal, tr("ID"));
         rmod->setHeaderData(rmod->fieldIndex("clientID"), Qt::Horizontal, tr("Kunde, Name"));
      }
   }


   // Populate the model
   rmod->select();

   // Set the model and hide the ID column
   //   tv->setModel( rmod );
   //   tv->setItemDelegate( new TableDelegate(tv));
   tv->setColumnHidden(rmod->fieldIndex("ID"), true);
   tv->setSelectionMode(QAbstractItemView::ContiguousSelection);

   tv->setModel(rmod);
   tv->setEditTriggers( QAbstractItemView::DoubleClicked |
                        QAbstractItemView::EditKeyPressed );
   /** !!!!!!!!!!!!!!!!!!!! tvs.first()->tv()->selectionModel() changed to
    * tvs[3]->tv()->selectionModel()
    */
   if (! (bool) connect( tvs.last()->tv()->selectionModel(),
                         SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
                         this, SLOT(currentChanged()) ) )
      emit statusMessage(tr("Slot connection returns false"));

   tv->setVisible( false );
   tv->resizeColumnsToContents();
   tv->resizeRowsToContents();
   tv->setVisible( true );

   updateActions();

}
void Browser::showRelatTable2(const QString &sqlTbl, QTableView *tv) {
   /**
    * Get active database pointer
    */
   QSqlDatabase pdb = connectionWidget->currentDatabase();

   QSqlRelationalTableModel *rmod = new CustomRelatMdl(tvc->tv(), pdb);
   rmod->setEditStrategy(QSqlTableModel::OnManualSubmit);
   rmod->setTable( tvc->tv()->objectName() );

   /** Remember the indexes of the columns */
   int colEmp = rmod->fieldIndex("workerID");
   int colPrj = rmod->fieldIndex("prjID");

   /** Set the relations to the other database tables */
   rmod->setRelation(colEmp, QSqlRelation(
                        "worker", "workerID", "Nachname"));
   rmod->setRelation(colPrj, QSqlRelation(
                        "prj", "prjID", "Beschreibung"));

   // Set the localized header captions
   rmod->setHeaderData(rmod->fieldIndex("worktimID"), Qt::Horizontal, tr("ID"));
   rmod->setHeaderData(rmod->fieldIndex("dat"), Qt::Horizontal, tr("Datum"));
   rmod->setHeaderData(colEmp, Qt::Horizontal, tr("Mitarb"));
   rmod->setHeaderData(colPrj, Qt::Horizontal, tr("PrjBesch"));
   rmod->setHeaderData(rmod->fieldIndex("hours"), Qt::Horizontal, tr("Stunden"));

   // Populate the model
   rmod->select();

   // Set the model and hide the ID column
   //   tv->setModel( rmod );
   //   tv->setItemDelegate( new TableDelegate(tv));
   tv->setColumnHidden(rmod->fieldIndex("ID"), true);
   tv->setSelectionMode(QAbstractItemView::SingleSelection);

   tv->setModel(rmod);
   tv->setEditTriggers( QAbstractItemView::DoubleClicked |
                        QAbstractItemView::EditKeyPressed );
   /** !!!!!!!!!!!!!!!!!!!! tvs.first()->tv()->selectionModel() changed to
    * tvs[3]->tv()->selectionModel()
    */
   if (! (bool) connect( tvs.last()->tv()->selectionModel(),
                         SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
                         this, SLOT(currentChanged()) ) )
      emit statusMessage(tr("Slot connection returns false"));

   tv->setVisible( false );
   tv->resizeColumnsToContents();
   tv->resizeRowsToContents();
   tv->setVisible( true );

   updateActions();

}
void Browser::exec() {
   InpFrm *inpFrm = InpFrm::getObjectPtr();

   QTableView *tvq = tvm->tv();
   QSqlQueryModel *model = new QSqlQueryModel(tvq);

   model->setQuery(QSqlQuery(inpFrm->getQueryText(),
                             connectionWidget->currentDatabase()));
   tvq->setModel(model);

   if (model->lastError().type() != QSqlError::NoError)
      emit statusMessage(model->lastError().text());
   else if (model->query().isSelect())
      emit statusMessage(tr("Query OK."));
   else
      emit statusMessage(tr("Query OK, number of affected rows: %1").arg(
                            model->query().numRowsAffected()));

   tvq->setSortingEnabled( true );
   tvq->setVisible( false );
   tvq->resizeColumnsToContents();
   tvq->resizeRowsToContents();
   tvq->setVisible( true );
   updateActions();
}
/**
 * Relational table model test - View relational WORKS 12-11-2015
 */
void Browser::showRelatTable(const QString &t, QTableView *tv) {
   /**
    * Get active database pointer
    */
   QSqlDatabase currentDb = connectionWidget->currentDatabase();

   /**
    * Instantiate a new custom CustomRelatMdl(..) table view model
    */
   QSqlRelationalTableModel *model = new CustomRelatMdl(tv, currentDb);

   model->setEditStrategy(QSqlRelationalTableModel::OnRowChange);

   model->setTable("worktime");
   model->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);
   model->setRelation(2, QSqlRelation("worker", "workerID", "Nachname"));
   model->setRelation(3, QSqlRelation("prj", "prjID", "Kurzform"));
   model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
   model->setHeaderData(1, Qt::Horizontal, QObject::tr("Datum"));
   model->setHeaderData(2, Qt::Horizontal, QObject::tr("worker Nachname"));
   model->setHeaderData(3, Qt::Horizontal, QObject::tr("prj kurzform"));
   model->setHeaderData(4, Qt::Horizontal, QObject::tr("Stunden"));

   model->select();


   if (model->lastError().type() != QSqlError::NoError)
      emit statusMessage(model->lastError().text());

   tv->setModel(model);
   tv->setEditTriggers(QAbstractItemView::DoubleClicked |
                       QAbstractItemView::EditKeyPressed);

   connect(tv->selectionModel(),
           SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
           this, SLOT(currentChanged()));
   tv->setVisible(false);
   tv->resizeColumnsToContents();
   tv->setVisible(true);
   updateActions();
}
void Browser::showMetaData(const QString &t) {

   QSqlRecord rec = connectionWidget->currentDatabase().record(t);
   QStandardItemModel *model = new QStandardItemModel(tvs.first()->tv());

   model->insertRows(0, rec.count());
   model->insertColumns(0, 7);

   model->setHeaderData(0, Qt::Horizontal, "Fieldname");
   model->setHeaderData(1, Qt::Horizontal, "Type");
   model->setHeaderData(2, Qt::Horizontal, "Length");
   model->setHeaderData(3, Qt::Horizontal, "Precision");
   model->setHeaderData(4, Qt::Horizontal, "Required");
   model->setHeaderData(5, Qt::Horizontal, "AutoValue");
   model->setHeaderData(6, Qt::Horizontal, "DefaultValue");


   for (int i = 0; i < rec.count(); ++i) {
      QSqlField fld = rec.field(i);
      model->setData(model->index(i, 0), fld.name());
      model->setData(model->index(i, 1), fld.typeID() == -1
                     ? QString(QMetaType::typeName(fld.type()))
                     : QString("%1 (%2)").arg(QMetaType::typeName(fld.type())).arg(fld.typeID()));
      model->setData(model->index(i, 2), fld.length());
      model->setData(model->index(i, 3), fld.precision());
      model->setData(model->index(i, 4), fld.requiredStatus() == -1 ? QVariant("?")
                                                                    : QVariant(bool(fld.requiredStatus())));
      model->setData(model->index(i, 5), fld.isAutoValue());
      model->setData(model->index(i, 6), fld.defaultValue());
   }

   tvs.first()->tv()->setModel(model);
   tvs.first()->tv()->setEditTriggers(QAbstractItemView::NoEditTriggers);

   updateActions();
}
void Browser::insertRow(/*QTableView &tv*/) {
   /**
     * Loop through every child of your QSplitter and call hasFocus() for
     * each one so you can know which of them has the focus (keyboard focus)
     */
   //   QObjectList objs(this->spHorzPrim->children());

   for (int i = 0; i < tvs.length(); i++) {
      //      QTableView *childa = qobject_cast<QTableView *>(objs.at(i));

      if ( tvs.at(i)->hasFocus() )
         Q_INFO << "table" << i << "has focus";
   }

   TabView *locTabView = qobject_cast<TabView *>( tvs.first() );
   QSqlTableModel *model =
         qobject_cast<QSqlTableModel *>(locTabView->tv()->model());

   if (!model)
      return;

   QModelIndex insertIndex = locTabView->tv()->currentIndex();
   int row = insertIndex.row() == -1 ? 0 : insertIndex.row();
   model->insertRow(row);
   insertIndex = model->index(row, 0);
   locTabView->tv()->setCurrentIndex(insertIndex);
   locTabView->tv()->edit(insertIndex);

   QWidget *wid = QWidget::focusWidget();
   Q_INFO << wid->accessibleName();

   wid = QApplication::focusWidget(),
         Q_INFO << wid->accessibleName();
}
void Browser::deleteRow(/*QTableView &tv*/) {
   TabView *locTabView = qobject_cast<TabView *>( tvs.first() );
   QSqlTableModel *model =
         qobject_cast<QSqlTableModel *>(locTabView->tv()->model());

   if (!model)
      return;

   QModelIndexList currentSelection =
         locTabView->tv()->selectionModel()->selectedIndexes();

   for (int i = 0; i < currentSelection.count(); ++i) {
      if (currentSelection.at(i).column() != 0)
         continue;

      model->removeRow(currentSelection.at(i).row());
   }

   updateActions();
}
void Browser::updateActions(/*QTableView &tv*/) {
   /**
    * Loop thru all available TableViews
    */
   foreach (TabView *tvl, tvs) {
      //      QSqlRelationalTableModel *tm = qobject_cast
      //            <QSqlRelationalTableModel *>( locTabView->tv()->model() );
      QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(tvl->tv()->model());

      bool enableIns = tm;
      bool enableDel = enableIns && tvl->tv()->currentIndex().isValid();

      insertRowAction->setEnabled( enableIns );
      deleteRowAction->setEnabled( enableDel );

      fieldStrategyAction  ->setEnabled(tm);
      rowStrategyAction    ->setEnabled(tm);
      manualStrategyAction ->setEnabled(tm);
      submitAction         ->setEnabled(tm);
      revertAction         ->setEnabled(tm);
      selectAction         ->setEnabled(tm);

      if (tm) {
         QSqlRelationalTableModel::EditStrategy es = tm->editStrategy();
         fieldStrategyAction->setChecked(
                  es == QSqlRelationalTableModel::OnFieldChange);
         rowStrategyAction->setChecked(
                  es == QSqlRelationalTableModel::OnRowChange);
         manualStrategyAction->setChecked(
                  es == QSqlRelationalTableModel::OnManualSubmit);
      }
   }
}
QFont Browser::selectFont() {
   bool ok;
   QFont font = QFontDialog::getFont(
            &ok, QFont("Helvetica [Cronyx]", 10), this);

   if (ok) {
      Q_INFO << font;
   } else {
      Q_INFO << font;
   }

   return QFont();
}
void Browser::customMenuRequested(QPoint pos) {
   QModelIndex index = this->tvs.first()->tv()->indexAt(pos);

   QMenu *menu = new QMenu(this);
   menu->addAction(new QAction("Action 1", this));
   menu->addAction(new QAction("Action 2", this));
   menu->addAction(new QAction("Action 3", this));
   menu->popup(tvs.first()->tv()->viewport()->mapToGlobal(pos));
}
void Browser::on_fieldStrategyAction_triggered() {
   TabView *locTabView = qobject_cast<TabView *>( tvs.first() );
   QSqlTableModel *tm =
         qobject_cast<QSqlTableModel *>(locTabView->tv()->model());

   if (tm)
      tm->setEditStrategy(QSqlTableModel::OnFieldChange);
}
void Browser::on_rowStrategyAction_triggered() {
   TabView *locTabView = qobject_cast<TabView *>( tvs.first() );
   QSqlTableModel *tm = qobject_cast<QSqlTableModel *>(
            locTabView->tv()->model());

   if (tm)
      tm->setEditStrategy(QSqlTableModel::OnRowChange);

   QWidget *wid = focusWidget();
   Q_INFO << wid->objectName();
}
void Browser::on_manualStrategyAction_triggered() {
   TabView *locTabView = qobject_cast<TabView *>( tvs.first() );
   QSqlTableModel *tm = qobject_cast<QSqlTableModel *>(
            locTabView->tv()->model());

   if (tm)
      tm->setEditStrategy(QSqlTableModel::OnManualSubmit);
}
void Browser::on_submitAction_triggered() {
   TabView *locTabView = qobject_cast<TabView *>( tvs.first() );
   QSqlTableModel *tm = qobject_cast<QSqlTableModel *>(
            locTabView->tv()->model());

   if (tm)
      tm->submitAll();
}
void Browser::on_revertAction_triggered() {
   TabView *locTabView = qobject_cast<TabView *>( tvs.first() );
   QSqlTableModel *tm = qobject_cast<QSqlTableModel *>(
            locTabView->tv()->model());

   if (tm)
      tm->revertAll();
}
void Browser::on_selectAction_triggered() {
   TabView *locTabView = qobject_cast<TabView *>( tvs.first() );
   QSqlTableModel *tm = qobject_cast<QSqlTableModel *>(
            locTabView->tv()->model());

   if (tm)
      tm->select();
}
/**
 * Entered after double clicking a table item in connection widget tree view
 */
void Browser::on_connectionWidget_tableActivated(const QString &sqlTab) {
   /**
    * Determine which TabView should be targeted by the activated tab model.
    * Either select an instance of TabView which hasn't a model loaded yet. To
    *
    * swap sql tables during runtime, a double-mouse-click event is captured
    * and the user-selected tv instance becomes addressed by the event handler.
    */
   foreach (TabView *_tvc, tvs) {
      /**
       * Check for pending active select requests.
       */
      if (_tvc->isActiveSelected()) {
         /**
          * If selected view is TableMain AND the activated connection widget
          * table is "tagesberichte" then select the "relational table model"
          *
          * 18-11-2015  Alles auf relational table model umgebaut
          */
         showRelatTable( sqlTab, _tvc );
         /**
          * Update TabViews group box title to match the prior activated sql
          * table name
          */
         _tvc->m_gb->setTitle( sqlTab );
         return;
      }
      else {
         /**
          * If no table widget is selected actively, use the tvc from tvs which has
          * currently no model loaded
          */
         if (_tvc->tv()->model() == 0x00) {
            showRelatTable(sqlTab, _tvc);
            /**
             * Update TabViews group box title to match the prior activated sql
             * table name
             */
            _tvc->m_gb->setTitle( sqlTab );
            return;
         }
         Q_INFO << tr("No model-less table view widget found!");
      }
   }
}
void Browser::onCyclic() {

   //   currentChanged();
   /* ---------------------------------------------------------------- */
   /*         Get the name of object under the mouse pointer           */
   /* ----------------------------------------------------------------*/
   if (cyclicObjInfo) {
      QWidget *widget = qApp->widgetAt(QCursor::pos());

      if (widget != 0x00) {
         QString widName = widget->objectName();
         Q_INFO << widName << widget->children();
      }
   }
}
void Browser::addConnection() {
   DbConnDlg dialog(this);

   if (dialog.exec() != QDialog::Accepted)
      return;

   if (dialog.useInMemoryDatabase()) {
      QMessageBox::warning(
               this, tr("Add connection"), tr("dialog.useInMemoryDatabase()"));
   } else {
      QSqlError err = addConnection(dialog.driverName(),
                                    dialog.databaseName(),
                                    dialog.hostName(),
                                    dialog.userName(),
                                    dialog.password(),
                                    dialog.port());

      if (err.type() != QSqlError::NoError)
         QMessageBox::warning(
                  this,  tr("Unable to open database"),
                  tr("An error occurred while opening the connection: ") + err.text());
   }
}
int  Browser::setFontAcc(QFont &font) {
   this->tva->tv()->setFont(font);
   return 0;
}
bool Browser::eventFilter(QObject *obj, QEvent *e) {
   QPoint pos;

   if (e->type() == QEvent::MouseButtonPress) {
      Q_INFO << "obj parent name:" << obj->parent()->objectName();

      QMouseEvent *me = static_cast<QMouseEvent *>(e);
      pos = me->pos();

      QWidget *widget = qApp->widgetAt(QCursor::pos());

      if (widget != 0x00) {
         Q_INFO << widget->parentWidget()->objectName();
         Q_INFO << widget->objectName();
      }

      /**
         * If event receiver object has one of the tabview accessnames...
         * ... reset all tabviews from beeing selected and mark the obj
         * obj->parent() after static casting to TabView...
         */
      if ( (((QStringList) TVA << TVB << TVC << TVM).join(','))
           .contains( obj->parent()->objectName()) ) {

         /**
             * Reset....
             */
         TabView *tv = static_cast<TabView *>(obj->parent());

         /**
             * Unselect if view was selected earlier
             */
         if (QVariant(tv->m_gb->property("select")).toBool()) {
            tv->m_gb->setProperty("select", false);
            emit tabViewSelChanged( 0 );
         } else {
            tv->m_gb->setProperty("select", true);
            tv->m_gb->setStyleSheet(tv->m_gb->styleSheet());
            emit tabViewSelChanged(tv);
         }
      }

      if (false) {
         QList<QObject *> objs = obj->children();

         for (int i = 0; i < objs.length(); i++) {
            Q_INFO << "obj child no." << i << objs[i]->objectName();
         }
      }

      return true;
   }

   if ((e->type() == QEvent::KeyPress)
       && false) {
      QKeyEvent *kev = static_cast<QKeyEvent *>(e);
      Q_INFO << "Keypress " << kev->key() << "by obj:" ;
      Q_INFO << "obj name:" << obj->objectName();
      Q_INFO << "obj parent name:" << obj->parent()->objectName();

      QList<QObject *> objs = obj->children();

      for (int i = 0; i < objs.length(); i++) {
         Q_INFO << "obj child no." << i << objs[i]->objectName();
         Q_INFO << "obj child no." << i << objs[i]->isWidgetType();
      }

   }

   /**
     * standard event processing
     */
   return QObject::eventFilter(obj, e);
}
QVector<int> CustomMdl::getCCol() const {
   return cCol;
}
void CustomMdl::setCCol(const QVector<int> &value) {
   cCol = value;
}
void Browser::showEvent(QShowEvent *e) {
   Q_UNUSED(e)
   if (true) {
      QSETTINGS;
      QList<QSplitter *> spls = findChildren<QSplitter *>();


      foreach (QSplitter *sp, spls) {
         QString objn = sp->objectName();
//         Q_INFO << objn;

         sp->restoreState(
                  config.value("Browser/" + objn, " ").toByteArray());
      }
   }
}

/** Testing (relational) editableSqlModel    19-11-2015 */
void Browser::SORTIT() {
   QSETTINGS;

   QString queryStr =
         config.value("InpFrm/default_worktime_query_plain","").toString();

   QSortFilterSqlQueryModel *model = new QSortFilterSqlQueryModel(this);
   InpFrm *inpFrm = InpFrm::getObjectPtr();

   QTableView *tvq = tvm->tv();


   if (QMessageBox::question(this, tr("Use default query?"),
                             tr("Query default worktime ?"),
                             QMessageBox::Yes |
                             QMessageBox::No, QMessageBox::No) ==
       QMessageBox::Yes) {
      model->setQuery(queryStr, connectionWidget->currentDatabase());
      Q_INFO << queryStr;
   }
   else {
      model->setQuery(inpFrm->getQueryText(),
                                connectionWidget->currentDatabase());
      Q_INFO << inpFrm->getQueryText();
   }

   tvq->setModel(model);

   if (model->lastError().type() != QSqlError::NoError)
      emit statusMessage(model->lastError().text());
   else if (model->query().isSelect())
      emit statusMessage(tr("Query OK."));
   else
      emit statusMessage(tr("Query OK, number of affected rows: %1").arg(
                            model->query().numRowsAffected()));
   model->setFilterColumn("wk.Vorname"); // will filter by user name
   model->setFilterFlags(Qt::MatchStartsWith);
   model->setFilter("");
   model->select();

   QFrame *frm = new QFrame();
   QGridLayout *gl = new QGridLayout();

   QLineEdit *oncol = new QLineEdit();
   connect(oncol, SIGNAL (textChanged(QString)), model, SLOT( setFilterColumn(QString)));

   QLineEdit *search = new QLineEdit();
   connect(search, SIGNAL (textChanged(QString)), model, SLOT( filter(QString)));

   QLabel *se = new QLabel("search");
   QLabel *co = new QLabel("FilterCol");

   gl->addWidget(se,0,0);
   gl->addWidget(search,0,1);

   gl->addWidget(co,1,0);
   gl->addWidget(oncol,1,1);
   frm->setLayout(gl);

   oncol->setVisible(true);
   search->setVisible(true);

   frm->show();
}

void Browser::initializeMdl(QSqlQueryModel *model) {
   QSqlDatabase pDb = connectionWidget->currentDatabase();
   model->setQuery("select * from worker", pDb);
   model->setHeaderData(0, Qt::Horizontal, QObject::tr("workerID"));
   model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nachname"));
   model->setHeaderData(2, Qt::Horizontal, QObject::tr("Vorname"));
   Q_INFO << model->lastError().databaseText();
   Q_INFO << model->lastError().driverText();
   Q_INFO << model->lastError();

////   mdl->setQuery("SELECT prjID, clientID, SubID, Beschreibung, Kurzform "
////                 "FROM prj", pDb);
//   mdl->setQuery("SELECT * FROM prj");
//   mdl->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
//   mdl->setHeaderData(1, Qt::Horizontal, QObject::tr("clId"));
//   mdl->setHeaderData(2, Qt::Horizontal, QObject::tr("subId"));
}
QTableView* Browser::createView(QSqlQueryModel *model,
                                const QString &title) {
   QTableView *view = new QTableView(0);
   view->setModel(model);
   static int offset = 0;

   view->setWindowTitle(title);
   view->move(100 + offset, 100 + offset);
   offset += 20;
   view->show();

   return view;
}

/*
void Browser::showTable22(const QString &t, QTableView *tv) {

   QSqlDatabase pdb = connectionWidget->currentDatabase();

   QSqlTableModel *model = new CustomMdl(tv, pdb);
   model->setEditStrategy(QSqlTableModel::OnRowChange);
   model->setTable( pdb.driver()->escapeIdentifier(t, QSqlDriver::TableName));
   model->select();

   if (model->lastError().type() != QSqlError::NoError)
      emit statusMessage(model->lastError().text());

   tv->setModel(model);
   tv->setEditTriggers( QAbstractItemView::DoubleClicked |
                        QAbstractItemView::EditKeyPressed );

   if (! (bool) connect( tvs.first()->tv()->selectionModel(),
                         SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
                         this, SLOT(currentChanged()) ) )
      emit statusMessage(tr("Slot connection returns false"));

   tv->setVisible( false );
   tv->resizeColumnsToContents();
   tv->resizeRowsToContents();
   tv->setVisible( true );

   updateActions();
}*/
