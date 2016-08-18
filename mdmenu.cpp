#include "mdmenu.h"


MdMenu::MdMenu(QWidget *parent)
	: QMenu(parent) {

}

MdMenu::MdMenu(const QString &title, QWidget *parent)
	: QMenu(parent) {
	setTitle(title);
}
