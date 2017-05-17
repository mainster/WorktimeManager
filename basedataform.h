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
//#include "models.h"
#include "nulldelegate.h"
#include "mdsplashscreen.h"
#include "mdplaintextedit.h"
#include "mdtabview.h"

//#include "browser.h"

class QComboBox;
class QDataWidgetMapper;
class QDateEdit;
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSqlRelationalTableModel;

#define	ID_COLUMN					0
#define	INT_VALIDATOR_UPPER		999999
#define	REAL_VALIDATOR_UPPER		999.99

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
	~BaseDataForm() {
		INFO << tr("close");
	}

	virtual void done(int result) override;
	virtual void reject() override;

protected:
	virtual void keyPressEvent(QKeyEvent *e) override;
	virtual void showEvent(QShowEvent *) override;

	QString getEditorText(QWidget *editor);
protected slots:
//	void refreshMapper();
	void populateComboBoxes();
	void onTextEdited(const QString &text);

private slots:
	void addNew();
	void deleteCurrent();
	void saveCurrent();
	void onCyclic();
	void clearEditorBackgrounds();
	void onTopButtonClicked(QAbstractButton *button);

private:
	void clearResetEditors(QList<QWidget *> editorList = QList<QWidget *>());
	void setButtonsState(QList<QPushButton *> btnList, bool state);
	void disableButtons(QList<QPushButton *> btnList = QList<QPushButton *>());
	void disableButtons(QButtonGroup *btnGroup);
	void enableButtons(QList<QPushButton *> btnList = QList<QPushButton *>());
	void enableButtons(QButtonGroup *btnGroup);

	QTableView *m_tableView;
	QSqlRelationalTableModel *tableModel;
//	SqlRtmWithNoneEntry *tableModel;
	QDataWidgetMapper *mapper;
	QList<MdTableInfo::Column_t> tableSchema;

	QList<QComboBox *> cbEditors;
	QList<QLineEdit *> leEditors;
	QList<MdPlainTextEdit *> teEditors;
	QList<QWidget *> allEditors;
	QList<DataMap *> dataMaps;

	QList<QLabel *> lblEditors;
	QLineEdit *lePrimaryKey;

	QButtonGroup *topButtonGroup;
	QPushButton *btnFirst, *btnPrevious, *btnNext, *btnLast, *btnAdd,
	*btnDelete, *btnClose, *btnSave;

	QStatusBar *stateBar;

	QDialogButtonBox *buttonBox;
	static const QString STYLESHEET, MSG_MANDATORY_EDITOR;

	bool m_rowCountChanged;

};

#endif
