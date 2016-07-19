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
		test;
	} keys;
};

class Locals {
public:
	static QFileInfo SQL_DATABASE;
	static QString SQL_DRIVER;
	Locals();
};

#endif // LOCALS_H
