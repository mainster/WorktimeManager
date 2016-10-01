#include "mdtable.h"

/* ======================================================================== */
/*                             MdTable::MdTable                             */
/* ======================================================================== */
MdTable::MdTable(const QString &tvObjName, const QString &tableName,
					  QWidget *parent) : QWidget(parent) {

	/*!
	 * Create group box and MdTabView instances.
	 */
	m_gb = new QGroupBox(this);
	m_tv = new MdTabView(tableName, this);
	m_tv->setObjectName(tvObjName);

	QGridLayout *gbLayout = new QGridLayout();
	QGridLayout *widgetLayout = new QGridLayout();

	gbLayout->addWidget(m_tv);
	m_gb->setLayout(gbLayout);

	widgetLayout->addWidget(m_gb);
	this->setLayout(widgetLayout);

	m_gb->setStyleSheet(StyleSheet_QGroupBox);
	m_gb->setObjectName(tr("gb"));

	m_gb->installEventFilter(this);
	//	m_tv->installEventFilter(this);
	//	m_tv->viewport()->installEventFilter(this);

	actBaseDataForm =	new QAction(tr("Stammdaten ändern"), this);
	m_tv->addActions(QList<QAction *>() << actBaseDataForm);

	connect(m_tv->getActSectionMask(), &QAction::toggled, this, &MdTable::onActSectionMask);
	connect(m_tv, &MdTabView::sqlTableNameChanged, this, &MdTable::onSqlTableNameChanged);
	connect(actBaseDataForm, &QAction::triggered, this, &MdTable::onActBaseDataForm);

	clearSelected();
	show();
}
/* ======================================================================== */
/*                            Callback handler                              */
/* ======================================================================== */
void MdTable::onActSectionMask(bool sectionMask) {
	QAbstractItemModel *aim = m_tv->model();

	if (! sectionMask) {
		mSectMsk->storeVisibleCols(/*m_tv*/);
		delete mSectMsk;
	}
	else {
		QList<QString> colHead;
		for (int k = 0; k < aim->columnCount(); k++)
			colHead << aim->headerData(k, Qt::Horizontal, Qt::DisplayRole).toString();

		//		m_gb->layout()->removeWidget(this);

		mSectMsk = new SectionMask(m_tv, this);
		connect(mSectMsk,	&SectionMask::dblClickChecked,
				  m_tv->getActSectionMask(), &QAction::setChecked);

		m_gb->layout()->addWidget(mSectMsk);

		//		qobject_cast<QVBoxLayout *>(m_gb->layout())->addWidget(mSectMsk);
		//		qobject_cast<QVBoxLayout *>(m_gb->layout())->addWidget(this);
		//		qobject_cast<QVBoxLayout *>(m_gb->layout())->setStretchFactor(this, 15);
	}
}
void MdTable::onActBaseDataForm() {
	QAction *action = qobject_cast<QAction *>(sender());
	if (! action)
		qReturn("Cast failed!");

	bdForm = new BaseDataForm(-1, tv(), 0);
	bdForm->show();
}
void MdTable::onSqlTableNameChanged(const QString &sqlTableName) {
	m_gb->setTitle(Md::tableAlias[ sqlTableName ]);

	actBaseDataForm->setEnabled(true);

}
/* ======================================================================== */
/*                              Event handler                               */
/* ======================================================================== */
bool MdTable::eventFilter(QObject *obj, QEvent *event) {
	QMouseEvent *mouseEv = static_cast<QMouseEvent *>(event);
	if (! mouseEv)
		return QObject::eventFilter(obj, event);

	if (obj == m_gb) {
		if (mouseEv->type() == QEvent::MouseButtonPress) {
			emit groupBoxMouseButtonPress(this);
			return true;
		}
		//		if (mouseEv->type() == QEvent::MouseButtonDblClick) {
		//			INFO << tr("Double click:") << obj;
		//			return true;
		//		}
	}
	return QObject::eventFilter(obj, event);
}
#define QFOLDINGSTART {
const QString MdTable::StyleSheet_QGroupBox = QString(
																 "QGroupBox::title {"
																 "    subcontrol-origin: margin; /* margin boarder padding content */"
																 "    subcontrol-position: top center; /* position at the top center */"
																 "    font: italic 9pt ""Arial"";"
																 "    font-weight: bold;"
																 "    top: 1.0ex;   "
																 "    padding: 0px 8px;"
																 "  padding-top: -2ex;"
																 "  color:  rgba(50,50,50,255);"
																 "} [selected=false] {"
																 "  color:  rgba(245,0,0,255);"
																 "} "
																 "QGroupBox, QGroupBox [selected=false] {"
																 "  margin-top: 1ex; /*margin-bottom: 2px; margin-left: 2px; margin-right: 2px;*/"
																 "    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E0E0E0, stop: 1 #FFFFFF);"
																 "    font: italic 9pt ""Arial"";"
																 "    font-weight: bold;"
																 "    border-radius: 5px;"
																 "  border: 2px solid;"
																 "  border-top-color: qlineargradient(spread:reflect, x1:0, y1:0, x2:0.5, y2:0, stop:0.10 rgba(82, 82, 82, 255), stop:1 rgba(169, 169, 169,20));"
																 "  border-left-color: rgba(105,105,105,255);"
																 "  border-right-color: rgba(105,105,105,205);"
																 "  border-bottom-color: rgba(105,105,105,205);"
																 "} [selected=true] { "
																 "  /* margin-top: 5px; margin-bottom: -2px; margin-left: -2px; margin-right: -2px; */"
																 "  border: 2px solid; "
																 "  border-top-color: qlineargradient(spread:reflect, x1:0, y1:0, x2:0.5, y2:0, stop:0.10 rgba(255,0,0,255), stop:1 rgba(247, 247, 247, 250));"
																 "  border-left-color: rgba(255,0,0,255);"
																 "  border-right-color: rgba(255,0,0,255);"
																 "  border-bottom-color: rgba(255,0,0,255);"
																 "}"
																 "QGroupBox::title:hover {"
																 "    color: rgba(235, 235, 235, 255);"
																 "}"
																 );
#define QFOLDINGEND }
