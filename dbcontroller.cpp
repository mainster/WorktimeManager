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
		Locals::SQLITE_DB_PATH.setFile(
					QFileDialog::getOpenFileName(0, tr("Open sqlite database ..."),
														  Locals::PROJECT_PATHS.at(0)));
	else
		/*!
		 * Load SQL file path from ini.
		 */
		Locals::SQLITE_DB_PATH.setFile(
					config.value(objectName() + Md::k.sqlDbFilePath, "").toString());

	/*!
	 * Validate SQL database path.
	 */
	if (! (Locals::SQLITE_DB_PATH.isFile() &&
			 Locals::SQLITE_DB_PATH.isWritable() &&
			 Locals::SQLITE_DB_PATH.isReadable()))
		qReturn("Bad sql database file!");

	/*!
	 * Open database pointed by Locals::SQLITE_DB_PATH.
	 */
	openDatabase(Locals::SQLITE_DB_PATH.canonicalFilePath());
}
QList<bool> DbController::getConnectionState(bool beQuiet) {
	QList<bool> summary;

	summary << mDb.isDriverAvailable(Locals::SQLITE_DRIVER)
			  << mDb.isOpen() << mDb.isOpenError() << mDb.isValid();

	if (! beQuiet)		INFO << summary << mDb.connectionNames();

	return summary;
}

/* ======================================================================== */
/*                               openDatabase                               */
/* ======================================================================== */
void DbController::openDatabase(const QString &database, const QString &driver) {
	QSETTINGS;
	/*!
	 * Query platforms available SQL driver names.
	 */
	QStringList drivers = QSqlDatabase::drivers();
	if (!drivers.contains(Locals::SQLITE_DRIVER))
		CRIT << tr("empty");

	if (mDb.isOpen())
		mDb.close();

	mDb = QSqlDatabase::addDatabase(Locals::SQLITE_DRIVER);

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
						 Locals::SQLITE_DB_PATH.filePath());
	ConnectionWidget::instance()->refresh();
}
void DbController::onOpenDatabase() {
	QSETTINGS;
//	QMessageBox messageBox();
//	QAbstractButton *disconnectButton =
//			messageBox.addButton(tr("Disconnect"), QMessageBox::ActionRole);

//	messageBox.exec()
//	if (messageBox.clickedButton() == disconnectButton) {
//		 INFO << tr("disconn");
//	}
	QMessageBox msg;
	msg.setWindowTitle(tr("Select SQL driver..."));
	msg.setText(tr("SQLITE3 or MYSQL??"));
	msg.setIcon(QMessageBox::Question);
	QAbstractButton *btnSqlite = new QPushButton(tr("SQLITE3"));
	QAbstractButton *btnMysql = new QPushButton(tr("MYSQL"));

	msg.addButton(btnSqlite, QMessageBox::ActionRole);
	msg.addButton(btnMysql, QMessageBox::ActionRole);
	msg.exec();

	if (msg.clickedButton() == btnSqlite) {
		Locals::SQLITE_DB_PATH.setFile(
					QFileDialog::getOpenFileName(0, tr("Open sqlite database ..."),
														  Locals::PROJECT_PATHS.at(0)));
		openDatabase(Locals::SQLITE_DB_PATH.canonicalFilePath(), Locals::SQLITE_DRIVER);
		return;
	}

	if (msg.clickedButton() == btnMysql) {
		openDatabase(Locals::MYSQL_DB_NAME, Locals::MYSQL_DRIVER);
	}
}
void DbController::onDriverMessage (const QString& name) {
	WARN << name;
}
bool DbController::addConnectionsByCmdline(QVariant args) {
	QSqlError err;

	foreach (QString s, args.toStringList()) {
		QUrl url(Locals::SQLITE_DB_PATH.baseName(), QUrl::TolerantMode);
		url.setScheme(Locals::SQLITE_DRIVER);

		if (!url.isValid()) {
			qWarning("Invalid URL: %s", qPrintable(s));
			continue;
		}

		if (QSqlError::NoError != (addConnection(
												url.scheme().toUpper(), url.path(), url.host(),
												url.userName(), url.password(), url.port(-1))).type()) {

			url = QUrl(Locals::SQLITE_DB_PATH.filePath(), QUrl::TolerantMode);
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

	mDb = QSqlDatabase::addDatabase( (driver.isEmpty()) ? Locals::SQLITE_DRIVER : driver );
	mDb.setDatabaseName((dbName.isEmpty()) ? Locals::SQLITE_DB_PATH.filePath() : dbName );
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
