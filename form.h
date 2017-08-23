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
	~Form() { /*delete ui;*/ }

private slots:
	void onEditDone();

protected:
	void keyPressEvent(QKeyEvent *e) override;

private:
	Ui::Form		*ui;
	QTextEdit	*txtEdit;

};

#endif // FORM_H
