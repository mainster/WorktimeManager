#include "mdnotes.h"
#include "ui_mdnotes.h"

MDNotes::MDNotes(const QString settingsKey, QWidget *parent)
	: QFrame(parent), ui(new Ui::MDNotes), mSettingsKey(settingsKey) {
	ui->setupUi(this);
	editor = ui->textEdit;

	ui->groupBox->setTitle(objectName() + tr("/") + settingsKey);
	ui->pbSave->setIcon(QIcon(":/images/save.png"));
	ui->pbStrikeOut->setIcon(QIcon(":/images/greenCheck.png"));
	ui->pbDeleteLast->setIcon(QIcon(":icoDelete"));

	/*!
	 * Restore text edit contents.
	 */
	QSETTINGS;
	if (! config.allKeys().join(',').contains(mSettingsKey))
		config.setValue(objectName() + tr("/") + mSettingsKey,
							 ui->textEdit->document()->toHtml());

	ui->textEdit->setTabStopWidth(20);
	ui->textEdit->setHtml(config.value(objectName() + tr("/") + mSettingsKey).toString());

	/*!
	 * Create and connect action objects.
	 */
	//	createActions();
	connectActions();
}

MDNotes::~MDNotes() {
	delete ui;
}
void MDNotes::onAnyKeyClicked(bool clicked) {
	QSETTINGS;
	Q_UNUSED(clicked);

	QPushButton *pb = qobject_cast<QPushButton *>(sender());
	if (!pb)		return;

	if (pb == ui->pbSave) {
		config.setValue(objectName() + tr("/") + mSettingsKey,
							 ui->textEdit->document()->toHtml());
		emit changed(objectName() + tr(": Note added"));
	}
	if (pb == ui->pbDeleteLast) {
		deleteLastLines(1);
		config.setValue(objectName() + tr("/") + mSettingsKey,
							 ui->textEdit->document()->toHtml());
	}
	if (pb == ui->pbStrikeOut) {
		toggleLineStrikeout();
	}
	if (pb == ui->pb1) {

	}
	if (pb == ui->pb2) {

	}

	config.sync();
}
void MDNotes::onOkClicked(bool clicked) {
	QSETTINGS;
	Q_UNUSED(clicked);
	config.setValue(objectName() + tr("/") + mSettingsKey,
						 ui->textEdit->document()->toHtml());
	config.sync();
	emit changed(objectName() + tr(": Note added"));
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
	//	pbStrikeOut

	if (false) {
		QList<QChar> ampersChars;
		foreach (QPushButton *pb, findChildren<QPushButton *>())
			ampersChars << pb->text().at( ui->pbSave->text().toStdString().find("&") + 1);

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
}
void MDNotes::connectActions() {
	//	connect(ui->pbOk, &QPushButton::clicked, this, &MDNotes::onOkClicked);
	//	connect(ui->pbDeleteLast, &QPushButton::clicked, this, &MDNotes::onDeleteLastClicked);
	foreach (QPushButton *pb, findChildren<QPushButton *>())
		connect(pb, &QPushButton::clicked, this, &MDNotes::onAnyKeyClicked);
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
	emit changed(objectName() + tr(": %1 lines removed").arg(lineCount));
}
void MDNotes::toggleLineStrikeout() {
	ui->textEdit->setFocus();

	//!< Get cursor instance and take a backup.
	QTextCursor c = editor->textCursor(),
			cOld  = c;
	QRegularExpression re("^\\s+");
	c.movePosition(QTextCursor::StartOfLine);
	c.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
	int len = re.match(c.selectedText()).capturedLength();
	c.movePosition(QTextCursor::StartOfLine);
	c.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, len);
	c.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);

	QTextCharFormat f = c.charFormat();
	f.setFontStrikeOut(! f.fontStrikeOut());
	c.mergeCharFormat(f);

	editor->setTextCursor(cOld);
	emit changed(objectName() + tr(": Line %1 striked out")
							 .arg(editor->textCursor().blockNumber() + 1));
}
