#ifndef TABVIEW_H
#define TABVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QTableView>
#include <QSqlTableModel>
#include <QKeyEvent>
#include <QColor>

//#include "browser.h"

namespace Ui {
class TabView;
//class Browser;
}

class TabView : public QWidget {

   Q_OBJECT

public:
   explicit TabView(QWidget *parent = 0);
   ~TabView();

   QGroupBox   *m_gb;
   QGroupBox *grBox() const;
   void setGrBox(QGroupBox *gb);
   QTableView *tv() const;
   void setTv(QTableView *tv);
   void setTvTitle(QString &s);
   bool isActiveSelected()
   { return activeSel; }
   void clearActiveSelection()
   {  activeSel = false; }


public slots:
   void onObjectNameChanged(const QString &objNam);
   void setGrBoxStyleSht(const QString &ss);
   void setGrBoxTitle(QString s);
   void onTabViewSelChanged(TabView *tv);

   void setAlternateRowCol(QColor &col, bool alternateEnabled = true);
   void restoreView();
signals:
   void onSelChanged(TabView *tv);

private:
   QTableView *m_tv;
   Ui::TabView *ui;
   bool activeSel;
//   Browser *browser;

};

#endif // TABVIEW_H
