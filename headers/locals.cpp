#include "globals.h"
#include "locals.h"


struct Md::confKeys_t Md::k {
	QString("/TabOrder"),
	QString("/AlternateRowColEnabled"),
	QString("/AlternateRowColor"),
	QString("/TableFont"),
	QString("/mCenterCols"),
	QString("/mVisibleCols"),
	QString("/mSectionIdxs"),
	QString("/WindowGeometry"),
	QString("/WindowState"),
	QString("/AllTabableWidgets")
};
QMap<QString, QString> Md::tableAlias {
	{ "client",		"Kunden" },
	{ "prj",			"Projekte" },
	{ "fehlzeit",	"Fehlzeiten" },
	{ "worker",		"Mitarbeiter" },
	{ "arch",		"Architekt" },
	{ "worktime",	"Arbeitszeit" },
	{ "worker",		"Mitarbeiter" }
};


QFileInfo Locals::SQL_DATABASE = QFileInfo("/var/lib/mysql/delbassoSQL.db");
QString Locals::SQL_DRIVER = QString("QSQLITE");
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

const QString Locals::MdTableStyleSheet = QString(
			"QGroupBox::title {"
			"   	subcontrol-origin: margin; /* margin boarder padding content */"
			"   	subcontrol-position: top center; /* position at the top center */"
			"   	top: 1.0ex;   "
			"  	padding: 0px 8px;"
			"	color:  rgba(50,50,50,255);"
			"} [select=false] {"
			"	color:  rgba(245,0,0,255);"
			"} "
			"QGroupBox {"
			"   	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E0E0E0, stop: 1 #FFFFFF);"
			"   	border-radius: 5px;"
			"	margin-top: 1ex; /*margin-bottom: 2px; margin-left: 2px; margin-right: 2px;*/"
			"   	font: italic 9pt ""Arial"";"
			"   	font-weight: normal;"
			"	border: 2px solid;"
			"	border-top-color: qlineargradient(spread:reflect, x1:0, y1:0, x2:0.5, y2:0, stop:0.10 rgba(82, 82, 82, 255), stop:1 rgba(169, 169, 169,20));"
			"	border-left-color: rgba(105,105,105,255);"
			"	border-right-color: rgba(105,105,105,205);"
			"	border-bottom-color: rgba(105,105,105,205);"
			"} [select=true] { "
			"	/* margin-top: 5px; margin-bottom: -2px; margin-left: -2px; margin-right: -2px; */"
			"	border: 2px solid; "
			"	border-top-color: qlineargradient(spread:reflect, x1:0, y1:0, x2:0.5, y2:0, stop:0.10 rgba(255,0,0,255), stop:1 rgba(247, 247, 247, 250));"
			"	border-left-color: rgba(255,0,0,255);"
			"	border-right-color: rgba(255,0,0,255);"
			"	border-bottom-color: rgba(255,0,0,255);"
			"}"
			"QGroupBox::title:hover {"
			"    color: rgba(235, 235, 235, 255);"
			"}"
			"QTableView {"
			"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #F0F0F0, stop: 1 #FFFFFF);"
			"	border: 0px solid gray;"
			"	border-radius: 5px;"
			"	margin-top: 15px;	"
			"	gridline-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0.19 rgba(97, 97, 97, 255), stop:1 rgba(247, 247, 247, 255));"
			" } [select=true] {"
			"	background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #C0C0C0, stop: 1 #FFFFFF); "
			"}"
			"QHeaderView::section {"
			"     background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
			"                                       stop:    0 #616161, stop: 0.5 #505050,"
			"                                       stop: 0.6 #434343, stop:    1 #656565);"
			"     color: white;"
			"     padding-left: 4px;"
			"     padding-right: 4px;"
			"     padding-top: 2px;"
			"     padding-bottom: 2px;"
			"     border: 1px solid #6c6c6c;"
			"}"
			" QHeaderView::section:checked {"
			"     background-color: rgb(31, 94, 233);"
			" }"
			);
#define QFOLDINGEND }

Locals::Locals()
{

}
