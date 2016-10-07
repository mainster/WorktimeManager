#ifndef HEADERALIAS_H
#define HEADERALIAS_H

#include <QObject>
#include <QMap>
#include <QMultiMap>

class HeaderAlias : public QObject {

	Q_OBJECT

public:
	HeaderAlias(QObject *parent) : QObject(parent) { }
	HeaderAlias(HeaderAlias *others, QObject *parent) : QObject(parent) {
		map = others->map;
	}
	~HeaderAlias() { }

	QMap<QString , int> map;

	int operator[](const QString &key);


private:
};

#endif // HEADERALIAS_H
