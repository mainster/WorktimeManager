#ifndef LOCALS_H
#define LOCALS_H

#include <QString>
#include "globals.h"

#define MAINWINDOW_TITLE QString(qApp->applicationName())


class Locals {
public:
	static QFileInfo SQL_DATABASE;
	static QString SQL_DRIVER;
	Locals();
};

#endif // LOCALS_H
