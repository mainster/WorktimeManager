#ifndef MDCOMBOBOX_H
#define MDCOMBOBOX_H

#include <QObject>
#include <QWidget>
#include <QComboBox>
#include <QVariant>
#include <QGroupBox>
#include <QTableView>

#include "globals.h"
#include "debug.h"

class MdComboBox : public QComboBox {

	Q_OBJECT

public:
	explicit MdComboBox(QWidget *parent = 0)
		: QComboBox(parent) {}
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
	bool setModelColumns(QList<quint8>columns) {
		/** Check if columns contains invalid column values */
		qSort(columns.begin(), columns.end());
		int colCount = static_cast<MdComboBox*>(model())->columnCount(QModelIndex()),
				rowCount = static_cast<MdComboBox*>(model())->rowCount(QModelIndex());

		if (columns.last() >= colCount) {
			INFO << tr("Invalid column index found");
			return false;
		}

		INFO << rowCount << colCount;

		QStringList stringList;
		for (QList<quint8>::iterator cIt = columns.begin(); cIt != columns.end(); cIt++) {
			for (int r = 0; r < rowCount; r++) {
				stringList << model()->data( model()->index(r, *cIt, QModelIndex()) ).toString();
			}
		}
		INFO << stringList;

//		QStringListModel *stringListModel = ;
		setModel( new QStringListModel(stringList) );
		return true;
	}

protected:
	virtual void timerEvent(QTimerEvent *) override {
		hidePopup();
		showPopup();
	}

protected slots:
	void onShowDropdownList(bool onOff) {
		(onOff) ? showPopup() : hidePopup();
	}

private:
	bool mShowPopups;
	int timerId;
	QTableView *tv;
//	static QTableView *tv;
};

#endif // MDCOMBOBOX_H
