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
//	static const struct stylesSheets_t {
//		QString redBoarder;
//	} styleSheets;

	explicit MdComboBox(QWidget *parent = 0) : QComboBox(parent) {}

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
	void update() {
		style()->unpolish(this);
		style()->polish(this);
		QComboBox::update();
	}
};

#endif // MDCOMBOBOX_H
