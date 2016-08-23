#include "filterform.h"
#include "ui_filterform.h"

FilterForm *FilterForm::inst = 0;

FilterForm::FilterForm(QWidget *parent, TabView *srcTable) :
	QWidget(parent), ui(new Ui::FilterForm) {

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
	proxyGB = ui->proxyGB;

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

	setWindowTitle(tr("Filtern und Sortieren von <SQL-table-name>"));
	resize(500, 900);

	if (srcTable != 0)
		setSourceModel(srcTable->tv()->model());

	installEventFilter(this);
	/* ======================================================================== */
	//	QSortFilterProxyModel *sfmodel = new QSortFilterProxyModel(this);
	//	QCompleter *completer = new QCompleter();
	//	sfmodel->setSourceModel();
	//	completer->setModel();
	//	completer->setCompletionColumn();

	//	filtPattCb = ui->cbFilterPattern;
	//	filtPattCb->setEditable(true);
	//	filtPattCb->setModel(proxyModel);
	//	filtPattCb->setModelColumn(2);
	//	ui->cbFilterPattern->setP

	//    installEventFilter(this);
}

FilterForm::~FilterForm() {
	delete ui;
}
void FilterForm::setSourceModel(QAbstractItemModel *model) {
	proxyModel->setSourceModel(model);
	sourceView->setModel(model);
}
void FilterForm::textFilterChanged() {
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
void FilterForm::cbTextFilterChanged() {
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
void FilterForm::dateFilterChanged() {
	//    qDebug().noquote() << fromDateEdit->date().toString("MM/dd/yy");

	proxyModel->setFilterMinimumDate(fromDateEdit->date());
	proxyModel->setFilterMaximumDate(toDateEdit->date());
}
/* ======================================================================== */
/*                              Event handler                               */
/* ======================================================================== */
void FilterForm::showEvent(QShowEvent *e) {
	QSETTINGS;
	restoreGeometry(config.value(objectName() + Md::k.windowGeometry).toByteArray());
	INFO << tr("Show");
	QWidget::showEvent(e);
}
void FilterForm::hideEvent(QHideEvent *e) {
	QSETTINGS;
	config.setValue(objectName() + Md::k.windowGeometry, saveGeometry());
	INFO << tr("hide");
	QWidget::hideEvent(e);
}
void FilterForm::closeEvent(QCloseEvent *e) {
	QSETTINGS;
	config.setValue(objectName() + Md::k.windowGeometry, saveGeometry());
	INFO << tr("close");
	QWidget::closeEvent(e);
}

void FilterForm::keyPressEvent(QKeyEvent *e) {
	INFO << Qt::Key(e->key());
	INFO << qApp->topLevelWindows();

	QMainWindow *p;
	foreach (QWindow *w, qApp->topLevelWindows()) {
		if (w->objectName()==tr("MainWindow"))
			p = qobject_cast<QMainWindow *>(w);
	}

	QWidget::keyPressEvent(e);
}
