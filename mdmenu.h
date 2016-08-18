#ifndef MDMENU_H
#define MDMENU_H

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QtCore>
#include <QtGui>
#include <QToolTip>
#include <QWidget>

class MdMenu : public QMenu {

	Q_OBJECT

public:
	explicit MdMenu(QWidget *parent = 0);
	explicit MdMenu(const QString &title, QWidget *parent = 0);

	~MdMenu();

	bool event (QEvent * e) {
		const QHelpEvent *helpEvent = static_cast <QHelpEvent *>(e);
		if (helpEvent->type() == QEvent::ToolTip && activeAction() != 0) {
			QToolTip::showText(helpEvent->globalPos(), activeAction()->toolTip());
		}
		else {
			QToolTip::hideText();
		}
		return MdMenu::event(e);
	}
};

#endif // MDMENU_H
