#ifndef MDNOTES_H
#define MDNOTES_H

#include "mdstatebar.h"

#include <QFrame>
#include <globals.h>
#include <debug.h>
#include <QAction>
#include <QShortcut>


namespace Ui {
class MDNotes;
}

class MDNotes : public QFrame {

	Q_OBJECT

public:
	MDNotes(const QString settingsKey, QWidget *parent = 0);
	MDNotes(const MDNotes &other, QWidget *parent = 0) : QFrame(parent){
		mSettingsKey = other.mSettingsKey;
	}
	~MDNotes();
	QString settingsKey() const;

signals:
	void changed(const QString &message);

protected:
	void connectActions();

protected slots:
	void onOkClicked(bool clicked);
	void onDeleteLastClicked(bool clicked);
	void deleteLastLines(const int lineCount);

	void createActions();
	void onAnyKeyClicked(bool clicked);
	void toggleLineStrikeout();
	void keyshortTrigger();
private:
	Ui::MDNotes *ui;
	QString mSettingsKey;
	QTextEdit	*editor;
};

#endif // MDNOTES_H
