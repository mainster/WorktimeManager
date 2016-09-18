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

	leEinstufung = new QLineEdit(this);
	lblEinstufung = new QLabel(tr("&Einstufung:"));
	lblEinstufung->setBuddy(leEinstufung);

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

	leVorschuss = new QLineEdit(this);
	leVorschuss->setValidator(new QIntValidator(-9999, 9999, this));
	lblVorschuss = new QLabel(tr("&Vorschuss:"));
	lblVorschuss->setBuddy(leVorschuss);

	firstButton = new QPushButton(tr("<< &Anfang"));
	previousButton = new QPushButton(tr("< &Letzter"));
	nextButton = new QPushButton(tr("&Nächster >"));
	lastButton = new QPushButton(tr("&Ende >>"));

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

	//    QSqlTableModel *relationModel =
	//            tableModel->relationModel(Employee_DepartmentId);
	//    departmentComboBox->setModel(relationModel);
	//    departmentComboBox->setModelColumn(
	//            relationModel->fieldIndex("name"));

	mapper = new QDataWidgetMapper(this);
	mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
	mapper->setModel(tableModel);
	mapper->setItemDelegate(new QSqlRelationalDelegate(this));
	mapper->addMapping(leLastName, Employee_Nachname);
	mapper->addMapping(leFirstName, Employee_Vorname);
	mapper->addMapping(leEinstufung, Employee_Einstufung);
	mapper->addMapping(leExtension, Employee_PersonalNr);
	mapper->addMapping(leHourlyWage, Employee_Stundensatz);
	mapper->addMapping(lePayGrade, Employee_Gehaltsstufe);
	mapper->addMapping(leHoursPerWeek, Employee_Wochenstunden);
	mapper->addMapping(leVorschuss, Employee_Vorschuss);

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
	mainLayout->addLayout(topButtonLayout, 0, 0, 1, 3);
	mainLayout->addWidget(lblLastName, 1, 0);
	mainLayout->addWidget(leLastName, 1, 1, 1, 2);
	mainLayout->addWidget(lblFirstName, 2, 0);
	mainLayout->addWidget(leFirstName, 2, 1, 1, 2);
	mainLayout->addWidget(lblEinstufung, 3, 0);
	mainLayout->addWidget(leEinstufung, 3, 1, 1, 2);
	mainLayout->addWidget(lblExtension, 4, 0);
	mainLayout->addWidget(leExtension, 4, 1);
	mainLayout->addWidget(lblHourlyWage, 5, 0);
	mainLayout->addWidget(leHourlyWage, 5, 1, 1, 2);
	mainLayout->addWidget(lblPayGrade, 6, 0);
	mainLayout->addWidget(lePayGrade, 6, 1);

	mainLayout->addWidget(buttonBox, 7, 0, 1, 3);
	mainLayout->setRowMinimumHeight(6, 10);
	mainLayout->setRowStretch(6, 1);
	mainLayout->setColumnStretch(2, 1);
	setLayout(mainLayout);

	if (id == -1) {
		nextButton->setFocus();
	} else {
		leLastName->setFocus();
	}

	setWindowTitle(tr("Edit Employees"));
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
