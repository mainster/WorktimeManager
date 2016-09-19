#include <QPainter>
#include "mdsplashscreen.h"


MdSplashScreen::MdSplashScreen(const QPixmap &pixmap)
	: QFrame(0, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint),
	  itsPixmap(pixmap) {
	setAttribute(Qt::WA_TranslucentBackground);
	setFixedSize(itsPixmap.size());
}
void MdSplashScreen::clearMessage() {
	itsMessage.clear();
	repaint();
	hide();
}
void MdSplashScreen::showMessage(const QString &message,
											int alignment,
											const QColor &color) {
	itsMessage = message;
	itsAlignment = alignment;
	itsColor = color;
	repaint();
}
void MdSplashScreen::paintEvent(QPaintEvent *e) {
	Q_UNUSED(e);
	QRect aTextRect(rect());
	aTextRect.setRect(aTextRect.x() + 5, aTextRect.y() + 5,
							aTextRect.width() - 10, aTextRect.height() - 10);

	QPainter aPainter(this);
	aPainter.drawPixmap(rect(), itsPixmap);
	aPainter.setPen(itsColor);
	aPainter.drawText(aTextRect, itsAlignment, itsMessage);
}
