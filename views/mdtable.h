#ifndef MDTABLE_H
#define MDTABLE_H

#include <QActionGroup>
#include <QCloseEvent>
#include <QColor>
#include <QDockWidget>
#include <QFont>
#include <QFrame>
#include <QGroupBox>
#include <QKeyEvent>
#include <QList>
#include <QMessageBox>
#include <QObject>
#include <QSet>
#include <QSqlRelationalTableModel>
#include <QSqlTableModel>
#include <QTableView>
#include <QTableWidget>
#include <QtCore>
#include <QtEvents>
#include <QtGui>
#include <QtSql>
#include <QtWidgets>
#include <QWidget>

#include "globals.h"
#include "connectionwidget.h"
#include "models.h"
#include "sectionmask.h"



/* ======================================================================== */
/*                              class MdTable                               */
/* ======================================================================== */
#define SET_STYLESHEETS

namespace Ui {
class MdTable;
//class Browser;
}

class MdTable : public QWidget {

	Q_OBJECT
	Q_PROPERTY(bool selected READ isSelected WRITE setSelected NOTIFY selectedChanged)
	Q_PROPERTY(QString sqlTableName READ sqlTableName WRITE setSqlTableName NOTIFY sqlTableNameChanged)

public:
	explicit MdTable(QWidget *parent = 0);
	~MdTable();

	void setSelectionMode(QAbstractItemView::SelectionMode mode) {
		m_tv->setSelectionMode(mode);
	}
	QItemSelectionModel *selectionModel() {
		return m_tv->selectionModel();
	}

	/* ======================================================================== */
	/*                            Getters / Setters                             */
	/* ======================================================================== */
	QGroupBox *grBox() const	{ return m_gb; }
	QTableView *tv() const		{ return m_tv; }
	bool isSelected()				{ return m_selected; }
	void setSelected(bool selected = true) {
		m_selected = selected;
		m_gb->setProperty("selected", m_selected);
#ifdef SET_STYLESHEETS
		setStyleSheet(styleSheet());
#endif
		emit selectedChanged(selected);
	}
	void clearSelected() {
		m_selected = false;
		m_gb->setProperty("selected", m_selected);
		setStyleSheet(styleSheet());
	};
	QString &sqlTableName()		{ return m_sqlTableName; }
	void setSqlTableName(const QString &name) {
		m_sqlTableName = name;
		emit sqlTableNameChanged(name);
	};

	SqlRtm *clearMdlSrces();

public slots:
	QFont restoreFont();
	SqlRtm *modelCast();
	void deleteRow();
	void insertRow();
	void onObjectNameChanged(const QString &objNam);
	void onSectionMoved(int logicalIdx, int oldVisualIdx, int newVisualIdx);
	void onSqlTableNameChanged(const QString &name);
	void onUpdateWriteActions();
	void refreshView();
	void removeColumnsConfig();
	void resetColumnsDefaultPos(bool allVisible);
	void resizeRowsColsToContents();
	void restoreColumnOrderAndVisability();
	void restoreView();
	void setAlternateRowCol(QColor &col, bool alternateEnabled = true);
	void setColumnHidden(const int column, const bool hide);
	void setEditTriggers(QTableView::EditTriggers triggers);
	void setModel(QAbstractItemModel *model);
	void storeFont(QFont font);

signals:
	void sqlTableNameChanged(const QString &name);
	void selectedChanged(bool isSelected);

protected:
	QList<QAction *> createActions();
	void connectActions();
	virtual void showEvent(QShowEvent *) override;
	virtual void mouseDoubleClickEvent(QMouseEvent *) override;
	bool eventFilter(QObject *obj, QEvent *event);
	void hideEvent(QHideEvent *);
	void restoreColumnOrderAndVisability2();

protected slots:
	void onClearSelection() {

	}
	void onActSectionMask(bool sectionMask = false);
	void onActGrStrategyTrigd(QAction *sender);
	void onActGrContextTrigd(QAction *sender);
	void storeActionState(QAction *sender);
	bool restoreActionObjects();

private:
	Ui::MdTable			*ui;
	QTableView        *m_tv;
	QGroupBox         *m_gb;
	SectionMask			*mSectMsk;
	QString				m_sqlTableName;
	bool					m_selected;
	QActionGroup		*actGrStrategy, *actGrContext;
	QAction				*actInsertRow, *actDeleteRow, *actFieldStrategy,
	*actRowStrategy, *actManualStrategy,*actSubmit, *actRevert, *actSelect, *actSectionMask;

};



#endif // MDTABLE_H
