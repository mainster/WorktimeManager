#ifndef FILTFORM_H
#define FILTFORM_H

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
class FiltForm;
}

class FiltForm : public QWidget {

    Q_OBJECT

public:
    explicit FiltForm(QWidget *parent = 0);
    ~FiltForm();
    void buildCustomUis(QAbstractItemModel *model);
//    bool eventFilter(QObject *obj, QEvent *ev);
    void setSourceModel(QAbstractItemModel *model);

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
    Ui::FiltForm *ui;
    static FiltForm  * inst,*insta;
    SortFiltProxyMdl * proxyModel;

    QGroupBox   * srcGBox;
    QGroupBox   * proxyGroupBox;
    QTreeView   * srcView;
    QTreeView   * proxyView;
    QCheckBox   * filtCaseSens;
    QLabel      * filtSearchLbl;
    QLabel      * fromLabel;
    QLabel      * toLabel;
    QLineEdit   * filtSearchPat;
    QComboBox   * filtSyntax;
    QDateEdit   * fromDateEdit;
    QDateEdit   * toDateEdit;

};

#endif // FILTFORM_H
