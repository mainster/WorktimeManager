#ifndef QSORTFILTERSQLQUERYMODEL_H
#define QSORTFILTERSQLQUERYMODEL_H

#include <QSqlQueryModel>
#include <QSqlRecord>

class QSortFilterSqlQueryModel : public QSqlQueryModel
{
   Q_OBJECT

public:

   explicit QSortFilterSqlQueryModel(QObject *parent = 0);

   //void setQuery(const QSqlQuery &query);
   void setQuery(const QString & query, const QSqlDatabase & db = QSqlDatabase::database() );

public slots:

   void setFilterColumn (const QString & column);
   void setFilterFlags (const Qt::MatchFlag flags);
   void setFilter( const QString & filter );
   void filter( const QString & filter );

   void select();

   virtual void setSort (int column, Qt::SortOrder order);
   virtual void sort (int column, Qt::SortOrder order);

private:

   QSqlDatabase queryDB;
   QString queryClause;
   Qt::MatchFlag filterFlags;
   QString filterString;
   QString filterColumn;
   int sortKeyColumn;
   Qt::SortOrder sortOrder;

signals:

};

#endif // QSORTFILTERSQLQUERYMODEL_H
