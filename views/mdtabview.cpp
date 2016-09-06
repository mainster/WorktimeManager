#include "mdtabview.h"


MdTabView::MdTabView(const QString &tableName, QWidget *parent)
	/*: QTableView(parent)*/ {

	//!< Create ui
	m_gb = new QGroupBox(parent);
	QGridLayout *gl = new QGridLayout();

	gl->addWidget(this);
	m_gb->setLayout(gl);

	addActions( createActions() );

	if (!tableName.isEmpty())
		createForeignModel(tableName);

	setSortingEnabled( true );
	clearSelected();
	installEventFilter(this);

	setSizeAdjustPolicy(QTableView::AdjustToContents);
	adjustSize();

	m_gb->show();
//		restoreFont();
	//		initHeaders();
	//		setupContextMenu();

	setStyleSheet(StyleSheet_QGroupBox + tr("\n") + StyleSheet_QTableView);
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
	m_gb->setTitle(tNam);
	//	restoreColumnOrderAndVisability();
	setFont(restoreFont());
}
void MdTabView::setSqlTable(const QString &tableName) {
	if (!tableName.isEmpty())
		createForeignModel(tableName);
}

/* ======================================================================== */
/*                            Getters / Setters                             */
/* ======================================================================== */
void MdTabView::setSelected(bool selected) {
	m_selected = selected;
#ifdef SET_STYLESHEETS
	setStyleSheet(styleSheet());
#endif
	emit selectedChanged(selected);
}
void MdTabView::clearSelected() {
	m_selected = false;
	setStyleSheet(styleSheet());
}
bool MdTabView::isSelected()				{ return m_selected; }
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
/* ======================================================================== */
/*                              Event handler                               */
/* ======================================================================== */
/* ======================================================================== */
/*                             Helper methodes                              */
/* ======================================================================== */
void MdTabView::setAlternateRowCol(QColor &col, bool alternateEnabled) {
	//		m_tv->setAlternatingRowColors(alternateEnabled);
	if (alternateEnabled && col.isValid()) {
		/*m_tv->*/setStyleSheet(
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
void MdTabView::resizeRowsColsToContents() {
	setVisible( false );
	resizeColumnsToContents();
	resizeRowsToContents();
	setVisible( true );
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
	//		connect(actGrStrategy, &QActionGroup::triggered, this, &MdTabView::onActGrStrategyTrigd);

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
	//		connect(actGrContext, &QActionGroup::triggered, this, &MdTabView::onActGrContextTrigd);

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
void MdTabView::connectActions() {

	//		connect(actSectionMask, &QAction::toggled, this, &MdTabView::onActSectionMask);
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
void MdTabView::storeFont(QFont f) {
	QSETTINGS;
	config.setValue(objectName() + Md::k.tableFont, QVariant::fromValue(f));
	setFont(f);
}
QFont MdTabView::restoreFont() {
	QSETTINGS;
	QFont f(config.value(objectName() + Md::k.tableFont).value<QFont>());
	INFO << tr("Restoring MdTable::Font:") << f;
	setFont(f);
	return f;
}

#define QFOLDINGSTART {
const QString MdTabView::StyleSheet_QGroupBox = QString(
			"QGroupBox::title {"
			"    subcontrol-origin: margin; /* margin boarder padding content */"
			"    subcontrol-position: top center; /* position at the top center */"
			"    font: italic 9pt ""Arial"";"
			"    font-weight: bold;"
			"    top: 1.0ex;   "
			"    padding: 0px 8px;"
			"  padding-top: -2ex;"
			"  color:  rgba(50,50,50,255);"
			"} [selected=false] {"
			"  color:  rgba(245,0,0,255);"
			"} "
			"QGroupBox, QGroupBox [selected=false] {"
			"  margin-top: 1ex; /*margin-bottom: 2px; margin-left: 2px; margin-right: 2px;*/"
			"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E0E0E0, stop: 1 #FFFFFF);"
			"    font: italic 9pt ""Arial"";"
			"    font-weight: bold;"
			"    border-radius: 5px;"
			"  border: 2px solid;"
			"  border-top-color: qlineargradient(spread:reflect, x1:0, y1:0, x2:0.5, y2:0, stop:0.10 rgba(82, 82, 82, 255), stop:1 rgba(169, 169, 169,20));"
			"  border-left-color: rgba(105,105,105,255);"
			"  border-right-color: rgba(105,105,105,205);"
			"  border-bottom-color: rgba(105,105,105,205);"
			"} [selected=true] { "
			"  /* margin-top: 5px; margin-bottom: -2px; margin-left: -2px; margin-right: -2px; */"
			"  border: 2px solid; "
			"  border-top-color: qlineargradient(spread:reflect, x1:0, y1:0, x2:0.5, y2:0, stop:0.10 rgba(255,0,0,255), stop:1 rgba(247, 247, 247, 250));"
			"  border-left-color: rgba(255,0,0,255);"
			"  border-right-color: rgba(255,0,0,255);"
			"  border-bottom-color: rgba(255,0,0,255);"
			"}"
			"QGroupBox::title:hover {"
			"    color: rgba(235, 235, 235, 255);"
			"}"
			);

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

