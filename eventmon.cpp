#include "eventmon.h"
#include "globals.h"

EventMon::EventMon(QObject *parent) :
   QObject(parent)
{
}



/*!
 \brief Event filter to set correct focus if more than one sql table view

 \param obj
 \param e
 \return bool
*/
bool EventMon::eventFilter(QObject *obj, QEvent *e) {

   if (e->type() == QEvent::MouseButtonPress) {
      Q_INFO << "obj parent name:" << obj->parent()->objectName();
      Q_INFO << ((QStringList) TVA << TVB << TVC << TVM).join(',');

      /**
       * If event receiver object has one of the tabview accessnames...
       * ... reset all tabviews from beeing selected and mark the obj
       * obj->parent() after static casting to TabView...
       */
      if ( (((QStringList) TVA << TVB << TVC << TVM).join(','))
           .contains( obj->parent()->objectName()) ) {

         /**
          * Reset....
          */
         foreach (TabView *tv, tvs) {
            tv->m_gb->setStyleSheet(Globals::gbStyleShtCenterHead);
         }
         TabView *t = static_cast<TabView *>(obj->parent());

         t->m_gb->setStyleSheet(
                  Globals::gbStyleShtCenterHead
                  .replace("border: 2px solid gray;",
                           "border: 2px dashed rgb(55, 55, 55);"));

      }

      if (false) {
         QList<QObject *> objs = obj->children();

         for (int i=0; i< objs.length(); i++) {
            Q_INFO << "obj child no." << i << objs[i]->objectName();
         }
      }
      return true;
   }

   if ((e->type() == QEvent::KeyPress)
       && false) {
      QKeyEvent *kev = static_cast<QKeyEvent *>(e);
      Q_INFO << "Keypress " << kev->key() << "by obj:" ;
      Q_INFO << "obj name:" << obj->objectName();
      Q_INFO << "obj parent name:" << obj->parent()->objectName();

      QList<QObject *> objs = obj->children();

      for (int i=0; i< objs.length(); i++) {
         Q_INFO << "obj child no." << i << objs[i]->objectName();
         Q_INFO << "obj child no." << i << objs[i]->isWidgetType();
      }

   }

   /**
    * standard event processing
    */
   return QObject::eventFilter(obj, e);
}



