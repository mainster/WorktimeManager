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
		WindowState;
	} k;
};

class Locals {
public:
	static QFileInfo SQL_DATABASE;
	static QString SQL_DRIVER;
	static const QString  browserStyleSheet, browserStyleSheetv2, MdTableStyleSheet;
	Locals();
};

#endif // LOCALS_H
