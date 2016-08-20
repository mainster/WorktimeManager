#include "ui_tabview.h"
#include "tabview.h"

class Browser;
class SectionMask;


/* ======================================================================== */
/*                             TabView::TabView                             */
/* ======================================================================== */
TabView::TabView(QWidget *parent) : QWidget(parent),
	ui(new Ui::TabView) {
	ui->setupUi(this);

	addActions( createActions() );
	connectActions();

	connect(this, &TabView::objectNameChanged,	this, &TabView::onObjectNameChanged);

	m_gb = ui->gb;
	m_tv = ui->mtv;

	/*! HACK to force redrawing if dynamic property stylesheets are used! */
	m_gb->setStyleSheet(this->m_gb->styleSheet());

	m_tv->setSortingEnabled( true );
	clearSelected();
	//	setActiveSelected( false );
}
TabView::~TabView() {
	delete ui;
}
void TabView::refreshView() {
	this->update();
}
/* ======================================================================== */
/*                           SQL record strategy                            */
/* ======================================================================== */
QAction *TabView::fieldStrategyAction() const {
	return tblActs[ tblActs.indexOf(actFieldStrategy) ];
}
QAction *TabView::rowStrategyAction() const {
	return tblActs[ tblActs.indexOf(actRowStrategy) ];
}
QAction *TabView::manualStrategyAction() const {
	return tblActs[ tblActs.indexOf(actManualStrategy) ];
}
QList<QAction *> TabView::getTblActs() const
{
	return tblActs;
}
void TabView::insertRow() {
	QModelIndex insertIndex = m_tv->currentIndex();
	int row = insertIndex.row();
	(row != -1)
			? insertIndex.row()
			: 0;

	m_tv->model()->insertRow(row);
	insertIndex = m_tv->model()->index(row, 0);
	m_tv->setCurrentIndex(insertIndex);
	m_tv->edit(insertIndex);
}
void TabView::deleteRow() {
	QModelIndexList currentSelection =
			m_tv->selectionModel()->selectedIndexes();

	for (int i = 0; i < currentSelection.count(); ++i) {
		if (currentSelection.at(i).column() != 0)
			continue;

		m_tv->model()->removeRow(currentSelection.at(i).row());
	}

	onUpdateActions();
}
void TabView::onUpdateActions() {
	/**
	  * DEPRECATED!!!
	  * Loop thru all available TableViews
	  *
	  * Instead of loop all TableViews, this methode should be connected
	  * to Browser::updateActions()
	  */

	SqlRtm *rtm = static_cast<SqlRtm *>(m_tv->model());
	/*!
	  * Check if cast of underlying table model is valid.
	  */
	if (! rtm)
		tr("cast failed: SqlRtm *rtm = static_cast<SqlRtm *>( %1 );")
				.arg(m_tv->model()->metaObject()->className());

	bool enableIns = (bool) rtm,
			enableDel = (bool) (enableIns && m_tv->currentIndex().isValid());

	int k = 0;
	tblActs[k++]->setEnabled( enableIns );
	tblActs[k++]->setEnabled( enableDel );

	for (;k < tblActs.length(); k++)
		tblActs[k]->setEnabled( (bool) rtm );


	if (rtm) {
		QSqlRelationalTableModel::EditStrategy es = rtm->editStrategy();
		fieldStrategyAction()->setChecked(
					es == QSqlRelationalTableModel::OnFieldChange);
		rowStrategyAction()->setChecked(
					es == QSqlRelationalTableModel::OnRowChange);
		manualStrategyAction()->setChecked(
					es == QSqlRelationalTableModel::OnManualSubmit);
	}

}

void TabView::onActInsertRowtriggered() { insertRow(); }
void TabView::onActDeleteRowtriggered() { deleteRow(); }
void TabView::onActFieldStrategytriggered() {
	QSqlTableModel *tm =
			qobject_cast<QSqlTableModel *>(m_tv->model());

	if (tm)
		tm->setEditStrategy(QSqlTableModel::OnFieldChange);
	onActSelect();
}
void TabView::onActRowStrategytriggered() {
	QSqlTableModel *tm = qobject_cast<QSqlTableModel *>(m_tv->model());

	if (tm)
		tm->setEditStrategy(QSqlTableModel::OnRowChange);
	else
		CRIT << tr("cast failed");

	QWidget *wid = focusWidget();
	INFO << wid->objectName();
}
void TabView::onActManualStrategytriggered() {
	QSqlTableModel *tm = qobject_cast<QSqlTableModel *>(m_tv->model());

	if (tm)
		tm->setEditStrategy(QSqlTableModel::OnManualSubmit);
}
void TabView::onActSubmittriggered() {
	QSqlTableModel *tm = qobject_cast<QSqlTableModel *>(m_tv->model());

	if (tm)
		tm->submitAll();
}
void TabView::onActReverttriggered() {
	QSqlTableModel *tm = qobject_cast<QSqlTableModel *>(m_tv->model());

	if (tm)
		tm->revertAll();
}
void TabView::onActSelect() {

	QSqlTableModel *tm = qobject_cast<QSqlTableModel *>(m_tv->model());

	if (tm)
		tm->select();
}

void TabView::onActSectionMask(bool sectionMask) {
	QAbstractItemModel *aim = m_tv->model();

	if (! sectionMask) {
		mSectMsk->storeColumnConfig(tv());
		delete mSectMsk;
	}
	else {
		QList<QString> colHead;
		for (int k = 0; k < aim->columnCount(); k++)
			colHead << aim->headerData(k, Qt::Horizontal, Qt::DisplayRole).toString();

		grBox()->layout()->removeWidget(m_tv);
		mSectMsk = new SectionMask(tv(), this);

		qobject_cast<QVBoxLayout *>(grBox()->layout())->addWidget(mSectMsk);
		qobject_cast<QVBoxLayout *>(grBox()->layout())->addWidget(m_tv);
		qobject_cast<QVBoxLayout *>(grBox()->layout())->setStretchFactor(m_tv, 15);
	}
}
void TabView::onObjectNameChanged(const QString &objNam) {
	m_gb->setTitle( objNam );
	//	m_gb->setAccessibleName(tr("gboxOf")+objNam);
	//	m_tv->setAccessibleName(objNam);
}
void TabView::onSectionMoved(int logicalIdx, int oldVisualIdx, int newVisualIdx) {
	INFO << tr("[ %1 ]:").arg(sqlTableName())
		  << tr("logicalIdx: %1, oldVisualIdx: %2, newVisualIdx %3")
			  .arg(logicalIdx).arg(oldVisualIdx).arg(newVisualIdx);

}
void TabView::onSqlTableNameChanged(const QString &name) {
	WARN << name;
}
void TabView::setEditTriggers(QTableView::EditTriggers triggers) {
	m_tv->setEditTriggers(triggers);
}
/* ======================================================================== */
/*                              Init methodes                               */
/* ======================================================================== */
void TabView::restoreView() {
	/** Make column/row position movable by dragging */
	m_tv->horizontalHeader()->setSectionsMovable(true);
	m_tv->verticalHeader()->setSectionsMovable(true);

	/** Restore alternating row color */
	QSETTINGS;
	bool altOnOff = config.value(objectName() + "/AlternateRowColEnable", "true").toBool();
	QColor col = config.value(objectName() + "/AlternateRowColor", "").value<QColor>();
	if (col.isValid())
		setAlternateRowCol( col, altOnOff);
}
QList<QAction *> TabView::createActions() {
	actInsertRow =		new QAction(tr("&Insert Row"), this);
	actDeleteRow =		new QAction(tr("&Delete Row"), this);
	actFieldStrategy =	new QAction(tr("Submit on &Field Change"), this);
	actRowStrategy =		new QAction(tr("Submit on &Row Change"), this);
	actManualStrategy = new QAction(tr("Submit &Manually"), this);
	actSubmit =			new QAction(tr("&Submit All"), this);
	actRevert =			new QAction(tr("Re&vert All"), this);
	actSelect =			new QAction(tr("S&elect"), this);
	actSectionMask =	new QAction(tr("Spaltenmaske"), this);

	actFieldStrategy->setCheckable(true);
	actRowStrategy->setCheckable(true);
	actManualStrategy->setCheckable(true);
	actSectionMask->setCheckable(true);
	actInsertRow->setEnabled(false);
	actDeleteRow->setEnabled(false);

	tblActs = findChildren<QAction *>(QString(), Qt::FindDirectChildrenOnly);

	QAction *sep1 = new QAction(this);
	sep1->setSeparator(true);

	tblActs.insert(tblActs.length() - 1, sep1);
	return tblActs;
}
void TabView::connectActions() {
	connect(actSectionMask, &QAction::toggled, this, &TabView::onActSectionMask);
}
/* ======================================================================== */
/*                             Helper methodes                              */
/* ======================================================================== */
void TabView::setAlternateRowCol(QColor &col, bool alternateEnabled) {
	m_tv->setAlternatingRowColors(alternateEnabled);

	if (alternateEnabled && col.isValid()) {
		m_tv->setStyleSheet(
					QString("alternate-background-color: rgba(%1,%2,%3,%4);")
					.arg( col.red())
					.arg( col.green())
					.arg( col.blue())
					.arg( col.alpha()) );
		QSETTINGS;
		config.setValue(objectName() + "/AlternateRowColEnable", alternateEnabled);
		config.setValue(objectName() + "/AlternateRowColor", col);
	}
}
void TabView::resizeRowsColsToContents() {
	m_tv->setVisible( false );
	m_tv->resizeColumnsToContents();
	m_tv->resizeRowsToContents();
	m_tv->setVisible( true );
}
void TabView::setColumnHidden(const int column, const bool hide) {
	m_tv->setColumnHidden(column, hide);
}
void TabView::setModel(QAbstractItemModel *model) {
	tv()->setModel(model);
}


