#include "mdtableinfo.h"


QList<MdTableInfo::TableInfo_column_t> MdTableInfo::queryInfo(
		const QString &tableName, QSqlDatabase db) {
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
	return tableInfo;
}
