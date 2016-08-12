#ifndef TABVIEW_H
#define TABVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QTableView>
#include <QSqlTableModel>
#include <QKeyEvent>
#include <QColor>

#include "models.h"

namespace Ui {
class TabView;
//class Browser;
}

class TabView : public QWidget {

	Q_OBJECT
	Q_PROPERTY(bool activeSelection READ isActiveSelected WRITE setActiveSelected)
	Q_PROPERTY(bool selected READ isSelected WRITE setSelected)

public:
	static QString tvStyleSheet;
	bool isActiveSelected()						{ return m_activeSelected; };
	void setActiveSelected(bool value)		{ m_activeSelected = value; };
	void clearActiveSelected()					{ m_activeSelected = false; };
	bool isSelected()								{ return m_selected; };
	void setSelected(bool selected)			{ m_selected = selected; };

	explicit TabView(QWidget *parent = 0);
	~TabView();

	QGroupBox *grBox() const;
	void setGrBox(QGroupBox *gb);
	QTableView *tv() const;
	void setTvTitle(QString &s);

	QList<QAction *> getTblActs() const;
	QAction *fieldStrategyAction() const;
	QAction *manualStrategyAction() const;
	QAction *rowStrategyAction() const;

public slots:
	void onObjectNameChanged(const QString &objNam);
	void setGrBoxStyleSht(const QString &ss);
	void setGrBoxTitle(QString s);
	void onTabViewSelChanged(TabView *tv);
	void setAlternateRowCol(QColor &col, bool alternateEnabled = true);
	void restoreView();

	void insertRow();
	void deleteRow();
	void onUpdateActions();
	void onActSelect();
	void onActReverttriggered();
	void onActSubmittriggered();
	void onActManualStrategytriggered();
	void onActRowStrategytriggered();
	void onActFieldStrategytriggered();
	void onActDeleteRowtriggered();
	void onActInsertRowtriggered();

	void refreshView();
	void resizeRowsColsToContents();
	void setColumnHidden(const int column, const bool hide);
	void setSelectionMode(QAbstractItemView::SelectionMode mode);
	void setModel(QAbstractItemModel *model);
	void setEditTriggers(QTableView::EditTriggers triggers);
	QItemSelectionModel *selectionModel();

signals:
	void onSelChanged(TabView *tv);

protected:
	QList<QAction *> createActions();

private:
	Ui::TabView         *ui;
	QTableView          *m_tv;
	QGroupBox           *m_gb;
	QList<QAction *>    tblActs;
	bool						m_activeSelected, m_selected;
	QAction					*actInsertRow, *actDeleteRow, *actFieldStrategy,
	*actRowStrategy, *actManualStrategy,*actSubmit, *actRevert, *actSelect;
};


#endif // TABVIEW_H
