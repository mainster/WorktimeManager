#ifndef EDITABLESQLMDL_H
#define EDITABLESQLMDL_H

#include <QSqlQueryModel>

class EditableSqlMdl : public QSqlQueryModel {
	Q_OBJECT

public:
	explicit EditableSqlMdl(QObject *parent = 0);
	~EditableSqlMdl() { QSqlQueryModel::clear(); }
	Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
	bool setData(const QModelIndex &index,
					 const QVariant &value, int role) Q_DECL_OVERRIDE;

private:
	bool setFirstName(int personId, const QString &firstName);
	bool setLastName(int personId, const QString &lastName);
	void refresh();
};

#endif // EDITABLESQLMDL_H
