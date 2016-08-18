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
	Q_PROPERTY(bool isSelected READ isSelected WRITE setSelect NOTIFY selectChanged)
	Q_PROPERTY(QString sqlTableName READ sqlTableName WRITE setSqlTableName NOTIFY sqlTableNameChanged)

public:
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

	static QString tvStyleSheet;

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
	void setGrBoxTitle(QString s);
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
	void sqlTableNameChanged(const QString &name);
	void selectChanged(bool isSelected);


protected:
	QList<QAction *> createActions();

protected slots:
	void onClearSelection() {

	}

private:
	Ui::TabView			*ui;
	QTableView        *m_tv;
	QGroupBox         *m_gb;
	QList<QAction *>  tblActs;
	QString				m_sqlTableName;
	bool					m_select;
	QAction				*actInsertRow, *actDeleteRow, *actFieldStrategy,
	*actRowStrategy, *actManualStrategy,*actSubmit, *actRevert, *actSelect;
};


#endif // TABVIEW_H
