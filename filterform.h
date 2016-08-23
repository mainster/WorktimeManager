#ifndef FILTERFORM_H
#define FILTERFORM_H

#include <QWidget>
#include <QCompleter>
#include <QObject>
#include <QtCore>
#include <QtEvents>

#include "mysortfilterproxymodel.h"
#include "globals.h"
#include "tabview.h"
//#include "browser.h"

namespace Ui {
class FilterForm;
}


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
//	Q_DECLARE_FLAGS(SourceTable, SourceTable)
//	Q_FLAG(SourceTable)

	explicit FilterForm(SourceTableType srcType, TabView *srcTable = 0, QWidget *parent = 0);

	~FilterForm();
	static FilterForm *instance(SourceTableType newSrcType, TabView *newSrcTable) {
		if(inst == 0)
			inst = new FilterForm(newSrcType, newSrcTable);
		return inst;
	}
	static FilterForm *instance() {
		if(inst == 0)
			inst = new FilterForm(useSelectedSource);
		return inst;
	}
//	void setSourceModel(QAbstractItemModel *model);
//		virtual bool eventFilter(QObject *o, QEvent *e) override;

public slots:
	SourceTableType sourceTableType() const { return mSourceTableType; }
	void setSourceTableType(SourceTableType sourceTable) {
		mSourceTableType = sourceTable;
		emit sourceTableTypeChanged(sourceTable);
	}
//	virtual void keyPressEvent(QKeyEvent *) override;
	void cbTextFilterChanged();

	void setSourceTabView(TabView *tv);
signals:
	void visibilityChanged(bool visible);
	void sourceTableTypeChanged(SourceTableType sourceTable);

protected:
	void showEvent(QShowEvent *e);
	void hideEvent(QHideEvent *e);
	void closeEvent(QCloseEvent *e);

	void keyPressEvent(QKeyEvent *e);
private slots:
	void textFilterChanged();
	void dateFilterChanged();

private:
	static FilterForm *inst;
	static const QString WINDOW_TITLE_PREFIX;
	Ui::FilterForm *ui;
	SfiltMdl *proxyModel;
	SourceTableType	mSourceTableType;
	TabView *mTv;
	QGroupBox *sourceGB;
	QGroupBox *proxyGB;
	QTreeView *sourceView;
	QTreeView *proxyView;
	QCheckBox *filtCaseSensCB;
	QLabel *filterPatternLabel;
	QLabel *fromLabel;
	QLabel *toLabel;
	QLineEdit *filtPattLE;
	QComboBox *filtSyntaxCB, *filtPattCb;
	QDateEdit *fromDateEdit;
	QDateEdit *toDateEdit;
	QAction *deleteAct;

	// QObject interface
public:
//	virtual bool eventFilter(QObject *, QEvent *) override;

};

//Q_DECLARE_OPERATORS_FOR_FLAGS(FilterForm::SourceTable)
Q_DECLARE_METATYPE(FilterForm::SourceTableType)

#endif // FILTERFORM_H
