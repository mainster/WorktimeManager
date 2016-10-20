#ifndef TBL2LISTMDL_H
#define TBL2LISTMDL_H

#include <QStringListModel>
#include <QAbstractItemModel>
#include <QAbstractListModel>

class Tbl2ListMdl : public QAbstractListModel {

	Q_OBJECT

public:
	Tbl2ListMdl(QAbstractItemModel *sourceModel, QObject *parent = 0)
		: QAbstractListModel(parent), mSrcMdl(sourceModel) {

	}

private:
	QAbstractItemModel *mSrcMdl;
};

#endif // TBL2LISTMDL_H
