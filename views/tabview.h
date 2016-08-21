#ifndef TABVIEW_H
#define TABVIEW_H

#include <QColor>
#include <QGroupBox>
#include <QKeyEvent>
#include <QSet>
#include <QSqlTableModel>
#include <QTableView>
#include <QWidget>
#include <QActionGroup>
#include <QFont>

#include "globals.h"
#include "connectionwidget.h"
#include "models.h"
#include "sectionmask.h"

namespace Ui {
class TabView;
//class Browser;
}

class TabView : public QWidget {

	Q_OBJECT
	Q_PROPERTY(bool isSelected READ isSelected WRITE setSelect NOTIFY selectChanged)
	Q_PROPERTY(QString sqlTableName READ sqlTableName WRITE setSqlTableName NOTIFY sqlTableNameChanged)

public:
	/*!
	 * Data structure for QHeaderView::sectionPosition().
	 */
//	typedef struct visualIdx_t {
//		int
//	};

	bool isSelected()		{ return m_select; };
	void setSelect(bool select) {
		m_select = select;
		m_gb->setProperty("select", m_select);
	};
	QString &sqlTableName()		{ return m_sqlTableName; };
	void setSqlTableName(const QString &name) {
		m_sqlTableName = name;
		emit sqlTableNameChanged(name);
	};
//	static QString tvStyleSheet;
	static QString tvStyleHeadView;
public slots:
	void setSelected() {
		m_select = true;
		m_gb->setProperty("select", m_select);
		setStyleSheet(styleSheet());
	};
	void clearSelected() {
		m_select = false;
		m_gb->setProperty("select", m_select);
		setStyleSheet(styleSheet());
	};

public:
	explicit TabView(QWidget *parent = 0);
	~TabView();

	/* ======================================================================== */
	/*                            Getters / Setters                             */
	/* ======================================================================== */
	QGroupBox *grBox() const	{ return m_gb; }
	QTableView *tv() const		{ return m_tv; }

	void setSelectionMode(QAbstractItemView::SelectionMode mode) {
		m_tv->setSelectionMode(mode);
	}
	QItemSelectionModel *selectionModel() {
		return m_tv->selectionModel();
	}


	SqlRtm *clearMdlSrces();
public slots:
	void onObjectNameChanged(const QString &objNam);
	void setAlternateRowCol(QColor &col, bool alternateEnabled = true);
	void restoreView();
	void insertRow();
	void deleteRow();
	void onUpdateWriteActions();
	void refreshView();
	void resizeRowsColsToContents();
	void setColumnHidden(const int column, const bool hide);
	void setModel(QAbstractItemModel *model);
	void setEditTriggers(QTableView::EditTriggers triggers);
	void onSectionMoved(int logicalIdx, int oldVisualIdx, int newVisualIdx);
	void onSqlTableNameChanged(const QString &name);
	void storeRtm();
	void restoreRtm();
	QFont restoreFont();
	void storeFont(QFont font);

	SqlRtm *modelCast();
signals:
	void sqlTableNameChanged(const QString &name);
	void selectChanged(bool isSelected);

protected:
	QList<QAction *> createActions();
	void connectActions();
	virtual void showEvent(QShowEvent *) override;
	virtual void mouseDoubleClickEvent(QMouseEvent *) override;
	bool eventFilter(QObject *obj, QEvent *event);
	void hideEvent(QHideEvent *ev);

protected slots:
	void onClearSelection() {

	}
	void onActSectionMask(bool sectionMask = false);
	void onActGrStrategyTrigd(QAction *sender);
	void onActGrContextTrigd(QAction *sender);
	void storeActionState(QAction *sender);
	bool restoreActionObjects();


private:
	Ui::TabView			*ui;
	QTableView        *m_tv;
	QGroupBox         *m_gb;
	SectionMask			*mSectMsk;
	QString				m_sqlTableName;
	bool					m_select;
	QActionGroup		*actGrStrategy, *actGrContext;
	QAction				*actInsertRow, *actDeleteRow, *actFieldStrategy,
	*actRowStrategy, *actManualStrategy,*actSubmit, *actRevert, *actSelect, *actSectionMask;

};


#endif // TABVIEW_H
