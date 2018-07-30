#ifndef MDPLAINTEXTEDIT_H
#define MDPLAINTEXTEDIT_H

#include <QWidget>
#include <QObject>
#include <QPlainTextEdit>
#include <QLineEdit>

class MdPlainTextEdit : public QPlainTextEdit {

	Q_OBJECT

public:
	explicit MdPlainTextEdit(QWidget *parent = nullptr)
		: QPlainTextEdit(parent) {
		connect(this, &MdPlainTextEdit::textChanged, this, &MdPlainTextEdit::onTextChanged);
	}
	~MdPlainTextEdit() {}

protected slots:
	void onTextChanged() {

		QSize size = document()->size().toSize();
		size.setHeight(parentWidget()->findChildren<QLineEdit *>().first()->height() * 3);
		size.setWidth(QWIDGETSIZE_MAX);
		setMaximumSize(size);
	}
};

#endif // MDPLAINTEXTEDIT_H
