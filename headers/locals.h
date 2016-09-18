#ifndef LOCALS_H
#define LOCALS_H

#include <QString>
#include "globals.h"

#define MAINWINDOW_TITLE QString(qApp->applicationName())

class Md {
public:
	static struct confKeys_t {
		QString
		focusOrder,
		altRowColOn,
		alterRowColor,
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

	static QMap<QString, QString> tableAlias;
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
