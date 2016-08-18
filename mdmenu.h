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

	virtual bool event (QEvent * e);
};

#endif // MDMENU_H
