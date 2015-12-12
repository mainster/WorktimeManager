#include "mainwindow.h"
#include <QApplication>

#include "browser.h"

#include <QtCore>
#include <QtWidgets>
#include <QtSql>
#include <QDebug>
#include <QObject>


int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   QCoreApplication::setOrganizationDomain("OrgDomain");
   QCoreApplication::setOrganizationName("OrgName");
   QCoreApplication::setApplicationName("AppName");
   QCoreApplication::setApplicationVersion("1.0.0");

   qDebug() << a.arguments();

   MainWindow w;
   w.show();

   return a.exec();
}
