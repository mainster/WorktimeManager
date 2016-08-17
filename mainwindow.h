#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFlag>
#include <QFlags>
#include <QMainWindow>
#include <QtSql>
#include <QSqlError>
#include <QtWidgets>
#include <QMenu>
#include <QRegularExpression>
#include <QActionGroup>

#include "ui_dbconndlg.h"
#include "browser.h"
#include "connectionwidget.h"
#include "dbconndlg.h"
#include "globals.h"
#include "inpfrm.h"
#include "locals.h"
#include "mdstatebar.h"
#include "tabledelegate.h"
#include "types.h"
#include "dbcontroller.h"

namespace Ui {
class MainWindow;
}

class ConnectionWidget;
QT_FORWARD_DECLARE_CLASS(QTableView)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QTextEdit)
QT_FORWARD_DECLARE_CLASS(QSqlError)


/* ======================================================================== */
/*                             class MainWindow                             */
/* ======================================================================== */
class MainWindow : public QMainWindow {

	Q_OBJECT
//	Q_PROPERTY(bool cyclicObjInfo READ cyclicObjInfo WRITE setCyclicObjInfo NOTIFY cyclicObjInfoChanged)

public:
	/*!
	 * Enumerations for actionConnection methodes
	 */
	enum ConnectReceiver {
		connectThis = 0x01,
		connectOthers = 0x02,
		connectAll = 0x04,
		connectNone = 0x08,
	};
	Q_DECLARE_FLAGS(ConnectReceivers, ConnectReceiver)
	Q_FLAG(ConnectReceivers)

	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

signals:
//	void cyclicObjInfoChanged();


public slots:
//	bool cyclicObjInfo() const { return m_cyclicObjInfo; }
//	void setCyclicObjInfo(bool cyclicObjInfo) { m_cyclicObjInfo = cyclicObjInfo; }
	void initDocks();
	void about() {
		QMessageBox::about
				(this, tr("About"),
				 tr("The SQL Browser demonstration shows how a data browser"
					 "can be used to visualize the results of SQL statements"
					 "on a live database"));
	}
	void connectActions(ConnectReceiver receivers = connectThis);

protected slots:
	void makeMenuBar();
	void onActCfgInpFrmTabOrdTrig() {
		emit inpFrm->changeFocusOrder(Qt::FocusChange_init);
	}
	void onActSaveTrig();
	void onActOpenTrig();
	void onBrowseSqlTrig(bool onOff) {
		browser->setVisible(onOff);
	}
	void onMenuStyleShtInpFrmTrig(bool b) {
		if (!b) return;

		//   actStyleShtA->setChecked( !b );
		//   actStyleShtInpFrm->setChecked( b );

		//   *browser->tvs()[0]->getGrBox();
		//   foreach (QGroupBox *gb, browser->getGBoxs()) {
		//      gb->setStyleSheet( Globals::gbStyleShtInpFrm);
		//   }
	}
	void onMenuStyleShtATrig(bool b);
	void onActExportTrig() {
		INFO << "!";
	}
	void onUnderConstrTrig();
	void onSetFont();
//	void onShowSqlQueryTogd(bool b);
	void onSetAlterRowColTrig();
	void onCyclic();
	void onResizerDlgTrig();
	void onInpFrmButtonClick(bool);
	bool restoreActionObjects();
	void createActions();
	void ACTION_STORE(QObject *obj, QString regex);
	void onActionGroupTrigd(QAction *sender);

private slots:
	void onOpenCloseInpFrm(bool onOff) {
		inpFrm->setVisible(onOff);
	}
	void onTblOpen(bool) {
		INFO << tr("I am an empty slot, use me!!!");
	}
	void onActCloseTrig();
	void onStyleSheetTrig();

protected:
	void setStyleSheet(const QString& styleSheet) {
		QWidget::setStyleSheet( styleSheet );
		style()->unpolish(this);
		style()->polish(this);
		update();
	}
	bool eventFilter(QObject *obj, QEvent *event);
	void showEvent(QShowEvent *e);
	void hideEvent(QHideEvent *) override;
	void closeEvent(QCloseEvent *e);
	void actionEvent(QActionEvent *e) override;
	void ACTION_RESTORE(QObject *obj, QString regex);

private:
	Ui::MainWindow *ui;
	Browser      *browser;
	InpFrm       *inpFrm;
	MDStateBar   *stateBar;
	SortWindow   *sortwindow;
	DbController *mDbc;

	QMenu        *cfgMenu, *mBar;
	QWidget      *wid;
	QTimer		 *timCyc;

	QActionGroup *actGrTbMain, *actGrTbMenu;

	QAction *actNew, *actOpen, *actSave, *actExport, *actBrowseSQL, *actInpForm, *actShowTbl,
			*actDbModMaster, *actClose, *actGbStyleShtA, *actGbSShtInpFrm, *actUnderConstr,
			*actSelFont, *actCyclicObjInfo, *actResizerDlg, *actShowSqlQuery, *actSetAlterRowCol,
			*actAutoFitTables, *actFilterTable, *actFilterTableWindow, *actFilterForm,
			*actCfgInpFrmTabOrd;

};



/* ======================================================================== */
/*                            class StaticSignal                            */
/* ======================================================================== */
#ifdef STATIC_SIGNAL_CLASS
class StaticSignal : public QObject {

	Q_OBJECT

public:
	static StaticSignal *instance(QObject *parent = 0) {
		if (inst == 0)
			return new StaticSignal(parent);
		return inst;
	}
	~StaticSignal() { }

signals:
	void showDropdownViews(bool onOff);

private:
	StaticSignal(QObject *parent)
		: QObject(parent) {
		inst = this;
	}
	static StaticSignal *inst;

};


//static void showDropdownCallback(bool onOff) {
//	StaticSignal::instance()->showDropdownViews(onOff);
//}
#endif

#endif // MAINWINDOW_H
