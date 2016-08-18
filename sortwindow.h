#ifndef SORTWINDOW_H
#define SORTWINDOW_H

#include "mysortfilterproxymodel.h"
#include "globals.h"
#include "tabview.h"

#include <QWidget>
#include <QCompleter>
#include <QObject>
#include <QtCore>
#include <QtEvents>

namespace Ui {
class SortWindow;
}

class SortWindow : public QWidget {

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

	explicit SortWindow(QWidget *parent = 0, TabView *srcTable = 0);
	~SortWindow();
	static SortWindow *instance(QWidget *parent = 0x00) {
		if(inst == 0)
			inst = new SortWindow(parent);
		return inst;
	}
	void setSourceModel(QAbstractItemModel *model);

public slots:
	SourceTable sourceTableFlg() const { return mSourceTable; }
	void setSourceTableFlg(SourceTable sourceTable) {
		mSourceTable = sourceTable;
		emit sourceTableChanged(sourceTable);
	}
	virtual void keyPressEvent(QKeyEvent *) override;
	void cbTextFilterChanged();

signals:
	void closesUncheck(bool b);
	void sourceTableChanged(SourceTable sourceTable);

protected slots:
	void closeEvent(QCloseEvent *e);


private slots:
	void textFilterChanged();
	void dateFilterChanged();

private:
	static SortWindow *inst;

	Ui::SortWindow *ui;
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


	// QWidget interface
protected:
};

//Q_DECLARE_OPERATORS_FOR_FLAGS(SortWindow::SourceTable)
Q_DECLARE_METATYPE(SortWindow::SourceTable)

#endif // SORTWINDOW_H
