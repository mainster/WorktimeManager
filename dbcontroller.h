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

	QSqlDatabase db() const { return mDb; }
	QList<bool> getConnectionState(bool beQuiet = false);

signals:

public slots:
	QSqlError addConnection(const QString &driver = "", const QString &dbName = "",
									const QString &host = "", const QString &user = "",
									const QString &passwd = "", int port = -1);
	bool addConnectionsByCmdline(QVariant args);

protected slots:
	void onDriverMessage(const QString &name);

private:
	DbController *inst;
	QSqlDatabase mDb;
};
#endif // DBCONTROLLER_H
