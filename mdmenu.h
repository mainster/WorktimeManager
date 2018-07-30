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
	explicit MdMenu(QWidget *parent = nullptr);
	explicit MdMenu(const QString &title, QWidget *parent = nullptr);

	~MdMenu();

	virtual bool event (QEvent * e);
	QAction *addMenu(MdMenu *menu) {
		return QMenu::addMenu(static_cast<QMenu *>(menu));
	}

	MdMenu *addMenu(const QString &title) {
		return static_cast<MdMenu *>(QMenu::addMenu(title));
	}

	MdMenu *addMenu(const QIcon &icon, const QString &title) {
		return static_cast<MdMenu *>(QMenu::addMenu(icon, title));
	}

};

#endif // MDMENU_H
