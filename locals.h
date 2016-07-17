#ifndef LOCALS_H
#define LOCALS_H

#include <QString>
#include "globals.h"

#define MAINWINDOW_TITLE QString(qApp->applicationName())


class Locals {
public:
	static QString
	SQL_DB_PATH,
	SQL_DB_FILE;

	Locals();
};

#endif // LOCALS_H
