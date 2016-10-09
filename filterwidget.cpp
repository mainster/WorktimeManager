#include "filterwidget.h"

FilterWidget::FilterWidget(QWidget *parent) : QWidget(parent) {
	QGroupBox *srcgb = new QGroupBox(0);
	srcgb->setTitle(tr("Source rows:"));
	QGroupBox *proxygb = new QGroupBox(0);
	proxygb->setTitle(tr("Proxy rows:"));

	QGridLayout *gl = new QGridLayout(srcgb);
	MdTabView *srctv = new MdTabView("worktime", srcgb);
	gl->addWidget(srctv, 0, 0, 1, 1);

	gl = new QGridLayout(proxygb);
	MdTabView *proxytv= new MdTabView("worktime", proxygb);
	gl->addWidget(proxytv, 0, 0, 1, 1);
	proxygb->setLayout(gl);

	QSplitter *spl = new QSplitter(Qt::Horizontal, 0);
	spl->addWidget(srcgb);
	spl->addWidget(proxygb);
	spl->show();
}
