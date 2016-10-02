#include "runtimetable.h"

const QString RuntimeTable::OVERTIME_TABLE_NAME = QString("runtime");

/* ======================================================================== */
/*                        RuntimeTable::RuntimeTable                        */
/* ======================================================================== */


bool RuntimeTable::recalcOvertime() {
	/* ======================================================================== */
	/*                        Query a ColumnSchema list                         */
	/* ======================================================================== */
	columnInfos = MdTableInfo::queryInfo(qobject_cast<MdTabView *>(
														 m_worktimeTv)->sqlTableName(),
													 DbController::db());

	/*!
	 * Query worker records.
	 */
	QSqlQueryModel *workerRecs = new QSqlQueryModel();
	//	workerRecs->setQuery("SELECT * FROM worker");
	//	workerRecs->clear();
	workerRecs->setQuery("SELECT wkt.worktimID, wkt.dat, "
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

	SfiltMdl *proxyModel = new SfiltMdl(this);
	proxyModel->setSourceModel(workerRecs);
	proxyModel->setDynamicSortFilter(true);

//	proxyModel->setSourceModel(workerRecs);
//	int iLastName = workerRecs->record(1).indexOf("Nachname");
//	QString lastName = workerRecs->record(1).value("Nachname").toString();
//	QRegExp regExp(lastName, Qt::CaseInsensitive, QRegExp::FixedString);
//	proxyModel->setFilterRegExp(regExp);
//	QRegExp regExp(QString::number(PN, 10), Qt::CaseInsensitive,
//						QRegExp::FixedString);
//	proxyModel->setFilterRegExp(regExp);

	int iKey = workerRecs->record(1).indexOf("workerID");
	int key = workerRecs->record(1).value("workerID").toInt();
	proxyModel->setFilterID( key );
	proxyModel->setFilterMaximumDate(QDate(2016, 12, 25));
	proxyModel->setFilterMinimumDate(QDate(2000, 12, 25));

	QTableView *proxyView = new QTableView();
	proxyView->setModel(proxyModel);
	proxyView->setAlternatingRowColors(true);
	proxyView->setSortingEnabled(true);
//	proxyView->sortByColumn(iLastName, Qt::AscendingOrder);
	proxyView->sortByColumn(iKey, Qt::AscendingOrder);
	proxyView->show();

	return true;

	/*!
	 * For each employee record ...
	 */
	for (int i = 0; i < 2/*workerRecs->rowCount()*/; ++i) {
		int iPN = workerRecs->record(i).indexOf("PersonalNr");
		int PN = workerRecs->record(i).value("PersonalNr").toInt();
		SfiltMdl *sfm = new SfiltMdl();
		workerProxyMdls.insert( PN, sfm);
		workerProxyMdls[ PN ]->setDynamicSortFilter(true);
		//				workerProxyMdls[ PN ]->setSourceModel(m_worktimeTv->model());
		//		QTableView *dummy = new QTableView();
		//		dummy->hide();
		//		dummy->setModel(workerRecs);
		workerProxyMdls[ PN ]->setSourceModel(workerRecs);

		/*!
		 * Apply column filter and sort direction.
		 */
		workerProxyMdls[ PN ]->setFilterFixedString(QString::number( PN ));
		workerProxyMdls[ PN ]->setFilterKeyColumn( iPN );
		workerProxyMdls[ PN ]->sort( iPN , Qt::AscendingOrder);

		INFO << workerProxyMdls[ PN ];
		INFO << workerProxyMdls[ PN ]->filterKeyColumn();
	}

#define	DEBUG_FILTER_MODEL
#ifdef	DEBUG_FILTER_MODEL
	foreach (SfiltMdl *proxyModel, workerProxyMdls) {
		QTableView *sfv = new QTableView();
		sfv->resizeColumnsToContents();
		sfv->resizeRowsToContents();

		sfv->setFixedSize(sfv->horizontalHeader()->length() + 600,
								sfv->verticalHeader()->length() + 600);

		sfv->setModel(proxyModel);
		sfv->show();
	}
#endif

	for (int k = 0; k < workerProxyMdls.count(); k++) {
		INFO << workerRecs->record(k).value("workerID").toInt()
			  << workerRecs->record(k).value("Nachname").toString()
			  << workerRecs->record(k).value("Vorname").toString()
			  << workerRecs->record(k).value("Wochenstunden").toString()
			  << workerRecs->record(k).value("PersonalNr").toString();
	}

	return true;
}
