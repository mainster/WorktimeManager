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
#include "filterform.h"
#include "mdtable.h"
#include "mdtabview.h"
#include "connectionwidget.h"
#include "mdmenu.h"

class MdTable;
class MdTabView;
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
		QList<MdTable *> tvsNoPtr() {
			mTvs.clear();
			mTvs << tva << tvb << tvc
				  << tvd << tvl1 << tvl2;
			return mTvs;
		}
		QList<MdTable *> *tvs()					{
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
		MdTable *currentSelected(bool nullIfNoSelect = false) {
			foreach (MdTable *tv, tvsNoPtr())
				if (tv->isSelected())
					return tv;

			if (nullIfNoSelect)
				return NULL;
			/*!
			 * If no table widget is selected actively, use the tvc from mTvs which has
			 * currently no model loaded.
			 */
			foreach (MdTable *tv, tvsNoPtr())
				if (tv->tv()->model() == 0)
					return tv;

			/*!
			 * If all tv's had an active model, return TVA.
			 */
			return tva;
		}

		MdTable *tva, *tvb, *tvc, *tvd, *tvl1, *tvl2;

	private:
		QString TVA, TVB, TVC, TVD, TVL1, TVL2;
		QList<MdTable *>  mTvs;
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
#ifdef SET_STYLESHEETS
		QWidget::setStyleSheet( styleSheet );
		style()->unpolish(this);
		style()->polish(this);
		update();
#else
		Q_UNUSED(styleSheet);
#endif
	}
	void resetStyleSheet(MdTable *tv) {
		tv->setStyleSheet(styleSheet());
		style()->unpolish(tv);
		style()->polish(tv);
	}
	void clearStyleSheet(MdTable *tv) {
		tv->style()->unpolish(tv);
	}
	QList<MdTable *> *tvs() {
		return mTabs.tvs();
	}

signals:
	void stateMsg(const QString &message, const int delay = 0);
	void visibilityChanged(bool b);
	void updateWriteActions();
	void tvSelectorChanged();
	/*!
	 * \brief clearSelection signal is emitted from Browser::eventFilter if a mdtable
	 * selection was detected. Each MdTable instance have to connect this signal to a
	 * onClearSelection() slot. After unselecting all MdTable instances, the new selected
	 * MdTable instance could be set by Browser::eventFilter.
	 */
	void clearSelections();

public slots:
//	bool addWorktimeRecord(int prjID, int workerID, QString date,
//								  double hours, int worktimID = -1, MdTabView worktime = NULL) {
//		if (worktime == NULL)
//			worktime = mTabs.tva;
//		SqlRtm *rtm = dynamic_cast<SqlRtm *>(mdtv->model());
//		rtm->setEditStrategy(QSqlTableModel::OnManualSubmit);

//		QSqlRecord rec = SqlRtm.record();
//		rec.setValue(tr("prjID"),		prjID);
//		rec.setValue(tr("workerID"),	workerID);
//		rec.setValue(tr("dat"),			date);
//		rec.setValue(tr("hours"),		hours);

//		rtm->insertRecord(-1, rec);
//		 if(! rtm->submitAll()) {
//			  INFO << tr("CategoriesEditor::CategoriesEditor:")
//					 << ConnectionWidget::currentDb().lastError().text();
//		 }


//	}

	TvSelectors tvSelector() const	{ return m_tvSelector; }
	QSqlDatabase getCurrentDatabase()	{ return connectionWidget()->currentDb(); }
	void setTvSelector(TvSelectors tvSelector)	{ m_tvSelector = tvSelector; }
	void currentChanged(QModelIndex,QModelIndex)	{ emit updateWriteActions(); }
	void onConnectWdgMetaDataReq(const QString &table);
	MdMenu *menuBarElement();
	QStandardItemModel *tblToMetaDataMdl(const QString &table);
	void autofitRowCol();
	void customMenuRequested(QPoint pos);
	void exec();
	void execCustomQuery();
	void onConWidgetTableActivated(const QString &sqlTab);
	void onCyclic();
//	void onSourceTableChanged(FilterForm::SourceTableType sourceTable);
	void requeryWorktimeTableView(QString nonDefaulQuery = "");
	MdTable *createForeignTable(const QString &tNam, MdTable *tvc);

	void resetColumnConfig();
protected:
	void createUi(QWidget *passParent = 0);
	void createActions();
	bool restoreColumnOrderAndVisability(MdTable *tv);

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
	MDStateBar			*stateBar;
	TvSelectors			m_tvSelector;
	QSplitter	*splitter, *splitter_2, *splitter_3,
	*splitter_4, *splitter_5, *splitter_6, *splitter_7;
	MdTabView   *mdtv;
	QActionGroup		*actGrTvSelectBy, *actGrTvCount;
	QAction				*actSelByNone, *actSelByGrBx, *actSelByVPort, *actSelByBoth;
	int m_stateCounter;

public:
	MdMenu					*browsMenu;
	ConnectionWidget *connectionWidget() const { return mConnectionWidget; }
	void setFont(const QFont f);
	void selectAndSetFont();
	void selectAndSetRowColor();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Browser::TvSelectors)
Q_DECLARE_METATYPE(Browser::TvSelector)
#endif

