#include "browser.h"

#define T_CYCLIC 250e-3
#define	COMMENT_OUT_UNUSED

bool ok;
//QFont("Helvetica [Cronyx]", 10)

struct Browser::mTabs_t Browser::mTabs = Browser::mTabs_t();
Browser *Browser::inst = 0;

/* ======================================================================== */
/*                             Browser::Browser                             */
/* ======================================================================== */
Browser::Browser(QWidget *parent)
	: QWidget(parent) {
	inst = this;
	setObjectName("Browser");

	m_stateCounter = 0;

	setStyleSheet(Locals::browserStyleSheetv2);
	style()->unpolish(this);
	style()->polish(this);
	update();

	createActions();
	createUi(this);
	connect(connectionWidget(),	&ConnectionWidget::tableActivated,
			  this,						&Browser::onConWidgetTableActivated);

	connect(this, &Browser::tvSelectorChanged, this, &Browser::onTvSelectorChanged);

	if (QSqlDatabase::drivers().isEmpty()) {
		QMessageBox::information
				(this, tr("No database drivers found"),
				 tr("This demo requires at least one Qt database driver. "
					 "Please check the documentation how to build the "
					 "Qt SQL plugins."));
		qApp->quit();
	}
	else {
		timCyc = new QTimer();
		timCyc->setInterval(T_CYCLIC * 1e3);
		connect( timCyc, &QTimer::timeout, this, &Browser::onCyclic);
		timCyc->start();
	}

	emit stateMsg(tr("Browser Ready."));

	QTimer::singleShot(50, this, SLOT(restoreUi()));
}
Browser::~Browser() {
	INFO << tr("close browser!");
	//	delete ui;
}

void Browser::onConnectWdgMetaDataReq(const QString &table)	{
	mTabs.currentSelected()->tv()->setModel( tblToMetaDataMdl(table) );
	mTabs.currentSelected()->tv()->setEditTriggers(QAbstractItemView::NoEditTriggers);
	autofitRowCol();
	emit updateWriteActions();
}
void Browser::requeryWorktimeTableView(QString nonDefaulQuery) {
	QSETTINGS_QUERYS;
	/*!
	 * Get pointer to current active database connection.
	 */
	QSqlDatabase pdb = connectionWidget()->currentDb();

	/*!
	 * Get list of all table views and requery all of them with gb name "worktime".
	 */
	QList<MdTable *> tvs = findChildren<MdTable *>();
	foreach (MdTable *tv, tvs) {
		if (! tv->grBox()->title().contains(tr("worktime")))
			tvs.removeOne(tv);
	}


	/** Select "tableMain" as target table view widget */
	QTableView *tvResp = mTabs.tvl1->tv();
	QSqlQueryModel *queryModel = new QSqlQueryModel(tvResp);
	QSqlQuery query;

	if (! nonDefaulQuery.isEmpty())
		query = QSqlQuery( nonDefaulQuery, pdb);
	else {
		/**
		 * Use a QTextEdit to transform a query saved as html into
		 * corresponding plain text query
		 */
		QTextEdit *te = new QTextEdit();
		te->hide();

		foreach (QString str, configQ.allKeys()) {
			if (str.contains("default_worktime_query")) {
				QString s = configQ.value(str, "").toString();
				te->setHtml( configQ.value("InpFrm/" + s, "").toString() );
			}
		}
		query = QSqlQuery(te->toPlainText(), pdb);
		INFO << te->toPlainText();
		delete te;
	}

	queryModel->setQuery( query );
	tvResp->setModel( queryModel );

	if (queryModel->lastError().type() != QSqlError::NoError)
		emit stateMsg(queryModel->lastError().text());
	else if (queryModel->query().isSelect())
		emit stateMsg(tr("Query OK."));
	else
		emit stateMsg(tr("Query OK, number of affected rows: %1").arg(
							  queryModel->query().numRowsAffected()));

	mTabs.tvl1->resizeRowsColsToContents();
	//	tvResp->setVisible( false );
	//	tvResp->resizeColumnsToContents();
	//	tvResp->resizeRowsToContents();
	//	tvResp->setVisible( true );
	emit updateWriteActions();

}
void Browser::exec() {
	InpFrm *inpFrm = InpFrm::instance();

	QSqlQueryModel *model = new QSqlQueryModel(mTabs.tvl1->tv());

	model->setQuery(QSqlQuery(inpFrm->getQueryText(),
									  connectionWidget()->currentDb()));
	mTabs.tvl1->tv()->setModel(model);

	if (model->lastError().type() != QSqlError::NoError)
		emit stateMsg(model->lastError().text());
	else if (model->query().isSelect())
		emit stateMsg(tr("Query OK."));
	else
		emit stateMsg(tr("Query OK, number of affected rows: %1").arg(
							  model->query().numRowsAffected()));

	mTabs.tvl1->tv()->setSortingEnabled( true );

	//	tvq->setVisible( false );
	//	tvq->resizeColumnsToContents();
	//	tvq->resizeRowsToContents();
	//	tvq->setVisible( true );

	mTabs.tvl1->resizeRowsColsToContents();
	emit updateWriteActions();
}
/* ======================================================================== */
void Browser::onConWidgetTableActivated(const QString &sqlTbl) {
	/*!
	 * Invoked after double clicking a table item in connection widget tree view.
	 *
	 * Determine which MdTable should be targeted by the activated tab model. Either select
	 * an instance of MdTable which hasn't a model loaded yet. To swap sql tables during
	 * runtime, a double-mouse-click event is captured and the user-selected tv instance
	 * becomes addressed by the event handler.
	 */

	if (sqlTbl.contains(tr("worktime"))) {
		mdtv = new MdTabView(sqlTbl);
		connect(this, &Browser::updateWriteActions,
				  mdtv, &MdTabView::onUpdateWriteActions);
	}

	createForeignTable(sqlTbl, mTabs.currentSelected());
	if (FilterForm::instance()->isVisible())
		FilterForm::instance()->setSourceTable(mTabs.currentSelected());
}
MdTable *Browser::createForeignTable(const QString &tNam, MdTable *tvc) {
	/**
	 * Get active database pointer
	 */
	QSqlDatabase pDb = connectionWidget()->currentDb();
	SqlRtm *rmod = new SqlRtm(SqlRtm::srcNew, tvc->tv(), pDb);
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

			/** Set the relations to the other database tables */
			rmod->setRelation(colEmp, QSqlRelation(
										"worker", "workerID", "Nachname"));
			rmod->setRelation(colPrj, QSqlRelation(
										"prj", "prjID", "Beschreibung"));

			// Set the localized header captions
			rmod->setHeaderData(rmod->fieldIndex("worktimID"), Qt::Horizontal, tr("ID"));
			rmod->setHeaderData(rmod->fieldIndex("dat"), Qt::Horizontal, tr("Datum"));
			rmod->setHeaderData(colEmp, Qt::Horizontal, tr("Mitarb"));
			rmod->setHeaderData(colPrj, Qt::Horizontal, tr("Beschreibung"));
			rmod->setHeaderData(rmod->fieldIndex("hours"), Qt::Horizontal, tr("Std"));

			break;
		}
		/* --------------------------------------------------------- */
		/*                      Table projects                       */
		/* --------------------------------------------------------- */
		if (tNam.contains("prj", Qt::CaseInsensitive)) {
			/** Remember the indexes of the columns */
			int colClient = rmod->fieldIndex("clientID");

			/** Set the relations to the other database tables */
			rmod->setRelation(colClient, QSqlRelation(
										"client", "clientID", "Name"));

			// Set the localized header captions
			rmod->setHeaderData(rmod->fieldIndex("prjID"), Qt::Horizontal, tr("ID"));
			rmod->setHeaderData(rmod->fieldIndex("clientID"), Qt::Horizontal, tr("Kunde, Name"));

			break;
		}
		/* --------------------------------------------------------- */
		/*                      Table worker                         */
		/* --------------------------------------------------------- */
		if (tNam.contains("worker", Qt::CaseInsensitive)) {
			// Set the localized header captions
			rmod->setHeaderData(rmod->fieldIndex("workerID"), Qt::Horizontal, tr("ID"));
			rmod->setHeaderData(rmod->fieldIndex("PersonalNr"), Qt::Horizontal, tr("PN"));
			rmod->setHeaderData(rmod->fieldIndex("Stundensatz"), Qt::Horizontal, tr("€/h"));
			break;
		}
		/* --------------------------------------------------------- */
		/*                      Table client                         */
		/* --------------------------------------------------------- */
		if (tNam.contains("client", Qt::CaseInsensitive)) {
			// Set the localized header captions
			rmod->setHeaderData(rmod->fieldIndex("clientID"), Qt::Horizontal, tr("ID"));
			rmod->setHeaderData(rmod->fieldIndex("Nummer"), Qt::Horizontal, tr("Knd. #"));
			rmod->setHeaderData(rmod->fieldIndex("Stundensatz"), Qt::Horizontal, tr("€/h"));

			break;
		}
		/* --------------------------------------------------------- */
		/*                      Table fehlzeit                       */
		/* --------------------------------------------------------- */
		if (tNam.contains("fehlzeit", Qt::CaseInsensitive)) {
			// Set the localized header captions
			rmod->setHeaderData(rmod->fieldIndex("fehlID"), Qt::Horizontal, tr("ID"));

			break;
		}
		/* --------------------------------------------------------- */
		/*                      Table arch									 */
		/* --------------------------------------------------------- */
		if (tNam.contains("arch", Qt::CaseInsensitive)) {
			// Set the localized header captions
			rmod->setHeaderData(rmod->fieldIndex("archID"), Qt::Horizontal, tr("ID"));

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
	INFO << tr("Model load delay: %1ms").arg(time->elapsed());

	// Set the model and hide the ID column
	tvc->setSelectionMode(QAbstractItemView::ContiguousSelection);
	tvc->setModel(rmod);
	tvc->setEditTriggers( QAbstractItemView::DoubleClicked |
								 QAbstractItemView::EditKeyPressed );

	for (int k = 0; k < rmod->columnCount(); k++)
		rmod->sectionIdxs().append( k );

	/** !!!!!!!!!!!!!!!!!!!! tvs()->first()->tv()->selectionModel() changed to
	  * mTvs[3]->tv()->selectionModel()
	 */

	if (! (bool) connect( tvc->selectionModel(),
								 SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
								 this, SLOT(currentChanged(QModelIndex,QModelIndex)) ) ) {
		emit stateMsg(tr("Slot connection returns false"));
		INFO << tr("Slot connection returns false");
	}
	tvc->resizeRowsColsToContents();

	//    emit updateActions();
	/**
	 * Update MdTables group box title to match the prior activated sql
	 * table name
	 */
	tvc->grBox()->setTitle(tNam);

	/*!
	 * Set tvcs object name to grBox()->title.
	 */
	tvc->setSqlTableName(tNam);
	//	tvc->setObjectName(tvc->grBox()->title());

	connect(this, &Browser::clearSelections,		tvc, &MdTable::clearSelected);
	connect(tvc, &MdTable::sqlTableNameChanged,	tvc, &MdTable::onSqlTableNameChanged);
	connect(this, &Browser::updateWriteActions,	tvc, &MdTable::onUpdateWriteActions);

	tvc->restoreColumnOrderAndVisability();
	tvc->tv()->setFont(tvc->restoreFont());
	return tvc;
}
/* ======================================================================== */
QStandardItemModel *Browser::tblToMetaDataMdl(const QString &table) {
	QSqlRecord rec = connectionWidget()->currentDb().record(table);
	QStandardItemModel *model = new QStandardItemModel(/*tvs()->first()->tv()*/);

	model->insertRows(0, rec.count());
	model->insertColumns(0, 7);

	model->setHeaderData(0, Qt::Horizontal, "Fieldname");
	model->setHeaderData(1, Qt::Horizontal, "Type");
	model->setHeaderData(2, Qt::Horizontal, "Length");
	model->setHeaderData(3, Qt::Horizontal, "Precision");
	model->setHeaderData(4, Qt::Horizontal, "Required");
	model->setHeaderData(5, Qt::Horizontal, "AutoValue");
	model->setHeaderData(6, Qt::Horizontal, "DefaultValue");

	for (int i = 0; i < rec.count(); ++i) {
		QSqlField fld = rec.field(i);
		model->setData(model->index(i, 0), fld.name());
		model->setData(model->index(i, 1), fld.typeID() == -1
							? QString(QMetaType::typeName(fld.type()))
							: QString("%1 (%2)").arg(QMetaType::typeName(fld.type())).arg(fld.typeID()));
		model->setData(model->index(i, 2), fld.length());
		model->setData(model->index(i, 3), fld.precision());
		model->setData(model->index(i, 4), fld.requiredStatus() == -1 ? QVariant("?")
																						  : QVariant(bool(fld.requiredStatus())));
		model->setData(model->index(i, 5), fld.isAutoValue());
		model->setData(model->index(i, 6), fld.defaultValue());
	}

	return model;
}
void Browser::customMenuRequested(QPoint pos) {
	QModelIndex index = this->tvs()->first()->tv()->indexAt(pos);
	Q_UNUSED(index);

	QMenu *menu = new QMenu(this);
	menu->addAction(new QAction("Action 1", this));
	menu->addAction(new QAction("Action 2", this));
	menu->addAction(new QAction("Action 3", this));
	menu->popup(tvs()->first()->tv()->viewport()->mapToGlobal(pos));
}
void Browser::autofitRowCol() {
	foreach (MdTable *tvc, mTabs.tvsNoPtr())
		tvc->resizeRowsColsToContents();
}
void Browser::onCyclic() {


}
void Browser::onTvSelectorChanged() {
	/*! Notify signal via proberty class */
	INFO << tr("m_tvSelector:") << tvSelector();
	/**** Safe all action states from Browser
	\*/
	//	Globals::ACTION_STORE(this);
}
void Browser::onActGroupTrigd(QAction *sender) {
	storeActionState(sender);

	if (sender->actionGroup() == actGrTvSelectBy)
		setTvSelector(sender->data().value<TvSelector>());
}
/*void Browser::onSourceTableChanged(FilterForm::SourceTableType sourceTable) {
	if (sourceTable == FilterForm::useWorktime) {
		foreach (MdTable *tv, mTabs.tvsNoPtr()) {
			//			INFO << tv->tv()->objectName() << tv->grBox()->title();
			if (tv->objectName().contains(tr("worktime")))
				FilterForm::instance()->setSourceTable(tv);
		}
	}
	if (sourceTable == FilterForm::useSelectedSource) {
		FilterForm::instance()->setSourceTable(mTabs.currentSelected());
	}

}*/
/* ======================================================================== */
/*                              Event handler                               */
/* ======================================================================== */
bool Browser::eventFilter(QObject *obj, QEvent *e) {
	MdTable *tv = NULL;
	bool ok = true;
	int k = 0;

	/*!
	 * Here we try determine a mouse button click within QGroupBox or QTableView::viewport()
	 * area. If true, set current selected MdTable as the "selected" mdTable.
	 */
	if (e->type() == QEvent::MouseButtonPress) {

		/*!
		 * tvSelector() returns the property which determines the valid sender objects.
		 * This could be none, QTableView::viewport(), QGroupBox or both.
		 * If obj->parent() obj could be casted to a QTableView, then the sender object obj
		 * was a viewPort instance.
		 */
		switch (tvSelector()) {
			case selectByViewPort: {
				/*!
				 * If obj->findChild<QTableView *> could be found, obj must be a QGroupBox.
				 */
				if (qobject_cast<QTableView *>(
						 obj->findChild<QTableView *>(QString(), Qt::FindDirectChildrenOnly)))
					return QWidget::eventFilter(obj, e);
			}; break;

			case selectByGroupBox: {
				/*!
				 * If obj->parent() is a QTableView, obj must be a viewPort.
				 */
				if (qobject_cast<QTableView *>(obj->parent()))
					return QWidget::eventFilter(obj, e);
			}; break;

			case selectByNone:	return QWidget::eventFilter(obj, e);	break;
			case selectByBoth:	break;
			default:	qWarning();
		}

		while (! (qobject_cast<MdTable *>(treeTravers<QObject>(obj, ++k, &ok))))
			if (! ok)	break;

		tv = qobject_cast<MdTable *>(treeTravers<QObject>(obj, k, &ok));

		if (tv == NULL)
			bReturn("Cannot cast sender of mouse click event!");

		/*!
		 * If the casted MdTable instance is currently NOT selected, emit the clearSelect
		 * signal and invoke tv->setSelected();
		 */
		if (! tv->isSelected()) {
			emit clearSelections();
			tv->setSelected();
			e->accept();
			return true;
		}
	}

	/**
	  * standard event processing
	  */
	return QObject::eventFilter(obj, e);
}
void Browser::showEvent(QShowEvent *) {
	SPLT_RESTORE(this);
	autofitRowCol();
	emit visibilityChanged( true );
}
void Browser::hideEvent(QHideEvent *) {
	SPLT_STORE(this);
	emit visibilityChanged( false );
}
void Browser::closeEvent(QCloseEvent *) {
	QSETTINGS;

	INFO << findChildren<QSplitter *>(QString(), Qt::FindDirectChildrenOnly);

	/**** Write splitter sizes to config
	\*/
	SPLT_STORE(this);

	/**** Write mdTable <-> SQL table relations
	  \*/
	foreach (MdTable *tv, mTabs.tvsNoPtr())
		config.setValue(objectName() + "/" + tv->objectName(), tv->grBox()->title());

	/**** Safe all action states from Browser
	\*/
	//	ACTION_STORE(this, tr("act*"));
}
/* ======================================================================== */
/*                              Init methodes                               */
/* ======================================================================== */
bool Browser::restoreUi() {
	bool ret = true;
	QSETTINGS;

	/**** Restore splitter sizes to config
	\*/
	SPLT_RESTORE(this);

	/**** Restore mdTable <-> SQL table assignements, but only if valid keys are stored
	 \*/
	foreach (MdTable *tv, mTabs.tvsNoPtr()) {
		if (! config.allKeys().join(',').contains( tv->objectName() ))
			continue;

		QString tabNam = config.value(objectName() + tr("/") + tv->objectName()).toString();
		tv->restoreView();
		createForeignTable( tabNam, tv );
	}

	/**** Restore all action states from Browser
	\*/
	restoreActionObjects();

	/**** Restore table font configs
	\*/
	//	INFO << mTabs.tva->confTableFont();
	//	INFO << mTabs.tva->confTableFont();
	return ret;
}
void Browser::createActions() {
	/* ======================================================================== */
	/*                          Create QAction objects                          */
	/* ======================================================================== */
	actSelByNone = new QAction(tr("Select by &None"), this);
	actSelByGrBx = new QAction(tr("Select by &Group Box"), this);
	actSelByVPort = new QAction(tr("Select by &View Port"), this);
	actSelByBoth = new QAction(tr("Select by &Both"), this);

	actSelByNone->setData(QVariant::fromValue(TvSelector::selectByNone));
	actSelByGrBx->setData(QVariant::fromValue(TvSelector::selectByGroupBox));
	actSelByVPort->setData(QVariant::fromValue(TvSelector::selectByViewPort));
	actSelByBoth->setData(QVariant::fromValue(TvSelector::selectByBoth));

	/* ======================================================================== */
	/*                             Action grouping                              */
	/* ======================================================================== */
	actGrTvSelectBy = new QActionGroup(this);
	PONAM(actGrTvSelectBy)->setExclusive(true);

	QList<QAction *> acts;
	acts << PONAM(actSelByNone) << PONAM(actSelByGrBx)
		  << PONAM(actSelByVPort) << PONAM(actSelByBoth);

	foreach (QAction *act, acts) {
		actGrTvSelectBy->addAction(act);
		act->setCheckable(true);
	}

	connect(actGrTvSelectBy, &QActionGroup::triggered, this, &Browser::onActGroupTrigd);

	/* ======================================================================== */
	actGrTvCount = new QActionGroup(this);
	PONAM(actGrTvCount)->setExclusive(true);

	for (int k = 3; k < 9; k++) {
		actGrTvCount->addAction(new QAction(tr(" %1 ").arg(k), actGrTvCount));
		actGrTvCount->actions().last()->setCheckable(true);
		actGrTvCount->actions().last()->setObjectName(tr("actTvCountN_%1").arg(k));
	}
	actGrTvCount->setExclusive(true);

	connect(actGrTvCount, &QActionGroup::triggered, this, &Browser::onActGroupTrigd);

	//	QAction *muSep_2	= setTvCntMu->addSeparator();
	//	muGrAct->addAction(PONAM(muSep_2));
}
bool Browser::restoreColumnOrderAndVisability(MdTable *tv) {
	Q_UNUSED(tv)
	return true;
}
MdMenu *Browser::menuBarElement() {
	browsMenu = new MdMenu(tr("&Browser"));

	MdMenu *tvSelectByMenu = browsMenu->addMenu(tr("Table selector config"));
	PONAM(tvSelectByMenu)->addActions(actGrTvSelectBy->actions());

	MdMenu *tvCountMenu = browsMenu->addMenu(tr("Table View count config"));
	PONAM(tvCountMenu)->addActions(actGrTvCount->actions());

	return browsMenu;
}
void Browser::createUi(QWidget *passParent) {
	/*!
	 * Build UI.
	 */

	grLay = new QGridLayout(passParent);
	mConnectionWidget = new ConnectionWidget(passParent);

	mTabs.tva = new MdTable(passParent);
	mTabs.tvb = new MdTable(passParent);
	mTabs.tvc = new MdTable(passParent);
	mTabs.tvd = new MdTable(passParent);
	mTabs.tvl1 = new MdTable(passParent);
	mTabs.tvl2 = new MdTable(passParent);

	mTabs.tva->setObjectName(tr("tva"));
	mTabs.tvb->setObjectName(tr("tvb"));
	mTabs.tvc->setObjectName(tr("tvc"));
	mTabs.tvd->setObjectName(tr("tvd"));
	mTabs.tvl1->setObjectName(tr("tvl1"));
	mTabs.tvl2->setObjectName(tr("tvl2"));

	splitter = new QSplitter(passParent);
	splitter_2 = new QSplitter(passParent);
	splitter_3 = new QSplitter(passParent);
	splitter_4 = new QSplitter(passParent);
	splitter_5 = new QSplitter(passParent);
	splitter_6 = new QSplitter(passParent);
	splitter_7 = new QSplitter(passParent);

	PONAM(splitter);
	PONAM(splitter_2);
	PONAM(splitter_3);
	PONAM(splitter_4);
	PONAM(splitter_5);
	PONAM(splitter_6);
	PONAM(splitter_7);

	splitter_7->setOrientation(Qt::Horizontal);
	splitter->setOrientation(Qt::Horizontal);
	splitter_2->setOrientation(Qt::Horizontal);
	splitter_5->setOrientation(Qt::Horizontal);
	splitter_3->setOrientation(Qt::Vertical);
	splitter_4->setOrientation(Qt::Vertical);
	splitter_6->setOrientation(Qt::Vertical);

	grLay->addWidget(splitter);
	grLay->addWidget(splitter_2);
	grLay->addWidget(splitter_3);
	grLay->addWidget(splitter_4);
	grLay->addWidget(splitter_5);
	grLay->addWidget(splitter_6);
	grLay->addWidget(splitter_7);

	splitter->addWidget(mTabs.tva);
	splitter->addWidget(mTabs.tvb);
	splitter_2->addWidget(mConnectionWidget);
	splitter_2->addWidget(mTabs.tvd);
	splitter_3->addWidget(splitter);
	splitter_3->addWidget(splitter_2);

	splitter_4->addWidget(splitter_3);

	splitter_5->addWidget(mTabs.tvl1);
	splitter_5->addWidget(mTabs.tvl2);
	splitter_6->addWidget(splitter_5);
	splitter_6->addWidget(mTabs.tvc);
	splitter_7->addWidget(splitter_4);
	splitter_7->addWidget(splitter_6);


	QList<QString> accName = mTabs.tvIds();

	foreach (MdTable *tv, mTabs.tvsNoPtr()) {
		QString currentName = accName.takeFirst();
		tv->setAccessibleName( currentName );
		tv->setObjectName( currentName );

		tv->setToolTip( tr("Tooltip: ") + currentName);
		tv->setContextMenuPolicy(Qt::ActionsContextMenu);
		tv->grBox()->installEventFilter( this );
		tv->tv()->viewport()->installEventFilter( this );
		tv->grBox()->setObjectName("gb");
	}
}
QTableView* Browser::createView(QSqlQueryModel *model, const QString &title) {
	QTableView *view = new QTableView(0);
	view->setModel(model);
	static int offset = 0;

	view->setWindowTitle(title);
	view->move(100 + offset, 100 + offset);
	offset += 20;
	view->show();

	return view;
}
bool Browser::restoreActionObjects () {
	QSETTINGS;

	/*!
	 * This restore loop handles only QAction objects which are kind of QActionGroup.
	 */
	QList<QActionGroup *> actGroups = findChildren<QActionGroup *>();

	foreach (QActionGroup *actGrp, actGroups) {
		actGrp->blockSignals(true);

		foreach (QAction *a, actGrp->actions()) {
			if (! config.allKeys().join(',').contains(a->objectName()))
				continue;

			a->setChecked(config.value(objectName() + tr("/") + a->objectName(), false).toBool());

			if (a->isChecked()) {
				if (a->actionGroup() == actGrTvSelectBy) {
					setTvSelector(a->data().value<TvSelector>());
					continue;
				}
				if (a->actionGroup() == actGrTvCount) {
					//					setTvCount....
					continue;
				}
			}
		}

		actGrp->blockSignals(false);
	}

	/*!
	 * Create list of ungrouped QAction objects.
	 */
	QList<QAction *> ungroupedActs;
	foreach (QAction * a, findChildren<QAction *>(QRegularExpression("act*")))
		if (! a->actionGroup())
			ungroupedActs << a;

	if (! ungroupedActs.isEmpty())
		INFO << tr("Ungrouped QAction objects:\n")
			  << listObjectNames<QAction *>(ungroupedActs)->join("\n");

	return true;
}
/* ======================================================================== */
/*                             Helper methodes                              */
/* ======================================================================== */
void Browser::storeActionState(QAction *sender) {
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
void Browser::selectAndSetFont() {
	if (mTabs.currentSelected(true) != NULL)
		mTabs.currentSelected()->storeFont(
					QFontDialog::getFont(&ok, mTabs.currentSelected()->tv()->font(), this));
	else {
		/*!
		 * If no tv is selected, configure all tvs.
		 */
		QFont f = QFontDialog::getFont(&ok, QFont("Helvetica [Cronyx]", 10), this);
		foreach (MdTable *_tv, mTabs.tvsNoPtr())
			_tv->storeFont(f);
	}
}
void Browser::selectAndSetRowColor() {
	if (mTabs.currentSelected(true) != NULL) {
		QPalette pal = mTabs.currentSelected()->tv()->palette();
		QColor color = QColorDialog::getColor(
								pal.background().color(), this,
								tr("Farbe für alternierenden Zeilenhintergrund"),
								QColorDialog::DontUseNativeDialog);

		if (color.isValid())
			mTabs.currentSelected()->setAlternateRowCol(color, true);
	}
	else {
		/*!
		 * If no tv is selected, configure all tvs.
		 */
		QPalette pal = mTabs.tva->tv()->palette();
		QColor color = QColorDialog::getColor(
								pal.background().color(), this,
								tr("Farbe für alternierenden Zeilenhintergrund"),
								QColorDialog::DontUseNativeDialog);

		if (color.isValid())
			foreach (MdTable *tv, mTabs.tvsNoPtr())
				tv->setAlternateRowCol(color, true);

	}
}
void Browser::resetColumnConfig() {
	if (mTabs.currentSelected(true) != NULL) {
		mTabs.currentSelected()->resetColumnsDefaultPos(true);
		mTabs.currentSelected()->removeColumnsConfig();
		mTabs.currentSelected()->modelCast()->resetModelSrcs();
	}
	else {
		foreach (MdTable *tv, mTabs.tvsNoPtr()) {
			tv->resetColumnsDefaultPos(true);
			tv->removeColumnsConfig();
			tv->modelCast()->resetModelSrcs();
		}
	}
}

#define QFOLDINGSTART {
#ifndef COMMENT_OUT_UNUSED
QAbstractItemModel *Browser::createMailModel(QObject *parent) {
	QStandardItemModel *model = new QStandardItemModel(0, 3, parent);

	model->setHeaderData(0, Qt::Horizontal, QObject::tr("Subject"));
	model->setHeaderData(1, Qt::Horizontal, QObject::tr("Sender"));
	model->setHeaderData(2, Qt::Horizontal, QObject::tr("Date"));

	stateBar->showMessage(tr("createMailModel"));

	//    addMail(model, "Happy New Year!", "Grace K. <grace@software-inc.com>",
	//            QDateTime(QDate(2006, 12, 31), QTime(17, 03)));
	//    addMail(model, "Radically new concept", "Grace K. <grace@software-inc.com>",
	//            QDateTime(QDate(2006, 12, 22), QTime(9, 44)));
	//    addMail(model, "Accounts", "pascale@nospam.com",
	//            QDateTime(QDate(2006, 12, 31), QTime(12, 50)));
	//    addMail(model, "Expenses", "Joe Bloggs <joe@bloggs.com>",
	//            QDateTime(QDate(2006, 12, 25), QTime(11, 39)));
	//    addMail(model, "Re: Expenses", "Andy <andy@nospam.com>",
	//            QDateTime(QDate(2007, 01, 02), QTime(16, 05)));
	//    addMail(model, "Re: Accounts", "Joe Bloggs <joe@bloggs.com>",
	//            QDateTime(QDate(2007, 01, 03), QTime(14, 18)));
	//    addMail(model, "Re: Accounts", "Andy <andy@nospam.com>",
	//            QDateTime(QDate(2007, 01, 03), QTime(14, 26)));
	//    addMail(model, "Sports", "Linda Smith <linda.smith@nospam.com>",
	//            QDateTime(QDate(2007, 01, 05), QTime(11, 33)));
	//    addMail(model, "AW: Sports", "Rolf Newschweinstein <rolfn@nospam.com>",
	//            QDateTime(QDate(2007, 01, 05), QTime(12, 00)));
	//    addMail(model, "RE: Sports", "Petra Schmidt <petras@nospam.com>",
	//            QDateTime(QDate(2007, 01, 05), QTime(12, 01)));

	return model;
}

#endif
#define QFOLDINGEND }
