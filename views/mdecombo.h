#ifndef MDECOMBO_H
#define MDECOMBO_H

#include <QWidget>
#include "dbcontroller.h"

class QComboBox;
class QDataWidgetMapper;
class QItemSelectionModel;
class QLabel;
class QLineEdit;
class QPushButton;
class QSqlRelationalTableModel;
class QStandardItemModel;
class QStringListModel;
class QTextEdit;


class MdECombo : public QWidget {
	Q_OBJECT
public:
	explicit MdECombo(QWidget *parent = 0);

signals:

public slots:

private:
	QSqlRelationalTableModel	*m_rtModel;
	QItemSelectionModel			*m_isModel;
	QDataWidgetMapper				*m_dwMapper;
	QComboBox						*m_cbx;
	QStringListModel				*m_slModel;
};

#endif // MDECOMBO_H
