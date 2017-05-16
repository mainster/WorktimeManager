#include <QtGui>
#include <QDockWidget>
#include <QMessageBox>
#include <QtSql>

#include "meincompleter.h"

meincompleter::meincompleter(){

    line = new QLineEdit;
    line->setMaximumHeight(30);
    setCentralWidget(line);

    this->setMaximumSize(500,130);
    this->setMinimumSize(500,130);

    docky = new QDockWidget();
    docky->setMaximumSize(500,100);
    docky->setMinimumSize(500,100);
    docky->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea,docky);

    liste = new QListWidget(docky);
    liste->setMaximumSize(500,100);
    liste->setMinimumSize(500,100);

    docky->hide();

    connect(line,SIGNAL(textChanged(QString)),this,SLOT(eingabe()));
    connect(liste,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(wahl()));

    kostenid = "leer";
}

void meincompleter::eingabe(){

    docky->hide();
    liste->clear();

    if (line->text().count() > 1) {

        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
                     db.setHostName("10.0.3.70");
                     db.setDatabaseName("sawhaushalt");
                     db.setUserName("root");
                     db.setPassword("alex1980");


        if (!db.open()) {
            QSqlError err = db.lastError();
            QMessageBox::information(0,QObject::tr("Fehler"),err.text());
            return;
        }

        QSqlQuery query;
        query.exec("select kostenart,SUBSTRING_INDEX(SUBSTRING_INDEX(kostenart, ')', 1),'(',-1) as nummer from grundkosten where kostenart like('%"
                   + line->text() + "%') order by nummer");
        QSqlRecord record = query.record();
        while (query.next()) {

            element = new QListWidgetItem();
            element->setText(query.value(record.indexOf("kostenart")).toString());
            element->setData(Qt::UserRole,query.value(record.indexOf("nummer")));
            liste->addItem(element);

            docky->show();

        }


        db.close();

    }

}

void meincompleter::wahl(){

    kostenid = liste->currentItem()->data(Qt::UserRole).toString();
    line->setText(liste->currentItem()->text());

    docky->hide();

}
