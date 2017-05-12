#include <QtWidgets>
#include "basicsortfilt.h"
#include "basicsortfilt.h"

BasicSortFilt::BasicSortFilt(QWidget *parent)
	: proxyModel(new QSortFilterProxyModel()) {

	createUi(proxyModel);
	connectUi();
//	setSourceModel(createMailModel(parent));
	show();
}

void BasicSortFilt::createUi(QSortFilterProxyModel *proxyModel) {
	sourceView = new QTreeView;
	sourceView->setRootIsDecorated(false);
	sourceView->setAlternatingRowColors(true);

	proxyView = new QTreeView;
	proxyView->setRootIsDecorated(false);
	proxyView->setAlternatingRowColors(true);
	proxyView->setModel(proxyModel);
	proxyView->setSortingEnabled(true);

	sortCaseSensitivityCheckBox = new QCheckBox(tr("Case sensitive sorting"));
	filterCaseSensitivityCheckBox = new QCheckBox(tr("Case sensitive filter"));

	filterPatternLineEdit = new QLineEdit;
	filterPatternLabel = new QLabel(tr("&Filter pattern:"));
	filterPatternLabel->setBuddy(filterPatternLineEdit);

	filterSyntaxComboBox = new QComboBox;
	filterSyntaxComboBox->addItem(tr("Regular expression"), QRegExp::RegExp);
	filterSyntaxComboBox->addItem(tr("Wildcard"), QRegExp::Wildcard);
	filterSyntaxComboBox->addItem(tr("Fixed string"), QRegExp::FixedString);
	filterSyntaxLabel = new QLabel(tr("Filter &syntax:"));
	filterSyntaxLabel->setBuddy(filterSyntaxComboBox);

	filterColumnComboBox = new QComboBox;
	filterColumnComboBox->addItem(tr("Subject"));
	filterColumnComboBox->addItem(tr("Sender"));
	filterColumnComboBox->addItem(tr("Date"));
	filterColumnLabel = new QLabel(tr("Filter &column:"));
	filterColumnLabel->setBuddy(filterColumnComboBox);

	sourceGroupBox = new QGroupBox(tr("Original Model"));
	proxyGroupBox = new QGroupBox(tr("Sorted/Filtered Model"));

	QHBoxLayout *sourceLayout = new QHBoxLayout;
	sourceLayout->addWidget(sourceView);
	sourceGroupBox->setLayout(sourceLayout);

	QGridLayout *proxyLayout = new QGridLayout;
	proxyLayout->addWidget(proxyView, 0, 0, 1, 3);
	proxyLayout->addWidget(filterPatternLabel, 1, 0);
	proxyLayout->addWidget(filterPatternLineEdit, 1, 1, 1, 2);
	proxyLayout->addWidget(filterSyntaxLabel, 2, 0);
	proxyLayout->addWidget(filterSyntaxComboBox, 2, 1, 1, 2);
	proxyLayout->addWidget(filterColumnLabel, 3, 0);
	proxyLayout->addWidget(filterColumnComboBox, 3, 1, 1, 2);
	proxyLayout->addWidget(filterCaseSensitivityCheckBox, 4, 0, 1, 2);
	proxyLayout->addWidget(sortCaseSensitivityCheckBox, 4, 2);
	proxyGroupBox->setLayout(proxyLayout);

	QVBoxLayout *mainLayout = new QVBoxLayout;

	mainLayout->addWidget(sourceGroupBox);
	mainLayout->addWidget(proxyGroupBox);

	setLayout(mainLayout);

	setWindowTitle(tr("Basic Sort/Filter Model"));
	resize(500, 450);

	proxyView->sortByColumn(1, Qt::AscendingOrder);
	filterColumnComboBox->setCurrentIndex(1);

	filterPatternLineEdit->setText("Andy|Grace");
	filterCaseSensitivityCheckBox->setChecked(true);
	sortCaseSensitivityCheckBox->setChecked(true);
}
void BasicSortFilt::connectUi(void) {

	connect(filterPatternLineEdit, &QLineEdit::textChanged,
			  this, &BasicSortFilt::filterRegExpChanged);

	typedef void (QComboBox::*QComboIntSignal)(int);
	connect(filterSyntaxComboBox,
			  static_cast<QComboIntSignal>(&QComboBox::currentIndexChanged),
			  this, &BasicSortFilt::filterRegExpChanged);
	connect(filterColumnComboBox,
			  static_cast<QComboIntSignal>(&QComboBox::currentIndexChanged),
			  this, &BasicSortFilt::filterColumnChanged);
	connect(filterCaseSensitivityCheckBox, &QAbstractButton::toggled,
			  this, &BasicSortFilt::filterRegExpChanged);
	connect(sortCaseSensitivityCheckBox, &QAbstractButton::toggled,
			  this, &BasicSortFilt::sortChanged);

}
void BasicSortFilt::setSourceModel(QAbstractItemModel *model) {
	proxyModel->setSourceModel(model);
	sourceView->setModel(model);
}
void BasicSortFilt::filterRegExpChanged() {
	QRegExp::PatternSyntax syntax =
		QRegExp::PatternSyntax(filterSyntaxComboBox->itemData(
										  filterSyntaxComboBox->currentIndex()).toInt());
	Qt::CaseSensitivity caseSensitivity =
		filterCaseSensitivityCheckBox->isChecked() ? Qt::CaseSensitive
		: Qt::CaseInsensitive;

	QRegExp regExp(filterPatternLineEdit->text(), caseSensitivity, syntax);
	proxyModel->setFilterRegExp(regExp);
}
void BasicSortFilt::filterColumnChanged() {
	proxyModel->setFilterKeyColumn(filterColumnComboBox->currentIndex());
}
void BasicSortFilt::sortChanged() {
	proxyModel->setSortCaseSensitivity(
		sortCaseSensitivityCheckBox->isChecked() ? Qt::CaseSensitive
		: Qt::CaseInsensitive);
}
void BasicSortFilt::addMail(QAbstractItemModel *model, const QString &subject,
									 const QString &sender, const QDateTime &date) {
	model->insertRow(0);
	model->setData(model->index(0, 0), subject);
	model->setData(model->index(0, 1), sender);
	model->setData(model->index(0, 2), date);
}
QAbstractItemModel *BasicSortFilt::createMailModel(QObject *parent) {
	QStandardItemModel *model = new QStandardItemModel(0, 3, parent);

	model->setHeaderData(0, Qt::Horizontal, QObject::tr("Subject"));
	model->setHeaderData(1, Qt::Horizontal, QObject::tr("Sender"));
	model->setHeaderData(2, Qt::Horizontal, QObject::tr("Date"));

	addMail(model, "Happy New Year!", "Grace K. <grace@software-inc.com>",
			  QDateTime(QDate(2006, 12, 31), QTime(17, 03)));
	addMail(model, "Radically new concept", "Grace K. <grace@software-inc.com>",
			  QDateTime(QDate(2006, 12, 22), QTime(9, 44)));
	addMail(model, "Accounts", "pascale@nospam.com",
			  QDateTime(QDate(2006, 12, 31), QTime(12, 50)));
	addMail(model, "Expenses", "Joe Bloggs <joe@bloggs.com>",
			  QDateTime(QDate(2006, 12, 25), QTime(11, 39)));
	addMail(model, "Re: Expenses", "Andy <andy@nospam.com>",
			  QDateTime(QDate(2007, 01, 02), QTime(16, 05)));
	addMail(model, "Re: Accounts", "Joe Bloggs <joe@bloggs.com>",
			  QDateTime(QDate(2007, 01, 03), QTime(14, 18)));
	addMail(model, "Re: Accounts", "Andy <andy@nospam.com>",
			  QDateTime(QDate(2007, 01, 03), QTime(14, 26)));
	addMail(model, "Sports", "Linda Smith <linda.smith@nospam.com>",
			  QDateTime(QDate(2007, 01, 05), QTime(11, 33)));
	addMail(model, "AW: Sports", "Rolf Newschweinstein <rolfn@nospam.com>",
			  QDateTime(QDate(2007, 01, 05), QTime(12, 00)));
	addMail(model, "RE: Sports", "Petra Schmidt <petras@nospam.com>",
			  QDateTime(QDate(2007, 01, 05), QTime(12, 01)));

	return model;
}
