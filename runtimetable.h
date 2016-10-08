#ifndef RUNTIMETABLE_H
#define RUNTIMETABLE_H

#include <QObject>
#include <QSql>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlQueryModel>
#include <QRegExp>
#include <QTableView>
#include <QList>
#include <QTableView>

#include "globals.h"
#include "debug.h"
#include "mdtabview.h"
#include "mysortfilterproxymodel.h"
#include "mdtableinfo.h"
#include "models.h"

class DbController;

/* ======================================================================== */
/*                               RuntimeTable                               */
/* ======================================================================== */
class RuntimeTable : public QObject {

	Q_OBJECT

public:
	explicit RuntimeTable(MdTabView *worktimeTv, QObject *parent)
		: QObject(parent), m_worktimeTv(worktimeTv) {
		INFO << m_worktimeTv << this;
	}
	~RuntimeTable() { }

	bool dropTable() {
		QSqlQuery query(QString("DROP TABLE runtime;"));
		return query.exec();
	}
	void setRuntimeTable(MdTabView *runtimeTable) { m_runtimeTv = runtimeTable; }
	bool recalcOvertime();

private:
	MdTabView *m_worktimeTv, *m_runtimeTv;
	QList<QString> m_workers;
	QMap<int, SfiltMdl *> workerProxyMdls;
	QList<MdTableInfo::TableInfo_column_t> worktimeColumnInfos;
	QList<MdTableInfo::TableInfo_column_t> runtimeColumnInfos;


	static const QString OVERTIME_TABLE_NAME;
};

#endif // RUNTIMETABLE_H
