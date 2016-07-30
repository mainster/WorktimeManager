#include "browser.h"
#include "ui_browser.h"

#define T_CYCLIC 250e-3

#define	COMMENT_OUT_UNUSED

#define TVA    "TableLeft"
#define TVB    "TableCenter"
#define TVC    "TableRight"
#define TVD    "TableBottom"
#define TVL2    "TableLong1"
#define TVL1    "TableLong2"

Browser *Browser::inst = 0;


Browser::Browser(QWidget *parent) : QWidget(parent),
	ui(new Ui::Browser), cyclicObjInfo(false) {
	setupUi(this);  inst = this;

	initTableView( inst, QStringList() << TVA << TVB << TVC << TVD << TVL1 << TVL2);

	foreach (TabView *tv, tvs)
		if (! (bool)  connect(this, SIGNAL(tabViewSelChanged(TabView *)),
									 tv,   SLOT(onTabViewSelChanged(TabView *)))) {
			qReturn("Connection fail!");
		}

	INFO << QSqlDatabase::connectionNames()
		  << QSqlDatabase::drivers();

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

	SqlRtm *cm = new SqlRtm();
	cm->setCenterCols(QVector<int>(0, 1));

	filterForm = SortWindow::getInstance();
	filterForm->setWindowTitle(tr("The window title ") + filterForm->objectName());
	filterForm->hide();
}
Browser::~Browser() {
	delete ui;
}
#define QFOLDINGSTART {
#ifdef OLD_CONSTRUCTOR

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
									  tvs[0],   SLOT(onTabViewSelChanged(TabView *))));
	bl.append( (bool) connect(this,     SIGNAL(tabViewSelChanged(TabView *)),
									  tvs[1],   SLOT(onTabViewSelChanged(TabView *))));
	bl.append( (bool) connect(this,     SIGNAL(tabViewSelChanged(TabView *)),
									  tvs[2],   SLOT(onTabViewSelChanged(TabView *))));
	bl.append( (bool) connect(this,     SIGNAL(tabViewSelChanged(TabView *)),
									  tvs[3],   SLOT(onTabViewSelChanged(TabView *))));
	bl.append( (bool) connect(this,     SIGNAL(tabViewSelChanged(TabView *)),
									  tvs[4],   SLOT(onTabViewSelChanged(TabView *))));
	bl.append( (bool) connect(this,     SIGNAL(tabViewSelChanged(TabView *)),
									  tvs[5],   SLOT(onTabViewSelChanged(TabView *))));

	if (bl.contains(false))
		INFO << tr("connect.tabViewSelChanged[0...6]:") << bl;

	INFO << QSqlDatabase::connectionNames()
		  << QSqlDatabase::drivers();

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
void Browser::initTableView(QWidget *parent, QStringList &accNam) {
	Q_UNUSED(parent)
	QList<QAction *> tblActs;

	tblActs.append(insertRowAction);
	tblActs.append(deleteRowAction);
	tblActs.append(fieldStrategyAction);
	tblActs.append(rowStrategyAction);
	tblActs.append(manualStrategyAction);
	tblActs.append(submitAction);
	tblActs.append(revertAction);
	tblActs.append(selectAction);

	//    QObjectList *queryList( const char *inheritsClass = 0,
	//                            const char *objName = 0,
	//                            bool regexpMatch = TRUE,
	//                            bool recursiveSearch = TRUE );

	//    QObjectList *queryTvs = queryList( "TabView" );

	//    foreach (QObject *obj, queryTvs) {
	//        tvs.append( static_cast<TabView *>obj );
	//    }
	tvs.append( tva );
	tvs.append( tvb );
	tvs.append( tvc );
	tvs.append( tvd );
	tvs.append( tvl1 );
	tvs.append( tvl2 );

	int i = 0;

	foreach (TabView *tv, tvs) {
		tv->addActions( tblActs );
		tv->setAccessibleName( accNam[i] );
		tv->setObjectName( accNam[i] );

		tv->setToolTip( tr("Tooltip: ") + accNam[i++]);
		tv->setContextMenuPolicy(Qt::ActionsContextMenu);
		tv->grBox()->installEventFilter( this );
		tv->grBox()->setObjectName("gb");
		//      tv->grBox()->setStyleSheet( Globals::gbStyleShtCenterPROPERTYS );
	}
}
#define QFOLDINGEND }
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
	INFO << tvs.length() << tvs;

	/** Select "tableMain" as target table view widget */
	QTableView *tvResp = tvl1->tv();
	QSqlQueryModel *model = new QSqlQueryModel(tvResp);
	QSqlQuery query;

	if (! nonDefaulQuery.isEmpty())
		query = QSqlQuery( nonDefaulQuery, pdb);
	else {
		/**
		 * Use a QTextEdit to transform a query saved as html into
		 * corresponding plain text query
		 */
		QTextEdit *te = new QTextEdit();

		foreach (QString str, configQ.allKeys()) {
			if (str.contains("default_worktime_query")) {
				QString s = configQ.value(str, "").toString();
				te->setHtml( configQ.value("InpFrm/" + s, "").toString() );
			}
		}
		query = QSqlQuery(te->toPlainText(), pdb);
	}

	model->setQuery( query );
	tvResp->setModel( model );

	if (model->lastError().type() != QSqlError::NoError)
		emit stateMsg(model->lastError().text());
	else if (model->query().isSelect())
		emit stateMsg(tr("Query OK."));
	else
		emit stateMsg(tr("Query OK, number of affected rows: %1").arg(
							  model->query().numRowsAffected()));

	tvResp->setVisible( false );
	tvResp->resizeColumnsToContents();
	tvResp->resizeRowsToContents();
	tvResp->setVisible( true );
	updateActions();

}
void Browser::exec() {
	InpFrm *inpFrm = InpFrm::instance();

	QTableView *tvq = tvl1->tv();
	QSqlQueryModel *model = new QSqlQueryModel(tvq);

	model->setQuery(QSqlQuery(inpFrm->getQueryText(),
									  connectionWidget->currentDatabase()));
	tvq->setModel(model);

	if (model->lastError().type() != QSqlError::NoError)
		emit stateMsg(model->lastError().text());
	else if (model->query().isSelect())
		emit stateMsg(tr("Query OK."));
	else
		emit stateMsg(tr("Query OK, number of affected rows: %1").arg(
							  model->query().numRowsAffected()));

	tvq->setSortingEnabled( true );
	tvq->setVisible( false );
	tvq->resizeColumnsToContents();
	tvq->resizeRowsToContents();
	tvq->setVisible( true );
	updateActions();
}
/**
 * Relational table model - View relational WORKS 12-11-2015
 */
void Browser::showRelatTable(const QString &tNam, TabView *tvc) {
	/**
	 * Get active database pointer
	 */
	QSqlDatabase pDb = connectionWidget->currentDatabase();
	QTableView *tv = tvc->tv();
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
			rmod->setHeaderData(rmod->fieldIndex("FehlID"), Qt::Horizontal, tr("ID"));

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
	tv->setColumnHidden(rmod->fieldIndex("ID"), true);
	tv->setSelectionMode(QAbstractItemView::ContiguousSelection);

	tv->setModel(rmod);
	tv->setEditTriggers( QAbstractItemView::DoubleClicked |
								QAbstractItemView::EditKeyPressed );

	/** !!!!!!!!!!!!!!!!!!!! tvs.first()->tv()->selectionModel() changed to
	 * tvs[3]->tv()->selectionModel()
	 */

	if (! (bool) connect( tv->selectionModel(),
								 SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
								 this, SLOT(currentChanged(QModelIndex,QModelIndex)) ) )
		emit stateMsg(tr("Slot connection returns false"));

	tv->setVisible( false );
	tv->resizeColumnsToContents();
	tv->resizeRowsToContents();
	tv->setVisible( true );

	updateActions();
	/**
	 * Update TabViews group box title to match the prior activated sql
	 * table name
	 */
	tvc->grBox()->setTitle( tNam );
}
void Browser::showMetaData(const QString &t) {

	QSqlRecord rec = connectionWidget->currentDatabase().record(t);
	QStandardItemModel *model = new QStandardItemModel(tvs.first()->tv());

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

	tvs.first()->tv()->setModel(model);
	tvs.first()->tv()->setEditTriggers(QAbstractItemView::NoEditTriggers);

	updateActions();
}
void Browser::insertRow(/*QTableView &tv*/) {
	/**
	  * Loop through every child of your QSplitter and call hasFocus() for
	  * each one so you can know which of them has the focus (keyboard focus)
	  */
	//   QObjectList objs(this->spHorzPrim->children());

	for (int i = 0; i < tvs.length(); i++) {
		//      QTableView *childa = qobject_cast<QTableView *>(objs.at(i));

		if ( tvs.at(i)->hasFocus() )
			INFO << "table" << i << "has focus";
	}

	TabView *locTabView = qobject_cast<TabView *>( tvs.first() );
	QSqlTableModel *model =
			qobject_cast<QSqlTableModel *>(locTabView->tv()->model());

	if (!model)
		return;

	QModelIndex insertIndex = locTabView->tv()->currentIndex();
	int row = insertIndex.row() == -1 ? 0 : insertIndex.row();
	model->insertRow(row);
	insertIndex = model->index(row, 0);
	locTabView->tv()->setCurrentIndex(insertIndex);
	locTabView->tv()->edit(insertIndex);

	QWidget *wid = QWidget::focusWidget();
	INFO << wid->accessibleName();

	wid = QApplication::focusWidget(),
			INFO << wid->accessibleName();
}
void Browser::deleteRow(/*QTableView &tv*/) {
	TabView *locTabView = qobject_cast<TabView *>( tvs.first() );
	QSqlTableModel *model =
			qobject_cast<QSqlTableModel *>(locTabView->tv()->model());

	if (!model)
		return;

	QModelIndexList currentSelection =
			locTabView->tv()->selectionModel()->selectedIndexes();

	for (int i = 0; i < currentSelection.count(); ++i) {
		if (currentSelection.at(i).column() != 0)
			continue;

		model->removeRow(currentSelection.at(i).row());
	}

	updateActions();
}
void Browser::updateActions(/*QTableView &tv*/) {
	/**
	 * Loop thru all available TableViews
	 */
	foreach (TabView *tvl, tvs) {
		//      QSqlRelationalTableModel *tm = qobject_cast
		//            <QSqlRelationalTableModel *>( locTabView->tv()->model() );
		QSqlTableModel * tm = qobject_cast<QSqlTableModel *>(tvl->tv()->model());

		bool enableIns = tm;
		bool enableDel = enableIns && tvl->tv()->currentIndex().isValid();

		insertRowAction->setEnabled( enableIns );
		deleteRowAction->setEnabled( enableDel );

		fieldStrategyAction  ->setEnabled(tm);
		rowStrategyAction    ->setEnabled(tm);
		manualStrategyAction ->setEnabled(tm);
		submitAction         ->setEnabled(tm);
		revertAction         ->setEnabled(tm);
		selectAction         ->setEnabled(tm);

		if (tm) {
			QSqlRelationalTableModel::EditStrategy es = tm->editStrategy();
			fieldStrategyAction->setChecked(
						es == QSqlRelationalTableModel::OnFieldChange);
			rowStrategyAction->setChecked(
						es == QSqlRelationalTableModel::OnRowChange);
			manualStrategyAction->setChecked(
						es == QSqlRelationalTableModel::OnManualSubmit);
		}
	}
}
void Browser::customMenuRequested(QPoint pos) {
	QModelIndex index = this->tvs.first()->tv()->indexAt(pos);
	Q_UNUSED(index);

	QMenu *menu = new QMenu(this);
	menu->addAction(new QAction("Action 1", this));
	menu->addAction(new QAction("Action 2", this));
	menu->addAction(new QAction("Action 3", this));
	menu->popup(tvs.first()->tv()->viewport()->mapToGlobal(pos));
}
void Browser::on_fieldStrategyAction_triggered() {
	TabView *locTabView = qobject_cast<TabView *>( tvs.first() );
	QSqlTableModel *tm =
			qobject_cast<QSqlTableModel *>(locTabView->tv()->model());

	if (tm)
		tm->setEditStrategy(QSqlTableModel::OnFieldChange);

	on_selectAction_triggered();
}
void Browser::on_rowStrategyAction_triggered() {
	TabView *locTabView = qobject_cast<TabView *>( tvs.first() );
	QSqlTableModel *tm = qobject_cast<QSqlTableModel *>(
									locTabView->tv()->model());

	if (tm)
		tm->setEditStrategy(QSqlTableModel::OnRowChange);

	QWidget *wid = focusWidget();
	INFO << wid->objectName();
}
void Browser::on_manualStrategyAction_triggered() {
	TabView *locTabView = qobject_cast<TabView *>( tvs.first() );
	QSqlTableModel *tm = qobject_cast<QSqlTableModel *>(
									locTabView->tv()->model());

	if (tm)
		tm->setEditStrategy(QSqlTableModel::OnManualSubmit);
}
void Browser::on_submitAction_triggered() {
	TabView *locTabView = qobject_cast<TabView *>( tvs.first() );
	QSqlTableModel *tm = qobject_cast<QSqlTableModel *>(
									locTabView->tv()->model());

	if (tm)
		tm->submitAll();
}
void Browser::on_revertAction_triggered() {
	TabView *locTabView = qobject_cast<TabView *>( tvs.first() );
	QSqlTableModel *tm = qobject_cast<QSqlTableModel *>(
									locTabView->tv()->model());

	if (tm)
		tm->revertAll();
}
void Browser::on_selectAction_triggered() {
	TabView *locTabView = qobject_cast<TabView *>( tvs.first() );
	QSqlTableModel *tm = qobject_cast<QSqlTableModel *>(
									locTabView->tv()->model());

	if (tm)
		tm->select();
}
void Browser::onBeforeUpdate(int row, QSqlRecord &record) {
	INFO << row;
	INFO << record;
}

/**
 * Entered after double clicking a table item in connection widget tree view
 */
void Browser::on_connectionWidget_tableActivated(const QString &sqlTbl) {
	/**
	 * Determine which TabView should be targeted by the activated tab model.
	 * Either select an instance of TabView which hasn't a model loaded yet. To
	 *
	 * swap sql tables during runtime, a double-mouse-click event is captured
	 * and the user-selected tv instance becomes addressed by the event handler.
	 */
	foreach (TabView *tv, tvs) {
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
			tv->grBox()->setTitle( sqlTbl );
			return;
		}
		else {
			/**
			 * If no table widget is selected actively, use the tvc from tvs which has
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
int  Browser::setFontAcc(QFont &font) {
	this->tva->tv()->setFont(font);
	return 0;
}
void Browser::autofitRowCol() {
	foreach (TabView *tvc, tvs) {
		tvc->tv()->setVisible( false );
		tvc->tv()->resizeColumnsToContents();
		tvc->tv()->resizeRowsToContents();
		tvc->tv()->setVisible( true );
	}
}
void Browser::initializeMdl(QSqlQueryModel *model) {
	QSqlDatabase pDb = connectionWidget->currentDatabase();
	model->setQuery("select * from worker", pDb);
	model->setHeaderData(0, Qt::Horizontal, QObject::tr("workerID"));
	model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nachname"));
	model->setHeaderData(2, Qt::Horizontal, QObject::tr("Vorname"));
	INFO << model->lastError().databaseText();
	INFO << model->lastError().driverText();
	INFO << model->lastError();

	////   mdl->setQuery("SELECT prjID, clientID, SubID, Beschreibung, Kurzform "
	////                 "FROM prj", pDb);
	//   mdl->setQuery("SELECT * FROM prj");
	//   mdl->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
	//   mdl->setHeaderData(1, Qt::Horizontal, QObject::tr("clId"));
	//   mdl->setHeaderData(2, Qt::Horizontal, QObject::tr("subId"));
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
bool Browser::eventFilter(QObject *obj, QEvent *e) {
	QPoint pos;

	if (e->type() == QEvent::MouseButtonPress) {
		INFO << "obj parent name:" << obj->parent()->objectName();

		QMouseEvent *me = static_cast<QMouseEvent *>(e);
		pos = me->pos();

		QWidget *widget = qApp->widgetAt(QCursor::pos());

		if (widget != 0x00) {
			INFO << widget->parentWidget()->objectName();
			INFO << widget->objectName();
		}

		/**
			* If event receiver object has one of the tabview accessnames...
			* ... reset all tabviews from beeing selected and mark the obj
			* obj->parent() after static casting to TabView...
			*/
		if ( (((QStringList) TVA << TVB << TVC << TVD << TVL1 << TVL2).join(','))
			  .contains( obj->parent()->objectName()) ) {

			/**
				 * Reset....
				 */
			TabView *tv = static_cast<TabView *>(obj->parent());

			/**
				 * Unselect if view was selected earlier
				 */
			if (QVariant(tv->grBox()->property("select")).toBool()) {
				tv->grBox()->setProperty("select", false);
				emit tabViewSelChanged( 0 );
			} else {
				tv->grBox()->setProperty("select", true);
				tv->grBox()->setStyleSheet(tv->grBox()->styleSheet());
				emit tabViewSelChanged(tv);
			}
		}
		else {
			INFO << tr("event receiver obj has NON of the known table names!  ")
				  <<  widget->parentWidget()->objectName();
		}

		if (false) {
			QList<QObject *> objs = obj->children();

			for (int i = 0; i < objs.length(); i++) {
				INFO << "obj child no." << i << objs[i]->objectName();
			}
		}

		return true;
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

	return false;

	/**
	  * standard event processing
	  */
	return QObject::eventFilter(obj, e);
}
void Browser::hideEvent(QShowEvent *) {
	QSPLT_STORE;
	emit visibilityChanged( false );
}
void Browser::showEvent(QShowEvent *) {
	QSPLT_RESTORE;
	emit visibilityChanged( true );
//	QSETTINGS;

//	foreach (QSplitter *sp, findChildren<QSplitter *>()) {
//		QString objn = sp->objectName();
//		sp->restoreState(config.value("Browser/" + objn, " ").toByteArray());
//	}
}
void Browser::onActFilterForm(bool b) {
	filterForm->setVisible(b);

	if (b) {
		filterForm->setSourceModel( tvs.last()->tv()->model() );
		filterForm->raise();
		filterForm->activateWindow();
	}
	else {
		filterForm->hide();
	}
}
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
