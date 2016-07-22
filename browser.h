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
#include "ui_browser.h"


namespace Ui {
class Browser;
}

class TabView;

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
class Browser : public QWidget, private Ui::Browser {

   Q_OBJECT

public:
   explicit Browser(QWidget *parent = 0);
	static Browser *instance(QWidget *parent = 0x00) {
      if(inst == 0)
         inst = new Browser(parent);
      return inst;
   }
	virtual ~Browser();

   void insertRow();
   void deleteRow();
   void updateActions();
   void resetStyleSheet(TabView *tv) {
      tv->setStyleSheet(styleSheet());
      style()->unpolish(tv);
      style()->polish(tv);
   }
   void clearStyleSheet(TabView *tv) {
      tv->style()->unpolish(tv);
   }
   QVector<TabView *> tvs;

   /**
    * Class accessors
    */
//   SortWindow *getSortwindow0() const;
	//   SortWindow *getSortwindow1() const;

signals:
	void stateMsg(const QString &message, const int delay = 0);
   void tabViewSelChanged(TabView *sel);
	void visibilityChanged(bool b);

public slots:
   void showMetaData(const QString &table);
   void currentChanged(QModelIndex,QModelIndex) { updateActions(); }
	void on_insertRowAction_triggered() { insertRow(); }
	void on_deleteRowAction_triggered() { deleteRow(); }
   void on_fieldStrategyAction_triggered();
   void on_rowStrategyAction_triggered();
   void on_manualStrategyAction_triggered();
   void on_submitAction_triggered();
   void on_revertAction_triggered();
   void on_selectAction_triggered();
   void on_connectionWidget_tableActivated(const QString &sqlTab);
	void on_connectionWidget_metaDataRequested(const QString &table) {
		showMetaData(table);
	}
   void onCyclic();
   void customMenuRequested(QPoint pos);
   void initTableView(QWidget *parent, QStringList &accNam);
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

protected slots:
   void showEvent(QShowEvent *e);
   bool eventFilter(QObject *obj, QEvent *e);
	void hideEvent(QShowEvent *);

private slots:
//   QAbstractItemModel *createMailModel(QObject *parent);

private:
	static Browser	*inst;
	Ui::Browser		*ui;
	QTimer			*timCyc;
	bool				cyclicObjInfo;
   /**
    * This pointer gots manipulated by the double click event handler.
    */
	TabView			* dblClickFocusedTv;
	SfiltMdl			*proxyModel;
	SortWindow		*filterForm;
	MDStateBar		*stateBar;

};

#endif

