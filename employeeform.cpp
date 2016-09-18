#include <QtGui>
#include <QtSql>

#include "employeeform.h"

EmployeeForm::EmployeeForm(int id, QAbstractItemModel *model, QWidget *parent)
	: QDialog(parent) {
	leLastName = new QLineEdit(this);
	lblLastName = new QLabel(tr("&Nachname:"));
	lblLastName->setBuddy(leLastName);

	leFirstName = new QLineEdit(this);
	lblFirstName = new QLabel(tr("&Vorname:"));
	lblFirstName->setBuddy(leFirstName);

//	leEinstufung = new QLineEdit(this);
//	lblEinstufung = new QLabel(tr("&Einstufung:"));
//	lblEinstufung->setBuddy(leEinstufung);

	leExtension = new QLineEdit(this);
	leExtension->setValidator(new QIntValidator(-99, 9999, this));
	lblExtension = new QLabel(tr("&PersonalNr:"));
	lblExtension->setBuddy(leExtension);

	QSETTINGS;
	qreal maxHourlyWage =
			config.value(objectName() + Md::k.maxHourlyWage, -1).toReal();

	leHourlyWage = new QLineEdit(this);
	QDoubleValidator *dv;

	if (maxHourlyWage > 0)
		dv = new QDoubleValidator(0.0, maxHourlyWage, 2, this);
	else
		dv = new QDoubleValidator(0.0, 999.0, 2, this);

	dv->setNotation(QDoubleValidator::StandardNotation);
	leHourlyWage->setValidator(dv);

	lblHourlyWage = new QLabel(tr("&Stundensatz:"));
	lblHourlyWage->setBuddy(leHourlyWage);

	leAddress = new QLineEdit(this);
	lblAddress = new QLabel(tr("&Anschrift:"));
	lblAddress->setBuddy(leAddress);

	lePayGrade = new QLineEdit(this);
	lePayGrade->setValidator(new QIntValidator(1, 9, this));
	lblPayGrade = new QLabel(tr("&Gehaltsstufe:"));
	lblPayGrade->setBuddy(lePayGrade);

	leHoursPerWeek = new QLineEdit(this);
	lblHoursPerWeek = new QLabel(tr("&Wochenstunden:"));
	lblHoursPerWeek->setBuddy(leHoursPerWeek);

//	leVorschuss = new QLineEdit(this);
//	leVorschuss->setValidator(new QIntValidator(-9999, 9999, this));
//	lblVorschuss = new QLabel(tr("&Vorschuss:"));
//	lblVorschuss->setBuddy(leVorschuss);

	firstButton = new QPushButton(tr("<<"));
	previousButton = new QPushButton(tr("< &Letzter"));
	nextButton = new QPushButton(tr("&Nächster >"));
	lastButton = new QPushButton(tr(">>"));

	addButton = new QPushButton(tr("&Add"));
	deleteButton = new QPushButton(tr("&Delete"));
	closeButton = new QPushButton(tr("&Close"));

	buttonBox = new QDialogButtonBox;
	buttonBox->addButton(addButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(deleteButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(closeButton, QDialogButtonBox::AcceptRole);

//	tableModel = new QSqlRelationalTableModel(this);
//	tableModel->setTable("worker");
//	//    tableModel->setRelation(Employee_DepartmentId,
//	//                            QSqlRelation("department", "id", "name"));
//	tableModel->setSort(Employee_Nachname, Qt::AscendingOrder);
//	tableModel->select();
	tableModel = dynamic_cast<QSqlRelationalTableModel *>(model);
	tableModel->select();

	mapper = new QDataWidgetMapper(this);
	mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
	mapper->setModel(tableModel);
	mapper->setItemDelegate(new QSqlRelationalDelegate(this));
	mapper->addMapping(leLastName, Employee_Nachname);
	mapper->addMapping(leFirstName, Employee_Vorname);
//	mapper->addMapping(leEinstufung, Employee_Einstufung);
	mapper->addMapping(leExtension, Employee_PersonalNr);
	mapper->addMapping(leHourlyWage, Employee_Stundensatz);
	mapper->addMapping(lePayGrade, Employee_Gehaltsstufe);
	mapper->addMapping(leHoursPerWeek, Employee_Wochenstunden);

	/*!
	 * Check current table for some foreign key and create a QComboBox.
	 */
	for (int k = 0; k < tableModel->rowCount(QModelIndex()); k++)
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
		}

	if (id != -1) {
		for (int row = 0; row < tableModel->rowCount(); ++row) {
			QSqlRecord record = tableModel->record(row);

			if (record.value(Employee_Id).toInt() == id) {
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
	mainLayout->addLayout(topButtonLayout, gridRow++, 0, 1, 3);
	mainLayout->addWidget(lblLastName, gridRow, 0);
	mainLayout->addWidget(leLastName, gridRow++, 1, 1, 2);
	mainLayout->addWidget(lblFirstName, gridRow, 0);
	mainLayout->addWidget(leFirstName, gridRow++, 1, 1, 2);

	for (int cbs = 0; cbs < cbRelations.count(); cbs++) {
		mainLayout->addWidget(lblRelations.at(cbs), gridRow, 0);
		mainLayout->addWidget(cbRelations.at(cbs), gridRow++, 1, 1, 2);
	}
	mainLayout->addWidget(lblExtension, gridRow, 0);
	mainLayout->addWidget(leExtension, gridRow++, 1);
	mainLayout->addWidget(lblHourlyWage, gridRow, 0);
	mainLayout->addWidget(leHourlyWage, gridRow++, 1, 1, 2);
	mainLayout->addWidget(lblPayGrade, gridRow, 0);
	mainLayout->addWidget(lePayGrade, gridRow++, 1);

	mainLayout->addWidget(buttonBox, gridRow, 0, 1, 3);
//	mainLayout->setRowMinimumHeight(6, 10);
//	mainLayout->setRowStretch(6, 1);
//	mainLayout->setColumnStretch(2, 1);
	setLayout(mainLayout);

	if (id == -1) {
		nextButton->setFocus();
	} else {
		leLastName->setFocus();
	}

	setWindowTitle(tr("Mitarbeiter-Stammdaten bearbeiten"));
}

void EmployeeForm::done(int result) {
	mapper->submit();
	QDialog::done(result);
}

void EmployeeForm::addEmployee() {
	int row = mapper->currentIndex();
	mapper->submit();
	tableModel->insertRow(row);
	mapper->setCurrentIndex(row);

	foreach(QLineEdit * le,
			  findChildren<QLineEdit *>(QString(), Qt::FindDirectChildrenOnly))
		le->clear();

	//    startDateEdit->setDate(QDate::currentDate());
	leLastName->setFocus();
}

void EmployeeForm::deleteEmployee() {
	int row = mapper->currentIndex();
	tableModel->removeRow(row);
	mapper->submit();
	mapper->setCurrentIndex(qMin(row, tableModel->rowCount() - 1));
}
