#include "form.h"
#include "ui_form.h"



Form::Form(QWidget *parent) : QWidget(parent),
	ui(new Ui::Form) {
	ui->setupUi(this);
	txtEdit = new QTextEdit(0);
	txtEdit->hide();

	connect(txtEdit, &QTextEdit::textChanged, this, &Form::onEditDone);

	ui->cbx->addItems(QList<QString>()
							<< tr("menu")
							<< tr("item")
							<< tr("Ende"));

}

Form::~Form() {
	delete ui;
}
