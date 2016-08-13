#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFlag>
#include <QFlags>
#include <QMainWindow>
#include <QtSql>
#include <QSqlError>
#include <QtWidgets>
#include <QMenu>

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

public slots:
	void onBrowseSqlTrig(bool b);
	void about() {
		QMessageBox::about
				(this, tr("About"),
				 tr("The SQL Browser demonstration shows how a data browser"
					 "can be used to visualize the results of SQL statements"
					 "on a live database"));
	}
	void onMenuStyleShtInpFrmTrig(bool b);
	void onMenuStyleShtATrig(bool b);
	void onActExportTrig() {
		INFO << "!";
	}
	void onUnderConstrTrig();
	void onSetFont();
	void initDocks();
	void onActHideSqlQueryTrig();
	void onSetAlterRowColTrig();
	void connectActions(ConnectReceiver receivers = connectThis);
	void onCyclic();
	void onResizerDlgTrig();
	void onInpFrmButtonClick(bool);

protected slots:
	void makeMenuBar();
	void onActCfgInpFrmTabOrdTrig() {
		emit inpFrm->changeFocusOrder(Qt::FocusChange_init);
	}
	void onActSaveTrig();
	void onActOpenTrig();
	bool restoreActionObjects();

private slots:
	void onOpenCloseInpFrm(bool onOff);
	void onTblOpen(bool);
	void onActCloseTrig();

protected:
	bool eventFilter(QObject *obj, QEvent *event);
	void showEvent(QShowEvent *e);
	void hideEvent(QHideEvent *) override;
	void closeEvent(QCloseEvent *e);

private:
	Ui::MainWindow *ui;
	static int   fuse;
	Browser      *browser;
	InpFrm       *inpFrm;
	QMenu        *cfgMenu;
	QWidget      *wid;
	MDStateBar   *stateBar;
	SortWindow   *sortwindow;
	QMenu        *mBar;
	DbController *mDbc;


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
