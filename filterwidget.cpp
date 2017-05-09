#include "filterwidget.h"

const QString FilterWidget::WINDOW_TITLE_PREFIX = QString("Filter für Tabelle [%1]");
const QString FilterWidget::PROXY_GROUPBOX_TITLE_PREFIX = QString("Proxy model [%1 rows]");
const QString FilterWidget::SOURCE_GROUPBOX_TITLE_PREFIX = QString("Source model [%1 rows]");

/* ======================================================================== */
/*                               FilterWidget                               */
/* ======================================================================== */
FilterWidget::FilterWidget(SourceTableType srcType, QList<MdTable *> allTbls,
									QWidget *parent) : QWidget(parent),
	mSourceTableType(srcType) {
	mTbl = NULL;

	/*!
	 * Main widget instantiation.
	 */
	mainLayout =	new QVBoxLayout();
	sourceLayout = new QGridLayout();
	sourceGB =		new QGroupBox(tr("Source"));
	sourceView =	new MdTabView(QString(), 0);

	proxyModel =	new SfiltMdl(this);
	proxyModel->setDynamicSortFilter(true);
	proxyGB =		new QGroupBox(tr("Proxy"));
	//	proxyView =		new MdTabView(proxyModel, 0);
	proxyView =		new MdTabView(QString(), 0);
	proxyLayout =	new QGridLayout();

	/*!
	 * Input widgets
	 */
	filtCaseSensCB = new QCheckBox();
	filtPattLe = new QLineEdit();
	filtPattLbl = new QLabel(tr("&Filter pattern:"));
	filtPattLbl->setBuddy(filtPattLe);

	filtSyntaxCB = new QComboBox();
	filtSyntaxCB->addItem(tr("Regular expression"), QRegExp::RegExp);
	filtSyntaxCB->addItem(tr("Wildcard"), QRegExp::Wildcard);
	filtSyntaxCB->addItem(tr("Fixed string"), QRegExp::FixedString);

	fromDateEdit = new QDateEdit();
	toDateEdit = new QDateEdit();
	fromLabel = new QLabel(tr("F&rom:"));
	fromLabel->setBuddy(fromDateEdit);
	toLabel = new QLabel(tr("&To:"));
	toLabel->setBuddy(toDateEdit);
	filtPattLbl = new QLabel(tr("&Filter pattern:"));
	filtPattLbl->setBuddy(filtPattLe);

	/*!
	 * Configure all QDateEdit's.
	 */
	QList<QDateEdit *> dateEdits;
	dateEdits << fromDateEdit << toDateEdit;
	foreach (QDateEdit *de, dateEdits) {
		de->setDisplayFormat("dd.MM.yyyy");
		de->setCalendarPopup(true);
		de->setLocale(QLocale::German);
		de->calendarWidget()->setLocale(QLocale::German);
	}


	if (srcType == FilterWidget::useWorktimeSource)
		foreach (MdTable *tbl, allTbls)
			if (tbl->sqlTableName().contains("worktime")) {
				setSourceTable(tbl);
			}
	/*!
	 * Connect Signals/Slots.
	 */
	connect(filtPattLe,     SIGNAL(textChanged(QString)),
			  this,           SLOT(textFilterChanged()));
	connect(filtSyntaxCB,   SIGNAL(currentIndexChanged(int)),
			  this,           SLOT(textFilterChanged()));
	connect(filtCaseSensCB, SIGNAL(toggled(bool)),
			  this,           SLOT(textFilterChanged()));
	connect(fromDateEdit,   SIGNAL(dateChanged(QDate)),
			  this,           SLOT(dateFilterChanged()));
	connect(toDateEdit,     SIGNAL(dateChanged(QDate)),
			  this,           SLOT(dateFilterChanged()));

	/*!
	 * Source layout definition
	 */
	sourceLayout->addWidget(sourceView,		0, 0, 1, 3);
	sourceGB->setLayout(sourceLayout);

	/*!
	 * Proxy layout definition
	 */
	proxyLayout->addWidget(proxyView,		0, 0, 1, 3);
	proxyLayout->addWidget(filtPattLbl,		1, 0, 1, 1);
	proxyLayout->addWidget(filtPattLe,		1, 1, 1, 1);
	proxyLayout->addWidget(filtCaseSensCB, 1, 2, 1, 1);
	proxyLayout->addWidget(fromLabel,		2, 0, 1, 1);
	proxyLayout->addWidget(fromDateEdit,	2, 1, 1, 1);
	proxyLayout->addWidget(filtSyntaxCB,	2, 2, 1, 1);
	proxyLayout->addWidget(toLabel,			3, 0, 1, 1);
	proxyLayout->addWidget(toDateEdit,		3, 1, 1, 1);
	proxyGB->setLayout(proxyLayout);

#define USE_SPLITTER
#ifdef USE_SPLITTER
	splitter	= new QSplitter(Qt::Vertical);
	splitter->addWidget(sourceGB);
	splitter->addWidget(proxyGB);
	mainLayout->addWidget(splitter);
#else
	mainLayout->addWidget(sourceGB);
	mainLayout->addWidget(proxyGB);
#endif

	/*!
	 * Init the application clipboard.
	 */
	clipboard = qApp->clipboard();

	proxyView->setAlternatingRowColors(true);
	proxyView->setModel(proxyModel);
	proxyView->setSortingEnabled(true);
	proxyView->sortByColumn(1, Qt::AscendingOrder);

	setWindowTitle(WINDOW_TITLE_PREFIX);
	resize(500, 900);

	foreach (MdTable *tbl, allTbls)
		connect (tbl, &MdTable::selectedChanged, this, &FilterWidget::onSelectedTableChange);

	/*!
	 * Pass main layout object to this's addLayout methode.
	 */
	setLayout(mainLayout);

	QTimer::singleShot(100, this, &FilterWidget::restoreWidgetStates);

	//	actSumSelection = new QAction(tr("Summiere Auswahl"), this);
	//	actSumSelection->setShortcut(QKeySequence("Ctrl+Shift+a"));
	////	actSumSelection->setShortcutContext(Qt::WidgetShortcut);
	//	connect(actSumSelection, &QAction::triggered, this, &FilterWidget::sumSelection);
	//	addAction(actSumSelection);
}
FilterWidget::~FilterWidget() {
	QSETTINGS;
	config.setValue(objectName() + Md::k.windowGeometry, saveGeometry());
	//	delete ui;
}
void FilterWidget::onFilterPatternTextChanged(/*const QString &text*/) {
	QString text = filtPattLe->text();

	if (text == tr("delete")) {
		delete sourceView;
		filtPattLe->clear();
	}

	if (text == tr("insert")) {
		sourceView = new MdTabView("worktime", 0);
		mainLayout->addWidget(sourceView);
		filtPattLe->clear();
	}

	if (text == tr("create")) {
		sourceView = new MdTabView("worktime", 0);
		filtPattLe->clear();
	}
}
/* ======================================================================== */
/*                             Callback handler                             */
/* ======================================================================== */
QList<QString> FilterWidget::copy(bool overwriteClipboardBuffer) {
	QItemSelectionModel * selection = sourceView->selectionModel();
	QModelIndexList indexes = selection->selectedIndexes();

	if(indexes.size() < 1)
		return QList<QString>();

	// QModelIndex::operator < sorts first by row, then by column.
	// this is what we need
	qSort(indexes);
	QStringList fields;

	foreach (QModelIndex idx, indexes)
		fields << idx.data(Qt::DisplayRole).toString();

	if (overwriteClipboardBuffer)
		clipboard->setText(fields.join(' '));
	else
		if (clipboard->text(QClipboard::Clipboard).isEmpty())
			clipboard->setText(fields.join(' '));

	return fields;
}
void FilterWidget::paste() {
	INFO << tr("paste");
	return;
}
void FilterWidget::sumSelection() {
	bool ok = true;
	QItemSelectionModel *selection = sourceView->selectionModel();
	QModelIndexList indexes = selection->selectedIndexes();

	if(indexes.size() < 1)
		return;

	// QModelIndex::operator < sorts first by row, then by column.
	// this is what we need
	qSort(indexes);

	QList<qreal> numerics;

	foreach (QModelIndex idx, indexes) {
		qreal field = idx.data(Qt::DisplayRole).toReal(&ok);
		if (! ok)
			selection->select(idx, QItemSelectionModel::Deselect);
		else
			numerics << field;
	}

	qreal sum = 0;
	foreach (qreal v, numerics)
		sum += v;

	INFO << sum;
}
void FilterWidget::restoreWidgetStates() {
	QSETTINGS;
	INFO << objectName() + Md::k.filtSyntaxCB;
	INFO << config.value(objectName() + Md::k.filtSyntaxCB, 0).toInt();
	INFO << config.value(objectName() + Md::k.minDateEdit, QDate::currentDate()).toDate();
	INFO << config.value(objectName() + Md::k.maxDateEdit, QDate::currentDate()).toDate();
	INFO << config.value(objectName() + Md::k.filtCaseSensCB, false).toBool();

	filtSyntaxCB->setCurrentIndex(config.value(objectName() + Md::k.filtSyntaxCB, 0).toInt());
	fromDateEdit->setDate(config.value(objectName() + Md::k.minDateEdit,
												  QDate::currentDate().addYears(-2)).toDate());
	toDateEdit->setDate(config.value(objectName() + Md::k.maxDateEdit,
												QDate::currentDate().addYears(2)).toDate());
	filtCaseSensCB->setChecked(config.value(objectName() + Md::k.filtCaseSensCB, false).toBool());

	if (config.value(objectName() + Md::k.windowGeometry).isValid())
		restoreGeometry(config.value(objectName() + Md::k.windowGeometry).toByteArray());

	/*!
	 * Try to add some of MdTabViews QAction objects to this widget. This is necessary
	 * if we want to use QKeySequence:: functions natively.
	 */
	//	QAction *act = sourceView->getActSumSelection();
	//	INFO << act;
	//	INFO << act->shortcut().toString();

}
void FilterWidget::setSourceTable(MdTable *tbl) {
	if (sourceTableType() == SourceTableType::useWorktimeSource)
		if (! tbl->sqlTableName().contains("worktime"))
			qReturn(QString(tr("SourceTableType::useWorktime but want to set ")
								 + tbl->sqlTableName()).toStdString().c_str());

	if (mTbl != tbl) {
		mTbl = tbl;
		sourceView->setModel(mTbl->tv()->model());

		while (sourceView->model()->canFetchMore(QModelIndex()))
			sourceView->model()->fetchMore(QModelIndex());

		/*!
		 * Clear models header QMap
		 */
		if (proxyModel->headIdxs())
			proxyModel->headIdxs()->map.clear();

		proxyModel->setSourceModel(mTbl->tv()->model());
		setWindowTitle(WINDOW_TITLE_PREFIX.arg(mTbl->tv()->sqlTableName()));
		onSourceRowCountChanged(0, sourceView->model()->rowCount());

		INFO << tr("New source view [%1] activated!").arg(tbl->sqlTableName());
	}
}
void FilterWidget::textFilterChanged() {
	QSETTINGS;
	QRegExp::PatternSyntax syntax =
			QRegExp::PatternSyntax(filtSyntaxCB->itemData(
											  filtSyntaxCB->currentIndex()).toInt());
	config.setValue(objectName() + Md::k.filtSyntaxCB,
						 filtSyntaxCB->itemData(filtSyntaxCB->currentIndex()).toInt());

	Qt::CaseSensitivity caseSensitivity =
			filtCaseSensCB->isChecked()
			? Qt::CaseSensitive
			: Qt::CaseInsensitive;
	config.setValue(objectName() + Md::k.filtCaseSensCB, caseSensitivity);

	QRegExp regExp(filtPattLe->text(), caseSensitivity, syntax);
	proxyModel->setFilterRegExp(regExp);
	onProxyRowCountChanged(0, proxyModel->rowCount());
}
void FilterWidget::onSourceRowCountChanged(const uint oldCount, const uint newCount) {
	Q_UNUSED(oldCount);
	sourceGB->setTitle(SOURCE_GROUPBOX_TITLE_PREFIX.arg(newCount));
}
void FilterWidget::onProxyRowCountChanged(const uint oldCount, const uint newCount) {
	Q_UNUSED(oldCount);
	proxyGB->setTitle(PROXY_GROUPBOX_TITLE_PREFIX.arg(newCount));
}
void FilterWidget::cbTextFilterChanged() {
	QSETTINGS;
	QRegExp::PatternSyntax syntax =
			QRegExp::PatternSyntax(filtSyntaxCB->itemData(
											  filtSyntaxCB->currentIndex()).toInt());
	config.setValue(objectName() + Md::k.filtSyntaxCB,
						 filtSyntaxCB->itemData(filtSyntaxCB->currentIndex()).toInt());

	Qt::CaseSensitivity caseSensitivity =
			filtCaseSensCB->isChecked()
			? Qt::CaseSensitive
			: Qt::CaseInsensitive;
	config.setValue(objectName() + Md::k.filtCaseSensCB, caseSensitivity);


	QRegExp regExp(filtPattLe->text(), caseSensitivity, syntax);
	proxyModel->setFilterRegExp(regExp);

	onProxyRowCountChanged(0, proxyModel->rowCount());
}
void FilterWidget::dateFilterChanged() {
	QSETTINGS;
	//    qDebug().noquote() << fromDateEdit->date().toString("MM/dd/yy");
	proxyModel->setFilterMinDate(fromDateEdit->date());
	proxyModel->setFilterMaxDate(toDateEdit->date());
	config.setValue(objectName() + Md::k.minDateEdit, fromDateEdit->date());
	config.setValue(objectName() + Md::k.maxDateEdit, toDateEdit->date());
	onProxyRowCountChanged(0, proxyModel->rowCount());
}
void FilterWidget::onSelectedTableChange(bool selected) {
	MdTable *changedTable = qobject_cast<MdTable *>(sender());
	if (! changedTable)
		qReturn("Cast failed");

	if ((mSourceTableType != FilterWidget::useWorktimeSource) &&
		 (! changedTable->sqlTableName().contains("worktime"))) {
		if (selected)
			setSourceTable(changedTable);
		else
			setSourceTable(NULL);
	}
}
/* ======================================================================== */
/*                              Event handler                               */
/* ======================================================================== */
void FilterWidget::showEvent(QShowEvent *e) {
	QSETTINGS;
	restoreGeometry(config.value(objectName() + Md::k.windowGeometry).toByteArray());

	if (Browser::instance()->mTabs.currentSelected(true) != NULL)
		setSourceTable(Browser::instance()->mTabs.currentSelected());

	QWidget::showEvent(e);
}
void FilterWidget::hideEvent(QHideEvent *e) {
	QSETTINGS;
	config.setValue(objectName() + Md::k.windowGeometry, saveGeometry());
	INFO << objectName() + Md::k.windowGeometry << tr("close");
	QWidget::hideEvent(e);
}
void FilterWidget::closeEvent(QCloseEvent *e) {
	QSETTINGS;
	config.setValue(objectName() + Md::k.windowGeometry, saveGeometry());
	QWidget::closeEvent(e);
}
void FilterWidget::keyPressEvent(QKeyEvent *e) {
	/*!
	 * This event implements a second action key (Esc) to close the filterForm.
	 * The action MainWindow::actFilterForm is configured to show/hide the FilterWidget::
	 * instance without invocation of FilterWidget::keyPressEvent(). This could simply be
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


/* ======================================================================== */
/* ======================================================================== */
/* ======================================================================== */
/* ======================================================================== */
FilterEdit::FilterEdit(QWidget *parent)
	: QLineEdit(parent)
	, m_patternGroup(new QActionGroup(this))
{
	setClearButtonEnabled(true);
	connect(this, &QLineEdit::textChanged, this, &FilterEdit::filterChanged);

	QMenu *menu = new QMenu(this);
	m_caseSensitivityAction = menu->addAction(tr("Case Sensitive"));
	m_caseSensitivityAction->setCheckable(true);
	connect(m_caseSensitivityAction, &QAction::toggled, this, &FilterEdit::filterChanged);

	menu->addSeparator();
	m_patternGroup->setExclusive(true);
	QAction *patternAction = menu->addAction("Fixed String");
	patternAction->setData(QVariant(int(QRegExp::FixedString)));
	patternAction->setCheckable(true);
	patternAction->setChecked(true);
	m_patternGroup->addAction(patternAction);
	patternAction = menu->addAction("Regular Expression");
	patternAction->setCheckable(true);
	patternAction->setData(QVariant(int(QRegExp::RegExp2)));
	m_patternGroup->addAction(patternAction);
	patternAction = menu->addAction("Wildcard");
	patternAction->setCheckable(true);
	patternAction->setData(QVariant(int(QRegExp::Wildcard)));
	m_patternGroup->addAction(patternAction);
	connect(m_patternGroup, &QActionGroup::triggered, this, &FilterEdit::filterChanged);

	const QIcon icon = QIcon(QPixmap(":/images/find.png"));
	QToolButton *optionsButton = new QToolButton;
#ifndef QT_NO_CURSOR
	optionsButton->setCursor(Qt::ArrowCursor);
#endif
	optionsButton->setFocusPolicy(Qt::NoFocus);
	optionsButton->setStyleSheet("* { border: none; }");
	optionsButton->setIcon(icon);
	optionsButton->setMenu(menu);
	optionsButton->setPopupMode(QToolButton::InstantPopup);

	QWidgetAction *optionsAction = new QWidgetAction(this);
	optionsAction->setDefaultWidget(optionsButton);
	addAction(optionsAction, QLineEdit::LeadingPosition);
}
Qt::CaseSensitivity FilterEdit::caseSensitivity() const
{
	return m_caseSensitivityAction->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
}
void FilterEdit::setCaseSensitivity(Qt::CaseSensitivity cs)
{
	m_caseSensitivityAction->setChecked(cs == Qt::CaseSensitive);
}
static inline QRegExp::PatternSyntax patternSyntaxFromAction(const QAction *a)
{
	return static_cast<QRegExp::PatternSyntax>(a->data().toInt());
}
QRegExp::PatternSyntax FilterEdit::patternSyntax() const
{
	return patternSyntaxFromAction(m_patternGroup->checkedAction());
}
void FilterEdit::setPatternSyntax(QRegExp::PatternSyntax s)
{
	foreach (QAction *a, m_patternGroup->actions()) {
		if (patternSyntaxFromAction(a) == s) {
			a->setChecked(true);
			break;
		}
	}
}


/* ======================================================================== */
/* ======================================================================== */
/* ======================================================================== */
/* ======================================================================== */

CustFilterWindow::CustFilterWindow()
{
	proxyModel = new MySortFilterProxyModel(this);

	sourceView = new QTreeView;
	sourceView->setRootIsDecorated(false);
	sourceView->setAlternatingRowColors(true);

	QHBoxLayout *sourceLayout = new QHBoxLayout;

	sourceLayout->addWidget(sourceView);
	sourceGroupBox = new QGroupBox(tr("Original Model"));
	sourceGroupBox->setLayout(sourceLayout);

	filterEdit = new FilterEdit();
	filterEdit->setText("Grace|Sports");
	connect(filterEdit, &FilterEdit::filterChanged,
			  this, &CustFilterWindow::textFilterChanged);

	filterPatternLabel = new QLabel(tr("&Filter pattern:"));
	filterPatternLabel->setBuddy(filterEdit);

	fromDateEdit = new QDateEdit;
	fromDateEdit->setDate(QDate(1970, 01, 01));
	fromLabel = new QLabel(tr("F&rom:"));
	fromLabel->setBuddy(fromDateEdit);

	toDateEdit = new QDateEdit;
	toDateEdit->setDate(QDate(2099, 12, 31));
	toLabel = new QLabel(tr("&To:"));
	toLabel->setBuddy(toDateEdit);

	connect(filterEdit, &QLineEdit::textChanged,
			  this, &CustFilterWindow::textFilterChanged);
	connect(fromDateEdit, &QDateTimeEdit::dateChanged,
			  this, &CustFilterWindow::dateFilterChanged);
	connect(toDateEdit, &QDateTimeEdit::dateChanged,
			  this, &CustFilterWindow::dateFilterChanged);

	proxyView = new QTreeView;
	proxyView->setRootIsDecorated(false);
	proxyView->setAlternatingRowColors(true);
	proxyView->setModel(proxyModel);
	proxyView->setSortingEnabled(true);
	proxyView->sortByColumn(1, Qt::AscendingOrder);

	cbx = new QComboBox;
	cbx->setModel(proxyModel);


	QGridLayout *proxyLayout = new QGridLayout;
	proxyLayout->addWidget(proxyView, 0, 0, 1, 3);
	proxyLayout->addWidget(filterPatternLabel, 1, 0);
	proxyLayout->addWidget(filterEdit, 1, 1);
	proxyLayout->addWidget(fromLabel, 3, 0);
	proxyLayout->addWidget(fromDateEdit, 3, 1, 1, 2);
	proxyLayout->addWidget(toLabel, 4, 0);
	proxyLayout->addWidget(toDateEdit, 4, 1, 1, 2);
	proxyLayout->addWidget(cbx, 5, 1, 1, 2);

	proxyGroupBox = new QGroupBox(tr("Sorted/Filtered Model"));
	proxyGroupBox->setLayout(proxyLayout);
	//! [5]

	//! [6]
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(sourceGroupBox);
	mainLayout->addWidget(proxyGroupBox);
	setLayout(mainLayout);

	setWindowTitle(tr("Custom Sort/Filter Model"));
	resize(500, 450);
}
void CustFilterWindow::setSourceModel(QAbstractItemModel *model)
{
	proxyModel->setSourceModel(model);
	sourceView->setModel(model);
}
void CustFilterWindow::textFilterChanged()
{
	QRegExp regExp(filterEdit->text(),
						filterEdit->caseSensitivity(),
						filterEdit->patternSyntax());
	proxyModel->setFilterRegExp(regExp);
}
void CustFilterWindow::dateFilterChanged()
{
	proxyModel->setFilterMinimumDate(fromDateEdit->date());
	proxyModel->setFilterMaximumDate(toDateEdit->date());
}
