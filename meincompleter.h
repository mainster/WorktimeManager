#ifndef MEINCOMPLETER_H
#define MEINCOMPLETER_H
#include <QWidget>
#include <QtGui>
#include <QtSql>

class meincompleter : public QMainWindow {
    Q_OBJECT
public:
    meincompleter();
    QString kostenid, buchkreis;

public slots:
    void eingabe();
    void wahl();

protected:

private:
    QDockWidget *docky;
    QLineEdit *line;
    QListWidget *liste;
    QListWidgetItem *element;
};

#endif // MEINCOMPLETER_H
