#ifndef MDSTRINGLISTMODEL_H
#define MDSTRINGLISTMODEL_H

#include <QAbstractItemModel>
#include <QAbstractListModel>

class MdStringListModel : public QAbstractListModel {

	Q_OBJECT

public:
	explicit MdStringListModel(QObject *parent = 0)
		: QAbstractListModel(parent), mColCfg(QList<short>() << 1 << 2) {}

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override {
		Q_UNUSED(parent);
		return createIndex(row, column);
	}
	QModelIndex parent(const QModelIndex &index) const override {
		return index.parent();
	}
	int rowCount(const QModelIndex &parent = QModelIndex()) const override {
		if (!parent.isValid()) return 0;
		return rowCount(parent);
	}
	int columnCount(const QModelIndex &parent = QModelIndex()) const override {
		if (!parent.isValid()) return 0;
		return 1;
	}

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
		if (!index.isValid()) return QVariant();

		int row = index.row(),
				col = index.column();

		switch (role) {
			case Qt::DisplayRole: {
				if (col == 0) {
					QStringList ret;
					foreach (short idx, mColCfg) {
						ret << createIndex(row, idx).data(Qt::DisplayRole).toString();
					}
					return QVariant(ret.join(','));
				}
			}; break;

			case Qt::TextAlignmentRole: {
				return QVariant(Qt::AlignCenter | Qt::AlignVCenter);
			}; break;

			default: break;
		}
		return QVariant();
	}

private:
	QList<short> mColCfg;
};

#endif  // MDSTRINGLISTMODEL_H
