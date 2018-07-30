#ifndef DBCONTROLLER_H
#define DBCONTROLLER_H

#include <QObject>
#include <QSql>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QFileDialog>
#include <QMessageBox>
#include <QAbstractButton>
#include <QPushButton>

#include "connectionwidget.h"
#include "locals.h"

class DbController : public QObject {

	Q_OBJECT

public:
	explicit DbController(QObject *parent = nullptr);
	static QSqlDatabase db() { return mDb; }
	QList<bool> getConnectionState(bool beQuiet = false);

signals:

public slots:
	QSqlError addConnection(const QString &driver = "", const QString &dbName = "",
									const QString &host = "", const QString &user = "",
									const QString &passwd = "", int port = -1);
	bool addConnectionsByCmdline(QVariant args);
	void onOpenDatabase();
	static void dropTable(const QString &tableName);

protected slots:
	bool openDatabase(const QString &database,
							const QString &driver = Locals::SQLITE_DRIVER);
	void onDriverMessage(const QString &name);

private:
	DbController *inst;
	static QSqlDatabase mDb;
};
#endif // DBCONTROLLER_H
