#ifndef INPFRM4_H
#define INPFRM4_H

#include <QObject>
#include <QWidget>
#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#include <QtSql>

#include <QModelIndex>
#include "dbcontroller.h"
#include "globals.h"
#include "mdcombobox.h"
#include "sqlrtm.h"
#include "sfiltmdl.h"
#include "browser.h"
#include "mdtabview.h"

class InpBoxWdg;

/* ======================================================================== */
/*                              class InpFrm4                               */
/* ======================================================================== */
class InpFrm4 : public QWidget {

	Q_OBJECT

public:
	explicit InpFrm4(QWidget *parent = 0);
	static InpFrm4 *instance(QWidget *parent = 0) {
		if (! mInst)
			mInst = new InpFrm4(parent);
		return mInst;
	}
	~InpFrm4() {};

protected:

protected slots:
	void onTextChanged(QString text);
private:
	Browser					*browser;
	SfiltMdl					*prjProxy;
	static InpFrm4			*mInst;
	QList<InpBoxWdg *>	inpBoxes;
};

/* ======================================================================== */
/* ======================================================================== */
/* ======================================================================== */
/*                             class InpBoxWdg                              */
/* ======================================================================== */
class InpBoxWdg : public QGroupBox {

	Q_OBJECT

public:
	explicit InpBoxWdg(const QString &title = QString(), QWidget *parent = 0);
	MdComboBox *cbx() const { return mCbx; }
	static const QString mStylesheetInp4;
	void setTable(MdTable *table) {
		setTitle(table->sqlTableName());
		mCbx->setModel(table->tv()->model());
	}

signals:
	void doAdjustSize();
	void textChanged(QString s);

protected:

protected slots:
	void onDoAdjustSize();

private:
	MdComboBox	*mCbx;
	QLabel		*mLbl;
	QGridLayout	*mGl;
};


#endif // INPFRM4_H
