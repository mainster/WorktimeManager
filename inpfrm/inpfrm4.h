#ifndef INPFRM4_H
#define INPFRM4_H

#include <QObject>
#include <QWidget>
#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#include <QtSql>

#include <QtWidgets>
#include <QModelIndex>
#include <QVector>
#include "dbcontroller.h"
#include "globals.h"
#include "mdcombobox.h"
#include "sqlrtm.h"
#include "sfiltmdl.h"
#include "browser.h"
#include "mdtabview.h"
#include "mdtable.h"

#include <QDataWidgetMapper>
#include <QCompleter>

class InpBoxWdg;

/* ======================================================================== */
/*                              class InpFrm4                               */
/* ======================================================================== */
class InpFrm4 : public QWidget {

	Q_OBJECT
	Q_PROPERTY(bool hasSrcTables READ hasSrcTables WRITE setHasSrcTables NOTIFY hasSrcTablesChanged)

public:
	explicit InpFrm4(const QList<MdTable *> tables = QList<MdTable *>(),
						  QWidget *parent = nullptr);
	static InpFrm4 *instance(QWidget *parent = nullptr) {
		if (! mInst)
			mInst = new InpFrm4(QList<MdTable *>(), parent);
		return mInst;
	}
	~InpFrm4() { WIN_STORE(this); }

	void setSourceTables(QList<MdTable *> tables);
	bool hasSrcTables() const { return mHasSrcTables; }
	void setHasSrcTables(bool value) {
		mHasSrcTables = value;
		emit hasSrcTablesChanged(value);
	}

signals:
	void hasSrcTablesChanged(bool value);

protected:
	void showEvent(QShowEvent *e);
	void hideEvent(QHideEvent *e);
	void closeEvent(QCloseEvent *e);
	void keyPressEvent(QKeyEvent *e);

protected slots:
	void onTextChanged(const QString &text);

private:
	Browser					*browser;
	SfiltMdl					*prjProxy;
	SqlRtm					*sourceRtm;
	static InpFrm4			*mInst;
	QList<InpBoxWdg *>	inpBoxes;
	QGridLayout				*vbl;
	QCheckBox				*mCb;
	bool						mHasSrcTables;
};


/* ======================================================================== */
/*                             class InpBoxWdg                              */
/* ======================================================================== */
class InpBoxWdg : public QGroupBox {

	Q_OBJECT

public:
	static const QString mStylesheetInp4;

	explicit InpBoxWdg(const QString &title = QString(), QWidget *parent = nullptr);
	MdComboBox *cbx() const { return mCbx; }
	MdTabView *tv() const { return mTv; }
	void setTable(MdTable *table);

signals:
	void doAdjustSize();
	void textChanged(QString s);

protected slots:
	void onDoAdjustSize();
	void onTvModelDataChanged(const QModelIndex /*&topLeft*/,
									  const QModelIndex /*&bottomRight*/,
									  const QVector<int> /*&roles = QVector<int>()*/);

private:
	MdComboBox	*mCbx;
	QCheckBox	*mCheck;
	QLabel		*mLbl;
	QGridLayout	*mGl;
	MdTabView	*mTv;
};


#endif // INPFRM4_H
