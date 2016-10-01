#include "dbcontroller.h"

QSqlDatabase DbController::mDb = QSqlDatabase();

/* ======================================================================== */
/*                        DbController::DbController                        */
/* ======================================================================== */
DbController::DbController(QObject *parent) : QObject(parent) {
	QSETTINGS;
	inst = this;
	setObjectName(tr("DbController"));

	/*!
	 * Check if config file contains key for sql path...
	 */
	if ((config.value(objectName() + Md::k.sqlDbFilePath, "").toString()).isEmpty())
		Locals::SQL_DB_PATH.setFile(
					QFileDialog::getOpenFileName(0, tr("Open sqlite database ..."),
														  Locals::PROJECT_PATHS.at(0)));
	else
		/*!
		 * Load SQL file path from ini.
		 */
		Locals::SQL_DB_PATH.setFile(
					config.value(objectName() + Md::k.sqlDbFilePath, "").toString());

	/*!
	 * Validate SQL database path.
	 */
	if (! (Locals::SQL_DB_PATH.isFile() &&
			 Locals::SQL_DB_PATH.isWritable() &&
			 Locals::SQL_DB_PATH.isReadable()))
		qReturn("Bad sql database file!");

	/*!
	 * Open database pointed by Locals::SQL_DB_PATH.
	 */
	openDatabase(Locals::SQL_DB_PATH.canonicalFilePath());
}
QList<bool> DbController::getConnectionState(bool beQuiet) {
	QList<bool> summary;

	summary << mDb.isDriverAvailable(Locals::SQL_DRIVER)
			  << mDb.isOpen() << mDb.isOpenError() << mDb.isValid();

	if (! beQuiet)		INFO << summary << mDb.connectionNames();

	return summary;
}

/* ======================================================================== */
/*                               openDatabase                               */
/* ======================================================================== */
void DbController::openDatabase(QString database) {
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
	INFO << objectName() << config.value(objectName() + Md::k.sqlDbFilePath);

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
	config.setValue(objectName() + Md::k.sqlDbFilePath,
						 Locals::SQL_DB_PATH.filePath());
	ConnectionWidget::instance()->refresh();
}
void DbController::onOpenDatabase() {
	QSETTINGS;
	Locals::SQL_DB_PATH.setFile(
				QFileDialog::getOpenFileName(0, tr("Open sqlite database ..."),
													  Locals::PROJECT_PATHS.at(0)));
	openDatabase(Locals::SQL_DB_PATH.canonicalFilePath());
}
void DbController::onDriverMessage (const QString& name) {
	WARN << name;
}
bool DbController::addConnectionsByCmdline(QVariant args) {
	QSqlError err;

	foreach (QString s, args.toStringList()) {
		QUrl url(Locals::SQL_DB_PATH.baseName(), QUrl::TolerantMode);
		url.setScheme(Locals::SQL_DRIVER);

		if (!url.isValid()) {
			qWarning("Invalid URL: %s", qPrintable(s));
			continue;
		}

		if (QSqlError::NoError != (addConnection(
												url.scheme().toUpper(), url.path(), url.host(),
												url.userName(), url.password(), url.port(-1))).type()) {

			url = QUrl(Locals::SQL_DB_PATH.filePath(), QUrl::TolerantMode);
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
QSqlError DbController::addConnection( const QString &driver, const QString &dbName,
													const QString &host, const QString &user,
													const QString &passwd, int port) {
	QSqlError err;
	/*QSqlDatabase db*/

	mDb = QSqlDatabase::addDatabase( (driver.isEmpty()) ? Locals::SQL_DRIVER : driver );
	mDb.setDatabaseName((dbName.isEmpty()) ? Locals::SQL_DB_PATH.filePath() : dbName );
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
