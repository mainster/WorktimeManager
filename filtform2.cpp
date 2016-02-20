#include "sortfiltproxymdl.h"
#include "filtform.h"
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
#include <QTableView>
#include <QAbstractTableModel>
#include <QAbstractItemModel>


FiltForm *FiltForm::inst = 0;
FiltForm *FiltForm::insta = 0;

/**
 * Default constructor
 */
FiltForm::FiltForm(QWidget *parent) :
    QWidget(parent), ui(new Ui::SortWindow) {

    ui->setupUi(this);
    proxyModel = new SortFiltProxyMdl(this);
    proxyModel->setDynamicSortFilter(true);

    sourceView = new QTreeView;
    sourceView->setRootIsDecorated(false);
    sourceView->setAlternatingRowColors(true);

    QHBoxLayout *sourceLayout = new QHBoxLayout;
    sourceLayout->addWidget(sourceView);
    sourceGroupBox = new QGroupBox(tr("Original Model"));
    sourceGroupBox->setLayout(sourceLayout);

    filterCaseSensitivityCheckBox = new QCheckBox(tr("Case sensitive filter"));
    filterCaseSensitivityCheckBox->setChecked(true);

    filterPatternLineEdit = new QLineEdit;
    filterPatternLineEdit->setText("Grace|Sports");

    filterPatternLabel = new QLabel(tr("&Filter pattern:"));
    filterPatternLabel->setBuddy(filterPatternLineEdit);

    filterSyntaxComboBox = new QComboBox;
    filterSyntaxComboBox->addItem(tr("Regular expression"), QRegExp::RegExp);
    filterSyntaxComboBox->addItem(tr("Wildcard"), QRegExp::Wildcard);
    filterSyntaxComboBox->addItem(tr("Fixed string"), QRegExp::FixedString);

    fromDateEdit = new QDateEdit;
    fromDateEdit->setDate( QDate::currentDate().addYears(-2));
    fromDateEdit->setDisplayFormat("dd.MM.yyyy");
    fromLabel = new QLabel(tr("F&rom:"));
    fromLabel->setBuddy(fromDateEdit);

    toDateEdit = new QDateEdit;
    toDateEdit->setDate( QDate::currentDate().addYears(2));
    toDateEdit->setDisplayFormat("dd.MM.yyyy");
    toLabel = new QLabel(tr("&To:"));
    toLabel->setBuddy(toDateEdit);

    connect(filterPatternLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(textFilterChanged()));
    connect(filterSyntaxComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(textFilterChanged()));
    connect(filterCaseSensitivityCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(textFilterChanged()));
    connect(fromDateEdit, SIGNAL(dateChanged(QDate)),
            this, SLOT(dateFilterChanged()));
    connect(toDateEdit, SIGNAL(dateChanged(QDate)),
            this, SLOT(dateFilterChanged()));

    proxyView = new QTreeView;
    proxyView->setRootIsDecorated(false);
    proxyView->setAlternatingRowColors(true);
    proxyView->setModel(proxyModel);
    proxyView->setSortingEnabled(true);
    proxyView->sortByColumn(1, Qt::AscendingOrder);

    QGridLayout *proxyLayout = new QGridLayout;
    proxyLayout->addWidget(proxyView, 0, 0, 1, 3);
    proxyLayout->addWidget(filterPatternLabel, 1, 0);
    proxyLayout->addWidget(filterPatternLineEdit, 1, 1);
    proxyLayout->addWidget(filterSyntaxComboBox, 1, 2);
    proxyLayout->addWidget(filterCaseSensitivityCheckBox, 2, 0, 1, 3);
    proxyLayout->addWidget(fromLabel, 3, 0);
    proxyLayout->addWidget(fromDateEdit, 3, 1, 1, 2);
    proxyLayout->addWidget(toLabel, 4, 0);
    proxyLayout->addWidget(toDateEdit, 4, 1, 1, 2);

    proxyGroupBox = new QGroupBox(tr("Sorted/Filtered Model"));
    proxyGroupBox->setLayout(proxyLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(sourceGroupBox);
    mainLayout->addWidget(proxyGroupBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Custom Sort/Filter Model"));
    resize(500, 450);

//    installEventFilter(this);
//    installEventFilter(this);
}

FiltForm::FiltForm(QTableView *tv, QWidget *parent) :
    QWidget(parent), ui(new Ui::SortWindow) {

    ui->setupUi(this);
    proxyModel = new SortFiltProxyMdl(this);
    proxyModel->setDynamicSortFilter(true);

    sourceView = new QTreeView;
    sourceView->setRootIsDecorated(false);
    sourceView->setAlternatingRowColors(true);

    QHBoxLayout *sourceLayout = new QHBoxLayout;
    sourceLayout->addWidget(sourceView);
    sourceGroupBox = new QGroupBox(tr("Original Model"));
    sourceGroupBox->setLayout(sourceLayout);

    filterCaseSensitivityCheckBox = new QCheckBox(tr("Case sensitive filter"));
    filterCaseSensitivityCheckBox->setChecked(true);

    filterPatternLineEdit = new QLineEdit;
    filterPatternLineEdit->setText("");

    filterPatternLabel = new QLabel(tr("&Filter pattern:"));
    filterPatternLabel->setBuddy(filterPatternLineEdit);

    filterSyntaxComboBox = new QComboBox;
    filterSyntaxComboBox->addItem(tr("Regular expression"), QRegExp::RegExp);
    filterSyntaxComboBox->addItem(tr("Wildcard"), QRegExp::Wildcard);
    filterSyntaxComboBox->addItem(tr("Fixed string"), QRegExp::FixedString);

    return;

    /*
     * Check header of the tv object
     */
    QAbstractItemModel *aitemMdl = tv->model();
    QStringList head;

    for (int i=0; i<aitemMdl->columnCount(); i++)
        head << aitemMdl->headerData(i, Qt::Horizontal).toString();

    Q_INFO << head;

    fromDateEdit = new QDateEdit;
    fromDateEdit->setDate( QDate::currentDate().addYears(-2));
    fromDateEdit->setDisplayFormat("dd.MM.yyyy");
    fromLabel = new QLabel(tr("F&rom:"));
    fromLabel->setBuddy(fromDateEdit);

    toDateEdit = new QDateEdit;
    toDateEdit->setDate( QDate::currentDate().addYears(2));
    toDateEdit->setDisplayFormat("dd.MM.yyyy");
    toLabel = new QLabel(tr("&To:"));
    toLabel->setBuddy(toDateEdit);

    connect(filterPatternLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(textFilterChanged()));
    connect(filterSyntaxComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(textFilterChanged()));
    connect(filterCaseSensitivityCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(textFilterChanged()));
    connect(fromDateEdit, SIGNAL(dateChanged(QDate)),
            this, SLOT(dateFilterChanged()));
    connect(toDateEdit, SIGNAL(dateChanged(QDate)),
            this, SLOT(dateFilterChanged()));

    proxyView = new QTreeView;
    proxyView->setRootIsDecorated(false);
    proxyView->setAlternatingRowColors(true);
    proxyView->setModel(proxyModel);
    proxyView->setSortingEnabled(true);
    proxyView->sortByColumn(1, Qt::AscendingOrder);

    QGridLayout *proxyLayout = new QGridLayout;
    proxyLayout->addWidget(proxyView, 0, 0, 1, 3);
    proxyLayout->addWidget(filterPatternLabel, 1, 0);
    proxyLayout->addWidget(filterPatternLineEdit, 1, 1);
    proxyLayout->addWidget(filterSyntaxComboBox, 1, 2);
    proxyLayout->addWidget(filterCaseSensitivityCheckBox, 2, 0, 1, 3);
    proxyLayout->addWidget(fromLabel, 3, 0);
    proxyLayout->addWidget(fromDateEdit, 3, 1, 1, 2);
    proxyLayout->addWidget(toLabel, 4, 0);
    proxyLayout->addWidget(toDateEdit, 4, 1, 1, 2);

    proxyGroupBox = new QGroupBox(tr("Sorted/Filtered Model"));
    proxyGroupBox->setLayout(proxyLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(sourceGroupBox);
    mainLayout->addWidget(proxyGroupBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Custom Sort/Filter Model"));
    resize(500, 450);

//    installEventFilter(this);
//    installEventFilter(this);
}
FiltForm::~FiltForm() {
    delete ui;
}

void FiltForm::setSourceModel(QAbstractItemModel *model) {
    proxyModel->setSourceModel(model);
    sourceView->setModel(model);
}
void FiltForm::textFilterChanged() {
    QRegExp::PatternSyntax syntax =
            QRegExp::PatternSyntax(filterSyntaxComboBox->itemData(
                    filterSyntaxComboBox->currentIndex()).toInt());
    Qt::CaseSensitivity caseSensitivity =
            filterCaseSensitivityCheckBox->isChecked()
            ? Qt::CaseSensitive
            : Qt::CaseInsensitive;

    QRegExp regExp(filterPatternLineEdit->text(), caseSensitivity, syntax);
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
//bool FiltForm::eventFilter(QObject *obj, QEvent *ev) {
//    if (ev->type() == QEvent::KeyPress) {
//        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(ev);
//        Q_INFO << tr("Ate key press %1").arg(keyEvent->key());
//        return QObject::eventFilter(obj, ev);
//    }

//    // pass the event on to the parent class
//    return QObject::eventFilter(obj, ev);
//}
