#include "inpfrm.h"
#include "ui_inpfrm.h"

#define QUERYPREFIX  tr("CUSTOM_QUERY_")
#define NEWQUERY     tr("NewQuery")

class MdComboBox;

/* ======================================================================== */
/*                              InpFrm::InpFrm                              */
/* ======================================================================== */
InpFrm* InpFrm::inst = 0;
InpFrm::InpFrm(QWidget *parent) :
	QDockWidget(parent), ui(new Ui::InpFrm), mEscapeTrigger(false) {
	ui->setupUi(this);
	inst = this;

	connectActions();

	ui->cbQueryIdent->setDuplicatesEnabled( false );
	ui->datePicker->setDate(QDate::currentDate());
	ui->gbSqlQuery->hide();
	this->setFixedHeight(125);

	WIN_RESTORE(this);

	initComboboxes();
	restoreTabOrder();

//	ui->gboxWorker->hide();
	/* ======================================================================== */

#ifdef SELF0
	QSqlRelationalTableModel *sqlTblMdl = new QSqlRelationalTableModel(this);
	QSortFilterProxyModel *proxyMdl = new QSortFilterProxyModel(this);
	proxyMdl->setDynamicSortFilter(true);
	sqlTblMdl->setEditStrategy(QSqlTableModel::OnManualSubmit);

	sqlTblMdl->setTable( tr("prj") );

	/** Set the relations to the other database tables */
	sqlTblMdl->setRelation(sqlTblMdl->fieldIndex("clientID"),
								  QSqlRelation("client", "clientID", "Name"));

	/** Set the localized header captions */
	sqlTblMdl->setHeaderData(sqlTblMdl->fieldIndex("prjID"), Qt::Horizontal, tr("ID"));
	sqlTblMdl->setHeaderData(sqlTblMdl->fieldIndex("clientID"), Qt::Horizontal, tr("Kunde, Name"));
	sqlTblMdl->select();
	/** Set the source model (sql table model) for the filter proxy model objects. */
	proxyMdl->setSourceModel( sqlTblMdl );
	proxyMdl->setFilterKeyColumn(-1);

	ui->testCb->setModel( proxyMdl );
	connect(ui->testLe, &QLineEdit::returnPressed, this, &InpFrm::onTestLeChanged);
#endif
#ifdef SELF
	QSqlRelationalTableModel *sqlTblMdl = new QSqlRelationalTableModel(this);
	SortFilterProxyModel *proxyMdl = new SortFilterProxyModel(this);
	proxyMdl->setDynamicSortFilter(true);
	sqlTblMdl->setEditStrategy(QSqlTableModel::OnManualSubmit);

	sqlTblMdl->setTable( tr("prj") );

	/** Set the relations to the other database tables */
	sqlTblMdl->setRelation(sqlTblMdl->fieldIndex("clientID"),
								  QSqlRelation("client", "clientID", "Name"));

	/** Set the localized header captions */
	sqlTblMdl->setHeaderData(sqlTblMdl->fieldIndex("prjID"), Qt::Horizontal, tr("ID"));
	sqlTblMdl->setHeaderData(sqlTblMdl->fieldIndex("clientID"), Qt::Horizontal, tr("Kunde, Name"));
	sqlTblMdl->select();

	/** Set the source model (sql table model) for the filter proxy model objects. */
	proxyMdl->setSourceModel( sqlTblMdl );

	QTableView *_tv = new QTableView();
	_tv->setModel(sqlTblMdl);
	_tv->show();
	ui->testCb->setModel( sqlTblMdl );

	ui->testCb->setModelColumn( /*proxyMdl->filterKeyColumn()*/0 );
	connect(ui->testLe, &QLineEdit::returnPressed, this, &InpFrm::onTestLeChanged);
#endif
}
InpFrm::~InpFrm() {
	WIN_STORE(this);

	this->removeEventFilter(this);

	foreach (QWidget *w, mSqlCbs)
		w->removeEventFilter(this);

	delete ui;
}
void InpFrm::initComboboxes() {
	/*!
	 * Initialize sql combobox list member mSqlCbs.
	 */
	mSqlCbs = findChildren<MdComboBox *>();


	foreach (MdComboBox *cb, mSqlCbs) {
		if (! cb->property("hasSqlMapper").isValid())
			mSqlCbs.removeOne(cb);
		else {
			/*!
			 * Install custom event filter object for SQL mapping purposes.
			 */
			cb->installEventFilter( new SqlEventFilter(this) );			
		}
	}

	mModels.append( fieldGroup_t() );
	mModels.append( fieldGroup_t() );
	mModels.append( fieldGroup_t() );

	mModels[IDX_PROJECT].tableModel->setTable(tr("prj"));
	mModels[IDX_CLIENT].tableModel->setTable(tr("client"));
	mModels[IDX_WORKER].tableModel->setTable(tr("worker"));

	mModels[IDX_PROJECT].proxyModel->setSourceModel(mModels.at(IDX_PROJECT).tableModel);
	mModels[IDX_CLIENT].proxyModel->setSourceModel(mModels.at(IDX_CLIENT).tableModel);
	mModels[IDX_WORKER].proxyModel->setSourceModel(mModels.at(IDX_WORKER).tableModel);

	mModels[IDX_PROJECT].setModelColumns(QList<quint8>() << 1 << 4, ui->cbPrjKurzform);
	mModels[IDX_CLIENT].setModelColumns(QList<quint8>() << 1 << 3, ui->cbClientKurzform);
	mModels[IDX_WORKER].setModelColumns(QList<quint8>() << 1 << 2 << 3, ui->cbWorkerNachname);

}
void InpFrm::initComboboxes2() {
	QList<QSqlTableModel *> tms;
	QList<QDataWidgetMapper *> dms;

	/** Request all combobox widgets within the inputFrm */
//	QList<QComboBox *> cbLst = findChildren<QComboBox *>();

	/*!
	 * Remove all comboboxes which are not based on sql list models
	 */
//	cbLst.removeOne(ui->cbQueryIdent);

	/*!
	  * QSqlTableModel is a high-level interface for reading and writing database
	  * records from a single table. It is built on top of the lower-level QSqlQuery and can
	  * be used to provide data to view classes such as QTableView.
	  * http://doc.qt.io/qt-5/qsqltablemodel.html#details
	  */
#ifdef INIT2
	prjmd = new QSqlTableModel();
	clmd = new QSqlTableModel();
	wkmd = new QSqlTableModel();
	prjmd->setTable("prj");
	clmd->setTable("client");
	wkmd->setTable("worker");

	prjm = new QDataWidgetMapper();
	clm = new QDataWidgetMapper();
	wkm = new QDataWidgetMapper();
	prjm->setModel( prjmd );
	clm->setModel( clmd );
	wkm->setModel( wkmd );

	prjm->addMapping(ui->cbPrjNummer,      prjmd->fieldIndex("Nummer"));
	prjm->addMapping(ui->cbPrjKurzform,    prjmd->fieldIndex("Kurzform"));
	clm->addMapping( ui->cbClientNummer,   clmd->fieldIndex("Nummer"));
	clm->addMapping( ui->cbClientKurzform, clmd->fieldIndex("Kurzform"));
	wkm->addMapping( ui->cbWorkerPersonalNr,  wkmd->fieldIndex("PersonalNr"));
	wkm->addMapping( ui->cbWorkerNachname,    wkmd->fieldIndex("Nachname"));
	wkm->addMapping( ui->cbWorkerVorname,     wkmd->fieldIndex("Vorname"));

	prjm->toFirst();
	clm->toFirst();
	wkm->toFirst();
#endif
	//   QList<QString> tbRls;
	//   QStringList tLst, kLst;

	//   tLst << "prj" << "client" << "worker";

	//   for (int i=0; i<3; i++) {
	//      tms.append( new QSqlTableModel() );
	//      tms.last()->setTable(tLst.at(i));
	//      dms.append( new QDataWidgetMapper() );
	//      dms.last()->setModel( tms.last() );
	//   }
	////   int k=0;

	////   dms[0]->addMapping(ui->cbPrjNummer, tms[0]->fieldIndex("Nummer"));
	////   dms[0]->addMapping(ui->cbPrjNummer, tms[0]->fieldIndex("Nummer"));

	////   while (tbRls[k++].contains("prj",Qt::CaseInsensitive)) {
	////   }
	////   while (tbRls[k++].contains("client",Qt::CaseInsensitive)) {
	////      dms[1]->addMapping();
	////   }
	////   while (tbRls[k++].contains("worker",Qt::CaseInsensitive)) {
	////      dms[2]->addMapping();
	////   }

	//   foreach (QComboBox *cbx, cbLst) {
	//      kLst << cbx->objectName();
	//      if (kLst.last().contains("prj",Qt::CaseInsensitive)) {
	//         tbRls.append( tr("prj,%1").arg(kLst.last().remove("cbPrj")));
	//         continue;
	//      }
	//      if (kLst.last().contains("client",Qt::CaseInsensitive)) {
	//         tbRls.append( tr("client,%1").arg(kLst.last().remove("cbClient")));
	//         continue;
	//      }
	//      if (kLst.last().contains("worker",Qt::CaseInsensitive)) {
	//         tbRls.append( tr("worker,%1").arg(kLst.last().remove("cbWorker")));
	//         continue;
	//      }
	//   }

	//   int i=0, n0=0, n1=0, n2=0;
	//   foreach (QString s, tbRls) {
	//      if (s.contains("prj")) {
	//         dms[0]->addMapping(cbLst[i], tms[0]->fieldIndex(kLst[i]));
	//         i++;
	////         INFO << tms[i-1] << kLst[i-1];
	//         continue;
	//      }

	//      if (s.contains("client")) {
	//         dms[1]->addMapping(cbLst[i], tms[1]->fieldIndex(kLst[i]));
	//         i++;
	////         INFO << tms[i-1] << kLst[i-1];
	//         continue;
	//      }

	//      if (s.contains("worker")) {
	//         dms[2]->addMapping(cbLst[i], tms[2]->fieldIndex(kLst[i]));
	//         i++;
	////         INFO << tms[i-1] << kLst[i-1];
	//         continue;
	//      }

	//   }

	//   for (int k=0; k<3; k++)
	//      dms[k]->toFirst();









	//   dms[k]->addMapping( );
	//   foreach (QComboBox *cbx, cbLst) {
	//      //      INFO << cbx->objectName() << cbx->accessibleName();

	//      tms.append( new QSqlTableModel(this) );
	//      tms.last()->setTable( tbRls.at(k).split(",").first() );
	//      tms.last()->setEditStrategy(QSqlTableModel::OnManualSubmit);
	//      tms.last()->select();

	//      fpms.append( new QSortFilterProxyModel() );
	//      fpms.last()->setSourceModel( tms.last() );
	//      fpms.last()->setFilterKeyColumn(
	//               tms.last()->fieldIndex( tbRls.at(k).split(",").last()) );
	//      cbLst[k]->setModel( fpms.last() );
	//      cbLst[k++]->setModelColumn( fpms.last()->filterKeyColumn() );
	//   }
}
QString InpFrm::getQueryText() const {
	return ui->teSqlQuerys->toPlainText();
}
void InpFrm::connectActions() {

	connect(ui->datePicker,         SIGNAL(dateChanged(QDate)),
			  this,                   SLOT(onInpFormChanges(QDate)));
	connect(ui->sbID,               SIGNAL(valueChanged(int)),
			  this,                   SLOT(onInpFormChanges(int)));
	connect(ui->teSqlQuerys,        SIGNAL(textChanged()),
			  this,                   SLOT(onSqlQuerysTextChanged()));
	connect(ui->cbQueryIdent,       SIGNAL(currentIndexChanged(int)),
			  this,                   SLOT(onCbQueryIndexChaned(int)));

	connect(this, &InpFrm::changeFocusOrder, this, &InpFrm::onChangeFocusOrder);

	QList<QComboBox*> cbs = findChildren<QComboBox *>();
	QList<QPushButton*> pbs = findChildren<QPushButton *>();

	foreach (QComboBox *cb, cbs)
		connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(onInpFormChanges(int)));

	foreach (QPushButton *pb, pbs) {
		if (pb->objectName() == tr("btnSaveQuery")) {
			connect(pb, &QPushButton::clicked, this, &InpFrm::saveSqlQueryInputText);
			continue;
		}

		if (pb->objectName() == tr("btnRestoreQuery")) {
			connect(pb, &QPushButton::clicked, this, &InpFrm::restoreSqlQueryInputText);
			continue;
		}

		if (! (bool)connect(pb, &QPushButton::clicked, this, &InpFrm::aButtonClick))
			CRIT << tr("connect(..) returned false or button %1").arg(pb->objectName());
	}



	//	connect(ui->btnSubmitQuery, &QPushButton::clicked, this, &InpFrm::onTest);
	//    connect(ui->btnSubmitQuery,     SIGNAL(clicked()),
	//            browser,                SLOT(exec()));

}
void InpFrm::onInpFormUserCommit() {
	QSqlQuery query;
	QString q;
	bool ok = true;

	/** Query actual count of records in worktime table */
	//    int worktimRowCount = 0;
	//    query.exec("SELECT worktimID, COUNT(*) FROM worktime;");
	//    if (query.lastError().type() != QSqlError::NoError) {
	//        QMessageBox::warning(this, "Fehler bei SQL Query",
	//                             "query.lastError().type() != QSqlError::NoError");
	//        return;
	//    }
	//    INFO << tr("query size:") << query.size();
	//    if (query.next())
	//        worktimRowCount = query.value(0).toInt(&ok);
	//    else
	//        emit stateMessage(tr("No \"next query\" result"));
	//    if (! ok)
	//        emit stateMessage(tr("toInt() of query result fails"));

	/** Query ID column and find max value. This MAY not be equal
	  * to actual count
	  * ------------------------ */
	QList<int> ints;
	query.exec("SELECT worktimID FROM worktime");
	if (query.lastError().type() != QSqlError::NoError) {
		QMessageBox::warning(this, "Fehler bei SQL Query",
									"query.lastError().type() != QSqlError::NoError");
		return;
	}
	while (query.next()) {
		ints.append( query.value(0).toInt(&ok) );
		if (! ok)
			emit stateMessage(tr("toInt() of query result fails"), 2000);
	}

	/** Find the most high worktimID entry value */
	qStableSort(ints.begin(), ints.end());
	int worktimID_max = ints.last();

	/** Find workerID based on InpFrm ui selections
	 * --------------------------------------------- */
	q = QString("SELECT workerID FROM worker WHERE Nachname = \"%1\" "
					"AND PersonalNr = %2")
		 .arg( ui->cbWorkerNachname->currentText() )
		 .arg( ui->cbWorkerPersonalNr->currentText().toInt() );
	query.exec( q );

	int workerID = -1;
	while (query.next()) {
		/**
		 * If workerID != -1, this is the second loop entry but workerID
		 * must be unique! --> Error
		 */
		if (workerID > 0) {
			QMessageBox::warning(this, "Fehler bei SQL Query",
										"Non-unique workerID detected!");
			return;
		}
		else {
			workerID = query.value(0).toInt();
		}
	}

	/** Find prjID based on InpFrm ui selections
	 * --------------------------------------------- */
	q = QString("SELECT prjID FROM prj WHERE Kurzform = \"%1\" "
					"AND Nummer = %2")
		 .arg( ui->cbPrjKurzform->currentText() )
		 .arg( ui->cbPrjNummer->currentText().toInt() );
	query.exec( q );

	int prjID = -1;
	while (query.next()) {
		/**
		 * If prjID != -1, this is the second loop entry but prjID
		 * must be unique! --> Error
		 */
		if (prjID > 0) {
			QMessageBox::warning(this, "Fehler bei SQL Query",
										"Non-unique prjID detected!");
			return;
		}
		else {
			prjID = query.value(0).toInt();
		}
	}


	/** Prepare query for new record */
	query.prepare("INSERT INTO worktime (worktimID, dat, workerID, prjID, hours) "
					  "VALUES (:worktimID, :dat, :workerID, :prjID, :hours)");

	query.bindValue(":worktimID",   ++ worktimID_max);
	query.bindValue(":dat",         ui->datePicker->date().toString("yyyy-MM-dd"));
	//    query.bindValue(":dat",         ui->datePicker->date().toString(Qt::LocaleDate));
	//    query.bindValue(":dat", ui->datePicker->date().toString(Qt::SystemLocaleShortDate));
	query.bindValue(":workerID",    workerID);
	query.bindValue(":prjID",       prjID);
	query.bindValue(":hours",       ui->leHrs->text().toInt());
	query.exec();
	if (query.lastError().type() != QSqlError::NoError) {
		QMessageBox::warning(this, "Fehler bei SQL Query",
									QString("Error type:" + query.lastError().driverText() +
											  "\nin final add-record query"));
		return;
	}

	//    browser->requeryWorktimeTableView(tr(""));
	emit requeryTableView();
	INFO.noquote() << query.lastQuery();

}
void InpFrm::aButtonClick(bool) {
	QPushButton *pbSender = qobject_cast<QPushButton *>(QObject::sender());

	if (pbSender == ui->btnSaveQuery) { saveSqlQueryInputText(); return; }
	if (pbSender == ui->btnRestoreQuery) { restoreSqlQueryInputText(); return; }
	if (pbSender == ui->btnSubmitQuery) {
		Browser::instance()->exec();
	}
	if (pbSender == ui->btnClear) {
//		foreach (MdComboBox *mcbx, QList<MdComboBox*>(listCast<MdComboBox*,QComboBox*>(mSqlCbs))) {
//			mcbx->makePermanentView(true);
//		}


	}
	if (pbSender == ui->btnOk) {
		QList<QWidget *> ws;
		ws << ui->datePicker
			<< ui->cbPrjKurzform
			<< ui->cbPrjNummer
			<< ui->cbClientKurzform
			<< ui->cbClientNummer
			<< ui->cbWorkerVorname
			<< ui->cbWorkerNachname
			<< ui->cbWorkerPersonalNr
			<< ui->leHrs;


		setFocusOrder( ws );
	}
	if (pbSender == ui->btnUndo) {
		ui->teSqlQuerys->show();
		ui->teSqlQuerys->setEnabled(true);
		ui->teSqlQuerys->setReadOnly(false);
		ui->teSqlQuerys->setText(tr("testtext"));
//		ui->teSqlQuerys->setWr
	}
	if (pbSender == ui->btnRedo) {
//		Browser *browser = Browser::instance();
//		browser->on_connectionWidget_metaDataRequested("prj");

		mSqlCbs[0]->setModel( mModels.at(IDX_PROJECT).tableModel );
		mSqlCbs[0]->setModelColumns(QList<quint8>() << 1 << 4);

		QModelIndex mix = mModels.at(IDX_PROJECT).tableModel->index(0,0,QModelIndex());
		INFO << mModels.at(IDX_PROJECT).tableModel->rowCount(QModelIndex())
			  << mModels.at(IDX_PROJECT).tableModel->columnCount(QModelIndex())
			  << mModels.at(IDX_PROJECT).tableModel->rowCount(mix)
			  << mModels.at(IDX_PROJECT).tableModel->columnCount(mix);
	}
}
void InpFrm::mapCbTableProxyOld() {
	/*!
	 * Creat a string list which contains the table name. For easy switching possibility
	 * within a group box (for example):
	 *  Worker:		firstname,	lastname
	 *  Worker:		lastname,	firstname
	 * ... it is necessary to do the following string operations:
	 */
	QList<QString> modelNames;
	foreach (QComboBox *cbx, mSqlCbs) {
		if (cbx->objectName().contains("client",Qt::CaseInsensitive)) {
			modelNames <<  tr("client,%1").arg(cbx->objectName().remove("cbClient"));
			continue;
		}
		if (cbx->objectName().contains("prj",Qt::CaseInsensitive)) {
			modelNames <<  tr("prj,%1").arg(cbx->objectName().remove("cbPrj"));
			continue;
		}
		if (cbx->objectName().contains("worker",Qt::CaseInsensitive)) {
			modelNames <<  tr("worker,%1").arg(cbx->objectName().remove("cbWorker"));
			continue;
		}
	}

	/*!
	 * Create "sql table model"- and "sort filter proxy model" for each
	 * element of mSqlCbs.
	 */
	int k=0;
	foreach (QComboBox *cbx, mSqlCbs ) {
		QSqlTableModel *sqlTblMdl = new QSqlTableModel(this);
		QSortFilterProxyModel *proxyMdl = new QSortFilterProxyModel(this);
		sqlTblMdl->setEditStrategy(QSqlTableModel::OnManualSubmit);

		sqlTblMdl->setTable( modelNames.at(k).split(",").at(0) );
//		INFO << tr("sqlTblMdl->setTable(%1)").arg( modelNames.at(k).split(",").at(0) );

		sqlTblMdl->select();

		/*!
		 * Set the source model (sql table model) for the filter proxy model objects.
		 */
		proxyMdl->setSourceModel( sqlTblMdl );
		/*!
		 * Select the column which should be provided by the proxy model.
		 */
		proxyMdl->setFilterKeyColumn(sqlTblMdl->fieldIndex( modelNames.at(k).split(",").at(1)) );
//		INFO << tr("setFilterKeyColumn(%1)").arg( modelNames.at(k).split(",").at(1) );

		cbx->setModel( proxyMdl );
		mSqlCbs[k++]->setModelColumn( proxyMdl->filterKeyColumn() );
	}

	/** Map comboBox indexes */
	connect(ui->cbClientKurzform, SIGNAL(currentIndexChanged(int)),
			  ui->cbClientNummer,   SLOT(setCurrentIndex(int)));
	connect(ui->cbClientNummer,   SIGNAL(currentIndexChanged(int)),
			  ui->cbClientKurzform, SLOT(setCurrentIndex(int)));

	connect(ui->cbPrjNummer,      SIGNAL(currentIndexChanged(int)),
			  ui->cbPrjKurzform,    SLOT(setCurrentIndex(int)));
	connect(ui->cbPrjKurzform,    SIGNAL(currentIndexChanged(int)),
			  ui->cbPrjNummer,      SLOT(setCurrentIndex(int)));

	connect(ui->cbWorkerPersonalNr,  SIGNAL(currentIndexChanged(int)),
			  ui->cbWorkerNachname,    SLOT(setCurrentIndex(int)));
	connect(ui->cbWorkerPersonalNr,  SIGNAL(currentIndexChanged(int)),
			  ui->cbWorkerVorname,     SLOT(setCurrentIndex(int)));
	connect(ui->cbWorkerNachname,    SIGNAL(currentIndexChanged(int)),
			  ui->cbWorkerPersonalNr,  SLOT(setCurrentIndex(int)));
	connect(ui->cbWorkerNachname,    SIGNAL(currentIndexChanged(int)),
			  ui->cbWorkerVorname,     SLOT(setCurrentIndex(int)));
	connect(ui->cbWorkerVorname,     SIGNAL(currentIndexChanged(int)),
			  ui->cbWorkerPersonalNr,  SLOT(setCurrentIndex(int)));
	connect(ui->cbWorkerVorname,     SIGNAL(currentIndexChanged(int)),
			  ui->cbWorkerNachname,    SLOT(setCurrentIndex(int)));

	/**
	 * Set CUSTOM_QUERY_COMMANDS combo box items
	 */
	QSETTINGS_QUERYS
			QStringList query_keys;

	//   /** Remove all items except "NewQuery" item */
	//   for (int i=0; i < ui->cbQueryIdent->count(); i++) {
	//      if (! ui->cbQueryIdent->itemText(i).contains(NEWQUERY))
	//         ui->cbQueryIdent->removeItem(i);
	//   }
	ui->cbQueryIdent->clear();

	foreach (QString str, configQ.allKeys()) {
		if (true /*str.contains( QUERYPREFIX )*/)
			query_keys << str.split('/').at(1);

		//      INFO << "Query key:" << query_keys.last();
		//      INFO << configQ.value(str).toString();

		ui->cbQueryIdent->addItem(query_keys.last(),
										  configQ.value(str).toString());

	}

	ui->cbQueryIdent->setDuplicatesEnabled( false );

}
void InpFrm::mapCbTableProxy() {
	mModels[IDX_PROJECT].setModelColumns(QList<quint8>() << 1 << 4, mSqlCbs.first());
	return;
	/*!
	 * Creat a string list which contains the table name. For easy switching possibility
	 * within a group box (for example):
	 *  Worker:		firstname,	lastname
	 *  Worker:		lastname,	firstname
	 * ... it is necessary to do the following string operations:
	 */
	QList<QString> modelNames;
	foreach (QComboBox *cbx, mSqlCbs) {
		if (cbx->objectName().contains("client",Qt::CaseInsensitive)) {
			modelNames <<  tr("client,%1").arg(cbx->objectName().remove("cbClient"));
			continue;
		}
		if (cbx->objectName().contains("prj",Qt::CaseInsensitive)) {
			modelNames <<  tr("prj,%1").arg(cbx->objectName().remove("cbPrj"));
			continue;
		}
		if (cbx->objectName().contains("worker",Qt::CaseInsensitive)) {
			modelNames <<  tr("worker,%1").arg(cbx->objectName().remove("cbWorker"));
			continue;
		}
	}

	INFO << modelNames;
	/*!
	 * Create "sql table model"- and "sort filter proxy model" for each
	 * element of mSqlCbs.
	 */
	int k=0;
	foreach (QComboBox *cbx, mSqlCbs ) {
		QSqlTableModel *sqlTblMdl = new QSqlTableModel(this);
		QSortFilterProxyModel *proxyMdl = new QSortFilterProxyModel(this);
		sqlTblMdl->setEditStrategy(QSqlTableModel::OnManualSubmit);

		sqlTblMdl->setTable( modelNames.at(k).split(",").at(0) );
//		INFO << tr("sqlTblMdl->setTable(%1)").arg( modelNames.at(k).split(",").at(0) );

		sqlTblMdl->select();

		/*!
		 * Set the source model (sql table model) for the filter proxy model objects.
		 */
		proxyMdl->setSourceModel( sqlTblMdl );
		/*!
		 * Select the column which should be provided by the proxy model.
		 */
		proxyMdl->setFilterKeyColumn(sqlTblMdl->fieldIndex( modelNames.at(k).split(",").at(1)) );
//		INFO << tr("setFilterKeyColumn(%1)").arg( modelNames.at(k).split(",").at(1) );

		cbx->setModel( proxyMdl );
		mSqlCbs[k++]->setModelColumn( proxyMdl->filterKeyColumn() );
	}

	/** Map comboBox indexes */
	connect(ui->cbClientKurzform, SIGNAL(currentIndexChanged(int)),
			  ui->cbClientNummer,   SLOT(setCurrentIndex(int)));
	connect(ui->cbClientNummer,   SIGNAL(currentIndexChanged(int)),
			  ui->cbClientKurzform, SLOT(setCurrentIndex(int)));

	connect(ui->cbPrjNummer,      SIGNAL(currentIndexChanged(int)),
			  ui->cbPrjKurzform,    SLOT(setCurrentIndex(int)));
	connect(ui->cbPrjKurzform,    SIGNAL(currentIndexChanged(int)),
			  ui->cbPrjNummer,      SLOT(setCurrentIndex(int)));

	connect(ui->cbWorkerPersonalNr,  SIGNAL(currentIndexChanged(int)),
			  ui->cbWorkerNachname,    SLOT(setCurrentIndex(int)));
	connect(ui->cbWorkerPersonalNr,  SIGNAL(currentIndexChanged(int)),
			  ui->cbWorkerVorname,     SLOT(setCurrentIndex(int)));
	connect(ui->cbWorkerNachname,    SIGNAL(currentIndexChanged(int)),
			  ui->cbWorkerPersonalNr,  SLOT(setCurrentIndex(int)));
	connect(ui->cbWorkerNachname,    SIGNAL(currentIndexChanged(int)),
			  ui->cbWorkerVorname,     SLOT(setCurrentIndex(int)));
	connect(ui->cbWorkerVorname,     SIGNAL(currentIndexChanged(int)),
			  ui->cbWorkerPersonalNr,  SLOT(setCurrentIndex(int)));
	connect(ui->cbWorkerVorname,     SIGNAL(currentIndexChanged(int)),
			  ui->cbWorkerNachname,    SLOT(setCurrentIndex(int)));

	/**
	 * Set CUSTOM_QUERY_COMMANDS combo box items
	 */
	QSETTINGS_QUERYS
			QStringList query_keys;

	//   /** Remove all items except "NewQuery" item */
	//   for (int i=0; i < ui->cbQueryIdent->count(); i++) {
	//      if (! ui->cbQueryIdent->itemText(i).contains(NEWQUERY))
	//         ui->cbQueryIdent->removeItem(i);
	//   }
	ui->cbQueryIdent->clear();

	foreach (QString str, configQ.allKeys()) {
		if (true /*str.contains( QUERYPREFIX )*/)
			query_keys << str.split('/').at(1);

		//      INFO << "Query key:" << query_keys.last();
		//      INFO << configQ.value(str).toString();

		ui->cbQueryIdent->addItem(query_keys.last(),
										  configQ.value(str).toString());

	}

	ui->cbQueryIdent->setDuplicatesEnabled( false );

}
void InpFrm::onInpFormChanges(int idx) {
	Q_UNUSED(idx);
	//   INFO << idx;
}
void InpFrm::onInpFormChanges(QDate date) {
	Q_UNUSED(date);
	//   INFO << date;
}

void InpFrm::onSqlQuerysTextChanged() {
	/**
	 * Check if teSqlQuery hasFocus to determine if the textCHanged signal
	 * was emitted in consequence of a Query restore (no new cb item) or
	 * in cons. of an user text input/change
	 */
	if (! ui->teSqlQuerys->hasFocus())
		return;

	/**
	 * generate new item only if there is no "New query" item at 0.
	 * If there is already a "New query" item, update item data only.
	 */
	if (! ui->cbQueryIdent->itemText(0).contains(NEWQUERY))
		ui->cbQueryIdent->insertItem(0, NEWQUERY,
											  ui->teSqlQuerys->toHtml());
	else
		ui->cbQueryIdent->setItemData(0, ui->teSqlQuerys->toHtml());

	ui->cbQueryIdent->setCurrentIndex(0);
}
void InpFrm::setSqlQueryTextboxVisible(bool visible) {
	ui->gbSqlQuery->setVisible( visible );

	(visible) ? (setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
														QSizePolicy::Expanding)),
					 ui->teSqlQuerys->setFocusPolicy(Qt::ClickFocus),
					 setMaximumHeight(300))
				 : (setFixedHeight(125));
}
void InpFrm::onCbQueryIndexChaned(int idx) {
	Q_UNUSED(idx);
	ui->teSqlQuerys->setHtml( ui->cbQueryIdent->currentData().toString() );
}

void InpFrm::saveSqlQueryInputText() {
	/**
 * Different SQL query commands could be saved in config file. A unique query
 * command identifier string could be set. All custom query cmds becomes
 * prefixed by CUSTOM_QUERY_.
 */
	QSETTINGS_QUERYS;
	QSETTINGS;
	bool ok; int k = 0;

	INFO << CUSTOM_QUERYS_PATH;

	QStringList query_keys;
	QString newID;

	foreach (QString str, configQ.allKeys())
		query_keys << str;

	do {
		newID = QUERYPREFIX + QString::number(++k);
	} while( query_keys.contains( newID ));

	newID = QInputDialog::getText(
				  this, tr("Save query command"),
				  tr("Stored query command identifiers:\n\n")
				  + query_keys.join('\n')
				  + tr("\n\nEnter query command identifier:"),
				  QLineEdit::Normal, newID, &ok);

	if (! (ok && !newID.isEmpty())) {
		QMessageBox::warning(this, tr("Error"), tr("Nothing changed"));
		return;
	}

	/** Save contens of query textbox to config "customQuerys" */
	configQ.setValue(objectName() + "/" + newID,
						  ui->teSqlQuerys->toHtml());

	config.setValue(objectName() + "/" + newID + "_plain",
						 ui->teSqlQuerys->toPlainText());

	if (ui->cbQueryIdent->itemText(0).contains(NEWQUERY))
		ui->cbQueryIdent->removeItem(0);

	mapCbTableProxy();
	ui->cbQueryIdent->setCurrentText(newID /*+ "_HTML"*/);

	/**
	 * If newID contains a substring indicating a WORKTIME table join query has
	 * to be saved, ask the user if this should be the new default query for
	 * worktime table and update the default query identifier string in cfg file
	 */
	if (newID.contains("worktim") &&
		 QMessageBox::question(this, tr("New default query identifier?"),
									  tr("Shold the identifier \"") + newID +
									  tr("\" be registered as new DEFAULT query identifier"
										  " for worktime table querys?"),
									  QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {

		configQ.setValue(objectName() + "/default_worktime_query", newID);

		config.setValue(objectName() + "/default_worktime_query_plain",
							 ui->teSqlQuerys->toPlainText());
	}

}
void InpFrm::restoreSqlQueryInputText() {
	QSETTINGS_QUERYS;

	QStringList query_keys;

	foreach (QString str, configQ.allKeys()) {
		if (str.contains( QUERYPREFIX ))
			query_keys << str;
	}

	ui->teSqlQuerys->setHtml(
				configQ.value(objectName() + "/SqlQueryText", "").toString());
}

void InpFrm::onChangeFocusOrder(Qt::FocusOrderState state) {
	/** Interrupt a running changeTabOrder process */
	if ((mChangeFocusFlag == Qt::FocusChange_isRunning ||
		  mChangeFocusFlag == Qt::FocusChange_rejected) &&
		 state == Qt::FocusChange_init) {
		emit stateMessage(tr("Change tab order process interrupted."
									"No changes in current tab order."), 2000);
		mChangeFocusFlag = Qt::FocusChange_none;
		return;
	}

//	/** If changeTabOrder process is not active && state == init, ... */
//	if (! (mChangeFocusFlag == Qt::FocusChange_isRunning) &&
//	    state == Qt::FocusChange_init) {
//		mObjFocusOrder.clear();
//		mChangeFocusFlag = Qt::FocusChange_isRunning;
//		emit stateMessage(tr("Init process to change the "
//		                     "input form tab order..."), 2000);
//		return;
//	}

//	/** --> Succesfull change of tab order objects
//	 * If this slot is called with parameter FocusChange_done &&
//	 * current state is FocusChange_isRunning
//	 * --> Succesfull change of tab order objects, run all setTabOrder(..) now
//	 */
//	if (mChangeFocusFlag == Qt::FocusChange_isRunning &&
//	    state == Qt::FocusChange_done) {

//		QList<QWidget *> widTabOrder = listCast<QWidget*, QObject*>( mObjFocusOrder );
//		QStringList lst;
//		for (int i=0; i < widTabOrder.length()-1; i++) {
//			widTabOrder[i]->setTabOrder(widTabOrder[i], widTabOrder[i+1]);
//			lst.append(widTabOrder[i]->objectName());
//		}

//		//        INFO << lst;
//		mChangeFocusFlag = Qt::FocusChange_done;

//		QSETTINGS;
//		config.setValue(this->objectName() + "/TabOrder", lst.join(","));
//		emit stateMessage(tr("Input form tab order successfully changed!"), 2000);
//		return;
//	}

//	return;

}
void InpFrm::restoreTabOrder() {
	/*!
	 * Check for valied tab order configuration data
	 */
	QSETTINGS;
	QStringList wNames;
	QList<QWidget *> tabOrderList;

	if (config.allKeys().contains(this->objectName() + Md::keys.focusOrder)) {
		INFO << tr("Valied tab order configuration data found!");
		emit stateMessage(tr("Valied tab order configuration data found!"), 4000);

		wNames = config.value(this->objectName() + tr("/TabOrder"), "")
					.toString().split(",");

		foreach (QString name, wNames) {
			/*!
			 * Check if widget with object name name and ->property("hasSqlMapper")
			 * .isValid() == true could be found.
			 */


			INFO << listFindByName<MdComboBox*>(mSqlCbs, name);

			auto itObj = std::find_if (mSqlCbs.begin(), mSqlCbs.end(), [](QComboBox *w)
			{ return w->objectName() == "cbPrjNummer"; } );
			INFO << *itObj;


//			if (userCtrlsChilds.indexOf(name) < 0)
//				CRIT << tr("Cild not found");

//			userCtrlsChilds.at( userCtrlsChilds.indexOf(name) )
//					->property("hasSqlMapper").isValid()) {
//				CRIT << tr("Cannot find child widget %1 with valid "
//							  """hasSqlMapper"" property!").arg(name);
//				continue;
//			}

//			tabOrderList << userCtrlsChilds.at( userCtrlsChilds.indexOf(name) );
		}
	}
	else {
		/*!
		 * Here a valid tab order list must be generated.
		 */
		tabOrderList = listCast<QWidget*,QObject*>(ui->userCtrlsLayout->children());
		foreach (QWidget *w, tabOrderList)
			if (! w->property("hasSqlMapper").isValid())
				tabOrderList.removeOne(w);
	}
	/* ======================================================================== */
	setFocusOrder(tabOrderList);
	/* ======================================================================== */
}
bool InpFrm::setFocusOrder(QList<QWidget *> targets) {
	/*!
	 * Request all child widgets from InpFrm and iterate through the QWidget list.
	 * If (*it).property("hasSqlMapper")->isValied() returns false, remove current
	 * widget from allTabable list.
	 */
	QList<QWidget *> allTabable = findChildren<QWidget *>();
	foreach (QWidget *w, allTabable)
		if (! w->property("hasSqlMapper").isValid())
			allTabable.removeOne(w);

	if ((targets.isEmpty()) ||
		 (targets.length() > allTabable.length()))
		bReturn("Wrong size of focusOrder list parameter");

	QList<QWidget *>::iterator it = targets.begin();

	while ((it - targets.begin()) < targets.length()) {
		/*!
		 * Remove the widget that is processed after this line from allTabable list. After
		 * break of the while loop, allTabable list represents the difference list:
		 * (allTabable - targets) --> set Qt::NoFocus policy
		 */
		allTabable.removeOne(*it);
		/*!
		 * If (*it) points to last list element, register .first() and break
		 */
		if ((it - targets.begin()) == targets.length() - 1) {
			setTabOrder((*it), targets.first());

			INFO << tr("setTabOrder(%1, %2); (Last)")
					  .arg((*it)->objectName())
					  .arg(targets.first()->objectName());
			break;
		}
		else {
			setTabOrder(*it, *(++it));
			INFO << tr("setTabOrder(%1, %2);")
					  .arg((*(it-1))->objectName())
					  .arg((*it)->objectName());
		}
	}

	/*!
	 * allTabable represents now the list difference:
	 * (allTabable - targets) --> set Qt::NoFocus policy
	 */
	foreach (QWidget *w, allTabable)
		w->setFocusPolicy(Qt::NoFocus);

	/*!
	 * Save the focus order list.
	 */
	mTabOrder.current = targets;
	return true;
}
Qt::FocusOrderState InpFrm::InpFrm::getChangeFocusFlag() const {
	return mChangeFocusFlag;
}
void InpFrm::InpFrm::setChangeFocusFlag(const Qt::FocusOrderState &stateFlag) {
	mChangeFocusFlag = stateFlag;
}
/* ======================================================================== */
/*                             Event callbacks                              */
/* ======================================================================== */
void InpFrm::hideEvent(QShowEvent *) {

}
void InpFrm::showEvent(QShowEvent *) {
	mEscapeTrigger = false;
	mapCbTableProxy();
}
void InpFrm::keyPressEvent(QKeyEvent *e) {
	/**
	 * If key press event for "Enter" is detected, emit a btnOk Clicked()
	 * signal to access onBtnOkClicked()
	 */
	if (e->key() == Qt::Key_Enter)
		emit ui->btnOk->clicked();

	/*!
	 * First escape key press event resets the current InpFrm widget focus to initial.
	 * Second escape key press event within 800ms hides the InpFrm::
	 */
	if (e->key() == Qt::Key_Escape) {
		if (! mEscapeTrigger) {
			mTabOrder.current.first()->setFocus(Qt::TabFocusReason);
			INFO << tr("start singledshot");
		}
		else hide();
	}
}
bool InpFrm::eventFilter(QObject *obj, QEvent *event) {
	if (! event)
		return true;

	/*!
	 * standard event processing
	 */
	return QObject::eventFilter(obj, event);
}

#define QFOLDINGSTART {
void InpFrm::onInpFormUserCommitOLD() {
	QSqlQuery query;
	QString q;

	/** Query actual count of records in worktime table */
	//    query.exec("SELECT worktimID FROM worktime");
	query.exec("SELECT wkt.worktimID FROM worktime wkt");

	if (query.lastError().type() != QSqlError::NoError) {
		QMessageBox::warning(this, "Fehler bei SQL Query",
									"query.lastError().type() != QSqlError::NoError");
		return;
	}
	INFO << query.last();

	/** Select actual worktimIDs
	 * ------------------------- */
	int worktimID_max = 0;
	QVector<int> ids;

	while (query.next()) {
		if (query.value(0).toInt() > worktimID_max)
			worktimID_max = query.value(0).toInt();

		ids.append( query.value(0).toInt() );
	}


	/** Find the next higher, unused worktimID */
	while ( ids.contains(++worktimID_max));

	/** Find workerID based on InpFrm ui selections
	 * --------------------------------------------- */
	q = QString("SELECT workerID FROM worker WHERE Nachname = \"%1\" "
					"AND PersonalNr = %2")
		 .arg( ui->cbWorkerNachname->currentText() )
		 .arg( ui->cbWorkerPersonalNr->currentText().toInt() );
	query.exec( q );

	int workerID = -1;
	while (query.next()) {
		/**
		 * If workerID != -1, this is the second loop entry but workerID
		 * must be unique! --> Error
		 */
		if (workerID > 0) {
			QMessageBox::warning(this, "Fehler bei SQL Query",
										"Non-unique workerID detected!");
			return;
		}
		else {
			workerID = query.value(0).toInt();
		}
	}

	/** Find prjID based on InpFrm ui selections
	 * --------------------------------------------- */
	q = QString("SELECT prjID FROM prj WHERE Kurzform = \"%1\" "
					"AND Nummer = %2")
		 .arg( ui->cbPrjKurzform->currentText() )
		 .arg( ui->cbPrjNummer->currentText().toInt() );
	query.exec( q );

	int prjID = -1;
	while (query.next()) {
		/**
		 * If prjID != -1, this is the second loop entry but prjID
		 * must be unique! --> Error
		 */
		if (prjID > 0) {
			QMessageBox::warning(this, "Fehler bei SQL Query",
										"Non-unique prjID detected!");
			return;
		}
		else {
			prjID = query.value(0).toInt();
		}
	}


	/** Prepare query for new record */
	query.prepare("INSERT INTO worktime (worktimID, dat, workerID, prjID, hours) "
					  "VALUES (:worktimID, :dat, :workerID, :prjID, :hours)");

	query.bindValue(":worktimID", worktimID_max);
	query.bindValue(":dat", ui->datePicker->date().toString(Qt::SystemLocaleShortDate));
	query.bindValue(":workerID", workerID);
	query.bindValue(":prjID", prjID);
	query.bindValue(":hours", ui->leHrs->text().toInt());

	query.exec();
	//    browser->requeryWorktimeTableView();
	emit requeryTableView();


	INFO.noquote() << query.lastQuery();

	if (query.lastError().type() != QSqlError::NoError) {
		QMessageBox::warning(this, "Fehler bei SQL Query",
									QString("Error type:" + query.lastError().driverText() +
											  "\nin final add-record query"));
		return;
	}
}
#define QFOLDINGEND }

/*!
*    mSqlCbs: cbPrjNummer,cbPrjKurzform,cbClientKurzform,cbClientNummer,cbWorkerPersonalNr,
*					cbWorkerNachname,cbWorkerVorname
* objTabable: cbPrjNummer,cbPrjKurzform,cbClientKurzform,cbClientNummer,cbWorkerPersonalNr,
*					cbWorkerNachname,cbWorkerVorname,cbQueryIdent,datePicker
* objTabAble: cbPrjNummer,cbPrjKurzform,cbClientKurzform,cbClientNummer,cbWorkerPersonalNr,
*					cbWorkerNachname,cbWorkerVorname,datePicker
*/



