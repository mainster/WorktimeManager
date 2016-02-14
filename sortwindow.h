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

class MySortFilterProxyModel;

namespace Ui {
class SortWindow;
}

class SortWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SortWindow(QWidget *parent = 0);
    ~SortWindow();
    void setSourceModel(QAbstractItemModel *model);

signals:
    void close(bool b);

protected slots:
    void closeEvent(QCloseEvent *e);
private slots:
    void textFilterChanged();
    void dateFilterChanged();

private:
    Ui::SortWindow *ui;
    MySortFilterProxyModel *proxyModel;

    QGroupBox *sourceGroupBox;
    QGroupBox *proxyGroupBox;
    QTreeView *sourceView;
    QTreeView *proxyView;
    QCheckBox *filterCaseSensitivityCheckBox;
    QLabel *filterPatternLabel;
    QLabel *fromLabel;
    QLabel *toLabel;
    QLineEdit *filterPatternLineEdit;
    QComboBox *filterSyntaxComboBox;
    QDateEdit *fromDateEdit;
    QDateEdit *toDateEdit;

};

#endif // SORTWINDOW_H
