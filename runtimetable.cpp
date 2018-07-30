#include "runtimetable.h"

const QString RuntimeTable::OVERTIME_TABLE_NAME = QString("runtime");

/* ======================================================================== */
/*                        RuntimeTable::RuntimeTable                        */
/* ======================================================================== */
bool RuntimeTable::recalcOvertime() {
	bool ok = false;
	/*!
	 * Query a ColumnSchema list.
	 */
	worktimeColumnInfos = MdTableInfo::querySchema(qobject_cast<MdTabView *>(
																	m_worktimeTv)->sqlTableName(), DbController::db());
	runtimeColumnInfos = MdTableInfo::querySchema(qobject_cast<MdTabView *>(
																  m_runtimeTv)->sqlTableName(), DbController::db());
	/*!
	 * Query worktime and worker records.
	 */
	QSqlQueryModel *worktimeRecs = new QSqlQueryModel();
	worktimeRecs->setQuery("SELECT wkt.worktimID, wkt.dat, wkt.hours,"
								  "wk.workerID, wk.Nachname, wk.Vorname, wk.PersonalNr "
								  "FROM worker wk "
								  "INNER JOIN worktime wkt "
								  "ON wkt.workerID = wk.workerID");

	QSqlQueryModel *workerRecs = new QSqlQueryModel();
	workerRecs->setQuery("SELECT * FROM worker");

	/*!
	 * Create a vector of all PN's
	 */
	QMap<int, int> workerPnId;		//!< Holds PN and ID
	for (int k = 0; k < workerRecs->rowCount(); k++)
		workerPnId.insert(workerRecs->record( k ).value("PersonalNr").toInt(),
								workerRecs->record( k ).value("workerID").toInt());

	/*!
	 * Check querys against errors.
	 */
	if ((worktimeRecs->lastError().type() != QSqlError::NoError) ||
		 (workerRecs->lastError().type() != QSqlError::NoError))
		bReturn("Error while query worker or worktime table");

	//	QDate minDate, maxDate;

	/*!
	 * Create new proxy model, config and set QSqlQueryModel worktimeRecs
	 * as it's source model, don't miss to fetch the whole model payload.
	 */
	SfiltMdl *proxyMdl = new SfiltMdl();
	proxyMdl->setDynamicSortFilter(true);
	proxyMdl->setSourceModel(worktimeRecs);
	while (proxyMdl->canFetchMore(QModelIndex()))
		proxyMdl->fetchMore(QModelIndex());

	/*!
	 * Sort the proxy model for date column so we don't need to sort it later.
	 */
	proxyMdl->sort(1);

	typedef QMap<QDate, qreal> records_t;
	QMap<int, records_t> workers;
	records_t recs;

	/*!
	 * Find the absolut min and max date values.
	 */
	QDate n = QDate::currentDate();
	QList<QDate> dates = QList<QDate>()
								<< QDate(n.year(), 1, 1)
								<< QDate(n.year()+1, 1, 1).addDays(-1)
								<< QDate(n.year(), n.month(), 1)
								<< QDate(n.year(), n.month()+1, 1).addDays(-1)
								<< DateTimeRangeMask::getMinDate()
								<< DateTimeRangeMask::getMaxDate();

	QList<QDate> datesSort( dates );

	TICs("calc 1");
	qSort(datesSort.begin(), datesSort.end());

	/*!
	 * Truncate the large proxy model to the upper and lower date bounds of all kind of
	 * date filter ranges.
	 */
	proxyMdl->setFilterMinDate(datesSort.first());
	proxyMdl->setFilterMaxDate(datesSort.last());
	worktimeSums.clear();

	/*!
	 * Loop for each worker PN.
	 */
	foreach (int PN, workerPnId.keys()) {
		proxyMdl->setFilterID(PN);

		worktimeSums.insert(PN, WorktimeSum_t());

		for (int r = 0; r < proxyMdl->rowCount(); r++) {
			QDate cDate = proxyMdl->index(r, 1).data().toDate();

			if (dates.at(2) <= cDate && cDate <= dates.at(3))
				worktimeSums[PN].monthly += proxyMdl->index(r, 2).data().toReal(&ok);
			if (! ok)	INFO << tr("toReal-Error 1");

			if (dates.at(0) <= cDate && cDate <= dates.at(1))
				worktimeSums[PN].yearly += proxyMdl->index(r, 2).data().toReal(&ok);
			if (! ok)	INFO << tr("toReal-Error 1");

			if (dates.at(4) <= cDate && cDate <= dates.at(5))
				worktimeSums[PN].inRange += proxyMdl->index(r, 2).data().toReal(&ok);
			if (! ok)	INFO << tr("toReal-Error 1");
		}
	}
	TOC;

	TICs("calc 1-b");

#define QFOLDINGSTART {
	//#define	DEBUG_FILTER_MODEL
#ifdef	DEBUG_FILTER_MODEL
	foreach (int PN, workerPnId.keys()) {
		INFO << PN
			  << tr(": y:") << worktimeSums[PN].yearly
			  << tr(": r:") << worktimeSums[PN].inRange
			  << tr(": m:") << worktimeSums[PN].monthly;
	}
	int ct = 5;
	qreal ssum = 0;

	foreach (SfiltMdl *proxyModel, workerProxyMdls) {
		MdTable *proxyTbl = new MdTable(tr("none"));

		proxyTbl->tv()->resizeColumnsToContents();
		proxyTbl->tv()->resizeRowsToContents();
		proxyTbl->tv()->setAlternatingRowColors(true);
		proxyTbl->tv()->setFixedSize(proxyTbl->tv()->horizontalHeader()->length() + 600,
											  proxyTbl->tv()->verticalHeader()->length() + 600);
		proxyTbl->tv()->setSortingEnabled(true);
		proxyTbl->tv()->sortByColumn(1, Qt::AscendingOrder);

		while (proxyModel->canFetchMore(QModelIndex()))
			proxyModel->fetchMore(QModelIndex());

		proxyModel->setFilterMinDate(DateTimeRangeMask::getMinDate());
		proxyModel->setFilterMaxDate(DateTimeRangeMask::getMaxDate());
		proxyTbl->tv()->setModel(proxyModel);
		proxyTbl->tv()->show();

		INFO << tr("row cnt: ") << proxyModel->rowCount();

		ssum = 0;
		for (int k = 0; k < proxyModel->rowCount(); k++) {
			ssum += proxyModel->index(k, 2).data().toReal(&ok);
			if (! ok)	INFO << tr("toReal-Error 3");
		}

		if (! --ct)
			break;
	}
#else
#define QFOLDINGEND }

	QList<QString> columns;
	foreach (MdTableInfo::Column_t ti, runtimeColumnInfos)
		columns << ti.name;
	columns.removeFirst();

	/*!
	 * Clear runtime table records.
	 */
	m_runtimeTv->clearRecords(true);

	foreach (int PN, workerPnId.keys()) {
		QSqlQuery q(DbController::db());
		q.clear();
		q.prepare(tr("INSERT INTO runtime (%1) "
						 "VALUES (:ID, :YEAR, :MONTH, :RANGE, 0, 0, 1)")
					 .arg(columns.join(", ")));
		q.bindValue(":ID", workerPnId[PN]);
		q.bindValue(":YEAR", worktimeSums[PN].yearly);
		q.bindValue(":MONTH", worktimeSums[PN].monthly);
		q.bindValue(":RANGE", worktimeSums[PN].inRange);
		q.exec();

		m_runtimeTv->getSqlRtm()->select();

		if (q.lastError().type() != QSqlError::NoError)
			bReturn(q.lastError().text());
	}
	TOC;
#endif
	return true;
}

#define QFOLDINGSTART {
//	proxyModel->setSourceModel(workerRecs);
//	int iLastName = workerRecs->record(1).indexOf("Nachname");
//	QString lastName = workerRecs->record(1).value("Nachname").toString();
//	QRegExp regExp(lastName, Qt::CaseInsensitive, QRegExp::FixedString);
//	proxyModel->setFilterRegExp(regExp);
//	QRegExp regExp(QString::number(PN, 10), Qt::CaseInsensitive,
//						QRegExp::FixedString);
//	proxyModel->setFilterRegExp(regExp);
#define QFOLDINGEND }
#define QFOLDINGSTART {
//	int iKey = workerRecs->record(1).indexOf("PersonalNr");
//	int key = workerRecs->record(1).value("PersonalNr").toInt();
//	proxyModel->setFilterID( key );
////	proxyModel->setFilterMaximumDate(QDate(2016, 12, 25));
////	proxyModel->setFilterMinimumDate(QDate(2000, 12, 25));

//	QTableView *proxyView = new QTableView();
//	proxyView->setModel(proxyModel);
//	proxyView->setAlternatingRowColors(true);
//	proxyView->setSortingEnabled(true);
////	proxyView->sortByColumn(iLastName, Qt::AscendingOrder);
//	proxyView->sortByColumn(iKey, Qt::AscendingOrder);
//	proxyView->show();

//	return true;
#define QFOLDINGEND }
