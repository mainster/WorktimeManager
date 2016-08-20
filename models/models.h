#ifndef MODELS_H
#define MODELS_H

#include <QtCore>
#include <QtWidgets>
#include <QSqlRelationalTableModel>

#include "globals.h"
#include "types.h"

/*!
 \brief Class declaration of an relational custom table model used as delegate.
 \class SqlRtm browser.h "browser.h"
*/
class SqlRtm: public QSqlRelationalTableModel {

	Q_OBJECT

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
	explicit SqlRtm(MdlSrc modelSource = srcNew,
						 QObject *parent = 0,
						 QSqlDatabase db = QSqlDatabase())
		: QSqlRelationalTableModel(parent, db) {

		Q_UNUSED(modelSource);
		//		if (modelSorce == srcRestoreIni) {
		//			QSETTINGS;
		//			QVector<int> ccs = config.value(objectName())
		//			QList<bool> vcs;

		//			mData = new mData_t();
		//			INFO << mData.mCenterCols->
		//			config.value()

		//		}
	}

	/*!
	 * \brief getCenterCols returns a list of columns which shold be center aligned
	 * \return
	 */
	QList<int> centerCols() const	{ return mCenterCols; }
	void setCenterCols(const QList<int> &value)  { mCenterCols = value; }

	/*!
	 * \brief data overrides the data() methode of the base class QSqlTableModel.
	 * By implementing more data roles (e.g. Qt::TextAlignmentRole) which the model can
	 * handle, this subclass becomes more flexible.
	 */
	QVariant data(const QModelIndex& idx, int role) const override {

		switch (role) {
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

	QList<bool> visibleCols() const { return mVisibleCols; }
	void setVisibleCols(const QList<bool> &visibleCols) { mVisibleCols = visibleCols; }

	void storeModel(const QString &sqlTableName) {
		QSETTINGS;
		/*!	Store model sources	*/
		config.setValue(sqlTableName + tr("/mCenterCols"),QVariant::fromValue(mCenterCols));
		config.setValue(sqlTableName + tr("/mVisibleCols"),QVariant::fromValue(mVisibleCols));
	}
	void restoreModel(const QString &sqlTableName) {
		QSETTINGS;
		/*!	Store model sources	*/
		mCenterCols = config.value(sqlTableName + tr("/mCenterCols")).value<QList<int> >();
		mVisibleCols = config.value(sqlTableName + tr("/mVisibleCols")).value<QList<bool> >();
	}

private:
	QList<int>	mCenterCols;
	QList<bool>		mVisibleCols;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(SqlRtm::MdlSrcs)
Q_DECLARE_METATYPE(SqlRtm::MdlSrc)

//< Deprecated
//< Deprecated
//< Deprecated
/*!
 \brief An alternative to subclussing QItemDelegate is to subclass the model
 and override data() method.

 \class CustomMdl browser.h "browser.h"
*/
class SqlTm: public QSqlTableModel{
	Q_OBJECT

public:

	QVector<int> getCCol() const;
	void setCCol(const QVector<int> &value);

	/*!
	 \brief Constructor of custom table model inherited from QSqlTableModel.
	 Subclassing only a table model and override some base methods to prevent
	 subclassing a complete delegate.

	 \param parent
	 \param db
	*/
	explicit SqlTm (QObject *parent = 0,
						 QSqlDatabase db = QSqlDatabase())
		: QSqlTableModel(parent, db) { }

	/*!
	\brief This methode overrides data() methode from its base class
	QSqlTableModel. Text alignment could be controlled by this extended methode.

	 \param idx
	 \param role
	 \return QVariant
	*/
	QVariant data(const QModelIndex& idx, int role) const Q_DECL_OVERRIDE {

		Qt::ItemDataRole idr = static_cast<Qt::ItemDataRole>(role);

		if ( idr == Qt::TextAlignmentRole ) {
			return QVariant( Qt::AlignCenter | Qt::AlignVCenter );
		}

		/**
		 * The extension is the ability to return a QBrush object rather then
		 * pure data handeling.
		 */
		if (idr == Qt::BackgroundRole && isDirty(idx)) {
			return QBrush(QColor(Qt::yellow));
		}

		return QSqlTableModel::data(idx, role);
	}

private:
	/**
	 * Hold a list of columns which shold be center aligned
	 */
	QVector<int> cCol;

};



#endif // MODELS_H
