#ifndef SORTWINDOW_H
#define SORTWINDOW_H

#include <QWidget>
#include <QCompleter>

class QAbstractItemModel;
class QCheckBox;
class QComboBox;
class QDateEdit;
class QGroupBox;
class QLabel;
class QLineEdit;
class QTreeView;

class SfiltMdl;

namespace Ui {
class SortWindow;
}

class SortWindow : public QWidget {
    Q_OBJECT

public:
    explicit SortWindow(QWidget *parent = 0);
    ~SortWindow();
    static SortWindow *getInstance(QWidget *parent = 0x00) {
       if(inst == 0)
          inst = new SortWindow(parent);
       return inst;
    }
    void setSourceModel(QAbstractItemModel *model);

public slots:
//    bool eventFilter(QObject *obj, QEvent *ev);
	 virtual void keyPressEvent(QKeyEvent *) override;

	 void cbTextFilterChanged();
signals:
    void closesUncheck(bool b);

protected:
protected slots:
    void closeEvent(QCloseEvent *e);
private slots:
    void textFilterChanged();
    void dateFilterChanged();

private:
    static SortWindow *inst;

    Ui::SortWindow *ui;
    SfiltMdl *proxyModel;

    QGroupBox *sourceGB;
    QGroupBox *proxyGB;
    QTreeView *sourceView;
    QTreeView *proxyView;
    QCheckBox *filtCaseSensCB;
    QLabel *filterPatternLabel;
    QLabel *fromLabel;
    QLabel *toLabel;
    QLineEdit *filtPattLE;
	 QComboBox *filtSyntaxCB, *filtPattCb;
    QDateEdit *fromDateEdit;
    QDateEdit *toDateEdit;
    QAction *deleteAct;


	 // QWidget interface
protected:
};

#endif // SORTWINDOW_H
