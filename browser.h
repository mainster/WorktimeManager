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


namespace Ui {
class Browser;
}

class TabView;
class ConnectionWidget;


/* ======================================================================== */
/*                      class Browser : public QWidget                      */
/* ======================================================================== */
class Browser : public QWidget {

	Q_OBJECT
	Q_PROPERTY(TvSelector tvSelector READ tvSelector WRITE setTvSelector NOTIFY tvSelectorChanged)
	Q_ENUMS(TvSelector)

public:
	enum TvSelector {
		selectByNone = 0x01,
		selectByViewPort = 0x02,
		selectByGroupBox = 0x04,
		selectByBoth = selectByViewPort | selectByGroupBox,
		empty = 0x10,
	};
	Q_DECLARE_FLAGS(TvSelectors, TvSelector)
	Q_FLAG(TvSelectors)

	TvSelector tvSelector() const { return m_tvSelector; }
	void setTvSelector(const TvSelector &tvSelector) { m_tvSelector = tvSelector; }

	/* ======================================================================== */
	/*                Public structure to deal with table data!                 */
	/* ======================================================================== */
	struct mTabs_t {
		/*! Constructor */
		mTabs_t() :
			TVA(QString("TableLeft")), TVB(QString("TableCenter")),
			TVC(QString("TableRight")), TVD(QString("TableBottom")),
			TVL1(QString("TableLong1")), TVL2(QString("TableLong2")) {	}

	public:
		QList<TabView *> tvsNoPtr()	const {
			return QList<TabView *>() << tva << tvb << tvc
											  << tvd << tvl1 << tvl2;
		}
		QList<TabView *> *tvs()					{
			mTvs << tva << tvb << tvc
				  << tvd << tvl1 << tvl2;
			return &mTvs;
		}
		QList<QString> tvIds() {
			return QList<QString>() << TVA << TVB << TVC
											<< TVD << TVL1 << TVL2;
		}

		TabView *tva, *tvb, *tvc, *tvd, *tvl1, *tvl2;

	private:
		QString TVA, TVB, TVC, TVD, TVL1, TVL2;
		QList<TabView *>  mTvs;
	};
	static struct mTabs_t mTabs;
	/* ======================================================================== */
	static QString tvStyleSheet;

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
		return mTabs.tvs();
	}

signals:
	void stateMsg(const QString &message, const int delay = 0);
	void tabViewSelChanged(TabView *sel);
	void visibilityChanged(bool b);
	void updateActions();
	void tvSelectorChanged();

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
	QMenu *menuBarElement();

 protected:
	void createUi(QWidget *passParent = 0);

protected slots:
	void showEvent(QShowEvent *e) override;
	void hideEvent(QHideEvent *e) override;
	bool eventFilter(QObject *obj, QEvent *e);
	void closeEvent(QCloseEvent *e);
	void onTvSelectorChanged();
	void onActMenuTrigd(bool state);

private:
	static Browser		*inst;
	ConnectionWidget	*connectionWidget;
	QGridLayout			*grLay;
	QTimer				*timCyc;
	bool					cyclicObjInfo;
	SfiltMdl				*proxyModel;
	SortWindow			*filterForm;
	MDStateBar			*stateBar;
	TvSelector			m_tvSelector;
	QSplitter	*splitter, *splitter_2, *splitter_3,
	*splitter_4, *splitter_5, *splitter_6, *splitter_7;
public:
	static const QString browserStyleSheet;
};


Q_DECLARE_OPERATORS_FOR_FLAGS(Browser::TvSelectors)

#endif

