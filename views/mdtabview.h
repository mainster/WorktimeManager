#ifndef MDTABVIEW_H
#define MDTABVIEW_H

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
/*                             class MdTabView                              */
/* ======================================================================== */
class MdTabView : public QTableView {

	Q_OBJECT
	Q_PROPERTY(bool selected READ isSelected WRITE setSelected NOTIFY selectedChanged)
	Q_PROPERTY(QString sqlTableName READ sqlTableName WRITE setSqlTableName NOTIFY sqlTableNameChanged)

public:
	explicit MdTabView(QWidget *parent = 0) : QTableView(parent) {
		MdTabView(QString(), parent);
	}
	explicit MdTabView(const QString &tableName, QWidget *parent = 0);
	~MdTabView() { }

	void createForeignModel(const QString &tNam);
	void onUpdateWriteActions();
	bool isSelected();
	QString &sqlTableName()		{ return m_sqlTableName; }
	void setSqlTable(const QString &tableName);

public slots:
	void setSelected(bool selected = true);
	void clearSelected();

signals:
	void sqlTableNameChanged(const QString &name);
	void selectedChanged(bool isSelected);

protected:
	void setAlternateRowCol(QColor &col, bool alternateEnabled);
	void storeFont(QFont f);
	void storeActionState(QAction *sender);
	QList<QAction *> createActions();
	void connectActions();
	void setSqlTableName(const QString &name) {
		m_sqlTableName = name;
		emit sqlTableNameChanged(name);
	}

protected slots:
	QFont restoreFont();
	void resizeRowsColsToContents();

private:
	void setStyleSheet(const QString& styleSheet) {
		QWidget::setStyleSheet( styleSheet );
		style()->unpolish(this);
		style()->polish(this);
		update();
	}
	void resetStyleSheet() {
		setStyleSheet(styleSheet());
		style()->unpolish(this);
		style()->polish(this);
	}
	void clearStyleSheet() {
		style()->unpolish(this);
	}
//	void showEvent(QShowEvent *);
//	void mouseDoubleClickEvent(QMouseEvent *);
//	bool eventFilter(QObject *obj, QEvent *event);
//	void hideEvent(QHideEvent *);

	static QString		m_stylesheet;
	SectionMask			*m_sectMsk;
	bool					m_selected;
	QString				m_sqlTableName;
	QGroupBox			*m_gb;

	QActionGroup		*actGrStrategy, *actGrContext;
	QAction				*actInsertRow, *actDeleteRow, *actFieldStrategy,
	*actRowStrategy, *actManualStrategy,*actSubmit, *actRevert, *actSelect, *actSectionMask;
	static const QString
	StyleSheet_QGroupBox, StyleSheet_QTableView;

};

#endif // MDTABVIEW_H
