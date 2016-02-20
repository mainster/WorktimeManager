#ifndef SORTWINDOW_H
#define SORTWINDOW_H

#include <QWidget>

class QAbstractItemModel;
class QCheckBox;
class QComboBox;
class QDateEdit;
class QGroupBox;
class QLabel;
class QLineEdit;
class QTreeView;
class QTableView;
class SortFiltProxyMdl;

namespace Ui {
class SortWindow;
}

class FiltForm : public QWidget {
    Q_OBJECT

public:
    explicit FiltForm(QWidget *parent = 0);
    explicit FiltForm(QTableView *tv, QWidget *parent = 0);
    static FiltForm *getInstance(QWidget *parent = 0x00) {
       if(inst == 0)
          inst = new FiltForm(parent);
       return inst;
    }
    static FiltForm *getInstance(QTableView *tv, QWidget *parent = 0x00) {
       if(insta == 0)
          insta = new FiltForm(tv, parent);
       return insta;
    }
    ~FiltForm();

public slots:
    void setSourceModel(QAbstractItemModel *model);
//    bool eventFilter(QObject *obj, QEvent *ev);

signals:
    void hideFiltForm(bool b = true);
    void showFiltForm(bool b = true);
    void closesUncheck(bool b);

protected slots:
    void closeEvent(QCloseEvent *e);
    void keyPressEvent(QKeyEvent *ev);

private slots:
    void textFilterChanged();
    void dateFilterChanged();
private:
    static FiltForm  * inst,*insta;
    Ui::SortWindow   * ui;
    SortFiltProxyMdl * proxyModel;

    QGroupBox   * sourceGroupBox;
    QGroupBox   * proxyGroupBox;
    QTreeView   * sourceView;
    QTreeView   * proxyView;
    QCheckBox   * filterCaseSensitivityCheckBox;
    QLabel      * filterPatternLabel;
    QLabel      * fromLabel;
    QLabel      * toLabel;
    QLineEdit   * filterPatternLineEdit;
    QComboBox   * filterSyntaxComboBox;
    QDateEdit   * fromDateEdit;
    QDateEdit   * toDateEdit;

};

#endif // SORTWINDOW_H
