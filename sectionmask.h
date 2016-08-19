#ifndef SECTIONMASK_H
#define SECTIONMASK_H

#include <QWidget>
#include <QCheckBox>
#include <QObject>
#include <QLayout>

class SectionMask : public QWidget {

	Q_OBJECT

public:
	SectionMask(QWidget *parent) : QWidget(parent) { }
	SectionMask(const QList<QString> headerTexts,
					QWidget *parent) : QWidget(parent) {

		QHBoxLayout *hbLay = new QHBoxLayout(this);

		foreach (QString s, headerTexts) {
			checkBoxs << new QCheckBox(s, this);
			hbLay->addWidget(checkBoxs.last());
		}

		setLayout(hbLay);
		show();
	}
	SectionMask(const SectionMask &other)
		: QWidget(new QWidget()) {
		checkBoxs = other.checkBoxs;
	}
	~SectionMask() { }

private:
	QList<QCheckBox *> checkBoxs;
};

#endif // SECTIONMASK_H
