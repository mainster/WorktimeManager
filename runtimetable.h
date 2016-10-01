#ifndef RUNTIMETABLE_H
#define RUNTIMETABLE_H

#include <QObject>
#include <QSql>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QRegExp>
#include <QTableView>
#include <QList>
#include <QTableView>

#include "globals.h"
#include "debug.h"
#include "mdtabview.h"
#include "mysortfilterproxymodel.h"

//#include "browser.h"

class RuntimeTable : public QObject {

	Q_OBJECT

public:
	explicit RuntimeTable(QTableView *worktimeTv, QObject *parent)
		: QObject(parent), m_worktimeTv(worktimeTv) {
		INFO << m_worktimeTv << this;
	}
	~RuntimeTable() { }

	bool dropTable() {
		QSqlQuery query(QString("DROP TABLE runtime;"));
		return query.exec();
	}
	void setRuntimeTable(QTableView *runtimeTable) { m_runtimeTv = runtimeTable; }
	bool recalcOvertime();

private:
	QTableView *m_worktimeTv, *m_runtimeTv;
	QList<QString> m_workers;
	QMap<int, SfiltMdl *> workerProxyMdls;
	QList<MdTableInfo::TableInfo_column_t> columnInfos;


	static const QString OVERTIME_TABLE_NAME;
};

#endif // RUNTIMETABLE_H
