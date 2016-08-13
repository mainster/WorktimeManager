#include "browser.h"

#define T_CYCLIC 250e-3
#define	COMMENT_OUT_UNUSED

struct Browser::mTabs_t Browser::mTabs = Browser::mTabs_t();
Browser *Browser::inst = 0;
#define QFOLDINGSTART {
const QString Browser::browserStyleSheet = QString(
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
															 ""
															 "QGroupBox::title {"
															 "   	subcontrol-origin: margin; /* margin boarder padding content */"
															 "   	subcontrol-position: top center; /* position at the top center */"
															 "   	top: 1.2ex;   "
															 "  	padding: 0px 8px"
															 "}"
															 ""
															 "QGroupBox::title:hover {"
															 "    color: rgba(235, 235, 235, 255);"
															 "}"
															 ""
															 " QTableView{"
															 "	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #F0F0F0, stop: 1 #FFFFFF);"
															 "	border: 0px solid gray;"
															 "	border-radius: 5px;"
															 "	margin-top: 15px; /* leave space at the top for the title */"
															 " }"
															 ""
															 "QTableView[select=false]{ background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #F0F0F0, stop: 1 #FFFFFF); }"
															 "QGroupBox[select=false]{ border: 1px solid; }"
															 ""
															 "QTableView[select=true]{ background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #C0C0C0, stop: 1 #FFFFFF); }"
															 "QGroupBox[select=true]{ border: 3px solid; }"
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
															 " /* style the sort indicator "
															 "QHeaderView::down-arrow {"
															 "	image: url(:/images/down_arrow.png);"
															 "}"
															 ""
															 "QHeaderView::up-arrow {"
															 "	image: url(:/images/up_arrow.png);"
															 "}*/");
#define QFOLDINGEND }

/* ======================================================================== */
/*                             Browser::Browser                             */
/* ======================================================================== */
Browser::Browser(QWidget *parent)
	: QWidget(parent), cyclicObjInfo(false) {
	inst = this;
	setObjectName("Browser");

	createUi(this);
	connect(connectionWidget,	&ConnectionWidget::tableActivated,
			  this,					&Browser::onConnectionWidgetTableActivated);

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

	//	SqlRtm *cm = new SqlRtm();
	//	cm->setCenterCols(QVector<int>(0, 1));

	filterForm = SortWindow::getInstance();
	filterForm->setWindowTitle(tr("The window title ") + filterForm->objectName());
	filterForm->hide();

	QTimer::singleShot(50, this, SLOT(restoreUi()));
}
Browser::~Browser() {
	INFO << tr("close browser!");
	//	delete ui;
}

void Browser::requeryWorktimeTableView(QString nonDefaulQuery) {
	QSETTINGS_QUERYS;
	/*!
	 * Get pointer to current active database connection.
	 */
	QSqlDatabase pdb = connectionWidget->currentDatabase();

	/*!
	 * Get list of all table views and requery all of them with gb name "worktime".
	 */
	QList<TabView *> tvs = findChildren<TabView *>();
	foreach (TabView *tv, tvs) {
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
	emit updateActions();

}
void Browser::exec() {
	InpFrm *inpFrm = InpFrm::instance();

	QSqlQueryModel *model = new QSqlQueryModel(mTabs.tvl1->tv());

	model->setQuery(QSqlQuery(inpFrm->getQueryText(),
									  connectionWidget->currentDatabase()));
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
	emit updateActions();
}
void Browser::showRelatTable(const QString &tNam, TabView *tvc) {
	/**
	 * Relational table model - View relational WORKS 12-11-2015
	 */

	/**
	 * Get active database pointer
	 */
	QSqlDatabase pDb = connectionWidget->currentDatabase();
	//	QTableView *tv = tvc->tv();
	QSqlRelationalTableModel *rmod = new SqlRtm(tvc->tv(), pDb);
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

	// Set the model and hide the ID column
	tvc->setColumnHidden(rmod->fieldIndex("ID"), false);
	tvc->setSelectionMode(QAbstractItemView::ContiguousSelection);

	tvc->setModel(rmod);
	tvc->setEditTriggers( QAbstractItemView::DoubleClicked |
								 QAbstractItemView::EditKeyPressed );

	/** !!!!!!!!!!!!!!!!!!!! tvs()->first()->tv()->selectionModel() changed to
	  * mTvs[3]->tv()->selectionModel()
	 */

	if (! (bool) connect( tvc->selectionModel(),
								 SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
								 this, SLOT(currentChanged(QModelIndex,QModelIndex)) ) )
		emit stateMsg(tr("Slot connection returns false"));

	//	tv->setVisible( false );
	//	tv->resizeColumnsToContents();
	//	tv->resizeRowsToContents();
	//	tv->setVisible( true );
	tvc->resizeRowsColsToContents();

	//    emit updateActions();
	/**
	 * Update TabViews group box title to match the prior activated sql
	 * table name
	 */
	tvc->grBox()->setTitle( tNam );
}
void Browser::showMetaData(const QString &t) {

	QSqlRecord rec = connectionWidget->currentDatabase().record(t);
	QStandardItemModel *model = new QStandardItemModel(tvs()->first()->tv());

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

	mTabs.tvs()->first()->tv()->setModel(model);
	mTabs.tvs()->first()->tv()->setEditTriggers(QAbstractItemView::NoEditTriggers);

	emit updateActions();
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
void Browser::onActFilterForm(bool b) {
	filterForm->setVisible(b);

	if (b) {
		filterForm->setSourceModel( tvs()->last()->tv()->model() );
		filterForm->raise();
		filterForm->activateWindow();
	}
	else {
		filterForm->hide();
	}
}
void Browser::onConnectionWidgetTableActivated(const QString &sqlTbl) {
	/**
	 * Entered after double clicking a table item in connection widget tree view
	 */

	/**
	 * Determine which TabView should be targeted by the activated tab model.
	 * Either select an instance of TabView which hasn't a model loaded yet. To
	 *
	 * swap sql tables during runtime, a double-mouse-click event is captured
	 * and the user-selected tv instance becomes addressed by the event handler.
	 */
	foreach (TabView *tv, mTabs.tvsNoPtr()) {
		/**
		 * Check for pending active select requests.
		 */
		if (tv->isActiveSelected()) {
			/**
			 * If selected view is TableMain AND the activated connection widget
			 * table is "tagesberichte" then select the "relational table model"
			 *
			 * 18-11-2015  Alles auf relational table model umgebaut
			 */
			showRelatTable( sqlTbl, tv );
			/**
				 * Update TabViews group box title to match the prior activated sql
				 * table name
				 */
			//			tv->grBox()->setTitle( sqlTbl );
			return;
		}
		else {
			/**
				 * If no table widget is selected actively, use the tvc from mTvs which has
			 * currently no model loaded
			 */
			if (tv->tv()->model() == 0x00) {
				showRelatTable(sqlTbl, tv);
				/**
				 * Update TabViews group box title to match the prior activated sql
				 * table name
				 */
				tv->grBox()->setTitle( sqlTbl );
				return;
			}
			INFO << tr("No model-less table view widget found!");
		}
	}
}
void Browser::autofitRowCol() {
	foreach (TabView *tvc, mTabs.tvsNoPtr())
		tvc->resizeRowsColsToContents();
}
void Browser::onCyclic() {

	//   currentChanged();
	/* ---------------------------------------------------------------- */
	/*         Get the name of object under the mouse pointer           */
	/* ----------------------------------------------------------------*/
	if (cyclicObjInfo) {
		QWidget *widget = qApp->widgetAt(QCursor::pos());

		if (widget != 0x00) {
			QString widName = widget->objectName();
			if (!widName.isEmpty())
				INFO << widName << widget->children();
		}
	}
}
void Browser::onTvSelectorChanged() {
	/*! Notify signal via proberty class */
	INFO << tr("m_tvSelector:") << tvSelector();
	/**** Safe all action states from Browser
	\*/
	//	Globals::ACTION_STORE(this);
}
void Browser::onActMenuTrigd(bool state) {
	Q_UNUSED(state)

	QAction *actSender = qobject_cast<QAction *>(sender());
	QList<QActionGroup *> muGroups = browsMenu->findChildren<QActionGroup *>();

	if (false) {
		INFO << actSender->objectName()
			  << tr("%1").arg(actSender->text())
			  << actSender->isChecked();
	}

	/*! Find actSenders corresponding action group */
	if (muGroups.at( muGroups.indexOf(actSender->actionGroup()) )
		 ->objectName().contains(tr("muGrSel"))) {
		setTvSelector(actSender->data().value<TvSelector>());
	}
}
int  Browser::setFontAcc(QFont &font) {
	mTabs.tva->tv()->setFont(font);
	return 0;
}
QFont Browser::selectFont() {
	bool ok;
	QFont font = QFontDialog::getFont(
						 &ok, QFont("Helvetica [Cronyx]", 10), this);

	if (ok) {
		INFO << font;
	} else {
		INFO << font;
	}

	return QFont();
}
/* ======================================================================== */
/*                              Event handler                               */
/* ======================================================================== */
bool Browser::eventFilter(QObject *obj, QEvent *e) {

	if (e->type() == QEvent::MouseButtonPress) {
		TabView *tv = NULL;

		foreach (TabView *_tv, mTabs.tvsNoPtr())
			switch (tvSelector()) {
				case selectByViewPort: {
					if (_tv->tv()->viewport() == obj)
						tv = _tv;
				}; break;

				case selectByGroupBox: {
					if (_tv->grBox() == obj)
						tv = _tv;
				}; break;

				case selectByBoth: {
					if ((_tv->grBox() == obj) ||
						 (_tv->tv()->viewport() == obj))
						tv = _tv;
				}; break;

				default:	break;
			}


		if (tv != NULL) {
			if (mTabs.tvIds().join(',').contains(tv->objectName())) {

				/**
			 * Unselect if view was selected earlier
			 */
				if (QVariant(tv->grBox()->property("select")).toBool()) {
					tv->grBox()->setProperty("select", false);
					emit tabViewSelChanged( 0 );
				}
				else {
					tv->grBox()->setProperty("select", true);
					tv->grBox()->setStyleSheet(tv->grBox()->styleSheet());
					emit tabViewSelChanged(tv);
				}

				return true;
			}
			else {
				INFO << tr("event receiver obj has NON of the known table names!  ");
			}
		}
	}

	if ((e->type() == QEvent::KeyPress) && false) {
		QKeyEvent *kev = static_cast<QKeyEvent *>(e);
		INFO << "Keypress " << kev->key() << "by obj:" ;
		INFO << "obj name:" << obj->objectName();
		INFO << "obj parent name:" << obj->parent()->objectName();

		QList<QObject *> objs = obj->children();

		for (int i = 0; i < objs.length(); i++) {
			INFO << "obj child no." << i << objs[i]->objectName();
			INFO << "obj child no." << i << objs[i]->isWidgetType();
		}

	}

	/**
	  * standard event processing
	  */
	return QObject::eventFilter(obj, e);
}
void Browser::showEvent(QShowEvent *) {
	SPLT_RESTORE(this);
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

	/**** Write tabView <-> SQL table relations
	  \*/
	foreach (TabView *tv, mTabs.tvsNoPtr())
		config.setValue(objectName() + "/" + tv->objectName(), tv->grBox()->title());

	/**** Safe all action states from Browser
	\*/
	ACTION_STORE(this, tr("act*"));
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

	/**** Restore tabview <-> SQL table assignements, but only if valid keys are stored
	 \*/

	foreach (TabView *tv, mTabs.tvsNoPtr()) {
		if (! config.allKeys().join(',').contains( tv->objectName() ))
			continue;

		QString tabNam = config.value(objectName() + tr("/") + tv->objectName()).toString();
		tv->restoreView();
		showRelatTable( tabNam, tv );
	}
	/**** Restore all action states from Browser
	\*/
	ACTION_RESTORE(this, tr("act*"));
	return ret;
}
QMenu *Browser::menuBarElement() {
	browsMenu = new QMenu(tr("Browser"));
	QMenu *tvSelCfgMu = browsMenu->addMenu(tr("Table selector config"));

	QActionGroup *muGrSel = new QActionGroup(browsMenu),
			*muGrAct = new QActionGroup(browsMenu);

	QAction *muSelByNone = tvSelCfgMu->addAction("Select by None", this, &Browser::onActMenuTrigd),
			*muSelByGrBx = tvSelCfgMu->addAction("Select by Group Box", this, &Browser::onActMenuTrigd),
			*muSelByVport = tvSelCfgMu->addAction("Select by View Port", this, &Browser::onActMenuTrigd),
			*muSelByBoth = tvSelCfgMu->addAction("Select by Both", this, &Browser::onActMenuTrigd),
			*muSep_1 = tvSelCfgMu->addSeparator();
	muSelByNone->setData(QVariant::fromValue(TvSelector::selectByNone));
	muSelByGrBx->setData(QVariant::fromValue(TvSelector::selectByGroupBox));
	muSelByVport->setData(QVariant::fromValue(TvSelector::selectByViewPort));
	muSelByBoth->setData(QVariant::fromValue(TvSelector::selectByBoth));
	muGrSel->addAction(PONAM(muSelByNone));
	muGrSel->addAction(PONAM(muSelByGrBx));
	muGrSel->addAction(PONAM(muSelByVport));
	muGrSel->addAction(PONAM(muSelByBoth));
	muGrSel->addAction(PONAM(muSep_1));

	foreach (QAction *act, muGrSel->actions())
		act->setCheckable(true);
	muGrSel->setExclusive(true);

	/* ======================================================================== */

	QMenu *setTvCntMu = browsMenu->addMenu(tr("Table View count config"));

	for (int k = 3; k < 9; k++) {
		muGrAct->addAction(setTvCntMu->addAction(tr(" %1 ").arg(k), this, &Browser::onActMenuTrigd));
		muGrAct->actions().last()->setCheckable(true);
		muGrAct->actions().last()->setObjectName(tr("muTvsN_%1").arg(k));
	}
	muGrAct->setExclusive(true);

	QAction *muSep_2	= setTvCntMu->addSeparator();
	muGrAct->addAction(PONAM(muSep_2));

	PONAM(muGrSel);
	PONAM(muGrAct);
	PONAM(tvSelCfgMu);
	PONAM(setTvCntMu);

	return browsMenu;
}
void Browser::createUi(QWidget *passParent) {
	/*!
	 * Build UI.
	 */

	grLay = new QGridLayout(passParent);
	connectionWidget = new ConnectionWidget();

	mTabs.tva = new TabView();
	mTabs.tvb = new TabView();
	mTabs.tvc = new TabView();
	mTabs.tvd = new TabView();
	mTabs.tvl1 = new TabView();
	mTabs.tvl2 = new TabView();

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
	splitter_2->addWidget(connectionWidget);
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

	foreach (TabView *tv, mTabs.tvsNoPtr()) {
		QString currentName = accName.takeFirst();
		tv->setAccessibleName( currentName );
		tv->setObjectName( currentName );

		tv->setToolTip( tr("Tooltip: ") + currentName);
		tv->setContextMenuPolicy(Qt::ActionsContextMenu);
		tv->grBox()->installEventFilter( this );
		tv->tv()->viewport()->installEventFilter( this );
		tv->grBox()->setObjectName("gb");

		/*!
		  * Connect TabView Signals/Slots
		  */
		if (! (bool)  connect(this, &Browser::tabViewSelChanged, tv, &TabView::onTabViewSelChanged))
			qReturn("Connection fail!");
		if (! (bool)  connect(this, &Browser::updateActions, tv, &TabView::onUpdateActions))
			qReturn("Connection fail!");
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
void Browser::ACTION_STORE(QObject *obj, QString regex) {
	QSETTINGS;
	QList<QAction *> acts = findChildren<QAction *>(QRegularExpression("act*"));

	foreach (QAction *act, acts)
		config.setValue(obj->objectName() + QString("/") + act->objectName(), act->isChecked());

	config.setValue(obj->objectName() + QString("/actionCtr"), acts.length());
	config.sync();
}
void Browser::ACTION_RESTORE(QObject *obj, QString regex) {
	QSETTINGS;
	QList<QAction *> acts = findChildren<QAction *>(QRegularExpression("act*"));

	qSort(acts.begin(), acts.end());
	foreach (QAction *act, acts)
		act->setChecked(config.value(obj->objectName() + QString("/") + act->objectName()).toBool());
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
#define QFOLDINGSTART {
#ifndef COMMENT_OUT_UNUSED

void Browser::BrowserOld(QWidget *parent) : QWidget(parent), ui(new Ui::Browser) {
	setupUi(this);

	inst = this;

	QStringList accNam;
	accNam << TVA << TVB << TVC << TVD << TVL1 << TVL2;
	initTableView( inst, accNam);

	//   connect (ui->btnA,      SIGNAL(clicked()),
	//            this,          SLOT(onTestBtnClicked()));
	//   connect (ui->btnB,      SIGNAL(clicked()),
	//            this,          SLOT(onTestBtnClicked()));
	QList<bool> bl;

	bl.append( (bool) connect(this,     SIGNAL(tabViewSelChanged(TabView *)),
									  mTabs.tvsNoPtr()[0],   SLOT(onTabViewSelChanged(TabView *))));
	bl.append( (bool) connect(this,     SIGNAL(tabViewSelChanged(TabView *)),
									  mTabs.tvsNoPtr()[1],   SLOT(onTabViewSelChanged(TabView *))));
	bl.append( (bool) connect(this,     SIGNAL(tabViewSelChanged(TabView *)),
									  mTabs.tvsNoPtr()[2],   SLOT(onTabViewSelChanged(TabView *))));
	bl.append( (bool) connect(this,     SIGNAL(tabViewSelChanged(TabView *)),
									  mTabs.tvsNoPtr()[3],   SLOT(onTabViewSelChanged(TabView *))));
	bl.append( (bool) connect(this,     SIGNAL(tabViewSelChanged(TabView *)),
									  mTabs.tvsNoPtr()[4],   SLOT(onTabViewSelChanged(TabView *))));
	bl.append( (bool) connect(this,     SIGNAL(tabViewSelChanged(TabView *)),
									  mTabs.tvsNoPtr()[5],   SLOT(onTabViewSelChanged(TabView *))));

	if (bl.contains(false))
		INFO << tr("connect.tabViewSelChanged[0...6]:") << bl;

	INFO << QSqlDatabase::connectionNames()
		  << QSqlDatabase::drivers();

	if (QSqlDatabase::drivers().isEmpty()) {
		QMessageBox::information
				(this, tr("No database drivers found"),
				 tr("This demo requires at least one Qt database driver..."));
		qApp->quit();
	}
	else {
		timCyc = new QTimer();
		timCyc->setInterval(T_CYCLIC * 1e3);
		connect( timCyc,    SIGNAL(timeout()),
					this,      SLOT(onCyclic()));
		timCyc->start();
	}

	emit stateMsg(tr("Browser Ready."));

	SqlTm *cm = new SqlTm();
	cm->setCCol(QVector<int>(0, 1));

	//    sortwindow.append( new SortWindow(0) );
	//    sortwindow.append( new SortWindow(0) );
	filterForm = SortWindow::getInstance();

	//    sortwindow.last()->setVisible(false);
	filterForm->setVisible(false);
}

#endif
#define QFOLDINGEND }

