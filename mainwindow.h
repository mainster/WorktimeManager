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
#include <QHeaderView>
#include <QFile>
#include <QFileInfo>

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
#include "mdnotes.h"
#include "mdmenu.h"
#include "textedit.h"
#include "filterwidget.h"

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

public slots:
	void initDocks();
	void about() {
		QMessageBox::about
				(this, tr("About"),
				 tr("The SQL Browser demonstration shows how a data browser"
					 "can be used to visualize the results of SQL statements"
					 "on a live database"));
	}
	void connectActions(ConnectReceiver receivers = connectThis);
	void setVisibleFake(QObject *);

protected slots:
	void makeMenu();
	void onActCfgInpFrmTabOrdTrig() {
		emit inpFrm->changeFocusOrder(Qt::FocusChange_init);
	}
	void onActSaveTrig();
	void onActOpenTrig();
	void onBrowseSqlToggd(bool b) {
		browser->setVisible(b);
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
	void onCyclic();
	void onResizerDlgTrig();
	void onInpFrmButtonClick(bool);
	bool restoreActionObjects();
	void createActions();
	void onActionGroupTrigd(QAction *sender);
	void onActNotesToggd(bool b) {
		notes.toDo->setVisible(b);
		notes.features->setVisible(b);
	}
	void onActRichTextToggd(bool);
	void onActFilterWindowSource(bool);
	void onActFilterForm(bool b);
	void onSomeHasBeenSelected(bool hasSelected);

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
#ifdef SET_STYLESHEETS
		QWidget::setStyleSheet( styleSheet );
		style()->unpolish(this);
		style()->polish(this);
		update();
#else
		Q_UNUSED(styleSheet);
#endif
	}
	bool eventFilter(QObject *obj, QEvent *event);
	void showEvent(QShowEvent *e);
	void hideEvent(QHideEvent *) override;
	void closeEvent(QCloseEvent *e);

private:
	void keyPressEvent(QKeyEvent *e);

	Ui::MainWindow *ui;
	Browser			*browser;
	InpFrm			*inpFrm;
	MDStateBar		*stateBar;
	FilterForm		*filterForm, *filterFormWkt;
	DbController	*mDbc;
	TextEdit			*richEditor;
	QMenu				*cfgMenu, *mBar;
	QWidget			*wid, *mCentralWidget;
	QTimer			*timCyc;

	struct notes_t {
		MDNotes *toDo,
		*comments,
		*features;
	} notes;

	QActionGroup *actGrTbMain, *actGrTbMenu, *actGrFilterWidg;

	QAction *actNew, *actOpen, *actSave, *actExport, *actBrowseSQL, *actInpForm,
	*actShowTbl, *actDbModMaster, *actClose, *actRichEdit, *actUnderConstr, *actNotes,
	*actGbStyleShtA, *actGbSShtInpFrm, *actSelFont, *actCyclicObjInfo, *actResizerDlg,
	*actShowSqlQuery, *actSetAlterRowCol, *actSetGridColor, *actAutoFitTables,
	*actFilterTable, *actFilterTableWindow, *actFilterForm, *actCfgInpFrmTabOrd,
	*actDoFiltWorktimeTbl, *actDoFiltSelectedTbl, *actResetConfig;

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
