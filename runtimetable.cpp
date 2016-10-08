#include "runtimetable.h"

const QString RuntimeTable::OVERTIME_TABLE_NAME = QString("runtime");

/* ======================================================================== */
/*                        RuntimeTable::RuntimeTable                        */
/* ======================================================================== */


bool RuntimeTable::recalcOvertime() {
	/* ======================================================================== */
	/*                        Query a ColumnSchema list                         */
	/* ======================================================================== */
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
	//	workerRecs->setHeaderData(0, Qt::Horizontal, tr("ID"));
	//	workerRecs->setHeaderData(1, Qt::Horizontal, tr("Date"));
	//	workerRecs->setHeaderData(2, Qt::Horizontal, tr("last"));
	//	workerRecs->setHeaderData(3, Qt::Horizontal, tr("first"));
	//	workerRecs->setHeaderData(4, Qt::Horizontal, tr("PN"));

	QTableView *view = new QTableView();
	view->setModel(workerRecs);
	view->show();

	/*!
	 * Clear old worker proxy models.
	 */
	workerProxyMdls.clear();
	/*!
	 * Create a SfiltMdl proxy model for each employee record ...
	 */
	//	int iKey = workerRecs->record(0).indexOf("PersonalNr");

	for (int i = 0; i < workerRecs->rowCount(); ++i) {
		//		int PN = workerRecs->record(i).value("PersonalNr").toInt();
		int workerID = workerRecs->record(i).value("workerID").toInt();

		SfiltMdl *sfm = new SfiltMdl();
		workerProxyMdls.insert(workerID, sfm);
		workerProxyMdls[workerID]->setDynamicSortFilter(true);
		workerProxyMdls[workerID]->setSourceModel(workerRecs);
		/*!
		 * Apply column filters
		 */
		workerProxyMdls[workerID]->setFilterID(workerRecs->record(i).value("PersonalNr").toInt());
		workerProxyMdls[workerID]->setFilterMinimumDate(QDate(2015,1,1));
		workerProxyMdls[workerID]->setFilterMaximumDate(QDate(2016,1,1));
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
	SqlRtm *runtimeTblMdl = dynamic_cast<SqlRtm *>(m_runtimeTv->model());

	QList<QString> columns;
	foreach (MdTableInfo::TableInfo_column_t ti, runtimeColumnInfos)
		columns << ti.name;

	columns.removeFirst();

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
			if (! ok)	INFO << tr("toReal-Error");

			//			QStringList s;
			//			for (int i = 0; i < proxyModel->columnCount(QModelIndex()); i++)
			//				s << proxyModel->index(k, i, QModelIndex()).data().toString();

			//			INFO << s.join(',');
		}

		QSqlQuery q(DbController::db());
		q.prepare("INSERT INTO runtime (workerID, Wochensumme, Monatssumme, Jahressumme, Ueberstunden, Vorschuss, fehlID) "
					 "VALUES (:ID, 0, 0, :SUM, 0, 0, 1)");
		q.bindValue(":ID", workerID);
		q.bindValue(":SUM", hoursSum);
		q.exec();
//		m_runtimeTv->getActSubmit()->trigger();

		INFO << q.lastQuery() << q.lastInsertId();

		m_runtimeTv->getSqlRtm()->select();

		if (q.lastError().type() != QSqlError::NoError) {
			INFO << q.lastError().text();
			return false;
		}


		//		QSqlQuery query(tr("INSERT INTO runtime (%1) VALUES (%2, %3, %4, %5, %6, %7, %8)")
		//							 .arg(columns.join(", "))
		//							 .arg(workerID)
		//							 .arg(-1)
		//							 .arg(-1)
		//							 .arg(hoursSum)
		//							 .arg(-1)
		//							 .arg(0)
		//							 .arg(0));

		//		QMapIterator<QString, QVariant> i(query->boundValues());
		//		while (i.hasNext()) {
		//			 i.next();
		//			 INFO << i.key().toUtf8().data() << ": "
		//					<< i.value().toString().toUtf8().data();
		//		}

		//		INFO << query->exec() << query->lastQuery();
		//		QSqlQuery query(tr(""))
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
