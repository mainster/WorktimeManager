#include "mdstatebar.h"

/* --------------------------------------------------------------------------
/         slInfo                   slMsg                      slErr			 */
/* --------------------------------------------------------------------------
|     Long time msg     |     msg changes all 5sec       |    Error msg      |
 --------------------------------------------------------------------------- */
#define N_MSG_SLOTS 3
#define CLOCK_SLOT 1

MDStateBar *MDStateBar::inst = 0;
int MDStateBar::PRIVATE_REFRESH_INTERVAL = 250;  // [ms]

MDStateBar::MDStateBar(QWidget *parent) : QStatusBar(parent) {
	inst = this;

	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	for (int i = 0; i < N_MSG_SLOTS + CLOCK_SLOT; i++)
		mslot.append(new QLabel(parent));

	int k = 0;
	QList<QPalette> pals;

	foreach(QLabel * l, mslot) {
		l->setFocusPolicy(Qt::NoFocus);
		l->setSizePolicy(this->sizePolicy());
		pals.append(l->palette());
		this->addWidget(l);
	}

	QColor col;
	col.setRgb(25, 5, 5, 50);
	pals[type_info].setColor(QPalette::Background, col);

	col.setRgb(25, 5, 5, 100);
	pals[type_msg].setColor(QPalette::Background, col);

	col.setRgb(25, 5, 5, 150);
	pals[type_err].setColor(QPalette::Background, col);

	k = 0;
	foreach(QLabel * l, mslot)
		l->setPalette(pals[k++]);

	prefix.str.append(QString("Info: "));
	prefix.str.append(QString("Message: "));
	prefix.str.append(QString("Error: "));
	prefix.str.append(QString("Time: "));

	/* Init clock structure */
	clock.date = QDate::currentDate();
	clock.time = QTime::currentTime();
	clock.slotNo = N_MSG_SLOTS + CLOCK_SLOT;
	clock.visible = true;

	/* Init private 1/4sec refresh timer */
	timMsg = new QTimer(this);
	timMsg->setInterval(PRIVATE_REFRESH_INTERVAL);
	connect(timMsg, SIGNAL(timeout()), this, SLOT(onCyclic()));
	timMsg->start();
}
MDStateBar *MDStateBar::getObjectPtr() { return inst; }
MDStateBar::~MDStateBar() {}

void MDStateBar::showError(const QString s, int timeout) {
	mslot[type_err]->setText(prefix.str[type_err] + s);
	Q_INFO << s;
	if (!timeout) return;

	QTimer::singleShot(timeout, Qt::CoarseTimer, this, SLOT(clearError()));
}
void MDStateBar::showMessage(QString s, int timeout) {
	mslot[type_msg]->setText(prefix.str[type_msg] + s);
	Q_INFO << s;
	if (!timeout) return;

	QTimer::singleShot(timeout, Qt::CoarseTimer, this, SLOT(clearMessage()));
}
void MDStateBar::showInfo(const QString s, int timeout) {
	mslot[type_info]->setText(prefix.str[type_info] + s);
	Q_INFO << s;
	if (!timeout) return;

	QTimer::singleShot(timeout, Qt::CoarseTimer, this, SLOT(clearInfo()));
}
void MDStateBar::showInSlot(const QVariant var, int slotNo) {
	/* Index of message slot starts counting from 1, not 0 */
	slotNo--;

	if (slotNo > N_MSG_SLOTS + CLOCK_SLOT) {
		Q_INFO << tr("Message slot index %1 out of bounds!!! mslot.length(): %2")
				.arg(slotNo)
				.arg(mslot.length());
		return;
	}

	if (var.canConvert(QMetaType::QTime)) {
		mslot[slotNo]
				->setText(prefix.str[type_clock] + var.toTime().toString(Qt::TextDate));
	} else {
		if (var.canConvert(QMetaType::QDate)) {
			mslot[slotNo]->setText(prefix.str[type_clock] +
										  var.toDate().toString(Qt::TextDate));
			return;
		} else {
			Q_INFO << tr("Can not decode QVariant message !!!");
			return;
		}
	}

	//    if (! timeout)
	//        return;

	//    QTimer::singleShot(timeout, Qt::CoarseTimer,this, SLOT(clearInfo()));
}
void MDStateBar::appendError(const QString s, const QString sep) {
	mslot[type_err]
			->setText(tr("%1%2").arg(mslot[type_err]->text()).arg(sep + s));
	Q_INFO << s;
}
void MDStateBar::appendMessage(const QString s, const QString sep) {
	mslot[type_msg]
			->setText(tr("%1%2").arg(mslot[type_msg]->text()).arg(sep + s));
	Q_INFO << s;
}
void MDStateBar::appendInfo(const QString s, const QString sep) {
	mslot[type_info]
			->setText(tr("%1%2").arg(mslot[type_info]->text()).arg(sep + s));
	Q_INFO << s;
}
void MDStateBar::appendInSlot(const QVariant var, int slotNo,
										const QString sep) {
	QString decod;

	/* Index of message slot starts counting from 1, not 0 */
	slotNo--;

	if (slotNo > N_MSG_SLOTS + CLOCK_SLOT) {
		Q_INFO << tr("Append message slot index %1 out of bounds!!! "
						 "mslot.length(): %2")
				.arg(slotNo)
				.arg(mslot.length());
		return;
	}

	if (var.canConvert(QMetaType::QTime)) {
		decod = var.toTime().toString(Qt::TextDate);
	} else {
		if (var.canConvert(QMetaType::QDate)) {
			decod = var.toDate().toString(Qt::TextDate);
			return;
		} else {
			Q_INFO << tr("Can not decode QVariant append message !!!");
			return;
		}
	}

	mslot[slotNo]
			->setText(prefix.str[type_clock] + mslot[slotNo]->text() + sep + decod);
}
void MDStateBar::clearError() {
	mslot[type_err]->clear();
	mslot[type_err]->setText(prefix.str[type_err]);
}
void MDStateBar::clearMessage() {
	mslot[type_msg]->clear();
	mslot[type_msg]->setText(prefix.str[type_msg]);
}
void MDStateBar::clearInfo() {
	mslot[type_info]->clear();
	mslot[type_info]->setText(prefix.str[type_info]);
}
void MDStateBar::clearClock() {
	mslot[type_clock]->clear();
	mslot[type_clock]->setText(prefix.str[type_clock]);
}
void MDStateBar::setClockVisible(const bool onoff, const int slotNo) {
	clock.visible = onoff;

	if (slotNo > -1) clock.slotNo = slotNo;
}
void MDStateBar::onCyclic() {
	clock.time = QTime::currentTime();
	clock.date = QDate::currentDate();

	if (clock.visible) {
		showInSlot(clock.time, clock.slotNo);
		appendInSlot(clock.date, clock.slotNo, tr("  "));
	}

	//    Q_INFO << clock.time << clock.date << tr("visible: ")
	//                         << clock.visible;
}
QTextEdit *MDStateBar::getTeStat() const { return new QTextEdit(); }
void MDStateBar::setTeStat(QTextEdit *value) { Q_UNUSED(value); }
