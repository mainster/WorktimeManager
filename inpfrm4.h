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

#include <QDataWidgetMapper>

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
	SqlRtm					*sourceRtm;
	static InpFrm4			*mInst;
	QList<InpBoxWdg *>	inpBoxes;
	QDataWidgetMapper *mapper;

};


/* ======================================================================== */
/* ======================================================================== */
/* ======================================================================== */
/*                             class InpBoxWdg                              */
/* ======================================================================== */
class InpBoxWdg : public QGroupBox {

	Q_OBJECT

public:
	explicit InpBoxWdg(const QString &title = QString(),
							 QWidget *parent = 0) : QGroupBox(parent) {
		mCbx = new MdComboBoxV2();
		mLbl = new QLabel(tr("spacer"));
		mGl = new QGridLayout();
		mTv = new MdTabView(QString());

		mGl->addWidget(mCbx, 0, 1, 1, 1);
		mGl->addWidget(mLbl, 1, 1, 1, 1);
		mGl->addWidget(mTv, 2, 1, -1, 1);

		mLbl->setAlignment(Qt::AlignCenter);
		mLbl->setFrameShape(QFrame::Box);
		mLbl->setMaximumHeight(30);
		setLayout(mGl);
		setTitle(title);

		connect(this, &InpBoxWdg::doAdjustSize, this, &InpBoxWdg::onDoAdjustSize);

		adjustSize();
		setStyleSheet(mStylesheetInp4);
	}
	MdComboBoxV2 *cbx() const { return mCbx; }
	MdTabView *tv() const { return mTv; }

	static const QString mStylesheetInp4;
	void setTable(MdTable *table) {
		setTitle(table->sqlTableName());
		mCbx->setModel(table->tv()->model());
		mTv->setModel(table->tv()->model());

		connect(mTv->model(), &QAbstractItemModel::dataChanged,
				  this, &InpBoxWdg::onTvModelDataChanged);
	}

signals:
	void doAdjustSize();
	void textChanged(QString s);

protected:

protected slots:
	void onTvModelDataChanged(const QModelIndex /*&topLeft*/, const QModelIndex /*&bottomRight*/,
									  const QVector<int> /*&roles = QVector<int>()*/) {
		mCbx->setModel(mTv->model());
	}
	void onDoAdjustSize() {
		mCbx->adjustSize();
	}

private:
	MdComboBoxV2	*mCbx;
	QLabel		*mLbl;
	QGridLayout	*mGl;
	MdTabView	*mTv;
};


#endif // INPFRM4_H
