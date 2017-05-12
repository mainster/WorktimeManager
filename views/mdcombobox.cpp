#include "mdcombobox.h"


MdComboBox::MdComboBox(QWidget *parent)
	: QComboBox(parent),
	  listMdl(new QStringListModel()),
	  mCompleter(new QCompleter(this)) {}

void MdComboBox::setModel(QAbstractItemModel *inputModel) {
	INFO << GN_BG(inputModel->metaObject()->className());
	inputMdl = inputModel;
}
void MdComboBox::setModelColumns(QList<short> columns) {
	if (! static_cast<QSqlRelationalTableModel *>(model()))
		qReturn("model() should return a QSqlRelationalTableModel!!");

	mColCfg = columns;
	refreshView();
}
bool MdComboBox::refreshView() {
	int colCount = inputMdl->columnCount(),
			rowCount = inputMdl->rowCount();

	QList<short int>::iterator biggest =
			std::max_element(mColCfg.begin(), mColCfg.end());

	/*!
		 * Check if vector columns contains invalid column values.
		 */
	if (colCount <= *biggest)
		bReturn("Invalid column index found");

	/*!
		 * Prepare QStringList object stringList which later can be used to create
		 * QStringListModel to source the QCompleter object.
		 */
	QStringList *stringList = new QStringList();
	for (int r = 0; r < rowCount; r++) {
		QStringList *row = new QStringList();

		for (QList<short>::iterator cIt = mColCfg.begin(); cIt != mColCfg.end(); cIt++) {
			*row << inputMdl->data( inputMdl->index(r, *cIt, QModelIndex()) ).toString();
		}

		*stringList << row->join(QString(", "));
	}

	/*!
		 * Prepare QStringList list object QList<QStringList> userData which later can be
		 * used as user data for QComboBox items.
		 */
	QList<QStringList *> userData;
	for (int r = 0; r < rowCount; r++) {
		userData.append( new QStringList() );

		for (int c = 0; c < colCount; c++) {
			*userData.last() << inputMdl->data(inputMdl->index(r, c, QModelIndex())).toString();
		}
	}
	delete listMdl;
	QComboBox::setModel( listMdl = new QStringListModel(*stringList) );

#ifdef USE_COMPLETER
	delete mCompleter;
	mCompleter = new QCompleter(*stringList);
	mCompleter->setCaseSensitivity(Qt::CaseInsensitive);
	mCompleter->setFilterMode(Qt::MatchContains);
	setCompleter(mCompleter);
#endif
	return true;
}
void MdComboBox::timerEvent(QTimerEvent *e) {
	lineEdit()->selectAll();
	QObject::killTimer(e->timerId());
}
void MdComboBox::focusInEvent(QFocusEvent *e) {
	if (isEditable())
		lineEdit()->setSelection(1,3);
	setEditable(true);
	QObject::startTimer(1);
	QComboBox::focusInEvent(e);
}
QCompleter *MdComboBox::getCompleter() const {
#ifdef USE_COMPLETER
	return mCompleter;
#else
	return QComboBox::completer();
#endif
}
