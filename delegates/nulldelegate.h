#ifndef NULLDELEGATE_H
#define NULLDELEGATE_H

#include <QObject>
#include <QSqlRelationalDelegate>
#include "debug.h"

/*!
 * \brief The NullDelegate class is necessary to provide a "null" (empty) value for
 * QDataMapped QComboBoxes.
 */
class NullDelegate : public QSqlRelationalDelegate {

	Q_OBJECT

public:

	NullDelegate (QObject *parent = 0)
		: QSqlRelationalDelegate(parent) { }

	void setEditorData(QWidget *editor, const QModelIndex &index) const {
		QComboBox *comboBox = qobject_cast<QComboBox *>(editor);

		if (comboBox) {
			INFO << index.data();
			if (index.data().isNull() ||
				 index.data().toString().isEmpty() ||
				 index.data().toString().contains("null", Qt::CaseInsensitive)) {
				comboBox->addItem(QString(), QVariant());
				comboBox->setCurrentIndex(comboBox->count());
//				editor->setProperty("text", QString());
//				comboBox->setCurrentText(QString());
//				return;
			}
			else
				editor->setProperty("text", index.data());
		}
//		else
			QSqlRelationalDelegate::setEditorData(editor, index);
	}
	void setModelData(QWidget *editor, QAbstractItemModel *model,
							const QModelIndex &index) const {
		if (editor->property("text") == "")
			model->setData(index, QVariant());
		else
			QSqlRelationalDelegate::setModelData(editor, model, index);
	}
};

#endif
