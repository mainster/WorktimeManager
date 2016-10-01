#ifndef DBCONTROLLER_H
#define DBCONTROLLER_H

#include <QObject>
#include <QSql>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QFileDialog>

#include "connectionwidget.h"
#include "locals.h"

class DbController : public QObject {

	Q_OBJECT

public:
	explicit DbController(QObject *parent = 0);
	static QSqlDatabase db() { return mDb; }
	QList<bool> getConnectionState(bool beQuiet = false);

signals:

public slots:
	QSqlError addConnection(const QString &driver = "", const QString &dbName = "",
									const QString &host = "", const QString &user = "",
									const QString &passwd = "", int port = -1);
	bool addConnectionsByCmdline(QVariant args);
	void onOpenDatabase();

protected slots:
	void openDatabase(QString database);
	void onDriverMessage(const QString &name);

private:
	DbController *inst;
	static QSqlDatabase mDb;
};
#endif // DBCONTROLLER_H
