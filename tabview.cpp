#include "ui_tabview.h"
#include "tabview.h"
#include "globals.h"
#include <QColor>


class Browser;

TabView::TabView(QWidget *parent) :
	QTableView(parent), ui(new Ui::TabView) {
	ui->setupUi(this);

	connect( this,    SIGNAL(objectNameChanged(QString)),
				this,    SLOT(onObjectNameChanged(QString)));
	connect( this,    SIGNAL(onSelChanged(TabView*)),
				this,    SLOT(onTabViewSelChanged(TabView*)));

	/*
	connect( parent->parent()->parent(),
							SIGNAL(tabViewSelChanged(TabView*)),
				this,    SLOT(onTabViewSelChanged(TabView*)));
*/

	this->m_gb = ui->gb;
//	this->m_tv = ui->mtv;
	this->m_gb->setProperty("select",false);
	/**
	 * This could be seen as "re-render functionality" due
	 * to stylesheet changes
	 */
	this->m_gb->setStyleSheet(this->m_gb->styleSheet());

	setSortingEnabled( true );
	activeSel = false;
}
TabView::~TabView() {
	delete ui;
}

void TabView::restoreView() {
	/** Make column/row position movable by dragging */
	horizontalHeader()->setSectionsMovable(true);
	verticalHeader()->setSectionsMovable(true);

	/** Restore alternating row color */
	QSETTINGS;
	bool altOnOff = config.value(objectName() + "/AlternateRowColEnable", "true").toBool();
	QColor col = config.value(objectName() + "/AlternateRowColor", "").value<QColor>();
	if (col.isValid())
		setAlternateRowCol( col, altOnOff);
}
void TabView::onObjectNameChanged(const QString &objNam) {
	m_gb->setTitle( objNam );
	m_gb->setAccessibleName(tr("gboxOf")+objNam);
	setAccessibleName(objNam);
}


void TabView::setGrBoxTitle(QString s) {
	m_gb->setTitle(s);
	ui->gb = m_gb;
}
QGroupBox *TabView::grBox() const {
	return m_gb;
}
void TabView::setGrBox(QGroupBox *gb) {
	m_gb = gb;
}
void TabView::setGrBoxStyleSht(const QString &ss) {
	tv()->setStyleSheet(ss);
}
void TabView::onTabViewSelChanged(TabView *tv) {
	/**
	 * If its not me, reset the StyleSheet for this instance
	 */
	if (tv != NULL) {
		if (this->objectName().contains(tv->objectName()))  {
			activeSel = true;
			return;
		}
		m_gb->setProperty("select", false);
		m_gb->setStyleSheet(m_gb->styleSheet());
		activeSel = false;
	}
}

/* ---------------------------------------------------------------- */
/*                      Helper methodes                          */
/* ---------------------------------------------------------------- */
void TabView::setAlternateRowCol(QColor &col, bool alternateEnabled) {
	setAlternatingRowColors(alternateEnabled);

	if (alternateEnabled && col.isValid()) {
		setStyleSheet(
					QString("alternate-background-color: rgba(%1,%2,%3,%4);")
					.arg( col.red())
					.arg( col.green())
					.arg( col.blue())
					.arg( col.alpha()) );
		QSETTINGS;
		config.setValue(objectName() + "/AlternateRowColEnable", alternateEnabled);
		config.setValue(objectName() + "/AlternateRowColor", col);
	}
}


