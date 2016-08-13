#include "ui_tabview.h"
#include "tabview.h"
#include "globals.h"
#include <QColor>


class Browser;

TabView::TabView(QWidget *parent)
    : QWidget(parent), ui(new Ui::TabView) {
    ui->setupUi(this);

    //    connect( Browser::instance(),   &Browser::updateActions,
    //             this,                  &TabView::updateActions);

	addActions( createActions() );

	connect( this,    SIGNAL(objectNameChanged(QString)),
				this,    SLOT(onObjectNameChanged(QString)));
	connect( this,    SIGNAL(onSelChanged(TabView*)),
				this,    SLOT(onTabViewSelChanged(TabView*)));

    this->m_gb = ui->gb;
    this->m_tv = ui->mtv;
    this->m_gb->setProperty("select",false);
    /**
    * This could be seen as "re-render functionality" due
    * to stylesheet changes
    */
    this->m_gb->setStyleSheet(this->m_gb->styleSheet());

    this->m_tv->setSortingEnabled( true );
	 setActiveSelected( false );
}
TabView::~TabView() {
    delete ui;
}

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
void TabView::refreshView() {
    this->update();
}
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
void TabView::onActInsertRowtriggered() {
	insertRow();
}
void TabView::onActDeleteRowtriggered() {
	deleteRow();
}
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
void TabView::onObjectNameChanged(const QString &objNam) {
    m_gb->setTitle( objNam );
    m_gb->setAccessibleName(tr("gboxOf")+objNam);
    m_tv->setAccessibleName(objNam);
}
QTableView *TabView::tv() const {
    return m_tv;
}
void TabView::setGrBoxTitle(QString s) {
    m_gb->setTitle(s);
    ui->gb = m_gb;
}
QGroupBox *TabView::grBox() const {
    return m_gb;
}
void TabView::onTabViewSelChanged(TabView *tv) {
    /**
    * If its not me, reset the StyleSheet for this instance
    */
    if (tv != NULL) {
        if (this->objectName().contains(tv->objectName()))  {
				setActiveSelected( true );
            return;
        }
        m_gb->setProperty("select", false);
        m_gb->setStyleSheet(m_gb->styleSheet());
		  setActiveSelected( false );
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
void TabView::setSelectionMode(QAbstractItemView::SelectionMode mode) {
	m_tv->setSelectionMode(mode);
}
QItemSelectionModel *TabView::selectionModel() {
	return m_tv->selectionModel();
}
void TabView::setModel(QAbstractItemModel *model) {
	m_tv->setModel(model);
}
void TabView::setEditTriggers(QTableView::EditTriggers triggers) {
	m_tv->setEditTriggers(triggers);
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

	actFieldStrategy->setCheckable(true);
	actRowStrategy->setCheckable(true);
	actManualStrategy->setCheckable(true);
	actInsertRow->setEnabled(false);
	actDeleteRow->setEnabled(false);

	tblActs = findChildren<QAction *>(QString(), Qt::FindDirectChildrenOnly);
	return tblActs;
}

/* ---------------------------------------------------------------- */
/*                      Helper methodes                          */
/* ---------------------------------------------------------------- */
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


