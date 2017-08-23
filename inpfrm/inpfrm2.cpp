#include "inpfrm2.h"
#include "ui_inpfrm2.h"

#define QUERYPREFIX  tr("CUSTOM_QUERY_")
#define NEWQUERY     tr("NewQuery")

#define FIXED_HEIGHT	 350

class MdComboBox;

/* ======================================================================== */
/*                              InpFrm::InpFrm                              */
/* ======================================================================== */
InpFrm2* InpFrm2::inst = 0;
InpFrm2::InpFrm2(QWidget *parent) : QDockWidget(parent),
	ui2(new Ui::InpFrm2), mEscapeTrigger(false) {
	ui2->setupUi(this);
	hide();	inst = this;

	fixedHeights = InpFrm2::fixedHeights_t(2 * FIXED_HEIGHT, FIXED_HEIGHT);

	QSETTINGS;
	if (config.allKeys().join(',').contains( objectName() + tr("/fixedHeight") ))
		fixedHeights.sqlQueryInvisible =
				config.value(objectName() + tr("/fixedHeight")).toInt();

	if (config.allKeys().join(',').contains( objectName() + tr("/fixedHeightQuery") ))
		fixedHeights.sqlQueryVisible =
				config.value(objectName() + tr("/fixedHeightQuery")).toInt();

	connectActions();

	ui2->cbQueryIdent->setDuplicatesEnabled( false );
	ui2->datePicker->setDate(QDate::currentDate());
	ui2->gbSqlQuery->hide();
	WIN_RESTORE(this);


	setFixedHeight(fixedHeights.sqlQueryInvisible);
	initComboboxes();

	restoreTabOrder();

	//	ui->gboxWorker->hide();
	/* ======================================================================== */
#define QFOLDINGSTART {
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
#define QFOLDINGEND }
}
InpFrm2::~InpFrm2() {
	WIN_STORE(this);

	this->removeEventFilter(this);

	foreach (QWidget *w, mSqlCbs)
		w->removeEventFilter(this);

	delete ui2;
}
void InpFrm2::onInpFormUserCommitAlt() {
	QSqlQuery query;
	QString q;
	bool ok = true;

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

	/** Find the greatest worktimID entry */
	qStableSort(ints.begin(), ints.end());
	int worktimID_max = ints.last();
	Q_UNUSED(worktimID_max);

	/*!
	  * Find workerID based on InpFrm ui selections
	  */
	QStringList strList = ui2->cbWorker->currentData(Qt::DisplayRole).toString().split(tr(", "));
	q = QString("SELECT workerID FROM worker WHERE Nachname = \"%1\" AND PersonalNr = %2")
		 .arg( strList.at(0)).arg( strList.at(2) );
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
		workerID = query.value(0).toInt();
	}

	/*!
	  * Find prjID based on InpFrm ui selections.
	  */
	strList = ui2->cbPrj->currentData(Qt::DisplayRole).toString().split(tr(", "));
	q = QString("SELECT prjID FROM prj WHERE Kurzform = \"%1\" AND Nummer = %2")
		 .arg(strList.at(0)).arg(strList.at(1));
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
		prjID = query.value(0).toInt();
	}


	/** Prepare query for new record */
	query.prepare("INSERT INTO worktime (dat, workerID, prjID, hours) "
					  "VALUES (:dat, :workerID, :prjID, :hours)");

	//	query.bindValue(":worktimID",   -1);
	query.bindValue(":dat",         ui2->datePicker->date().toString("yyyy-MM-dd"));
	query.bindValue(":workerID",    workerID);
	query.bindValue(":prjID",       prjID);
	query.bindValue(":hours",       ui2->leHrs->text().toInt());
	query.exec();
	if (query.lastError().type() != QSqlError::NoError) {
		QMessageBox::warning(this, "Fehler bei SQL Query",
									QString("Error type:" + query.lastError().driverText() +
											  "\nin final add-record query"));
		return;
	}

	//    Browser::instance()->requeryWorktimeTableView(tr(""));
	//    Browser::inst
	emit newWorktimeRecord();
	INFO << query.lastQuery();

}
void InpFrm2::onInpFormUserCommit() {
	QSqlQuery query;
	QString q;

	/*!
	  * Find workerID based on InpFrm ui selections
	  */
	QStringList strList = ui2->cbWorker->currentData(Qt::DisplayRole).toString().split(tr(", "));
	q = QString("SELECT workerID FROM worker WHERE Nachname = \"%1\" AND PersonalNr = %2")
		 .arg( strList.at(0)).arg( strList.at(2) );
	query.exec( q );

	if (query.lastError().type() != QSqlError::NoError)
		qReturn("Bad query command feedback!");
	int workerID = query.value(0).toInt();

	/*!
	  * Find prjID based on InpFrm ui selections.
	  */
	strList = ui2->cbPrj->currentData(Qt::DisplayRole).toString().split(tr(", "));
	q = QString("SELECT prjID FROM prj WHERE Kurzform = \"%1\" AND Nummer = %2")
		 .arg(strList.at(0)).arg(strList.at(1));
	query.exec( q );

	if (query.lastError().type() != QSqlError::NoError)
		qReturn("Bad query command feedback!");
	int prjID = query.value(0).toInt();


	/*!
	 * Prepare query for new record.
	 */
	query.clear();
	query.prepare("INSERT INTO worktime (dat, workerID, prjID, hours) "
					  "VALUES (:dat, :workerID, :prjID, :hours)");
	query.bindValue(":dat",         ui2->datePicker->date().toString("yyyy-MM-dd"));
	query.bindValue(":workerID",    workerID);
	query.bindValue(":prjID",       prjID);
	query.bindValue(":hours",       ui2->leHrs->text().toInt());
	query.exec();

	if (query.lastError().type() != QSqlError::NoError) {
		QMessageBox::warning(this, "Fehler bei SQL Query",
									QString("Error type:" + query.lastError().driverText() +
											  "\nin final add-record query"));
		return;
	}

	emit newWorktimeRecord();
	Browser::instance()->mTabs.findByTableName(tr("worktime"))->tv()->reset();
}
void InpFrm2::onCbIndexChanged(const int index) {
	MdComboBox *cbSender = qobject_cast<MdComboBox *>(QObject::sender());

	INFO  << cbSender << cbSender->currentText() << index << cbSender->currentIndex();

	/*!
	  * Since the combobox text could be a project number or project "Kurzform", we
	  * have to find the correct record within the SQL table "prj" and set the row
	  * to be viewed via ui->lblPrj.
	  */
	if (cbSender == ui2->cbPrj) {
		return;
	}
	if (cbSender == ui2->cbClient) {

		return;
	}
	if (cbSender == ui2->cbWorker) {

		return;
	}
}
void InpFrm2::aButtonClick(bool) {
	QPushButton *pbSender = qobject_cast<QPushButton *>(QObject::sender());
	QList<QWidget *> ws;
	/*
 * ws << ui->datePicker
		  << ui->cbPrj
		  << ui->cbPrjNummer
		  << ui->cbClient
		  << ui->cbClientNummer
		  << ui->cbWorkerVorname
		  << ui->cbWorker
		  << ui->cbWorkerPersonalNr
		  << ui->leHrs;


	 setFocusOrder( ws );
*/
	if (pbSender == ui2->btnSaveQuery) { /*saveSqlQueryInputText();*/ return; }
	if (pbSender == ui2->btnRestoreQuery) { /*restoreSqlQueryInputText();*/ return; }
	if (pbSender == ui2->btnSubmitQuery) {
		//		Browser::instance()->exec();
		Browser::instance()->execCustomQuery();
	}
	if (pbSender == ui2->btnClear) {
		//		foreach (MdComboBox *mcbx, QList<MdComboBox*>(listCast<MdComboBox*,QComboBox*>(mSqlCbs))) {
		//			mcbx->makePermanentView(true);
		//		}
		onInpFormUserCommit();

	}
	if (pbSender == ui2->btnOk) {
		onInpFormUserCommitAlt();
	}
	if (pbSender == ui2->btnUndo) {


		QList<QWidget *> targs =
				QList<QWidget *>() << ui2->datePicker << ui2->leHrs << ui2->btnOk << ui2->cbPrj
										 << ui2->cbWorker << ui2->cbClient;
		setFocusOrder(targs);

	}
	if (pbSender == ui2->btnRedo) {
		//		Browser *browser = Browser::instance();
		//		browser->on_connectionWidget_metaDataRequested("prj");

		mSqlCbs[0]->setModel( mModels.at(IDX_PROJECT).tableModel );
		mModels.at(IDX_PROJECT).tableModel->select();
		mSqlCbs[0]->setModelColumns(QList<short>() << 1 << 4);

		QModelIndex mix = mModels.at(IDX_PROJECT).tableModel->index(0,0,QModelIndex());
		INFO << mix;
		INFO << mModels.at(IDX_PROJECT).tableModel->rowCount(QModelIndex())
			  << mModels.at(IDX_PROJECT).tableModel->columnCount(QModelIndex())
			  << mModels.at(IDX_PROJECT).tableModel->rowCount(mix)
			  << mModels.at(IDX_PROJECT).tableModel->columnCount(mix);
	}
}
QList<QWidget *> InpFrm2::getTabableWidgets() {
	QList<QWidget *> inpWids;

	inpWids << listCast<QWidget*>(findChildren<QComboBox *>(QRegularExpression("cb*")));
	inpWids << listCast<QWidget*>(findChildren<QDateEdit *>(QRegularExpression("date*")));
	inpWids << listCast<QWidget*>(findChildren<QSpinBox *>(QRegularExpression("sb*")));
	inpWids << listCast<QWidget*>(findChildren<QLineEdit *>(QRegularExpression("le*")));
	inpWids << listCast<QWidget*>(findChildren<QRadioButton *>(QRegularExpression("rb*")));
	inpWids << listCast<QWidget*>(findChildren<QPushButton *>(QRegularExpression("pb*")));
	inpWids << listCast<QWidget*>(findChildren<QPushButton *>(QRegularExpression("btn*")));

	foreach (QWidget *w, inpWids)
		if (w->objectName().contains("qt_"))
			inpWids.removeOne(w);

	//	INFO << (*listObjectNames<QWidget *>(inpWids)).join("\n");

	return inpWids;
}
void InpFrm2::onInpFormChanges(int idx) {
	Q_UNUSED(idx);
	//   INFO << idx;
}
void InpFrm2::onInpFormChanges(QDate date) {
	Q_UNUSED(date);
	//   INFO << date;
}
void InpFrm2::onSqlQuerysTextChanged() {
	/**
	  * Check if teSqlQuery hasFocus to determine if the textCHanged signal
	  * was emitted in consequence of a Query restore (no new cb item) or
	  * in cons. of an user text input/change
	  */
	if (! ui2->teSqlQuerys->hasFocus())
		return;

	/**
	  * generate new item only if there is no "New query" item at 0.
	  * If there is already a "New query" item, update item data only.
	  */
	if (! ui2->cbQueryIdent->itemText(0).contains(NEWQUERY))
		ui2->cbQueryIdent->insertItem(0, NEWQUERY,
											  ui2->teSqlQuerys->toHtml());
	else
		ui2->cbQueryIdent->setItemData(0, ui2->teSqlQuerys->toHtml());

	ui2->cbQueryIdent->setCurrentIndex(0);
}
void InpFrm2::setQueryBoxVisible(bool visible) {
	ui2->gbSqlQuery->setVisible( visible );
	if (visible) {
		setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		ui2->teSqlQuerys->setFocusPolicy(Qt::ClickFocus);
		setMaximumHeight( fixedHeights.sqlQueryVisible );
		return;
	}
	setFixedHeight( fixedHeights.sqlQueryInvisible );
}
void InpFrm2::onCbQueryIndexChaned(int idx) {
	Q_UNUSED(idx);
	ui2->teSqlQuerys->setHtml( ui2->cbQueryIdent->currentData().toString() );
}
void InpFrm2::onChangeFocusOrder(Qt::FocusOrderState state) {
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
void InpFrm2::restoreTabOrder() {
	/*!
	  * Check for valied tab order configuration data
	  */
	QSETTINGS;

	/*!
	 * Write all tab-able widget object names to config file.
	 */
	QList<QWidget *> tabOrderList = filterForProperty(
				findChildren<QWidget *>(), "hasSqlMapper");
	setFocusOrder(tabOrderList);

	//	QList<QWidget *> tabOrderList;
//	tabOrderList = findChildren<QWidget *>();
//	foreach (QWidget *w, tabOrderList)
//		if (! w->property("hasSqlMapper").isValid())
//			tabOrderList.removeOne(w);

	config.setValue(objectName() + Md::k.allTabableWidgets,
						 listObjectNames<QWidget *>(tabOrderList)->join(','));

	if (config.allKeys().join(',').contains(objectName() + Md::k.focusOrder)) {
		emit stateMessage(tr("Valied tab order configuration found!"), 4000);

		foreach (const QString s, config.value(
						objectName() + Md::k.focusOrder).toString().split(','))
			tabOrderList << findChildren<QWidget *>(s);
	}
	else {
		/*!
		 * Use the "all tab-able" list from methode entrance
		 */
		INFO << tr("NO valied tab order configuration found!");
	}
	/* ======================================================================== */
	setFocusOrder(tabOrderList/* << tabOrderList.first() */);

	//	for (int k = 0; k< (tabOrderList.length() - 1); k++) {
	//		setTabOrder(tabOrderList.at(k), tabOrderList.at(k+1));
	//		INFO << tr("setTabOrder([%1]").arg(k) << tabOrderList.at(k)->objectName()
	//		     << tr(",[%1]").arg(k+1) << tabOrderList.at(k+1)->objectName();
	//	}

	/* ======================================================================== */
}
bool InpFrm2::setFocusOrder(QList<QWidget *> targets) {
	/*!
	  * Request all child widgets from InpFrm and iterate through the QWidget list.
	  * If (*it).property("hasSqlMapper")->isValied() returns false, remove current
	  * widget from allTabable list.
	  */

	if ((targets.isEmpty()) ||
		 (targets.length() > getTabableWidgets().length()))
		bReturn("Wrong size of focusOrder list parameter");

	/*!
			* Remove the widget that is processed after this line from allTabable list. After
			* break of the while loop, allTabable list represents the difference list:
			* (allTabable - targets) --> set Qt::NoFocus policy
			*/
	/*!
	 * If (*it) points to last list element, register .first() and break
	 */
	for (int k = 0; k < (targets.length() - 1); k++) {
		setTabOrder(targets.at(k), targets.at(k + 1));
		//		INFO << tr("setTabOrder([%1]").arg(k) << targets.at(k)->objectName()
		//			  << tr(",[%1]").arg(k+1) << targets.at(k+1)->objectName();
	}

	/*!
	 * Clear now tab function for all other input widgets.
	 */
	QList<QWidget *> noFocusWids = getTabableWidgets();
	//	INFO << *listObjectNames<QWidget *>(noFocusWids);
	//	INFO << *listObjectNames<QWidget *>(targets);

	foreach (QWidget *w, targets)
		noFocusWids.removeOne(w);

	INFO << *listObjectNames<QWidget *>(noFocusWids);

	foreach (QWidget *w, noFocusWids)
		w->setFocusPolicy(Qt::NoFocus);

	/*!
	  * Save the focus order list.
	  */
	mTabOrder.current = targets;

	/*!
	 * Write tab order to config file.
	 */
	QSETTINGS;
	config.setValue(objectName() + Md::k.focusOrder,
						 listObjectNames<QWidget *>(mTabOrder.current)->join(','));
	config.sync();

	return true;
}
Qt::FocusOrderState InpFrm2::InpFrm2::getChangeFocusFlag() const {
	return mChangeFocusFlag;
}
void InpFrm2::setChangeFocusFlag(const Qt::FocusOrderState &stateFlag) {
	mChangeFocusFlag = stateFlag;
}
void InpFrm2::onDockLocationChanged(Qt::DockWidgetArea area) {
	QSETTINGS;
	config.setValue(objectName() + tr("/") + KEY_DOCKWIDGETAREA, (uint) area);

}
void InpFrm2::onUndockEvent(bool isUndocked) {
	if (isUndocked)
		WIN_RESTORE(this);
}
/* ======================================================================== */
/*                             Event callbacks                              */
/* ======================================================================== */
void InpFrm2::showEvent(QShowEvent *) {
	update();
	updateGeometry();
	mEscapeTrigger = false;
	//	mapCbTableProxy();
}
void InpFrm2::hideEvent(QShowEvent *) {

}
void InpFrm2::closeEvent(QCloseEvent *) {
	QSETTINGS;

	//	if (dock)
	/**** Write mainWindow geometry and window state
	\*/
	WIN_STORE(this);
}
void InpFrm2::keyPressEvent(QKeyEvent *e) {
	/**
	  * If key press event for "Enter" is detected, emit a btnOk Clicked()
	  * signal to access onBtnOkClicked()
	  */
	if (e->key() == Qt::Key_Enter) {
		emit ui2->btnOk->clicked();
		e->accept();
		return;
	}

	/*!
	  * First escape key press event resets the current InpFrm widget focus to initial.
	  * Second escape key press event within 800ms hides the InpFrm::
	  */
	if (e->key() == Qt::Key_Escape) {
		if (! mEscapeTrigger) {
			if (mChangeFocusFlag == Qt::FocusChange_isRunning) {
				mTabOrder.current.first()->setFocus(Qt::TabFocusReason);
				INFO << tr("start singledshot");
			}
		}
		else hide();
	}
	e->ignore();
}
void InpFrm2::resizeEvent(QResizeEvent *) {
	//	if (isFloating())
	//		WIN_STORE(this);
}
/* ======================================================================== */
/*									    Init methodes										    */
/* ======================================================================== */
void InpFrm2::textFilterChanged1() {
	int i = 0;
	 QRegExp regExp(mFiltEdits.at(i)->text(),
						 mFiltEdits.at(i)->caseSensitivity(),
						 mFiltEdits.at(i)->patternSyntax());
	 mProxys[i]->setFilterRegExp(regExp);
}
void InpFrm2::textFilterChanged2() {
	int i = 1;
	 QRegExp regExp(mFiltEdits.at(i)->text(),
						 mFiltEdits.at(i)->caseSensitivity(),
						 mFiltEdits.at(i)->patternSyntax());
	 mProxys[i]->setFilterRegExp(regExp);
}
void InpFrm2::textFilterChanged3() {
	int i = 2;
	 QRegExp regExp(mFiltEdits.at(i)->text(),
						 mFiltEdits.at(i)->caseSensitivity(),
						 mFiltEdits.at(i)->patternSyntax());
	mProxys[i]->setFilterRegExp(regExp);
}
void InpFrm2::initListViews(void) {
//	QList<QWidget *> ws = findChildren<QWidget *>();

//	foreach (QWidget *w, ws) {
//		INFO << w->objectName() << w->sizePolicy();
//		w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//	}

	QList<QGroupBox *> gbs = QList<QGroupBox *>()
			<< ui2->gboxPrj	<< ui2->gboxClient	<< ui2->gboxWorker;

	QFrame frm1;

	QGridLayout *gl = new QGridLayout();

	int rr = 0;
	foreach (QGroupBox *gb, gbs) {
		mTblWids.append(new QTableView(gb));
		mProxys.append(new MySortFilterProxyModel(this));
		mFiltEdits.append(new FilterEdit());
		mFiltEdits.last()->setText("Grace|Sports");

		gl->addWidget(mTblWids.last(), 0, rr, Qt::AlignCenter);
		gl->addWidget(mFiltEdits.last(), 1, rr++, Qt::AlignCenter);
		mTblWids.last()->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		mTblWids.last()->show();
		mFiltEdits.last()->show();
	}

	frm1.setLayout(gl);
	frm1.setFixedSize(QSize(400, 300));
	frm1.setParent(this);
	frm1.show();

	connect(mFiltEdits.at(0), &FilterEdit::filterChanged, this, &InpFrm2::textFilterChanged1);
	connect(mFiltEdits.at(0), &FilterEdit::textChanged, this, &InpFrm2::textFilterChanged1);
	mTblWids[0]->setModel(ui2->gboxPrj->findChild<MdComboBox *>()->model());
	connect(mFiltEdits.at(1), &FilterEdit::filterChanged, this, &InpFrm2::textFilterChanged2);
	connect(mFiltEdits.at(1), &FilterEdit::textChanged, this, &InpFrm2::textFilterChanged2);
	mTblWids[1]->setModel(ui2->gboxClient->findChild<MdComboBox *>()->model());
	connect(mFiltEdits.at(2), &FilterEdit::filterChanged, this, &InpFrm2::textFilterChanged3);
	connect(mFiltEdits.at(2), &FilterEdit::textChanged, this, &InpFrm2::textFilterChanged3);
	mTblWids[2]->setModel(ui2->gboxWorker->findChild<MdComboBox *>()->model());



//		mTblWids.append(new QTableWidget(ws.at(k)));
//		mTblWids.last()->setModel(ws.at(k)->model());
//		QGridLayout *gl = new QGridLayout(ws.at(k));
//		gl->addWidget(ws.at(k));
//		gl->addWidget(mTblWids.last());
//		ws[k]->setLayout(gl);
//		mTblWids.last()->show();
//	}
}

void InpFrm2::onCbLineEditChanged(void) {
	QLineEdit *le = qobject_cast<QLineEdit *>(QObject::sender());
	if (le)
		INFO << le->parent()->objectName() << le->parentWidget()->objectName();
}

void InpFrm2::initComboboxes() {
	/*!
	  * Initialize sql combobox list member mSqlCbs.
	  */
	mSqlCbs = listCast<MdComboBox *>(filterForProperty(
				listCast<QWidget *>(findChildren<MdComboBox *>()), "hasSqlMapper"));
	/*!
	 * Install custom event filter object for SQL mapping purposes.
	 */
	foreach (MdComboBox *cb, mSqlCbs)
		cb->installEventFilter( new SqlEventFilter(this) );

	/*!
	  * Append field groups to model struct mModels
	  */
	mModels << fieldGroup_t(tr("prj"), ui2->cbPrj)
			  << fieldGroup_t(tr("client"), ui2->cbClient)
			  << fieldGroup_t(tr("worker"), ui2->cbWorker);

	/*!
	 * Set foreign key relations.
	 */
	mModels[IDX_PROJECT].tableModel->setRelation(
				mModels[IDX_PROJECT].tableModel->fieldIndex("clientID"),
				QSqlRelation("client", "clientID", "Name"));

	foreach (fieldGroup_t field, mModels) {
		/*!
			* QSqlRelationalTableModel::Select and set proxy models source model.
			*/
		field.tableModel->select();
		field.proxyModel->setSourceModel(field.tableModel);
		field.proxyModel->invalidate();
		/*!
			* Set the combobox model to proxyModel.
			*/
		field.comboBox->setModel(field.proxyModel);
	}

	QList<QString> list;
	foreach (fieldGroup_t field, mModels) {
		list.clear();
		list << tr("Table: %1").arg(field.tableModel->tableName());
		for (int k = 0; k < field.tableModel->columnCount(QModelIndex()); k++)
			list << field.tableModel->record().fieldName(k);
		//		INFO << list;
	}
	/*!
	  * Select the model columns which should be transformed into string list model and
	  * used for comboboxes dropdown menu.
	  */
	QList<short> idx;
	idx << mModels[IDX_PROJECT].tableModel->fieldIndex("Kurzform")
		 << mModels[IDX_PROJECT].tableModel->fieldIndex("Nummer");
	//	INFO << idx;
	ui2->cbPrj->setModelColumn/*s*/(idx.at(0));

	idx.clear();
	idx << mModels[IDX_CLIENT].tableModel->fieldIndex("Kurzform")
		 << mModels[IDX_CLIENT].tableModel->fieldIndex("Nummer");
	//	INFO << idx;
	ui2->cbClient->setModelColumn/*s*/(idx.at(0));

	idx.clear();
	idx << mModels[IDX_WORKER].tableModel->fieldIndex("Nachname")
		 << mModels[IDX_WORKER].tableModel->fieldIndex("Vorname")
		 << mModels[IDX_WORKER].tableModel->fieldIndex("PersonalNr");
	//	INFO << idx;
	ui2->cbWorker->setModelColumn/*s*/(idx.at(0));

	connect(ui2->cbPrj, SIGNAL(currentIndexChanged(int)), this, SLOT(onCbIndexChanged(int)));
	connect(ui2->cbClient, SIGNAL(currentIndexChanged(int)), this, SLOT(onCbIndexChanged(int)));
	connect(ui2->cbWorker, SIGNAL(currentIndexChanged(int)), this, SLOT(onCbIndexChanged(int)));

	connect(ui2->cbPrj->lineEdit(), &QLineEdit::textChanged, this, &InpFrm2::onCbLineEditChanged);
	connect(ui2->cbClient->lineEdit(), &QLineEdit::textChanged, this, &InpFrm2::onCbLineEditChanged);
	connect(ui2->cbWorker->lineEdit(), &QLineEdit::textChanged, this, &InpFrm2::onCbLineEditChanged);

	//	ui->cbPrj->setModelColumns(QList<quint8>() << 1 << 5);
	//	ui->cbClient->setModelColumns(QList<quint8>() << 1 << 3);
	//	ui->cbWorker->setModelColumns(QList<quint8>() << 1 << 2);

}
QString InpFrm2::getQueryText() const {
	return ui2->teSqlQuerys->toPlainText();
}
void InpFrm2::connectActions() {

	connect(ui2->datePicker,         SIGNAL(dateChanged(QDate)),
			  this,                   SLOT(onInpFormChanges(QDate)));
	connect(ui2->sbID,               SIGNAL(valueChanged(int)),
			  this,                   SLOT(onInpFormChanges(int)));
	connect(ui2->teSqlQuerys,        SIGNAL(textChanged()),
			  this,                   SLOT(onSqlQuerysTextChanged()));
	connect(ui2->cbQueryIdent,       SIGNAL(currentIndexChanged(int)),
			  this,                   SLOT(onCbQueryIndexChaned(int)));

	connect(this, &InpFrm2::changeFocusOrder, this, &InpFrm2::onChangeFocusOrder);
	connect(this, &InpFrm2::dockLocationChanged, this, &InpFrm2::onDockLocationChanged);
	connect(this, &InpFrm2::topLevelChanged, this, &InpFrm2::onUndockEvent);

	QList<QComboBox*> cbs = findChildren<QComboBox *>();
	QList<QPushButton*> pbs = findChildren<QPushButton *>();

	foreach (QComboBox *cb, cbs)
		connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(onInpFormChanges(int)));

	foreach (QPushButton *pb, pbs) {
		if (pb->objectName() == tr("btnSaveQuery")) {
			connect(pb, &QPushButton::clicked, this, &InpFrm2::saveSqlQueryInputText);
			continue;
		}

		if (pb->objectName() == tr("btnRestoreQuery")) {
			connect(pb, &QPushButton::clicked, this, &InpFrm2::restoreSqlQueryInputText);
			continue;
		}

		if (! (bool)connect(pb, &QPushButton::clicked, this, &InpFrm2::aButtonClick))
			CRIT << tr("connect(..) returned false or button %1").arg(pb->objectName());
	}
}
void InpFrm2::saveSqlQueryInputText() {
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
						  ui2->teSqlQuerys->toHtml());

	config.setValue(objectName() + "/" + newID + "_plain",
						 ui2->teSqlQuerys->toPlainText());

	if (ui2->cbQueryIdent->itemText(0).contains(NEWQUERY))
		ui2->cbQueryIdent->removeItem(0);

	//	mapCbTableProxy();
	ui2->cbQueryIdent->setCurrentText(newID /*+ "_HTML"*/);

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
							 ui2->teSqlQuerys->toPlainText());
	}

}
void InpFrm2::restoreSqlQueryInputText() {
	QSETTINGS_QUERYS;
	QStringList customQueryID;

	ui2->cbQueryIdent->clear();

	/*!
	  * Set CUSTOM_QUERY_COMMANDS combo box items
	  */
	foreach (QString str, configQ.allKeys()) {
		customQueryID << str.split('/').at(1);
		ui2->cbQueryIdent->addItem(customQueryID.last(),
										  configQ.value(str).toString());
	}
}
/* ======================================================================== */
/*                                 Helpers                                  */
/* ======================================================================== */
QList<QWidget *> InpFrm2::filterForProperty(
		QList<QWidget *> list, const char* property) {
	foreach (QWidget *w, list) {
		if (! w->property(property).isValid())
			list.removeOne(w);
		else {
			if (! w->property(property).toBool())
				list.removeOne(w);
			else {
				/*!
				 * Install custom event filter object for SQL mapping purposes.
				 */
				w->installEventFilter( new SqlEventFilter(this) );
			}
		}
	}
	return list;
}

#define QFOLDINGSTART {
//void InpFrm::onInpFormUserCommitOLD() {
//	QSqlQuery query;
//	QString q;

//	/** Query actual count of records in worktime table */
//	//    query.exec("SELECT worktimID FROM worktime");
//	query.exec("SELECT wkt.worktimID FROM worktime wkt");

//	if (query.lastError().type() != QSqlError::NoError) {
//		QMessageBox::warning(this, "Fehler bei SQL Query",
//									"query.lastError().type() != QSqlError::NoError");
//		return;
//	}
//	INFO << query.last();

//	/** Select actual worktimIDs
//	 * ------------------------- */
//	int worktimID_max = 0;
//	QVector<int> ids;

//	while (query.next()) {
//		if (query.value(0).toInt() > worktimID_max)
//			worktimID_max = query.value(0).toInt();

//		ids.append( query.value(0).toInt() );
//	}


//	/** Find the next higher, unused worktimID */
//	while ( ids.contains(++worktimID_max));

//	/** Find workerID based on InpFrm ui selections
//	 * --------------------------------------------- */
//	q = QString("SELECT workerID FROM worker WHERE Nachname = \"%1\" "
//					"AND PersonalNr = %2")
//		 .arg( ui->cbWorker->currentText() )
//		 .arg( ui->cbWorkerPersonalNr->currentText().toInt() );
//	query.exec( q );

//	int workerID = -1;
//	while (query.next()) {
//		/**
//		 * If workerID != -1, this is the second loop entry but workerID
//		 * must be unique! --> Error
//		 */
//		if (workerID > 0) {
//			QMessageBox::warning(this, "Fehler bei SQL Query",
//										"Non-unique workerID detected!");
//			return;
//		}
//		else {
//			workerID = query.value(0).toInt();
//		}
//	}

//	/** Find prjID based on InpFrm ui selections
//	 * --------------------------------------------- */
//	q = QString("SELECT prjID FROM prj WHERE Kurzform = \"%1\" "
//					"AND Nummer = %2")
//		 .arg( ui->cbPrj->currentText() )
//		 .arg( ui->cbPrjNummer->currentText().toInt() );
//	query.exec( q );

//	int prjID = -1;
//	while (query.next()) {
//		/**
//		 * If prjID != -1, this is the second loop entry but prjID
//		 * must be unique! --> Error
//		 */
//		if (prjID > 0) {
//			QMessageBox::warning(this, "Fehler bei SQL Query",
//										"Non-unique prjID detected!");
//			return;
//		}
//		else {
//			prjID = query.value(0).toInt();
//		}
//	}


//	/** Prepare query for new record */
//	query.prepare("INSERT INTO worktime (worktimID, dat, workerID, prjID, hours) "
//					  "VALUES (:worktimID, :dat, :workerID, :prjID, :hours)");

//	query.bindValue(":worktimID", worktimID_max);
//	query.bindValue(":dat", ui->datePicker->date().toString(Qt::SystemLocaleShortDate));
//	query.bindValue(":workerID", workerID);
//	query.bindValue(":prjID", prjID);
//	query.bindValue(":hours", ui->leHrs->text().toInt());

//	query.exec();
//	//    browser->requeryWorktimeTableView();
//	emit requeryTableView();


//	INFO.noquote() << query.lastQuery();

//	if (query.lastError().type() != QSqlError::NoError) {
//		QMessageBox::warning(this, "Fehler bei SQL Query",
//									QString("Error type:" + query.lastError().driverText() +
//											  "\nin final add-record query"));
//		return;
//	}
//}
#define QFOLDINGEND }

/*!
*    mSqlCbs: cbPrjNummer,cbPrjKurzform,cbClientKurzform,cbClientNummer,cbWorkerPersonalNr,
*					cbWorkerNachname,cbWorkerVorname
* objTabable: cbPrjNummer,cbPrjKurzform,cbClientKurzform,cbClientNummer,cbWorkerPersonalNr,
*					cbWorkerNachname,cbWorkerVorname,cbQueryIdent,datePicker
* objTabAble: cbPrjNummer,cbPrjKurzform,cbClientKurzform,cbClientNummer,cbWorkerPersonalNr,
*					cbWorkerNachname,cbWorkerVorname,datePicker
*/

