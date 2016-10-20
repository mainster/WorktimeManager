#include "globals.h"
#include "locals.h"

const QList<QString> Locals::BASE_DATA_TABLE_NAMES {
	QString("client"),
			QString("prj"),
			QString("fehlzeit"),
			QString("arch"),
			QString("sub"),
			QString("worker"),
			QString("grading")
};

struct Md::confKeys_t Md::k {
	QString("/TabOrder"),
	QString("/AlternateRowColEnabled"),
	QString("/AlternateRowColor"),
	QString("/GridLineColor"),
	QString("/TableFont"),
	QString("/mCenterCols"),
	QString("/mVisibleCols"),
	QString("/mSectionIdxs"),
	QString("/WindowGeometry"),
	QString("/WindowState"),
	QString("/AllTabableWidgets"),
	QString("/MaxHourlyWage"),
	QString("/CustomDatabaseFilePath"),
	QString("/MinDateEdit"),
	QString("/MaxDateEdit"),
	QString("/FiltCaseSensCheckBox"),
	QString("/FiltSyntaxComboBox")
};

QMap<QString, QString> Md::tableAlias {
	{ "client",		"Kunden" },
	{ "prj",			"Projekte" },
	{ "fehlzeit",	"Fehlzeiten" },
	{ "arch",		"Architekten" },
	{ "sub",			"Subunternehmer" },
	{ "worktime",	"Arbeitszeiten" },
	{ "worker",		"Mitarbeiter" },
	{ "grading",	"Einstufungen" },
	{ "runtime",	"Überstd/Vorsch" }
};

QMap<QString, QString> Md::headerAlias {
	{ "client/Nummer",	"Knd. #" },
	{ "prj/Nummer",		"Prj. #" },
	{ "Stundensatz",		"€/h" },
	{ "dat",					"Datum" },
	{ "hours",				"Std" },
	{ "prj/clientID",		"Kunde, Name" },
	{ "PersonalNr",		"PN" },
	{ "Wochenstunden",	"h/Woche" },
	{ "prj/archID",		"Architekt, Name" },
	{ "prj/subID",			"Subunt., Name" },
	{ "workerID",			"Mitarbeiter" },
	{ "fehlID",				"Grund" }
};


QFileInfo Locals::SQLITE_DB_PATH =
		QFileInfo("/home/mainster/CODES_local/qt_creator/worktimeManagerSubdirPrj/"
					 "WorktimeManager/sqlite/delbasso_SQLITE.db");

QString Locals::SQLITE_DRIVER = QString("QSQLITE");
QString Locals::MYSQL_DB_NAME = QString("delbasso_MYSQL");
QString Locals::MYSQL_DRIVER = QString("QMYSQL");

QStringList Locals::PROJECT_PATHS = QStringList()
		<< QString("/home/mainster/CODES_local/qt_creator/worktimeManagerSubdirPrj/WorktimeManager")
		<< QString("/home/mainster/CODES_local/qt_creator/WorktimeManager");

#define QFOLDINGSTART {
const QString Locals::browserStyleSheet = QString(
			"QGroupBox{"
			"   	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E0E0E0, stop: 1 #FFFFFF);"
			"   	border-radius: 5px;"
			"  	margin-top: 1ex; /* leave space at the top for the title */"
			"   	font: italic 9pt ""Arial"";"
			"   	font-weight: normal;"
			"	border: 1px solid;    "
			" 	border-color: rgb(105, 105, 105);"
			"   	color: black;"
			"}"
			""
			"QGroupBox::title {"
			"   	subcontrol-origin: margin; /* margin boarder padding content */"
			"   	subcontrol-position: top center; /* position at the top center */"
			"   	top: 1.2ex;   "
			"  	padding: 0px 8px"
			"   	font-weight: bold;"
			"}"
			""
			"QGroupBox::title:hover {"
			"    color: rgba(235, 235, 235, 255);"
			"}"
			""
			" QTableView{"
			"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #F0F0F0, stop: 1 #FFFFFF);"
			"	border: 0px solid gray;"
			"	border-radius: 5px;"
			"	margin-top: 15px; /* leave space at the top for the title */"
			" }"
			""
			"QTableView[select=false]{ background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #F0F0F0, stop: 1 #FFFFFF); }"
			"QGroupBox[select=false]{ border: 1px solid; }"
			""
			"QTableView[select=true]{ background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #C0C0C0, stop: 1 #FFFFFF); }"
			"QGroupBox[select=true]{ border: 3px solid; }"
			""
			"/* Customize Table header */"
			" QHeaderView::section {"
			"     background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
			"                                       stop:    0 #616161, stop: 0.5 #505050,"
			"                                       stop: 0.6 #434343, stop:    1 #656565);"
			"     color: white;"
			"     padding-left: 4px;"
			"     padding-right: 4px;"
			"     padding-top: 2px;"
			"     padding-bottom: 2px;"
			"     border: 1px solid #6c6c6c;"
			" }"
			""
			" QHeaderView::section:checked {"
			"     background-color: rgb(31, 94, 233);"
			" }"
			""
			" /* style the sort indicator "
			"QHeaderView::down-arrow {"
			"	image: url(:/images/down_arrow.png);"
			"}"
			""
			"QHeaderView::up-arrow {"
			"	image: url(:/images/up_arrow.png);"
			"}*/");

const QString Locals::browserStyleSheetv2 = QString(
			"QSplitter::handle {"
			"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, "
			"    stop:0 rgba(255, 255, 255, 0), "
			"    stop:0.407273 rgba(200, 200, 200, 255), "
			"    stop:0.4825 rgba(101, 104, 113, 235), "
			"    stop:0.6 rgba(255, 255, 255, 0));"
			"    image: url(:/images/splitter.png);"
			"}"
			"QTreeView {"
			"    show-decoration-selected: 1;"
			"}"
			"QTreeView::item {"
			"    border: 1px solid #d9d9d9;"
			"    border-top-color: transparent;"
			"    border-bottom-color: transparent;"
			"}"
			"QTreeView::item:hover {"
			"    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);"
			"    border: 1px solid #bfcde4;"
			"}"
			"QTreeView::item:selected {"
			"    border: 1px solid #567dbc;"
			"}"
			"QTreeView::item:selected:active{"
			"    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
			"}"
			"QTreeView::item:selected:!active {"
			"    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6b9be8, stop: 1 #577fbf);"
			"}"
			"QTreeView{"
			"   background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E0E0E0, stop: 1 #FFFFFF);"
			"   border-radius: 5px;"
			"   margin-top: 1ex; /* leave space at the top for the title */"
			"   font: italic 9pt ""Arial"";"
			"   /*font-weight: bold;*/"
			"   color: black;"
			"}"
			"QToolTip {"
			"    border: 2px solid darkkhaki;"
			"    padding: 5px;"
			"    border-radius: 3px;"
			"    opacity: 400;"
			"}"
			"QLabel{"
			"color: ;"
			"	color: rgb(70, 70, 70);"
			"}");


#define QFOLDINGEND }

Locals::Locals()
{

}
