#include "globals.h"

QString Globals::STR_NOTIFY = "notify-send --app-name=\"@APP@\" -t @MSG_TIME@ \"@MSG_HEAD@\" \"@MSG_BODY@";
Globals * Globals::inst = 0x00;

//QString Globals::SQL_DB_PATH     = "/var/lib/mysql/";
QString Globals::SQL_DB_PATH     = "/home/mainster/CODES_local/mysql/";
QString Globals::SQL_DB_FILE     = "delbassoSQL.db";

QStringList Globals::strLst      = QStringList();

QString Globals::ddgbStyleShtA = "QGroupBox {"
      "background-color: qlineargradient("
      "x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E0E0E0, stop: 1 #FFFFFF);"
      "border: 2px solid gray; border-radius: 5px;"
      "margin-top: 4ex;"          /* leave space at the top for the title */
      "}"

      "QGroupBox::title {"
      "subcontrol-origin: margin;"
      "subcontrol-position: top center; "  /* position at the top center */
      "padding: 0 3px;"
      "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
      "stop: 0 #FFOECE, stop: 1 #FFFFFF);}";

QString Globals::ddgbCheckableStyleSht =
      " QGroupBox::indicator {"
      "    width: 13px;"
      "    height: 13px;"
      "}"
      "QGroupBox::indicator:unchecked {"
      "    image: url(://iconClose.png);"
      "}"
      "QGroupBox {"
      "background-color: qlineargradient("
      "x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E0E0E0, stop: 1 #FFFFFF);"
      "border: 2px solid gray; border-radius: 5px;"
      "margin-top: 4ex;"          /* leave space at the top for the title */
      "}"

      "QGroupBox::title {"
      "subcontrol-origin: margin;"
      "subcontrol-position: top center; "  /* position at the top center */
      "padding: 0 3px;"
      "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
      "stop: 0 #FFOECE, stop: 1 #FFFFFF);}";

QString Globals::ddgbStyleShtInpFrm =
      " QGroupBox {"
      "   background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
      "   stop: 0 #E0E0E0, stop: 1 #FFFFFF);"
      "   border: 2px solid red;"
      "   border-radius: 5px;"
      "   margin-top: 3ex; /* leave space at the top for the title */"
      " }"
      ""
      " QGroupBox::title {"
      "   subcontrol-origin: boarder; /* margin boarder padding content */"
      "   subcontrol-position: top center; /* position at the top center */"
      "   padding: 0 3px"
      " }";

QString Globals::ddtvStyleShtA =
      " QTableView{"
      "  background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E0E0E0, stop: 1 #FFFFFF);"
      "  border: 0px solid gray;"
      "  border-radius: 5px;"
      "  margin-top: 10px; /* leave space at the top for the title */"
      "}";

QString Globals::ddgbStyleShtCenterHead =
     " QGroupBox {"
     "      background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E0E0E0, stop: 1 #FFFFFF);"
     "      border: 2px solid gray;"
     "      border-radius: 5px;"
     "   margin-top: 1ex; /* leave space at the top for the title */"
     "      font: italic 9pt \"Arial\";"
     "   font-weight: bold;"
     "   color: black;"
     " }"
     ""
     " QGroupBox::title {"
     "   subcontrol-origin: margin; /* margin boarder padding content */"
     "   subcontrol-position: top center; /* position at the top center */"
     "   top: 3px;   "
     "   padding: 0px 6px"
     " }";

QString Globals::gbStyleShtCenterPROPERTYS =
      "QGroupBox#tva[selected=false] { border: 2px solid gray;  }"
      "QGroupBox#tva[selected=true] { border: 1px dashed rgb(55, 55, 55);   }"
      ""
      "QGroupBox#tvb[selected=false] { border: 2px solid gray;  }"
      "QGroupBox#tvb[selected=true] { border: 1px dashed rgb(55, 55, 55);   }"
      ""
      "QGroupBox#tvc[selected=false] { border: 2px solid gray;  }"
      "QGroupBox#tvc[selected=true] { border: 1px dashed rgb(55, 55, 55);   }"
      ""
      "QGroupBox#tvl1[selected=false] { border: 2px solid gray;  }"
      "QGroupBox#tvl1[selected=true] { border: 1px dashed rgb(55, 55, 55);   }"
      ""
      " QGroupBox {"
      "   background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E0E0E0, stop: 1 #FFFFFF);"
      "   border-radius: 5px;"
      "   margin-top: 1ex; /* leave space at the top for the title */"
      "   font: italic 9pt \"Arial\";"
      "   font-weight: bold;"
      "   color: black;"
      "}"
      ""
      "QGroupBox::title {"
      "   subcontrol-origin: margin; /* margin boarder padding content */"
      "   subcontrol-position: top center; /* position at the top center */"
      "   top: 3px;   "
      "   padding: 0px 6px"
      "} ";



QStringList Globals::widToStrLst(QWidgetList &lst) {

   strLst.clear();
   foreach (QWidget *w, lst) {
      strLst << w->objectName();
   }
   return strLst;
}
QString Globals::widToStr(QWidgetList &lst, QString sep) {

   strLst.clear();
   foreach (QWidget *w, lst) {
      strLst << w->objectName();
   }
   return strLst.join(sep);
}
QStringList Globals::objToStrLst(QObjectList &lst) {

   strLst.clear();
   foreach (QObject *o, lst) {
      strLst << o->objectName();
   }
   return strLst;
}
QString Globals::objToStr(QObjectList &lst, QString sep) {

   strLst.clear();
   foreach (QObject *o, lst) {
      strLst << o->objectName();
   }
   return strLst.join(sep);
}
Globals::Globals() {
}
