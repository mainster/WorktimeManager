#include "editablesqlmdl.h"

#include <QtSql>

EditableSqlMdl::EditableSqlMdl(QObject *parent)
    : QSqlQueryModel(parent) {
}

Qt::ItemFlags EditableSqlMdl::flags(
      const QModelIndex &index) const {

   Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    if (index.column() == 1 || index.column() == 2)
        flags |= Qt::ItemIsEditable;
    return flags;
}

bool EditableSqlMdl::setData(const QModelIndex &index,
                             const QVariant &value, int /* role */) {
    if (index.column() < 1 || index.column() > 2)
        return false;

    QModelIndex primaryKeyIndex = QSqlQueryModel::index(index.row(), 0);
    int id = data(primaryKeyIndex).toInt();

    clear();

    bool ok;
    if (index.column() == 1) {
        ok = setFirstName(id, value.toString());
    } else {
        ok = setLastName(id, value.toString());
    }
    refresh();
    return ok;
}

void EditableSqlMdl::refresh() {
    setQuery("select * from person");
    setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    setHeaderData(1, Qt::Horizontal, QObject::tr("First name"));
    setHeaderData(2, Qt::Horizontal, QObject::tr("Last name"));
}

bool EditableSqlMdl::setFirstName(int personId, const QString &firstName) {
    QSqlQuery query;
    query.prepare("update person set firstname = ? where id = ?");
    query.addBindValue(firstName);
    query.addBindValue(personId);
    return query.exec();
}

bool EditableSqlMdl::setLastName(int personId, const QString &lastName) {
    QSqlQuery query;
    query.prepare("update person set lastname = ? where id = ?");
    query.addBindValue(lastName);
    query.addBindValue(personId);
    return query.exec();
}
