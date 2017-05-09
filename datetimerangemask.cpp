#include "datetimerangemask.h"
DateTimeRangeMask *DateTimeRangeMask::m_inst = 0;

QDate DateTimeRangeMask::mMinDate = QDate();
QDate DateTimeRangeMask::mMaxDate = QDate();

void DateTimeRangeMask::onResult(QNetworkReply* reply) {
	if (reply->error() != QNetworkReply::NoError)
		return;  // ...only in a blog post

	QString data = reply->readAll().toStdString().c_str();
	data.remove(QRegularExpression("[\\\"\\{\\}]"));
	QStringList dataList = data.split(',');
	//	QRegExp rx("[\\w\\u0-9\\-\\.\\s]+\\:[0-9-]+\\,");
	//	int pos = 0;

	foreach (QString s, dataList) {
		s.replace("\\u00e4", "ä");
		s.replace("\\u00C4", "Ä");
		s.replace("\\u00f6", "ö");
		s.replace("\\u00d6", "Ö");
		s.replace("\\u00fc", "ü");
		s.replace("\\u00dc", "Ü");
		s.replace("\\u00df", "ß");

		holidays.insert(QDate::fromString(s.split(':').at(1),
													 "yyyy-MM-dd"), s.split(':').at(0));
//		INFO << tr("At") << holidays.lastKey() << tr("it's:") << holidays.last();
	}

	delete manager;
}


