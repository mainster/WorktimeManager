#include "mdnotes.h"
#include "ui_mdnotes.h"

MDNotes::MDNotes(const QString settingsKey, QWidget *parent)
	: QFrame(parent), ui(new Ui::MDNotes), mSettingsKey(settingsKey) {
	ui->setupUi(this);

	ui->groupBox->setTitle(objectName() + tr("/") + settingsKey);
	ui->pbDeleteLast->setIcon(QIcon(":icoDelete"));
	ui->pbOk->setIcon(QIcon(":/images/greenCheck.png"));

	/*!
	 * Restore text edit contents.
	 */
	QSETTINGS;
	if (! config.allKeys().join(',').contains(mSettingsKey))
		config.setValue(objectName() + tr("/") + mSettingsKey,
							 ui->textEdit->document()->toHtml());

	ui->textEdit->setHtml(config.value(objectName() + tr("/") + mSettingsKey).toString());

	/*!
	 * Create and connect action objects.
	 */
	createActions();
	connectActions();
}

MDNotes::~MDNotes() {
	delete ui;
}
void MDNotes::onOkClicked(bool clicked) {
	QSETTINGS;
	Q_UNUSED(clicked);
	config.setValue(objectName() + tr("/") + mSettingsKey,
						 ui->textEdit->document()->toHtml());
	config.sync();
	emit settingsSaved(objectName() + tr(": Note added"));
}
void MDNotes::onDeleteLastClicked(bool clicked) {
	QSETTINGS;
	Q_UNUSED(clicked);

	deleteLastLines(1);
	config.setValue(objectName() + tr("/") + mSettingsKey,
						 ui->textEdit->document()->toHtml());
	config.sync();
}
QString MDNotes::settingsKey() const {
	return mSettingsKey;
}
void MDNotes::createActions() {
	actKeyOk = new QAction(ui->textEdit);
	actKeyDeleteLast = new QAction(ui->textEdit);

	QList<QChar> ampersChars;
	foreach (QPushButton *pb, findChildren<QPushButton *>())
		ampersChars << pb->text().at( ui->pbOk->text().toStdString().find("&") + 1);

	/*!
	 * Search for duplicate QChar in ampersChars.
	 */
	QSet<QChar> charSet = QSet<QChar>::fromList(ampersChars);
	if (charSet.count() < ampersChars.count())
		WARN << tr("Duplicate ampersanded character found!");

	actKeyOk->setShortcut(tr("Alt+%1").arg(ampersChars.at(0)));
	actKeyDeleteLast->setShortcut(tr("Alt+%1").arg(ampersChars.at(1)));

	INFO << actKeyOk->shortcut() << actKeyDeleteLast->shortcut();
}
void MDNotes::connectActions() {
	connect(ui->pbOk, &QPushButton::clicked, this, &MDNotes::onOkClicked);
	connect(ui->pbDeleteLast, &QPushButton::clicked, this, &MDNotes::onDeleteLastClicked);
}
void MDNotes::deleteLastLines(const int lineCount) {
	ui->textEdit->setFocus();

	for (int k = 0; k < lineCount; k++) {
		QTextCursor storeCursorPos = ui->textEdit->textCursor();
		ui->textEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
		ui->textEdit->moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
		ui->textEdit->moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
		ui->textEdit->textCursor().removeSelectedText();
		ui->textEdit->textCursor().deletePreviousChar();
		ui->textEdit->setTextCursor(storeCursorPos);
	}
	emit settingsSaved(objectName() + tr(": %1 lines removed").arg(lineCount));
}
