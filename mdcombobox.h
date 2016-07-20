#ifndef MDCOMBOBOX_H
#define MDCOMBOBOX_H

#include <QObject>
#include <QWidget>
#include <QComboBox>
#include "inpfrm.h"

class MdComboBox : public QComboBox {

	Q_OBJECT

public:
	explicit MdComboBox(QWidget *parent = 0);

signals:

public slots:
};

#endif // MDCOMBOBOX_H
