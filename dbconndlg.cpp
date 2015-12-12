#include "dbconndlg.h"
#include "ui_dbconndlg.h"
#include "globals.h"

#include <QSqlDatabase>
#include <QFileDialog>

DbConnDlg::DbConnDlg(QWidget *parent)
   : QDialog(parent) {
   ui.setupUi(this);

   QStringList drivers = QSqlDatabase::drivers();

   // remove compat names
   drivers.removeAll("QMYSQL3");
   drivers.removeAll("QOCI8");
   drivers.removeAll("QODBC3");
   drivers.removeAll("QPSQL7");
   drivers.removeAll("QTDS7");

   if (!drivers.contains("QSQLITE"))
      ui.dbCheckBox->setEnabled(false);

   ui.comboDriver->addItems(drivers);

   QString dbNam = QFileDialog::
         getOpenFileName(this, tr("Open SQL database file"), Globals::SQL_DB_PATH,
                         tr("database files (*.db);;all files (*)"));

   ui.editDatabase->setText(dbNam);
}

DbConnDlg::~DbConnDlg() {
}
QString DbConnDlg::driverName() const {
   return ui.comboDriver->currentText();
}
QString DbConnDlg::databaseName() const {
   return ui.editDatabase->text();
}
QString DbConnDlg::userName() const {
   return ui.editUsername->text();
}
QString DbConnDlg::password() const {
   return ui.editPassword->text();
}
QString DbConnDlg::hostName() const {
   return ui.editHostname->text();
}
int DbConnDlg::port() const {
   return ui.portSpinBox->value();
}
bool DbConnDlg::useInMemoryDatabase() const {
   return ui.dbCheckBox->isChecked();
}
void DbConnDlg::on_okButton_clicked() {
   if (ui.comboDriver->currentText().isEmpty()) {
      QMessageBox::information(this, tr("No database driver selected"),
                               tr("Please select a database driver"));
      ui.comboDriver->setFocus();
   } else {
      accept();
   }
}
