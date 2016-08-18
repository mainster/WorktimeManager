#include "mdmenu.h"


MdMenu::MdMenu(QWidget *parent)
	: QMenu(parent) {

}

MdMenu::MdMenu(const QString &title, QWidget *parent)
	: QMenu(parent) {
	setTitle(title);
}

MdMenu::~MdMenu() {

}

bool MdMenu::event(QEvent *e) {
	const QHelpEvent *helpEvent = static_cast <QHelpEvent *>(e);
	if (helpEvent->type() == QEvent::ToolTip && activeAction() != 0) {
		QToolTip::showText(helpEvent->globalPos(), activeAction()->toolTip());
	}
	else {
		QToolTip::hideText();
	}
	return QMenu::event(e);
}

