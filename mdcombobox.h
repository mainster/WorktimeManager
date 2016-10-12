#ifndef MDCOMBOBOX_H
#define MDCOMBOBOX_H

#include <QObject>
#include <QWidget>
#include <QComboBox>
#include <QVariant>
#include <QGroupBox>
#include <QTableView>
#include <QSqlRelationalTableModel>
#include <algorithm>
#include <QCompleter>
#include <QLineEdit>
#include <QStringListModel>

#include "globals.h"
#include "debug.h"
#include "sfiltmdl.h"
#include "sqlrtm.h"

class MdComboBoxV2 : public QComboBox {

	Q_OBJECT

public:
	explicit MdComboBoxV2(QWidget *parent = 0)
		: QComboBox(parent),
		  mColsCfg(QList<short>()),
		  listMdl(new QStringListModel()),
		  proxyMdl(new SfiltMdl()) {
		proxyMdl->setDynamicSortFilter(true);
		proxyMdl->setFilterCaseSensitivity(Qt::CaseInsensitive);
	}
	~MdComboBoxV2() {
		delete listMdl;
		delete proxyMdl;
	}

	virtual void setModel(QAbstractItemModel *model_) {
		inputMdl = model_;
		if (qobject_cast<SqlRtm *>(inputMdl))
			proxyMdl->setSourceModel(qobject_cast<SqlRtm *>(inputMdl));

		if (qobject_cast<SfiltMdl *>(inputMdl))
			proxyMdl->setSourceModel(qobject_cast<SfiltMdl *>(inputMdl));

		proxyMdl->setFilterRegExp("*");

		refreshListModel("*");
		QComboBox::setModel(listMdl);
	}
	bool setModelColumns(QList<short> columns) {
		if (! static_cast<QSqlRelationalTableModel *>(model()))
			bReturn("modle() should return a QSqlRelationalTableModel!!");

		mColsCfg = columns;

		return true;
	}
	QStringList &columnMerge(SfiltMdl *model) {
		int colCount = model->columnCount(QModelIndex()),
				rowCount = model->rowCount(QModelIndex());

		QList<short int>::iterator biggest =
				std::max_element(mColsCfg.begin(), mColsCfg.end());

		/*!
		 * Check if vector columColsCfgmns contains invalid column values.
		 */
		if (colCount <= *biggest)
			CRIT << tr("Invalid column index found");

		/*!
		 * Prepare QStringList object stringList which later can be used to create
		 * QStringListModel to source the QCompleter object.
		 */
		QStringList *stringList = new QStringList();

		for (int r = 0; r < rowCount; r++) {
			QStringList *row = new QStringList();

			for (QList<short>::iterator cIt = mColsCfg.begin(); cIt != mColsCfg.end(); cIt++) {
				*row << model->data( model->index(r, *cIt, QModelIndex()) ).toString();
			}

			*stringList << row->join(QString(", "));
		}

		/*!
		 * Prepare QStringList list object QList<QStringList> userData which later can be
		 * used as user data for QComboBox items.
		 */
		QList<QStringList *> userData;
		for (int r = 0; r < rowCount; r++) {
			userData.append( new QStringList() );

			for (int c = 0; c < colCount; c++) {
				*userData.last() << model->data(model->index(r, c, QModelIndex())).toString();
			}
		}

		return *stringList;
	}

public slots:
	void refreshListModel(const QString &filterString) {
		proxyMdl->setFilterRegExp(filterString);
		QStringList sl = columnMerge(proxyMdl);
		listMdl->setStringList(sl);
	}

protected:
	virtual void timerEvent(QTimerEvent *e) override {
		lineEdit()->selectAll();
		QObject::killTimer(e->timerId());
	}
	virtual void focusInEvent(QFocusEvent *e) override {
		if (isEditable())
			lineEdit()->setSelection(1,3);
		setEditable(true);
		QObject::startTimer(1);
		QComboBox::focusInEvent(e);
	}

protected slots:

private:
	int timerId;
	QList<short>			mColsCfg;
	QPointer<SqlRtm>		sqlRtmMdl;
	QPointer<SfiltMdl>	sfiltMdl;
	QAbstractItemModel	*inputMdl;
	QStringListModel		*listMdl;
	SfiltMdl					*proxyMdl;

#ifdef USE_COMPLETER
	QCompleter			*completer;
#endif
};






class MdComboBox : public QComboBox {

	Q_OBJECT

public:
	explicit MdComboBox(QWidget *parent = 0)
		: QComboBox(parent), mColsCfg(QList<short>()) { }
	~MdComboBox() { }

	virtual void setModel(QAbstractItemModel *model_) {
		if (qobject_cast<SqlRtm *>(model_)) {
			sqlRtmMdl = qobject_cast<SqlRtm *>(model_);
			QComboBox::setModel( sqlRtmMdl.data() );
			INFO << YEL_BG("SqlRtm");
			return;
		}

		if (qobject_cast<SfiltMdl *>(model_)) {
			sfiltMdl = qobject_cast<SfiltMdl *>(model_);
			QComboBox::setModel( sfiltMdl.data() );
			INFO << YEL_BG("SfiltMdl");
		}

		INFO << GN_BG(model()->metaObject()->className());

	}

	bool setModelColumns(QList<short> columns) {
		if (! static_cast<QSqlRelationalTableModel *>(model()))
			bReturn("modle() should return a QSqlRelationalTableModel!!");

		mColsCfg = columns;

		/*!
		 * Set a new QStringListModel from stringList as model for the QComboBox object.
		QList<QStringList *>::iterator it = userData.begin();
		for (int r = 0; r < rowCount; r++) {
			QVariant var;
			var.setValue(**(it++));
			INFO << var.toStringList();
			INFO << model()->index(r, 0, QModelIndex()).isValid();
			INFO << model()->setData( model()->index(r, 0, QModelIndex()), var, Qt::DisplayRole);

			INFO << model()->index(r, 0, QModelIndex()).data(Qt::DisplayRole).toStringList();
			INFO << model()->data( model()->index(r, 0, QModelIndex()), Qt::DisplayRole).toStringList();
		}
		*/

#ifdef USE_COMPLETER
		completer = new QCompleter(*stringList, this);
		completer->setCaseSensitivity(Qt::CaseInsensitive);
		completer->setFilterMode(Qt::MatchContains);
		setCompleter(completer);
#endif
		return true;
	}
	QStringListModel *transformModel(QAbstractItemModel *model) {
		int colCount = model->columnCount(QModelIndex()),
				rowCount = model->rowCount(QModelIndex());

		//		 QList<short int>::iterator biggest =
		//				 std::max_element(mColsCfg.begin(), mColsCfg.end());

		//		/*!
		//		 * Check if vector columColsCfgmns contains invalid column values.
		//		 */
		//		if (colCount <= *biggest)
		//			bReturn("Invalid column index found");

		/*!
		 * Prepare QStringList object stringList which later can be used to create
		 * QStringListModel to source the QCompleter object.
		 */
		QStringList *stringList = new QStringList();
		for (int r = 0; r < rowCount; r++) {
			QStringList *row = new QStringList();

			for (QList<short>::iterator cIt = mColsCfg.begin(); cIt != mColsCfg.end(); cIt++) {
				*row << model->data( model->index(r, *cIt, QModelIndex()) ).toString();
			}

			*stringList << row->join(QString(", "));
		}

		/*!
		 * Prepare QStringList list object QList<QStringList> userData which later can be
		 * used as user data for QComboBox items.
		 */
		QList<QStringList *> userData;
		for (int r = 0; r < rowCount; r++) {
			userData.append( new QStringList() );

			for (int c = 0; c < colCount; c++) {
				*userData.last() << model->data(model->index(r, c, QModelIndex())).toString();
			}
		}

		return new QStringListModel(*stringList);
	}

public slots:
	void refreshModel(QAbstractItemModel *model) {
		setModel(transformModel(model));
	}

protected:
	virtual void timerEvent(QTimerEvent *e) override {
		lineEdit()->selectAll();
		QObject::killTimer(e->timerId());
	}
	virtual void focusInEvent(QFocusEvent *e) override {
		if (isEditable())
			lineEdit()->setSelection(1,3);
		setEditable(true);
		QObject::startTimer(1);
		QComboBox::focusInEvent(e);
	}

protected slots:

private:
	int timerId;
	QList<short>			mColsCfg;
	QPointer<SqlRtm>		sqlRtmMdl;
	QPointer<SfiltMdl>	sfiltMdl;

#ifdef USE_COMPLETER
	QCompleter			*completer;
#endif
};


#ifdef USE_COMPLETER
		completer = new QCompleter(*stringList, this);
		completer->setCaseSensitivity(Qt::CaseInsensitive);
		completer->setFilterMode(Qt::MatchContains);
		setCompleter(completer);
#endif

#endif // MDCOMBOBOX_H

