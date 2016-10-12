#include "sfiltmdl.h"

/* ======================================================================== */
/*                            SfiltMdl::SfiltMdl                            */
/* ======================================================================== */
void SfiltMdl::setFilterMinDate(const QDate &date) {
	m_minDate = date;
	invalidateFilter();
	//	qDebug() << tr("min date:") << m_minDate.toString("dd.MM.yyyy");
}
void SfiltMdl::setFilterMaxDate(const QDate &date) {
	m_maxDate = date;
	invalidateFilter();
	//	qDebug() << tr("max date:") << m_maxDate.toString("dd.MM.yyyy");
}
void SfiltMdl::setFilterID(const int ID) {
	mID = ID;
	invalidateFilter();
}
/* ======================================================================== */
/*                             filterAcceptsRow                             */
/* ======================================================================== */
bool SfiltMdl::filterAcceptsRow(int srcRow, const QModelIndex &srcParent) const {
	if (mHeadIdxs->map.isEmpty())		return false;
	//	bReturn("Tryed to accept filter but headIdxs is empty!");

	QModelIndex iDate, iMita, iBesc, iPers, iCli, iPrj;

	iDate = sourceModel()->index(srcRow, mHeadIdxs->map.value("Datum", -1), srcParent);
	if (! iDate.isValid())
		iDate = sourceModel()->index(srcRow, mHeadIdxs->map.value("dat", -1), srcParent);

	iMita = sourceModel()->index(srcRow, mHeadIdxs->map.value("Mitarbeiter", -1), srcParent);
	iBesc = sourceModel()->index(srcRow, mHeadIdxs->map.value("Beschreibung", -1), srcParent);
	iPers = sourceModel()->index(srcRow, mHeadIdxs->map.value("PersonalNr", -1), srcParent);
	iCli = sourceModel()->index(srcRow, mHeadIdxs->map.value("Kunde, Name", -1), srcParent);
	iPrj = sourceModel()->index(srcRow, mHeadIdxs->map.value("Kunde, Name", -1), srcParent);

//	INFONR << mHeadIdxs->map.keys();
	/* ======================================================================== */
	/*                    If a filterID has been passed ...                     */
	/* ======================================================================== */
	if (filterID() > -1) {
		/*!
		 * Filter for PersonalNr within a given date range.
		 */
		if (iDate.isValid() && iPers.isValid()) {
			return ((sourceModel()->data( iPers ).toInt() == filterID()) ? true : false) &&
					dateInRange(sourceModel()->data(iDate).toDate());
		}
		/*!
		 * Filter for projects of a given client.
		 */
		if (iCli.isValid())
			return (sourceModel()->data(iCli).toInt() == filterID())
					? true : false;
	}

#ifdef DEBUG_VALIDITY
	QStringList s;

	if (! iDate.isValid()) s << tr("iDate: invalid");
	if (! iMita.isValid()) s << tr("iMita: invalid");
	if (! iBesc.isValid()) s << tr("iBesc: invalid");
	if (! iPers.isValid()) s << tr("iPers: invalid");

	if (! s.isEmpty())
		INFO << s.join(" ");
#endif

	/* ======================================================================== */
	/*  Try to filter for Datum, Mitarb, Beschreibung (inside worktime table)   */
	/* ======================================================================== */
	/*!
	 * Filter expression implemented for sql table "runtime".
	 */
	if (iDate.isValid() && iMita.isValid() && iBesc.isValid() && iPers.isValid())
		return (sourceModel()->data( iMita ).toString().contains(filterRegExp())	||	//< RegExp must match to field Mitarb...
				  sourceModel()->data( iPers ).toString().contains(filterRegExp())		||	//< ... or it must match to field Beschreibung...
				  sourceModel()->data( iBesc ).toString().contains(filterRegExp()))	&&	//< ... or it must match to field PersonalNr...
				dateInRange(sourceModel()->data(iDate).toDate());							//< ... but in all cases, it must be in date range.
	/*!
	 * Filter expression implemented for sql table "worktime".
	 */
	if (iDate.isValid() && iMita.isValid() && iBesc.isValid() /*&& iPers.isValid()*/)
		return (sourceModel()->data( iMita ).toString().contains(filterRegExp())	||
				  sourceModel()->data( iBesc ).toString().contains(filterRegExp()))	&&
				dateInRange(sourceModel()->data(iDate).toDate());
	/*!
	 * Filter expression implemented for sql table ????
	 */
	if (! iDate.isValid() && iMita.isValid() && iBesc.isValid() && iPers.isValid())
		return (sourceModel()->data( iMita ).toString().contains(filterRegExp())	||
				  sourceModel()->data( iPers ).toString().contains(filterRegExp())	||
				  sourceModel()->data( iBesc ).toString().contains(filterRegExp()));
	/*!
	 * Filter expression implemented for MdComboBox's used QStringListModel-proxy:
	 * In a model that holds all columns of sql table "prj", this expression should exclude
	 * all rows where the client doesn't match the filterRegExp().
	 */
	if (! iDate.isValid() && ! iMita.isValid() && ! iPers.isValid() && iCli.isValid() && iPrj.isValid())
		return (sourceModel()->data( iCli ).toString().contains(filterRegExp()));

	/*!
	 * Filter expression implemented for ???
	 */
	if (iCli.isValid())
		return (sourceModel()->data( iCli ).toString().contains(filterRegExp()));

	return false;
	//	bReturn("Bad filter accepts row call!");
}
bool SfiltMdl::lessThan(const QModelIndex &left, const QModelIndex &right) const {
	QVariant leftData = sourceModel()->data(left);
	QVariant rightData = sourceModel()->data(right);

	if (leftData.type() == QVariant::DateTime) {
		return leftData.toDateTime() < rightData.toDateTime();
	}
	else {
		QRegExp *emailPattern = new QRegExp("([\\w\\.]*@[\\w\\.]*)");

		QString leftString = leftData.toString();
		if(left.column() == 1 && emailPattern->indexIn(leftString) != -1)
			leftString = emailPattern->cap(1);

		QString rightString = rightData.toString();
		if(right.column() == 1 && emailPattern->indexIn(rightString) != -1)
			rightString = emailPattern->cap(1);

		return QString::localeAwareCompare(leftString, rightString) < 0;
	}
}
bool SfiltMdl::dateInRange(const QDate &date) const {
	//    qDebug().noquote() << tr("min: ") << minDate
	//                     << tr(" date: ") << date
	//                     << tr(" max: ") << maxDate;

	return (!m_minDate.isValid() || date > m_minDate)
			&& (!m_maxDate.isValid() || date < m_maxDate);
}
void SfiltMdl::setSourceModel(QAbstractItemModel *sourceModel) {
	beginResetModel();
	QSortFilterProxyModel::setSourceModel(sourceModel);
	endResetModel();

	/*!
	 * Create/Update lookup table for table columns. This QMap modifies the
	 * filterAcceptsRow(..) behavior. If the SfiltMdl's sourceModel dosen't
	 * contain "date" columns, the filterAcceptsRow(..) methode must exclude
	 * the condition corresponding to the date range.
	 */
	for (int k = 0; k < columnCount(QModelIndex()); k++)
		mHeadIdxs->map[ headerData(k, Qt::Horizontal).toString() ] = k;

//	INFO << mHeadIdxs->map;

	//	INFO << columnCount(QModelIndex()) << mHeadIdxs;


	//	/*!
	//	 * Write SqlTableName to member field.
	//	 */
	//	mSqlTableName = qobject_cast<MdTabView *>(sourceModel)->sqlTableName();

	//	/*!
	//	 * Query table info list from current table.
	//	 */
	//	mTableInfo = MdTableInfo::queryInfo(mSqlTableName, DbController::db());

	//	INFO << columnIndexes;

	//	columnIndexes.insert(sourceModel->
	//	field
	//	foreach (MdTableInfo::TableInfo_column_t ti, mTableInfo)
	//		if (ti.name.contains("datum"), Qt::CaseInsensitive)
	//			INFO << mSqlTableName << ti;

	//	TOC;
}

