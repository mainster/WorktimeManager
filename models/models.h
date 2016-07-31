#ifndef MODELS_H
#define MODELS_H

#include <QtCore>
#include <QtWidgets>
#include <QSqlRelationalTableModel>

/*!
 \brief Class declaration of an relational custom table model used as delegate.

 \class SqlRtm browser.h "browser.h"
*/
class SqlRtm: public QSqlRelationalTableModel {

	Q_OBJECT

public:
	/**
	 * Hold a list of columns which shold be center aligned
	 */
	QVector<int> getCenterCols() const {
		return mCenterCols;
	}
	void setCenterCols(const QVector<int> &value) {
		mCenterCols = value;
	}
	/*!
	 \brief Constructor of custom relational table model inherited from
	 QSqlRelationalTableModel. Subclass only a table model and override some
	 base methods to prevent subclassing a complete delegate.

	 \param parent
	 \param db
	*/
	explicit SqlRtm(QObject *parent = 0, QSqlDatabase db = QSqlDatabase())
		: QSqlRelationalTableModel(parent, db) { }
	/*!
	\brief This methode overrides data() methode from its base class
	QSqlTableModel. Text alignment could be controlled by this extended methode.

	 \param idx
	 \param role
	 \return QVariant
	*/
	QVariant data(const QModelIndex& idx, int role) const Q_DECL_OVERRIDE {
		if ( role == Qt::TextAlignmentRole )
			return QVariant( Qt::AlignCenter | Qt::AlignVCenter );
		/**
		 * The extension is the ability to return a QBrush object rather then
		 * pure data handeling.
		 */
		if (role == Qt::BackgroundRole && isDirty(idx))
			return QBrush(QColor(Qt::yellow));

		return QSqlTableModel::data(idx, role);
	}

private:
	QVector<int> mCenterCols;

};

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
