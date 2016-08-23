#ifndef FILTERFORM_H
#define FILTERFORM_H

#include <QWidget>
#include <QCompleter>
#include <QObject>
#include <QtCore>
#include <QtEvents>
#include <QTableView>

#include "mysortfilterproxymodel.h"
#include "globals.h"
#include "mdtable.h"
#include "browser.h"

namespace Ui {
class FilterForm;
}

class Browser;

class FilterForm : public QWidget {

	Q_OBJECT
	Q_PROPERTY(SourceTableType sourceTableType READ sourceTableType WRITE setSourceTableType NOTIFY sourceTableTypeChanged)

public:

	enum SourceTableType {
		useNoneSource = 0x01,
		useSelectedSource = 0x02,
		useWorktimeSource = 0x04,
	};
	Q_ENUM(SourceTableType)

	explicit FilterForm(SourceTableType srcType, QList<MdTable *> allTvs,
							  QWidget *parent = NULL);

	~FilterForm();
	static FilterForm *instance(SourceTableType newSrcType, QList<MdTable *> allTvs) {
		if(inst == 0)
			inst = new FilterForm(newSrcType, allTvs);
		return inst;
	}
	static FilterForm *instance() {
		if(inst == 0)
			inst = new FilterForm(useSelectedSource, QList<MdTable *>());
		return inst;
	}

public slots:
	SourceTableType sourceTableType() const { return mSourceTableType; }
	void setSourceTableType(SourceTableType sourceTable) {
		mSourceTableType = sourceTable;
		emit sourceTableTypeChanged(sourceTable);
	}
	void cbTextFilterChanged();
	void setSourceTabView(MdTable *tv);

signals:
	void visibilityChanged(bool visible);
	void sourceTableTypeChanged(SourceTableType sourceTable);

protected:
	void showEvent(QShowEvent *e);
	void hideEvent(QHideEvent *e);
	void closeEvent(QCloseEvent *e);
	void keyPressEvent(QKeyEvent *e);

protected slots:
	void onSelectedTableChange(bool selected);
	void onSourceRowCountChanged(const uint oldCount, const uint newCount);
	void onProxyRowCountChanged(const uint oldCount, const uint newCount);

private slots:
	void textFilterChanged();
	void dateFilterChanged();

private:
	static FilterForm *inst;
	static const QString WINDOW_TITLE_PREFIX;
	static const QString PROXY_GROUPBOX_TITLE_PREFIX;
	static const QString SOURCE_GROUPBOX_TITLE_PREFIX;
	Ui::FilterForm *ui;
	SfiltMdl *proxyModel;
	SourceTableType	mSourceTableType;
	MdTable *mTv;
	QGroupBox *sourceGB;
	QGroupBox *proxyGB;
	//	QTreeView *sourceView;
	//	QTreeView *proxyView;
	QTableView *sourceView;
	QTableView *proxyView;
	QCheckBox *filtCaseSensCB;
	QLabel *filterPatternLabel;
	QLabel *fromLabel;
	QLabel *toLabel;
	QLineEdit *filtPattLE;
	QComboBox *filtSyntaxCB, *filtPattCb;
	QDateEdit *fromDateEdit;
	QDateEdit *toDateEdit;
	QAction *deleteAct;
};

//Q_DECLARE_OPERATORS_FOR_FLAGS(FilterForm::SourceTable)
Q_DECLARE_METATYPE(FilterForm::SourceTableType)

#endif // FILTERFORM_H
