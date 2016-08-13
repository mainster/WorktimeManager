/**
 ** This file is part of the WorktimeManager project.
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
 * @file browser.h
 * @author Manuel Del Basso
 * @date 18-07-2016
 * @brief File containing ???????????????????????????????.
 *
 * Here typically goes a more extensive explanation of what the header
 * defines. Doxygens tags are words preceeded by either a backslash @TAG
 * or by an at symbol @@TAG.
 */
#ifndef BROWSER_H
#define BROWSER_H

#include <QCloseEvent>
#include <QDockWidget>
#include <QFrame>
#include <QGroupBox>
#include <QList>
#include <QMessageBox>
#include <QObject>
#include <QSqlRelationalTableModel>
#include <QSqlTableModel>
#include <QTableView>
#include <QTableWidget>
#include <QtCore>
#include <QtEvents>
#include <QtGui>
#include <QtSql>
#include <QtWidgets>
#include <QWidget>

#include "dbconndlg.h"
#include "globals.h"
#include "inpfrm.h"
#include "mdstatebar.h"
#include "models.h"
#include "mysortfilterproxymodel.h"
#include "sortwindow.h"
#include "tabview.h"
#include "connectionwidget.h"

//#include "ui_browser.h"


namespace Ui {
class Browser;
}

class TabView;
class ConnectionWidget;

/*!
 * \class [class name]
 *
 * \brief [brief description]
 *
 * [detailed description]
 *
 * \author [your name]
 * \date
 */


//#define TVA    "TableLeft"
//#define TVB    "TableCenter"
//#define TVC    "TableRight"
//#define TVD    "TableBottom"
//#define TVL2   "TableLong1"
//#define TVL1   "TableLong2"


//TVA  = "TableLeft",
//TVB  = "TableCenter",
//TVC  = "TableRight",
//TVD  = "TableBottom",
//TVL2 = "TableLong1",
//TVL1 = "TableLong2";

class Browser : public QWidget {

	Q_OBJECT

public:
	static struct mTvs {
		QList<TabView *>  obj;
		QList<QString *>  name;
		QString TVA, TVB, TVC, TVD, TVL1, TVL2;

		mTvs() :
			TVA(QString("TableLeft")), TVB(QString("TableCenter")),
			TVC(QString("TableRight")), TVD(QString("TableBottom")),
			TVL2(QString("TableLong1")), TVL1(QString("TableLong2")) {	}
	} mTvs;

	static const QString browserStyleSheet;

	explicit Browser(QWidget *parent = 0);
	static Browser *instance(QWidget *parent = 0x00) {
		if(inst == 0)
			inst = new Browser(parent);
		return inst;
	}
	virtual ~Browser();

	//    void insertRow(QTableView *tv);
	void deleteRow(QTableView *tv);
	void resetStyleSheet(TabView *tv) {
		tv->setStyleSheet(styleSheet());
		style()->unpolish(tv);
		style()->polish(tv);
	}
	void clearStyleSheet(TabView *tv) {
		tv->style()->unpolish(tv);
	}
	QList<TabView *> *tvs() {
		return &mTvs;
	}
//	void QSPLT_RESTORE();
//	void QSPLT_STORE();

signals:
	void stateMsg(const QString &message, const int delay = 0);
	void tabViewSelChanged(TabView *sel);
	void visibilityChanged(bool b);
	void updateActions();

public slots:
	void showMetaData(const QString &table);
	void currentChanged(QModelIndex,QModelIndex) { emit updateActions(); }
	void onConnectionWidgetTableActivated(const QString &sqlTab);
	void on_connectionWidget_metaDataRequested(const QString &table) {
		showMetaData(table);
	}
	void onCyclic();
	void customMenuRequested(QPoint pos);
	QFont selectFont();
	int setFontAcc(QFont &font);
	void onCyclicObjInfoTrig(bool b) { cyclicObjInfo = b; }
	void exec();
	void requeryWorktimeTableView(QString nonDefaulQuery = "");
	void showRelatTable(const QString &tNam, TabView *tvc);
	QTableView *createView(QSqlQueryModel *model, const QString &title);
	void initializeMdl(QSqlQueryModel *model);
	QSqlDatabase getCurrentDatabase() {
		return connectionWidget->currentDatabase();
	}
	void autofitRowCol();
	void onActFilterForm(bool b);
	void onBeforeUpdate(int row, QSqlRecord &record);

protected:
	void createUi(QWidget *passParent = 0);

protected slots:
	void showEvent(QShowEvent *e) override;
	void hideEvent(QHideEvent *e) override;
	bool eventFilter(QObject *obj, QEvent *e);
	void mousePressEvent(QMouseEvent *e) override;
private slots:
	//   QAbstractItemModel *createMailModel(QObject *parent);

private:
	static Browser      *inst;
	ConnectionWidget *connectionWidget;
	QGridLayout *grLay;
	TabView *tva, *tvb, *tvc, *tvd, *tvl1, *tvl2;
	QSplitter *splitter, *splitter_2, *splitter_3, *splitter_4,
	*splitter_5, *splitter_6, *splitter_7;


	//    Ui::Browser         *ui;
	QTimer              *timCyc;
	bool					cyclicObjInfo;
	SfiltMdl			*proxyModel;
	SortWindow          *filterForm;
	MDStateBar          *stateBar;


	// QWidget interface
protected:
	void closeEvent(QCloseEvent *e);
};

#endif

