#ifndef SFILTMDL_H
#define SFILTMDL_H


#include <QObject>
#include <QtWidgets>
#include <QSortFilterProxyModel>
#include <QAbstractItemModel>
#include <QAbstractTableModel>
#include <QtCore>

#include "dbcontroller.h"
#include "debug.h"
#include "globals.h"
#include "locals.h"
#include "headeralias.h"
#include "sfiltmdl.h"
#include "mdtableinfo.h"
#include "types.h"



/* ======================================================================== */
/*                         Sort filter proxy model                          */
/* ======================================================================== */
class SfiltMdl : public QSortFilterProxyModel {

	Q_OBJECT

public:
	explicit SfiltMdl(QObject *parent = 0)
		: QSortFilterProxyModel(parent), mID(-1), mHeadIdxs(new HeaderAlias()) {	}
	explicit SfiltMdl(const SfiltMdl &others, QObject *parent = 0)
		: QSortFilterProxyModel(parent) {
		m_minDate = others.m_minDate;
		m_maxDate = others.m_maxDate;
		mHeadIdxs = others.mHeadIdxs;
	}
	~SfiltMdl() { }

	QDate filterMinimumDate() const { return m_minDate; }
	void setFilterMinDate(const QDate &date);

	QDate filterMaximumDate() const { return m_maxDate; }
	void setFilterMaxDate(const QDate &date);

	void setFilterID(const int ID);
	int filterID() const { return mID; }

	virtual void setSourceModel(QAbstractItemModel *sourceModel) override;

//	QPointer<HeaderAlias> headIdxs() const { return mHeadIdxs; }
	HeaderAlias *headIdxs() const { return mHeadIdxs; }

protected:
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
	bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
	bool dateInRange(const QDate &date) const;
	QDate m_minDate;
	QDate m_maxDate;
	int	mID;
	QList<MdTableInfo::TableInfo_column_t> mTableInfo;
	QString mSqlTableName;

//	QPointer<HeaderAlias> mHeadIdxs;
	HeaderAlias *mHeadIdxs;
//	QMap<QString , int> headIdxs;

};

#endif
