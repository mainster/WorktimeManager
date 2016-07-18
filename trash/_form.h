#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QLineEdit>
#include <QVector>
#include <QPushButton>
#include <QTimer>
#include <QObject>
#include <QTableView>

namespace Ui {
class Form;
}

class Form : public QWidget
{
   Q_OBJECT

public:
   explicit Form(QWidget *parent = 0);
   ~Form();

public slots:
   void onTimTimeout();
   void onA_BtnClick();
   void onB_BtnClick();
   void onC_BtnClick();
private:
   Ui::Form *ui;
   QVector<QLineEdit *>lev;
   QVector<QPushButton *>pbv;
   QTimer *tim;
   int ctr;
};

#endif // FORM_H
