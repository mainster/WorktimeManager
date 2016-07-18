#include "sqlreldel.h"
#include <QSqlRelationalTableModel>
#include <QSortFilterProxyModel>

#include <QDebug>
#include <QSqlRecord>

SqlRelDel::SqlRelDel(QObject *parent) : QSqlRelationalDelegate(parent) {}

