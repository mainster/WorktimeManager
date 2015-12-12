#ifndef MODELS_H
#define MODELS_H

#include <QAbstractTableModel>

class Models : public QAbstractTableModel {

   Q_OBJECT

public:
   explicit Models(QObject *parent = 0);

   int rowCount(const QModelIndex & parent = QModelIndex()) const;
   int columnCount(const QModelIndex &parent) const;
   QVariant data(const QModelIndex &index, int role) const;
   QVariant headerData(int section, Qt::Orientation orientation, int role) const;
   bool setData(const QModelIndex &index, const QVariant &value, int role);
   Qt::ItemIsEditable flags(const QModelIndex &index) const;

signals:

public slots:

};

#endif // MODELS_H
