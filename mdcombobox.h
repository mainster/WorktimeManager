#ifndef MDCOMBOBOX_H
#define MDCOMBOBOX_H

#include <QObject>
#include <QWidget>
#include <QComboBox>
#include <QVariant>
#include "inpfrm.h"

class MdComboBox : public QComboBox {

	Q_OBJECT

public:
	explicit MdComboBox(QWidget *parent = 0) : QComboBox(parent) {}
	~MdComboBox() {}

	void update() {
		style()->unpolish(this);
		style()->polish(this);
		QComboBox::update();
	}

signals:


protected:
	virtual void focusInEvent(QFocusEvent *e) override {
		setProperty("hasFocus", true);
		update();
		QComboBox::focusInEvent(e);
	}
	virtual void focusOutEvent(QFocusEvent *e) override {
		setProperty("hasFocus", false);
		update();
		QComboBox::focusOutEvent(e);
	}

};

#endif // MDCOMBOBOX_H
