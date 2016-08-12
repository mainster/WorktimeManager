#ifndef TABVIEW_H
#define TABVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QTableView>
#include <QSqlTableModel>
#include <QKeyEvent>
#include <QColor>

#include "models.h"


class TabView : public QTableView {

	Q_OBJECT

public:
	explicit TabView(QWidget *parent = 0);
	~TabView();

	QGroupBox *grBox() const		{ return m_gb; }
	void setGrBox(QGroupBox *gb)	{ m_gb = gb; }
	bool isActiveSelected()			{ return property("activeSel").toBool(); }
	void clearActiveSelection()	{ setProperty("activeSel", false); }
	void setAlternateRowCol(QColor &col, bool alternateEnabled = true);
	QList<QAction *> getTblActs() const;

public slots:
	void restoreView();
	void onTabViewSelChanged(TabView *tv);
	void insertRow();
	void deleteRow();
	void refreshView();
	void resizeRowsColsToContents();

	void onUpdateActions();
	void onActInsertRowtriggered();
	void onActDeleteRowtriggered();
	void onActFieldStrategytriggered();
	void onActRowStrategytriggered();
	void onActManualStrategytriggered();
	void onActSubmittriggered();
	void onActReverttriggered();
	void onActSelect();

signals:
	void onSelChanged(TabView *tv);

protected:
	QList<QAction *> createActions();
	void onObjectNameChanged(const QString &objNam);

private:
	QGroupBox			*m_gb;
	QList<QAction *>    tblActs;
	QAction
	*actInsertRow, *actDeleteRow, *actFieldStrategy, *actRowStrategy, *actManualStrategy,
	*actSubmit, *actRevert, *actSelect;
};


#endif // TABVIEW_H
