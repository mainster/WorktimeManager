#include "dbcontroller.h"


DbController::DbController(QObject *parent) : QObject(parent) {
	inst = this;
	setObjectName(tr("DbController"));
	QSETTINGS;

	/*!
	 * Query platforms available SQL driver names.
	 */
	QStringList drivers = QSqlDatabase::drivers();
	if (!drivers.contains(Locals::SQL_DRIVER))
		CRIT << tr("empty");

	QSqlDatabase mDb = QSqlDatabase::addDatabase(Locals::SQL_DRIVER);

	/*!
	 * Check if ini contains a custom sql database file path...
	 */
	INFO << objectName() << config.value(objectName() + Md::k.customDbFilePath);

	if (config.allKeys().join(',').contains(Md::k.customDbFilePath)) {
		Locals::SQL_DATABASE.setFile(config.value(Md::k.customDbFilePath).toString());
		INFO << Locals::SQL_DATABASE.filePath();
	}
	else {
		if (! Locals::SQL_DATABASE.dir().entryList().contains(
				 Locals::SQL_DATABASE.fileName(), Qt::CaseSensitive)) {
			QString customPath =
					QFileDialog::getOpenFileName(0, tr("Open sqlite database ..."),
														  Locals::SQL_DATABASE.absoluteDir()
														  .absolutePath());
			Locals::SQL_DATABASE.setFile(customPath);
			config.setValue(objectName() + Md::k.customDbFilePath, customPath);
		}
	}
	mDb.setDatabaseName(Locals::SQL_DATABASE.filePath());

	/*!
	 * If no connection is available, try to open the SQL database
	 */
	if ((! mDb.open()) || QSqlDatabase::connectionNames().isEmpty()) {
		qReturn("(! mDb.open())");
	}

	if (! bool( connect((&mDb)->driver(),	SIGNAL(notification(QString)),
							  this,				SLOT(onDriverMessage(QString)))))
		qReturn("QObject::connect driver notification to error handler slot failed!");

}

QSqlError DbController::addConnection( const QString &driver, const QString &dbName,
													const QString &host, const QString &user,
													const QString &passwd, int port) {
	QSqlError err;
	/*QSqlDatabase db*/

	mDb = QSqlDatabase::addDatabase( (driver.isEmpty()) ? Locals::SQL_DRIVER : driver );
	mDb.setDatabaseName((dbName.isEmpty()) ? Locals::SQL_DATABASE.filePath() : dbName );
	mDb.setHostName(host);
	mDb.setPort(port);

	if (!mDb.open(user, passwd)) {
		err = mDb.lastError();
		mDb = QSqlDatabase();
		INFO << tr("No db access");
		QSqlDatabase::removeDatabase(dbName);
		return err;
	}
	else INFO << tr("db is open");

	INFO << tr("db->tables(QSql::Tables):") << mDb.tables(QSql::Tables);
	INFO << tr("db->tables(QSql::AllTables):") << mDb.tables(QSql::AllTables);

	return err;
}
void DbController::onDriverMessage (const QString& name) {
	WARN << name;
}


bool DbController::addConnectionsByCmdline(QVariant args) {
	QSqlError err;

	foreach (QString s, args.toStringList()) {
		QUrl url(Locals::SQL_DATABASE.baseName(), QUrl::TolerantMode);
		url.setScheme(Locals::SQL_DRIVER);

		if (!url.isValid()) {
			qWarning("Invalid URL: %s", qPrintable(s));
			continue;
		}

		if (QSqlError::NoError != (addConnection(
												url.scheme().toUpper(), url.path(), url.host(),
												url.userName(), url.password(), url.port(-1))).type()) {

			url = QUrl(Locals::SQL_DATABASE.filePath(), QUrl::TolerantMode);
			url.setScheme("QMYSQL");

			QSqlError sqlErr = addConnection(
						url.scheme().toUpper(), url.path(), url.host(),
						url.userName(), url.password(), url.port(-1));
			if (sqlErr.type() != QSqlError::NoError) {
				bReturn(tr("Unable to open connection: %s")
						  .arg(sqlErr.text()));
			}
			return true;
		}
		return true;
	}
	return false;
}

QList<bool> DbController::getConnectionState(bool beQuiet) {
	QList<bool> summary;

	summary << mDb.isDriverAvailable(Locals::SQL_DRIVER)
			  << mDb.isOpen() << mDb.isOpenError() << mDb.isValid();

	if (! beQuiet)		INFO << summary << mDb.connectionNames();

	return summary;
}
