#include <QtGui>
#include <QtSql>

#include "basedataform.h"

BaseDataForm::BaseDataForm(int id, QTableView *tableView, QWidget *parent)
	: QDialog(parent), m_tableView(tableView) {
	setObjectName(tr("BaseDataForm_%1")
					  .arg(qobject_cast<MdTabView *>(tableView)->sqlTableName()));
	QSETTINGS;

	/* ======================================================================== */
	/*                          Create button objects                           */
	/* ======================================================================== */
	firstButton = new QPushButton(tr("<<"));
	previousButton = new QPushButton(tr("< &Letzter"));
	nextButton = new QPushButton(tr("&Nächster >"));
	lastButton = new QPushButton(tr(">>"));

	addButton = new QPushButton(tr("&Hinzufügen"));
	deleteButton = new QPushButton(tr("&Löschen"));
	saveButton = new QPushButton(tr("&Anwenden"));
	closeButton = new QPushButton(tr("A&bbruch"));

	firstButton->setFixedWidth(30);
	lastButton->setFixedWidth(30);

	buttonBox = new QDialogButtonBox;
	buttonBox->addButton(addButton, QDialogButtonBox::ResetRole);
	buttonBox->addButton(deleteButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(saveButton, QDialogButtonBox::ApplyRole);
	buttonBox->addButton(closeButton, QDialogButtonBox::RejectRole);

	/* ======================================================================== */
	/*                             Set table model                              */
	/* ======================================================================== */
	tableModel = dynamic_cast<QSqlRelationalTableModel *>(tableView->model());
	tableModel->select();

	mapper = new QDataWidgetMapper(this);
	mapper->setModel(tableModel);
	mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
//	mapper->setItemDelegate(new QSqlRelationalDelegate(this));
//	mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
	mapper->setItemDelegate(new NullDelegate(this));

	/* ======================================================================== */
	/*                        Query a ColumnSchema list                         */
	/* ======================================================================== */
	QList<ColumnSchema> columnSchemas;
	QSqlQuery query(tr("PRAGMA table_info('%1')").arg(
							 qobject_cast<MdTabView *>(tableView)->sqlTableName()));

	while (query.next()) {
		ColumnSchema columnSchema;
		columnSchema.cid			= query.value(query.record().indexOf("cid")).toInt();
		columnSchema.name			= query.value(query.record().indexOf("name")).toString();
		columnSchema.type			= query.value(query.record().indexOf("type")).toString();
		columnSchema.notnull		= query.value(query.record().indexOf("notnull")).toBool();
		columnSchema.defaultVal = query.value(query.record().indexOf("dflt_value"));
		columnSchema.pk			= query.value(query.record().indexOf("pk")).toInt();
		columnSchemas << columnSchema;
	}

	/* ======================================================================== */
	/*                      Create button and main layout                       */
	/* ======================================================================== */
	QHBoxLayout *topButtonLayout = new QHBoxLayout;
	topButtonLayout->setContentsMargins(20, 0, 20, 5);
	topButtonLayout->addStretch();
	topButtonLayout->addWidget(firstButton);
	topButtonLayout->addWidget(previousButton);
	topButtonLayout->addWidget(nextButton);
	topButtonLayout->addWidget(lastButton);
	topButtonLayout->addStretch();

	QGridLayout *mainLayout = new QGridLayout;
	int gridRow = 0;
	mainLayout->addLayout(topButtonLayout, gridRow++, 0, 1, 2);

	int k = 0;
	foreach (ColumnSchema cs, columnSchemas) {
		/*!
		 * Check current table for some foreign key and create a QComboBox.
		 */
		if (tableModel->relationModel(k)) {
			QSqlTableModel *rtm = tableModel->relationModel(k);
			INFO << tr("Column %1 has foreign key for: %2/%3")
					  .arg(k)
					  .arg(rtm->tableName())
					  .arg(tableModel->headerData(k, Qt::Horizontal).toString());

			cbRelations << new QComboBox();
//			cbRelations.last()->addItem(QString(), QVariant());
			lblRelations << new QLabel(tableModel->headerData(k, Qt::Horizontal).toString());
			mapper->addMapping(cbRelations.last(), k);
			cbRelations.last()->setModel(rtm);
			if (lblRelations.last()->text().contains(tr("Name")))
				cbRelations.last()->setModelColumn(rtm->fieldIndex("Name"));
			else
				cbRelations.last()->setModelColumn(0);

			mainLayout->addWidget(lblRelations.last(), gridRow, 0);
			mainLayout->addWidget(cbRelations.last(), gridRow++, 1, 1, 1);
		}
		else {
			leRelations << new QLineEdit(this);
			lblRelations << new QLabel(cs.name);
			lblRelations.last()->setBuddy(leRelations.last());
			mapper->addMapping(leRelations.last(), k);

			mainLayout->addWidget(lblRelations.last(), gridRow, 0);
			mainLayout->addWidget(leRelations.last(), gridRow++, 1, 1, 1);

			/*!
			 * Check if current ColumnSchema holds the primary key.
			 */
			if (cs.pk) {
				leRelations.last()->setEnabled(false);
				lePrimaryKey = leRelations.last();
				k++;
				continue;
			}

			if (cs.type.contains(tr("INT"))) {
				/*!
				 * Check if a config value has been stored in settings ini.
				 */
				int upperValue = config.value(objectName() + tr("/%1").arg(cs.name),
														INT_VALIDATOR_UPPER).toInt();

				if (upperValue != INT_VALIDATOR_UPPER)
					INFO << tr("Validator INT value for: %1 = %2")
							  .arg(objectName() + tr("/%1").arg(cs.name))
							  .arg(upperValue);

				leRelations.last()->setValidator(new QIntValidator(0, upperValue, this));
			}

			if (cs.type.contains(tr("REAL"))) {
				/*!
				 * Check if a config value has been stored in settings ini.
				 */
				qreal upperValue = config.value(objectName() + tr("/%1").arg(cs.name),
														  REAL_VALIDATOR_UPPER).toReal();

				if (upperValue != REAL_VALIDATOR_UPPER)
					INFO << tr("Validator REAL value for: %1 = %2")
							  .arg(objectName() + tr("/%1").arg(cs.name))
							  .arg(upperValue);

				QDoubleValidator *dv = new QDoubleValidator(0, upperValue, 2, this);
				dv->setNotation(QDoubleValidator::StandardNotation);
				leRelations.last()->setValidator(dv);
			}
		}

		k++;
	}

	if (id != -1)
		for (int row = 0; row < tableModel->rowCount(); ++row) {
			QSqlRecord record = tableModel->record(row);
			if (record.value(ID_COLUMN).toInt() == id) {
				mapper->setCurrentIndex(row);
				break;
			}
		}
	else
		mapper->toFirst();

	connect(firstButton,		&QPushButton::clicked, mapper, &QDataWidgetMapper::toFirst);
	connect(previousButton, &QPushButton::clicked, mapper, &QDataWidgetMapper::toPrevious);
	connect(nextButton,		&QPushButton::clicked, mapper, &QDataWidgetMapper::toNext);
	connect(lastButton,		&QPushButton::clicked, mapper, &QDataWidgetMapper::toLast);

	connect(addButton,		&QPushButton::clicked, this, &BaseDataForm::addNew);
	connect(deleteButton,	&QPushButton::clicked, this, &BaseDataForm::deleteCurrent);
	connect(saveButton,		&QPushButton::clicked, this, &BaseDataForm::saveCurrent);
	connect(closeButton,		&QPushButton::clicked, this, &BaseDataForm::reject);

	mainLayout->addWidget(buttonBox, gridRow, 0, 1, 2);
	//	mainLayout->setRowMinimumHeight(6, 10);
	//		mainLayout->setRowStretch(6, 1);
	//		mainLayout->setColumnStretch(2, 1);
	setLayout(mainLayout);

	if (id == -1)
		nextButton->setFocus();
	else
		leRelations.first()->setFocus();


	setWindowTitle(tr("Stammdaten bearbeiten: %1")
						.arg(Md::tableAlias[ qobject_cast<MdTabView *>(
							tableView)->sqlTableName() ]));

//	QTimer *tim = new QTimer(this);
//	tim->setInterval(500);
//	connect(tim, &QTimer::timeout, this, &BaseDataForm::onCyclic);
//	tim->start();
}
void BaseDataForm::onCyclic() {
//	qobject_cast<SqlRtm *>(tableModel)->select();
}
void BaseDataForm::addNew() {
	qobject_cast<SqlRtm *>(m_tableView->model())->revertAll();

	int row = mapper->currentIndex() + 1;
	//	mapper->submit();
	tableModel->insertRow(row);

	foreach(QLineEdit *le, leRelations)
		le->clear();

	foreach(QComboBox *cb, cbRelations)
		cb->setCurrentIndex(0);

	mapper->setCurrentIndex(row);
	lePrimaryKey->setText(QString::number(row + 1, 10));

	qobject_cast<QGridLayout *>(
				layout())->itemAtPosition(layout()->indexOf(lePrimaryKey), 1)
			->widget()->setFocus();

//	Browser *browser = Browser::instance();
//	emit browser->sqlTableDataChanged();
}
void BaseDataForm::deleteCurrent() {
	int row = mapper->currentIndex();
	tableModel->removeRow(row);
	mapper->submit();
	mapper->setCurrentIndex(qMin(row, tableModel->rowCount() - 1));
}
void BaseDataForm::saveCurrent() {
	mapper->submit();
	static_cast<SqlRtm *>(m_tableView->model())->submitAll();
//	Browser *browser = Browser::instance();
//	emit browser->sqlTableDataChanged();

}
void BaseDataForm::done(int result) {
	mapper->submit();
	static_cast<SqlRtm *>(m_tableView->model())->submitAll();
	QDialog::done(result);
}
void BaseDataForm::reject() {
	mapper->revert();
	static_cast<SqlRtm *>(m_tableView->model())->revertAll();
	QDialog::reject();
}
void BaseDataForm::keyPressEvent(QKeyEvent *e) {
	if (e->key() == Qt::Key_F5) {
		static_cast<SqlRtm *>(m_tableView->model())->select();
		static_cast<SqlRtm *>(mapper->model())->select();

		//			MdSplashScreen *splash = new MdSplashScreen(QPixmap(1920,1080));
		//			splash->show();

		//			splash->showMessage(QString("Processing..."),
		//									  Qt::AlignVCenter | Qt::AlignHCenter, Qt::red);

		//			QTimer::singleShot(3000, splash, &MdSplashScreen::clearMessage);
	}
}


