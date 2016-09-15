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

#define KEY_DOCKWIDGETAREA	QString("DockWidgetArea")
/* ======================================================================== */
/*                               class InpFrm                               */
/* ======================================================================== */
class InpFrm : public QDockWidget {

	Q_OBJECT
//	Q_PROPERTY(TvSelectors tvSelector READ tvSelector WRITE setTvSelector NOTIFY tvSelectorChanged)

//public slots:
//	TvSelectors tvSelector() const { return m_tvSelector; }
//	void setTvSelector(TvSelectors tvSelector) { m_tvSelector = tvSelector; }

public:
	struct fieldGroup_t {

		fieldGroup_t(const QString &tableName, MdComboBox *comboBox, QObject *parent = 0)
			: tableModel( new QSqlRelationalTableModel(parent) ),
			  pProxyModel( new QSortFilterProxyModel(parent) ),
			  listModel( new QStringListModel(parent) ),
			  comboBox( comboBox ) {
			tableModel->setTable(tableName);
		}

		QSqlRelationalTableModel *tableModel;
		QSortFilterProxyModel *pProxyModel;
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

	void setQueryBoxVisible(bool visible);
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
	void showEvent(QShowEvent *);
	void aButtonClick(bool);
	void onInpFormChanges(int idx);
	void onCbIndexChanged(const int index);

	void onInpFormUserCommitAlt();
protected:
	virtual void keyPressEvent(QKeyEvent *) override;
	virtual void resizeEvent(QResizeEvent *) override;
	void connectActions();
	void restoreTabOrder();

	QList<QWidget *> getTabableWidgets();
protected slots:
	void onInpFormChanges(QDate date);
	void onChangeFocusOrder(Qt::FocusOrderState state);
	void hideEvent(QShowEvent *);
	void closeEvent(QCloseEvent *) override;
	void onUndockEvent(bool isUndocked);
	void onDockLocationChanged(Qt::DockWidgetArea area);

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

	struct fixedHeights_t {
		fixedHeights_t(int visible = 0, int invisible = 0)
			: sqlQueryVisible(visible),
			  sqlQueryInvisible(invisible) { }

		int sqlQueryVisible, sqlQueryInvisible;
	};
	fixedHeights_t fixedHeights;

};

#endif // INPFRM_H
