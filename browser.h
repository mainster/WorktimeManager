/** \file
 * [] browser.h
 *
 * @author mainster
 */

#ifndef BROWSER_H
#define BROWSER_H

#define QFOLDINGSTART {

#include <QObject>
#include <QtGui>
#include <QtCore>
#include <QTableWidget>
#include <QTableView>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QDockWidget>
#include <QGroupBox>
#include <QFrame>
#include <QWidget>

#include "ui_browser.h"
#include "globals.h"
#include "tabview.h"
#include "QSortFilterSqlQueryModel.h"
#include "mysortfilterproxymodel.h"
#include "sortwindow.h"

#define QFOLDINGEND }

namespace Ui {
class Browser;
}

class TabView;

/*!
 * \class [class name]
 *
 * \brief [brief description]
 *
 * [detailed description]
 *
 * \author [your name]
 * \date
 */
class Browser : public QWidget, private Ui::Browser {
   Q_OBJECT
public:
   explicit Browser(QWidget *parent = 0);
   static Browser *getInstance(QWidget *parent = 0x00) {
      if(inst == 0)
         inst = new Browser(parent);
      return inst;
   }
   static Browser *getObjectPtr() {
      return inst;
   }
   /*virtual*/ ~Browser();

   QSqlError
   addConnection(const QString &driver, const QString &dbName,
                 const QString &host, const QString &user,
                 const QString &passwd, int port = -1);

   void insertRow();
   void deleteRow();
   void updateActions();
   void resetStyleSheet(TabView *tv) {
      tv->setStyleSheet(styleSheet());
      style()->unpolish(tv);
      style()->polish(tv);
   }
   void clearStyleSheet(TabView *tv) {
      tv->style()->unpolish(tv);
   }
   QVector<TabView *> tvs;

   /**
    * Class accessors
    */
   SortWindow *getSortwindow() const;

signals:
   void stateMsg(const QString &message);
   void tabViewSelChanged(TabView *sel);

public slots:
   void showMetaData(const QString &table);
   void addConnection();
   void currentChanged() { updateActions(); }
   void on_insertRowAction_triggered()
   { insertRow(); }
   void on_deleteRowAction_triggered()
   { deleteRow(); }
   void on_fieldStrategyAction_triggered();
   void on_rowStrategyAction_triggered();
   void on_manualStrategyAction_triggered();
   void on_submitAction_triggered();
   void on_revertAction_triggered();
   void on_selectAction_triggered();
   void on_connectionWidget_tableActivated(const QString &sqlTab);
   void on_connectionWidget_metaDataRequested(const QString &table)
   { showMetaData(table); }
   void onCyclic();
   void customMenuRequested(QPoint pos);
   void initTableView(QWidget *parent, QStringList &accNam);
   QFont selectFont();
   int setFontAcc(QFont &font);
   void onCyclicObjInfoTrig(bool b)
   { cyclicObjInfo = b; }
   void showEvent(QShowEvent *e);
   void exec();
   void requeryWorktimeTableView(QString nonDefaulQuery = "");
   /** Show table view methodes */
   void showRelatTable(const QString &sqlTbl, TabView *tvc);
   void showRelatTable(const QString &t, QTableView *tv);
   void showRelatTable2(const QString &sqlTbl, QTableView *tv);
   QTableView *createView(QSqlQueryModel *model, const QString &title);
   void initializeMdl(QSqlQueryModel *model);
   QSqlDatabase getCurrentDatabase() {
      return connectionWidget->currentDatabase();
   }
   void SORTIT();
   void autofitRowCol();
   void onActFilterWindowTable(bool b);

protected slots:
   bool eventFilter(QObject *obj, QEvent *e);

private slots:
   QAbstractItemModel *createMailModel(QObject *parent);
   void addMail(QAbstractItemModel *model, const QString &subject, const QString &sender, const QDateTime &date);

//private slots:
//    void textFilterChanged();
//    void dateFilterChanged();

private:
   static Browser *inst;
   Ui::Browser *ui;
   QTimer *timCyc;
   bool cyclicObjInfo;
   /**
    * This pointer gots manipulated by the double click event handler.
    */
   TabView * dblClickFocusedTv;

   MySortFilterProxyModel *proxyModel;
   SortWindow *sortwindow;
};


/*!
 \brief Class declaration of an relational custom table model used as delegate.

 \class CustomRelatMdl browser.h "browser.h"
*/
class CustomRelatMdl: public QSqlRelationalTableModel {

   Q_OBJECT

public:
   /** 
    * Hold a list of columns which shold be center aligned 
    */
   QVector<int> getCCol() const;
   void setCCol(const QVector<int> &value);

   /*!
    \brief Constructor of custom relational table model inherited from
    QSqlRelationalTableModel. Subclass only a table model and override some
    base methods to prevent subclassing a complete delegate.

    \param parent
    \param db
   */
   explicit CustomRelatMdl(QObject *parent = 0,
                           QSqlDatabase db = QSqlDatabase())
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
   QVector<int> cCol;

};


/*!
 \brief An alternative to subclussing QItemDelegate is to subclass the model
 and override data() method.

 \class CustomMdl browser.h "browser.h"
*/
class CustomMdl: public QSqlTableModel{
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
   explicit CustomMdl (QObject *parent = 0,
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



#endif

