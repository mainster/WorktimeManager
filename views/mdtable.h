#ifndef MDTABLE_H
#define MDTABLE_H

#include <QObject>
#include <QWidget>
#include <QGroupBox>

#include "mdtabview.h"

class MdTable : public QWidget {

	Q_OBJECT
	Q_PROPERTY(bool selected READ isSelected WRITE setSelected NOTIFY selectedChanged)

public:
	int mouseWheelCnt;

	explicit MdTable(const QString &tvObjName,
						  const QString &tableName = QString(),
						  QWidget *parent = 0);

	bool isSelected()	{ return m_selected; }
	void setSelected(bool selected = true) {
		m_selected = selected;
		m_gb->setProperty("selected", m_selected);
#ifdef SET_STYLESHEETS
		setStyleSheet(styleSheet());
#endif
		emit selectedChanged(selected);
	}
	void clearSelected() {
		m_selected = false;
		if (m_gb)
			m_gb->setProperty("selected", m_selected);
		setStyleSheet(styleSheet());
	}

	MdTabView *tv() const { return m_tv; }
	QGroupBox *gb() const { return m_gb; }

signals:
	void selectedChanged(bool isSelected);
	void groupBoxMouseButtonPress(MdTable *sender);
public slots:

protected:
	const static QString StyleSheet_QGroupBox;

	bool eventFilter(QObject *obj, QEvent *e);

protected slots:
	void onActSectionMask(bool sectionMask);
	void onTabViewObjectNameChanged(const QString &objNam);

private:
	QGroupBox	*m_gb;
	MdTabView	*m_tv;
	SectionMask	*mSectMsk;
	bool			m_selected;


};

#endif // MDTABLE_H
