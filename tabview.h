#ifndef TABVIEW_H
#define TABVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QTableView>
#include <QSqlTableModel>
#include <QKeyEvent>
#include <QColor>

//#include "browser.h"
#include "models.h"

namespace Ui {
class TabView;
//class Browser;
}

class TabView : public QWidget {

    Q_OBJECT

public:
    explicit TabView(QWidget *parent = 0);
    ~TabView();

    QGroupBox *grBox() const;
    void setGrBox(QGroupBox *gb);
    QTableView *tv() const;
    void setTv(QTableView *tv);
    void setTvTitle(QString &s);
    bool isActiveSelected()
    { return activeSel; }
    void clearActiveSelection()
    {  activeSel = false; }

    QList<QAction *> getTblActs() const;
    void setTblActs(const QList<QAction *> &value);
    QAction *fieldStrategyAction() const;
    QAction *manualStrategyAction() const;
    QAction *rowStrategyAction() const;

public slots:
    void onObjectNameChanged(const QString &objNam);
    void setGrBoxStyleSht(const QString &ss);
    void setGrBoxTitle(QString s);
    void onTabViewSelChanged(TabView *tv);
    void setAlternateRowCol(
            QColor &col, bool alternateEnabled = true);
    void restoreView();

    void on_insertRowAction_triggered();
    void insertRow();
    void deleteRow();
    void onUpdateActions();

    void on_deleteRowAction_triggered();
    void on_fieldStrategyAction_triggered();
    void on_rowStrategyAction_triggered();
    void on_manualStrategyAction_triggered();
    void on_submitAction_triggered();
    void on_revertAction_triggered();
    void on_selectAction_triggered();

signals:
    void onSelChanged(TabView *tv);

private:
    Ui::TabView		*ui;
    QTableView		*m_tv;
    QGroupBox		*m_gb;
    bool				activeSel;
    QList<QAction *> tblActs;
    //   Browser *browser;

};


#endif // TABVIEW_H
