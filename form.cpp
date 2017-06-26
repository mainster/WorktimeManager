#include "form.h"
#include "ui_form.h"



Form::Form(QWidget *parent) : QWidget(parent), ui(new Ui::Form) {
	ui->setupUi(this);
	txtEdit = new QTextEdit(0);
	txtEdit->hide();

	connect(txtEdit, &QTextEdit::textChanged, this, &Form::onEditDone);

	ui->cbx->addItems(QList<QString>() << tr("menu") << tr("item") << tr("Ende")
							<< tr("No 554155")<< tr("No 55424244155") << tr("No 554155"));
	ui->cbx->setEditable(true);

}

void Form::onEditDone() {
	if (txtEdit->toPlainText().contains("<DONE>")) {
		QString ss = txtEdit->toPlainText().remove("\n<DONE>");
		INFO << ss;
		Globals::setStylesheet(this, ss);
		txtEdit->hide();
	}
}
void Form::keyPressEvent(QKeyEvent *e) {
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
