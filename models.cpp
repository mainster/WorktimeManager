#include "models.h"

Models::Models(QObject *parent) :
    QAbstractTableModel(parent) {
}

int Models::rowCount(const QModelIndex &parent) const {

}

int Models::columnCount(const QModelIndex &parent) const {

}

QVariant Models::data(const QModelIndex &index, int role) const {

}

QVariant Models::headerData(
      int section, Qt::Orientation orientation, int role) const {

}

bool Models::setData(
      const QModelIndex &index, const QVariant &value, int role) {

}

Qt::ItemIsEditable Models::flags(
      const QModelIndex &index) const {

}
