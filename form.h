#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QLineEdit>
#include <QVector>
#include <QPushButton>
#include <QTimer>
#include <QObject>
#include <QTableView>
#include <QTextEdit>
#include "globals.h"

namespace Ui {
class Form;
}

class Form : public QWidget {

	Q_OBJECT

public:
	explicit Form(QWidget *parent = 0);
	~Form();

private slots:
	void onEditDone() {
		if (txtEdit->toPlainText().contains("<DONE>")) {
			QString ss = txtEdit->toPlainText().remove("\n<DONE>");
			INFO << ss;
			Globals::setStylesheet(this, ss);
			txtEdit->hide();
		}
	}

protected:
	void keyPressEvent(QKeyEvent *e) override {
		if (e->key() == Qt::Key_Escape) {
			QWidget::close();
			e->accept();
			return;
		}
		if (e->key() == Qt::Key_F4) {
			foreach (QWidget *w, findChildren<QWidget *>())
				w->setEnabled(!w->isEnabled());

			e->accept();
			return;
		}
		if (e->key() == Qt::Key_F2) {
			txtEdit->clear();
			txtEdit->insertPlainText(QWidget::styleSheet());
			txtEdit->show();
			e->accept();
			return;
		}
		QWidget::keyPressEvent(e);
	}

private:
	Ui::Form		*ui;
	QTextEdit	*txtEdit;

};

#endif // FORM_H
