#include <QtGui>
#include <QtSql>

#include "basedataform.h"

BaseDataForm::BaseDataForm(int id, QTableView *tableView, QWidget *parent)
	: QDialog(parent) {
	setObjectName(tr("BaseDataForm_%1")
					  .arg(qobject_cast<MdTabView *>(tableView)->sqlTableName()));
	QSETTINGS;

	firstButton = new QPushButton(tr("<<"));
	previousButton = new QPushButton(tr("< &Letzter"));
	nextButton = new QPushButton(tr("&Nächster >"));
	lastButton = new QPushButton(tr(">>"));

	addButton = new QPushButton(tr("&Add"));
	deleteButton = new QPushButton(tr("&Delete"));
	closeButton = new QPushButton(tr("&Close"));

	firstButton->setFixedWidth(40);
	lastButton->setFixedWidth(40);

	buttonBox = new QDialogButtonBox;
	buttonBox->addButton(addButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(deleteButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(closeButton, QDialogButtonBox::AcceptRole);

	tableModel = dynamic_cast<QSqlRelationalTableModel *>(tableView->model());
	tableModel->select();

	mapper = new QDataWidgetMapper(this);
	mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
	mapper->setModel(tableModel);
	mapper->setItemDelegate(new QSqlRelationalDelegate(this));

	/*!
	 * Query a string list of table column names.
	 */
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
			INFO << tr("Column %1 has foreign key for table %2")
					  .arg(k).arg(rtm->tableName());
			INFO << tableModel->headerData(k, Qt::Horizontal);

			cbRelations << new QComboBox();
			lblRelations << new QLabel(tableModel->headerData(k, Qt::Horizontal).toString());
			mapper->addMapping(cbRelations.last(), k);
			cbRelations.last()->setModel(rtm);
			cbRelations.last()->setModelColumn(1);

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

			if (cs.type.contains(tr("INT"))) {
				/*!
				 * Check if a config value has been stored in settings ini.
				 */
				INFO << tr("Validator INT value for: %1 = %2")
						  .arg(objectName() + tr("/%1").arg(cs.name))
						  .arg(config.value(objectName() + tr("/%1").arg(cs.name),999).toInt());
				leRelations.last()->setValidator(
							new QIntValidator(0, config.value(objectName() + tr("/%1").arg(cs.name),
																		 999).toInt(), this));
			}
			if (cs.type.contains(tr("REAL"))) {
				/*!
				 * Check if a config value has been stored in settings ini.
				 */
				INFO << tr("Validator REAL value for: %1 = %2")
						  .arg(objectName() + tr("/%1").arg(cs.name))
						  .arg(config.value(objectName() + tr("/%1").arg(cs.name),999).toReal());
				QDoubleValidator *dv =
						new QDoubleValidator(0, config.value(objectName() + tr("/%1").arg(cs.name),
																		 999).toReal(), 2, this);
				dv->setNotation(QDoubleValidator::StandardNotation);
				leRelations.last()->setValidator(dv);
			}
		}

		k++;
	}

	if (id != -1) {
		 for (int row = 0; row < tableModel->rowCount(); ++row) {
			  QSqlRecord record = tableModel->record(row);
			  if (record.value(ID_COLUMN).toInt() == id) {
					mapper->setCurrentIndex(row);
					break;
			  }
		 }
	}
	else
		mapper->toFirst();

	connect(firstButton, SIGNAL(clicked()), mapper, SLOT(toFirst()));
	connect(previousButton, SIGNAL(clicked()), mapper, SLOT(toPrevious()));
	connect(nextButton, SIGNAL(clicked()), mapper, SLOT(toNext()));
	connect(lastButton, SIGNAL(clicked()), mapper, SLOT(toLast()));
	connect(addButton, SIGNAL(clicked()), this, SLOT(addEmployee()));
	connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteEmployee()));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));

	mainLayout->addWidget(buttonBox, gridRow, 0, 1, 2);
	//	mainLayout->setRowMinimumHeight(6, 10);
	//	mainLayout->setRowStretch(6, 1);
	//	mainLayout->setColumnStretch(2, 1);
	setLayout(mainLayout);

	if (id == -1)
		nextButton->setFocus();
	else
		leRelations.first()->setFocus();

	setWindowTitle(tr("Mitarbeiter-Stammdaten bearbeiten"));
}

void BaseDataForm::done(int result) {
	mapper->submit();
	QDialog::done(result);
}

void BaseDataForm::addEmployee() {
	int row = mapper->currentIndex();
	mapper->submit();
	tableModel->insertRow(row);
	mapper->setCurrentIndex(row);

	foreach(QLineEdit *le, leRelations)
		le->clear();

	foreach(QComboBox *cb, cbRelations)
		cb->setCurrentIndex(0);

	leRelations.first()->setFocus();
}

void BaseDataForm::deleteEmployee() {
	int row = mapper->currentIndex();
	tableModel->removeRow(row);
	mapper->submit();
	mapper->setCurrentIndex(qMin(row, tableModel->rowCount() - 1));
}
