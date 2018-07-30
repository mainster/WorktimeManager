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

#define USE_COMPLETER


class MdComboBox : public QComboBox {

	Q_OBJECT

public:

#ifdef USE_COMPLETER

	explicit MdComboBox(QWidget *parent = nullptr);

#else

	explicit MdComboBox(QWidget *parent = nullptr)
		: QComboBox(parent), listMdl(new QStringListModel()) { }

#endif

	~MdComboBox() { }

	virtual void setModel(QAbstractItemModel *inputModel);

	void setModelColumns(QList<short> columns);
	QCompleter *getCompleter() const;

public slots:
	bool refreshView();

protected:
	virtual void timerEvent(QTimerEvent *e) override;
	virtual void focusInEvent(QFocusEvent *e) override;

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
