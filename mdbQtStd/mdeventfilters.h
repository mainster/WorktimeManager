/**
 ** This file is part of the worktimeManagerSubdirPrj project.
 ** Copyright 2016 Manuel Del Basso <manuel.delbasso@gmail.com>.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/
/*!
 * @file mdeventfilters.h
 * @author Manuel Del Basso
 * @date 20-07-2016
 * @brief File containing ???????????????????????????????.
 *
 * Here typically goes a more extensive explanation of what the header
 * defines. Doxygens tags are words preceeded by either a backslash @TAG
 * or by an at symbol @@TAG.
 */
#ifndef MDEVENTFILTERS_H
#define MDEVENTFILTERS_H

#include <QObject>
#include "globals.h"
#include "debug.h"
#include "inpfrm.h"

class InpFrm;

/* ======================================================================== */
/*                         class GlobalEventFilter                          */
/* ======================================================================== */
class GlobalEventFilter : public QObject {

	Q_OBJECT

public:
	explicit GlobalEventFilter(bool printDebug = false, QObject *parent = 0)
		: QObject(parent), mPrintDebug(printDebug) {}

protected:
	virtual bool eventFilter(QObject *obj, QEvent *event);

private:
	bool	mPrintDebug;
};

/* ======================================================================== */
/*                           class SqlEventFilter                           */
/* ======================================================================== */
class SqlEventFilter : public QObject {

	Q_OBJECT

public:
	explicit SqlEventFilter(QObject *parent = 0)
		: QObject(parent) {}

protected:
	bool eventFilter(QObject *obj, QEvent *event);


private:

};

#endif
