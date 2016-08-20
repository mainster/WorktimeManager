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

Q_DECLARE_METATYPE(QList<int>)
Q_DECLARE_METATYPE(QList<bool>)

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	qRegisterMetaTypeStreamOperators<QList<int> >("QList<int>");
	qRegisterMetaTypeStreamOperators<QList<bool> >("QList<bool>");
	QSETTINGS_INIT;
	qDebug() << a.arguments();

	a.installEventFilter(new GlobalEventFilter(/*true*/));
	MainWindow w;
	w.show();

	return a.exec();
}
