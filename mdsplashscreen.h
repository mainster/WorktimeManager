#ifndef MDSPLASHSCREEN_H
#define MDSPLASHSCREEN_H

#include <QFrame>
#include <QObject>

class MdSplashScreen : public QFrame {

public:
	MdSplashScreen(const QPixmap& pixmap = QPixmap());

public slots:
	void clearMessage();
	void showMessage(const QString& message,
						  int alignment = Qt::AlignLeft,
						  const QColor& color = Qt::black);

private:
	virtual void paintEvent(QPaintEvent* e);

	QPixmap itsPixmap;
	QString itsMessage;
	int itsAlignment;
	QColor itsColor;
};

#endif // MDSPLASHSCREEN_H
