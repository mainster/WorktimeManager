#ifndef SFILTMDL_H
#define SFILTMDL_H


#include <QObject>
#include <QtWidgets>
#include <QSortFilterProxyModel>
#include <QAbstractItemModel>
#include <QAbstractTableModel>
#include <QtCore>

#include "debug.h"
#include "globals.h"
#include "headeralias.h"
#include "mdtableinfo.h"
#include "types.h"



/* ======================================================================== */
/*                         Sort filter proxy model                          */
/* ======================================================================== */
class SfiltMdl : public QSortFilterProxyModel {

	Q_OBJECT

 public:
	explicit SfiltMdl(QObject *parent = nullptr)
		: QSortFilterProxyModel(parent), mID(-1), mHeadIdxs(new HeaderAlias()) {	}
	explicit SfiltMdl(const SfiltMdl &others, QObject *parent = nullptr)
		: QSortFilterProxyModel(parent) {
		m_minDate = others.m_minDate;
		m_maxDate = others.m_maxDate;
		mHeadIdxs = others.mHeadIdxs;
	}
	~SfiltMdl() { }

	QDate filterMinimumDate() const {
		return m_minDate;
	}
	void setFilterMinDate(const QDate &date);

	QDate filterMaximumDate() const {
		return m_maxDate;
	}
	void setFilterMaxDate(const QDate &date);

	void setFilterID(const int ID);
	int filterID() const {
		return mID;
	}

	virtual void setSourceModel(QAbstractItemModel *sourceModel) override;

	//	QPointer<HeaderAlias> headIdxs() const { return mHeadIdxs; }
	HeaderAlias *headIdxs() const {
		return mHeadIdxs;
	}

 protected:
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
	bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

 private:
	bool dateInRange(const QDate &date) const;
	QDate m_minDate;
	QDate m_maxDate;
	int	mID;
	QList<MdTableInfo::Column_t> mTableInfo;
	QString mSqlTableName;

	//	QPointer<HeaderAlias> mHeadIdxs;
	HeaderAlias *mHeadIdxs;
	//	QMap<QString , int> headIdxs;

};


/* ======================================================================== */
/*                       class MySortFilterProxyModel                       */
/* ======================================================================== */
class MySortFilterProxyModel : public QSortFilterProxyModel {
	Q_OBJECT

 public:
	MySortFilterProxyModel(QObject *parent = nullptr);

	QDate filterMinimumDate() const {
		return minDate;
	}
	void setFilterMinimumDate(const QDate &date);

	QDate filterMaximumDate() const {
		return maxDate;
	}
	void setFilterMaximumDate(const QDate &date);

 protected:
	bool filterAcceptsRow(int sourceRow,
								 const QModelIndex &sourceParent) const Q_DECL_OVERRIDE;
	bool lessThan(const QModelIndex &left,
					  const QModelIndex &right) const Q_DECL_OVERRIDE;

 private:
	bool dateInRange(const QDate &date) const;

	QDate minDate;
	QDate maxDate;
};

#endif
