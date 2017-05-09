#include <QtGui>
#include "mysortfilterproxymodel.h"

//#define     ID              0
#define     Datum           1
#define     Mitarb          2
#define     Beschreibung    3

/* ======================================================================== */
/*                            SfiltMdl::SfiltMdl                            */
/* ======================================================================== */

void SfiltMdl::setFilterMinimumDate(const QDate &date) {
	m_minDate = date;
	invalidateFilter();
//	qDebug() << tr("min date:") << m_minDate.toString("dd.MM.yyyy");
}
void SfiltMdl::setFilterMaximumDate(const QDate &date) {
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
bool SfiltMdl::filterAcceptsRow(int srcRow, const QModelIndex &sourceParent) const {
	if (mHeadIdxs->map.isEmpty())
		return false;
//	bReturn("Tryed to accept filter but headIdxs is empty!");

	QModelIndex iDate = sourceModel()->index(
								  srcRow, mHeadIdxs->map.value("Datum", -1), sourceParent);
	if (! iDate.isValid())
		iDate = sourceModel()->index(
					  srcRow, mHeadIdxs->map.value("dat", -1), sourceParent);
	QModelIndex iMita = sourceModel()->index(
								  srcRow, mHeadIdxs->map.value("Mitarbeiter", -1), sourceParent);
	QModelIndex iBesc = sourceModel()->index(
								  srcRow, mHeadIdxs->map.value("Beschreibung", -1), sourceParent);
	QModelIndex iPers = sourceModel()->index(
								  srcRow, mHeadIdxs->map.value("PersonalNr", -1), sourceParent);

	/* ======================================================================== */
	/*                       Try to filter for PersonalNr                       */
	/*									Within a given date range								 */
	/* ======================================================================== */
	if (filterID() > -1) {
		if (iDate.isValid() && iPers.isValid())
			return ((sourceModel()->data(iPers).toInt() == filterID()) ? true : false) &&
					dateInRange(sourceModel()->data(iDate).toDate());
	}

	/* ======================================================================== */
	/*  Try to filter for Datum, Mitarb, Beschreibung (inside worktime table)   */
	/* ======================================================================== */

	QStringList s;

	if (! iDate.isValid()) s << tr("iDate: invalid");
	if (! iMita.isValid()) s << tr("iMita: invalid");
	if (! iBesc.isValid()) s << tr("iBesc: invalid");
	if (! iPers.isValid()) s << tr("iPers: invalid");

//	if (! s.isEmpty())
//		INFO << s.join(" ");

	if (iDate.isValid() && iMita.isValid() && iBesc.isValid() && iPers.isValid())
		return (sourceModel()->data(iMita).toString().contains(filterRegExp())	||	//< RegExp must match to field Mitarb...
				  sourceModel()->data(iPers).toString().contains(filterRegExp())		||	//< ... or it must match to field Beschreibung...
				  sourceModel()->data(iBesc).toString().contains(filterRegExp()))	&&	//< ... or it must match to field PersonalNr...
				dateInRange(sourceModel()->data(iDate).toDate());							//< ... but in all cases, it must be in date range.

	if (iDate.isValid() && iMita.isValid() && iBesc.isValid() /*&& iPers.isValid()*/)
		return (sourceModel()->data(iMita).toString().contains(filterRegExp())	||	//< RegExp must match to field Mitarb...
				  sourceModel()->data(iBesc).toString().contains(filterRegExp()))	&&	//< ... or it must match to field PersonalNr...
				dateInRange(sourceModel()->data(iDate).toDate());							//< ... but in all cases, it must be in date range.

	if (! iDate.isValid() && iMita.isValid() && iBesc.isValid() && iPers.isValid())
		return (sourceModel()->data(iMita).toString().contains(filterRegExp())	||	//< RegExp must match to field Mitarb...
				  sourceModel()->data(iPers).toString().contains(filterRegExp())		||	//< ... or it must match to field Beschreibung...
				  sourceModel()->data(iBesc).toString().contains(filterRegExp()));	//< ... or it must match to field PersonalNr...

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




MySortFilterProxyModel::MySortFilterProxyModel(QObject *parent)
	: QSortFilterProxyModel(parent) {
}
void MySortFilterProxyModel::setFilterMinimumDate(const QDate &date) {
	minDate = date;
	invalidateFilter();
}
void MySortFilterProxyModel::setFilterMaximumDate(const QDate &date) {
	maxDate = date;
	invalidateFilter();
}
bool MySortFilterProxyModel::filterAcceptsRow(int sourceRow,
															 const QModelIndex &sourceParent) const {
	QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
	QModelIndex index1 = sourceModel()->index(sourceRow, 1, sourceParent);
	QModelIndex index2 = sourceModel()->index(sourceRow, 2, sourceParent);

	return (sourceModel()->data(index0).toString().contains(filterRegExp())
			  || sourceModel()->data(index1).toString().contains(filterRegExp()))
			 && dateInRange(sourceModel()->data(index2).toDate());
}
bool MySortFilterProxyModel::lessThan(const QModelIndex &left,
												  const QModelIndex &right) const {
	QVariant leftData = sourceModel()->data(left);
	QVariant rightData = sourceModel()->data(right);



	if (leftData.type() == QVariant::DateTime) {
		return leftData.toDateTime() < rightData.toDateTime();
	} else {
		static QRegExp emailPattern("[\\w\\.]*@[\\w\\.]*)");

		QString leftString = leftData.toString();

		if (left.column() == 1 && emailPattern.indexIn(leftString) != -1)
			leftString = emailPattern.cap(1);

		QString rightString = rightData.toString();

		if (right.column() == 1 && emailPattern.indexIn(rightString) != -1)
			rightString = emailPattern.cap(1);

		return QString::localeAwareCompare(leftString, rightString) < 0;
	}
}
bool MySortFilterProxyModel::dateInRange(const QDate &date) const {
	return (!minDate.isValid() || date > minDate)
			 && (!maxDate.isValid() || date < maxDate);
}


///* ======================================================================== */
///*                            SortFilterProxyModel                          */
///* ======================================================================== */
//void SortFilterProxyModel::setFilterKeyColumns(const QList<qint32> &filterColumns) {
//	m_columnPatterns.clear();

//	foreach(qint32 column, filterColumns)
//		m_columnPatterns.insert(column, QString());
//}
//void SortFilterProxyModel::addFilterFixedString(qint32 column, const QString &pattern) {
//	if(!m_columnPatterns.contains(column))
//		return;

//	m_columnPatterns[column] = pattern;
//}
//bool SortFilterProxyModel::filterAcceptsRow(int sourceRow,
//														  const QModelIndex &sourceParent) const {
//	if(m_columnPatterns.isEmpty())
//		return true;

//	bool ret = false;

//	for(QMap<qint32, QString>::const_iterator iter = m_columnPatterns.constBegin();
//		 iter != m_columnPatterns.constEnd(); ++iter) {
//		QModelIndex index = sourceModel()->index(sourceRow, iter.key(), sourceParent);
//		ret = (index.data().toString() == iter.value());

//		if(!ret)		return ret;
//	}
//	return ret;
//}

#include "moc_mysortfilterproxymodel.cpp"



////#define DEBUG_ACCEPT_ROW
//#ifdef DEBUG_ACCEPT_ROW
//	INFO << tr("iMitarb:") << sourceModel()->data(iMitarb).toString().contains(filterRegExp())
//		  << tr("|| iPN:") << sourceModel()->data(iPN).toString().contains(filterRegExp())
//		  << tr("|| iBesch:") << sourceModel()->data(iBesch).toString().contains(filterRegExp())
//		  << tr("&& iDate:") << dateInRange(sourceModel()->data(iDate).toDate());
//#endif
