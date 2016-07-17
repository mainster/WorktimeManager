#ifndef LOCALS_H
#define LOCALS_H

#include <QString>
#include "globals.h"

#define MAINWINDOW_TITLE QString(qApp->applicationName())


class Locals {
public:
	static QFileInfo	SQL_DATABASE;

	Locals();
};

#endif // LOCALS_H
