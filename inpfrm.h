#ifndef INPFRM_H
#define INPFRM_H

#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#include <QtSql>

#include "globals.h"
#include "mdstatebar.h"
#include "mdeventfilters.h"
#include "types.h"
#include "mdcombobox.h"
#include "sortfilterproxymodel.h"

namespace Ui {
class InpFrm;
}
namespace Qt {
enum FocusOrderState {
	FocusChange_none,
	FocusChange_init,
	FocusChange_isRunning,
	FocusChange_done,
	FocusChange_rejected,
};
Q_ENUMS(FocusOrderState)
}

#define MAX_TAB_ORDER_WIDGETS		8

/* ======================================================================== */
/*                               class InpFrm                               */
/* ======================================================================== */
class InpFrm : public QDockWidget {

	Q_OBJECT

public:

	struct /*mTabOrder_t*/ {
	public:
		QList<QWidget *> current;
		QList<QWidget *> *next() { return &mNext; }

		bool addWidgetToNext(QWidget *widget) {
			if (mNext.length() == MAX_TAB_ORDER_WIDGETS)
				return false;

			mNext << widget;
			return true;
		}

	private:
		QList<QWidget *> mNext;
	} mTabOrder;

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
	Qt::FocusOrderState getChangeFocusFlag() const;
	void setChangeFocusFlag(const Qt::FocusOrderState &stateFlag);
	bool setFocusOrder(QList<QWidget *> targets);

signals:
	void changeFocusOrder(Qt::FocusOrderState state = Qt::FocusChange_init);
	void stateMessage(const QString msg, const int option);
	void requeryTableView();
	void buttonClicked(const QPushButton *button);
	void showDropdownView(bool onOff );

public slots:
	void initComboboxes();
	QString getQueryText() const;
	void saveSqlQueryInputText();
	void restoreSqlQueryInputText();
	void mapCbTableProxyOld();
	void mapCbTableProxy();
	void onSqlQuerysTextChanged();
	void onCbQueryIndexChaned(int idx);
	bool eventFilter(QObject *obj, QEvent *ev);
	void showEvent(QShowEvent *);
	void aButtonClick(bool);
	void onInpFormChanges(int idx);

	void onTestLeChanged();
protected:
	void connectActions();
	virtual void keyPressEvent(QKeyEvent *) override;
	void restoreTabOrder();

protected slots:
	void onInpFormChanges(QDate date);
	void onChangeFocusOrder(Qt::FocusOrderState state);
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
//	QList<QWidget *>				mSqlCbs;
	QList<QComboBox*>				mSqlCbs;
	QSqlQuery						query;
	QStringList						tblLst;
	QTextEdit						*gbSqlQuery;
	Qt::FocusOrderState			mChangeFocusFlag;
	bool								mEscapeTrigger;

};

#endif // INPFRM_H
