#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <QWidget>
#include <QtWidgets>
#include <QClipboard>

#include "globals.h"
#include "mdtabview.h"

#include <QWidget>
#include <QCompleter>
#include <QObject>
#include <QtCore>
#include <QtEvents>
#include <QTableView>

#include "sfiltmdl.h"
#include "globals.h"
#include "mdtable.h"
#include "mdtabview.h"
#include "browser.h"

namespace Ui {
class FilterWidget;
}

class Browser;

class FilterWidget : public QWidget {

	Q_OBJECT
	Q_PROPERTY(SourceTableType sourceTableType READ sourceTableType WRITE setSourceTableType NOTIFY sourceTableTypeChanged)

public:

	enum SourceTableType {
		useNoneSource = 0x01,
		useSelectedSource = 0x02,
		useWorktimeSource = 0x04,
	};
	Q_ENUM(SourceTableType)

	explicit FilterWidget(SourceTableType srcType,
								 QList<MdTable *> allTbls,
								 QWidget *parent = NULL);
	explicit FilterWidget(QWidget *parent = NULL);
	~FilterWidget();

	void setSourceView(MdTabView *table);

public slots:
	SourceTableType sourceTableType() const { return mSourceTableType; }
	void setSourceTableType(SourceTableType sourceTable) {
		mSourceTableType = sourceTable;
		emit sourceTableTypeChanged(sourceTable);
	}
	void cbTextFilterChanged();
	void setSourceTable(MdTable *tbl);

signals:
	void visibilityChanged(bool visible);
	void sourceTableTypeChanged(SourceTableType sourceTable);

protected:
	void showEvent(QShowEvent *e);
	void hideEvent(QHideEvent *e);
	void closeEvent(QCloseEvent *e);
	void keyPressEvent(QKeyEvent *e);
	QList<QString> copy(bool overwriteClipboardBuffer);

protected slots:
	void onSelectedTableChange(bool selected);
	void onSourceRowCountChanged(const uint oldCount, const uint newCount);
	void onProxyRowCountChanged(const uint oldCount, const uint newCount);
	void paste();
	void sumSelection();
	void restoreWidgetStates();

private slots:
	void textFilterChanged();
	void dateFilterChanged();
	void onFilterPatternTextChanged();

private:
	static const QString WINDOW_TITLE_PREFIX;
	static const QString PROXY_GROUPBOX_TITLE_PREFIX;
	static const QString SOURCE_GROUPBOX_TITLE_PREFIX;
	FilterWidget	*filterWidget;
	SqlRtm			*sourceModel;
	SfiltMdl			*proxyModel;

	MdTabView		*sourceView, *proxyView;
	QGroupBox		*sourceGB, *proxyGB;
	QCheckBox		*filtCaseSensCB;
	QLabel			*filtPattLbl, *fromLabel, *toLabel;
	QComboBox		*filtSyntaxCB, *filtPattCb;
	QDateEdit		*fromDateEdit, *toDateEdit;
	QLineEdit		*filtPattLe;
	QVBoxLayout		*mainLayout;
	QSplitter		*splitter;
	QGridLayout		*proxyLayout, *sourceLayout;
	QAction			*actSumSelection;
	QClipboard		*clipboard;

	SourceTableType	mSourceTableType;
	MdTable				*mTbl;
};

//Q_DECLARE_OPERATORS_FOR_FLAGS(FilterForm::SourceTable)
Q_DECLARE_METATYPE(FilterWidget::SourceTableType)


#endif // FILTERWIDGET_H
