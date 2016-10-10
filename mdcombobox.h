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


class MdComboBox : public QComboBox {

	Q_OBJECT

public:
	explicit MdComboBox(QWidget *parent = 0)
		: QComboBox(parent) {
	}
	~MdComboBox() {
//		delete tv;
	}

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
			tv = new QTableView(/*parent()*/);
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
	bool setModelColumns(QList<short> columns) {
		if (! static_cast<QSqlRelationalTableModel *>(model()))
			bReturn("modle() should return a QSqlRelationalTableModel!!");

		int colCount = model()->columnCount(QModelIndex()),
				rowCount = model()->rowCount(QModelIndex());

		auto biggest = std::max_element(columns.begin(), columns.end());

		/*!
		 * Check if vector columns contains invalid column values.
		 */
		if (colCount <= *biggest)
			bReturn("Invalid column index found");

//		INFO << rowCount << colCount;

		/*!
		 * Prepare QStringList object stringList which later can be used to create
		 * QStringListModel to source the QCompleter object.
		 */
		QStringList *stringList = new QStringList();
		for (int r = 0; r < rowCount; r++) {
			QStringList *row = new QStringList();
			for (QList<short>::iterator cIt = columns.begin(); cIt != columns.end(); cIt++) {
				*row << model()->data( model()->index(r, *cIt, QModelIndex()) ).toString();
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
				*userData.last() << model()->data( model()->index(r, c, QModelIndex()) ).toString();
			}
		}

		setModel( new QStringListModel(*stringList) );

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

		completer = new QCompleter(*stringList, this);
		completer->setCaseSensitivity(Qt::CaseInsensitive);
		completer->setFilterMode(Qt::MatchContains);
		setCompleter(completer);

//		for (int r = 0; r < rowCount; r++) {
//			INFO << model()->data( model()->index(r, 0, QModelIndex()), Qt::UserRole).toStringList();
//		}

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
	void onShowDropdownList(bool onOff) {
		(onOff) ? showPopup() : hidePopup();
	}

private:
	bool mShowPopups;
	int timerId;
	QTableView *tv;
	QCompleter *completer;
};

#endif // MDCOMBOBOX_H
