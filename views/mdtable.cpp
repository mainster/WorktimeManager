#include "ui_mdtable.h"
#include "mdtable.h"

class Browser;
class SectionMask;

/* ======================================================================== */
/*                             MdTable::MdTable                             */
/* ======================================================================== */
MdTable::MdTable(QWidget *parent) : QWidget(parent),
	ui(new Ui::MdTable) {
	ui->setupUi(this);

	addActions( createActions() );
	connectActions();

	connect(this,			&MdTable::objectNameChanged,
			  this,			&MdTable::onObjectNameChanged);

	m_gb = new QGroupBox(this);
	m_tv = new QTableView(this);
	m_gb->setObjectName(tr("gb"));
	m_tv->setObjectName(tr("mtv"));

	layout()->addWidget(m_gb);

	QVBoxLayout *vbl = new QVBoxLayout(m_gb);
	vbl->addWidget(m_tv);

	m_gb->setLayout(vbl);
//	m_gb->setParent(ui->gb->parentWidget());
//	m_tv->setParent(ui->mtv->parentWidget());

	/*! HACK to force redrawing if dynamic property stylesheets are used! */
#ifdef SET_STYLESHEETS
	m_gb->setStyleSheet(this->m_gb->styleSheet());
#endif
	m_tv->setSortingEnabled( true );
	clearSelected();

	m_tv->installEventFilter(this);
	m_gb->installEventFilter(this);

	connect(m_tv->horizontalHeader(),	&QHeaderView::sectionMoved,
			  this,								&MdTable::onSectionMoved);

	restoreFont();

	QTimer::singleShot(500, this, SLOT(restoreActionObjects()));
	//	QTimer::singleShot(8000, this, SLOT(restoreActionObjects()));
	//	setActiveSelected( false );
}
MdTable::~MdTable() {
	delete ui;
}
void MdTable::refreshView() {
	this->update();
}
/* ======================================================================== */
/*                           SQL record strategy                            */
/* ======================================================================== */
void MdTable::insertRow() {
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
void MdTable::deleteRow() {
	QModelIndexList currentSelection =
			m_tv->selectionModel()->selectedIndexes();

	for (int i = 0; i < currentSelection.count(); ++i) {
		if (currentSelection.at(i).column() != 0)
			continue;

		m_tv->model()->removeRow(currentSelection.at(i).row());
	}

	onUpdateWriteActions();
}
void MdTable::setEditTriggers(QTableView::EditTriggers triggers) {
	m_tv->setEditTriggers(triggers);
}
void MdTable::onActGrStrategyTrigd(QAction *sender) {
	if (! static_cast<SqlRtm *>(m_tv->model()))
		return;

	if ((sender != actFieldStrategy) &&
		 (sender != actRowStrategy) &&
		 (sender != actManualStrategy))
		qReturn("Bad sender detected!");

	SqlRtm *rtm = static_cast<SqlRtm *>(m_tv->model());

	if (sender == actFieldStrategy)
		rtm->setEditStrategy(QSqlTableModel::OnFieldChange);

	if (sender == actRowStrategy)
		rtm->setEditStrategy(QSqlTableModel::OnRowChange);

	if (sender == actManualStrategy)
		rtm->setEditStrategy(QSqlTableModel::OnManualSubmit);
}
void MdTable::onActGrContextTrigd(QAction *sender) {
	if (! static_cast<SqlRtm *>(m_tv->model()))
		return;

	if ((sender != actSubmit) && (sender != actRevert) &&
		 (sender != actSelect) && (sender != actInsertRow) &&
		 (sender != actDeleteRow))
		qReturn("Bad sender detected!");

	SqlRtm *rtm = static_cast<SqlRtm *>(m_tv->model());

	if (sender == actSubmit)	rtm->submitAll();
	if (sender == actRevert)	rtm->revertAll();
	if (sender == actSelect)	rtm->select();
	if (sender == actInsertRow) insertRow();
	if (sender == actDeleteRow) deleteRow();
}
void MdTable::onUpdateWriteActions() {
	/*!
	 * Check if there is a valid model set before calling update procedure.
	 */

	SqlRtm *rtm = static_cast<SqlRtm *>(m_tv->model());
	if (! rtm)	qReturn(tr("cast failed: SqlRtm *rtm = static_cast<SqlRtm *>( %1 );")
							  .arg(m_tv->model()->metaObject()->className()));

	//	INFO << m_sqlTableName << tr("currentIndexIsValied:")
	//		  << m_tv->currentIndex().isValid();

	actInsertRow->setEnabled(m_tv->currentIndex().isValid());
	actDeleteRow->setEnabled(m_tv->currentIndex().isValid());
}
void MdTable::onActSectionMask(bool sectionMask) {
	QAbstractItemModel *aim = m_tv->model();

	if (! sectionMask) {
		mSectMsk->storeVisibleCols(/*tv()*/);
		delete mSectMsk;
	}
	else {
		QList<QString> colHead;
		for (int k = 0; k < aim->columnCount(); k++)
			colHead << aim->headerData(k, Qt::Horizontal, Qt::DisplayRole).toString();

		grBox()->layout()->removeWidget(m_tv);
		mSectMsk = new SectionMask(tv(), this);
		connect(mSectMsk, &SectionMask::dblClickChecked, actSectionMask, &QAction::setChecked);
		qobject_cast<QVBoxLayout *>(grBox()->layout())->addWidget(mSectMsk);
		qobject_cast<QVBoxLayout *>(grBox()->layout())->addWidget(m_tv);
		qobject_cast<QVBoxLayout *>(grBox()->layout())->setStretchFactor(m_tv, 15);
	}
}
void MdTable::onSectionMoved(int logicalIdx, int oldVisualIdx, int newVisualIdx) {
	Q_UNUSED(oldVisualIdx);
	qobject_cast<SqlRtm *>(tv()->model())->setSectionIdx(logicalIdx, newVisualIdx);
	modelCast()->storeModel(sqlTableName());
}
void MdTable::onSqlTableNameChanged(const QString &name) {
	WARN << name;
}
void MdTable::onObjectNameChanged(const QString &objNam) {
	m_gb->setTitle( objNam );
	//	m_gb->setAccessibleName(tr("gboxOf")+objNam);
	//	m_tv->setAccessibleName(objNam);
}
/* ======================================================================== */
/*                              Init methodes                               */
/* ======================================================================== */
void MdTable::restoreView() {
	/** Make column/row position movable by dragging */
	m_tv->horizontalHeader()->setSectionsMovable(true);
	m_tv->verticalHeader()->setSectionsMovable(true);

	/** Restore alternating row color */
	QSETTINGS;
	QColor color = config.value(objectName() + Md::k.alterRowColor,
										 QColor(Qt::white)).value<QColor>();
	if (color.isValid())
		setAlternateRowCol(
					color, config.value(objectName() + Md::k.altRowColOn, "true").toBool());

	/*!	Restore model sources	*/
}
void MdTable::restoreColumnOrderAndVisability() {
	//!< Restore the models source members
	modelCast()->restoreModelSrcsFromIni(sqlTableName());

	//!< restoreVisibleCols(); takes access into models column-show/hide source list
	SectionMask *sm = new SectionMask(tv(), this);
	sm->restoreVisibleCols();
	delete sm;

	//!< modelCast()->sectionIdxs(); takes the section order list from model source...
	QList<int> sectIdxs = modelCast()->sectionIdxs();

	//!< ... and iterates over all elements so the restore process is complete
	for (int k = 0; k < sectIdxs.length(); k++)
		tv()->horizontalHeader()->moveSection(k, sectIdxs.at(k));

}
QList<QAction *> MdTable::createActions() {
	actInsertRow =		new QAction(tr("&Insert Row"), this);
	actDeleteRow =		new QAction(tr("&Delete Row"), this);
	actSubmit =			new QAction(tr("&Submit All"), this);
	actRevert =			new QAction(tr("Re&vert All"), this);
	actSelect =			new QAction(tr("S&elect"), this);
	actSectionMask =	new QAction(tr("Spaltenmaske"), this);
	actFieldStrategy =	new QAction(tr("Submit on &Field Change"), this);
	actRowStrategy =		new QAction(tr("Submit on &Row Change"), this);
	actManualStrategy =	new QAction(tr("Submit &Manually"), this);

	/* ======================================================================== */
	/*                             Action grouping                              */
	/* ======================================================================== */
	/*!
	 * Create action group for strategy actions
	 */
	actGrStrategy = new QActionGroup(this);
	PONAM(actGrStrategy)->setExclusive(true);

	QList<QAction *> acts;
	acts << PONAM(actFieldStrategy) << PONAM(actRowStrategy)
		  << PONAM(actManualStrategy);

	foreach (QAction *act, acts) {
		actGrStrategy->addAction(act);
		act->setCheckable(true);
	}

	/*!
	 * Connect action group triggered signal to a common slot.
	 */
	connect(actGrStrategy, &QActionGroup::triggered, this, &MdTable::storeActionState);
	connect(actGrStrategy, &QActionGroup::triggered, this, &MdTable::onActGrStrategyTrigd);

	/* ======================================================================== */

	/*!
	 * Create action group for context menu actions.
	 */
	actGrContext = new QActionGroup(this);
	PONAM(actGrContext)->setExclusive(false);

	QList<QAction *> acts2;
	acts2 << PONAM(actInsertRow) << PONAM(actDeleteRow) << PONAM(actSubmit)
			<< PONAM(actRevert)  << PONAM(actSelect);

	foreach (QAction *act, acts2)
		actGrContext->addAction(act);

	/*!
	 * Connect action group triggered signal to a save-action-state slot.
	 */
	connect(actGrContext, &QActionGroup::triggered, this, &MdTable::storeActionState);
	connect(actGrContext, &QActionGroup::triggered, this, &MdTable::onActGrContextTrigd);

	/* ======================================================================== */
	/*                       Action object configurations                       */
	/* ======================================================================== */
	actSectionMask->setCheckable(true);

	//	tblActs = findChildren<QAction *>(QString(), Qt::FindDirectChildrenOnly);

	QAction *sep1 = new QAction(this);
	sep1->setSeparator(true);
	QAction *sep2 = new QAction(this);
	sep2->setSeparator(true);

	acts2 << sep1 << acts << sep2 << actSectionMask;
	return acts2;
}
void MdTable::connectActions() {
	connect(actSectionMask, &QAction::toggled, this, &MdTable::onActSectionMask);
}
/* ======================================================================== */
/*                              Event handler                               */
/* ======================================================================== */
void MdTable::showEvent(QShowEvent *) {
}
void MdTable::mouseDoubleClickEvent(QMouseEvent *) {
	INFO << m_sqlTableName << tr("double");
}
bool MdTable::eventFilter(QObject *obj, QEvent *event) {
	QMouseEvent *mouseEv = static_cast<QMouseEvent *>(event);
	if (! mouseEv)
		return QObject::eventFilter(obj, event);

	if (obj->objectName().contains(tr("mtv")))
		if (mouseEv->type() == QEvent::MouseButtonDblClick) {
			actSectionMask->trigger();
			//			INFO << tr("Double click:") << obj;
			return true;
		}
	return QObject::eventFilter(obj, event);
}
void MdTable::hideEvent(QHideEvent *) {
	modelCast()->storeModel(sqlTableName());
}
/* ======================================================================== */
/*                             Helper methodes                              */
/* ======================================================================== */
void MdTable::setAlternateRowCol(QColor &col, bool alternateEnabled) {
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
void MdTable::storeFont(QFont f) {
	QSETTINGS;
	config.setValue(objectName() + Md::k.tableFont, QVariant::fromValue(f));
	tv()->setFont(f);
}
QFont MdTable::restoreFont() {
	QSETTINGS;
	QFont f(config.value(objectName() + Md::k.tableFont).value<QFont>());
	tv()->setFont(f);
	return f;
}
void MdTable::resizeRowsColsToContents() {
	m_tv->setVisible( false );
	m_tv->resizeColumnsToContents();
	m_tv->resizeRowsToContents();
	m_tv->setVisible( true );
}
void MdTable::setColumnHidden(const int column, const bool hide) {
	m_tv->setColumnHidden(column, hide);
}
void MdTable::setModel(QAbstractItemModel *model) {
	tv()->setModel(model);
//	QTableView::viewportEntered()
}
void MdTable::storeActionState(QAction *sender) {
	QSETTINGS;

	if (sender->isCheckable()) {
		/*!
		 * If action group is exclusive, clear all action config flags.
		 */
		if (sender->actionGroup()->isExclusive())
			foreach(QAction *act, sender->actionGroup()->actions())
				config.setValue(objectName() + tr("/") + act->objectName(), false);

		config.setValue(objectName() + tr("/") + sender->objectName(), sender->isChecked());
		config.sync();
	}
}
bool MdTable::restoreActionObjects() {
	QSETTINGS;

	//	actGrStrategy->blockSignals(true);
	//	actGrContext->blockSignals(true);

	foreach (QAction *a, QList<QAction *>()
				<< actGrStrategy->actions()
				<< actGrContext->actions()) {
		if (! config.allKeys().join(',').contains(a->objectName()))
			continue;
		if (config.value(objectName() + tr("/") + a->objectName(), false).toBool()) {
			if (! a->isChecked()) {
				//				INFO << a->objectName();
				a->trigger();
			}
		}
	}

	//	actGrStrategy->blockSignals(false);
	//	actGrContext->blockSignals(false);

	return true;
}
SqlRtm *MdTable::modelCast() {
	return qobject_cast<SqlRtm *>(tv()->model());
}
void MdTable::removeColumnsConfig() {
	QSETTINGS;
	config.remove(sqlTableName() + Md::k.centerCols);
	config.remove(sqlTableName() + Md::k.visibleCols);
	config.remove(sqlTableName() + Md::k.sectionIdxs);

	config.sync();
}
void MdTable::resetColumnsDefaultPos(bool allVisible) {
	QHeaderView *h = tv()->horizontalHeader();
	h->hide();
	for (int k = 0; k < tv()->model()->columnCount(); k++) {
		h->moveSection(h->visualIndex(k), k);
		if (allVisible)	h->setSectionHidden(k, false);
	}
	h->show();
}
