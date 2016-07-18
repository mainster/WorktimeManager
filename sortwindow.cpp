#include "mysortfilterproxymodel.h"
#include "sortwindow.h"
#include "ui_sortwindow.h"
#include "globals.h"

#include <QtGui>
#include <QTreeView>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QDate>
#include <QDateEdit>
#include <QtCore>
#include <QComboBox>
#include <QMainWindow>

SortWindow *SortWindow::inst = 0;

SortWindow::SortWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SortWindow) {

    ui->setupUi(this);
    proxyModel = new SfiltMdl(this);
    proxyModel->setDynamicSortFilter(true);

    sourceView = ui->sourceView;
    sourceGB = ui->sourceGB;
    filtCaseSensCB = ui->filtCaseSensCB;
    filtPattLE = ui->filtPattLE;
    filterPatternLabel = new QLabel(tr("&Filter pattern:"));
    filterPatternLabel->setBuddy(filtPattLE);

    filtSyntaxCB = ui->filtSyntaxCB;
    filtSyntaxCB->addItem(tr("Regular expression"), QRegExp::RegExp);
    filtSyntaxCB->addItem(tr("Wildcard"), QRegExp::Wildcard);
    filtSyntaxCB->addItem(tr("Fixed string"), QRegExp::FixedString);

    fromDateEdit = ui->fromDateEdit;
    fromDateEdit->setDate( QDate::currentDate().addYears(-2));
    fromDateEdit->setDisplayFormat("dd.MM.yyyy");
    fromLabel = new QLabel(tr("F&rom:"));
    fromLabel->setBuddy(fromDateEdit);

    toDateEdit = ui->toDateEdit;
    toDateEdit->setDate( QDate::currentDate().addYears(2));
    toDateEdit->setDisplayFormat("dd.MM.yyyy");
    toLabel = new QLabel(tr("&To:"));
    toLabel->setBuddy(toDateEdit);

    connect(filtPattLE,     SIGNAL(textChanged(QString)),
            this,           SLOT(textFilterChanged()));
    connect(filtSyntaxCB,   SIGNAL(currentIndexChanged(int)),
            this,           SLOT(textFilterChanged()));
    connect(filtCaseSensCB, SIGNAL(toggled(bool)),
            this,           SLOT(textFilterChanged()));
    connect(fromDateEdit,   SIGNAL(dateChanged(QDate)),
            this,           SLOT(dateFilterChanged()));
    connect(toDateEdit,     SIGNAL(dateChanged(QDate)),
            this,           SLOT(dateFilterChanged()));

    proxyView = ui->proxyView;
    proxyView->setRootIsDecorated(false);
    proxyView->setAlternatingRowColors(true);
    proxyView->setModel(proxyModel);
    proxyView->setSortingEnabled(true);
    proxyView->sortByColumn(1, Qt::AscendingOrder);

    proxyGB = ui->proxyGB;

    setWindowTitle(tr("Custom Sort/Filter Model"));
    resize(500, 900);

    //    installEventFilter(this);
}

SortWindow::~SortWindow() {
    delete ui;
}
void SortWindow::setSourceModel(QAbstractItemModel *model) {
    proxyModel->setSourceModel(model);
    sourceView->setModel(model);
}
void SortWindow::textFilterChanged() {
    QRegExp::PatternSyntax syntax =
            QRegExp::PatternSyntax(filtSyntaxCB->itemData(
                                       filtSyntaxCB->currentIndex()).toInt());
    Qt::CaseSensitivity caseSensitivity =
            filtCaseSensCB->isChecked()
            ? Qt::CaseSensitive
            : Qt::CaseInsensitive;

    QRegExp regExp(filtPattLE->text(), caseSensitivity, syntax);
    proxyModel->setFilterRegExp(regExp);

    //    qDebug().noquote() << tr("from") << fromDateEdit->dateTime()
    //                       << tr("to") << toDateEdit->dateTime();
}
void SortWindow::dateFilterChanged() {
    //    qDebug().noquote() << fromDateEdit->date().toString("MM/dd/yy");

    proxyModel->setFilterMinimumDate(fromDateEdit->date());
    proxyModel->setFilterMaximumDate(toDateEdit->date());
}
void SortWindow::closeEvent(QCloseEvent *e) {
    Q_UNUSED(e);
    QSETTINGS;
    emit closesUncheck(false);
}
bool SortWindow::eventFilter(QObject *obj, QEvent *ev) {
    if (ev->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(ev);
        INFO << tr("Ate key press %1").arg(keyEvent->key());
        return QObject::eventFilter(obj, ev);
    }

    // pass the event on to the parent class
    return QObject::eventFilter(obj, ev);
}




//    //     = new QTreeView;
//    sourceView = ui->sourceView;
//    sourceView->setRootIsDecorated(false);
//    sourceView->setAlternatingRowColors(true);

////    QHBoxLayout *sourceLayout = new QHBoxLayout;
////    sourceLayout->addWidget(sourceView);
////    //    sourceGroupBox = new QGroupBox(tr("Original Model"));
//    sourceGB = ui->sourceGB;
////    sourceGB->setLayout(sourceLayout);

//    //    filterCaseSensitivityCheckBox = new QCheckBox(tr("Case sensitive filter"));
//    filtCaseSensCB = ui->filtCaseSensCB;
//    filtCaseSensCB->setText(tr("Case sensitive filter"));
//    filtCaseSensCB->setChecked(true);

//    //    filterPatternLineEdit = new QLineEdit;
//    //    filterPatternLineEdit->setText("Grace|Sports");
//    filtPattLE = ui->filtPattLE;

//    filterPatternLabel = new QLabel(tr("&Filter pattern:"));
//    filterPatternLabel->setBuddy(filtPattLE);

//    filtSyntaxCB = ui->filtSyntaxCB;
//    filtSyntaxCB->addItem(tr("Regular expression"), QRegExp::RegExp);
//    filtSyntaxCB->addItem(tr("Wildcard"), QRegExp::Wildcard);
//    filtSyntaxCB->addItem(tr("Fixed string"), QRegExp::FixedString);

//    //    fromDateEdit = new QDateEdit;
//    fromDateEdit = ui->fromDateEdit;
//    fromDateEdit->setDate( QDate::currentDate().addYears(-2));
//    fromDateEdit->setDisplayFormat("dd.MM.yyyy");
//    fromLabel = new QLabel(tr("F&rom:"));
//    fromLabel->setBuddy(fromDateEdit);

//    //    toDateEdit = new QDateEdit;
//    toDateEdit = ui->toDateEdit;
//    toDateEdit->setDate( QDate::currentDate().addYears(2));
//    toDateEdit->setDisplayFormat("dd.MM.yyyy");
//    toLabel = new QLabel(tr("&To:"));
//    toLabel->setBuddy(toDateEdit);

//    connect(filtPattLE,     SIGNAL(textChanged(QString)),
//            this,           SLOT(textFilterChanged()));
//    connect(filtSyntaxCB,   SIGNAL(currentIndexChanged(int)),
//            this,           SLOT(textFilterChanged()));
//    connect(filtCaseSensCB, SIGNAL(toggled(bool)),
//            this,                   SLOT(textFilterChanged()));
//    connect(fromDateEdit,           SIGNAL(dateChanged(QDate)),
//            this,                   SLOT(dateFilterChanged()));
//    connect(toDateEdit,             SIGNAL(dateChanged(QDate)),
//            this,                   SLOT(dateFilterChanged()));

//    //    proxyView = new QTreeView;
//    proxyView = ui->proxyView;
//    proxyView->setRootIsDecorated(false);
//    proxyView->setAlternatingRowColors(true);
//    proxyView->setModel(proxyModel);
//    proxyView->setSortingEnabled(true);
//    proxyView->sortByColumn(1, Qt::AscendingOrder);


//    QGridLayout *proxyLayout = ui->gridLayout;
////    QGridLayout *proxyLayout = new QGridLayout;
////    proxyLayout->addWidget(proxyView, 0, 0, 1, 3);
////    proxyLayout->addWidget(filterPatternLabel, 1, 0);
////    proxyLayout->addWidget(filtPattLE, 1, 1);
////    proxyLayout->addWidget(filtSyntaxCB, 1, 2);
////    proxyLayout->addWidget(filtCaseSensCB, 2, 0, 1, 3);
////    proxyLayout->addWidget(fromLabel, 3, 0);
////    proxyLayout->addWidget(fromDateEdit, 3, 1, 1, 2);
////    proxyLayout->addWidget(toLabel, 4, 0);
////    proxyLayout->addWidget(toDateEdit, 4, 1, 1, 2);

//    //    proxyGroupBox = new QGroupBox(tr("Sorted/Filtered Model"));
//    proxyGB = ui->proxyGB;
//    proxyGB->setLayout(proxyLayout);

//    QVBoxLayout *mainLayout = new QVBoxLayout;
//    mainLayout->addWidget(sourceGB);
//    mainLayout->addWidget(proxyGB);
//    setLayout(mainLayout);
