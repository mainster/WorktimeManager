#ifndef MDTABLE_H
#define MDTABLE_H

#include <QActionGroup>
#include <QCloseEvent>
#include <QColor>
#include <QDockWidget>
#include <QFont>
#include <QFrame>
#include <QGroupBox>
#include <QKeyEvent>
#include <QList>
#include <QMessageBox>
#include <QObject>
#include <QSet>
#include <QSqlRelationalTableModel>
#include <QSqlTableModel>
#include <QTableView>
#include <QTableWidget>
#include <QtCore>
#include <QtEvents>
#include <QtGui>
#include <QtSql>
#include <QtWidgets>
#include <QWidget>

#include "globals.h"
#include "connectionwidget.h"
#include "models.h"
#include "sectionmask.h"


//class Browser;

/* ======================================================================== */
/*                             class MdTabView                              */
/* ======================================================================== */
class MdTabView : public QTableView {

	Q_OBJECT
	Q_PROPERTY(bool selected READ isSelected WRITE setSelected NOTIFY selectedChanged)
	Q_PROPERTY(QString sqlTableName READ sqlTableName WRITE setSqlTableName NOTIFY sqlTableNameChanged)
	Q_PROPERTY(QString title READ title WRITE setTitle)

public:
	explicit MdTabView(const QString &tableName, QWidget *parent = 0)
		: QTableView(parent) {

		addActions( createActions() );
		connectActions();

		connect(this,			&MdTabView::objectNameChanged,
				  this,			&MdTabView::onObjectNameChanged);

		lblTitle = new QLabel(tableName, this);
		QGridLayout *gl = new QGridLayout(this);

		gl->addWidget(lblTitle);
		setLayout(gl);
		setSortingEnabled( true );
		clearSelected();

		installEventFilter(this);
//		m_gb->installEventFilter(this);

//		connect(horizontalHeader(),		&QHeaderView::sectionMoved,
//				  this,							&MdTabView::onSectionMoved);

		restoreFont();

//		QTimer::singleShot(500, this, SLOT(restoreActionObjects()));
		show();
	}
	~MdTabView() { }

	/* ======================================================================== */
	/*                            Getters / Setters                             */
	/* ======================================================================== */
	bool isSelected()				{ return m_selected; };
	void setSelected(bool selected = true) {
		m_selected = selected;
//		m_gb->setProperty("select", m_selected);
#ifdef SET_STYLESHEETS
		setStyleSheet(styleSheet());
#endif
		emit selectedChanged(selected);
	};
	void clearSelected() {
		m_selected = false;
//		m_gb->setProperty("select", m_selected);
		setStyleSheet(styleSheet());
	};
	QString &sqlTableName()		{ return m_sqlTableName; };
	void setSqlTableName(const QString &name) {
		m_sqlTableName = name;
		emit sqlTableNameChanged(name);
	};
	QString title() const { return lblTitle->text(); }
	void setTitle(const QString title)  { lblTitle->text() = title; }

public slots:
	void createForeignTableDERIVED(const QString &tNam/*, QTableView *tvc*/) {
		/**
		 * Get active database pointer
		 */
		//		ConnectionWidget *connectionWidget = ConnectionWidget::instance();
		QSqlDatabase pDb = ConnectionWidget::instance()->currentDb();
		SqlRtm *rmod = new SqlRtm(SqlRtm::srcNew, this, pDb);
		rmod->setEditStrategy(QSqlTableModel::OnFieldChange);
		rmod->setTable(pDb.driver()->escapeIdentifier(tNam, QSqlDriver::TableName));
		/**
		 * Because the target database contains more than one table related on
		 * foreign keys, varying process is necessary to set the different
		 * relations correctly. Which processing branch must be used depends on
		 * the table name passed by sqlTbl
		 */
		/** Processing branch for table ... */
		while (1) {
			/* --------------------------------------------------------- */
			/*                      Table worktimes                      */
			/* --------------------------------------------------------- */
			if (tNam.contains("worktime", Qt::CaseInsensitive)) {
				/** Remember the indexes of the columns */
				int colEmp = rmod->fieldIndex("workerID");
				int colPrj = rmod->fieldIndex("prjID");

				/** Set the relations to the other database tables */
				rmod->setRelation(colEmp, QSqlRelation(
											"worker", "workerID", "Nachname"));
				rmod->setRelation(colPrj, QSqlRelation(
											"prj", "prjID", "Beschreibung"));

				// Set the localized header captions
				rmod->setHeaderData(rmod->fieldIndex("worktimID"), Qt::Horizontal, tr("ID"));
				rmod->setHeaderData(rmod->fieldIndex("dat"), Qt::Horizontal, tr("Datum"));
				rmod->setHeaderData(colEmp, Qt::Horizontal, tr("Mitarb"));
				rmod->setHeaderData(colPrj, Qt::Horizontal, tr("Beschreibung"));
				rmod->setHeaderData(rmod->fieldIndex("hours"), Qt::Horizontal, tr("Std"));

				break;
			}
			/* --------------------------------------------------------- */
			/*                      Table projects                       */
			/* --------------------------------------------------------- */
			if (tNam.contains("prj", Qt::CaseInsensitive)) {
				/** Remember the indexes of the columns */
				int colClient = rmod->fieldIndex("clientID");

				/** Set the relations to the other database tables */
				rmod->setRelation(colClient, QSqlRelation(
											"client", "clientID", "Name"));

				// Set the localized header captions
				rmod->setHeaderData(rmod->fieldIndex("prjID"), Qt::Horizontal, tr("ID"));
				rmod->setHeaderData(rmod->fieldIndex("clientID"), Qt::Horizontal, tr("Kunde, Name"));

				break;
			}
			/* --------------------------------------------------------- */
			/*                      Table worker                         */
			/* --------------------------------------------------------- */
			if (tNam.contains("worker", Qt::CaseInsensitive)) {
				// Set the localized header captions
				rmod->setHeaderData(rmod->fieldIndex("workerID"), Qt::Horizontal, tr("ID"));
				rmod->setHeaderData(rmod->fieldIndex("PersonalNr"), Qt::Horizontal, tr("PN"));
				rmod->setHeaderData(rmod->fieldIndex("Stundensatz"), Qt::Horizontal, tr("€/h"));
				break;
			}
			/* --------------------------------------------------------- */
			/*                      Table client                         */
			/* --------------------------------------------------------- */
			if (tNam.contains("client", Qt::CaseInsensitive)) {
				// Set the localized header captions
				rmod->setHeaderData(rmod->fieldIndex("clientID"), Qt::Horizontal, tr("ID"));
				rmod->setHeaderData(rmod->fieldIndex("Nummer"), Qt::Horizontal, tr("Knd. #"));
				rmod->setHeaderData(rmod->fieldIndex("Stundensatz"), Qt::Horizontal, tr("€/h"));

				break;
			}
			/* --------------------------------------------------------- */
			/*                      Table fehlzeit                       */
			/* --------------------------------------------------------- */
			if (tNam.contains("fehlzeit", Qt::CaseInsensitive)) {
				// Set the localized header captions
				rmod->setHeaderData(rmod->fieldIndex("fehlID"), Qt::Horizontal, tr("ID"));

				break;
			}
			/* --------------------------------------------------------- */
			/*                      Table arch									 */
			/* --------------------------------------------------------- */
			if (tNam.contains("arch", Qt::CaseInsensitive)) {
				// Set the localized header captions
				rmod->setHeaderData(rmod->fieldIndex("archID"), Qt::Horizontal, tr("ID"));

				break;
			}
			/* --------------------------------------------------------- */
			/*                      Table Unknown                        */
			/* --------------------------------------------------------- */
			QMessageBox::warning(this, tr("Warnung"),
										tr("Unbekannter SQL Tabellenbezeichner: %1")
										.arg(tNam), QMessageBox::Ok);
			break;
		}

		// Populate the model
		rmod->select();

		// Set the model and hide the ID column
		setSelectionMode(QAbstractItemView::ContiguousSelection);
		setModel(rmod);
		setEditTriggers( QAbstractItemView::DoubleClicked |
							  QAbstractItemView::EditKeyPressed );

		for (int k = 0; k < rmod->columnCount(); k++)
			rmod->sectionIdxs().append( k );

		/** !!!!!!!!!!!!!!!!!!!! tvs()->first()->tv()->selectionModel() changed to
		  * mTvs[3]->tv()->selectionModel()
		 */

		if (! (bool) connect( selectionModel(),
									 SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
									 this, SLOT(currentChanged(QModelIndex,QModelIndex)) ) ) {
			//			emit stateMsg(tr("Slot connection returns false"));
			INFO << tr("Slot connection returns false");
		}
		resizeRowsColsToContents();

		//    emit updateActions();
		/**
		 * Update MdTables group box title to match the prior activated sql
		 * table name
		 */

		//		grBox()->setTitle(tNam);
		setTitle(tNam);

		/*!
		 * Set tvcs object name to grBox()->title.
		 */
		setSqlTableName(tNam);
		//	setObjectName(grBox()->title());

		//		connect(this, &Browser::clearSelections,		tvc, &MdTable::clearSelected);
		connect(this, &MdTabView::sqlTableNameChanged,	 this, &MdTabView::onSqlTableNameChanged);
		//	connect(this, &Browser::updateWriteActions,	tvc, &MdTabView::onUpdateWriteActions);

		//	restoreColumnOrderAndVisability();
		setFont(restoreFont());

	}
	void onUpdateWriteActions() {
		/*!
		 * Check if there is a valid model set before calling update procedure.
		 */

		SqlRtm *rtm = static_cast<SqlRtm *>(model());
		if (! rtm)	qReturn(tr("cast failed: SqlRtm *rtm = static_cast<SqlRtm *>( %1 );")
								  .arg(model()->metaObject()->className()));

		//	INFO << m_sqlTableName << tr("currentIndexIsValied:")
		//		  << currentIndex().isValid();

		actInsertRow->setEnabled(currentIndex().isValid());
		actDeleteRow->setEnabled(currentIndex().isValid());
	}

signals:
	void sqlTableNameChanged(const QString &name);
	void selectedChanged(bool isSelected);

protected slots:
	QList<QAction *> createActions() {
		actInsertRow =		new QAction(tr("&Insert Row"), this);
		actDeleteRow =		new QAction(tr("&Delete Row"), this);
		actSubmit =			new QAction(tr("&Submit All"), this);
		actRevert =			new QAction(tr("Re&vert All"), this);
		actSelect =			new QAction(tr("S&elect"), this);
		actSectionMask =	new QAction(tr("Spaltenmaske"), this);
		actFieldStrategy =	new QAction(tr("Submit on &Field Change"), this);
		actRowStrategy =		new QAction(tr("Submit on &Row Change"), this);
		actManualStrategy =	new QAction(tr("Submit &Manually"), this);

		/* ======================================================================== */
		/*                             Action grouping                              */
		/* ======================================================================== */
		/*!
		 * Create action group for strategy actions
		 */
		actGrStrategy = new QActionGroup(this);
		PONAM(actGrStrategy)->setExclusive(true);

		QList<QAction *> acts;
		acts << PONAM(actFieldStrategy) << PONAM(actRowStrategy)
			  << PONAM(actManualStrategy);

		foreach (QAction *act, acts) {
			actGrStrategy->addAction(act);
			act->setCheckable(true);
		}

		/*!
		 * Connect action group triggered signal to a common slot.
		 */
		connect(actGrStrategy, &QActionGroup::triggered, this, &MdTabView::storeActionState);
//		connect(actGrStrategy, &QActionGroup::triggered, this, &MdTabView::onActGrStrategyTrigd);

		/* ======================================================================== */

		/*!
		 * Create action group for context menu actions.
		 */
		actGrContext = new QActionGroup(this);
		PONAM(actGrContext)->setExclusive(false);

		QList<QAction *> acts2;
		acts2 << PONAM(actInsertRow) << PONAM(actDeleteRow) << PONAM(actSubmit)
				<< PONAM(actRevert)  << PONAM(actSelect);

		foreach (QAction *act, acts2)
			actGrContext->addAction(act);

		/*!
		 * Connect action group triggered signal to a save-action-state slot.
		 */
		connect(actGrContext, &QActionGroup::triggered, this, &MdTabView::storeActionState);
//		connect(actGrContext, &QActionGroup::triggered, this, &MdTabView::onActGrContextTrigd);

		/* ======================================================================== */
		/*                       Action object configurations                       */
		/* ======================================================================== */
		actSectionMask->setCheckable(true);

		//	tblActs = findChildren<QAction *>(QString(), Qt::FindDirectChildrenOnly);

		QAction *sep1 = new QAction(this);
		sep1->setSeparator(true);
		QAction *sep2 = new QAction(this);
		sep2->setSeparator(true);

		acts2 << sep1 << acts << sep2 << actSectionMask;
		return acts2;
	}
	void connectActions() {
//		connect(actSectionMask, &QAction::toggled, this, &MdTabView::onActSectionMask);
	}
	void onObjectNameChanged(const QString &objNam) {
		setTitle( objNam );
		//	m_gb->setAccessibleName(tr("gboxOf")+objNam);
		//	m_tv->setAccessibleName(objNam);
	}
	void onSqlTableNameChanged(const QString &name) {
		WARN << name;
	}
	/* ======================================================================== */
	/*                              Event handler                               */
	/* ======================================================================== */
	void showEvent(QShowEvent *) {
	}
	void mouseDoubleClickEvent(QMouseEvent *) {
		INFO << m_sqlTableName << tr("double");
	}
	bool eventFilter(QObject *obj, QEvent *event) {
		QMouseEvent *mouseEv = static_cast<QMouseEvent *>(event);
		if (! mouseEv)
			return QObject::eventFilter(obj, event);

		if (obj->objectName().contains(tr("mtv")))
			if (mouseEv->type() == QEvent::MouseButtonDblClick) {
				actSectionMask->trigger();
				//			INFO << tr("Double click:") << obj;
				return true;
			}
		return QObject::eventFilter(obj, event);
	}
	void hideEvent(QHideEvent *) {
//		modelCast()->storeModel(sqlTableName());
	}
	/* ======================================================================== */
	/*                             Helper methodes                              */
	/* ======================================================================== */
	void setAlternateRowCol(QColor &col, bool alternateEnabled) {
//		m_tv->setAlternatingRowColors(alternateEnabled);

		if (alternateEnabled && col.isValid()) {
			/*m_tv->*/setStyleSheet(
						QString("alternate-background-color: rgba(%1,%2,%3,%4);")
						.arg( col.red())
						.arg( col.green())
						.arg( col.blue())
						.arg( col.alpha()) );
			QSETTINGS;
			config.setValue(objectName() + "/AlternateRowColEnable", alternateEnabled);
			config.setValue(objectName() + "/AlternateRowColor", col);
		}
	}
	void storeFont(QFont f) {
		QSETTINGS;
		config.setValue(objectName() + Md::k.tableFont, QVariant::fromValue(f));
		setFont(f);
	}
	QFont restoreFont() {
		QSETTINGS;
		QFont f(config.value(objectName() + Md::k.tableFont).value<QFont>());
		INFO << tr("Restoring MdTable::Font:") << f;
		setFont(f);
		return f;
	}
	void storeActionState(QAction *sender) {
		QSETTINGS;

		if (sender->isCheckable()) {
			/*!
			 * If action group is exclusive, clear all action config flags.
			 */
			if (sender->actionGroup()->isExclusive())
				foreach(QAction *act, sender->actionGroup()->actions())
					config.setValue(objectName() + tr("/") + act->objectName(), false);

			config.setValue(objectName() + tr("/") + sender->objectName(), sender->isChecked());
			config.sync();
		}
	}
	void resizeRowsColsToContents() {
		setVisible( false );
		resizeColumnsToContents();
		resizeRowsToContents();
		setVisible( true );
	}

private:
	SectionMask			*mSectMsk;
	QString				m_sqlTableName;
	bool					m_selected;
	QActionGroup		*actGrStrategy, *actGrContext;
	QAction				*actInsertRow, *actDeleteRow, *actFieldStrategy,
	*actRowStrategy, *actManualStrategy,*actSubmit, *actRevert, *actSelect, *actSectionMask;
	QLabel				*lblTitle;
};

/* ======================================================================== */
/*                              class MdTable                               */
/* ======================================================================== */

namespace Ui {
class MdTable;
//class Browser;
}

class MdTable : public QWidget {

	Q_OBJECT
	Q_PROPERTY(bool selected READ isSelected WRITE setSelected NOTIFY selectedChanged)
	Q_PROPERTY(QString sqlTableName READ sqlTableName WRITE setSqlTableName NOTIFY sqlTableNameChanged)

public:
	explicit MdTable(QWidget *parent = 0);
	~MdTable();

	void setSelectionMode(QAbstractItemView::SelectionMode mode) {
		m_tv->setSelectionMode(mode);
	}
	QItemSelectionModel *selectionModel() {
		return m_tv->selectionModel();
	}

	/* ======================================================================== */
	/*                            Getters / Setters                             */
	/* ======================================================================== */
	QGroupBox *grBox() const	{ return m_gb; }
	QTableView *tv() const		{ return m_tv; }
	bool isSelected()				{ return m_selected; };
	void setSelected(bool selected = true) {		
		m_selected = selected;
		m_gb->setProperty("select", m_selected);
#ifdef SET_STYLESHEETS
		setStyleSheet(styleSheet());
#endif
		emit selectedChanged(selected);
	};
	void clearSelected() {
		m_selected = false;
		m_gb->setProperty("select", m_selected);
		setStyleSheet(styleSheet());
	};
	QString &sqlTableName()		{ return m_sqlTableName; };
	void setSqlTableName(const QString &name) {
		m_sqlTableName = name;
		emit sqlTableNameChanged(name);
	};

	SqlRtm *clearMdlSrces();

public slots:
	QFont restoreFont();
	SqlRtm *modelCast();
	void deleteRow();
	void insertRow();
	void onObjectNameChanged(const QString &objNam);
	void onSectionMoved(int logicalIdx, int oldVisualIdx, int newVisualIdx);
	void onSqlTableNameChanged(const QString &name);
	void onUpdateWriteActions();
	void refreshView();
	void removeColumnsConfig();
	void resetColumnsDefaultPos(bool allVisible);
	void resizeRowsColsToContents();
	void restoreColumnOrderAndVisability();
	void restoreView();
	void setAlternateRowCol(QColor &col, bool alternateEnabled = true);
	void setColumnHidden(const int column, const bool hide);
	void setEditTriggers(QTableView::EditTriggers triggers);
	void setModel(QAbstractItemModel *model);
	void storeFont(QFont font);

signals:
	void sqlTableNameChanged(const QString &name);
	void selectedChanged(bool isSelected);

protected:
	QList<QAction *> createActions();
	void connectActions();
	virtual void showEvent(QShowEvent *) override;
	virtual void mouseDoubleClickEvent(QMouseEvent *) override;
	bool eventFilter(QObject *obj, QEvent *event);
	void hideEvent(QHideEvent *);
	void restoreColumnOrderAndVisability2();

protected slots:
	void onClearSelection() {

	}
	void onActSectionMask(bool sectionMask = false);
	void onActGrStrategyTrigd(QAction *sender);
	void onActGrContextTrigd(QAction *sender);
	void storeActionState(QAction *sender);
	bool restoreActionObjects();

private:
	Ui::MdTable			*ui;
	QTableView        *m_tv;
	QGroupBox         *m_gb;
	SectionMask			*mSectMsk;
	QString				m_sqlTableName;
	bool					m_selected;
	QActionGroup		*actGrStrategy, *actGrContext;
	QAction				*actInsertRow, *actDeleteRow, *actFieldStrategy,
	*actRowStrategy, *actManualStrategy,*actSubmit, *actRevert, *actSelect, *actSectionMask;

};



#endif // MDTABLE_H
