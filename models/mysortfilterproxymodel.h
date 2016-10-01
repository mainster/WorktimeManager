#ifndef MYSORTFILTERPROXYMODEL_H
#define MYSORTFILTERPROXYMODEL_H

#include <QDate>
#include <QSortFilterProxyModel>
#include "debug.h"

/* ======================================================================== */
/*                                  SfiltMdl                                */
/* ======================================================================== */
class SfiltMdl : public QSortFilterProxyModel {

	Q_OBJECT

public:
	explicit SfiltMdl(QObject *parent = 0)
		: QSortFilterProxyModel(parent) { }
	explicit SfiltMdl(SfiltMdl &others, QObject *parent = 0)
		: QSortFilterProxyModel(parent) {
		minDate = others.minDate;
		maxDate = others.maxDate;
	}
	~SfiltMdl() { }

	QDate filterMinimumDate() const { return minDate; }
	void setFilterMinimumDate(const QDate &date);

	QDate filterMaximumDate() const { return maxDate; }
	void setFilterMaximumDate(const QDate &date);
	virtual void setSourceModel(QAbstractItemModel *sourceModel) override;

protected:
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
	bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
	bool dateInRange(const QDate &date) const;
	QDate minDate;
	QDate maxDate;
};

/* ======================================================================== */
/*                            SortFilterProxyModel                          */
/* ======================================================================== */
class SortFilterProxyModel : public QSortFilterProxyModel {

	Q_OBJECT

public:
	explicit SortFilterProxyModel(QObject *parent = 0)
		: QSortFilterProxyModel(parent) {}

	void setFilterKeyColumns(const QList<qint32> &filterColumns);
	void addFilterFixedString(qint32 column, const QString &pattern);

protected:
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
	QMap<qint32, QString> m_columnPatterns;
};

#endif
