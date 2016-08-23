#ifndef FILTERFORM_H
#define FILTERFORM_H

#include "mysortfilterproxymodel.h"
#include "globals.h"
#include "tabview.h"

#include <QWidget>
#include <QCompleter>
#include <QObject>
#include <QtCore>
#include <QtEvents>


namespace Ui {
class FilterForm;
}

class FilterForm : public QWidget {

	Q_OBJECT
	Q_PROPERTY(SourceTable sourceTableFlg READ sourceTableFlg WRITE setSourceTableFlg NOTIFY sourceTableChanged)

public:
	enum SourceTable {
		useNone = 0x01,
		useSelected = 0x02,
		useWorktime = 0x04,
	};
	Q_ENUM(SourceTable)
//	Q_DECLARE_FLAGS(SourceTable, SourceTable)
//	Q_FLAG(SourceTable)

	explicit FilterForm(QWidget *parent = 0, TabView *srcTable = 0);
	~FilterForm();
	static FilterForm *instance(QWidget *parent = 0x00) {
		if(inst == 0)
			inst = new FilterForm(parent);
		return inst;
	}
	void setSourceModel(QAbstractItemModel *model);

//		virtual bool eventFilter(QObject *o, QEvent *e) override;

public slots:
	SourceTable sourceTableFlg() const { return mSourceTable; }
	void setSourceTableFlg(SourceTable sourceTable) {
		mSourceTable = sourceTable;
		emit sourceTableChanged(sourceTable);
	}
//	virtual void keyPressEvent(QKeyEvent *) override;
	void cbTextFilterChanged();

signals:
	void visibilityChanged(bool visible);
	void sourceTableChanged(SourceTable sourceTable);

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
	Ui::FilterForm *ui;
	SfiltMdl *proxyModel;
	SourceTable	mSourceTable;
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
Q_DECLARE_METATYPE(FilterForm::SourceTable)

#endif // FILTERFORM_H
