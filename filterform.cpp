#include "filterform.h"
#include "ui_filterform.h"

FilterForm *FilterForm::inst = 0;
const QString FilterForm::WINDOW_TITLE_PREFIX = QString("Filter für Tabelle [%1]");
const QString FilterForm::PROXY_GROUPBOX_TITLE_PREFIX = QString("Proxy model [%1 rows]");
const QString FilterForm::SOURCE_GROUPBOX_TITLE_PREFIX = QString("Source model [%1 rows]");


FilterForm::FilterForm(SourceTableType srcType, QList<MdTable *> allTvs, QWidget *parent)
	: QWidget(parent), ui(new Ui::FilterForm), mSourceTableType(srcType) {
	inst = this;
	mTv = NULL;

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
#ifdef VIEW_IS_TREE
	proxyView->setRootIsDecorated(false);
#endif
	proxyView->setAlternatingRowColors(true);
	proxyView->setModel(proxyModel);
	proxyView->setSortingEnabled(true);
	proxyView->sortByColumn(1, Qt::AscendingOrder);

	setWindowTitle(WINDOW_TITLE_PREFIX);
	resize(500, 900);

	foreach (MdTable *tv, allTvs)
		connect (tv, &MdTable::selectedChanged, this, &FilterForm::onSelectedTableChange);

	installEventFilter(this);
}

FilterForm::~FilterForm() {
	delete ui;
}
void FilterForm::setSourceTabView(MdTable *tv) {
	if (sourceTableType() == SourceTableType::useWorktimeSource)
		if (! tv->objectName().contains("worktime"))
			qReturn(QString(tr("SourceTableType::useWorktime but want to set ")
								 + tv->objectName()).toStdString().c_str());

	if (mTv != tv) {
		mTv = tv;
		sourceView->setModel(mTv->tv()->model());
		proxyModel->setSourceModel(mTv->tv()->model());

		setWindowTitle(WINDOW_TITLE_PREFIX.arg(mTv->sqlTableName()));
		onSourceRowCountChanged(0, sourceView->model()->rowCount());


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
	onProxyRowCountChanged(0, proxyModel->rowCount());
}
void FilterForm::onSourceRowCountChanged(const uint oldCount, const uint newCount) {
	Q_UNUSED(oldCount);
	sourceGB->setTitle(SOURCE_GROUPBOX_TITLE_PREFIX.arg(newCount));
}
void FilterForm::onProxyRowCountChanged(const uint oldCount, const uint newCount) {
	Q_UNUSED(oldCount);
	proxyGB->setTitle(PROXY_GROUPBOX_TITLE_PREFIX.arg(newCount));
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

	onProxyRowCountChanged(0, proxyModel->rowCount());
}
void FilterForm::dateFilterChanged() {
	//    qDebug().noquote() << fromDateEdit->date().toString("MM/dd/yy");
	proxyModel->setFilterMinimumDate(fromDateEdit->date());
	proxyModel->setFilterMaximumDate(toDateEdit->date());
	onProxyRowCountChanged(0, proxyModel->rowCount());
}
void FilterForm::onSelectedTableChange(bool selected) {
	MdTable *newTable = qobject_cast<MdTable *>(sender());
	if (! newTable)	qReturn("Cast failed");

	(selected)
			? setSourceTabView(newTable)
			: setSourceTabView(NULL);
}

/* ======================================================================== */
/*                              Event handler                               */
/* ======================================================================== */
void FilterForm::showEvent(QShowEvent *e) {
	QSETTINGS;
	restoreGeometry(config.value(objectName() + Md::k.windowGeometry).toByteArray());

	if (Browser::instance()->mTabs.currentSelected(true) != NULL)
		setSourceTabView(Browser::instance()->mTabs.currentSelected());

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
