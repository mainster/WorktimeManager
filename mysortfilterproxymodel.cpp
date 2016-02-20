#include <QtGui>

#include "mysortfilterproxymodel.h"

MySortFilterProxyModel::MySortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent) {
}

void MySortFilterProxyModel::setFilterMinimumDate(const QDate &date) {
    minDate = date;
    invalidateFilter();
    qDebug() << tr("min date:") << minDate.toString("dd.MM.yyyy");
}

void MySortFilterProxyModel::setFilterMaximumDate(const QDate &date) {
    maxDate = date;
    invalidateFilter();
    qDebug() << tr("max date:") << maxDate.toString("dd.MM.yyyy");
}

#define     ID              0
#define     Datum           1
#define     Mitarb          2
#define     Beschreibung    3

bool MySortFilterProxyModel::filterAcceptsRow(int sourceRow,
                                              const QModelIndex &sourceParent) const {
    QModelIndex idxDate = sourceModel()->index(sourceRow, Datum, sourceParent);
    QModelIndex idxMitarb = sourceModel()->index(sourceRow, Mitarb, sourceParent);
    QModelIndex idxBesch = sourceModel()->index(sourceRow, Beschreibung, sourceParent);

    return (sourceModel()->data(idxMitarb).toString().contains(filterRegExp()) ||
            sourceModel()->data(idxBesch).toString().contains(filterRegExp())) &&
            dateInRange(sourceModel()->data(idxDate).toDate());
}

bool MySortFilterProxyModel::lessThan(const QModelIndex &left,
                                      const QModelIndex &right) const {
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

bool MySortFilterProxyModel::dateInRange(const QDate &date) const {
//    qDebug().noquote() << tr("min: ") << minDate
//                     << tr(" date: ") << date
//                     << tr(" max: ") << maxDate;

    return (!minDate.isValid() || date > minDate)
           && (!maxDate.isValid() || date < maxDate);
}
