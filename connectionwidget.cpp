#include "connectionwidget.h"
//#include "eventmon.h"

#include <QtWidgets>
#include <QtSql>

ConnectionWidget *ConnectionWidget::inst = 0;

ConnectionWidget::ConnectionWidget(QWidget *parent) : QWidget(parent) {
	inst = this;
	QVBoxLayout *layout = new QVBoxLayout(this);
	tree = new QTreeWidget(this);
	tree->setObjectName(QLatin1String("tree"));
	tree->setHeaderLabels(QStringList(tr("database")));
	tree->header()->setSectionResizeMode(QHeaderView::Stretch);
	refreshAction = new QAction(tr("Refresh"), tree);
	metaDataAction = new QAction(tr("Show Schema"), tree);
	removeAction = new QAction(tr("Delete Table"), tree);
	connect(refreshAction, SIGNAL(triggered()), SLOT(refresh()));
	connect(metaDataAction, SIGNAL(triggered()), SLOT(showMetaData()));
	connect(removeAction, &QAction::triggered, this, &ConnectionWidget::removeTable);

	tree->addAction(refreshAction);
	tree->addAction(metaDataAction);
	tree->addAction(removeAction);
	tree->setContextMenuPolicy(Qt::ActionsContextMenu);

	layout->addWidget(tree);
	tree->expandAll();

	QMetaObject::connectSlotsByName(this);

	QTimer::singleShot(500, Qt::CoarseTimer, this, SLOT(refresh()));
}
ConnectionWidget::~ConnectionWidget() {}
static QList<QString *> qDBCaption(const QSqlDatabase &db) {
	QFileInfo fi(db.databaseName());
	QList<QString *> str;

	str << new QString(db.driverName());	// tree header/root element
	str.at(0)->append(QLatin1Char(':'));
	str.at(0)->append(fi.baseName());

	str << new QString(db.driverName());
	str.at(1)->append(QLatin1Char(':'));
	str.at(1)->append(db.databaseName());

	if (!db.userName().isEmpty())
		str.at(1)->append(db.userName());
	if (!db.hostName().isEmpty()) {
		str.at(1)->append(QLatin1Char('@'));
		str.at(1)->append(db.hostName());
	}

	return str;
}
void ConnectionWidget::removeTable() {
//	buttonBox = new QDialogButtonBox(
//				QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

//	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
//	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	QMessageBox msgBox;
	msgBox.setText("Remove SQL table action");
	msgBox.setInformativeText("Are you sure to DROP sql table from sql database?");
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Cancel);

	QRegExp rx("\\[(.*)\\]");
	int idx = rx.indexIn(tree->currentItem()->text(0));
	QString tableName = rx.capturedTexts().at(1);
	int ret = msgBox.exec();

	if (ret == QMessageBox::Yes) {
		if (tableName.isEmpty())
			qReturn("Failure while query table name via QRegExp \\[(.*)\\]");

		INFO << tr("Remove table %1 accepted!").arg(tableName);
		DbController::dropTable(tableName);
		return;

		QSqlQuery *query = new QSqlQuery(tr("DROP table %1").arg(tableName));
		if (! query->exec()) {
			CRIT << tr("SQL query failure detected: %1").arg(query->lastQuery());
			CRIT << tr("Last query: %1").arg(query->lastError().text());
		}

//		QSqlQuery query(currentDb());
//		if (! query.exec(tr("DROP TABLE %1").arg(tableName)))
//			WARN << query.lastError();
		refresh();
	}
	else
		qReturn(tr("Remove table %1 rejected!").arg(tableName));

}
void ConnectionWidget::refresh() {
	tree->clear();
	QStringList connectionNames = QSqlDatabase::connectionNames();

	bool gotActiveDb = false;
	for (int i = 0; i < connectionNames.count(); ++i) {
		QTreeWidgetItem *root = new QTreeWidgetItem(tree);
		QSqlDatabase db = QSqlDatabase::database(connectionNames.at(i), false);

		root->setText(0, *qDBCaption(db).at(0));
		QString s = *qDBCaption(db).at(1);
		setDetails(s);

		if (connectionNames.at(i) == activeDb) {
			gotActiveDb = true;
			setActive(root);
		}
		if (db.isOpen()) {
			QStringList tables = db.tables();
			for (int t = 0; t < tables.count(); ++t) {
				QTreeWidgetItem *table = new QTreeWidgetItem(root);
				table->setText(0, Md::tableAlias[tables.at(t)]
						+ tr("  [%1]").arg(tables.at(t)));
			}
		}
	}
	if (!gotActiveDb) {
		activeDb = connectionNames.value(0);
		setActive(tree->topLevelItem(0));
	}

	tree->doItemsLayout();
	tree->expandAll();
}
QSqlDatabase ConnectionWidget::currentDb() const {
	return QSqlDatabase::database(activeDb);
}
static void qSetBold(QTreeWidgetItem *item, bool bold) {
	QFont font = item->font(0);
	font.setBold(bold);
	item->setFont(0, font);
}
void ConnectionWidget::setActive(QTreeWidgetItem *item) {
	for (int i = 0; i < tree->topLevelItemCount(); ++i) {
		if (tree->topLevelItem(i)->font(0).bold())
			qSetBold(tree->topLevelItem(i), false);
	}

	if (!item) return;

	qSetBold(item, true);
	activeDb =
			QSqlDatabase::connectionNames().value(tree->indexOfTopLevelItem(item));
}
void ConnectionWidget::on_tree_itemActivated(QTreeWidgetItem *item,
															int /* column */) {
	if (!item)	return;

	if (!item->parent())
		setActive(item);
	else {
		setActive(item->parent());

		/*!
		 * Check if table aliases has been used.
		 */
		QRegularExpression re("\\[\\w+\\]");
		if (re.match(item->text(0)).hasMatch())
			emit tableActivated(re.match(item->text(0))
									  .captured(0)
									  .remove(tr("["))
									  .remove(tr("]")));
		else
			emit tableActivated(item->text(0));
	}
}
void ConnectionWidget::showMetaData() {
	QTreeWidgetItem *cItem = tree->currentItem();
	if (!cItem || !cItem->parent())
		return;
	setActive(cItem->parent());
	emit metaDataRequested(cItem->text(0));
}
void ConnectionWidget::on_tree_currentItemChanged(QTreeWidgetItem *current,
																  QTreeWidgetItem *) {
	metaDataAction->setEnabled(current && current->parent());
}


#define QFOLDINGSTART {
// ConnectionWidget::ConnectionWidget(QWidget *parent)
//   : QWidget(parent) {

//   QVBoxLayout *layout = new QVBoxLayout(this);
//   tree = new QTreeWidget(this);
//   tree->setObjectName(QLatin1String("tree"));
//   tree->setHeaderLabels(QStringList(tr("database")));
//   tree->header()->setSectionResizeMode(QHeaderView::Stretch);

//   QAction *refreshAction = new QAction(tr("Refresh"), tree);
//   metaDataAction = new QAction(tr("Show Schema"), tree);
//   connect(refreshAction, SIGNAL(triggered()), SLOT(refresh()));
//   connect(metaDataAction, SIGNAL(triggered()), SLOT(showMetaData()));

//   tree->addAction(refreshAction);
//   tree->addAction(metaDataAction);
//   tree->setContextMenuPolicy(Qt::ActionsContextMenu);

//   layout->addWidget(tree);

//   QMetaObject::connectSlotsByName(this);

//}
// ConnectionWidget::~ConnectionWidget() {
//}

// static QString qDBCaption(const QSqlDatabase &db) {
//   QString nm = db.driverName();
//   nm.append(QLatin1Char(':'));
//   if (!db.userName().isEmpty())
//      nm.append(db.userName()).append(QLatin1Char('@'));
//   nm.append(db.databaseName());
//   return nm;
//}

// void ConnectionWidget::refresh() {
//   tree->clear();
//   QStringList connectionNames = QSqlDatabase::connectionNames();

//   bool gotActiveDb = false;
//   for (int i = 0; i < connectionNames.count(); ++i) {
//      QTreeWidgetItem *root = new QTreeWidgetItem(tree);
//      QSqlDatabase db = QSqlDatabase::database(connectionNames.at(i), false);
//      root->setText(0, qDBCaption(db));
//      if (connectionNames.at(i) == activeDb) {
//         gotActiveDb = true;
//         setActive(root);
//      }
//      if (db.isOpen()) {
//         QStringList tables = db.tables();
//         for (int t = 0; t < tables.count(); ++t) {
//            QTreeWidgetItem *table = new QTreeWidgetItem(root);
//            table->setText(0, tables.at(t));
//         }
//      }
//   }
//   if (!gotActiveDb) {
//      activeDb = connectionNames.value(0);
//      setActive(tree->topLevelItem(0));
//   }

//   tree->doItemsLayout(); // HACK
//}

// QSqlDatabase ConnectionWidget::currentDatabase() const {
//   return QSqlDatabase::database(activeDb);
//}

// static void qSetBold(QTreeWidgetItem *item, bool bold) {
//   QFont font = item->font(0);
//   font.setBold(bold);
//   item->setFont(0, font);
//}

// void ConnectionWidget::setActive(QTreeWidgetItem *item) {
//   for (int i = 0; i < tree->topLevelItemCount(); ++i) {
//      if (tree->topLevelItem(i)->font(0).bold())
//         qSetBold(tree->topLevelItem(i), false);
//   }

//   if (!item)
//      return;

//   qSetBold(item, true);
//   activeDb =
// QSqlDatabase::connectionNames().value(tree->indexOfTopLevelItem(item));
//}

// void ConnectionWidget::on_tree_item_Activate(QTreeWidgetItem *item, int) {

//   if (!item)
//      return;

//   if (!item->parent()) {
//      setActive(item);
//   } else {
//      setActive(item->parent());
//      emit tableActivated(item->text(0));
//   }
//}

// void ConnectionWidget::
// showMetaData() {
//   QTreeWidgetItem *cItem = tree->currentItem();
//   if (!cItem || !cItem->parent())
//      return;
//   setActive(cItem->parent());
//   emit metaDataRequested(cItem->text(0));
//}

// void ConnectionWidget::
// on_tree_current_Item_Changed(QTreeWidgetItem *,
//                             QTreeWidgetItem *) {
//   metaDataAction->setEnabled(current && current->parent());
//   INFO << "!";
//}

#define QFOLDINGEND }
