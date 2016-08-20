#include <QApplication>
#include <QtCore>
#include <QtWidgets>
#include <QtSql>
#include <QDebug>
#include <QObject>

#include "mainwindow.h"
#include "textedit.h"
//#include "browser.h"
//#include "mdeventfilters.h"


int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QSETTINGS_INIT;
	qDebug() << a.arguments();

	a.installEventFilter(new GlobalEventFilter(/*true*/));
	MainWindow w;
	w.show();

	return a.exec();
}
