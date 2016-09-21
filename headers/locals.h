#ifndef LOCALS_H
#define LOCALS_H

#include <QString>
#include "globals.h"

#define MAINWINDOW_TITLE QString(qApp->applicationName())

//#define USE_QPOINTER

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
		maxHourlyWage;
	} k;

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

class Locals {
public:
	static QFileInfo SQL_DATABASE;
	static QString SQL_DRIVER;
	static QStringList PROJECT_PATHS;
	static const QString
	browserStyleSheet,
	browserStyleSheetv2;
	Locals();
};

#endif // LOCALS_H
