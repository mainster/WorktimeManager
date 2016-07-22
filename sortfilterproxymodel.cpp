#include "sortfilterproxymodel.h"

SortFilterProxyModel::SortFilterProxyModel(QObject *parent)
	: QSortFilterProxyModel(parent) {}

void SortFilterProxyModel::setFilterKeyColumns(const QList<qint32> &filterColumns)
{
	m_columnPatterns.clear();

	foreach(qint32 column, filterColumns)
		m_columnPatterns.insert(column, QString());
}

void SortFilterProxyModel::addFilterFixedString(qint32 column, const QString &pattern)
{
	if(!m_columnPatterns.contains(column))
		return;

	m_columnPatterns[column] = pattern;
}

bool SortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	if(m_columnPatterns.isEmpty())
		return true;

	bool ret = false;

	for(QMap<qint32, QString>::const_iterator iter = m_columnPatterns.constBegin();
		 iter != m_columnPatterns.constEnd();
		 ++iter)
	{
		QModelIndex index = sourceModel()->index(sourceRow, iter.key(), sourceParent);
		INFO << index.data().toString();
		ret = (index.data().toString() == iter.value());

		if(!ret)
			return ret;
	}

	return ret;
}

#include "moc_sortfilterproxymodel.cpp"
