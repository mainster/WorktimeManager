#ifndef SFILTMDLLIST_H
#define SFILTMDLLIST_H

#include <QSortFilterProxyModel>

class SfiltMdlList : public QSortFilterProxyModel {

	Q_OBJECT

public:
	explicit SfiltMdlList(QObject *parent = 0) : QSortFilterProxyModel(parent) {}

	int rowCount(const QModelIndex &parent = QModelIndex()) const override {
		if (!parent.isValid()) return 0;

		return QSortFilterProxyModel::rowCount(parent);
	}



private:
};

#endif  // SFILTMDLLIST_H
