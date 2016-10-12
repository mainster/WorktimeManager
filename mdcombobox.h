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

#include "globals.h"
#include "debug.h"

/*!
 * do-while() loop is a trick to let you define multi-statement macros and
 * call them like functions. Note the lack of trailing ';'
 */
#define qReturn(msg)		do { WARN << QString(msg); return; } while(0)
#define bReturn(msg)		do { WARN << QString(msg); return false; } while(0)
#define vReturn(msg)		do { WARN << QString(msg); return QVariant(); } while(0)
#define milReturn(msg)	do { WARN << QString(msg); return QModelIndexList(); } while(0)

//#define USE_COMPLETER


class MdComboBox : public QComboBox {

	Q_OBJECT

public:
#ifdef USE_COMPLETER
	explicit MdComboBox(QWidget *parent = 0)
		: QComboBox(parent), listMdl(new QStringListModel()),
		  mCompleter(new QCompleter(this)) {
	}
#else
	explicit MdComboBox(QWidget *parent = 0)
		: QComboBox(parent), listMdl(new QStringListModel()) { }
#endif
	~MdComboBox() { }

	virtual void setModel(QAbstractItemModel *inputModel) {
		INFO << GN_BG(inputModel->metaObject()->className());

		inputMdl = inputModel;
	}
	void setModelColumns(QList<short> columns) {
		if (! static_cast<QSqlRelationalTableModel *>(model()))
			qReturn("modle() should return a QSqlRelationalTableModel!!");

		mColCfg = columns;
		refreshView();
	}
	QCompleter *getCompleter() const {
#ifdef USE_COMPLETER
		return mCompleter;
#else
		return QComboBox::completer();
#endif
	}

public slots:
	bool refreshView() {
		int colCount = inputMdl->columnCount(),
				rowCount = inputMdl->rowCount();

		QList<short int>::iterator biggest =
				std::max_element(mColCfg.begin(), mColCfg.end());

		/*!
		 * Check if vector columns contains invalid column values.
		 */
		if (colCount <= *biggest)
			bReturn("Invalid column index found");

		/*!
		 * Prepare QStringList object stringList which later can be used to create
		 * QStringListModel to source the QCompleter object.
		 */
		QStringList *stringList = new QStringList();
		for (int r = 0; r < rowCount; r++) {
			QStringList *row = new QStringList();

			for (QList<short>::iterator cIt = mColCfg.begin(); cIt != mColCfg.end(); cIt++) {
				*row << inputMdl->data( inputMdl->index(r, *cIt, QModelIndex()) ).toString();
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
				*userData.last() << inputMdl->data(inputMdl->index(r, c, QModelIndex())).toString();
			}
		}
		delete listMdl;
		QComboBox::setModel( listMdl = new QStringListModel(*stringList) );

#ifdef USE_COMPLETER
		delete mCompleter;
		mCompleter = new QCompleter(*stringList);
		mCompleter->setCaseSensitivity(Qt::CaseInsensitive);
		mCompleter->setFilterMode(Qt::MatchContains);
		setCompleter(mCompleter);
#endif
		return true;
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
	QTableView *tv;
	QStringListModel *listMdl;
	QList<short> mColCfg;
	QCompleter *mCompleter;
	QAbstractItemModel *inputMdl;
};

/*
 *
 * #ifdef USE_COMPLETER
		completer = new QCompleter(*stringList, this);
		completer->setCaseSensitivity(Qt::CaseInsensitive);
		completer->setFilterMode(Qt::MatchContains);
		setCompleter(completer);
#endif


void showPopup(bool holdOpen = false)  {
	mShowPopups = holdOpen;
	QComboBox::showPopup();
	if (holdOpen)
		this->setFocusPolicy(Qt::NoFocus);
}
void hidePopup() override {
	if (! mShowPopups)
		QComboBox::hidePopup();
}

void makePermanentView(bool onOff = true) {
	if (onOff) {
		tv = new QTableView();
		INFO << QComboBox::modelColumn();
		tv->setModel( QComboBox::model() );
		QComboBox::setModelColumn(QComboBox::modelColumn());
		tv->show();
	}
	else {
		tv->hide();
		delete tv;
	}
}
*/

#endif // MDCOMBOBOX_H
