#ifndef MDNOTES_H
#define MDNOTES_H

#include "mdstatebar.h"

#include <QObject>
#include <QFrame>
#include <QPushButton>
#include <globals.h>
#include <debug.h>
#include <QAction>
#include <QShortcut>
#include <QEvent>

namespace Ui {
class MDNotes;
}

class MDNotes : public QFrame {

	Q_OBJECT

public:
	struct Button_t {
	public:

		Button_t() {}
		Button_t(QString label,
					QString shortcutSequence = "",
					QPushButton *pButton = NULL,
					bool enableShortcut = true)
			: mLabel(label), mShortcutSequence(shortcutSequence) {

			if (pButton == NULL)
				obj = new QPushButton(/*mParent*/);
			else
				obj = pButton;

			if (enableShortcut) {
				obj->setText(tr("&%1").arg(label));
				obj->setShortcut(QKeySequence::fromString(mShortcutSequence));
			}
		}

		void disableShortcut() {
			obj->setText(mLabel);
			obj->setShortcut(QKeySequence());
		}
		void enableShortcut() {
			obj->setText(tr("&%1").arg(mLabel));
			obj->setShortcut(QKeySequence::fromString(mShortcutSequence));
		}

		QString mLabel;
		QString mShortcutSequence;
		QPushButton *obj;
	};

	Button_t *pbSave, *pbDeleteLast, *pbStrikeOut;

	MDNotes(const QString settingsKey, QWidget *parent = nullptr);
	MDNotes(const MDNotes &other, QWidget *parent = nullptr) : QFrame(parent){
		mSettingsKey = other.mSettingsKey;
	}
	~MDNotes();
	QString settingsKey() const;
	virtual bool eventFilter(QObject *o, QEvent *e) override;

signals:
	void changed(const QString &message);

protected:
	void connectActions();
	virtual void focusInEvent(QFocusEvent *e) override;
	virtual void focusOutEvent(QFocusEvent *e) override;

protected slots:
	void onOkClicked(bool clicked);
	void onDeleteLastClicked(bool clicked);
	void deleteLastLines(const int lineCount);
	void createActions();
	void onAnyKeyClicked(bool clicked);
	void toggleLineStrikeout();
	void keyshortTrigger();

private:
	Ui::MDNotes		*ui;
	QTextEdit		*editor;
	QString			mSettingsKey;

};
//Q_DECLARE_METATYPE(MDNotes::Button_t)

#endif // MDNOTES_H
