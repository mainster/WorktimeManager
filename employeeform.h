#ifndef EMPLOYEEFORM_H
#define EMPLOYEEFORM_H

#include <QDialog>
#include <QDataWidgetMapper>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QtWidgets>
#include "globals.h"

class QComboBox;
class QDataWidgetMapper;
class QDateEdit;
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSqlRelationalTableModel;

/*
	workerID  INT PRIMARY   KEY   NOT NULL,         -- Primary key
	Nachname                TEXT  NOT NULL UNIQUE,
	Vorname                 TEXT  NOT NULL,
	Einstufung              TEXT  NOT NULL,         -- Meister, Monteur
	PersonalNr              INT   NOT NULL UNIQUE,  -- PersonalNr
	Stundensatz             REAL,                   -- Stundensatz 35.70 €
	Anschrift               CHAR(50),               -- Anschrift
	Gehaltsstufe            INT,                    -- Gehaltsstufe 1, 2, 3
	Wochenstunden           REAL,                   -- Wochenstunden 40h Woche
	Vorschuss               REAL                    -- Bereits geholt
*/
enum {
	Employee_Id = 0,
	Employee_Nachname = 1,
	Employee_Vorname = 2,
	Employee_Einstufung = 3,
	Employee_PersonalNr = 4,
	Employee_Stundensatz = 5,
	Employee_Anschrift = 6,
	Employee_Gehaltsstufe = 7,
	Employee_Wochenstunden = 8,
	Employee_Vorschuss = 9,
};

class EmployeeForm : public QDialog
{
	Q_OBJECT

public:
	EmployeeForm(int id, QAbstractItemModel *model, QWidget *parent = 0);

	void done(int result);

private slots:
	void addEmployee();
	void deleteEmployee();

private:
	QSqlRelationalTableModel *tableModel;
	QDataWidgetMapper *mapper;

	QLabel *lblLastName, *lblFirstName, *lblEinstufung, *lblExtension, *lblHourlyWage,
	*lblAddress, *lblPayGrade, *lblHoursPerWeek, *lblVorschuss;
	QLineEdit *leLastName, *leFirstName, *leEinstufung, *leExtension, *leHourlyWage,
	*leAddress, *lePayGrade, *leHoursPerWeek, *leVorschuss;

//	QComboBox *departmentComboBox;
//	QLineEdit *extensionLineEdit;
//	QLineEdit *emailEdit;
//	QDateEdit *startDateEdit;

	QPushButton *firstButton;
	QPushButton *previousButton;
	QPushButton *nextButton;
	QPushButton *lastButton;
	QPushButton *addButton;
	QPushButton *deleteButton;
	QPushButton *closeButton;
	QDialogButtonBox *buttonBox;
};

#endif
