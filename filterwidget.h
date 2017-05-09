#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <QClipboard>
#include <QCompleter>
#include <QObject>
#include <QTableView>
#include <QtCore>
#include <QtEvents>
#include <QtWidgets>
#include <QWidget>
#include <QWidget>

#include "browser.h"
#include "globals.h"
#include "mdtable.h"
#include "mdtabview.h"
#include "sfiltmdl.h"

namespace Ui {
	class FilterWidget;
}

QT_BEGIN_NAMESPACE
class Browser;
class QAction;
class QActionGroup;
QT_END_NAMESPACE

/* ======================================================================== */
/*                            class FilterWidget                            */
/* ======================================================================== */
class FilterWidget : public QWidget {

	Q_OBJECT
	Q_PROPERTY(SourceTableType sourceTableType READ sourceTableType WRITE
				  setSourceTableType NOTIFY sourceTableTypeChanged)

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

 public slots:
	SourceTableType sourceTableType() const {
		return mSourceTableType;
	}
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

Q_DECLARE_METATYPE(FilterWidget::SourceTableType)


Q_DECLARE_METATYPE(QRegExp::PatternSyntax)

/* ======================================================================== */
/*                             class FilterEdit                             */
/* ======================================================================== */
class FilterEdit : public QLineEdit {
	Q_OBJECT
	Q_PROPERTY(Qt::CaseSensitivity caseSensitivity READ caseSensitivity WRITE
				  setCaseSensitivity)
	Q_PROPERTY(QRegExp::PatternSyntax patternSyntax READ patternSyntax WRITE
				  setPatternSyntax)
 public:
	explicit FilterEdit(QWidget *parent = 0);

	Qt::CaseSensitivity caseSensitivity() const;
	void setCaseSensitivity(Qt::CaseSensitivity);

	QRegExp::PatternSyntax patternSyntax() const;
	void setPatternSyntax(QRegExp::PatternSyntax);

 signals:
	void filterChanged();

 private:
	QAction *m_caseSensitivityAction;
	QActionGroup *m_patternGroup;
};


/* ======================================================================== */
/*                          class CustFilterWindow                          */
/* ======================================================================== */
class CustFilterWindow : public QWidget {
	Q_OBJECT

 public:
	CustFilterWindow();

	void setSourceModel(QAbstractItemModel *model);

 private slots:
	void textFilterChanged();
	void dateFilterChanged();

 private:
	MySortFilterProxyModel *proxyModel;

	QGroupBox *sourceGroupBox;
	QGroupBox *proxyGroupBox;
	QTreeView *sourceView;
	QTreeView *proxyView;
	QLabel *filterPatternLabel;
	QLabel *fromLabel;
	QLabel *toLabel;
	FilterEdit *filterEdit;
	QDateEdit *fromDateEdit;
	QDateEdit *toDateEdit;
};


#endif // FILTERWIDGET_H
