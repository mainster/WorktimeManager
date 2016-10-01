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
#include <QPointer>

#include "globals.h"
#include "connectionwidget.h"
#include "dbcontroller.h"
#include "models.h"
#include "sectionmask.h"
#include "mdtableinfo.h"


/* ======================================================================== */
/*                              class MdTabView                               */
/* ======================================================================== */
#define SET_STYLESHEETS

class MdTabView : public QTableView {

	Q_OBJECT
	Q_PROPERTY(QString sqlTableName READ sqlTableName WRITE setSqlTableName NOTIFY sqlTableNameChanged)

public:
	explicit MdTabView(const QString &tableName = QString(), QWidget *parent = 0);
	~MdTabView() {}

	void setSelectionMode(QAbstractItemView::SelectionMode mode) {
		QTableView::setSelectionMode(mode);
	}
	QItemSelectionModel *selectionModel() {
		return QTableView::selectionModel();
	}

	/* ======================================================================== */
	/*                            Getters / Setters                             */
	/* ======================================================================== */
	//	MdTabView *tv() const;
	QString &sqlTableName()		{ return m_sqlTableName; }
	void setSqlTableName(const QString &name);
	SqlRtm *clearMdlSrces();
	void createForeignModel(const QString &tableName);
	QAction *getActSectionMask() const { return actSectionMask; }
	void setActSectionMask(QAction *value) { actSectionMask = value; }

public slots:
	QFont restoreFont();
	void deleteRow();
	void insertRow();
	void onSectionMoved(int logicalIdx, int oldVisualIdx, int newVisualIdx);
	void onUpdateWriteActions();
	void refreshView();
	void removeColumnsConfig();
	void resetColumnsDefaultPos(bool allVisible);
	void resizeRowsColsToContents();
	void restoreColumnOrderAndVisability();
	void restoreView();
	void setAlternateRowCol(QColor &color, bool alternateEnabled = true);
	void setGridColor(QColor &color);
	void setColumnHidden(const int column, const bool hide);
	void setEditTriggers(QTableView::EditTriggers triggers);
	void storeFont(QFont font);
	SqlRtm *modelCast() {
		Q_ASSERT(qobject_cast<SqlRtm *>(model()));
		return qobject_cast<SqlRtm *>(model());
	}
	bool clearRecords() {
		QSqlQuery query(QString("DELETE * FROM %1").arg(sqlTableName()));
		return query.exec();
	}

signals:
	void sqlTableNameChanged(const QString &name);
	void viewportMouseButtonPress(MdTabView *sender);

protected:
	QList<QAction *> createActions();
	virtual void showEvent(QShowEvent *) override;
	virtual void mouseDoubleClickEvent(QMouseEvent *e) override;
	void hideEvent(QHideEvent *);
	void restoreColumnOrderAndVisability2();
	void wheelEvent(QWheelEvent *event);
	void mousePressEvent(QMouseEvent *e);
	void refreshStylesheet(QString stylesheet = QString());

protected slots:
	void onActGrStrategyTrigd(QAction *sender);
	void onActGrContextTrigd(QAction *sender);
	void storeActionState(QAction *sender);
	bool restoreActionObjects();

private:
	QString				m_sqlTableName;
	QActionGroup		*actGrStrategy, *actGrContext;

	QAction *actInsertRow, *actDeleteRow, *actFieldStrategy, *actRowStrategy,
	*actManualStrategy,*actSubmit, *actRevert, *actSelect, *actSectionMask;

	QMap<QString, QString> stylesheetPvs;

#ifdef USE_QPOINTER
	QPointer<SqlRtm> sqlRtm;
#else
	SqlRtm *sqlRtm;
#endif
	static const QString	StyleSheet_QTableView;
};



#endif // MDTABLE_H
