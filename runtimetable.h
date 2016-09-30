#ifndef RUNTIMETABLE_H
#define RUNTIMETABLE_H

#include <QObject>
#include <QSql>
#include <QSqlQuery>
#include "globals.h"
#include "debug.h"

class RuntimeTable : public QObject {

	Q_OBJECT

public:
	explicit RuntimeTable(/*QObject *parent = 0*/)  { }
	~RuntimeTable() {}

	bool recalcOvertime() {
		INFO << dropTable();
		return true;
	}
	bool dropTable() {
	   return true;
		QSqlQuery query(QString("DROP TABLE runtime;"));
		return query.exec();
	}

};

#endif // RUNTIMETABLE_H
