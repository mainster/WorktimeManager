#include <QApplication>
#include <QtCore>
#include <QtWidgets>
#include <QtSql>
#include <QDebug>
#include <QObject>

#include "mainwindow.h"
#include "browser.h"
// #include "mdcombobox.h"
#include "mdglobaleventfilter.h"


int main(int argc, char *argv[]) {
   QApplication a(argc, argv);

   qDebug() << a.arguments();

	a.installEventFilter(new GlobalEventFilter(/*true*/));
   MainWindow w;
   w.show();

   return a.exec();
}
