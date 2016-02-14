#include "inpfrm.h"
#include "ui_inpfrm.h"
#include "tablewindow.h"
#include "tabledelegate.h"
#include "initdb.h"
#include "browser.h"
//#include "QSortFilterSqlQueryModel.h"

#include <QtSql>
#include <QMessageBox>
#include <QDockWidget>
#include <QDataWidgetMapper>
#include <QInputDialog>

#define QUERYPREFIX  tr("CUSTOM_QUERY_")
#define NEWQUERY     tr("NewQuery")

InpFrm* InpFrm::inst = 0;


InpFrm::InpFrm(QWidget *parent) : QDockWidget(parent),
   ui(new Ui::InpFrm) {
   ui->setupUi(this);
   QSETTINGS;

   browser = Browser::getObjectPtr();
   connectActions();

   ui->cbQueryIdent->setDuplicatesEnabled( false );
   ui->datePicker->setDate(QDate::currentDate());

   if (!QSqlDatabase::drivers().contains("QSQLITE"))
      QMessageBox::critical(this, "Unable to load database", "This demo needs the SQLITE driver");

   ui->gbSqlQuery->hide();
   this->setFixedHeight(125);

   QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
   db.setDatabaseName(Globals::SQL_DB_PATH + Globals::SQL_DB_FILE);

   /** Request all combobox widgets within the inputFrm */
   cbs = findChildren<QComboBox *>();
   /** Remove all comboboxes which are not based on sql list models */
   cbs.removeOne(ui->cbQueryIdent);

   /** Request all combobox widgets within the inputFrm */
   QList<QComboBox *> cbsTabable = findChildren<QComboBox *>();
   QList<QDateEdit *> desTabable = findChildren<QDateEdit *>();

   foreach (QComboBox *cb, cbsTabable)
      objTabAble.append( static_cast<QObject*>(cb) );
   foreach (QDateEdit *de, desTabable)
      objTabAble.append( static_cast<QObject*>(de) );
   foreach (QObject *o, objTabAble) {
      if ((! o->objectName().contains("cbPrj")) &&
          (! o->objectName().contains("cbClient")) &&
          (! o->objectName().contains("cbWorker")) &&
          (! o->objectName().contains("datePicker")))
         objTabAble.removeOne(o);
   }
   Q_INFO  << "";
   Q_INFO << tr("objTabable:") << Globals::objToStr(objTabAble);

   /**
    * Install event filter to capture focus changed event for
    * all input form combo boxes
    */
   foreach (QComboBox *cbx, cbsTabable)
      cbx->lineEdit()->installEventFilter(this);
   foreach (QComboBox *cbx, cbs)
      cbx->lineEdit()->installEventFilter(this);

   ui->datePicker->installEventFilter( this );
   //   this->installEventFilter( this );

   /** Check for valied tab order configuration data */
   QStringList lst;
   if (config.allKeys().contains(this->objectName() + tr("/TabOrder"))) {
      Q_INFO << tr("Valied tab order configuration data found!");
      lst = config.value(this->objectName() + tr("/TabOrder"), "").toStringList();

      foreach (QString s, lst) {
         objTabOrder.append( findChild<QComboBox*>(s) );
      }
      Q_INFO << lst;

      QList<QWidget *> widTabOrder = objLstToWidLst( objTabOrder );
      for (int i=0; i < widTabOrder.length()-1; i++) {
         widTabOrder[i]->setTabOrder(widTabOrder[i], widTabOrder[i+1]);
      }
   }
}
QList<QWidget *> InpFrm::objLstToWidLst(QList<QObject *> lst) {
   QList<QWidget *> lstw;

   foreach (QObject *o, lst) {
      lstw.append( qobject_cast<QWidget *>(o));
   }
   return lstw;
}

InpFrm::~InpFrm() {
   saveInpFrmGeometry();
   this->removeEventFilter(this);

   foreach (QComboBox *cbx, cbs)
      cbx->removeEventFilter(this);

   delete ui;
}
QSqlError InpFrm::addConnection(const QString &driver, const QString &dbName,
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
   //   connectionWidget->refresh();

   return err;
}

void InpFrm::initComboboxes() {
   QList<QSqlTableModel *> tms;
   QList<QDataWidgetMapper *> dms;

   //   QList<QSortFilterProxyModel *> fpms;

   /** Request all combobox widgets within the inputFrm */
   QList<QComboBox *> cbLst = findChildren<QComboBox *>();
   /** Remove all comboboxes which are not based on sql list models */
   cbLst.removeOne(ui->cbQueryIdent);


   prjmd = new QSqlTableModel();
   clmd = new QSqlTableModel();
   wkmd = new QSqlTableModel();
   prjm = new QDataWidgetMapper();
   clm = new QDataWidgetMapper();
   wkm = new QDataWidgetMapper();

   prjmd->setTable("prj");
   clmd->setTable("client");
   wkmd->setTable("worker");

   prjm->setModel( prjmd );
   clm->setModel( clmd );
   wkm->setModel( wkmd );

   prjm->addMapping(ui->cbPrjNummer,      prjmd->fieldIndex("Nummer"));
   prjm->addMapping(ui->cbPrjKurzform,    prjmd->fieldIndex("Kurzform"));
   clm->addMapping( ui->cbClientNummer,   clmd->fieldIndex("Nummer"));
   clm->addMapping( ui->cbClientKurzform, clmd->fieldIndex("Kurzform"));
   wkm->addMapping( ui->cbWorkerPersonalNr,  wkmd->fieldIndex("PersonalNr"));
   wkm->addMapping( ui->cbWorkerNachname,    wkmd->fieldIndex("Nachname"));
   wkm->addMapping( ui->cbWorkerVorname,     wkmd->fieldIndex("Vorname"));

   prjm->toFirst();
   clm->toFirst();
   wkm->toFirst();

   //   QList<QString> tbRls;
   //   QStringList tLst, kLst;

   //   tLst << "prj" << "client" << "worker";

   //   for (int i=0; i<3; i++) {
   //      tms.append( new QSqlTableModel() );
   //      tms.last()->setTable(tLst.at(i));
   //      dms.append( new QDataWidgetMapper() );
   //      dms.last()->setModel( tms.last() );
   //   }
   ////   int k=0;

   ////   dms[0]->addMapping(ui->cbPrjNummer, tms[0]->fieldIndex("Nummer"));
   ////   dms[0]->addMapping(ui->cbPrjNummer, tms[0]->fieldIndex("Nummer"));

   ////   while (tbRls[k++].contains("prj",Qt::CaseInsensitive)) {
   ////   }
   ////   while (tbRls[k++].contains("client",Qt::CaseInsensitive)) {
   ////      dms[1]->addMapping();
   ////   }
   ////   while (tbRls[k++].contains("worker",Qt::CaseInsensitive)) {
   ////      dms[2]->addMapping();
   ////   }

   //   foreach (QComboBox *cbx, cbLst) {
   //      kLst << cbx->objectName();
   //      if (kLst.last().contains("prj",Qt::CaseInsensitive)) {
   //         tbRls.append( tr("prj,%1").arg(kLst.last().remove("cbPrj")));
   //         continue;
   //      }
   //      if (kLst.last().contains("client",Qt::CaseInsensitive)) {
   //         tbRls.append( tr("client,%1").arg(kLst.last().remove("cbClient")));
   //         continue;
   //      }
   //      if (kLst.last().contains("worker",Qt::CaseInsensitive)) {
   //         tbRls.append( tr("worker,%1").arg(kLst.last().remove("cbWorker")));
   //         continue;
   //      }
   //   }

   //   int i=0, n0=0, n1=0, n2=0;
   //   foreach (QString s, tbRls) {
   //      if (s.contains("prj")) {
   //         dms[0]->addMapping(cbLst[i], tms[0]->fieldIndex(kLst[i]));
   //         i++;
   ////         Q_INFO << tms[i-1] << kLst[i-1];
   //         continue;
   //      }

   //      if (s.contains("client")) {
   //         dms[1]->addMapping(cbLst[i], tms[1]->fieldIndex(kLst[i]));
   //         i++;
   ////         Q_INFO << tms[i-1] << kLst[i-1];
   //         continue;
   //      }

   //      if (s.contains("worker")) {
   //         dms[2]->addMapping(cbLst[i], tms[2]->fieldIndex(kLst[i]));
   //         i++;
   ////         Q_INFO << tms[i-1] << kLst[i-1];
   //         continue;
   //      }

   //   }

   //   for (int k=0; k<3; k++)
   //      dms[k]->toFirst();









   //   dms[k]->addMapping( );
   //   foreach (QComboBox *cbx, cbLst) {
   //      //      Q_INFO << cbx->objectName() << cbx->accessibleName();

   //      tms.append( new QSqlTableModel(this) );
   //      tms.last()->setTable( tbRls.at(k).split(",").first() );
   //      tms.last()->setEditStrategy(QSqlTableModel::OnManualSubmit);
   //      tms.last()->select();

   //      fpms.append( new QSortFilterProxyModel() );
   //      fpms.last()->setSourceModel( tms.last() );
   //      fpms.last()->setFilterKeyColumn(
   //               tms.last()->fieldIndex( tbRls.at(k).split(",").last()) );
   //      cbLst[k]->setModel( fpms.last() );
   //      cbLst[k++]->setModelColumn( fpms.last()->filterKeyColumn() );
   //   }
}
QString InpFrm::getQueryText() const {
   return ui->teSqlQuerys->toPlainText();
}
void InpFrm::connectActions() {
    connect(ui->datePicker, SIGNAL(dateChanged(QDate)), this, SLOT(onInpFormChanges(QDate)));
    connect(ui->sbID, SIGNAL(valueChanged(int)), this, SLOT(onInpFormChanges(int)));
    connect(ui->cbClientKurzform, SIGNAL(currentIndexChanged(int)), this, SLOT(onInpFormChanges(int)));
    connect(ui->cbClientNummer, SIGNAL(currentIndexChanged(int)), this, SLOT(onInpFormChanges(int)));
    connect(ui->cbPrjKurzform, SIGNAL(currentIndexChanged(int)), this, SLOT(onInpFormChanges(int)));
    connect(ui->cbPrjNummer, SIGNAL(currentIndexChanged(int)), this, SLOT(onInpFormChanges(int)));
    connect(ui->cbWorkerNachname, SIGNAL(currentIndexChanged(int)), this, SLOT(onInpFormChanges(int)));
    connect(ui->cbWorkerVorname, SIGNAL(currentIndexChanged(int)), this, SLOT(onInpFormChanges(int)));
    connect(ui->cbWorkerPersonalNr, SIGNAL(currentIndexChanged(int)), this, SLOT(onInpFormChanges(int)));
    connect(ui->btnSubmitQuery, SIGNAL(clicked()), browser, SLOT(exec()));
    connect(ui->btnSaveQuery, SIGNAL(clicked()), this, SLOT(saveSqlQueryInputText()));
    connect(ui->btnRestoreQuery, SIGNAL(clicked()), this, SLOT(restoreSqlQueryInputText()));
    connect(ui->btnOk, SIGNAL(clicked()), this, SLOT(onBtnOkClicked()));
    connect(ui->btnUndo, SIGNAL(clicked()), this, SLOT(onBtnUndoClicked()));
    connect(ui->btnRedo, SIGNAL(clicked()), this, SLOT(onBtnRedoClicked()));
    connect(ui->btnClear, SIGNAL(clicked()), this, SLOT(onBtnClearClicked()));
    connect(ui->teSqlQuerys, SIGNAL(textChanged()), this, SLOT(onSqlQuerysTextChanged()));
    connect(ui->cbQueryIdent, SIGNAL(currentIndexChanged(int)), this, SLOT(onCbQueryIdentIndexChaned(int)));
    connect(this, SIGNAL(changeInpFrmTabOrder(InpFrm::states)), this, SLOT(onChangeTabOrderSlot(InpFrm::states)));
}
void InpFrm::onChangeTabOrderSlot(InpFrm::states state) {
   /** Interrupt a running changeTabOrder process */
   if ((changeTabOrder == state_running_changeTabOrder ||
        changeTabOrder == state_rejected_changeTabOrder) &&
       state == state_init_changeTabOrder) {
      emit browser->stateMsg(tr("Change tab order process interrupted."
                                "No changes in current tab order."));
      changeTabOrder = state_none;
      return;
   }

   /** If changeTabOrder process is not active && state == init, ... */
   if (! (changeTabOrder == state_running_changeTabOrder) &&
       state == state_init_changeTabOrder) {
      objTabOrder.clear();
      changeTabOrder = state_running_changeTabOrder;
      emit browser->stateMsg(
               tr("Init process to change the input form tab order..."));
      return;
   }

   /** --> Succesfull change of tab order objects
    * If this slot is called with parameter state_done_changeTabOrder &&
    * current state is state_running_changeTabOrder
    * --> Succesfull change of tab order objects, run all setTabOrder(..) now
    */
   if (changeTabOrder == state_running_changeTabOrder &&
       state == state_done_changeTabOrder) {

      QList<QWidget *> widTabOrder = objLstToWidLst( objTabOrder );
      QStringList lst;
      for (int i=0; i < widTabOrder.length()-1; i++) {
         widTabOrder[i]->setTabOrder(widTabOrder[i], widTabOrder[i+1]);
         lst.append(widTabOrder[i]->objectName());
      }

      Q_INFO << lst;
      changeTabOrder = state_done_changeTabOrder;

      QSETTINGS;
      config.setValue(this->objectName() + "/TabOrder", lst.join(","));
      emit browser->stateMsg(
               tr("Input form tab order successfully changed!"));
      return;
   }

   return;

}
void InpFrm::onBtnOkClicked() {
   QSqlQuery query;
   QString q;

   /** Query actual count of records in worktime table */
   query.exec("SELECT worktimID FROM worktime");

   if (query.lastError().type() != QSqlError::NoError) {
      QMessageBox::warning(this, "Fehler bei SQL Query",
                           "query.lastError().type() != QSqlError::NoError");
      return;
   }

   /** Select actual worktimIDs
    * ------------------------- */
   int worktimID_max = 0;
   QVector<int> ids;

   while (query.next()) {
      if (query.value(0).toInt() > worktimID_max)
         worktimID_max = query.value(0).toInt();

      ids.append( query.value(0).toInt() );
   }

   /** Find the next higher, unused worktimID */
   while ( ids.contains(++worktimID_max));

   /** Find workerID based on InpFrm ui selections
    * --------------------------------------------- */
   q = QString("SELECT workerID FROM worker WHERE Nachname = \"%1\" "
               "AND PersonalNr = %2")
         .arg( ui->cbWorkerNachname->currentText() )
         .arg( ui->cbWorkerPersonalNr->currentText().toInt() );
   query.exec( q );

   int workerID = -1;
   while (query.next()) {
      /**
       * If workerID != -1, this is the second loop entry but workerID
       * must be unique! --> Error
       */
      if (workerID > 0) {
         QMessageBox::warning(this, "Fehler bei SQL Query",
                              "Non-unique workerID detected!");
         return;
      }
      else {
         workerID = query.value(0).toInt();
      }
   }

   /** Find prjID based on InpFrm ui selections
    * --------------------------------------------- */
   q = QString("SELECT prjID FROM prj WHERE Kurzform = \"%1\" "
               "AND Nummer = %2")
         .arg( ui->cbPrjKurzform->currentText() )
         .arg( ui->cbPrjNummer->currentText().toInt() );
   query.exec( q );

   int prjID = -1;
   while (query.next()) {
      /**
       * If prjID != -1, this is the second loop entry but prjID
       * must be unique! --> Error
       */
      if (prjID > 0) {
         QMessageBox::warning(this, "Fehler bei SQL Query",
                              "Non-unique prjID detected!");
         return;
      }
      else {
         prjID = query.value(0).toInt();
      }
   }


   /** Prepare query for new record */
   query.prepare("INSERT INTO worktime (worktimID, dat, workerID, prjID, hours) "
                 "VALUES (:worktimID, :dat, :workerID, :prjID, :hours)");

   query.bindValue(":worktimID", worktimID_max);
   query.bindValue(":dat", ui->datePicker->date().toString(Qt::SystemLocaleShortDate));
   query.bindValue(":workerID", workerID);
   query.bindValue(":prjID", prjID);
   query.bindValue(":hours", ui->leHrs->text().toInt());

   query.exec();
   browser->requeryWorktimeTableView();

   Q_INFO.noquote() << query.lastQuery();

   if (query.lastError().type() != QSqlError::NoError) {
      QMessageBox::warning(this, "Fehler bei SQL Query",
                           QString("Error type:" + query.lastError().driverText() +
                                   "\nin final add-record query"));
      return;
   }
}
void InpFrm::onBtnUndoClicked() {

}
void InpFrm::onBtnRedoClicked() {
    QProcessEnvironment env;
    Q_INFO  << env.value(tr("PLATFORM"), "env PLATFORM not found");
    Q_INFO  << env.value(tr("APP_NAME"), "env APP_NAME not found");
    Q_INFO  << env.value(tr("APP_VERSION"), "env APP_VERSION not found");
}
void InpFrm::onBtnClearClicked() {

}
void InpFrm::refreshCbDropDownLists() {
   QList<QSqlTableModel *> tms;
   QList<QSortFilterProxyModel *> fpms;

   QList<QString> tbRls;
   QStringList oLst;

   foreach (QComboBox *cbx, cbs) {
      oLst << cbx->objectName();
      if (oLst.last().contains("client",Qt::CaseInsensitive)) {
         tbRls.append( tr("client,%1").arg(oLst.last().remove("cbClient")));
         continue;
      }
      if (oLst.last().contains("prj",Qt::CaseInsensitive)) {
         tbRls.append( tr("prj,%1").arg(oLst.last().remove("cbPrj")));
         continue;
      }
      if (oLst.last().contains("worker",Qt::CaseInsensitive)) {
         tbRls.append( tr("worker,%1").arg(oLst.last().remove("cbWorker")));
         continue;
      }
   }

   int k=0;
   foreach (QComboBox *cbx, cbs) {
      //      Q_INFO << cbx->objectName() << cbx->accessibleName();

      tms.append( new QSqlTableModel(this) );
      tms.last()->setTable( tbRls.at(k).split(",").first() );
      tms.last()->setEditStrategy(QSqlTableModel::OnManualSubmit);
      tms.last()->select();

      fpms.append( new QSortFilterProxyModel() );
      fpms.last()->setSourceModel( tms.last() );
      fpms.last()->setFilterKeyColumn(
               tms.last()->fieldIndex( tbRls.at(k).split(",").last()) );
      cbs[k]->setModel( fpms.last() );
      cbs[k++]->setModelColumn( fpms.last()->filterKeyColumn() );
   }

   /** Map comboBox indexes */
   connect(ui->cbClientKurzform, SIGNAL(currentIndexChanged(int)),
           ui->cbClientNummer,   SLOT(setCurrentIndex(int)));
   connect(ui->cbClientNummer,   SIGNAL(currentIndexChanged(int)),
           ui->cbClientKurzform, SLOT(setCurrentIndex(int)));

   connect(ui->cbPrjNummer,      SIGNAL(currentIndexChanged(int)),
           ui->cbPrjKurzform,    SLOT(setCurrentIndex(int)));
   connect(ui->cbPrjKurzform,    SIGNAL(currentIndexChanged(int)),
           ui->cbPrjNummer,      SLOT(setCurrentIndex(int)));

   connect(ui->cbWorkerPersonalNr,  SIGNAL(currentIndexChanged(int)),
           ui->cbWorkerNachname,    SLOT(setCurrentIndex(int)));
   connect(ui->cbWorkerPersonalNr,  SIGNAL(currentIndexChanged(int)),
           ui->cbWorkerVorname,     SLOT(setCurrentIndex(int)));
   connect(ui->cbWorkerNachname,    SIGNAL(currentIndexChanged(int)),
           ui->cbWorkerPersonalNr,  SLOT(setCurrentIndex(int)));
   connect(ui->cbWorkerNachname,    SIGNAL(currentIndexChanged(int)),
           ui->cbWorkerVorname,     SLOT(setCurrentIndex(int)));
   connect(ui->cbWorkerVorname,     SIGNAL(currentIndexChanged(int)),
           ui->cbWorkerPersonalNr,  SLOT(setCurrentIndex(int)));
   connect(ui->cbWorkerVorname,     SIGNAL(currentIndexChanged(int)),
           ui->cbWorkerNachname,    SLOT(setCurrentIndex(int)));

   /**
    * Set CUSTOM_QUERY_COMMANDS combo box items
    */
   QSETTINGS_QUERYS;
   QStringList query_keys;

   //   /** Remove all items except "NewQuery" item */
   //   for (int i=0; i < ui->cbQueryIdent->count(); i++) {
   //      if (! ui->cbQueryIdent->itemText(i).contains(NEWQUERY))
   //         ui->cbQueryIdent->removeItem(i);
   //   }
   ui->cbQueryIdent->clear();

   foreach (QString str, configQ.allKeys()) {
      if (true /*str.contains( QUERYPREFIX )*/)
         query_keys << str.split('/').at(1);

      Q_INFO << "Query key:" << query_keys.last();
      ui->cbQueryIdent->addItem(query_keys.last(),
                                configQ.value(str).toString());
   }

   ui->cbQueryIdent->setDuplicatesEnabled( false );

}
void InpFrm::onInpFormChanges(int idx) {
   Q_UNUSED(idx);
   //   Q_INFO << idx;
}
void InpFrm::onInpFormChanges(QDate date) {
   Q_UNUSED(date);
   //   Q_INFO << date;
}
void InpFrm::showError(const QSqlError &err) {
   QMessageBox::critical(this, "Unable to initialize Database",
                         "Error initializing database: " + err.text());
}
void InpFrm::on_submitButton_clicked() {
   ui->gbSqlQuery->setVisible( true );

   //   exec();

   ui->teSqlQuerys->setFocus();;
   ui->gbSqlQuery->setVisible( false );
}
void InpFrm::on_clearButton_clicked() {
   ui->teSqlQuerys->clear();
   ui->teSqlQuerys->setFocus();
}
bool InpFrm::gbSqlQueryIsVisible() const {
   return ui->gbSqlQuery->isVisible();
}
void InpFrm::gbSqlQuerySetVisible(bool vis) {
   ui->gbSqlQuery->setVisible( vis );

   if (vis) {
      setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Expanding));
      this->setMaximumHeight(300);
   }
   else {
      this->setFixedHeight(125);
   }
}
void InpFrm::onCbQueryIdentIndexChaned(int idx) {
   Q_UNUSED(idx);
   ui->teSqlQuerys->setHtml( ui->cbQueryIdent->currentData().toString() );
}
void InpFrm::onSqlQuerysTextChanged() {
   /**
    * Check if teSqlQuery hasFocus to determine if the textCHanged signal
    * was emitted in consequence of a Query restore (no new cb item) or
    * in cons. of an user text input/change
    */
   if (! ui->teSqlQuerys->hasFocus())
      return;

   /**
    * generate new item only if there is no "New query" item at 0.
    * If there is already a "New query" item, update item data only.
    */
   if (! ui->cbQueryIdent->itemText(0).contains(NEWQUERY))
      ui->cbQueryIdent->insertItem(0, NEWQUERY,
                                   ui->teSqlQuerys->toHtml());
   else
      ui->cbQueryIdent->setItemData(0, ui->teSqlQuerys->toHtml());

   ui->cbQueryIdent->setCurrentIndex(0);
}
/**
 * Different SQL query commands could be saved in config file. A unique query
 * command identifier string could be set. All custom query cmds becomes
 * prefixed by CUSTOM_QUERY_.
 */
void InpFrm::saveSqlQueryInputText() {
   QSETTINGS_QUERYS;
   QSETTINGS;
   bool ok; int k = 0;

   QStringList query_keys;
   QString newID;

   foreach (QString str, configQ.allKeys())
      query_keys << str;

   do {
      newID = QUERYPREFIX + QString::number(++k);
   } while( query_keys.contains( newID ));

   newID = QInputDialog::getText(
            this, tr("Save query command"),
            tr("Stored query command identifiers:\n\n")
            + query_keys.join('\n')
            + tr("\n\nEnter query command identifier:"),
            QLineEdit::Normal, newID, &ok);

   if (! (ok && !newID.isEmpty())) {
      QMessageBox::warning(this, tr("Error"), tr("Nothing changed"));
      return;
   }

   /** Save contens of query textbox to config "customQuerys" */
   configQ.setValue(objectName() + "/" + newID,
                    ui->teSqlQuerys->toHtml());

   config.setValue(objectName() + "/" + newID + "_plain",
                   ui->teSqlQuerys->toPlainText());

   if (ui->cbQueryIdent->itemText(0).contains(NEWQUERY))
      ui->cbQueryIdent->removeItem(0);

   refreshCbDropDownLists();
   ui->cbQueryIdent->setCurrentText(newID /*+ "_HTML"*/);

   /**
    * If newID contains a substring indicating a WORKTIME table join query has
    * to be saved, ask the user if this should be the new default query for
    * worktime table and update the default query identifier string in cfg file
    */
   if (newID.contains("worktim") &&
       QMessageBox::question(this, tr("New default query identifier?"),
                             tr("Shold the identifier \"") + newID +
                             tr("\" be registered as new DEFAULT query identifier"
                                " for worktime table querys?"),
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {

      configQ.setValue(objectName() + "/default_worktime_query", newID);

      config.setValue(objectName() + "/default_worktime_query_plain",
                      ui->teSqlQuerys->toPlainText());
   }

}
void InpFrm::restoreSqlQueryInputText() {
   QSETTINGS_QUERYS;

   QStringList query_keys;

   foreach (QString str, configQ.allKeys()) {
      if (str.contains( QUERYPREFIX ))
         query_keys << str;
   }

   ui->teSqlQuerys->setHtml(
            configQ.value(objectName() + "/SqlQueryText", "").toString());
}
void InpFrm::showEventDelay() {
   //   if (f_gbSqlQueryIsVisible)
   //      this->sizeHint();
   //   else
   //      this->setFixedHeight(125);

}
void InpFrm::showEvent(QShowEvent *) {
   //   QTimer::singleShot(25, Qt::CoarseTimer, this, SLOT(showEventDelay()));
   refreshCbDropDownLists();
}
bool InpFrm::eventFilter(QObject *obj, QEvent *ev) {
   if (! ev)
      return true;

   /** Event type KeyPress */
   if (ev->type() == QEvent::KeyPress) {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(ev);

      /**
       * If key press event for "Enter" is detected, emit a btnOk Clicked()
       * signal to access onBtnOkClicked()
       */
      if (keyEvent->key() == Qt::Key_Enter)
         emit ui->btnOk->clicked();

      if (keyEvent->key() == Qt::Key_Escape)
         ui->cbPrjKurzform->lineEdit()->selectAll();
      return QObject::eventFilter(obj, ev);
   }

   /** Event type mouse button...*/
   if (ev->type() == QEvent::MouseButtonPress) {
      QMouseEvent *mev = static_cast<QMouseEvent *>(ev);


      if(mev->button() == Qt::LeftButton) {
         /** Wenn der cast auf ein QLineEdit Object geführt hat, muss parentWidget
       * als wid gesetzt werden um mit wid->objectName() dann cbPrjNummer ...
       * zurück zu bekommen
       */
         QObject *objFoc;
         if (obj == ui->datePicker)
            objFoc = obj;
         else
            objFoc = qobject_cast<QObject *>( obj );

         if (objFoc != 0x00) {
            if ((changeTabOrder == state_running_changeTabOrder &&
                 objTabOrder.length() < objTabAble.length())) {
               if (objTabAble.contains(static_cast<QWidget *>(objFoc)) &&
                   (! objTabOrder.contains(static_cast<QWidget *>(objFoc))) ) {
                  objTabOrder.append(static_cast<QWidget *>(objFoc));
                  emit browser->stateMsg(tr("Tab order: ")
                                         + Globals::objToStr(objTabOrder, ", "));
               }
               else {
                  emit browser->stateMsg(
                           tr("Tab order: ") + Globals::objToStr(objTabOrder,", ")
                           + tr(",") + objFoc->objectName() + tr("rejected!"));
               }
            }
            return true;
         }
      }
   }
   /** standard event processing */
   return QObject::eventFilter(obj, ev);


      /**
       * Used to select the whole text by single click into editable combobox
       * or to detect combobox object order if changeTabOrderInProgress is true
       */
      //      if(mev->button() == Qt::LeftButton) {

      //         if (objFoc != 0x00) {

      //            if ((changeTabOrder == state_running_changeTabOrder &&
      //                 objTabOrder.length() < wsTabable.length())) {
      //               /** If casted cbo is in the "tabable" list but not in order list */
      //               if (wsTabable.contains(static_cast<QWidget *>(objFoc)) &&
      //                   (! objTabOrder.contains(static_cast<QWidget *>(objFoc))) ) {
      //                  objTabOrder.append(static_cast<QWidget *>(objFoc));
      //                  emit browser->stateMsg(tr("Tab order: ")
      //                                         + Globals::widToStr(objTabOrder, ", "));
      //               }
      //               else {
      //                  emit browser->stateMsg(
      //                           tr("Tab order: ") + Globals::widToStr(objTabOrder,", ")
      //                           + tr(",") + objFoc->objectName() + tr("rejected!"));
      //               }
      //            }
      //            if (wsTabable.length() == objTabOrder.length()) {
      //               onChangeTabOrderSlot(state_done_changeTabOrder);
      //            }

      //            return true;
      //         }

      //         if (objFoc != 0x00) {
      //            if ((changeTabOrder == state_running_changeTabOrder &&
      //                 objTabOrder.length() < wsTabable.length())) {
      //               /** If casted cbo is in the "tabable" list but not in order list */
      //               if (wsTabable.contains(objFoc->parentWidget()) && !objTabOrder.contains(objFoc->parentWidget())) {
      //                  objTabOrder.append(objFoc->parentWidget());
      //                  emit browser->stateMsg(tr("Tab order: ")
      //                                         + Globals::widToStr(objTabOrder, ", "));
      //               }
      //               else {
      //                  emit browser->stateMsg(
      //                           tr("Tab order: ") + Globals::widToStr(objTabOrder,", ")
      //                           + tr(",") + objFoc->objectName() + tr("rejected!"));
      //               }
      //            }
      //            if (wsTabable.length() == objTabOrder.length()) {
      //               onChangeTabOrderSlot(state_done_changeTabOrder);
      //            }

      ////            led->selectAll();
      //            return true; //QObject::eventFilter(obj, ev);
      //         }
      //      }
//   }

}
