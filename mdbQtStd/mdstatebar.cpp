#include "mdstatebar.h"

/* --------------------------------------------------------------------------
/         slInfo                   slMsg                      slErr			 */
/* --------------------------------------------------------------------------
|     Long time msg     |     msg changes all 5sec       |    Error msg      |
 --------------------------------------------------------------------------- */
#define N_SLOTS	5

MDStateBar *MDStateBar::inst = 0;
int MDStateBar::PRIVATE_REFRESH_INTERVAL = 250;  // [ms]

MDStateBar::MDStateBar(QWidget *parent) : QStatusBar(parent) {
	inst = this;

	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	m_slotMap["info"] = Slot_t(type_info, "Info: ", Qt::AlignLeft, "");
	m_slotMap["msg"] = Slot_t(type_msg, "Message: ", Qt::AlignLeft, "");
	m_slotMap["err"] = Slot_t(type_err, "Error: ", Qt::AlignLeft, "font-weight: bold; color: red;");
	m_slotMap["sum"] = Slot_t(type_sum, QString("Auswahl-%1:   ")
									  .arg(MathSymbolSigma).toStdString().c_str(),
									  Qt::AlignCenter, "");
	m_slotMap["time"] = Slot_t(type_clock, "", Qt::AlignCenter, "");

	QMapIterator<QString, Slot_t> it(m_slotMap);

	while (it.hasNext()) {
		it.next();
		it.value().m_label->setSizePolicy(this->sizePolicy());
		it.value().m_label->setFocusPolicy(Qt::NoFocus);
		this->addWidget(it.value().m_label);
	}

	/* Init clock structure */
	clock.date = QDate::currentDate();
	clock.time = QTime::currentTime();
	m_slotMap["time"].m_var = QVariant::fromValue(clock);

	/* Init private 1/4sec refresh timer */
	timMsg = new QTimer(this);
	timMsg->setInterval(PRIVATE_REFRESH_INTERVAL);
	connect(timMsg, SIGNAL(timeout()), this, SLOT(onCyclic()));
	timMsg->start();
}
MDStateBar *MDStateBar::getObjectPtr() { return inst; }
MDStateBar::~MDStateBar() {}

void MDStateBar::showError(const QString s, const int timeout) {
	m_slotMap["err"].setText(s);
	INFO << s;
	if (!timeout) return;

	QTimer::singleShot(timeout, Qt::CoarseTimer, this, SLOT(clearError()));
}
void MDStateBar::showMessage(const QString s, const int timeout) {
	m_slotMap["msg"].setText(s);
	INFO << s;
	if (!timeout) return;

	QTimer::singleShot(timeout, Qt::CoarseTimer, this, SLOT(clearMessage()));
}
void MDStateBar::showMessage2sec(const QString &s) {
	m_slotMap["msg"].setText(s);
	INFO << s;
	QTimer::singleShot(2000, Qt::CoarseTimer, this, SLOT(clearMessage()));
}
void MDStateBar::showInfo(const QString s, const int timeout) {
	m_slotMap["info"].setText(s);
	INFO << s;
	if (!timeout) return;

	QTimer::singleShot(timeout, Qt::CoarseTimer, this, SLOT(clearInfo()));
}
void MDStateBar::showInSlot(const QVariant var, const char *slot) {
	if (var.canConvert(QMetaType::QTime)) {
		m_slotMap[slot].setText(var.toTime().toString(Qt::TextDate));
		return;
	}

	if (var.canConvert(QMetaType::QDate)) {
		m_slotMap[slot].setText(var.toDate().toString(Qt::TextDate));
		return;
	}

	if (var.canConvert(QMetaType::QReal)) {
		m_slotMap["sum"].setText(QString::number(var.toReal()));
		m_slotMap["sum"].setBold(true);
		QTimer::singleShot(1000, this, SLOT(sumFontWeightNormal()));
		return;
	}

	INFO << tr("Can not decode QVariant message !!!");
}
void MDStateBar::sumFontWeightNormal() {
	m_slotMap["sum"].setBold(false);
}
void MDStateBar::appendError(const QString s, const QString sep) {
	m_slotMap["err"].setText(tr("%1%2").arg(m_slotMap["err"]
									 .m_label->text()).arg(sep + s));
	INFO << s;
}
void MDStateBar::appendMessage(const QString s, const QString sep) {
	m_slotMap["msg"].setText(tr("%1%2").arg(m_slotMap["msg"]
									 .m_label->text()).arg(sep + s));
	INFO << s;
}
void MDStateBar::appendInfo(const QString s, const QString sep) {
	m_slotMap["info"].setText(tr("%1%2").arg(m_slotMap["info"]
									  .m_label->text()).arg(sep + s));
	INFO << s;
}
void MDStateBar::appendInSlot(const QVariant var, const char *slot, const QString sep) {
	if (var.canConvert(QMetaType::QTime)) {
		m_slotMap[slot].appendText(sep + var.toTime().toString(Qt::TextDate));
		return;
	}

	if (var.canConvert(QMetaType::QDate)) {
		m_slotMap[slot].appendText(sep + var.toDate().toString(Qt::TextDate));
		return;
	}

	if (var.canConvert(QMetaType::QString)) {
		m_slotMap[slot].appendText(sep + var.toString());
		return;
	}
}
void MDStateBar::clearError() {
	m_slotMap["err"].clear();
}
void MDStateBar::clearMessage() {
	m_slotMap["msg"].clear();
}
void MDStateBar::clearInfo() {
	m_slotMap["info"].clear();
}
void MDStateBar::clearClock() {
	m_slotMap["clock"].clear();
}
void MDStateBar::onCyclic() {
	clock.time = QTime::currentTime();
	clock.date = QDate::currentDate();
	showInSlot(clock.time, "time");
}


QTextEdit *MDStateBar::getTeStat() const { return new QTextEdit(); }
void MDStateBar::setTeStat(QTextEdit *value) { Q_UNUSED(value); }
