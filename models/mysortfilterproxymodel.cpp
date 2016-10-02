#include <QtGui>
#include "mysortfilterproxymodel.h"

//#define     ID              0
#define     Datum           1
#define     Mitarb          2
#define     Beschreibung    3

/* ======================================================================== */
/*                            SfiltMdl::SfiltMdl                            */
/* ======================================================================== */
SfiltMdl::SfiltMdl(QObject *parent)
	: QSortFilterProxyModel(parent) {
}
void SfiltMdl::setFilterMinimumDate(const QDate &date) {
	m_minDate = date;
	invalidateFilter();
	qDebug() << tr("min date:") << m_minDate.toString("dd.MM.yyyy");
}
void SfiltMdl::setFilterMaximumDate(const QDate &date) {
	m_maxDate = date;
	invalidateFilter();
	qDebug() << tr("max date:") << m_maxDate.toString("dd.MM.yyyy");
}
void SfiltMdl::setFilterID(const int ID) {
	mID = ID;
	invalidateFilter();
}
bool SfiltMdl::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
	if (headIdxs.isEmpty())
		bReturn("tryed to accept filter but headIdxs is empty!");

	INFO << headIdxs["Datum"] << headIdxs["Mitarb"] << headIdxs["Beschreibung"];

	QModelIndex iDate = sourceModel()->index(sourceRow, headIdxs["Datum"], sourceParent);
	QModelIndex iMitarb = sourceModel()->index(sourceRow, headIdxs["Mitarb"], sourceParent);
	QModelIndex iBesch = sourceModel()->index(sourceRow, headIdxs["Beschreibung"], sourceParent);
	QModelIndex iPN = sourceModel()->index(sourceRow, headIdxs["PersonalNr"], sourceParent);

#define DEBUG_ACCEPT_ROW
#ifdef DEBUG_ACCEPT_ROW
	INFO << tr("iMitarb:") << sourceModel()->data(iMitarb).toString().contains(filterRegExp())
		  << tr("|| iPN:") << sourceModel()->data(iPN).toString().contains(filterRegExp())
		  << tr("|| iBesch:") << sourceModel()->data(iBesch).toString().contains(filterRegExp())
		  << tr("&& iDate:") << dateInRange(sourceModel()->data(iDate).toDate());
#endif
	return (sourceModel()->data(iMitarb).toString().contains(filterRegExp()) ||	//< RegExp must match to field Mitarb...
			  sourceModel()->data(iPN).toString().contains(filterRegExp())  ||	//< ... or it must match to field Beschreibung...
			  sourceModel()->data(iBesch).toString().contains(filterRegExp())) &&	//< ... or it must match to field PersonalNr...
			dateInRange(sourceModel()->data(iDate).toDate());								//< ... but in all cases, it must be in date range.
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
	 * Create/Update lookup table for table columns
	 */
	for (int k = 0; k < columnCount(QModelIndex()); k++)
		headIdxs.insert(headerData(k, Qt::Horizontal).toString(), k);

	INFO << headIdxs;
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


