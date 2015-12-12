#include "conf.h"
#include "browser.h"

#include <QtCore>
#include <QObject>
#include <QDebug>
#include <typeinfo>


class Browser;

QString Conf::ARG_GEO  = "%1/Geometry";
QString Conf::ARG_STA = "%1/State";
//Browser Conf::browser = 0;


Conf::Conf(QObject *parent) /*:*/
  /* QWidget(parent)*/ {
}
