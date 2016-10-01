#include "runtimetable.h"

const QString RuntimeTable::OVERTIME_TABLE_NAME = QString("runtime");

/* ======================================================================== */
/*                        RuntimeTable::RuntimeTable                        */
/* ======================================================================== */


bool RuntimeTable::recalcOvertime() {
	/* ======================================================================== */
	/*                        Query a ColumnSchema list                         */
	/* ======================================================================== */
	//	QList<MdTabView::TableInfo_column> columnSchemas =
	//			qobject_cast<MdTabView *>(m_)->queryTableInfo();

	/*!
	 * Query worker records.
	 */
	QSqlQueryModel workerRecs;
	workerRecs.setQuery("SELECT * FROM worker");

	/*!
	 * For each employee record ...
	 */
	for (int i = 0; i < workerRecs.rowCount(); ++i) {
		SfiltMdl *proxyMdl = new SfiltMdl();

		proxyMdl->setDynamicSortFilter(true);
		proxyMdl->setSourceModel(m_worktimeTv->model());

		INFO << tr("before filter/sort") << proxyMdl->rowCount(QModelIndex());
		/* ======================================================================== */
		/*                               Apply filter                               */
		/* ======================================================================== */
		QRegExp regExp(workerRecs.record(i).value("Nachname").toString(),
							Qt::CaseInsensitive, QRegExp::FixedString);
		proxyMdl->setFilterRegExp(regExp);
		proxyMdl->setFilterKeyColumn( 1 );

		/* ======================================================================== */
		/*                            Apply column sort.                            */
		/* ======================================================================== */
		proxyMdl->sort( 1 , Qt::AscendingOrder);

#define DEBUG_FILTER_MODEL
#ifdef DEBUG_FILTER_MODEL
		QTableView *sfv = new QTableView();
		sfv->resizeColumnsToContents();
		sfv->resizeRowsToContents();

		sfv->setFixedSize(sfv->horizontalHeader()->length() + 600,
								sfv->verticalHeader()->length() + 600);

		sfv->setModel(proxyMdl);
		sfv->show();

#endif

		INFO << tr("after filter/sort") << proxyMdl->rowCount(QModelIndex());


		INFO << workerRecs.record(i).value("workerID").toInt()
			  << workerRecs.record(i).value("Nachname").toString()
			  << workerRecs.record(i).value("Vorname").toString()
			  << workerRecs.record(i).value("PersonalNr").toString()
			  << workerRecs.record(i).value("Wochenstunden").toString();
	}

	//	foreach (MdTabView::TableInfo_column ti, columnSchemas)
	//		INFO << ti.name << ti.type;

	return true;
}
