#include "dbcontroller.h"


DbController::DbController(QObject *parent)
	: QObject(parent) {
	QSETTINGS;
	inst = this;
	setObjectName(tr("DbController"));

	Locals *locals = new Locals();
	/*!
	 * Load SQL file path from ini.
	 */
	if ((config.value(objectName() + Md::k.sqlDatabaseFilePath, "")
		  .toString()).isEmpty()) {
		Locals::SQL_DATABASE.setFile(
					QFileDialog::getOpenFileName(0, tr("Open sqlite database ..."),
														  Locals::PROJECT_PATHS.at(0)));
	}
	else
		Locals::SQL_DATABASE.setFile(
					config.value(objectName() + Md::k.sqlDatabaseFilePath, "")
					.toString());

//	if ((config.value(objectName() + Md::k.sqlDatabaseFilePath, "")
//		  .toString()).isEmpty()) {
//		locals->SQL_DATABASE.setFile(
//					QFileDialog::getOpenFileName(0, tr("Open sqlite database ..."),
//														  locals->PROJECT_PATHS.at(0)));
//	}
//	else
//		locals->SQL_DATABASE.setFile(
//					config.value(objectName() + Md::k.sqlDatabaseFilePath, "")
//					.toString());


	/*!
	 * If no sql path key stored in ini...
	 */
//	if (Locals::SQL_DATABASE.path().isEmpty()) {
//		Locals::SQL_DATABASE.setFile(
//					QFileDialog::getOpenFileName(0, tr("Open sqlite database ..."),
//														  Locals::PROJECT_PATHS.at(0)));
//	}
	INFO << Locals::SQL_DATABASE.filePath();

	/*!
	 * Validate SQL database path.
	 */
	if (! (Locals::SQL_DATABASE.isFile() &&
			 Locals::SQL_DATABASE.isWritable() &&
			 Locals::SQL_DATABASE.isReadable())) {
		CRIT << tr("Bad sql database file!");
		return;
	}
//	if (! (locals->SQL_DATABASE.isFile() &&
//			 locals->SQL_DATABASE.isWritable() &&
//			 locals->SQL_DATABASE.isReadable())) {
//		CRIT << tr("Bad sql database file!");
//		return;
//	}

	newDatabase();

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
void DbController::newDatabase(QString database) {
	QSETTINGS;
	/*!
	 * Query platforms available SQL driver names.
	 */
	QStringList drivers = QSqlDatabase::drivers();
	if (!drivers.contains(Locals::SQL_DRIVER))
		CRIT << tr("empty");

	if (mDb.isOpen())
		mDb.close();

	mDb = QSqlDatabase::addDatabase(Locals::SQL_DRIVER);

	/*!
	 * Check if ini contains a custom sql database file path...
	 */
	INFO << objectName() << config.value(objectName() + Md::k.sqlDatabaseFilePath);

	if (database.isEmpty())
		mDb.setDatabaseName(Locals::SQL_DATABASE.filePath());
	else
		mDb.setDatabaseName(database);

	/*!
	 * If no connection is available, try to open the SQL database
	 */
	if ((! mDb.open()) || QSqlDatabase::connectionNames().isEmpty())
		qReturn("(! mDb.open())");

	if (! bool( connect((&mDb)->driver(),	SIGNAL(notification(QString)),
							  this,				SLOT(onDriverMessage(QString)))))
		qReturn("QObject::connect driver notification to error handler slot failed!");

	/*!
	 * Store current db path to ini file.
	 */
	config.setValue(objectName() + Md::k.sqlDatabaseFilePath,
						 Locals::SQL_DATABASE.filePath());
	ConnectionWidget::instance()->refresh();
}
void DbController::onNewConnection() {
	QSETTINGS;
	Locals::SQL_DATABASE.setFile(
				QFileDialog::getOpenFileName(0, tr("Open sqlite database ..."),
													  Locals::PROJECT_PATHS.at(0)));
	newDatabase();
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
