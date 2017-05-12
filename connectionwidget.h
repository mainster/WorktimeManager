#ifndef CONNECTIONWIDGET_H
#define CONNECTIONWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QSqlDatabase>
#include <QDialogButtonBox>
#include <QObject>

#include "globals.h"
#include "dbcontroller.h"

QT_FORWARD_DECLARE_CLASS(QTreeWidget)
QT_FORWARD_DECLARE_CLASS(QTreeWidgetItem)
QT_FORWARD_DECLARE_CLASS(QSqlDatabase)
QT_FORWARD_DECLARE_CLASS(QMenu)


class ConnectionWidget: public QWidget {
	Q_OBJECT

public:
	ConnectionWidget(QWidget *parent = 0);
	static ConnectionWidget *instance(QWidget *parent = 0) {
		if (inst == 0)
			inst = new ConnectionWidget(parent);

		return inst;
	}
	virtual ~ConnectionWidget();
	QSqlDatabase currentDb() const;

signals:
	void tableActivated(const QString &table);
	void metaDataRequested(const QString &tableName);
	void detailesChanged(QString &details);

public slots:
	void refresh();
	void showMetaData();
	void on_tree_itemActivated(QTreeWidgetItem *item, int);
	void on_tree_currentItemChanged(QTreeWidgetItem *current,
											  QTreeWidgetItem *);
	QString details() const {
		return m_details;
	}

private slots:
	void removeTable();
	void setDetails(QString &details) {
		m_details = details;
		emit detailesChanged(m_details);
	}

private:
	static ConnectionWidget *inst;
	void setActive(QTreeWidgetItem *);
	QTreeWidget *tree;
	QAction *metaDataAction, *refreshAction, *removeAction;
	QDialogButtonBox *buttonBox;
	QString activeDb;
	QString m_details;
};

#endif
