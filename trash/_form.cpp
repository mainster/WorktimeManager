#include "form.h"
#include "ui_form.h"

#include <QWidget>
#include <QtCore>
#include <QPushButton>
#include <QMenuBar>
#include <QLineEdit>
#include <QSpinBox>
#include <QTimer>
#include "globals.h"
#include <QObject>

#include <QTableView>

Form::Form(QWidget *parent) :  QWidget(parent), ui(new Ui::Form) {
   ui->setupUi(this);


   ui->tv1->setProperty("sel","true");
   tim = new QTimer(this);
   tim->setInterval(1000);
   tim->start();

   connect (ui->pb1,    SIGNAL(clicked()),
            this,       SLOT(onA_BtnClick()));
   connect (ui->pb2,    SIGNAL(clicked()),
            this,       SLOT(onB_BtnClick()));
   connect (ui->pb3,    SIGNAL(clicked()),
            this,       SLOT(onC_BtnClick()));
   connect (tim,        SIGNAL(timeout()),
            this,       SLOT(onTimTimeout()));

//   ui->tv1->m_gb->setProperty("select",true);
//   ui->tv2->m_gb->setProperty("select","true");
//   ui->tv1->m_gb->setStyleSheet(ui->tv1->m_gb->styleSheet());
//   ui->tv2->m_gb->setStyleSheet(ui->tv2->m_gb->styleSheet());
}

Form::~Form() {
   delete ui;
}

void Form::onTimTimeout() {
   /* ---------------------------------------------------------------- */
   /*         Get the name of object under the mouse pointer           */
   /* ---------------------------------------------------------------- */
   QWidget *widget = qApp->widgetAt(QCursor::pos());
   if (widget != 0) {
      QString widName = widget->objectName();
      INFO << widName << widget;

//      QPushButton *pbn = static_cast<QPushButton *>(widget);
//      if ((pbn != 0) && pbn1 !=0)
//         INFO << pbn->styleSheet();

////      TabView *tv = static_cast<TabView *>(widget);
////      if (tv != 0) {
////         INFO << tv->styleSheet();

//      }
   }
}
void Form::onA_BtnClick() {
   ui->tv1->parent()->setProperty("select","true");
   ui->tv2->parent()->setProperty("select","false");
   ui->tv1->parentWidget()->setStyleSheet(ui->tv1->styleSheet());
   ui->tv2->parentWidget()->setStyleSheet(ui->tv2->styleSheet());

   return;
}
void Form::onB_BtnClick() {
   ui->tv1->m_gb->setProperty("select","false");
   ui->tv1->m_gb->setStyleSheet(ui->tv1->m_gb->styleSheet());
   ui->tv2->m_gb->setStyleSheet(ui->tv2->m_gb->styleSheet());
   return;
}
void Form::onC_BtnClick() {
   ui->tv1->m_gb->setProperty("select","true");
//   ui->tv1->setProperty("sel","false");
//   ui->tv2->setProperty("sel","true");
   ui->tv1->m_gb->setStyleSheet(ui->tv1->m_gb->styleSheet());
//   ui->tv1->setStyleSheet(ui->tv1->styleSheet());
//   ui->tv2->setStyleSheet(ui->tv2->styleSheet());
   return;
}













//QMenuBar *menuBar = new QMenuBar(this);

//QMenu *fileMenu =  menuBar->addMenu(QObject::tr("&File"));
//fileMenu->addAction(QObject::tr("Add &Connection..."), this, SLOT(addConnection()));
//fileMenu->addSeparator();
//fileMenu->addAction(QObject::tr("&Quit"), qApp, SLOT(quit()));

//QMenu *helpMenu = menuBar->addMenu(QObject::tr("&Help"));
//helpMenu->addAction(QObject::tr("About"), this, SLOT(about()));
//helpMenu->addAction(QObject::tr("About Qt"), qApp, SLOT(aboutQt()));

//this->pbn1 = 0;

//QPushButton *pb = new QPushButton("&ok", this);
//connect (pb, SIGNAL(clicked()),
//         this, SLOT(onBtnClick()));

//QPushButton *pb1 = new QPushButton("&pk", this);
//connect (pb1, SIGNAL(clicked()),
//         this, SLOT(onBtnClick1()));

//ui->vlay->addWidget(pb);
//ui->vlay->addWidget(pb1);

//tim = new QTimer(this);
//tim->setInterval(250);
//tim->start();

//connect(tim,   SIGNAL(timeout()),
//        this,  SLOT(onTimTimeout()));

////   ui->myTv->setProperty("prop","rank1");

//for (int i=0; i < 3; i++) {
//   lev.append(new QLineEdit(this));
//   lev.last()->setProperty("mandatoryField", true);
//   ui->vlay->addWidget(lev.last());
//   pbv.append( new QPushButton(this));
//   pbv.last()->setStyleSheet(
//            "QPushButton#evilButton { background-color: red }");
//   ui->vlay->addWidget(pbv.last());
//}

//ui->tvat->setObjectName(QString::fromUtf8("tvat"));
//ui->tvat->setStyleSheet(Globals::gbStyleShtCenterPROPERTYS);

//void Form::doPolish() {
//   this->pbn1->ensurePolished();
//   pbn1 = 0;
//}


//   lev[0]->setProperty("mandatoryField", false);
//   lev[2]->setProperty("mandatoryField", false);

//   pbv[1]->setObjectName("evilButton");
//   pbv[1]->setStyleSheet(pbv[1]->styleSheet());

//   QString::compare(ui->myTv->property("prop").toString(),
//                    "rank1") ?
//            ui->myTv->setProperty("prop", "rank2")
//          : ui->myTv->setProperty("prop", "rank1");

//   ui->myTv->setStyleSheet(ui->myTv->styleSheet());


//   //   ui->lineEdit_3->setProperty("urgent", true);
////   style()->unpolish(ui->lineEdit);
////   style()->unpolish(ui->lineEdit_2);
////   style()->unpolish(ui->lineEdit_3);
////   style()->polish(ui->lineEdit);
////   style()->polish(ui->lineEdit_2);
////   style()->polish(ui->lineEdit_3);
//}*/
