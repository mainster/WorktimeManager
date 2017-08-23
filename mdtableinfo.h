#ifndef MDTABLEINFO_H
#define MDTABLEINFO_H

#include <QObject>
#include <QWidget>
#include <QTableView>
#include <QtSql>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlRecord>
#include "locals.h"

class MdTableInfo : public QTableView {

	Q_OBJECT

public:
	/*!
	 * @brief The Column_t struct
	 */
	typedef struct {
		int cid;
		QString name;
		QString type;
		bool notnull;
		bool unique;
		QVariant defaultVal;
		int pk;
	} Column_t;

	explicit MdTableInfo(QWidget *parent) : QTableView(parent) { }
	~MdTableInfo() {}

	static QList<Column_t> querySchema(const QString &tableName, QSqlDatabase db) {
		QList<Column_t> tableInfo;

		QStringList columns;
		QSqlQuery q(tr("SELECT sql FROM sqlite_master where name='%1'")
						.arg(tableName), db);

		if (q.next())
			columns << q.value(0).toString().split("\n");

		q.exec(tr("PRAGMA table_info('%1')").arg(tableName));

		while (q.next()) {
			Column_t tableColumn;
			tableColumn.cid			= q.value(q.record().indexOf("cid")).toInt();
			tableColumn.name			= q.value(q.record().indexOf("name")).toString();
			tableColumn.type			= q.value(q.record().indexOf("type")).toString();
			tableColumn.notnull		= q.value(q.record().indexOf("notnull")).toBool();
			tableColumn.defaultVal	= q.value(q.record().indexOf("dflt_value"));
			tableColumn.pk				= q.value(q.record().indexOf("pk")).toInt();

			tableColumn.unique = false;
			foreach (QString s, columns) {
				if (s.contains(tableColumn.name, Qt::CaseInsensitive) &&
					 s.contains(Md::sqlConstraints.unique, Qt::CaseInsensitive)) {
					tableColumn.unique = true;
					break;
				}
			}

			tableInfo << tableColumn;
		}
		q.finish();

		return tableInfo;
	}

protected:

private:

};

#endif // MDTABLEINFO_H
