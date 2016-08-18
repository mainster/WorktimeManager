#ifndef MDMENUBAR_H
#define MDMENUBAR_H

#include <QObject>
#include <QAction>
#include <QMenu>
#include <QObject>
#include <QtCore>
#include <QtGui>
#include <QToolTip>
#include <QWidget>
#include <QMenuBar>

#include "mdmenu.h"

class MdMenuBar : public QMenuBar {

	Q_OBJECT

public:
	explicit MdMenuBar(QWidget *parent = 0)
		: QMenuBar(parent) { }
	~MdMenuBar() { }

	QAction *addMenu(MdMenu *menu) {
		return QMenuBar::addMenu(static_cast<QMenu *>(menu));
	}

	MdMenu *addMenu(const QString &title) {
		return dynamic_cast<MdMenu *>(QMenuBar::addMenu(title));
	}

	MdMenu *addMenu(const QIcon &icon, const QString &title) {
		return dynamic_cast<MdMenu *>(QMenuBar::addMenu(icon, title));
	}

	MdMenu *addMenu2(const QString &title) {
		return static_cast<MdMenu *>(QMenuBar::addMenu(title));
	}

	MdMenu *addMenu2(const QIcon &icon, const QString &title) {
		return static_cast<MdMenu *>(QMenuBar::addMenu(icon, title));
	}
};

#endif // MDMENUBAR_H
