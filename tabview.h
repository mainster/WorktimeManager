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

   void setTv(QTableView *tv);
   void setTvTitle(QString &s);
   void setGrBox(QGroupBox *gb);
   void clearActiveSelection()
   {  activeSel = false; }
   bool isActiveSelected()
   { return activeSel; }

   QGroupBox    *m_gb;
   QGroupBox    *grBox() const;
   QTableView   *tv() const;

public slots:
   void onObjectNameChanged(const QString &objNam);
   void onTabViewSelChanged(TabView *tv);
   void setGrBoxStyleSht(const QString &ss);
   void setGrBoxTitle(QString s);
   void setAlternateRowCol(QColor &col, bool alternateEnabled = true);
   void restoreView();

signals:
   void onSelChanged(TabView *tv);

private:
   QTableView   *m_tv;
   Ui::TabView  *ui;
   bool         activeSel;

};


#endif // TABVIEW_H
