#ifndef SECTIONMASK_H
#define SECTIONMASK_H

#include <QWidget>
#include <QCheckBox>
#include <QObject>
#include <QLayout>
#include <QAbstractItemModel>
#include <QDebug>
#include <QTableView>
#include <QObject>
#include "models.h"
#include "globals.h"


class SectionMask : public QWidget {

	Q_OBJECT

public:
	SectionMask(const QList<QString> headerTexts,
					QWidget *parent) : QWidget(parent) {
		Q_UNUSED(headerTexts);
		inst = this;
	}
	SectionMask(QTableView *_tv, QWidget *parent) : QWidget(parent) {
		inst = this;
		tv = _tv;
//		INFO << tv->objectName();

		if (! tv) {
			qDebug().noquote() << tr("Cast parent to MdTable failed!");
			return;
		}

		QList<QPair<QString, bool> > columnHeads;
		QAbstractItemModel *aim = tv->model();
		sqlColumnCount = aim->columnCount();
		sqlRowCount = aim->rowCount();

		for (int k = 0; k < sqlColumnCount; k++)
			columnHeads << QPair<QString, bool>(
						aim->headerData(k, Qt::Horizontal, Qt::DisplayRole).toString(),
						tv->isColumnHidden(k));

//		INFO << columnHeads;

		QHBoxLayout *hbLay = new QHBoxLayout(this);

		/*!
		 * Since columnHeads is direct generated via aim->headerData(k
		 * and the next for loop is indexed by [0, columnHeads.count()[
		 * k will represent the logical index nummer from the SQL table templates.
		 */
		for (int logicIdx = 0; logicIdx < columnHeads.count(); logicIdx++) {
			QPair<QString, bool> pair = columnHeads.at(logicIdx);
			checkBoxs << new QCheckBox(pair.first, this);
			checkBoxs.last()->setObjectName(tr("cb_") + pair.first);
			checkBoxs.last()->setProperty("logicalIndex", QVariant::fromValue(logicIdx));
			connect(checkBoxs.last(),	&QCheckBox::toggled,
					  this,					&SectionMask::onCheckboxTogd);
			checkBoxs.last()->setChecked(! pair.second);
			hbLay->addWidget(checkBoxs.last());
		}

		restoreVisibleCols();
		show();
	}
	SectionMask(const SectionMask &other)
		: QWidget(new QWidget()) {
		inst = this;
		checkBoxs = other.checkBoxs;
	}
	static SectionMask *instance() {
		if (! inst) {
			qDebug().noquote() << tr("Instance pointer is empty!");
			return NULL;
		}
		return inst;
	}
	~SectionMask() { }

	bool storeVisibleCols(/*QTableView *tv*/) {
		if (! static_cast<SqlRtm *>(tv->model()))
			return false;
		/* ======================================================================== */
		/*  Store column configuration at this point and close/delete SectionMask   */
		/* ======================================================================== */
		QList<bool> cols;
		foreach (QCheckBox *cb, checkBoxs)
			cols << cb->isChecked();

		/*! Store to model sources */
		static_cast<SqlRtm *>(tv->model())->setVisibleCols(cols);

		/*! store to ini file */
//		static_cast<SqlRtm *>(tv->model())->storeModel(tv->);

		return true;
	}
	bool restoreVisibleCols(const QList<bool> visibleCols = QList<bool>()) {
		if (! static_cast<SqlRtm *>(tv->model()))
			return false;

		QList<bool> vcs;

		(! visibleCols.length())
				? (vcs = static_cast<SqlRtm *>(tv->model())->visibleCols())
				: (vcs = visibleCols);

		for (int k = 0; k < vcs.count(); k++)
			tv->setColumnHidden(k, !vcs.at(k));

		return true;
	}

signals:
	void dblClickChecked(bool b = false);

protected slots:
	void onCheckboxTogd(bool showColumn) {
		QCheckBox *cb = qobject_cast<QCheckBox *>(sender());
//		INFO << cb->objectName();
		tv->setColumnHidden(cb->property("logicalIndex").toInt(), !showColumn);
	}

protected:
	void mouseDoubleClickEvent(QMouseEvent *e) override {
		e->accept();
//		storeVisibleCols();
//		close();
		emit dblClickChecked();
	}

private:
	static SectionMask * inst;
	QList<QCheckBox *> checkBoxs;
	int sqlColumnCount, sqlRowCount;
	QTableView *tv;
};

#endif // SECTIONMASK_H
