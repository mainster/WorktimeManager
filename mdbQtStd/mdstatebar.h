#ifndef MDSTATEBAR_H
#define MDSTATEBAR_H

#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#include <QDebug>
#include <QLabel>
#include <QLayout>
#include <QList>
#include <QStatusBar>
#include <QTextEdit>
#include <QTimer>
#include <QVector>
#include <QWidget>
#include <QMap>
#include <QPalette>

#include "globals.h"
#include "debug.h"


class MDStateBar : public QStatusBar {
	Q_OBJECT

public:
	enum msgType {
		type_info = 0,
		type_msg = 1,
		type_err = 2,
		type_sum = 3,
		type_clock = 4,
	};
	Q_DECLARE_FLAGS(msgTypes, msgType)

	typedef struct {
		 QDate date;
		 QTime time;
	} clock_t;
	clock_t clock;

	struct Slot_t {
		Slot_t() { m_label = new QLabel(); }
		Slot_t(uint index,
				 QString prefix = QString(),
				 Qt::Alignment align = Qt::AlignLeft,
				 QString styleSheet = QString())
			: m_index(index),
			  m_prefix(prefix),
			  m_align(align) {
			m_label = new QLabel();
			m_label->setAlignment(align);
//			m_label->setFocusPolicy(Qt::NoFocus);
//			m_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			m_label->setStyleSheet(styleSheet);
		}
		void setText(const QString s) {
			m_label->setText(m_prefix + s);
		}
		void setBold(const bool on) {
			m_label->setText(m_label->text().remove("<b>").remove("</b>"));

			if (on) {
				QStringList strLst = m_label->text().split(m_prefix, QString::KeepEmptyParts);
				if (strLst.length() == 2)
					m_label->setText(m_prefix + QString("<b>%1</b>").arg(strLst.at(1)));
				else
					INFO << "Error, only one split port!!!";
			}
		}
		void appendText(const QString s) {
			m_label->setText(m_label->text() + s);
		}
		void clear(void) {
			m_label->clear();
			m_label->setText(m_prefix);
		}

	public:
		uint m_index;
		QString m_prefix;
		Qt::Alignment m_align;
		QString m_style;
		QLabel *m_label;
		QVariant m_var;
	};
	QMap<QString, struct Slot_t> m_slotMap;

	explicit MDStateBar(QWidget *parent = 0);
	static MDStateBar *instance(QWidget *parent = 0) {
		if (inst == 0)
			inst = new MDStateBar(parent);
		return inst;
	}
	~MDStateBar();

	QTextEdit *getTeStat() const;
	void setTeStat(QTextEdit *value);
	static MDStateBar *getObjectPtr();

public slots:
	void showError(const QString s, const int timeout = 0);
	void showMessage2sec(const QString &s);
	void showMessage(const QString s, const int timeout = 0);
	void showInfo(const QString s, const int timeout);
	void showInfo(QString &s) {
		showInfo(s, 0);
	}
	void showInSlot(const QVariant var, const char *slot);
	void sumFontWeightNormal();
	void appendError(const QString s, const QString sep = " ");
	void appendMessage(const QString s, const QString sep = " ");
	void appendInfo(const QString s, const QString sep = " ");
	void appendInSlot(const QVariant var, const char *slot, const QString sep);
	void clearError();
	void clearMessage();
	void clearInfo();
	void clearClock();

private slots:
	void onCyclic();

private:
	static int PRIVATE_REFRESH_INTERVAL;
	static MDStateBar *inst;
	QTimer *timMsg;
};

Q_DECLARE_METATYPE(MDStateBar::Slot_t)
Q_DECLARE_METATYPE(MDStateBar::clock_t)
Q_DECLARE_OPERATORS_FOR_FLAGS(MDStateBar::msgTypes)

#endif // MDSTATEBAR_H
