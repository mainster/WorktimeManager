#include "globals.h"
#include "locals.h"


struct Md::confKeys_t Md::keys {
	QString("/TabOrder"),
	QString("/TabOrder"),
};


QFileInfo Locals::SQL_DATABASE = QFileInfo("/var/lib/mysql/delbassoSQL.db");
QString Locals::SQL_DRIVER = QString("QSQLITE");

Locals::Locals()
{

}
