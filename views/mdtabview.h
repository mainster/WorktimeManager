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
	Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)

public:
	explicit MdTabView(QWidget *parent = 0);
	explicit MdTabView(const QString &tableName, QWidget *parent = 0);
	~MdTabView() { }

	void createForeignModel(const QString &tNam);
	void onUpdateWriteActions();
	bool isSelected();
	QString title();

public slots:
	void setSelected(bool selected = true);
	void clearSelected();
	void setTitle(const QString &title);

signals:
	void titleChanged(const QString &name);
	void selectedChanged(bool isSelected);

protected:
	void setAlternateRowCol(QColor &col, bool alternateEnabled);
	void storeFont(QFont f);
	void storeActionState(QAction *sender);
	QList<QAction *> createActions();
	void connectActions();

protected slots:
	QFont restoreFont();
	void resizeRowsColsToContents();

private:
	void showEvent(QShowEvent *);
	void mouseDoubleClickEvent(QMouseEvent *);
	bool eventFilter(QObject *obj, QEvent *event);
	void hideEvent(QHideEvent *);

	SectionMask			*m_sectMsk;
	QLabel				*m_titleLabel;
	bool					m_selected;

	QActionGroup		*actGrStrategy, *actGrContext;
	QAction				*actInsertRow, *actDeleteRow, *actFieldStrategy,
	*actRowStrategy, *actManualStrategy,*actSubmit, *actRevert, *actSelect, *actSectionMask;
};

#endif // MDTABVIEW_H
