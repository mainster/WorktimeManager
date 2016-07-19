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

	explicit InpFrm(QWidget *parent = 0);
	static InpFrm *instance(QWidget *parent = 0x00) {
		if(inst == 0)
			inst = new InpFrm(parent);
		return inst;
	}
	~InpFrm();

	bool gbSqlQueryIsVisible() const;
	void gbSqlQuerySetVisible(bool vis);
	void onInpFormUserCommit();
	void onInpFormUserCommitOLD();

signals:
	void changeInpFrmTabOrder(InpFrm::states state);
	void stateMessage(const QString msg, const int option);
	void requeryTableView();
	void buttonClicked(const QPushButton *button);

public slots:
	void initComboboxes();
	QString getQueryText() const;
	void saveSqlQueryInputText();
	void restoreSqlQueryInputText();
	void refreshCbDropDownLists();
	void onSqlQuerysTextChanged();
	void onCbQueryIdentIndexChaned(int idx);
	bool eventFilter(QObject *obj, QEvent *ev);
	void showEvent(QShowEvent *);
	void aButtonClick(bool b);
	void onInpFormChanges(int idx);

protected:
	void connectActions();
	virtual void keyPressEvent(QKeyEvent *) override;

protected slots:
	void onInpFormChanges(QDate date);
	void onChangeTabOrderSlot(InpFrm::states state);
	QList<QWidget *> objLstToWidLst(QList<QObject *> lst);

	void hideEvent(QShowEvent *);
private slots:

private:
	Ui::InpFrm						*ui;
	static InpFrm					*inst;
	int								workerIdx, projIdx;
	InpFrm::states					changeTabOrder;
	QSqlRelationalTableModel	*model;
	QSqlTableModel					*prjmd, *clmd, *wkmd;
	QDataWidgetMapper				*prjm, *clm, *wkm;
	MDStateBar						*stateBar;
	QDateEdit						*de;
	QDialog							*dlg;
	QVector<QRadioButton*>		rbv;
	QList<QComboBox *>			mCbs;
	QList<QObject *>				objTabAble, objTabOrder;
	QSqlQuery						query;
	QStringList						tblLst;
	QTextEdit						*gbSqlQuery;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(InpFrm::states)
#endif // INPFRM_H
