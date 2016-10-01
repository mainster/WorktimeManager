#ifndef RUNTIMETABLE_H
#define RUNTIMETABLE_H

#include <QObject>
#include <QSql>
#include <QSqlQuery>
#include <QSqlRecord>
#include "globals.h"
#include "debug.h"

class RuntimeTable : public QObject {

	Q_OBJECT

public:
	explicit RuntimeTable(/*QObject *parent = 0*/)  { }
	~RuntimeTable() {}

	bool recalcOvertime() {

		QSqlQueryModel model;
		model.setQuery("SELECT * FROM worker");

		for (int i = 0; i < model.rowCount(); ++i) {
			 INFO << model.record(i).value("workerID").toInt()
					<< model.record(i).value("Vorname").toString();
		}

		return true;
	}
	bool dropTable() {
		QSqlQuery query(QString("DROP TABLE runtime;"));
		return query.exec();
	}
};

#endif // RUNTIMETABLE_H
