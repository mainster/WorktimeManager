#ifndef SQLRELDEL_H
#define SQLRELDEL_H

#include <QObject>
#include <QSqlRelationalDelegate>
#include <QSqlRelationalTableModel>


class SqlRelDel : public QSqlRelationalDelegate {

	Q_OBJECT

public:
	explicit SqlRelDel(QObject *parent = 0);


signals:

public slots:

};


#endif // SQLRELDEL_H
