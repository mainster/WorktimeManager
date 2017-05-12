#ifndef MDTABLEINFO_H
#define MDTABLEINFO_H

#include <QObject>
#include <QWidget>
#include <QTableView>
#include <QSql>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlRecord>

class MdTableInfo : public QTableView {

	Q_OBJECT

public:
	/*!
	 * \brief The TableInfo_column_t struct
	 */
	static struct TableInfo_column_t {
		int cid;
		QString name;
		QString type;
		bool notnull;
		QVariant defaultVal;
		int pk;
	} TableInfo_column;


	explicit MdTableInfo(QWidget *parent) : QTableView(parent) { }
	~MdTableInfo() {}

	static QList<TableInfo_column_t> queryInfo(const QString &tableName,
															 QSqlDatabase db) {
		static QList<TableInfo_column_t> tableInfo;
		tableInfo.clear();

		QSqlQuery query(tr("PRAGMA table_info('%1')").arg(tableName), db);

		while (query.next()) {
			TableInfo_column_t tableColumn;
			tableColumn.cid			= query.value(query.record().indexOf("cid")).toInt();
			tableColumn.name			= query.value(query.record().indexOf("name")).toString();
			tableColumn.type			= query.value(query.record().indexOf("type")).toString();
			tableColumn.notnull		= query.value(query.record().indexOf("notnull")).toBool();
			tableColumn.defaultVal = query.value(query.record().indexOf("dflt_value"));
			tableColumn.pk			= query.value(query.record().indexOf("pk")).toInt();
			tableInfo << tableColumn;
		}
		query.finish();
		return tableInfo;
	}

protected:

private:

};

#endif // MDTABLEINFO_H
