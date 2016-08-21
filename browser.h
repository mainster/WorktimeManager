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
#include "mdmenu.h"

class TabView;
//class ConnectionWidget;

/* ======================================================================== */
/*                      class Browser : public QWidget                      */
/* ======================================================================== */
class Browser : public QWidget {

	Q_OBJECT
	Q_PROPERTY(TvSelectors tvSelector READ tvSelector WRITE setTvSelector NOTIFY tvSelectorChanged)

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
		QList<TabView *> tvsNoPtr() {
			mTvs.clear();
			mTvs << tva << tvb << tvc
				  << tvd << tvl1 << tvl2;
			return mTvs;
		}
		QList<TabView *> *tvs()					{
			mTvs.clear();
			mTvs << tva << tvb << tvc
				  << tvd << tvl1 << tvl2;
			return &mTvs;
		}
		QList<QString> tvIds() {
			return QList<QString>() << TVA << TVB << TVC
											<< TVD << TVL1 << TVL2;
		}

		/*!
		 * Get currently selected tv.
		 */
		TabView *currentSelected(bool nullIfNoSelect = false) {
			foreach (TabView *tv, tvsNoPtr())
				if (tv->isSelected())
					return tv;

			if (nullIfNoSelect)
				return NULL;
			/*!
			 * If no table widget is selected actively, use the tvc from mTvs which has
			 * currently no model loaded.
			 */
			foreach (TabView *tv, tvsNoPtr())
				if (tv->tv()->model() == 0)
					return tv;

			/*!
			 * If all tv's had an active model, return TVA.
			 */
			return tva;
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
	void setStyleSheet(const QString& styleSheet) {
		QWidget::setStyleSheet( styleSheet );
		style()->unpolish(this);
		style()->polish(this);
		update();
	}
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
	void visibilityChanged(bool b);
	void updateWriteActions();
	void tvSelectorChanged();
	/*!
	 * \brief clearSelection signal is emitted from Browser::eventFilter if a tabview
	 * selection was detected. Each TabView instance have to connect this signal to a
	 * onClearSelection() slot. After unselecting all TabView instances, the new selected
	 * TabView instance could be set by Browser::eventFilter.
	 */
	void clearSelections();

public slots:
	TvSelectors tvSelector() const	{ return m_tvSelector; }
	QSqlDatabase getCurrentDatabase()	{ return connectionWidget()->currentDb(); }
	void setTvSelector(TvSelectors tvSelector)	{ m_tvSelector = tvSelector; }
	void currentChanged(QModelIndex,QModelIndex)	{ emit updateWriteActions(); }
	void onConnectWdgMetaDataReq(const QString &table);
	MdMenu *menuBarElement();
	QStandardItemModel *tblToMetaDataMdl(const QString &table);
	QTableView *createView(QSqlQueryModel *model, const QString &title);
	void autofitRowCol();
	void customMenuRequested(QPoint pos);
	void exec();
	void onActFilterForm(bool b);
	void onConWidgetTableActivated(const QString &sqlTab);
	void onCyclic();
	void onSourceTableChanged(SortWindow::SourceTable sourceTable);
	void requeryWorktimeTableView(QString nonDefaulQuery = "");
	TabView *createForeignTable(const QString &tNam, TabView *tvc);

	void resetColumnConfig();
protected:
	void createUi(QWidget *passParent = 0);
	void createActions();
	bool restoreColumnOrderAndVisability(TabView *tv);

protected slots:
	bool restoreUi();
	void showEvent(QShowEvent *e) override;
	void hideEvent(QHideEvent *e) override;
	bool eventFilter(QObject *obj, QEvent *e);
	void closeEvent(QCloseEvent *e);
	void onTvSelectorChanged();
	void onActGroupTrigd(QAction *action);
	void storeActionState(QAction *sender);
	bool restoreActionObjects();

private:
	static Browser		*inst;
	ConnectionWidget	*mConnectionWidget;
	QGridLayout			*grLay;
	QTimer				*timCyc;
	SfiltMdl				*proxyModel;
	SortWindow			*filterForm;
	MDStateBar			*stateBar;
	TvSelectors			m_tvSelector;
	QSplitter	*splitter, *splitter_2, *splitter_3,
	*splitter_4, *splitter_5, *splitter_6, *splitter_7;

	QActionGroup		*actGrTvSelectBy, *actGrTvCount;
	QAction				*actSelByNone, *actSelByGrBx, *actSelByVPort, *actSelByBoth;

public:
	MdMenu					*browsMenu;
	static const QString  browserStyleSheet, browserStyleSheetv2;
	ConnectionWidget *connectionWidget() const { return mConnectionWidget; }
	void setFont(const QFont f);
	void selectAndSetFont();
	void selectAndSetRowColor();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Browser::TvSelectors)
Q_DECLARE_METATYPE(Browser::TvSelector)
#endif

