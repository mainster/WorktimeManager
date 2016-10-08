#ifndef HEADERALIAS_H
#define HEADERALIAS_H

#include <QObject>
#include <QMap>
#include <QMultiMap>

class HeaderAlias : public QObject {

	Q_OBJECT

public:
	HeaderAlias(QObject *parent = 0) : QObject(parent) { }
	HeaderAlias(const HeaderAlias *others, QObject *parent) : QObject(parent) {
		map = others->map;
	}
	~HeaderAlias() { }


//	int operator[](const QString &key);
	QMap<QString , int> map;


private:
};

#endif // HEADERALIAS_H
