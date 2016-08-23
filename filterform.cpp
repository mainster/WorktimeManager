#include "filterform.h"
#include "ui_filterform.h"

FilterForm *FilterForm::inst = 0;
const QString FilterForm::WINDOW_TITLE_PREFIX = QString("Filtern und Sortieren von");


FilterForm::FilterForm(SourceTableType srcType, TabView *srcTable, QWidget *parent)
	: QWidget(parent), ui(new Ui::FilterForm), mSourceTableType(srcType) {
	inst = this;

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

	setWindowTitle(WINDOW_TITLE_PREFIX);
	resize(500, 900);

	if (srcTable != 0)
		setSourceTabView(srcTable);

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
//void FilterForm::setSourceModel(QAbstractItemModel *model) {
//	proxyModel->setSourceModel(model);
//	sourceView->setModel(model);
//}
void FilterForm::setSourceTabView(TabView *tv) {
	if (sourceTableType() == SourceTableType::useWorktimeSource)
		if (! tv->objectName().contains("worktime"))
			qReturn(QString(tr("SourceTableType::useWorktime but want to set ")
								 + tv->objectName()).toStdString().c_str());
	if (mTv != tv) {
		mTv = tv;
		proxyModel->setSourceModel(mTv->tv()->model());
		sourceView->setModel(mTv->tv()->model());
		setWindowTitle(WINDOW_TITLE_PREFIX + mTv->sqlTableName());
		INFO << tr("New source view [%1] activated!").arg(tv->objectName());
	}
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

//	foreach (QWindow *w, qApp->topLevelWindows())
//		listObjectNames<QWindow *>(qApp->topLevelWindows())

	QWidget::showEvent(e);
}
void FilterForm::hideEvent(QHideEvent *e) {
	QSETTINGS;
	config.setValue(objectName() + Md::k.windowGeometry, saveGeometry());
	QWidget::hideEvent(e);
}
void FilterForm::closeEvent(QCloseEvent *e) {
	QSETTINGS;
	config.setValue(objectName() + Md::k.windowGeometry, saveGeometry());
	INFO << tr("close");
	QWidget::closeEvent(e);
}

void FilterForm::keyPressEvent(QKeyEvent *e) {
	/*!
	 * This event implements a second action key (Esc) to close the filterForm.
	 * The action MainWindow::actFilterForm is configured to show/hide the FilterForm::
	 * instance without invocation of FilterForm::keyPressEvent(). This could simply be
	 * reached by calling the FilterForms memberfunction filterForm::addAction(actFilterForm)
	 * and add the action object to the widgets action list.
	 * To manage propper QAction QShortcut:: operation for a shortcut that differs from
	 * the actFilterForm configured one, it is necessary to trigger the mainwindows QAction
	 * in the keyPressEvent instead of simply invoke this->close().
	 */
	if (e->key() == Qt::Key_Escape) {
		if (! listObjectNames<QAction *>(actions())->join(',').contains("actFilterForm")) {
			WARN << tr("this->actions() does not contain actFilterForm ");
			return QWidget::keyPressEvent(e);
		}
		else
			foreach (QAction *act, actions())
				if (act->objectName().contains(tr("actFilterForm")))
					act->setChecked(false);

		e->accept();
		return;
	}

	QWidget::keyPressEvent(e);
}
