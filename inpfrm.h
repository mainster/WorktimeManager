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
namespace Qt {
enum FocusOrderChangeState {
	FocusOrderChange_none,
	FocusOrderChange_init,
	FocusOrderChange_isRunning,
	FocusOrderChange_done,
	FocusOrderChange_rejected,
};
Q_ENUMS(FocusOrderChangeState)
}

class InpFrm;

/* ======================================================================== */
/*                           class FocusEvFilter                            */
/* ======================================================================== */
class FocusEvFilter : public QObject {

	Q_OBJECT

public:
	explicit FocusEvFilter(QObject *parent = 0)
		: QObject(parent) {}

//	QList<QObject *> getObjFocusOrder() const { return objFocusOrder; }
//	QList<QObject *> *objFocusOrder() { return *objFocusOrder; }

protected:

	bool eventFilter(QObject *obj, QEvent *event);

private:
};

/* ======================================================================== */
/*                               class InpFrm                               */
/* ======================================================================== */
class InpFrm : public QDockWidget {

	Q_OBJECT

public:


	explicit InpFrm(QWidget *parent = 0);
	static InpFrm *instance(QWidget *parent = 0x00) {
		if(inst == 0)
			inst = new InpFrm(parent);
		return inst;
	}
	~InpFrm();

	void setSqlQueryTextboxVisible(bool visible);
	void onInpFormUserCommit();
	void onInpFormUserCommitOLD();

	QList<QObject *> getObjFocusOrder() const { return mObjFocusOrder; }
	QList<QObject *> *objFocusOrder() { return &mObjFocusOrder; }

	Qt::FocusOrderChangeState getChangeFocusFlag() const;
	void setChangeFocusFlag(const Qt::FocusOrderChangeState &stateFlag);

	bool setFocusOrder(QList<QObject *> focusOrder);
signals:
	void changeFocusOrder(Qt::FocusOrderChangeState state);
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
	void onCbQueryIndexChaned(int idx);
	bool eventFilter(QObject *obj, QEvent *ev);
	void showEvent(QShowEvent *);
	void aButtonClick(bool);
	void onInpFormChanges(int idx);

protected:
	void connectActions();
	virtual void keyPressEvent(QKeyEvent *) override;

protected slots:
	void onInpFormChanges(QDate date);
	void onChangeFocusOrder(Qt::FocusOrderChangeState state);
	void hideEvent(QShowEvent *);

private slots:

private:
	Ui::InpFrm						*ui;
	static InpFrm					*inst;
	int								workerIdx, projIdx;
	QSqlRelationalTableModel	*model;
	QSqlTableModel					*prjmd, *clmd, *wkmd;
	QDataWidgetMapper				*prjm, *clm, *wkm;
	MDStateBar						*stateBar;
	QDateEdit						*de;
	QDialog							*dlg;
	QVector<QRadioButton*>		rbv;
	QList<QComboBox *>			mSqlCbs;
	QList<QObject *>				objTabAble;
	QSqlQuery						query;
	QStringList						tblLst;
	QTextEdit						*gbSqlQuery;
	bool								mEscapeTrigger;

	Qt::FocusOrderChangeState	mChangeFocusFlag;
	QList<QObject*>				mObjFocusOrder;
};
//Q_DECLARE_OPERATORS_FOR_FLAGS(InpFrm::states)

#endif // INPFRM_H
