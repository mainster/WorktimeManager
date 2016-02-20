#include "filtform.h"
#include "ui_filtform.h"

#include "sortfiltproxymdl.h"
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
#include <QTableView>
#include <QAbstractTableModel>
#include <QAbstractItemModel>


FiltForm::FiltForm(QWidget *parent) :
    QWidget(parent), ui(new Ui::FiltForm) {
    ui->setupUi(this);


    proxyModel = new SortFiltProxyMdl(this);
    proxyModel->setDynamicSortFilter(true);

    srcView = new QTreeView(ui->treeSrcMdl);
    srcView->setRootIsDecorated(false);
    srcView->setAlternatingRowColors(true);

    QHBoxLayout *sourceLayout = new QHBoxLayout;
    sourceLayout->addWidget(srcView);
    srcGBox = new QGroupBox(tr("Original Model"));
    srcGBox->setLayout(sourceLayout);

    filtCaseSens = new QCheckBox(ui->caseSensSel);
    filtCaseSens->setText("Case sensitive filter");
    filtCaseSens->setChecked(true);

    filtSearchPat = new QLineEdit(ui->leFilter);
    filtSearchPat->setText("");

    filtSearchLbl = new QLabel(tr("&Filter pattern:"));
    filtSearchLbl->setBuddy(filtSearchPat);

    filtSyntax = new QComboBox(ui->cbxFiltSyntax);
    filtSyntax->addItem(tr("Regular expression"), QRegExp::RegExp);
    filtSyntax->addItem(tr("Wildcard"), QRegExp::Wildcard);
    filtSyntax->addItem(tr("Fixed string"), QRegExp::FixedString);

}
FiltForm::~FiltForm() {
    delete ui;
}

void FiltForm::setSourceModel(QAbstractItemModel *model) {
    proxyModel->setSourceModel(model);
    srcView->setModel(model);
}
void FiltForm::buildCustomUis(QAbstractItemModel *model) {
    QStringList head;

    for (int i=0; i<model->columnCount(); i++)
        head << model->headerData(i, Qt::Horizontal).toString();

    Q_INFO << head;

    fromDateEdit = new QDateEdit(ui->dateEditFrom);
    fromDateEdit->setDate( QDate::currentDate().addYears(-2));
    fromDateEdit->setDisplayFormat("dd.MM.yyyy");
    fromLabel = ui->lblDateFrom;
    fromLabel->setBuddy(fromDateEdit);

    toDateEdit = new QDateEdit(ui->dateEditTo);
    toDateEdit->setDate( QDate::currentDate().addYears(2));
    toDateEdit->setDisplayFormat("dd.MM.yyyy");
    toLabel = ui->lblDateTo;
    toLabel->setBuddy(toDateEdit);

    connect(filtSearchPat,  SIGNAL(textChanged(QString)),
            this,           SLOT(textFilterChanged()));
    connect(filtSyntax,     SIGNAL(currentIndexChanged(int)),
            this,           SLOT(textFilterChanged()));
    connect(filtCaseSens,   SIGNAL(toggled(bool)),
            this,           SLOT(textFilterChanged()));
    connect(fromDateEdit,   SIGNAL(dateChanged(QDate)),
            this,           SLOT(dateFilterChanged()));
    connect(toDateEdit,     SIGNAL(dateChanged(QDate)),
            this,           SLOT(dateFilterChanged()));

    srcView = new QTreeView(ui->treeProxyMdl);
    proxyView->setRootIsDecorated(false);
    proxyView->setAlternatingRowColors(true);
    proxyView->setModel(proxyModel);
    proxyView->setSortingEnabled(true);
    proxyView->sortByColumn(1, Qt::AscendingOrder);

    QGridLayout lay = this->layout();

//    QGridLayout *proxyLayout = this->layout()
//    proxyLayout->addWidget(proxyView,       0, 0, 1, 3);
//    proxyLayout->addWidget(filtSearchLbl,   1, 0);
//    proxyLayout->addWidget(filtSearchPat,   1, 1);
//    proxyLayout->addWidget(filtSyntax,      1, 2);
//    proxyLayout->addWidget(filtCaseSens,    1, 3);
//    proxyLayout->addWidget(fromLabel,       3, 0);
//    proxyLayout->addWidget(fromDateEdit,    3, 1, 1, 2);
//    proxyLayout->addWidget(toLabel,         4, 0);
//    proxyLayout->addWidget(toDateEdit,      4, 1, 1, 2);

//    proxyGroupBox = new QGroupBox(tr("Sorted/Filtered Model"));
//    proxyGroupBox->setLayout(proxyLayout);

//    QVBoxLayout *mainLayout = new QVBoxLayout;
//    mainLayout->addWidget(srcGBox);
//    mainLayout->addWidget(proxyGroupBox);
//    setLayout(mainLayout);

    setWindowTitle(tr("Filter Dialog"));
    resize(600, 950);
}
void FiltForm::textFilterChanged() {
    QRegExp::PatternSyntax syntax =
            QRegExp::PatternSyntax(filtSyntax->itemData(
                    filtSyntax->currentIndex()).toInt());
    Qt::CaseSensitivity caseSensitivity =
            filtCaseSens->isChecked()
            ? Qt::CaseSensitive
            : Qt::CaseInsensitive;

    QRegExp regExp(filtSearchPat->text(), caseSensitivity, syntax);
    proxyModel->setFilterRegExp(regExp);

//    qDebug().noquote() << tr("from") << fromDateEdit->dateTime()
//                       << tr("to") << toDateEdit->dateTime();
}
void FiltForm::dateFilterChanged() {
//    qDebug().noquote() << fromDateEdit->date().toString("MM/dd/yy");

    proxyModel->setFilterMinimumDate(fromDateEdit->date());
    proxyModel->setFilterMaximumDate(toDateEdit->date());
}
void FiltForm::keyPressEvent(QKeyEvent *ev) {
    /**
     * Use this keyPressEvent handler to detect if key ESC was triggered
    */

    /*
    * If esc is received, emit closeFiltTriggered signal
    */
    if (ev->key() == Qt::Key_Escape) {
        emit hideFiltForm(true);
        emit showFiltForm(false);
    }

    if (ev->key() == Qt::Key_Enter) {
        Q_INFO << tr("Enter triggered");
    }

    return QWidget::keyPressEvent(ev);
}
void FiltForm::closeEvent(QCloseEvent *e) {
    Q_UNUSED(e);
    QSETTINGS;
    emit closesUncheck(false);
}
