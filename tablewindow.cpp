#include "tablewindow.h"
#include "tabledelegate.h"
//#include "initdb.h"
#include <QtSql>
#include <QMessageBox>
#include <QDataWidgetMapper>
#include "dbconndlg.h"


TableWindow * TableWindow::inst = 0x00;


TableWindow::TableWindow(QWidget *parent) {
   //   : QWidget(parent){
    Q_UNUSED(parent);

   ui.setupUi(this);

   if (!QSqlDatabase::drivers().contains("QSQLITE"))
      QMessageBox::critical(this, "Unable to load database", "This demo needs the SQLITE driver");

   // Init db
   DbConnDlg dialog(this);
   if (dialog.exec() != QDialog::Accepted)
      return;

   if (dialog.useInMemoryDatabase()) {
      QSqlDatabase::database("in_mem_db", false).close();
      QSqlDatabase::removeDatabase("in_mem_db");
      QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "in_mem_db");
      db.setDatabaseName(":memory:");
      if (!db.open())
         QMessageBox::warning(this, tr("Unable to open database"), tr("An error occurred while "
                                                                      "opening the connection: ") + db.lastError().text());
      QSqlQuery q("", db);
      q.exec("drop table Movies");
      q.exec("drop table Names");
      q.exec("create table Movies (id integer primary key, Title varchar, Director varchar, Rating number)");
      q.exec("insert into Movies values (0, 'Metropolis', 'Fritz Lang', '8.4')");
      q.exec("insert into Movies values (1, 'Nosferatu, eine Symphonie des Grauens', 'F.W. Murnau', '8.1')");
      q.exec("insert into Movies values (2, 'Bis ans Ende der Welt', 'Wim Wenders', '6.5')");
      q.exec("insert into Movies values (3, 'Hardware', 'Richard Stanley', '5.2')");
      q.exec("insert into Movies values (4, 'Mitchell', 'Andrew V. McLaglen', '2.1')");
      q.exec("create table Names (id integer primary key, Firstname varchar, Lastname varchar, City varchar)");
      q.exec("insert into Names values (0, 'Sala', 'Palmer', 'Morristown')");
      q.exec("insert into Names values (1, 'Christopher', 'Walker', 'Morristown')");
      q.exec("insert into Names values (2, 'Donald', 'Duck', 'Andeby')");
      q.exec("insert into Names values (3, 'Buck', 'Rogers', 'Paris')");
      q.exec("insert into Names values (4, 'Sherlock', 'Holmes', 'London')");
      //      connectionWidget->refresh();
   }
   else {
      QSqlError err = addConnection(dialog.driverName(),
                                    dialog.databaseName(),
                                    dialog.hostName(),
                                    dialog.userName(),
                                    dialog.password(),
                                    dialog.port());
      if (err.type() != QSqlError::NoError)
         QMessageBox::warning(this, tr("Unable to open database"),
                              tr("An error occurred while "
                                 "opening the connection: ") + err.text());
   }

   //   inpForm = InpFrm::getObjectPtr();

   // Create the data model
   model = new QSqlRelationalTableModel(ui.TimeTable);
   model->setEditStrategy(QSqlTableModel::OnManualSubmit);
   model->setTable("tblTages");

   // Remember the indexes of the columns
   employeeIdx = model->fieldIndex("id_Mitarb");
   projIdx     = model->fieldIndex("ProjektID");

   // Set the relations to the other database tables
   model->setRelation(employeeIdx,
                      QSqlRelation("Mitarbeiter", "id_Mitarb", "Nachname"));
   model->setRelation(projIdx,
                      QSqlRelation("Projekt", "ProjektID", "Beschreibung"));

   // Set the localized header captions
   model->setHeaderData(employeeIdx, Qt::Horizontal, tr("Mitarb"));
   model->setHeaderData(projIdx, Qt::Horizontal, tr("PrjBesch"));
   model->setHeaderData(model->fieldIndex("Datum"), Qt::Horizontal, tr("Datum"));
   model->setHeaderData(model->fieldIndex("Stunden"), Qt::Horizontal, tr("Stunden"));
   model->setHeaderData(model->fieldIndex("id_Fehlzeit"), Qt::Horizontal, tr("Fehlz"));

   // Populate the model
   if (!model->select()) {
      showError(model->lastError());
      return;
   }

   // Set the model and hide the ID column
   ui.TimeTable->setModel(model);
   ui.TimeTable->setItemDelegate(new TableDelegate(ui.TimeTable));
   ui.TimeTable->setColumnHidden(model->fieldIndex("TagesID"), true);
   ui.TimeTable->setSelectionMode(QAbstractItemView::SingleSelection);


   // Initialize the Author combo box
   ui.authorEdit->setModel(model->relationModel(employeeIdx));
   ui.authorEdit->setModelColumn(model->relationModel(employeeIdx)->fieldIndex("name"));

   ui.genreEdit->setModel(model->relationModel(projIdx));
   ui.genreEdit->setModelColumn(model->relationModel(projIdx)->fieldIndex("name"));

   QDataWidgetMapper *mapper = new QDataWidgetMapper(this);
   mapper->setModel(model);
   mapper->setItemDelegate(new TableDelegate(this));
   mapper->addMapping(ui.titleEdit, model->fieldIndex("title"));
   mapper->addMapping(ui.yearEdit, model->fieldIndex("year"));
   mapper->addMapping(ui.authorEdit, employeeIdx);
   mapper->addMapping(ui.genreEdit, projIdx);
   mapper->addMapping(ui.ratingEdit, model->fieldIndex("rating"));

   INFO << tr("connect error....");
   connect(ui.TimeTable->selectionModel(),
           SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
           mapper, SLOT(setCurrentModelIndex(QModelIndex)));

   INFO << tr("... here?");

   ui.TimeTable->setCurrentIndex(model->index(0, 0));
}

void TableWindow::showError(const QSqlError &err) {
   QMessageBox::critical(this, "Unable to initialize Database",
                         "Error initializing database: " + err.text());
}

QSqlError TableWindow::addConnection(const QString &driver, const QString &dbName,
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
