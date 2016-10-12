/**
 ** This file is part of the libreTerminal0.11 project.
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
 * @file debug.h
 * @author Manuel Del Basso
 * @date 30-05-2016
 * @brief File containing macro definitions for colored qDebug() output.
 *
 * <http://brianmilco.blogspot.de/2011/11/color-debug-output-with-qt-and-qdebug.html/>
 *
 * Here typically goes a more extensive explanation of what the header
 * defines. Doxygens tags are words preceeded by either a backslash @TAG
 * or by an at symbol @@TAG.
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <QMessageLogContext>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <stdio.h>
#include <stdlib.h>
#include <QTime>
#include <QVariant>

#if QT_VERSION < 0x050500
#error "Bad QtVersion for debug purposes!"
#endif

void mdMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

QString dbgFuncName(QString name);
QString dbgFuncNameMdb(QString name);

#define INFO (qInfo().noquote() << dbgFuncNameMdb(Q_FUNC_INFO) << QString(":"))
#define WARN (qWarning().noquote() << dbgFuncNameMdb(Q_FUNC_INFO) << QString(":"))
#define CRIT (qCritical().noquote() << dbgFuncNameMdb(Q_FUNC_INFO) << QString(":"))

#define TIC TicToc::tic(tr("try 1"));
#define TICs(s) TicToc::tic(tr(s));
#define TOC TicToc::toc();

/* ======================================================================== */
/*                               class TicToc                               */
/* ======================================================================== */
class TicToc {

public:
	static struct type_t {
		QString mLabel;
		QTime mTime;
	} t;

	static struct type_t2 {
		QString mLabel;
		QTime *mTime;
	} *t2;

	TicToc() {}
	static void tic(QString label) {
		t2->mTime = new QTime();
		t2->mLabel = label;
		t2->mTime->start();
	}
	static void toc() {
		if (! t2->mTime->elapsed()) {
			INFO << QString("No running tic [%1(%2)]").arg(__FILE__).arg(__LINE__);
			return;
		}
		INFO << QString("%1 (%2): %3ms").arg(t2->mLabel).arg(__LINE__).arg(t2->mTime->elapsed());
		delete t2->mTime;
	}
	~TicToc() {}

private:

};

#endif // DEBUG_H


///* ======================================================================== */
///*                     class DbgPrintOnlyNonEqualLines                      */
///* ======================================================================== */
//class DbgPrintOnlyNonEqualLines {
//public:
////	DbgPrintOnlyNonEqualLines() : QDebug(QtInfoMsg){ }

//	void operator<<(const QVariant v) {
//		if (! v.canConvert<QString>())
//			return;

//		if (v.toString() != mMsg) {
//			qDebug().noquote() << QString("\033[48;5;10mINFO\033[0m ")
//									 << v.toString()
//									 << qPrintable(dbgFuncNameMdb(Q_FUNC_INFO)) << ":";
//			mMsg = v.toString();
//		}
//	}

//private:
//	static QString mMsg;
//};
