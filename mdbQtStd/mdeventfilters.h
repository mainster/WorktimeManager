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
 * @file mdglobaleventfilter.h
 * @author Manuel Del Basso
 * @date 20-07-2016
 * @brief File containing ???????????????????????????????.
 *
 * Here typically goes a more extensive explanation of what the header
 * defines. Doxygens tags are words preceeded by either a backslash @TAG
 * or by an at symbol @@TAG.
 */
#ifndef MDGLOBALEVENTFILTER_H
#define MDGLOBALEVENTFILTER_H

#include <QObject>
// #include "mdcombobox.h"
#include "globals.h"
#include "debug.h"

class GlobalEventFilter : public QObject {

	Q_OBJECT

public:
	explicit GlobalEventFilter(bool printDebug = false, QObject *parent = 0)
		: QObject(parent), mPrintDebug(printDebug) {}

protected:

	/* ======================================================================== */
	/*                               eventFilter                                */
	/* ======================================================================== */
	virtual bool eventFilter(QObject *obj, QEvent *event) {
		/*!
		 * Event handler for global focus events.
		 */
		if ((event->type() == QEvent::FocusIn) ||
			 (event->type() == QEvent::FocusOut)) {

			if (mPrintDebug) {
				INFO << tr("Class:") << obj->metaObject()->className()
					  << tr("property ""hasFocus"":") << obj->property("hasFocus").toBool();
			}

			if (! qobject_cast<QWidget *>(obj))
				return QObject::eventFilter(obj, event);

			QWidget *wdg = qobject_cast<QWidget *>(obj);

			/*!
			 * Validate obj if custom property "hasFocus" exists and set/clear the value
			 */
			if (wdg->property("hasFocus").isValid()) {
				wdg->setProperty("hasFocus",(event->type() == QEvent::FocusIn) ? true : false);
				wdg->style()->unpolish(wdg);
				wdg->style()->polish(wdg);
				wdg->update();
			}
		}
		/*!
		 * Call superclass methode and pass the return value.
		 */
		return QObject::eventFilter(obj, event);
	}

private:
	bool	mPrintDebug;
};
#endif // MDGLOBALEVENTFILTER_H
