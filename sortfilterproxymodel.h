#ifndef SORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QObject>
#include "debug.h"

class SortFilterProxyModel : public QSortFilterProxyModel {

	Q_OBJECT

public:
	explicit SortFilterProxyModel(QObject *parent = 0);
	void setFilterKeyColumns(const QList<qint32> &filterColumns);
	void addFilterFixedString(qint32 column, const QString &pattern);

protected:
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
	QMap<qint32, QString> m_columnPatterns;
};

#endif // SORTFILTERPROXYMODEL_H
