#include "globals.h"
#include "locals.h"


QFileInfo Locals::SQL_DATABASE = QFileInfo("/var/lib/mysql/delbassoSQL.db");
QString Locals::SQL_DRIVER = QString("QSQLITE");

Locals::Locals()
{

}
