#ifndef GLOBALS_H
#define GLOBALS_H

#include <QString>
#include <QProcess>
#include <QSettings>
#include <QDebug>
#include <QTime>
#include <QObject>
#include <QVariant>
#include <QComboBox>

#define CONFIG_PATH tr("/home/mainster/.config/" +qgetenv("APP_NAME") + qgetenv("APP_VERSION") + "/config")
#define CUSTOM_QUERYS_PATH tr("/home/mainster/.config/" +qgetenv("APP_NAME")  + qgetenv("APP_VERSION") + "/customQuerys")
#define MAINWINDOW_TITLE tr(qgetenv("APP_NAME") + qgetenv("APP_VERSION"))
#define Q_INFO (qDebug().noquote() << QTime::currentTime() << Q_FUNC_INFO )
#define QSETTINGS QSettings config(CONFIG_PATH, QSettings::IniFormat);
#define QSETTINGS_QUERYS QSettings configQ(CUSTOM_QUERYS_PATH, QSettings::IniFormat);

/**
 * Env vars:
 *
 * APP_NAME=WorktimeManager_v
 * APP_VERSION=1.35
 */



class Globals {

public:
   static QString
   SQL_DB_PATH,
   SQL_DB_FILE;

   static QString
   ddgbStyleShtA,
   ddgbStyleShtInpFrm,
   ddgbCheckableStyleSht,
   ddtvStyleShtA,
   ddgbStyleShtCenterHead,
   gbStyleShtCenterPROPERTYS;

public:
   static Globals *getObjectPtr() {
      if(inst == 0)
         inst = new Globals();
      return inst;

   }

   void notify_os(QString msgBody,
                  QString msgHead = "Globals",
                  QString msgTime = "2000") {

      S_STR_NOTIFY = STR_NOTIFY
            .replace("@MSG_TIME@", msgTime)
            .replace("@APP@","SQL browser")
            .replace("@MSG_HEAD@", msgHead)
            .replace("@MSG_BODY@", msgBody);
      proc.start(STR_NOTIFY);
   }
   static QStringList widToStrLst(QWidgetList &lst);
   static QStringList objToStrLst(QObjectList &lst);
   static QString widToStr(QWidgetList &lst, QString sep = ",");
   static QString objToStr(QObjectList &lst, QString sep = ",");

public:  /** Declarations */
   static QString STR_NOTIFY;

public slots:
private:
   Globals();
   static Globals * inst;

   QProcess proc;
   QString S_STR_NOTIFY;

   static QStringList strLst;
};

#endif // GLOBALS_H
