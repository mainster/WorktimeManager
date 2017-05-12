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
#include "sqlrtm.h"
#include "sfiltmdl.h"
#include "datetimerangemask.h"
#include "mdtableinfo.h"
#include "sectionmask.h"
#define SET_STYLESHEETS

/* ======================================================================== */
/*                             class MdTabView                              */
/* ======================================================================== */
class MdTabView : public QTableView {

	Q_OBJECT
	Q_PROPERTY(QString sqlTableName READ sqlTableName WRITE setSqlTableName NOTIFY sqlTableNameChanged)

public:
	enum ModelType {
		Model_Unknown = 0x00,
		Model_SqlRtm = 0x01,
		Model_SfiltMdl = 0x02,
		Model_BaseClass = 0x04,
	};
	Q_ENUM(ModelType)

	/* ======================================================================== */
	/*                           MdTabView::MdTabView                           */
	/* ======================================================================== */
	explicit MdTabView(SfiltMdl *proxyModel, QWidget *parent);
	explicit MdTabView(const QString &tableName/* = QString()*/, QWidget *parent = 0);
	~MdTabView() {}

	ModelType mModelType;
	void setSelectionMode(QAbstractItemView::SelectionMode mode) {
		QTableView::setSelectionMode(mode);
	}
	QItemSelectionModel *selectionModel() {
		return QTableView::selectionModel();
	}

	/* ======================================================================== */
	/*                            Getters / Setters                             */
	/* ======================================================================== */
	QString &sqlTableName()		{ return m_sqlTableName; }
	void setSqlTableName(const QString &name);
	SqlRtm *clearMdlSrces();
	void createForeignModel(const QString &tableName);
	QAction *getActSectionMask() const { return actSectionMask; }
	QAction *getActDateTimeRngMsk() const { return actDateTimeRngMsk; }
	void setActSectionMask(QAction *value) { actSectionMask = value; }
	QPointer<SqlRtm> getSqlRtm() const { return sqlRtm; }
	QAction *getActSubmit() const { return actSubmit; }
	QAction *getActSumSelection() const { return actSumSelection; }
	void setModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;
	QMessageBox msgBoxSure;

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
	SqlRtm *sqlRtmCast() {
		Q_ASSERT(qobject_cast<SqlRtm *>(model()));
		return qobject_cast<SqlRtm *>(model());
	}
	SfiltMdl *sfiltMdlCast() {
		Q_ASSERT(qobject_cast<SfiltMdl *>(model()));
		return qobject_cast<SfiltMdl *>(model());
	}
	void initMsgBoxes() {
		msgBoxSure.setIcon(QMessageBox::Warning);
		msgBoxSure.setTextFormat(Qt::RichText);
		msgBoxSure.setStandardButtons(QMessageBox::Cancel | QMessageBox::Yes);
		msgBoxSure.setButtonText(QMessageBox::Cancel, tr("&Abbrechen"));
		msgBoxSure.setButtonText(QMessageBox::Yes, tr("&Ja"));
		msgBoxSure.setDefaultButton(QMessageBox::Cancel);
	}
	bool clearRecords(bool yesDoIt = false) {
		if (! yesDoIt) {
			msgBoxSure.setWindowTitle(tr("Alle SQL-Einträge löschen?!"));
			msgBoxSure.setText(tr("Sollen wirklich alle Zeilen aus Tabelle \"%1\" gelöscht werden?\n"
										 "Direkter Datenbank-Zugriff, <b>kein STRG+z möglich!</b>")
									 .arg(Md::tableAlias[sqlTableName()]));

			if (msgBoxSure.exec() != QMessageBox::Yes)
				return false;
		}

		QSqlQuery q(DbController::db());
		q.prepare(tr("DELETE FROM %1").arg(sqlTableName()));
		q.exec();

		sqlRtm->select();

		if (q.lastError().type() != QSqlError::NoError) {
			INFO << q.lastError().text();
			q.finish();
			return false;
		}

		q.finish();
		return true;
	}
	void onActDropTableTrigd() {
		msgBoxSure.setWindowTitle(tr("Tabelle aus SQL-Datenbank löschen?!"));
		msgBoxSure.setText(tr("Soll Tabelle \"%1\" wirklich aus der SQL-Datenbank "
									 "gelöscht werden?\nDirekter Datenbank-Zugriff, "
									 "<b>kein STRG+z möglich!</b>")
								 .arg(Md::tableAlias[sqlTableName()]));

		if (msgBoxSure.exec() != QMessageBox::Yes)
			return;

		DbController::dropTable(sqlTableName());
	}
	QList<QString> copy(bool overwriteClipboardBuffer = true);
	void paste();
	void sumSelection();

signals:
	void sqlTableNameChanged(const QString &name);
	void viewportMouseButtonPress(MdTabView *sender);
	void sumOfSelection(const QVariant sum, const char *slot);

protected:
	QList<QAction *> createActions();
	virtual void showEvent(QShowEvent *) override;
	virtual void mouseDoubleClickEvent(QMouseEvent *e) override;
	void hideEvent(QHideEvent *) override;
	void wheelEvent(QWheelEvent *event) override;
	void mousePressEvent(QMouseEvent *e) override;
	void focusOutEvent(QFocusEvent *) override;
	void restoreColumnOrderAndVisability2();
	void refreshStylesheet(QString stylesheet = QString());

protected slots:
	void onActGrStrategyTrigd(QAction *sender);
	void onActGrContextTrigd(QAction *sender);
	void storeActionState(QAction *sender);
	bool restoreActionObjects();
	void selectionChanged(const QItemSelection &selected,
								 const QItemSelection &deselected) override;

private slots:

private:
	QString				m_sqlTableName;
	QActionGroup		*actGrStrategy, *actGrContext;

	QAction *actInsertRow, *actDeleteRow, *actFieldStrategy, *actRowStrategy,
	*actManualStrategy,*actSubmit, *actRevert, *actSelect, *actSectionMask,
	*actDateTimeRngMsk, *actClearRecords, *actDropTable, *actCopy, *actPaste,
	*actSumSelection;

	QMap<QString, QString> stylesheetPvs;
	QClipboard *clipboard;

#ifdef USE_QPOINTER
	QPointer<SqlRtm> sqlRtm;
#else
	SqlRtm *sqlRtm;
#endif
	static const QString	StyleSheet_QTableView;

};



#endif // MDTABLE_H
