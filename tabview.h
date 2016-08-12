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
	Q_PROPERTY(bool activeSelection READ isActiveSelection WRITE setActiveSelection)
	Q_PROPERTY(bool selected READ isSelected WRITE setSelected)

public:
	static QString tvStyleSheet;
	bool isActiveSelection() { return m_activeSelection; };
	bool isSelected() { return m_selected; };
	void setActiveSelection(bool activeSelection) { m_activeSelection = activeSelection; };
	void setSelected(bool selected) { m_selected = selected; };

	explicit TabView(QWidget *parent = 0);
	~TabView();

	QGroupBox *grBox() { return m_gb; }
//	QTableView *tv() const		{ return m_tv; }
//	void setTv(QTableView *tv)	{ m_tv = tv; }
//	bool isActiveSelected()			{ return property("activeSel").toBool(); }
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
//	QTableView			*m_tv;
	QGroupBox			 *m_gb;
	QList<QAction *>    tblActs;
	QAction
	*actInsertRow, *actDeleteRow, *actFieldStrategy, *actRowStrategy, *actManualStrategy,
	*actSubmit, *actRevert, *actSelect;
	bool					m_activeSelection, m_selected;
};


#endif // TABVIEW_H
