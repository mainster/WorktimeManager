#ifndef RUNTIMETABLE_H
#define RUNTIMETABLE_H

#include <QObject>
#include <QtSql>
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
#include "mdtableinfo.h"
#include "mdtable.h"
#include "sfiltmdl.h"

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

	/*!
	 * The key represents the worker ID from current sql query records.
	 */
//	QMap<int, SfiltMdl *> workerProxyMdls;

	typedef struct {
		qreal weekly, monthly, yearly, inRange;
	} WorktimeSum_t;

	/*!
	 * The index must match the current worker ID.
	 */
	QMap<int, WorktimeSum_t> worktimeSums;

	QList<MdTableInfo::Column_t> worktimeColumnInfos;
	QList<MdTableInfo::Column_t> runtimeColumnInfos;


	static const QString OVERTIME_TABLE_NAME;
	QNetworkReply *m_currentReply;
};

#endif // RUNTIMETABLE_H
