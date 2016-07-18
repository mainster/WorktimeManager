#ifndef CONF_H
#define CONF_H

#include <QWidget>
#include <QtCore>
#include <QObject>
#include <QDebug>
#include <typeinfo>
#include <QSettings>

#include "browser.h"
#include "globals.h"

class Conf : public QWidget {
   Q_OBJECT
public:
   explicit Conf(QObject *parent = 0);
   int saveWindowGeo(QObject *obj) {
      QString objNam;
      QSETTINGS;
      objNam = obj->objectName();


      qDebug() << typeid(obj).name();
//      config.setValue( ARG_GEO.arg(objNam), obj->saveGeometry());
//      config.setValue( ARG_STA.arg(objNam), saveState());
      return 0;
   }

   int saveWindowGeo(QVariant var) {
      QSETTINGS;

      if ( var.canConvert<Browser>() ) {
			browser = qvariant_cast<Browser*>(var);
			config.setValue( ARG_GEO.arg("Browser"), browser->saveGeometry() );
      }
      else return -1;

//      qDebug() << typeid(obj).name();
//      config.setValue( ARG_GEO.arg(objNam), obj->saveGeometry());
//      config.setValue( ARG_STA.arg(objNam), saveState());
      return 0;
   }

   int restoreWindowGeo(QObject *obj) {
      QString objNam;
      QSETTINGS;
      objNam = obj->objectName();

      qDebug() << typeid(obj).name();

//      restoreGeometry(config.value( ARG_GEO.arg(objNam), " ").toByteArray() );
//      restoreState(config.value(    ARG_STA.arg(objNam), " ").toByteArray() );
      return 0;
   }

private:
   Browser *browser;
   static /*const*/ QString ARG_GEO;//  = "%1/Geometry";
   static /*const*/ QString ARG_STA;// = "%1/State";
};

#endif // CONF_H
