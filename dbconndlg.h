#ifndef DBCONNDLG_H
#define DBCONNDLG_H

#include <QDialog>
#include <QMessageBox>

#include "ui_dbconndlg.h"

class DbConnDlg: public QDialog
{
    Q_OBJECT
public:
    DbConnDlg(QWidget *parent = 0);
    ~DbConnDlg();

    QString driverName() const;
    QString databaseName() const;
    QString userName() const;
    QString password() const;
    QString hostName() const;
    int port() const;
    bool useInMemoryDatabase() const;

private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked() { reject(); }
    void on_dbCheckBox_clicked() { ui.connGroupBox->setEnabled(!ui.dbCheckBox->isChecked()); }

private:
    Ui::DbConnDlgUi ui;
};

#endif
