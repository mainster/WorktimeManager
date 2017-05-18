#ifndef LOCALS_H
#define LOCALS_H

#include <QString>
#include "globals.h"

#define MAINWINDOW_TITLE QString(qApp->applicationName())

//#define USE_QPOINTER

const QChar MathSymbolSquareRoot(0x221A);
const QChar MathSymbolPi(0x03A0);
const QChar MathSymbolDelta(0x0394);
const QChar MathSymbolSigma(0x03A3);

class Md {
public:
	static struct confKeys_t {
		QString
		focusOrder,
		alterRowColOn,
		alterRowColor,
		gridLineColor,
		tableFont,
		centerCols,
		visibleCols,
		sectionIdxs,
		windowGeometry,
		WindowState,
		allTabableWidgets,
		maxHourlyWage,
		sqlDbFilePath,
		minDateEdit,
		maxDateEdit,
		filtCaseSensCB,
		filtSyntaxCB;
	} k;

	/*!
	 * Lookup-table which holds SQL constraint strings.
	 */
	static struct SqlConstraints_t {
		const char *notNull;
		const char *unique;
	} sqlConstraints;

	static QString unAliasTableName(const QString maybeAliasedTable) {
		/*!
		 * Test table for alias extension and get sqlTableName if alias extension is used.
		 */
		QRegularExpression re("\\[(\\w+)\\]");
		QRegularExpressionMatch match = re.match(maybeAliasedTable);

		if (match.hasMatch())
			return match.captured(0).remove(QRegularExpression("[\\[\\]]"));

		return maybeAliasedTable;
	}

	/*!
	 * Lookup-table which holds aliases for sql table names.
	 */
	static QMap<QString, QString> tableAlias;

	/*!
	 * Lookup-table which holds aliases for sql table column header fields.
	 */
	static QMap<QString, QString> headerAlias;
};

Q_ENUMS(SqlConstraint)

class Locals {
public:
	static QFileInfo SQLITE_DB_PATH;
	static QString MYSQL_DB_NAME;
	static QString SQLITE_DRIVER;
	static QString MYSQL_DRIVER;
	static QStringList PROJECT_PATHS;
	static const QList<QString> BASE_DATA_TABLE_NAMES;
	static const QString
	browserStyleSheet,
	browserStyleSheetv2;
	Locals();
};

#endif // LOCALS_H
