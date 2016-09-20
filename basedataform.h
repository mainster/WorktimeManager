#ifndef BASEDATAFORM_H
#define BASEDATAFORM_H

#include <QDialog>
#include <QDataWidgetMapper>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QtWidgets>
#include <QSplashScreen>

#include "globals.h"
#include "mdtabview.h"
#include "models.h"
#include "nulldelegate.h"
#include "mdsplashscreen.h"
#include "mdplaintextedit.h"

//#include "browser.h"

class QComboBox;
class QDataWidgetMapper;
class QDateEdit;
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSqlRelationalTableModel;
//class Browser;

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

#define	ID_COLUMN					0
#define	INT_VALIDATOR_UPPER		999999
#define	REAL_VALIDATOR_UPPER		999.99

struct ColumnSchema_t {
	int cid;
	QString name;
	QString type;
	bool notnull;
	QVariant defaultVal;
	int pk;
};
typedef struct ColumnSchema_t ColumnSchema;

class BaseDataForm : public QDialog {

	Q_OBJECT

public:
	struct DataMap {
		DataMap(QWidget *widget, const int column)
			: w(widget), column(column) { }
		QWidget *w;
		int column;
	};

	BaseDataForm(int id, QTableView *tableView, QWidget *parent = 0);

	void done(int result);
	virtual void reject() override;

protected:
	virtual void keyPressEvent(QKeyEvent *e) override;

protected slots:
//	void refreshMapper();
	void populateComboBoxes();

private slots:
	void addNew();
	void deleteCurrent();
	void saveCurrent();
	void onCyclic();
	void clearEditorBackgrounds();

	void onTopButtonClicked(QAbstractButton *button);

private:
	QTableView *m_tableView;
	QSqlRelationalTableModel *tableModel;
//	SqlRtmWithNoneEntry *tableModel;
	QDataWidgetMapper *mapper;

	QList<QComboBox *> cbEditors;
	QList<QLineEdit *> leEditors;
	QList<MdPlainTextEdit *> teEditors;
	QList<QWidget *> allEditors;
	QList<DataMap *> dataMaps;

	QList<QLabel *> lblEditors;
	QLineEdit *lePrimaryKey;

	QButtonGroup *topButtonGroup;
	QPushButton *firstButton, *previousButton, *nextButton, *lastButton, *addButton,
	*deleteButton, *closeButton, *saveButton;

	QStatusBar *stateBar;

	QDialogButtonBox *buttonBox;
	static const QString STYLESHEET, MSG_MANDATORY_EDITOR;

	bool m_rowCountChanged;

};

#endif
