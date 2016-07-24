#include "mdcombobox.h"

//QTableView *MdComboBox::tv = 0;

bool MdComboBox::setModelColumns(QList<quint8> columns, QSqlRelationalTableModel *model) {
	/** Check if columns contains invalid column values */
	qSort(columns.begin(), columns.end());
	//		int colCount = model()->columnCount(QModelIndex()),
	//				rowCount = model()->rowCount(QModelIndex());
	int colCount = model->columnCount(QModelIndex()),
			rowCount = model->rowCount(QModelIndex());

	if (columns.last() >= colCount)
		bReturn("Invalid column index found");

	INFO << rowCount << colCount;

	QStringList stringList;
	for (QList<quint8>::iterator cIt = columns.begin(); cIt != columns.end(); cIt++) {
		for (int r = 0; r < rowCount; r++) {
			stringList << model->data( model->index(r, *cIt, QModelIndex()) ).toString();
		}
	}
	INFO << stringList;

	//		QStringListModel *stringListModel = ;
	setModel( new QStringListModel(stringList) );
	return true;
}
