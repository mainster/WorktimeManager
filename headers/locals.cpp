#include "globals.h"
#include "locals.h"


struct Md::confKeys_t Md::k {
	QString("/TabOrder"),
	QString("/AlternateRowColEnabled"),
	QString("/AlternateRowColor"),
	QString("/TableFont"),
	QString("/mCenterCols"),
	QString("/mVisibleCols"),
	QString("/mSectionIdxs")
};


QFileInfo Locals::SQL_DATABASE = QFileInfo("/var/lib/mysql/delbassoSQL.db");
QString Locals::SQL_DRIVER = QString("QSQLITE");

Locals::Locals()
{

}
