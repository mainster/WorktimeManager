#ifndef TABVIEW_H
#define TABVIEW_H

#include <QtGui>
#include <QtWidgets>
#include <QObject>
#include <QSql>

class TabView : public QTableView {

	Q_OBJECT

public:
	explicit TabView(QWidget *parent);

private:

};

#endif // TABVIEW_H
