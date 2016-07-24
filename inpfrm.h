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
#include "browser.h"

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
		fieldGroup_t(QObject *parent = 0)
			: tableModel( new QSqlRelationalTableModel(parent) ),
			  proxyModel( new QSortFilterProxyModel(parent) ),
			  listModel( new QStringListModel(parent)) {}

		bool setModelColumns(/*const*/ QList<quint8> &columns, MdComboBox *comboBox) {
			/** Check if columns contains invalid column values */
			qSort(columns.begin(), columns.end());
			int colCount = proxyModel->columnCount(QModelIndex()),
					rowCount = proxyModel->rowCount(QModelIndex());

			if (columns.last() >= colCount)
				bReturn("Invalid column index found");

			INFO << rowCount << colCount;
			INFO << tableModel->rowCount(QModelIndex()) << tableModel->columnCount(QModelIndex());

			QStringList stringList;
			for (QList<quint8>::const_iterator cIt = columns.begin(); cIt != columns.end(); cIt++) {
				for (int r = 0; r < rowCount; r++) {
					stringList << proxyModel->data(
										  proxyModel->index(r, *cIt, QModelIndex()) ).toString();
				}
			}
			INFO << stringList;

			comboBox->setModel( new QStringListModel(stringList) );
			return true;
		}

		QSqlRelationalTableModel *tableModel;
		QSortFilterProxyModel *proxyModel;
		QStringListModel *listModel;
		QDataWidgetMapper *widgetMapper;
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
	void onInpFormUserCommitOLD();
	Qt::FocusOrderState getChangeFocusFlag() const;
	void setChangeFocusFlag(const Qt::FocusOrderState &stateFlag);
	bool setFocusOrder(QList<QWidget *> targets);

	void initComboboxes2();
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
//	QSqlRelationalTableModel	*model;
//	QSqlTableModel					*prjmd, *clmd, *wkmd;
//	QDataWidgetMapper				*prjm, *clm, *wkm;
	MDStateBar						*stateBar;
	QDateEdit						*de;
	QDialog							*dlg;
	QVector<QRadioButton*>		rbv;
//	QList<QWidget *>				mSqlCbs;
	QList<MdComboBox *>			mSqlCbs;
	QSqlQuery						query;
	QStringList						tblLst;
	QTextEdit						*gbSqlQuery;
	Qt::FocusOrderState			mChangeFocusFlag;
	bool								mEscapeTrigger;
//	SortFilterProxyModel			*proxyModel;
};

#endif // INPFRM_H
