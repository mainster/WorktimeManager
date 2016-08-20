#ifndef TABVIEWB_H
#define TABVIEWB_H

#include <QActionGroup>
#include <QColor>
#include <QGroupBox>
#include <QKeyEvent>
#include <QObject>
#include <QSet>
#include <QSqlTableModel>
#include <QTableView>
#include <QWidget>

#include "globals.h"
#include "connectionwidget.h"
#include "models.h"
#include "sectionmask.h"

//#ifndef TABVIEW_H
//#define TABVIEW_H

//#include <QWidget>
//#include <QMainWindow>
//#include <QVariant>
//#include <QtWidgets>
//#include <QTableView>
//#include <QObject>
//#include <QModelIndexList>
//#include <QModelIndex>
//#include <QMetaType>
//#include <QInputDialog>
//#include <QColorDialog>
//#include <QColor>

//#include "tvm.h"
//#include "globals.h"
//#include "types.h"
//#include "nfctag.h"
//#include "table.h"
//#include "delegate.h"
//#include "dialog.h"
//#include "types.h"
//#include "decoder.h"
//#include "ctrls.h"

//class Table;
//class Dialog;
//class Delegate;
//class Ctrls;

/* ======================================================================== */
/*                     QTableView subclass                                  */
/* ======================================================================== */
class TabViewB : public QTableView {

	Q_OBJECT

public:
	/**
	 * @brief The XML struct holds some const strings representing XML tags.
	 */
	static const struct XML{
		struct IDENT {
			QString ID, TSTAMP, RSSI;
		} IDENT;

		struct MODEL {
			QString ROOT, ROWS, COLS, FRAM, BGCOL, ALIGN;
		} MODEL;
	} XML;

	static const QSize
	DEFAULT_HHEAD_MIN_SIZE,
	DEFAULT_VHEAD_MIN_SIZE;
	static const int
	DEFAULT_HHEAD_SECTION_SIZE,
	DEFAULT_VHEAD_SECTION_SIZE,
	MAX_WINDOW_HEIGHT;
	static const QString
	DEFAULT_CELL_BOARDER_STYLE,
	DEFAULT_GRID_STYLESHEET;

	/* ======================================================================== */

	explicit TabViewB(uID_t uID, int initRows, int initCols,
						  QWidget *parent = 0, Qt::WindowFlags f=0);
	~TabViewB();
	void setupContextMenu(QTableView *_w);
	void createActions();
	void updateColor(QModelIndex midx);
	void setupContextMenu();
	bool getDoCompare();
	/*!
	 * \brief TabViewB::setColor is used to customize items. The role must be set in order
	 * to manipulate background, boarders ...
	 * \param role
	 */
	void setColor(int role = Qt::DisplayRole);
	static const QString DEFAULT_XML_WRITE;
	static const QString EMPTY_CELL;
	static const QChar ROW_SEP;
	static const QChar COL_SEP;
	QAction *actShowEquals;
	SqlRtm *getTm() const;
	void setTm(SqlRtm *tm);
	void paintEventM(QPaintEvent *e);
	virtual void keyReleaseEvent(QKeyEvent *e) override;
	void setCustomDelegate(const bool b);
	QAction *getActExportXml() const { return actExportXml; }
	QSize sizeHint() const;
	void initHeaders();
	void onTimeout();
	QAction *actExportXml;
	void keyPressEvent(QKeyEvent *event);
	void setRangeColorOld(const QVariant rng, QColor color,
								 Qt::ItemDataRole role = Qt::BackgroundRole);

public slots:
//	bool initBground(Decoder *dec = 0);
	bool modelToXml(QString filePath = QString());
	bool xmlToModel(QString filePath = QString());
	void adjustTableGeometry();
	void selectFont();
	void clear();
	void delegateSignals(const bool b);
	void setBgColor();
	void setBoarderColor();

private:
	SqlRtm	*mTvm;
//	Dialog *dialog;
	QAction *actBgColor, *actBoarderColor, *actFont, *actImportXml,
	*actClear, *actFirstSep, *actSecondSep, *actFitToCont, *actDelegBlk, *actUseDefaults,
	*actInitBground, *actSetParent;

//	Delegate *delegate;
//	QAbstractItemDelegate *defaultDelegate;

//	Ctrls *ctrl;
};


#endif // TABVIEWB_H
