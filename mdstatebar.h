#ifndef MDSTATEBAR_H
#define MDSTATEBAR_H

#include <QWidget>
#include <QTextEdit>
#include <QStatusBar>
#include <QTime>
#include <QTimer>
#include <QDate>
#include <QLayout>
#include <QLabel>

class MDStateBar : public QStatusBar {
   Q_OBJECT

public:
   enum msgType {
      type_info = 0,
      type_msg = 1,
      type_err = 2,
      type_clock = 3
   };
   Q_DECLARE_FLAGS(msgTypes, msgType)

   explicit MDStateBar(QWidget *parent = 0);
   static MDStateBar *getObjectPtr();
   ~MDStateBar();

   QTextEdit *getTeStat() const;
   void setTeStat(QTextEdit *value);

public slots:   
   void showError(const QString s, int timeout = 0);
   void showMessage(QString s, int timeout = 0);
   void showInfo(const QString s, int timeout = 0);
   void showInSlot(const QVariant var, int slotNo);

   void appendError(const QString s, const QString sep = " ");
   void appendMessage(const QString s, const QString sep = " ");
   void appendInfo(const QString s, const QString sep = " ");
   void appendInSlot(const QVariant var, int slotNo, const QString sep);
   void clearError();
   void clearMessage();
   void clearInfo();
   void clearClock();

   void setClockVisible(const bool onoff, int slotNo = -1);

private slots:
   void onCyclic();

private:
   static int PRIVATE_REFRESH_INTERVAL;
   static MDStateBar *inst;
   QList<QLabel *> mslot;
   QTimer *timMsg;

   typedef struct {
      QList<QString> str;
   } prefix_t;
   prefix_t prefix;

   typedef struct {
       QDate date;
       QTime time;
       bool visible;
       int slotNo;
   } clock_t;
   clock_t clock;


};

Q_DECLARE_OPERATORS_FOR_FLAGS(MDStateBar::msgTypes)

#endif // MDSTATEBAR_H
