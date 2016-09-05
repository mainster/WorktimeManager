#include "mdnotes.h"
#include "ui_mdnotes.h"


MDNotes::MDNotes(const QString settingsKey, QWidget *parent)
	: QFrame(parent), ui(new Ui::MDNotes), mSettingsKey(settingsKey) {
	ui->setupUi(this);
	editor = ui->textEdit;

	ui->groupBox->setTitle(objectName() + tr("/") + settingsKey);

	pbSave			= new Button_t("Speichern", "Ctrl+s", ui->pbSave, false);
	pbDeleteLast	= new Button_t("Löschen",	"Ctrl+l", ui->pbDeleteLast, false);
	pbStrikeOut		= new Button_t("Erledigt",	"Ctrl+e", ui->pbStrikeOut, false);

	pbSave->obj->setIcon(QIcon(":/images/save.png"));
	pbStrikeOut->obj->setIcon(QIcon(":/images/greenCheck.png"));
	pbDeleteLast->obj->setIcon(QIcon(":icoDelete"));

//	if (settingsKey.contains("toDo"))
//		pbSave->obj->setShortcut(QKeySequence("Ctrl+s"));

	/*!
	 * Restore text edit contents.
	 */
	QSETTINGS;
	if (! config.allKeys().join(',').contains(mSettingsKey))
		config.setValue(objectName() + tr("/") + mSettingsKey,
							 ui->textEdit->document()->toHtml());

	ui->textEdit->setTabStopWidth(20);
	ui->textEdit->setHtml(config.value(objectName() + tr("/") + mSettingsKey).toString());

	editor->installEventFilter(this);
	/*!
	 * Create and connect action objects.
	 */
	//	createActions();
	connectActions();
}
MDNotes::~MDNotes() {
	delete ui;
}
void MDNotes::keyshortTrigger() {
	INFO << tr("Shortcut");
	ui->pbSave->click();
}
void MDNotes::onAnyKeyClicked(bool clicked) {
	QSETTINGS;
	Q_UNUSED(clicked);

	QPushButton *pb = qobject_cast<QPushButton *>(sender());
	if (!pb)		return;

	if (pb == pbSave->obj) {
		config.setValue(objectName() + tr("/") + mSettingsKey,
							 ui->textEdit->document()->toHtml());
		emit changed(objectName() + tr(": Note added"));
	}
	if (pb == pbDeleteLast->obj) {
		deleteLastLines(1);
		config.setValue(objectName() + tr("/") + mSettingsKey,
							 ui->textEdit->document()->toHtml());
	}
	if (pb == pbStrikeOut->obj) {
		toggleLineStrikeout();
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
/* ======================================================================== */
/*                              Event handler                               */
/* ======================================================================== */
bool MDNotes::eventFilter(QObject *o, QEvent *e) {
	if (e->type() == QEvent::FocusIn)
		MDNotes::focusInEvent(static_cast<QFocusEvent *>(e));

	if (e->type() == QEvent::FocusOut)
		MDNotes::focusOutEvent(static_cast<QFocusEvent *>(e));

	return QObject::eventFilter(o, e);
}
void MDNotes::focusOutEvent(QFocusEvent *e) {
	pbSave->disableShortcut();
	pbDeleteLast->disableShortcut();
	pbStrikeOut->disableShortcut();
	e->accept();
}
void MDNotes::focusInEvent(QFocusEvent *e) {
	pbSave->enableShortcut();
	INFO << tr("ui obj:") << ui->pbSave->shortcut().toString();
	pbDeleteLast->enableShortcut();
	pbStrikeOut->enableShortcut();
	e->accept();
}

