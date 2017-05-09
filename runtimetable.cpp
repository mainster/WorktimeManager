#include "runtimetable.h"

const QString RuntimeTable::OVERTIME_TABLE_NAME = QString("runtime");

/* ======================================================================== */
/*                        RuntimeTable::RuntimeTable                        */
/* ======================================================================== */


bool RuntimeTable::recalcOvertime() {
	/* ======================================================================== */
	/*                        Query a ColumnSchema list                         */
	/* ======================================================================== */
	worktimeColumnInfos =
			MdTableInfo::queryInfo(qobject_cast<MdTabView *>(m_worktimeTv)
											->sqlTableName(), DbController::db());
	runtimeColumnInfos =
			MdTableInfo::queryInfo(qobject_cast<MdTabView *>(m_runtimeTv)
										  ->sqlTableName(), DbController::db());

	/*!
	 * Query worker records.
	 */
	QSqlQueryModel *workerRecs = new QSqlQueryModel();
	workerRecs->setQuery("SELECT wkt.worktimID, wkt.dat, wkt.hours,"
								"wk.workerID, wk.Nachname, wk.Vorname, wk.PersonalNr "
								"FROM worker wk "
								"INNER JOIN worktime wkt "
								"ON wkt.workerID = wk.workerID");

	//	QTableView *view = new QTableView();
	//	view->setModel(workerRecs);
	//	view->show();

	/*!
	 * Clear old worker proxy models.
	 */
	workerProxyMdls.clear();

	/*!
	 * Create a SfiltMdl proxy model for each employee record ...
	 */
	for (int i = 0; i < workerRecs->rowCount(); ++i) {
		int workerID = workerRecs->record(i).value("workerID").toInt();

		SfiltMdl *sfm = new SfiltMdl();
		workerProxyMdls.insert(workerID, sfm);
		workerProxyMdls[workerID]->setDynamicSortFilter(true);
		workerProxyMdls[workerID]->setSourceModel(workerRecs);
		/*!
		 * Apply column filters
		 */
		workerProxyMdls[workerID]->setFilterID(workerRecs->record(i).value("PersonalNr").toInt());

		workerProxyMdls[workerID]->setFilterMinimumDate(DateTimeRangeMask::getMinDate());
		workerProxyMdls[workerID]->setFilterMaximumDate(DateTimeRangeMask::getMaxDate());
		//		DateTimeRangeMask *dateTimeRngMsk = DateTimeRangeMask::inst();
		//		if (dateTimeRngMsk) {
		//			workerProxyMdls[workerID]->setFilterMinimumDate(dateTimeRngMsk->getDeMinDate()->date());
		//			workerProxyMdls[workerID]->setFilterMaximumDate(dateTimeRngMsk->getDeMaxDate()->date());
		//		}
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

	/*!
	 * Iterate over each worker proxy model and sum the "hours" column.
	 */
	bool ok;
	qreal hoursSum;

	foreach (int workerID, workerProxyMdls.keys()) {
		SfiltMdl *proxyModel = workerProxyMdls.value(workerID);
		hoursSum = 0;

		for (int k = 0; k < proxyModel->rowCount(QModelIndex()); k++) {
			hoursSum += proxyModel->index(k, 2, QModelIndex()).data().toReal(&ok);
			if (! ok)
				INFO << tr("toReal-Error");
		}

		QSqlQuery q(DbController::db());
		q.prepare(tr("INSERT INTO runtime (%1) "
						 "VALUES (:ID, 0, 0, :SUM, 0, 0, 1)").arg(columns.join(", ")));
		q.bindValue(":ID", workerID);
		q.bindValue(":SUM", hoursSum);
		q.exec();

		//		INFO << q.lastQuery() << q.lastInsertId();

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
