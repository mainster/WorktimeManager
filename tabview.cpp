//#include "ui_tabview.h"
#include "tabview.h"
#include "globals.h"
#include <QColor>


class Browser;

#define QFOLDINGSTART {
QString gbStyleSheet = QString(
								  "QGroupBox{"
								  "   	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E0E0E0, stop: 1 #FFFFFF);"
								  "   	border-radius: 5px;"
								  "  	margin-top: 1ex; /* leave space at the top for the title */"
								  "   	font: italic 9pt ""Arial"";"
								  "   	font-weight: bold;"
								  "	border: 1px solid;    "
								  " 	border-color: rgb(105, 105, 105);"
								  "   	color: black;"
								  "}"
								  "QGroupBox::title {"
								  "   	subcontrol-origin: margin; /* margin boarder padding content */"
								  "   	subcontrol-position: top center; /* position at the top center */"
								  "   	top: 1.2ex;   "
								  "  	padding: 0px 8px"
								  "} "
								  "QGroupBox::title:hover {"
								  "    color: rgba(235, 235, 235, 255);"
								  "}"
								  " QTableView{"
								  "	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #F0F0F0, stop: 1 #FFFFFF);"
								  "	border: 0px solid gray;"
								  "	border-radius: 5px;"
								  "	margin-top: 15px; /* leave space at the top for the title */"
								  " }"
								  "QTableView[select=false]{ background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #F0F0F0, stop: 1 #FFFFFF); }"
								  "QGroupBox[select=false]{ border-color: rgb(105, 105, 105); }"
								  ""
								  "QTableView[select=true]{ background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #C0C0C0, stop: 1 #FFFFFF); }"
								  "QGroupBox[select=true]{ border-color: rgb(0, 28, 170) }"
								  ""
								  ""
								  "/* Customize Table header */"
								  " QHeaderView::section {"
								  "     background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
								  "                                       stop:    0 #616161, stop: 0.5 #505050,"
								  "                                       stop: 0.6 #434343, stop:    1 #656565);"
								  "     color: white;"
								  "     padding-left: 4px;"
								  "     padding-right: 4px;"
								  "     padding-top: 2px;"
								  "     padding-bottom: 2px;"
								  "     border: 1px solid #6c6c6c;"
								  " }"
								  ""
								  " QHeaderView::section:checked {"
								  "     background-color: rgb(31, 94, 233);"
								  " }"
								  ""
								  " /* style the sort indicator */"
								  "QHeaderView::down-arrow {"
								  "/*     image: url(:/images/down_arrow.png);*/"
								  " }"
								  ""
								  " QHeaderView::up-arrow {"
								  "/*     image: url(:/images/up_arrow.png);*/"
								  " }");
#define QFOLDINGEND }

TabView::TabView(QWidget *parent) : QTableView(parent) {

	/*!
	 * Create new QGroupBox widget and set TabView parent to to it.
	 */
	m_gb = new QGroupBox(parent);
//	setParent(m_gb);
	m_gb->show();
	show();

	/*!
	 * Create GridLayout glay, add this widget and set the layout of m_gb member to glay->
	 */
	QGridLayout *glay = new QGridLayout();
	glay->addWidget(this);
	m_gb->setLayout(glay);
	m_gb->setStyleSheet(gbStyleSheet);

	/*!
	 * Add new property "select" to the groupBox.
	 */
	m_gb->setProperty("select", false);

	/*!
	 * Add new property "activeSel" to this QTabView.
	 */
	setProperty("activeSel", false);
	addActions( createActions() );
	setSortingEnabled( true );

	connect(this, &TabView::objectNameChanged, this, &TabView::onObjectNameChanged);
	connect(this, &TabView::onSelChanged, this, &TabView::onTabViewSelChanged);
}
TabView::~TabView() {
	delete m_gb;
}

void TabView::restoreView() {
	/** Make column/row position movable by dragging */
	horizontalHeader()->setSectionsMovable(true);
	verticalHeader()->setSectionsMovable(true);

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
QList<QAction *> TabView::getTblActs() const
{
	return tblActs;
}

void TabView::insertRow() {
	QModelIndex insertIndex = currentIndex();
	int row = insertIndex.row();
	(row != -1)
			? insertIndex.row()
			: 0;

	model()->insertRow(row);
	insertIndex = model()->index(row, 0);
	setCurrentIndex(insertIndex);
	edit(insertIndex);
}
void TabView::deleteRow() {
	QModelIndexList currentSelection =
			selectionModel()->selectedIndexes();

	for (int i = 0; i < currentSelection.count(); ++i) {
		if (currentSelection.at(i).column() != 0)
			continue;

		model()->removeRow(currentSelection.at(i).row());
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

	SqlRtm *rtm = static_cast<SqlRtm *>(model());
	/*!
	  * Check if cast of underlying table model is valid.
	  */
	if (! rtm)
		tr("cast failed: SqlRtm *rtm = static_cast<SqlRtm *>( %1 );")
				.arg(model()->metaObject()->className());

	bool enableIns = (bool) rtm,
			enableDel = (bool) (enableIns && currentIndex().isValid());

	int k = 0;
	tblActs[k++]->setEnabled( enableIns );
	tblActs[k++]->setEnabled( enableDel );

	for (;k < tblActs.length(); k++)
		tblActs[k]->setEnabled( (bool) rtm );


	if (rtm) {
		QSqlRelationalTableModel::EditStrategy es = rtm->editStrategy();
		actFieldStrategy->setChecked(
					es == QSqlRelationalTableModel::OnFieldChange);
		actRowStrategy->setChecked(
					es == QSqlRelationalTableModel::OnRowChange);
		actManualStrategy->setChecked(
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
			qobject_cast<QSqlTableModel *>(model());

	if (tm)
		tm->setEditStrategy(QSqlTableModel::OnFieldChange);
	onActSelect();
}
void TabView::onActRowStrategytriggered() {
	QSqlTableModel *tm = qobject_cast<QSqlTableModel *>(model());

	if (tm)
		tm->setEditStrategy(QSqlTableModel::OnRowChange);
	else
		CRIT << tr("cast failed");

	QWidget *wid = focusWidget();
	INFO << wid->objectName();
}
void TabView::onActManualStrategytriggered() {
	QSqlTableModel *tm = qobject_cast<QSqlTableModel *>(model());

	if (tm)
		tm->setEditStrategy(QSqlTableModel::OnManualSubmit);
}
void TabView::onActSubmittriggered() {
	QSqlTableModel *tm = qobject_cast<QSqlTableModel *>(model());

	if (tm)
		tm->submitAll();
}
void TabView::onActReverttriggered() {
	QSqlTableModel *tm = qobject_cast<QSqlTableModel *>(model());

	if (tm)
		tm->revertAll();
}
void TabView::onActSelect() {

	QSqlTableModel *tm = qobject_cast<QSqlTableModel *>(model());

	if (tm)
		tm->select();
}
void TabView::onObjectNameChanged(const QString &objNam) {
	m_gb->setTitle( objNam );
	m_gb->setAccessibleName(tr("gboxOf")+objNam);
	setAccessibleName(objNam);
}
void TabView::onTabViewSelChanged(TabView *tv) {
	/**
	 * If its not me, reset the StyleSheet for this instance
	 */
	if (tv != NULL) {
		if (this->objectName().contains(tv->objectName()))  {
			setProperty("activeSel", true);
			return;
		}
		m_gb->setProperty("select", false);
		m_gb->setStyleSheet(m_gb->styleSheet());
		setProperty("activeSel", false);
	}
}
void TabView::resizeRowsColsToContents() {
	setVisible( false );
	resizeColumnsToContents();
	resizeRowsToContents();
	setVisible( true );
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
	setAlternatingRowColors(alternateEnabled);

	if (alternateEnabled && col.isValid()) {
		setStyleSheet(
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


