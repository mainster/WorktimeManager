#ifndef INPFRM_H
#define INPFRM_H

#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#include <QtSql>

#include "globals.h"
#include "mdstatebar.h"

namespace Ui {
class InpFrm;
}

class InpFrm : public QDockWidget {

	Q_OBJECT

public:
	enum state {
		state_none,
		state_init_changeTabOrder,
		state_running_changeTabOrder,
		state_done_changeTabOrder,
		state_rejected_changeTabOrder,
	};
	Q_DECLARE_FLAGS(states, state)

	static InpFrm *instance(QWidget *parent = 0x00) {
		if(inst == 0)
			inst = new InpFrm(parent);
		return inst;
	}
	~InpFrm();

	void restoreInpFrmGeometry();
	void saveInpFrmGeometry();

	QSqlError addConnection(const QString &driver,
									const QString &dbName,
									const QString &host,
									const QString &user,
									const QString &passwd, int port);
	bool gbSqlQueryIsVisible() const;
	void gbSqlQuerySetVisible(bool vis);
	void onBtnOkClicked2();
	void onBtnOkClickedOLD();

signals:
	void changeInpFrmTabOrder(InpFrm::states state);
	void stateMessage(QString msg);
	void requeryTableView();

public slots:
	void showEvent(QShowEvent *);
	QString getQueryText() const;
	void saveSqlQueryInputText();
	void restoreSqlQueryInputText();
	void refreshCbDropDownLists();
	void onBtnOkClicked();
	void onBtnUndoClicked();
	void onBtnRedoClicked();
	void onBtnClearClicked();
	void onSqlQuerysTextChanged();
	void onCbQueryIdentIndexChaned(int idx);
	bool eventFilter(QObject *obj, QEvent *ev);
	void initComboboxes();

protected:
	void connectActions();

protected slots:
	void onInpFormChanges(int idx = -1);
	void onInpFormChanges(QDate date);
	void showEventDelay();
	void onChangeTabOrderSlot(InpFrm::states state);
	QList<QWidget *> objLstToWidLst(QList<QObject *> lst);

private:
	explicit InpFrm(QWidget *parent = 0);

	Ui::InpFrm *ui;
	static InpFrm *inst;
	void showError(const QSqlError &err);
	QSqlRelationalTableModel *model;
	int workerIdx, projIdx;
	QDialog *dlg;
	QVector<QRadioButton*> rbv;
	QStringList tblLst;
	QTextEdit *gbSqlQuery;
	//	Browser *browser;
	QDataWidgetMapper *prjm, *clm, *wkm;
	QSqlTableModel *prjmd, *clmd, *wkmd;
	QList<QComboBox *> cbs;
	QDateEdit *de;
	MDStateBar * stateBar;
	//   QWidgetList wsTabable, objTabOrder;
	//   QList<QWidget *> wsTabAble, objTabOrder;
	//   QList<QWidget> wsNoP;
	QList<QObject *> objTabAble, objTabOrder;
	//   QObjectList objTabable;

	InpFrm::states changeTabOrder;
	//   void buildTable__worktime();
	//   void buildTable__worktimeTst();
	QSqlQuery query;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(InpFrm::states)
#endif // INPFRM_H
