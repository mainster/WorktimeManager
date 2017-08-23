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
protected:
	virtual void mousePressEvent(QMouseEvent *) override;

private:
	virtual void paintEvent(QPaintEvent* e) override;

	QPixmap itsPixmap;
	QString itsMessage;
	int itsAlignment;
	QColor itsColor;

	// QWidget interface
};

#endif // MDSPLASHSCREEN_H
