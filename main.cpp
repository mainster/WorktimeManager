#include <QApplication>
#include <QThread>
#include <QtCore>
#include <QtWidgets>
#include <QtSql>
#include <QDebug>
#include <QObject>

#include "mainwindow.h"
#include "textedit.h"
#include "mdsplashscreen.h"
//#include "browser.h"
//#include "mdeventfilters.h"

Q_DECLARE_METATYPE(QList<int>)
Q_DECLARE_METATYPE(QList<bool>)

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);

	class SleeperThread : public QThread {

	public:
		static void msleep(unsigned long msecs)
		{ QThread::msleep(msecs); }
	};

	MdSplashScreen *splash = new MdSplashScreen(QPixmap(":/images/splashScreen.png"));
	splash->show();

	qRegisterMetaTypeStreamOperators<QList<int> >("QList<int>");
	qRegisterMetaTypeStreamOperators<QList<bool> >("QList<bool>");
	QSETTINGS_INIT;
	qDebug() << a.arguments();

	a.installEventFilter(new GlobalEventFilter(/*true*/));
	MainWindow w;

	QTimer::singleShot(2500, splash, &QSplashScreen::close);
	QTimer::singleShot(2600, &w, &MainWindow::show);


	w.show();
	return a.exec();
}
