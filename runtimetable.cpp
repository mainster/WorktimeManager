#include "runtimetable.h"

const QString RuntimeTable::OVERTIME_TABLE_NAME = QString("runtime");

/* ======================================================================== */
/*                        RuntimeTable::RuntimeTable                        */
/* ======================================================================== */
bool RuntimeTable::recalcOvertime() {
	bool ok;
	/*!
	 * Query a ColumnSchema list.
	 */
	worktimeColumnInfos = MdTableInfo::queryInfo(qobject_cast<MdTabView *>(
																	m_worktimeTv)->sqlTableName(), DbController::db());
	runtimeColumnInfos = MdTableInfo::queryInfo(qobject_cast<MdTabView *>(
																  m_runtimeTv)->sqlTableName(), DbController::db());
	/*!
	 * Query worker records.
	 */
	QSqlQueryModel *workerRecs = new QSqlQueryModel();
	workerRecs->setQuery("SELECT wkt.worktimID, wkt.dat, wkt.hours,"
								"wk.workerID, wk.Nachname, wk.Vorname, wk.PersonalNr "
								"FROM worker wk "
								"INNER JOIN worktime wkt "
								"ON wkt.workerID = wk.workerID");
	/*!
	 * Clear old worker proxy models.
	 */
	workerProxyMdls.clear();

	/*!
	 * Create a SfiltMdl proxy model for each employee record ...
	 */
	for (int i = 0; i < workerRecs->rowCount(); ++i) {
		int ID = workerRecs->record(i).value("workerID").toInt();

		SfiltMdl *sfm = new SfiltMdl();
		workerProxyMdls.insert(ID, sfm);
		workerProxyMdls[ID]->setDynamicSortFilter(true);
		workerProxyMdls[ID]->setSourceModel(workerRecs);
		/*!
		 * Apply column filters
		 */
		workerProxyMdls[ID]->setFilterID(workerRecs->record(i).value("PersonalNr").toInt());

		QDate n = QDate::currentDate();

		/*!
		 * Setup date filter min and max to fit the running month, eg. if QDate::currentDate
		 * returns 11.05.2016, set the date range to 01.05.2016 ... (01.06.2016 - 1 day).
		 * Iterate over each worker proxy model and sum the "hours" column.
		 */
		workerProxyMdls[ID]->setFilterMinDate(QDate(n.year(), n.month(), 1));
		workerProxyMdls[ID]->setFilterMaxDate(QDate(n.year(), n.month() + 1, 1).addDays(-1));

		worktimeSums.insert(ID, WorktimeSum_t());
		for (int k = 0; k < workerProxyMdls[ID]->rowCount(QModelIndex()); k++) {
			worktimeSums[ID].monthly +=
					workerProxyMdls[ID]->index(k, 2, QModelIndex()).data().toReal(&ok);
			if (! ok)	INFO << tr("toReal-Error");
		}

		/*!
		 * Setup date filter min and max to fit the running year, eg. if QDate::currentDate
		 * returns 11.05.2016, set the date range to 01.01.2016 ... (01.01.2017 - 1 day).
		 */
		workerProxyMdls[ID]->setFilterMinDate(QDate(n.year(), 1, 1));
		workerProxyMdls[ID]->setFilterMaxDate(QDate(n.year() + 1, 1, 1).addDays(-1));

		for (int k = 0; k < workerProxyMdls[ID]->rowCount(QModelIndex()); k++) {
			worktimeSums[ID].yearly +=
					workerProxyMdls[ID]->index(k, 2, QModelIndex()).data().toReal(&ok);
			if (! ok)	INFO << tr("toReal-Error");
		}

		/*!
		 * Setup date filter min and max to fit the range passed from DateTimeRangeMask.
		 */
		workerProxyMdls[ID]->setFilterMinDate(DateTimeRangeMask::getMinDate());
		workerProxyMdls[ID]->setFilterMaxDate(DateTimeRangeMask::getMaxDate());

		for (int k = 0; k < workerProxyMdls[ID]->rowCount(QModelIndex()); k++) {
			worktimeSums[ID].inRange +=
					workerProxyMdls[ID]->index(k, 2, QModelIndex()).data().toReal(&ok);
			if (! ok)	INFO << tr("toReal-Error");
		}

	}

	//	#define	DEBUG_FILTER_MODEL
#ifdef	DEBUG_FILTER_MODEL
	foreach (SfiltMdl *proxyModel, workerProxyMdls) {
		QTableView *proxyView = new QTableView();
		proxyView->resizeColumnsToContents();
		proxyView->resizeRowsToContents();
		proxyView->setAlternatingRowColors(true);
		proxyView->setFixedSize(proxyView->horizontalHeader()->length() + 600,
										proxyView->verticalHeader()->length() + 600);
		proxyView->setSortingEnabled(true);
		proxyView->sortByColumn(iKey, Qt::AscendingOrder);

		proxyView->setModel(proxyModel);
		proxyView->show();
	}
#else

	QList<QString> columns;
	foreach (MdTableInfo::TableInfo_column_t ti, runtimeColumnInfos)
		columns << ti.name;

	columns.removeFirst();

	/*!
	 * Clear runtime table records.
	 */
	m_runtimeTv->clearRecords();
	QSqlQuery q(DbController::db());

	foreach (int ID, workerProxyMdls.keys()) {
		SfiltMdl *proxyModel = workerProxyMdls.value(ID);

		q.clear();
		q.prepare(tr("INSERT INTO runtime (%1) "
						 "VALUES (:ID, :YEAR, :MONTH, :RANGE, 0, 0, 1)").arg(columns.join(", ")));
		q.bindValue(":ID", ID);
		q.bindValue(":YEAR", worktimeSums[ID].yearly);
		q.bindValue(":MONTH", worktimeSums[ID].monthly);
		q.bindValue(":RANGE", worktimeSums[ID].inRange);
		q.exec();

		m_runtimeTv->getSqlRtm()->select();

		if (q.lastError().type() != QSqlError::NoError) {
			INFO << q.lastError().text();
			return false;
		}
	}
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
