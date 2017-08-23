#include "mdtabview.h"

class Browser;
class SectionMask;

#define USE_TABLE_UI

/* ======================================================================== */
/*                             MdTabView::MdTabView                         */
/* ======================================================================== */
MdTabView::MdTabView(SfiltMdl *proxyModel, QWidget *parent)
	: QTableView(parent), mModelType(Model_Unknown) {
	setModel(proxyModel);
	MdTabView("PROXYMODEL", parent);
}
MdTabView::MdTabView(const QString &tableName, QWidget *parent)
	: QTableView(parent), mModelType(Model_Unknown) {

	if (! tableName.contains("PROXYMODEL")) {
		if (!tableName.isEmpty()) {
			createForeignModel(tableName);
			setSqlTableName(tableName);
		}
		//		else
		//			WARN << tr("No valid table name passed");
	}

	/*!
	 * Create this classes QAction objects.
	 */
	addActions( createActions() );
	setContextMenuPolicy(Qt::ActionsContextMenu);
	setSortingEnabled( true );
	setSelectionMode(QAbstractItemView::MultiSelection);
	//	QTableView::horizontalHeader()->setStretchLastSection(true);

	setSizeAdjustPolicy(QTableView::AdjustToContents);
	adjustSize();

	setStyleSheet(StyleSheet_QTableView);
	connect(horizontalHeader(), &QHeaderView::sectionMoved,
			  this, &MdTabView::onSectionMoved);

	clipboard = qApp->clipboard();
	restoreFont();

	initMsgBoxes();

	QTimer::singleShot(200, this, SLOT(restoreActionObjects()));
	//	QTimer::singleShot(8000, this, SLOT(restoreActionObjects()));
	//	setActiveSelected( false );
}
/* ======================================================================== */
void MdTabView::setSqlTableName(const QString &name) {
	m_sqlTableName = name;
	emit sqlTableNameChanged(m_sqlTableName);
}
void MdTabView::createForeignModel(const QString &tableName) {
	/*!
	 * Get pointer to active database.
	 * Instantiate new relational sql table model.
	 */
	if (! sqlRtm.isNull())
		delete sqlRtm.data();

	sqlRtm = new SqlRtm(ConnectionWidget::instance()->currentDb(), this);
	sqlRtm->setEditStrategy(QSqlTableModel::OnFieldChange);
	/*!
	 * Set the passed sql table name.
	 */
	sqlRtm->setTable(tableName);

	QTime *swatch = new QTime();
	swatch->start();

	/*!
	 * Fetch the models whole data from sql database.
	 */
	while (sqlRtm->canFetchMore())
		sqlRtm->fetchMore();
	INFO << tr("Model fetch delay: %1ms").arg(swatch->elapsed());
	delete swatch;

	/*!
	 * Because the target database contains more than one table related on foreign keys,
	 * varying process is necessary to set the different relations correctly. Which
	 * processing branch must be used depends on the table name passed by sqlTbl.
	 */

	/*!
	 * Processing branches for table ...
	 */
	do {
		/* --------------------------------------------------------- */
		/*                      Table worktimes                      */
		/* --------------------------------------------------------- */
		if (tableName.contains("worktime", Qt::CaseInsensitive)) {
			/** Remember the indexes of the columns */
			int colEmp = sqlRtm->fieldIndex("workerID");
			int colPrj = sqlRtm->fieldIndex("prjID");

			/** Set the relations to foreign database tables */
			sqlRtm->setRelation(colEmp, QSqlRelation("worker", "workerID", "Nachname"));
			sqlRtm->setRelation(colPrj, QSqlRelation("prj", "prjID", "Beschreibung"));

			//!< Set the localized header captions
			sqlRtm->setHHeaderData(sqlRtm->fieldIndex("worktimID"), tr("ID"));
			sqlRtm->setHHeaderData(sqlRtm->fieldIndex("dat"), Md::headerAlias[ "dat" ]);
			sqlRtm->setHHeaderData(sqlRtm->fieldIndex("hours"), Md::headerAlias[ "hours" ]);
			sqlRtm->setHHeaderData(colEmp, Md::headerAlias[ "workerID" ]);
			sqlRtm->setHHeaderData(colPrj, tr("Beschreibung"));

			break;
		}
		/* --------------------------------------------------------- */
		/*                      Table projects                       */
		/* --------------------------------------------------------- */
		if (tableName.contains("prj", Qt::CaseInsensitive)) {
			/** Remember the indexes of the columns */
			int colClient = sqlRtm->fieldIndex("clientID");
			int colSub = sqlRtm->fieldIndex("subID");
			int colArch = sqlRtm->fieldIndex("archID");

			sqlRtm->setJoinMode(QSqlRelationalTableModel::LeftJoin);
			/** Set the relations to the other database tables */
			sqlRtm->setRelation(colClient, QSqlRelation("client", "clientID", "Name"));
			sqlRtm->setRelation(colSub, QSqlRelation("arch", "archID", "Name"));
			sqlRtm->setRelation(colArch, QSqlRelation("arch", "archID", "Name"));

			//!< Set the localized header captions
			sqlRtm->setHHeaderData(sqlRtm->fieldIndex("prjID"), tr("ID"));
			sqlRtm->setHHeaderData(sqlRtm->fieldIndex("clientID"),
										  Md::headerAlias[ tr("%1/clientID").arg(tableName) ]);
			sqlRtm->setHHeaderData(sqlRtm->fieldIndex("subID"),
										  Md::headerAlias[ tr("%1/subID").arg(tableName) ]);
			sqlRtm->setHHeaderData(sqlRtm->fieldIndex("archID"),
										  Md::headerAlias[ tr("%1/archID").arg(tableName) ]);
			sqlRtm->setHHeaderData(sqlRtm->fieldIndex("Nummer"),
										  Md::headerAlias[ tr("%1/Nummer").arg(tableName) ]);

			break;
		}
		/* --------------------------------------------------------- */
		/*                      Table worker                         */
		/* --------------------------------------------------------- */
		if (tableName.contains("worker", Qt::CaseInsensitive)) {
			/** Remember the indexes of the columns */
			int colGrading = sqlRtm->fieldIndex("gradingID");

			/** Set the relations to foreign database tables */
			sqlRtm->setRelation(colGrading, QSqlRelation("grading", "gradingID", "Einstufung"));

			//!< Set the localized header captions
			sqlRtm->setHHeaderData(sqlRtm->fieldIndex("workerID"), tr("ID"));
			sqlRtm->setHHeaderData(sqlRtm->fieldIndex("PersonalNr"),
										  Md::headerAlias[ "PersonalNr" ]);
			sqlRtm->setHHeaderData(sqlRtm->fieldIndex("Stundensatz"),
										  Md::headerAlias[ "Stundensatz" ]);
			sqlRtm->setHHeaderData(sqlRtm->fieldIndex("Wochenstunden"),
										  Md::headerAlias[ "Wochenstunden" ]);

			break;
		}
		/* --------------------------------------------------------- */
		/*                      Table client                         */
		/* --------------------------------------------------------- */
		if (tableName.contains("client", Qt::CaseInsensitive)) {
			//!< Set the localized header captions
			sqlRtm->setHHeaderData(sqlRtm->fieldIndex("clientID"), tr("ID"));
			sqlRtm->setHHeaderData(sqlRtm->fieldIndex("Nummer"),
										  Md::headerAlias[ tr("%1/Nummer").arg(tableName) ]);

			break;
		}
		/* --------------------------------------------------------- */
		/*                      Table fehlzeit                       */
		/* --------------------------------------------------------- */
		if (tableName.contains("fehlzeit", Qt::CaseInsensitive)) {
			//!< Set the localized header captions
			sqlRtm->setHHeaderData(sqlRtm->fieldIndex("fehlID"), tr("ID"));

			break;
		}
		/* --------------------------------------------------------- */
		/*                      Table arch									 */
		/* --------------------------------------------------------- */
		if (tableName.contains("arch", Qt::CaseInsensitive)) {
			//!< Set the localized header captions
			sqlRtm->setHHeaderData(sqlRtm->fieldIndex("archID"), tr("ID"));

			break;
		}
		/* --------------------------------------------------------- */
		/*                      Table grading								 */
		/* --------------------------------------------------------- */
		if (tableName.contains("grading", Qt::CaseInsensitive)) {
			//!< Set the localized header captions
			sqlRtm->setHHeaderData(sqlRtm->fieldIndex("gradingID"), tr("ID"));

			break;
		}
		/* --------------------------------------------------------- */
		/*                      Table runtime								 */
		/* --------------------------------------------------------- */
		if (tableName.contains("runtime", Qt::CaseInsensitive)) {
			/** Remember the indexes of the columns */
			int colEmp = sqlRtm->fieldIndex("workerID");
			int colFehl = sqlRtm->fieldIndex("fehlID");

			/** Set the relations to foreign database tables */
			sqlRtm->setRelation(colEmp, QSqlRelation("worker", "workerID", "Nachname"));
			sqlRtm->setRelation(colFehl, QSqlRelation("fehlzeit", "fehlID", "Grund"));

			//!< Set the localized header captions
			sqlRtm->setHHeaderData(sqlRtm->fieldIndex("runtimeID"), tr("ID"));
			sqlRtm->setHHeaderData(colEmp, Md::headerAlias[ "workerID" ]);
			sqlRtm->setHHeaderData(colFehl, Md::headerAlias[ "fehlID" ]);


			break;
		}
		/* --------------------------------------------------------- */
		/*                      Table Unknown                        */
		/* --------------------------------------------------------- */
		QMessageBox::warning(this, tr("Warnung"),
									tr("Unbekannter SQL Tabellenbezeichner: %1")
									.arg(tableName), QMessageBox::Ok);
		break;
	} while(1);

	/*!
	 * Replace "*summe" by "*-<SigmaSign>".
	 */
	for (int k = 0; k < sqlRtm->columnCount(); k++) {
		QString colData = sqlRtm->headerData(k).toString();
		if (colData.contains("summe", Qt::CaseInsensitive)) {
			colData.replace("summe", tr("-%1").arg(MathSymbolSigma));
			sqlRtm->setHeaderData(k, Qt::Horizontal, colData, Qt::EditRole);
		}
	}

	/*!
	 * Populate the model.
	 */
	sqlRtm->select();

	/*!
	 * Set the model to this's base class and hide the ID column.
	 */
	setModel(sqlRtm);
	setEditTriggers( QAbstractItemView::DoubleClicked |
						  QAbstractItemView::EditKeyPressed );

	/*!
	 * Sorting column and direction differs from sql table name.
	 */
	if (tableName.contains("worktime"))
		sortByColumn(0, Qt::DescendingOrder);
	else {
		int k;
		for (k = 0; k < model()->columnCount(); k++) {
			QString colHead;
			if (mModelType == MdTabView::Model_SqlRtm)
				colHead = sqlRtmCast()->headerData(k).toString();

			if (colHead.contains(tr("Name"), Qt::CaseSensitive) ||
				 colHead.contains(tr("Nachname"), Qt::CaseSensitive) ||
				 colHead.contains(tr("Einstufung"), Qt::CaseSensitive)) {
				sortByColumn(k, Qt::AscendingOrder);
				break;
			}
		}
		/*!
		 * If break has not been reached...
		 */
		if (k >= model()->columnCount())
			sortByColumn(0, Qt::AscendingOrder);
	}

	for (int k = 0; k < sqlRtm->columnCount(); k++)
		sqlRtm->sectionIdxs().append( k );

	if (! (bool) connect(selectionModel(), &QItemSelectionModel::currentRowChanged,
								this, &MdTabView::currentChanged)) {
		//			emit stateMsg(tr("Slot connection returns false"));
		INFO << tr("Slot connection returns false");
	}
	resizeRowsColsToContents();
	//    emit updateActions();
	setSqlTableName(tableName);
	restoreColumnOrderAndVisability();
	setFont(restoreFont());
}
void MdTabView::refreshView() {
	this->update();
}
void MdTabView::setModel(QAbstractItemModel *model) {
	QTableView::setModel(model);

	if (qobject_cast<SqlRtm *>(model))							mModelType = Model_SqlRtm;
	else if (qobject_cast<SfiltMdl *>(model))					mModelType = Model_SfiltMdl;
	else if (qobject_cast<QAbstractItemModel *>(model))	mModelType = Model_BaseClass;
	else mModelType = Model_Unknown;

	INFO << mModelType;

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
	if ((sender != actSubmit) && (sender != actRevert) &&
		 (sender != actSelect) && (sender != actInsertRow) &&
		 (sender != actDeleteRow) && (sender != actCopy) &&
		 (sender != actPaste) && (sender != actSumSelection))
		qReturn("Bad sender detected!");

	if (sender == actInsertRow)		insertRow();
	if (sender == actDeleteRow)		deleteRow();
	if (sender == actCopy)				copy();
	if (sender == actPaste)				paste();
	if (sender == actSumSelection)	sumSelection();

	if (mModelType != MdTabView::Model_SqlRtm)
		return;

	SqlRtm *rtm = sqlRtmCast();

	if (sender == actSubmit)		rtm->submitAll();
	if (sender == actRevert)		rtm->revertAll();
	if (sender == actSelect)		rtm->select();
}
void MdTabView::onUpdateWriteActions() {
	/*!
	 * Check if there is a valid model set before calling update procedure.
	 */
	if (mModelType == MdTabView::Model_SqlRtm)
		if (! sqlRtmCast())
			qReturn(tr("cast failed: SqlRtm *rtm = static_cast<SqlRtm *>( %1 );")
					  .arg(model()->metaObject()->className()));

	actInsertRow->setEnabled(currentIndex().isValid());
	actDeleteRow->setEnabled(currentIndex().isValid());
}
void MdTabView::onSectionMoved(int logicalIdx, int oldVisualIdx, int newVisualIdx) {
	Q_UNUSED(oldVisualIdx);

	if (mModelType == MdTabView::Model_SqlRtm) {
		sqlRtmCast()->setSectionIdx(logicalIdx, newVisualIdx);
		sqlRtmCast()->storeModel(sqlTableName());
	}
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
	QColor *color = new QColor(config.value(objectName() + Md::k.alterRowColor,
														 QColor(Qt::white)).value<QColor>());
	if (color->isValid())
		setAlternateRowCol(
					*color, config.value(objectName() + Md::k.alterRowColOn, "true").toBool());

	color = new QColor(config.value(objectName() + Md::k.gridLineColor,
											  QColor(Qt::gray)).value<QColor>());
	if (color->isValid())
		setGridColor(*color);

	delete color;

	/*!	Restore model sources	*/
}
void MdTabView::restoreColumnOrderAndVisability() {
	//!< Restore the models source members
	if (mModelType != MdTabView::Model_SqlRtm)
		return;

	sqlRtmCast()->restoreModelSrcsFromIni(sqlTableName());

	//!< restoreVisibleCols(); takes access into models column-show/hide source list
	SectionMask *sm = new SectionMask(this, this);
	sm->restoreVisibleCols();
	delete sm;

	//!< modelCast()->sectionIdxs(); takes the section order list from model source...
	QList<int> sectIdxs = sqlRtmCast()->sectionIdxs();

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
	actCopy =			new QAction(tr("Kopieren"), this);
	actPaste =			new QAction(tr("Einfügen"), this);
	actSumSelection =	new QAction(tr("Summe über Auswahl-Maske bilden"), this);
	actClearRecords =	new QAction(tr("Alle Zeilen löschen"), this);
	actDropTable =		new QAction(tr("Tabelle löschen"), this);
	actDateTimeRngMsk =	new QAction(tr("Datumsbereich"), this);
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
			<< PONAM(actRevert)  << PONAM(actSelect) << PONAM(actCopy) << PONAM(actPaste)
			<< PONAM(actSumSelection);

	actCopy->setShortcut(QKeySequence("Ctrl+C"));
	actPaste->setShortcut(QKeySequence("Ctrl+V"));
	actSumSelection->setShortcut(QKeySequence("Ctrl+Shift+s"));
	actCopy->setShortcutContext(Qt::WidgetShortcut);
	actPaste->setShortcutContext(Qt::WidgetShortcut);
	actSumSelection->setShortcutContext(Qt::WidgetShortcut);

	foreach (QAction *act, acts2)
		actGrContext->addAction(act);

	/*!
	 * Connect action group triggered signal to a save-action-state slot.
	 */
	connect(actGrContext, &QActionGroup::triggered, this, &MdTabView::storeActionState);
	connect(actGrContext, &QActionGroup::triggered, this, &MdTabView::onActGrContextTrigd);

	connect(actClearRecords, &QAction::triggered, this, &MdTabView::clearRecords);
	connect(actDropTable, &QAction::triggered, this, &MdTabView::onActDropTableTrigd);

	/* ======================================================================== */
	/*                       Action object configurations                       */
	/* ======================================================================== */
	actSectionMask->setCheckable(true);
	actDateTimeRngMsk->setCheckable(true);

	acts2 << acts;

	QAction *sep = new QAction(this);
	sep->setSeparator(true);
	acts2 << sep << actSectionMask << actDateTimeRngMsk;

	sep = new QAction(this);
	sep->setSeparator(true);
	acts2 << sep << actClearRecords << actDropTable;

	return acts2;
}
/* ======================================================================== */
/*                              Event handler                               */
/* ======================================================================== */
void MdTabView::showEvent(QShowEvent *) {
}
void MdTabView::mouseDoubleClickEvent(QMouseEvent *e) {
	Qt::KeyboardModifiers keyboardModifiers =
			QApplication::queryKeyboardModifiers();

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
	Qt::KeyboardModifiers keyboardModifiers =
			QApplication::queryKeyboardModifiers();

	if (keyboardModifiers != Qt::ControlModifier)
		emit viewportMouseButtonPress(this);

	/*!
	 * Should be ignored since we only want to inform some instances about the viewport
	 * mouse click. Also we need the ControlModifier event to be passed to superclass
	 * when the QTableView::selectionMode() "Extended" is in use!
	 */
	e->ignore();
	QTableView::mousePressEvent(e);
}
void MdTabView::hideEvent(QHideEvent *) {
	if (mModelType == MdTabView::Model_SqlRtm)
		sqlRtmCast()->storeModel(sqlTableName());
}
void MdTabView::wheelEvent (QWheelEvent * event) {
	/*!
	 * Change font size has been moved to GlobalEventFilter::eventFilter
	 */

	/*!
	 * query current keyboard modifiers
	 */
	Qt::KeyboardModifiers keyboardModifiers =
			QApplication::queryKeyboardModifiers();
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
void MdTabView::focusOutEvent(QFocusEvent *) {
	clearSelection();
}
/* ======================================================================== */
/*                             Callback handler                             */
/* ======================================================================== */
QList<QString> MdTabView::copy(bool overwriteClipboardBuffer) {
	QItemSelectionModel * selection = selectionModel();
	QModelIndexList indexes = selection->selectedIndexes();

	if(indexes.size() < 1)
		return QList<QString>();

	// QModelIndex::operator < sorts first by row, then by column.
	// this is what we need
	qSort(indexes);
	QStringList fields;

	foreach (QModelIndex idx, indexes)
		fields << idx.data(Qt::DisplayRole).toString();

	if (overwriteClipboardBuffer)
		clipboard->setText(fields.join(' '));
	else
		if (clipboard->text(QClipboard::Clipboard).isEmpty())
			clipboard->setText(fields.join(' '));

	return fields;
}
void MdTabView::paste() {
	INFO << tr("paste");
	return;
}
void MdTabView::sumSelection() {
	bool ok = true;
	QItemSelectionModel * selection = selectionModel();
	QModelIndexList indexes = selection->selectedIndexes();

	if(indexes.size() < 1)
		return;

	// QModelIndex::operator < sorts first by row, then by column.
	// this is what we need
	qSort(indexes);

	QList<qreal> numerics;

	foreach (QModelIndex idx, indexes) {
		qreal field = idx.data(Qt::DisplayRole).toReal(&ok);
		if (! ok)
			selection->select(idx, QItemSelectionModel::Deselect);
		else
			numerics << field;
	}

	qreal sum = 0;
	foreach (qreal v, numerics)
		sum += v;

	INFO << sum;
	emit sumOfSelection(QVariant::fromValue(sum), "sum");
}
/* ======================================================================== */
/*                             Helper methodes                              */
/* ======================================================================== */
void MdTabView::setAlternateRowCol(QColor &color, bool alternateEnabled) {
	QSETTINGS;
	if (color.isValid()) {
		stylesheetPvs.insert("alternate-background-color:", tr("rgba(%1,%2,%3,%4);")
									.arg( color.red()).arg( color.green())
									.arg( color.blue()).arg( color.alpha()));

		config.setValue(objectName() + Md::k.alterRowColor, color);
		refreshStylesheet();
	}
	setAlternatingRowColors(alternateEnabled);
	config.setValue(objectName() + Md::k.alterRowColOn, alternateEnabled);

}
void MdTabView::setGridColor(QColor &color) {
	QSETTINGS;
	stylesheetPvs.insert("gridline-color:", tr("rgba(%1,%2,%3,%4);")
								.arg( color.red()).arg( color.green())
								.arg( color.blue()).arg( color.alpha()));

	config.setValue(objectName() + Md::k.gridLineColor, color);
	refreshStylesheet();
}
void MdTabView::refreshStylesheet(QString stylesheet) {
	QString s;

	if (stylesheet.isEmpty()) {
		QStringList keys = stylesheetPvs.keys();
		QStringList values = stylesheetPvs.values();

		for (int k = 0; k < stylesheetPvs.count(); k++)
			s.append(keys.at(k) + " " + values.at(k) + "\n");
	}
	else
		s = stylesheet;

	Globals::setStylesheet(this, s);
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
	QTableView::setColumnHidden(column, hide);
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
void MdTabView::selectionChanged(const QItemSelection &selected,
											const QItemSelection &deselected) {
	Q_UNUSED(selected);
	Q_UNUSED(deselected);
	sumSelection();
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
