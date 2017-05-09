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
															 QSqlDatabase db);

protected:

private:

};

#endif // MDTABLEINFO_H
