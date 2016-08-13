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
#include "mysortfilterproxymodel.h"
#include "browser.h"
#include "qpair.h"

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

/*!
 * Access index definition for QList<fieldGroup_t> mModels.
 */
#define	IDX_PROJECT			0
#define	IDX_CLIENT			1
#define	IDX_WORKER			2
#define	IDX_CONTRACOTR		3

/* ======================================================================== */
/*                               class InpFrm                               */
/* ======================================================================== */
class InpFrm : public QDockWidget {

	Q_OBJECT

public:
	struct fieldGroup_t {

		fieldGroup_t(const QString &tableName, MdComboBox *comboBox, QObject *parent = 0)
			: tableModel( new QSqlRelationalTableModel(parent) ),
			  proxyModel( new QSortFilterProxyModel(parent) ),
			  listModel( new QStringListModel(parent) ),
			  comboBox( comboBox ) {
			tableModel->setTable(tableName);
		}

		QSqlRelationalTableModel *tableModel;
		QSortFilterProxyModel *proxyModel;
		QStringListModel *listModel;
		QDataWidgetMapper *widgetMapper;
		MdComboBox *comboBox;
	};

	QList<fieldGroup_t> mModels;


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
	Qt::FocusOrderState getChangeFocusFlag() const;
	void setChangeFocusFlag(const Qt::FocusOrderState &stateFlag);
	bool setFocusOrder(QList<QWidget *> targets);

signals:
	void changeFocusOrder(Qt::FocusOrderState state = Qt::FocusChange_init);
	void stateMessage(const QString msg, const int option);
    void newWorktimeRecord();
	void buttonClicked(const QPushButton *button);
	void showDropdownView(bool onOff );

public slots:
	void initComboboxes();
	QString getQueryText() const;
	void saveSqlQueryInputText();
	void restoreSqlQueryInputText();
	void onSqlQuerysTextChanged();
	void onCbQueryIndexChaned(int idx);
	bool eventFilter(QObject *obj, QEvent *ev);
	void showEvent(QShowEvent *);
	void aButtonClick(bool);
	void onInpFormChanges(int idx);
	void onCbIndexChanged(const int index);

protected:
	void connectActions();
	virtual void keyPressEvent(QKeyEvent *) override;
	void restoreTabOrder();

protected slots:
	void onInpFormChanges(QDate date);
	void onChangeFocusOrder(Qt::FocusOrderState state);
	void hideEvent(QShowEvent *);
	void closeEvent(QCloseEvent *) override;

private slots:

private:
	Ui::InpFrm						*ui;
	static InpFrm					*inst;
	int								workerIdx, projIdx;
	MDStateBar						*stateBar;
	QDateEdit						*de;
	QDialog							*dlg;
	QVector<QRadioButton*>		rbv;
	QList<MdComboBox *>			mSqlCbs;
	QSqlQuery						query;
	QStringList						tblLst;
	QTextEdit						*gbSqlQuery;
	Qt::FocusOrderState			mChangeFocusFlag;
	bool								mEscapeTrigger;

};

#endif // INPFRM_H
