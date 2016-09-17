#include <QtGui>
#include "mysortfilterproxymodel.h"

#define     ID              0
#define     Datum           1
#define     Mitarb          2
#define     Beschreibung    3

/* ======================================================================== */
/*                                  SfiltMdl                                */
/* ======================================================================== */
void SfiltMdl::setFilterMinimumDate(const QDate &date) {
    minDate = date;
    invalidateFilter();
    qDebug() << tr("min date:") << minDate.toString("dd.MM.yyyy");
}
void SfiltMdl::setFilterMaximumDate(const QDate &date) {
    maxDate = date;
    invalidateFilter();
    qDebug() << tr("max date:") << maxDate.toString("dd.MM.yyyy");
}

bool SfiltMdl::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    QModelIndex idxDate = sourceModel()->index(sourceRow, Datum, sourceParent);
    QModelIndex idxMitarb = sourceModel()->index(sourceRow, Mitarb, sourceParent);
    QModelIndex idxBesch = sourceModel()->index(sourceRow, Beschreibung, sourceParent);

    return (sourceModel()->data(idxMitarb).toString().contains(filterRegExp()) ||
            sourceModel()->data(idxBesch).toString().contains(filterRegExp())) &&
            dateInRange(sourceModel()->data(idxDate).toDate());
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

    return (!minDate.isValid() || date > minDate)
           && (!maxDate.isValid() || date < maxDate);
}

/* ======================================================================== */
/*                            SortFilterProxyModel                          */
/* ======================================================================== */
void SortFilterProxyModel::setFilterKeyColumns(const QList<qint32> &filterColumns) {
    m_columnPatterns.clear();

    foreach(qint32 column, filterColumns)
        m_columnPatterns.insert(column, QString());
}
void SortFilterProxyModel::addFilterFixedString(qint32 column, const QString &pattern) {
    if(!m_columnPatterns.contains(column))
        return;

    m_columnPatterns[column] = pattern;
}
bool SortFilterProxyModel::filterAcceptsRow(int sourceRow,
                                            const QModelIndex &sourceParent) const {
    if(m_columnPatterns.isEmpty())
        return true;

    bool ret = false;

    for(QMap<qint32, QString>::const_iterator iter = m_columnPatterns.constBegin();
         iter != m_columnPatterns.constEnd(); ++iter) {
        QModelIndex index = sourceModel()->index(sourceRow, iter.key(), sourceParent);
        ret = (index.data().toString() == iter.value());

        if(!ret)		return ret;
    }
    return ret;
}
void SfiltMdl::setSourceModel(QAbstractItemModel *sourceModel) {
	TICs("setSourceModel()");
	beginResetModel();
	QSortFilterProxyModel::setSourceModel(sourceModel);
	endResetModel();
	TOC;
}

#include "moc_mysortfilterproxymodel.cpp"


