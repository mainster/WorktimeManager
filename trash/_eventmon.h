#ifndef EVENTMON_H
#define EVENTMON_H

#include <QObject>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include "browser.h"
#include "connectionwidget.h"
#include "globals.h"

/**
 * http://doc.qt.io/qt-5/qobject.html#installEventFilter
 *
 * Warning: If you delete the receiver object in your eventFilter() function,
 * be sure to return true. If you return false, Qt sends the event to the
 * deleted object and the program will crash. Note that the filtering object
 * must be in the same thread as this object. If filterObj is in a different
 * thread, this function does nothing. If either filterObj or this object are
 * moved to a different thread after calling this function, the event filter
 * will not be called until both objects have the same thread affinity again
 * (it is not removed).
 */
//class TabView;

class EventMon : public QObject {
    Q_OBJECT

public:
   explicit EventMon(QObject *parent = 0);

public slots:

signals:

protected:
   bool eventFilter(QObject *obj, QEvent *e);

};


#endif // EVENTMON_H
