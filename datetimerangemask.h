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
#include <QCalendarWidget>
#include <QSpacerItem>

#include "globals.h"
#include "locals.h"

class DateTimeRangeMask : public QWidget {

	Q_OBJECT

public:
	DateTimeRangeMask(QWidget *parent)
		: QWidget(parent) {
		QSETTINGS;

		m_inst = this;
		setObjectName(tr("DateTimeRangeMask"));

		lblMinDate = new QLabel(tr("Von: "), this);
		lblMaxDate = new QLabel(tr("Bis: "), this);
		lblMonth = new QLabel(tr("Monat: "), this);
		lblWeek = new QLabel(tr("Woche: "), this);

		deMinDate = new QDateEdit(this);
		deMaxDate = new QDateEdit(this);
		PONAM(deMinDate);
		PONAM(deMaxDate);

		lblMinDate->setBuddy(deMinDate);
		lblMaxDate->setBuddy(deMaxDate);

		INFO << listCast<QDateEdit *>(findChildren<QDateEdit *>(QString(), Qt::FindDirectChildrenOnly));

		foreach (QDateEdit *de, listCast<QDateEdit *>(
						findChildren<QDateEdit *>(QString(), Qt::FindDirectChildrenOnly))) {
			de->setMinimumDate(QDate(2000, 1, 1));
			de->setMaximumDate(QDate(2050, 1, 1));
			de->setDisplayFormat(tr("dd.MM.yyyy"));
			de->setCalendarPopup(true);
			de->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
			de->setFixedWidth(120);
			de->setLocale(QLocale::German);
			de->calendarWidget()->setLocale(QLocale::German);

			connect(de, &QDateEdit::dateChanged, this, &DateTimeRangeMask::onDateEditChanged);
		}
		deMinDate->setDate(config.value(objectName() + Md::k.minDateEdit,
												  QDate::currentDate()).toDate());
		deMaxDate->setDate(config.value(objectName() + Md::k.maxDateEdit,
												  QDate::currentDate()).toDate());

		QGridLayout *gl = new QGridLayout(this);

		/*!
		 * Fixed reproducible segmentation fault when destructing DateTimeRangeMask:: objects.
		 * Problem was the spacer object which has been added two times to the gl layout
		 * instance?!!!!
		 */
		gl->addWidget(lblMinDate,	0, 0, 1, 1);
		gl->addWidget(deMinDate,	0, 1, 1, 1);
		QSpacerItem *spacer = new QSpacerItem(50, 10, QSizePolicy::Expanding);
		gl->addItem(spacer,			0, 2, 1, 1);
		gl->addWidget(lblMaxDate,	1, 0, 1, 1);
		gl->addWidget(deMaxDate,	1, 1, 1, 1);
		spacer = new QSpacerItem(50, 10, QSizePolicy::Expanding);
		gl->addItem(spacer,			1, 2, 1, 1);

		lblMonth->hide();
		lblWeek->hide();

		queryHolidyLookup();

		show();
	}

	DateTimeRangeMask(const DateTimeRangeMask &others)
		: QWidget(new QWidget()) {
		m_inst = this;
		lblMinDate = others.lblMinDate;
		lblMaxDate = others.lblMaxDate;
		lblMonth = others.lblMonth;
		lblWeek = others.lblWeek;
		deMinDate = others.deMinDate;
		deMaxDate = others.deMaxDate;
		mMinDate = others.mMinDate;
		mMaxDate = others.mMaxDate;
		holidays = others.holidays;
		manager = others.manager;
	}

	static DateTimeRangeMask *inst() {
		if (! m_inst) {
			qDebug().noquote() << tr("Instance pointer is empty!");
			return NULL;
		}
		return m_inst;
	}
	~DateTimeRangeMask() {

	}

	void queryHolidyLookup() {
		/*!
		 * web query for "Feiertage" dates in BaWü.
		 */
		manager = new QNetworkAccessManager(this);
		connect(manager, SIGNAL(finished(QNetworkReply*)),
				  this, SLOT(onResult(QNetworkReply*)));

		for (int year = 2015; year < 2030; year++)
			manager->get(QNetworkRequest(
								 QUrl(tr("http://feiertage.jarmedia.de/api/?jahr=%1&nur_land=BW&nur_daten")
										.arg(year))));
	}
	static QDate getMinDate() { return mMinDate; }
	static QDate getMaxDate() { return mMaxDate; }

protected slots:
	void onResult(QNetworkReply *reply);
	void onDateEditChanged(const QDate &date) {
		QSETTINGS;

		if (qobject_cast<QDateEdit *>(sender())->objectName().contains("deMinDate")) {
			config.setValue(objectName() + Md::k.minDateEdit, date);
			mMinDate = date;
			if (holidays.contains(date))
				INFO << tr("Feiertag:") << date.toString("dd.MM.yyyy:") << holidays.value(date);
		}

		if (qobject_cast<QDateEdit *>(sender())->objectName().contains("deMaxDate")) {
			config.setValue(objectName() + Md::k.maxDateEdit, date);
			mMaxDate = date;
			if (holidays.contains(date))
				INFO << tr("Feiertag:") << date.toString("dd.MM.yyyy:") << holidays.value(date);
		}
	}
	void resizeLineEditToContents() {
		QDateEdit *dateEdit = qobject_cast<QDateEdit *>(sender());
		if (! dateEdit)
			qReturn("bad cast");

		QString text = dateEdit->text();
		QFontMetrics fm = dateEdit->fontMetrics();
		int w = fm.boundingRect(text).width() * 2;
		dateEdit->resize(w, dateEdit->height());
	}

private:
	static DateTimeRangeMask *m_inst;
	QLabel *lblMinDate, *lblMaxDate, *lblMonth, *lblWeek;
	QDateEdit *deMinDate, *deMaxDate;
	static QDate mMinDate, mMaxDate;
	QMap<QDate, QString> holidays;
	QNetworkAccessManager *manager;
};

#endif // DATETIMERANGEMASK_H
