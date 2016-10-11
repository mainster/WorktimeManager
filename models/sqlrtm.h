#ifndef SQLRTM_H
#define SQLRTM_H

#include <QtWidgets>
#include <QObject>
#include <QWidget>
#include <QSqlRelationalTableModel>
#include <QAbstractItemModel>
#include <QAbstractTableModel>
#include <QtCore>
#include <QMessageBox>

#include "debug.h"
#include "globals.h"
#include "types.h"


/*!
 \brief Class declaration of an relational custom table model used as delegate.
 \class SqlRtm browser.h "browser.h"
*/
/* ======================================================================== */
/*                        SQL relational table model                        */
/* ======================================================================== */
class SqlRtm: public QSqlRelationalTableModel {

	Q_OBJECT

	Q_PROPERTY(QList<bool> visibleCols READ visibleCols WRITE setVisibleCols NOTIFY visibleColsChanged)
	Q_PROPERTY(QList<int> sectionIdxs READ sectionIdxs WRITE setSectionIdxs NOTIFY sectionIdxsChanged)
	Q_PROPERTY(QList<int> centerCols READ centerCols WRITE setCenterCols NOTIFY centerColsChanged)

public:
	enum MdlSrc {
		srcNew = 0x01,
		srcRestoreIni = 0x02,
	};
	Q_DECLARE_FLAGS(MdlSrcs, MdlSrc)
	Q_FLAG(MdlSrcs)

	/*!
	 * \brief SqlRtm Constructor of custom relational table model inherited from
	 * QSqlRelationalTableModel. Subclass only a table model and override some basemethods
	 * to prevent subclassing a complete delegate.
	 * \param parent
	 * \param db
	 */
	explicit SqlRtm(QSqlDatabase db = QSqlDatabase(), QObject *parent = 0)
		: QSqlRelationalTableModel(parent, db) {

		connect (this, &SqlRtm::srcChanged, this, &SqlRtm::onSrcChanged);
	}

	/*!
	 * \brief data overrides the data() methode of the base class QSqlTableModel.
	 * By implementing more data roles (e.g. Qt::TextAlignmentRole) which the model can
	 * handle, this subclass becomes more flexible.
	 */
	QVariant data(const QModelIndex& idx, int role) const override {
		switch (role) {
#ifdef WITH_NONE_ENTRY
			case Qt::DisplayRole: {
				if (idx.row() == 0) {
					  // if we are at the desired column return the None item
					  if (idx.data().isNull())
							 return QVariant("None");
					  // otherwise a non valid QVariant
					  else
							 return QVariant();
				}
				// Return the parent's data
				else
					 return SqlRtm::data(createIndex(idx.row() - 1, idx.column()), role);
			}; break;
#endif
			case Qt::TextAlignmentRole: {
				return QVariant(Qt::AlignCenter | Qt::AlignVCenter);
			}; break;

			case Qt::BackgroundRole: {
				if (isDirty(idx))
					return QBrush(QColor(Qt::yellow));
			}; break;

			default: break;
		}
		return QSqlTableModel::data(idx, role);
	}
	virtual QVariant headerData(int section, Qt::Orientation orientation = Qt::Horizontal,
										 int role = Qt::DisplayRole) const {
		return QSqlRelationalTableModel::headerData(section, orientation, role);
	}
	virtual void setHeaderData(int section, const QVariant &value,
										Qt::Orientation orientation = Qt::Horizontal,
										int role = Qt::DisplayRole) {
		QSqlRelationalTableModel::setHeaderData(section, orientation, value, role);
	}

	void storeModel(const QString &sqlTableName) {
		QSETTINGS;
		/*!	Store model sources	*/
		config.setValue(sqlTableName + Md::k.centerCols, QVariant::fromValue(mCenterCols));
		config.setValue(sqlTableName + Md::k.visibleCols,QVariant::fromValue(mVisibleCols));
		config.setValue(sqlTableName + Md::k.sectionIdxs,QVariant::fromValue(mSectionIdxs));
	}
	void restoreModelSrcsFromIni(const QString &sqlTableName) {
		QSETTINGS;
		/*!	Store model sources	*/
		mCenterCols = config.value(sqlTableName + Md::k.centerCols).value<QList<int> >();
		mVisibleCols = config.value(sqlTableName + Md::k.visibleCols).value<QList<bool> >();
		mSectionIdxs = config.value(sqlTableName + Md::k.sectionIdxs).value<QList<int> >();
		emit srcChanged();
	}

	/*!
	 * \brief getCenterCols returns a list of columns which shold be center aligned
	 * \return
	 */
	QList<int> centerCols() const	{ return mCenterCols; }
	void setCenterCols(const QList<int> &value)  {
		mCenterCols = value;
		emit centerColsChanged(mCenterCols);
	}

	QList<bool> visibleCols() const { return mVisibleCols; }
	void setVisibleCols(const QList<bool> &visibleCols) {
		mVisibleCols = visibleCols;
		emit visibleColsChanged(mVisibleCols);
	}

	QList<int> /***/sectionIdxs()  { return /*&*/mSectionIdxs; }
	void setSectionIdxs(const QList<int> &sectionIdxs) {
		mSectionIdxs = sectionIdxs;
		emit sectionIdxsChanged(mSectionIdxs);
	}
	void setSectionIdx(const int logicalIdx, const int visualIdx) {
		/*!
		 * Init if sectionIdxs list is empty.
		 */
		if (mSectionIdxs.count() < columnCount()) {
			mSectionIdxs.clear();

			for (int k = 0; k < columnCount(); k++)
				mSectionIdxs << k;
		}

		mSectionIdxs[logicalIdx] = visualIdx;
		emit sectionIdxsChanged(mSectionIdxs);
	}

	void onSrcChanged() {
		INFO << tr("srcChanged() signal emitted");
		setVisibleCols(mVisibleCols);
		for (int k = 0; k< mSectionIdxs.length(); k++)
			setSectionIdx(k, mSectionIdxs.at(k));
	}
	void resetModelSrcs() {
		mCenterCols.clear();
		mVisibleCols.clear();
		mSectionIdxs.clear();
	}
	void submitAll() {
		QSqlRelationalTableModel::submitAll();
		emit recordChanged();
	}

signals:
	void srcChanged();
	void centerColsChanged(QList<int> centerCols);
	void visibleColsChanged(QList<bool> visibleCols);
	void sectionIdxsChanged(QList<int> sectionIdxs);
	void recordChanged();

protected slots:

private:
	QList<int>	mCenterCols;
	QList<bool>	mVisibleCols;
	QList<int>	mSectionIdxs;

};
Q_DECLARE_OPERATORS_FOR_FLAGS(SqlRtm::MdlSrcs)
Q_DECLARE_METATYPE(SqlRtm::MdlSrc)



#endif // SQLRTM_H
