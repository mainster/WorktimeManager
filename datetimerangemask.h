#ifndef DATETIMERANGEMASK_H
#define DATETIMERANGEMASK_H

#include <QWidget>
#include <QObject>
#include <QAbstractItemModel>
#include <QCheckBox>
#include <QDebug>
#include <QLayout>
#include <QTableView>
#include <QLabel>
#include <QDateEdit>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>
#include <QUrl>

#include "globals.h"

class DateTimeRangeMask : public QWidget {

	Q_OBJECT

public:
	DateTimeRangeMask(QWidget *parent) : QWidget(parent) {
		m_inst = this;
		queryHolidyLookup();
	}
	DateTimeRangeMask(QTableView *_tv, QWidget *parent) : QWidget(parent) {
		m_inst = this;
		tv = _tv;
		INFO << tv->objectName();

		if (! tv) {
			qDebug().noquote() << tr("Cast parent to MdTable failed!");
			return;
		}

		lblMinDate = new QLabel(tr("Von: "), this);
		lblMaxDate = new QLabel(tr("Bis: "), this);
		lblMonth = new QLabel(tr("Monat: "), this);
		lblWeek = new QLabel(tr("Woche: "), this);

		deMinDate = new QDateEdit(this);
		deMaxDate = new QDateEdit(this);
		PONAM(deMinDate);
		PONAM(deMaxDate);

		connect(deMinDate, &QDateEdit::dateChanged, this, &DateTimeRangeMask::onDateEditChanged);
		connect(deMaxDate, &QDateEdit::dateChanged, this, &DateTimeRangeMask::onDateEditChanged);
		lblMinDate->setBuddy(deMinDate);
		lblMaxDate->setBuddy(deMaxDate);

		foreach (QDateEdit *de, listCast<QDateEdit *>(
						findChildren<QDateEdit *>(QString(), Qt::FindDirectChildrenOnly))) {
			de->setMinimumDate(QDate(2000, 1, 1));
			de->setMaximumDate(QDate(2050, 1, 1));
			de->setDisplayFormat(tr("dd.MM.yyyy"));
		}
		deMinDate->setDate(QDate(2015, 1, 1));
		deMaxDate->setDate(QDate(2017, 1, 1));

		QGridLayout *gl = new QGridLayout(this);

		gl->addWidget(lblMinDate, 0, 0, 1, 1);
		gl->addWidget(deMinDate, 0, 1, 1, 1);
		gl->addWidget(lblMaxDate, 1, 0, 1, 1);
		gl->addWidget(deMaxDate, 1, 1, 1, 1);

		lblMonth->hide();
		lblWeek->hide();

		queryHolidyLookup();
		show();
	}
	static DateTimeRangeMask *inst() {
		if (! m_inst) {
			qDebug().noquote() << tr("Instance pointer is empty!");
			return NULL;
		}
		return m_inst;
	}
	~DateTimeRangeMask() { }

	void queryHolidyLookup() {
		/*!
		 * web query for "Feiertage" dates in BaWü.
		 */
		QNetworkAccessManager *manager = new QNetworkAccessManager(this);
		connect(manager, SIGNAL(finished(QNetworkReply*)),
				  this, SLOT(onResult(QNetworkReply*)));

		for (int year = 2015; year < 2020; year++)
			manager->get(QNetworkRequest(QUrl(tr("http://feiertage.jarmedia.de/api/?jahr=%1&nur_land=BW&nur_daten")
														 .arg(year))));
	}
	QDateEdit *getDeMinDate() const { return deMinDate; }
	QDateEdit *getDeMaxDate() const { return deMaxDate; }

protected slots:
	void onResult(QNetworkReply *reply);
	void onDateEditChanged(const QDate &date) {

		if (qobject_cast<QDateEdit *>(sender())->objectName().contains("deMinDate"))
			if (holidays.contains(date))
				INFO << tr("Feiertag:") << date.toString("dd.MM.yyyy:") << holidays.value(date);

		if (qobject_cast<QDateEdit *>(sender())->objectName().contains("deMaxDate"))
			if (holidays.contains(date))
				INFO << tr("Feiertag:") << date.toString("dd.MM.yyyy:") << holidays.value(date);
	}

private:
	static DateTimeRangeMask *m_inst;
	QTableView *tv;
	QLabel *lblMinDate, *lblMaxDate, *lblMonth, *lblWeek;
	QDateEdit *deMinDate, *deMaxDate;
	QMap<QDate, QString> holidays;
};

#endif // DATETIMERANGEMASK_H
