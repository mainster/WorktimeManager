#include "mdtabview.h"

class Browser;
class SectionMask;

#define USE_TABLE_UI
/* ======================================================================== */
/*                             MdTabView::MdTabView                         */
/* ======================================================================== */
MdTabView::MdTabView(const QString &tableName, QWidget *parent)
	: QTableView(parent) {

	addActions( createActions() );

	if (!tableName.isEmpty())
		createForeignModel(tableName);

	setSortingEnabled( true );

	setSizeAdjustPolicy(QTableView::AdjustToContents);
	adjustSize();

	setStyleSheet(StyleSheet_QTableView);

	//	/*! HACK to force redrawing if dynamic property stylesheets are used! */
	//	#ifdef SET_STYLESHEETS
	//		m_gb->setStyleSheet(this->m_gb->styleSheet());
	//	#endif

	connect(horizontalHeader(), &QHeaderView::sectionMoved, this, &MdTabView::onSectionMoved);
	setContextMenuPolicy(Qt::ActionsContextMenu);

	restoreFont();

	QTimer::singleShot(500, this, SLOT(restoreActionObjects()));
	//	QTimer::singleShot(8000, this, SLOT(restoreActionObjects()));
	//	setActiveSelected( false );
}
void MdTabView::setSqlTableName(const QString &name) {
	m_sqlTableName = name;
	emit sqlTableNameChanged(m_sqlTableName);
}
void MdTabView::createForeignModel(const QString &tNam) {
	/**
		 * Get active database pointer
		 */
	QSqlDatabase pDb = ConnectionWidget::instance()->currentDb();
	SqlRtm *rmod = new SqlRtm(SqlRtm::srcNew, this, pDb);
	rmod->setEditStrategy(QSqlTableModel::OnFieldChange);
	rmod->setTable(pDb.driver()->escapeIdentifier(tNam, QSqlDriver::TableName));
	/**
		 * Because the target database contains more than one table related on
		 * foreign keys, varying process is necessary to set the different
		 * relations correctly. Which processing branch must be used depends on
		 * the table name passed by sqlTbl
		 */
	/** Processing branch for table ... */
	while (1) {
		/* --------------------------------------------------------- */
		/*                      Table worktimes                      */
		/* --------------------------------------------------------- */
		if (tNam.contains("worktime", Qt::CaseInsensitive)) {
			/** Remember the indexes of the columns */
			int colEmp = rmod->fieldIndex("workerID");
			int colPrj = rmod->fieldIndex("prjID");

			/** Set the relations to foreign database tables */
			rmod->setRelation(colEmp, QSqlRelation("worker", "workerID", "Nachname"));
			rmod->setRelation(colPrj, QSqlRelation("prj", "prjID", "Beschreibung"));

			//!< Set the localized header captions
			rmod->setHeaderData(rmod->fieldIndex("worktimID"), Qt::Horizontal, tr("ID"));
			rmod->setHeaderData(rmod->fieldIndex("dat"), Qt::Horizontal, Md::headerAlias[ "dat" ]);
			rmod->setHeaderData(rmod->fieldIndex("hours"), Qt::Horizontal, Md::headerAlias[ "hours" ]);
			rmod->setHeaderData(colEmp, Qt::Horizontal, tr("Mitarb"));
			rmod->setHeaderData(colPrj, Qt::Horizontal, tr("Beschreibung"));

			break;
		}
		/* --------------------------------------------------------- */
		/*                      Table projects                       */
		/* --------------------------------------------------------- */
		if (tNam.contains("prj", Qt::CaseInsensitive)) {
			/** Remember the indexes of the columns */
			int colClient = rmod->fieldIndex("clientID");
			int colSub = rmod->fieldIndex("subID");
			int colArch = rmod->fieldIndex("archID");

			rmod->setJoinMode(QSqlRelationalTableModel::LeftJoin);
			/** Set the relations to the other database tables */
			rmod->setRelation(colClient, QSqlRelation("client", "clientID", "Name"));
			rmod->setRelation(colSub, QSqlRelation("arch", "archID", "Name"));
			rmod->setRelation(colArch, QSqlRelation("arch", "archID", "Name"));

			//!< Set the localized header captions
			rmod->setHeaderData(rmod->fieldIndex("prjID"), Qt::Horizontal, tr("ID"));
			rmod->setHeaderData(rmod->fieldIndex("clientID"), Qt::Horizontal,
									  Md::headerAlias[ tr("%1/clientID").arg(tNam) ]);
			rmod->setHeaderData(rmod->fieldIndex("subID"), Qt::Horizontal,
									  Md::headerAlias[ tr("%1/subID").arg(tNam) ]);
			rmod->setHeaderData(rmod->fieldIndex("archID"), Qt::Horizontal,
									  Md::headerAlias[ tr("%1/archID").arg(tNam) ]);

			break;
		}
		/* --------------------------------------------------------- */
		/*                      Table worker                         */
		/* --------------------------------------------------------- */
		if (tNam.contains("worker", Qt::CaseInsensitive)) {
			/** Remember the indexes of the columns */
			int colGrading = rmod->fieldIndex("gradingID");

			/** Set the relations to foreign database tables */
			rmod->setRelation(colGrading, QSqlRelation("grading", "gradingID", "Einstufung"));

			//!< Set the localized header captions
			rmod->setHeaderData(rmod->fieldIndex("workerID"), Qt::Horizontal, tr("ID"));
			rmod->setHeaderData(rmod->fieldIndex("PersonalNr"), Qt::Horizontal,
									  Md::headerAlias[ "PersonalNr" ]);
			rmod->setHeaderData(rmod->fieldIndex("Stundensatz"), Qt::Horizontal,
									  Md::headerAlias[ "Stundensatz" ]);
			rmod->setHeaderData(rmod->fieldIndex("Wochenstunden"), Qt::Horizontal,
									  Md::headerAlias[ "Wochenstunden" ]);
			break;
		}
		/* --------------------------------------------------------- */
		/*                      Table client                         */
		/* --------------------------------------------------------- */
		if (tNam.contains("client", Qt::CaseInsensitive)) {
			//!< Set the localized header captions
			rmod->setHeaderData(rmod->fieldIndex("clientID"), Qt::Horizontal, tr("ID"));
			rmod->setHeaderData(rmod->fieldIndex("Nummer"), Qt::Horizontal,
									  Md::headerAlias[ tr("%1/Nummer").arg(tNam) ]);
			break;
		}
		/* --------------------------------------------------------- */
		/*                      Table fehlzeit                       */
		/* --------------------------------------------------------- */
		if (tNam.contains("fehlzeit", Qt::CaseInsensitive)) {
			//!< Set the localized header captions
			rmod->setHeaderData(rmod->fieldIndex("fehlID"), Qt::Horizontal, tr("ID"));

			break;
		}
		/* --------------------------------------------------------- */
		/*                      Table arch									 */
		/* --------------------------------------------------------- */
		if (tNam.contains("arch", Qt::CaseInsensitive)) {
			//!< Set the localized header captions
			rmod->setHeaderData(rmod->fieldIndex("archID"), Qt::Horizontal, tr("ID"));

			break;
		}
		/* --------------------------------------------------------- */
		/*                      Table grading								 */
		/* --------------------------------------------------------- */
		if (tNam.contains("grading", Qt::CaseInsensitive)) {
			//!< Set the localized header captions
			rmod->setHeaderData(rmod->fieldIndex("gradingID"), Qt::Horizontal, tr("ID"));

			break;
		}
		/* --------------------------------------------------------- */
		/*                      Table runtime								 */
		/* --------------------------------------------------------- */
		if (tNam.contains("runtime", Qt::CaseInsensitive)) {
			//!< Set the localized header captions
			rmod->setHeaderData(rmod->fieldIndex("runtimeID"), Qt::Horizontal, tr("ID"));

			break;
		}
		/* --------------------------------------------------------- */
		/*                      Table Unknown                        */
		/* --------------------------------------------------------- */
		QMessageBox::warning(this, tr("Warnung"),
									tr("Unbekannter SQL Tabellenbezeichner: %1")
									.arg(tNam), QMessageBox::Ok);
		break;
	}

	// Populate the model
	rmod->select();

	QTime *time = new QTime();
	time->start();
	while (rmod->canFetchMore())
		rmod->fetchMore();
	INFO << tr("Model fetch delay: %1ms").arg(time->elapsed());

	// Set the model and hide the ID column
	setSelectionMode(QAbstractItemView::ContiguousSelection);
	setModel(rmod);
	setEditTriggers( QAbstractItemView::DoubleClicked |
						  QAbstractItemView::EditKeyPressed );

	for (int k = 0; k < rmod->columnCount(); k++)
		rmod->sectionIdxs().append( k );

	if (! (bool) connect( selectionModel(),
								 SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
								 this, SLOT(currentChanged(QModelIndex,QModelIndex)) ) ) {
		//			emit stateMsg(tr("Slot connection returns false"));
		INFO << tr("Slot connection returns false");
	}
	resizeRowsColsToContents();
	//    emit updateActions();
	setSqlTableName(tNam);
	restoreColumnOrderAndVisability();
	setFont(restoreFont());
}
void MdTabView::refreshView() {
	this->update();
}
/* ======================================================================== */
/*                           SQL record strategy                            */
/* ======================================================================== */
void MdTabView::insertRow() {
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
void MdTabView::deleteRow() {
	QModelIndexList currentSelection =
			selectionModel()->selectedIndexes();

	for (int i = 0; i < currentSelection.count(); ++i) {
		if (currentSelection.at(i).column() != 0)
			continue;

		model()->removeRow(currentSelection.at(i).row());
	}

	onUpdateWriteActions();
}
void MdTabView::setEditTriggers(QTableView::EditTriggers triggers) {
	QTableView::setEditTriggers(triggers);
}
void MdTabView::onActGrStrategyTrigd(QAction *sender) {
	if (! static_cast<SqlRtm *>(model()))
		return;

	if ((sender != actFieldStrategy) &&
		 (sender != actRowStrategy) &&
		 (sender != actManualStrategy))
		qReturn("Bad sender detected!");

	SqlRtm *rtm = static_cast<SqlRtm *>(model());

	if (sender == actFieldStrategy)
		rtm->setEditStrategy(QSqlTableModel::OnFieldChange);

	if (sender == actRowStrategy)
		rtm->setEditStrategy(QSqlTableModel::OnRowChange);

	if (sender == actManualStrategy)
		rtm->setEditStrategy(QSqlTableModel::OnManualSubmit);
}
void MdTabView::onActGrContextTrigd(QAction *sender) {
	if (! static_cast<SqlRtm *>(model()))
		return;

	if ((sender != actSubmit) && (sender != actRevert) &&
		 (sender != actSelect) && (sender != actInsertRow) &&
		 (sender != actDeleteRow))
		qReturn("Bad sender detected!");

	SqlRtm *rtm = static_cast<SqlRtm *>(model());

	if (sender == actSubmit)	rtm->submitAll();
	if (sender == actRevert)	rtm->revertAll();
	if (sender == actSelect)	rtm->select();
	if (sender == actInsertRow) insertRow();
	if (sender == actDeleteRow) deleteRow();
}
void MdTabView::onUpdateWriteActions() {
	/*!
	 * Check if there is a valid model set before calling update procedure.
	 */

	SqlRtm *rtm = static_cast<SqlRtm *>(model());
	if (! rtm)	qReturn(tr("cast failed: SqlRtm *rtm = static_cast<SqlRtm *>( %1 );")
							  .arg(model()->metaObject()->className()));

	//	INFO << m_sqlTableName << tr("currentIndexIsValied:")
	//		  << currentIndex().isValid();

	actInsertRow->setEnabled(currentIndex().isValid());
	actDeleteRow->setEnabled(currentIndex().isValid());
}
void MdTabView::onSectionMoved(int logicalIdx, int oldVisualIdx, int newVisualIdx) {
	Q_UNUSED(oldVisualIdx);
	if (! modelCast())
		CRIT << tr("ModelCast failed");
	modelCast()->setSectionIdx(logicalIdx, newVisualIdx);
	modelCast()->storeModel(sqlTableName());
}
/* ======================================================================== */
/*                              Init methodes                               */
/* ======================================================================== */
void MdTabView::restoreView() {
	/** Make column/row position movable by dragging */
	horizontalHeader()->setSectionsMovable(true);
	verticalHeader()->setSectionsMovable(true);

	/** Restore alternating row color */
	QSETTINGS;
	QColor color = config.value(objectName() + Md::k.alterRowColor,
										 QColor(Qt::white)).value<QColor>();
	if (color.isValid())
		setAlternateRowCol(
					color, config.value(objectName() + Md::k.altRowColOn, "true").toBool());

	/*!	Restore model sources	*/
}
void MdTabView::restoreColumnOrderAndVisability() {
	//!< Restore the models source members
	if (! modelCast())
		CRIT << tr("ModelCast failed");
	modelCast()->restoreModelSrcsFromIni(sqlTableName());

	//!< restoreVisibleCols(); takes access into models column-show/hide source list
	SectionMask *sm = new SectionMask(this, this);
	sm->restoreVisibleCols();
	delete sm;

	//!< modelCast()->sectionIdxs(); takes the section order list from model source...
	QList<int> sectIdxs = modelCast()->sectionIdxs();

	//!< ... and iterates over all elements so the restore process is complete
	for (int k = 0; k < sectIdxs.length(); k++)
		horizontalHeader()->moveSection(k, sectIdxs.at(k));

}
QList<QAction *> MdTabView::createActions() {
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
	connect(actGrStrategy, &QActionGroup::triggered, this, &MdTabView::storeActionState);
	connect(actGrStrategy, &QActionGroup::triggered, this, &MdTabView::onActGrStrategyTrigd);

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
	connect(actGrContext, &QActionGroup::triggered, this, &MdTabView::storeActionState);
	connect(actGrContext, &QActionGroup::triggered, this, &MdTabView::onActGrContextTrigd);

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
/* ======================================================================== */
/*                              Event handler                               */
/* ======================================================================== */
void MdTabView::showEvent(QShowEvent *) {
}
void MdTabView::mouseDoubleClickEvent(QMouseEvent *e) {
	Qt::KeyboardModifiers keyboardModifiers = QApplication::queryKeyboardModifiers();

	if (keyboardModifiers == Qt::ControlModifier) {
		actSectionMask->trigger();
		e->accept();
	}
	else
		e->ignore();

	QTableView::mouseDoubleClickEvent(e);
}
void MdTabView::mousePressEvent(QMouseEvent *e) {
	/*!
	 * Event handler for single mouse button press events inside QTableView
	 * viewport area.
	 */
	Qt::KeyboardModifiers keyboardModifiers = QApplication::queryKeyboardModifiers();

	if (keyboardModifiers != Qt::ControlModifier) {
		emit viewportMouseButtonPress(this);
		e->ignore();
		QTableView::mousePressEvent(e);
	}
	else
		e->accept();
}
void MdTabView::hideEvent(QHideEvent *) {
//	return; //@@@MDB
	if (! modelCast())
		qReturn("ModelCast failed");
	modelCast()->storeModel(sqlTableName());
}
void MdTabView::wheelEvent (QWheelEvent * event) {
	/*!
	 * Change font size has been moved to GlobalEventFilter::eventFilter
	 */

	/*!
	 * query current keyboard modifiers
	 */
	Qt::KeyboardModifiers keyboardModifiers = QApplication::queryKeyboardModifiers();
	int dsize = event->delta() / abs(event->delta());     /**< Delta size */

	if(keyboardModifiers == Qt::ShiftModifier) {
		verticalScrollBar()->setValue(
					verticalScrollBar()->value() - 4 * dsize );
		event->accept();
		return;
	}

	verticalScrollBar()->setValue(
				verticalScrollBar()->value() - dsize );

	event->accept();
}
/* ======================================================================== */
/*                             Helper methodes                              */
/* ======================================================================== */
void MdTabView::setAlternateRowCol(QColor &col, bool alternateEnabled) {
	setAlternatingRowColors(alternateEnabled);

	if (alternateEnabled && col.isValid()) {
		QTableView::setStyleSheet(
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
void MdTabView::storeFont(QFont f) {
	QSETTINGS;
	config.setValue(objectName() + Md::k.tableFont, QVariant::fromValue(f));
	setFont(f);
}
QFont MdTabView::restoreFont() {
	QSETTINGS;
	QFont f(config.value(objectName() + Md::k.tableFont).value<QFont>());
	//	INFO << tr("Restoring MdTabView::Font:") << f;
	setFont(f);
	return f;
}
void MdTabView::resizeRowsColsToContents() {
	setVisible( false );
	resizeColumnsToContents();
	resizeRowsToContents();
	setVisible( true );
}
void MdTabView::setColumnHidden(const int column, const bool hide) {
	setColumnHidden(column, hide);
}
void MdTabView::storeActionState(QAction *sender) {
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
bool MdTabView::restoreActionObjects() {
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
SqlRtm *MdTabView::modelCast() {
//	QAbstractItemModel *itm = model();

	if (! qobject_cast<SqlRtm *>(model()))
		CRIT << tr("dynamic cast failed");
	return qobject_cast<SqlRtm *>(model());
}
void MdTabView::removeColumnsConfig() {
	QSETTINGS;
	config.remove(sqlTableName() + Md::k.centerCols);
	config.remove(sqlTableName() + Md::k.visibleCols);
	config.remove(sqlTableName() + Md::k.sectionIdxs);

	config.sync();
}
void MdTabView::resetColumnsDefaultPos(bool allVisible) {
	QHeaderView *h = horizontalHeader();
	h->hide();
	for (int k = 0; k < model()->columnCount(); k++) {
		h->moveSection(h->visualIndex(k), k);
		if (allVisible)	h->setSectionHidden(k, false);
	}
	h->show();
}

#define QFOLDINGSTART {
const QString MdTabView::StyleSheet_QTableView = QString(
			"QTableView {"
			"	margin-top: 1ex; "
			"	background-color: white;"
			" 	border-radius: 5px;"
			"	border: 0px solid;"
			"	border-top-color: qlineargradient(spread:reflect, x1:0, y1:0, x2:0.5, y2:0, stop:0.10 rgba(82, 82, 82, 255), stop:1 rgba(169, 169, 169,20));"
			"	border-left-color: rgba(105,105,105,255);"
			"	border-right-color: rgba(105,105,105,205);"
			"	border-bottom-color: rgba(105,105,105,205);"
			" } [selected=true] {"
			"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #C0C0C0, stop: 1 #FFFFFF); "
			"}"
			"QHeaderView::section {"
			"     background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
			"                                       stop:    0 #616161, stop: 0.5 #505050,"
			"                                       stop: 0.6 #434343, stop:    1 #656565);"
			"     color: white;"
			"     padding-left: 4px;"
			"     padding-right: 4px;"
			"     padding-top: 2px;"
			"     padding-bottom: 2px;"
			"     border: 1px solid #6c6c6c;"
			"}"
			" QHeaderView::section:checked {"
			"     background-color: rgb(31, 94, 233);"
			"}"
			);
#define QFOLDINGEND }

