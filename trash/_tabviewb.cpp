#include "tabviewb.h"
#include <QMetaType>

/**********************************************************************//**
 *		Initialize static const class members
 **************************************************************************/
const QString TabViewB::DEFAULT_GRID_STYLESHEET
= QString("QTableView::item { border-left: %1px solid lightGray;  border-top: %1px solid lightGray;  }"
			 "QTableView{  border-bottom: %1px solid lightGray;  border-right: %1px solid lightGray;  }");
const QString TabViewB::DEFAULT_XML_WRITE = CONFIG_PATH + tr("/xml/");
const QString TabViewB::EMPTY_CELL = tr("");
const QChar TabViewB::ROW_SEP = QChar('\n');
const QChar TabViewB::COL_SEP = QChar(',');
const QSize TabViewB::DEFAULT_HHEAD_MIN_SIZE = QSize(5, 5);
const QSize TabViewB::DEFAULT_VHEAD_MIN_SIZE = QSize(5, 5);
const int TabViewB::DEFAULT_HHEAD_SECTION_SIZE = 10;
const int TabViewB::DEFAULT_VHEAD_SECTION_SIZE = 10;
const QString TabViewB::DEFAULT_CELL_BOARDER_STYLE =
		QString("QTableView::item {  border: 1px solid red;  }");
const int TabViewB::MAX_WINDOW_HEIGHT = 1053;
const struct TabViewB::XML
		TabViewB::XML ({
							  { "nfcID", "timestamp", "rssi" },
							  { "mData", "rows", "cols", "fram", "bgColors", "txtAlign" }
						  });

/**********************************************************************//**
 *		Class constructors
 **************************************************************************/
TabViewB::TabViewB(uID_t uID, int initRows, int initCols,
					  QWidget *parent, Qt::WindowFlags /*f*/)
	: QTableView(parent) {

//	mTvm = new SqlRtm(uID, initRows, initCols, true, this);
	mTvm = new SqlRtm(this);
	/*! Set the new model as model for this TabViewB instance */
	setModel(mTvm);

	/*! Setup size policys */
	initHeaders();

	/*! Init Bground marker */
	/*! Called via parent */

	createActions();
	setupContextMenu();
	setSizeAdjustPolicy(QTableView::AdjustToContents);

	/*! Switch off horizonatal scrollbar; though this is not really needed here */
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	/*! Register custom delegate for item customizations */
//	delegate = new Delegate(this);
//	defaultDelegate = itemDelegate();
	setItemDelegate(delegate);
	adjustSize();
	show();


}
TabViewB::~TabViewB() {
	delete defaultDelegate;
	delete delegate;
}

/**********************************************************************//**
 *		Methodes
 **************************************************************************/
void TabViewB::initHeaders() {
	horizontalHeader()->reset();
	horizontalHeader()->setDefaultSectionSize(DEFAULT_HHEAD_SECTION_SIZE);
	horizontalHeader()->setMinimumSectionSize(DEFAULT_HHEAD_SECTION_SIZE);
	horizontalHeader()->setMinimumSize(DEFAULT_HHEAD_MIN_SIZE);
	horizontalHeader()->show();

	verticalHeader()->reset();
	verticalHeader()->setDefaultSectionSize(DEFAULT_VHEAD_SECTION_SIZE);
	verticalHeader()->setMinimumSectionSize(DEFAULT_VHEAD_SECTION_SIZE);
	verticalHeader()->setMinimumSize(DEFAULT_VHEAD_MIN_SIZE);
	verticalHeader()->show();
}
//bool TabViewB::initBground(Decoder *dec) {
	/*!
	 * Cast parent() QObject to Table::Decoder:: object.
	 */
//	Decoder *dec2 = static_cast<Table*>(parent())->mDec;
//	Table *tbl = static_cast<Table*>(parent());

//	if (! dec)	bReturn("dynamic_cast<Table *>(parent())->mDec FAILED!");
//	INFO << dec->m.hist.range.at(0);
//	INFO << dec->m.trend.ptr.blkRef().addr;
//	INFO << tbl->mDec->m.serial.range.at(0);

//	SqlRtm *tm = getTm();
//	ItemStyle (tm->addr2Index(dec->m.serial.range.at(0)), rangeSize(dec->m.serial.range), dec->m.serial.color).debug();
//	ItemStyle (tm->addr2Index(dec->m.runtime.range.at(0)), rangeSize(dec->m.runtime.range), dec->m.runtime.color).debug();
//	ItemStyle (tm->addr2Index(dec->m.trend.ptr.index), 1 ,  dec->m.trend.colors.ptr).debug();
//	ItemStyle (tm->addr2Index(dec->m.hist.ptr.index), 1, dec->m.hist.colors.even).debug();


//	/*!
//	 * Create list of ItemStyle objects for the short range elements.
//	 */
//	QList<ItemStyle> mix({	ItemStyle (tm->addr2Index(dec->m.serial.range.at(0)),
//									rangeSize(dec->m.serial.range), dec->m.serial.color),
//									ItemStyle (tm->addr2Index(dec->m.runtime.range.at(0)),
//									rangeSize(dec->m.runtime.range), dec->m.runtime.color),
//									ItemStyle (tm->addr2Index(dec->m.trend.ptr.index), 1 ,
//									dec->m.trend.colors.ptr),
//									ItemStyle (tm->addr2Index(dec->m.hist.ptr.index), 1,
//									dec->m.hist.colors.even)});
//	/*!
//	 * Define structs of ItemStyle pointers for the multi-long-range items and create instances.
//	 */
//	struct s_itm {  ItemStyle *odd, *even;  };
//		s_itm cTrend, cHist;

//	cTrend.even = new ItemStyle (tm->addr2Index(dec->m.trend.blocks.at(0).addr), 6,
//										 dec->m.trend.colors.even);
//	cTrend.odd = new ItemStyle (tm->addr2Index(dec->m.trend.blocks.at(1).addr), 6,
//										 dec->m.trend.colors.odd);
//	cHist.even = new ItemStyle (tm->addr2Index(dec->m.hist.blocks.at(0).addr), 6,
//										 dec->m.hist.colors.even);
//	cHist.odd = new ItemStyle (tm->addr2Index(dec->m.hist.blocks.at(1).addr), 6,
//										 dec->m.hist.colors.odd);

//	/*!
//	 * Iterate through the blk_t lists of trend/history range and append each blk_t::addr to
//	 * the corresponding ItemStyle structure member.
//	 */
//	QList<Decoder::blk_t>::const_iterator it = dec->m.trend.blocks.begin();
//	for (it++; it < dec->m.trend.blocks.end(); it++) {

//		((it -dec->m.trend.blocks.begin()) % 2)
//				? cTrend.even->appendIndexRng(tm->addr2Index(short((*it).addr)), 6)
//				: cTrend.odd->appendIndexRng(tm->addr2Index(short((*it).addr)), 6);
//	}

//	it = dec->m.hist.blocks.begin();
//	for (it++; it < dec->m.hist.blocks.end(); it++) {

//		((it -dec->m.hist.blocks.begin()) % 2)
//				? cHist.even->appendIndexRng(tm->addr2Index(short((*it).addr)), 6)
//				: cHist.odd->appendIndexRng(tm->addr2Index(short((*it).addr)), 6);
//	}

//	/*!
//	 * Iterate through the short range elements ItemStyle list and invoke the models setData() member.
//	 */
//	foreach (ItemStyle itm, mix)
//		tm->setData(itm.getIndexes().first(), QVariant::fromValue(itm), Qt::BackgroundRole);

//	/*!
//	 * Invoke the models setData() member for each member of the long range structures.
//	 */
//	tm->setData(cTrend.even->getIndexes().first(), QVariant::fromValue(*cTrend.even), Qt::BackgroundRole);
//	tm->setData(cTrend.odd->getIndexes().first(), QVariant::fromValue(*cTrend.odd), Qt::BackgroundRole);
//	tm->setData(cHist.even->getIndexes().first(), QVariant::fromValue(*cHist.even), Qt::BackgroundRole);
//	tm->setData(cHist.odd->getIndexes().first(), QVariant::fromValue(*cHist.odd), Qt::BackgroundRole);

//	/*!
//	 * Instantiate a new ItemStyle and append the two dereferenced pointer items.
//	 */
//	ItemStyle itm(tm->addr2Index(dec->m.trend.ptr.blkRef().addrRng().at(0)),
//					  rangeSize(dec->m.trend.ptr.blkRef().addrRng()), Qt::blue, 3,Qt::DashLine);

//	itm.appendIndexRng(tm->addr2Index(dec->m.hist.ptr.blkRef().addrRng().at(0)),
//							 rangeSize(dec->m.hist.ptr.blkRef().addrRng()));

//	tm->setData(itm.getIndexes().first(), QVariant::fromValue(itm),Qt::BoarderRole);

//	return true;
//}
void TabViewB::clear() {
	mTvm->mSrc.clearModel();
	//	mTvm->mData.clearModel();
	//	mTvm->mData.initModel(7,7);
}
QSize TabViewB::sizeHint() const {
	return QTableView::sizeHint();
}
void TabViewB::adjustTableGeometry() {
	//	disconnect(_tm,		SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
	//				  this,		SLOT(adjustTableSize(QModelIndex,QModelIndex,QVector<int>)));

	resizeColumnsToContents();
	resizeRowsToContents();

	verticalScrollBar()->setEnabled(true);
	verticalScrollBar()->setVisible(true);

	int height =
			verticalHeader()->length() +
			horizontalHeader()->width() +
			horizontalScrollBar()->height();/*+
			ui->gridLayoutPrim->contentsMargins().top() +
			ui->gridLayoutPrim->contentsMargins().bottom();*/

	int width =
			horizontalHeader()->length() +
			verticalHeader()->width() +
			verticalScrollBar()->width();/*+
			ui->gridLayoutPrim->contentsMargins().left() +
			ui->gridLayoutPrim->contentsMargins().right();*/

	if (height > MAX_WINDOW_HEIGHT)
		height = MAX_WINDOW_HEIGHT;

	resize(width, height);
}
void TabViewB::updateColor(QModelIndex midx) {
	QPixmap pix(16, 16);
	QColor col;

	if (midx.isValid())
		col = model()->data(midx, Qt::BackgroundColorRole).value<QColor>();

	if (!col.isValid())
		col = palette().base().color();

	QPainter pt(&pix);
	pt.fillRect(0, 0, 16, 16, col);

	QColor lighter = col.light();
	pt.setPen(lighter);
	QPoint lightFrame[] = {QPoint(0, 15), QPoint(0, 0), QPoint(15, 0)};
	pt.drawPolyline(lightFrame, 3);

	pt.setPen(col.dark());
	QPoint darkFrame[] = {QPoint(1, 15), QPoint(15, 15), QPoint(15, 1)};
	pt.drawPolyline(darkFrame, 3);

	pt.end();

	actBgColor->setIcon(pix);
}
void TabViewB::setBgColor() {
	setColor(Qt::BackgroundColorRole);
}
void TabViewB::setBoarderColor() {
	setColor(Qt::BoarderRole);
}
void TabViewB::setColor(int role) {
	/*!
	 * Get list of selected items .
	 */
	QColor nextColor;
	QColorDialog cDialog(this); cDialog.setOptions( QColorDialog::DontUseNativeDialog );
	cDialog.setCurrentColor( model()->data (currentIndex(), role).value<QColor>() );

	if (! cDialog.currentColor().isValid())
		cDialog.setCurrentColor( QColor(Qt::gray) );

	/*! Invoke QColorDialog::getColor() methode */
	if (! actUseDefaults->isChecked())
		nextColor = QColorDialog::getColor(cDialog.currentColor(), this,
													  cDialog.windowTitle(),
													  cDialog.options());

	/*! Don't change color if nextColor is not valied, canceled dialog -> return */
	if (! nextColor.isValid())		return;

	/*! TODO: Request QPen style */
	/*! Create ItemStyle:: object and write it to the models data source */
	ItemStyle itm = ItemStyle(selectionModel()->selection().indexes(),
									  nextColor, 2, Qt::SolidLine);
	getTm()->setData(currentIndex(), QVariant::fromValue(itm),
						  QFlags<Qt::ItemDataRole>(role));
	//	visualRegionForSelection()

	//	updateColor(mi);
}
SqlRtm* TabViewB::getTm() const {
	return mTvm;
}
void TabViewB::setTm(SqlRtm *tm) {
	mTvm = tm;
}
void TabViewB::selectFont() {
	QModelIndexList mil = selectedIndexes();

	if (mil.length() <= 0)
		return;

	bool ok = false;
	QFont selectedFont = QFontDialog::getFont(&ok, this->font(), this);

	if (!ok) return;

	foreach (QModelIndex mi, mil) {
		model()->setData(mi, selectedFont, Qt::FontRole);
	}
}
void TabViewB::createActions() {

	actBgColor			= new QAction(QPixmap(16, 16), tr("bg &Color"), this);
	actBoarderColor	= new QAction(tr("Set boarder color"), this);
	actClear				= new QAction(tr("Clear"), this);
	actDelegBlk			= new QAction(tr("Toggle Delegate"), this);
	actExportXml		= new QAction(tr("ExporXML"), this);
	actFirstSep			= new QAction(this);
	actFont				= new QAction(tr("Font..."), this);
	actFitToCont		= new QAction(tr("fitToContents"), this);
	actImportXml		= new QAction(tr("ImportXML"), this);
	actSecondSep		= new QAction(this);
	actShowEquals		= new QAction(tr("Show equal cells"), this);
	actUseDefaults		= new QAction(tr("Use Defaults"), this);
	actInitBground		= new QAction(tr("Init Bground"), this);
	actSetParent		= new QAction(tr("Change parent"), this);

	if (mTvm != 0)
		connect(actShowEquals,		&QAction::toggled,	mTvm,	&SqlRtm::setDoCompare);
	connect(actClear,				&QAction::triggered, this, &TabViewB::clear);
	connect(actDelegBlk,			&QAction::triggered, this, &TabViewB::delegateSignals);
	connect(actBgColor,			&QAction::triggered, this, &TabViewB::setBgColor);
	connect(actBoarderColor,	&QAction::triggered, this, &TabViewB::setBoarderColor);
	connect(actFont,				&QAction::triggered, this, &TabViewB::selectFont);
	connect(actFitToCont,		SIGNAL(triggered()), this, SLOT(adjustTableGeometry()));
	connect(actExportXml,		SIGNAL(triggered()), this, SLOT(modelToXml()));
	connect(actImportXml,		SIGNAL(triggered()), this, SLOT(xmlToModel()));
	connect(actInitBground,		SIGNAL(triggered()), this, SLOT(initBground()));
	//	connect(actSetParent,		SIGNAL(triggered()),	this, SLOT(changeParent()));

	actShowEquals	->setShortcut( Qt::CTRL | Qt::ALT	| Qt::Key_E );
	actBoarderColor->setShortcut( Qt::CTRL | Qt::SHIFT	| Qt::Key_B );
	actBgColor		->setShortcut( Qt::CTRL | Qt::SHIFT	| Qt::Key_T );
	actFitToCont	->setShortcut( Qt::CTRL | Qt::SHIFT	| Qt::Key_E );
	actDelegBlk		->setShortcut( Qt::CTRL | Qt::ALT	| Qt::Key_D );
	actClear			->setShortcut( Qt::Key_Delete);

	/*! Set initial action states */
	actShowEquals->setCheckable(true);
	actUseDefaults->setCheckable(true);

	actSecondSep->setSeparator(true);
	actFirstSep->setSeparator(true);

	/*! Restore last actions state */
	QACTION_RESTORE;
}
void TabViewB::setupContextMenu() {
	addAction(actBgColor);
	addAction(actBoarderColor);
	addAction(actClear);
	addAction(actDelegBlk);
	addAction(actExportXml);
	addAction(actFirstSep);
	addAction(actFont);
	addAction(actFitToCont);
	addAction(actImportXml);
	addAction(actSecondSep);
	addAction(actShowEquals);
	addAction(actUseDefaults);
	addAction(actInitBground);
	addAction(actSetParent);
	setContextMenuPolicy(Qt::ActionsContextMenu);
}
bool TabViewB::modelToXml(QString filePath) {
	if (! QDir(TabViewB::DEFAULT_XML_WRITE).exists()) {
		QDir().mkdir(TabViewB::DEFAULT_XML_WRITE);
	}
	if (filePath.isEmpty()) {
		filePath = TabViewB::DEFAULT_XML_WRITE + tr("xmlout_%1_%2.xml")
					  .arg(mTvm->getUId().id)
					  .arg(mTvm->getUId().tstamp.toString(Qt::ISODate).replace(":","-"));
	}
	QFile file(filePath);

	//	file.setFileName( QFileDialog::getSaveFileName(this, tr("Open xml export dir"), "/home/") );

	if (! file.open(QIODevice::ReadWrite))
		QMessageBox::critical(0, tr("XML export error"),
									 tr("The following error ocured while try to XML export:\n%1")
									 .arg(file.errorString()), QMessageBox::Ok);

	int mROWS = mTvm->mSrc.mRowCount,
			mCOLS = mTvm->mSrc.mColCount;

	/*! Clear out file previous contents */
	file.resize(0);
	QStringList sl;

	QXmlStreamWriter xml(&file);
	xml.setAutoFormatting(true);

	/*! XML document start */
	xml.writeStartDocument();

	/*! Root element */
	xml.writeStartElement(XML.MODEL.ROOT);

	/*! Place ident tags here */
	xml.writeTextElement(XML.IDENT.ID, mTvm->getUId().id);
	xml.writeTextElement(XML.IDENT.TSTAMP, mTvm->getUId().tstamp.toString(Qt::ISODate));
	xml.writeTextElement(XML.IDENT.RSSI, QString::number(mTvm->getUId().rssi));
	xml.writeTextElement(XML.MODEL.ROWS, QString::number(mROWS));
	xml.writeTextElement(XML.MODEL.COLS, QString::number(mCOLS));

	/*! Write member fram to xml */
	xml.writeStartElement(XML.MODEL.FRAM);

	//	for (int k = 0; k < mTvm->mData.fram.count(); k++)
	for (int k = 0; k < mROWS; k++)
		//		xml.writeCharacters(mTvm->mData.framRow2Str(k)->join(ROW_SEP) + COL_SEP);
		xml.writeCharacters(mTvm->mSrc.framRow2Str(k)->join(COL_SEP) + ROW_SEP);
	//	xml.writeCharacters(mTvm->mData.fram[k].join(ROW_SEP) + COL_SEP);
	xml.writeEndElement();


	/*! Write member bgColors to xml */
	xml.writeStartElement(XML.MODEL.BGCOL);

	for (int k = 0; k < mROWS; k++) {
		sl.clear();

		for (int l = 0; l < mCOLS; l++)
			if (mTvm->mSrc.mBground.at(mTvm->modelIndexToAddr(k, l)).pen().color().isValid())
				sl.append(mTvm->mSrc.mBground.at(mTvm->modelIndexToAddr(k, l)).pen().color().name());
			else
				tr("Invalied color");

		xml.writeCharacters(sl.join(COL_SEP) + ROW_SEP);
	}
	xml.writeEndElement();

#ifdef XML_WRITE_TXT_ALIGN
	/*! Write member txtAligns to xml */
	xml.writeStartElement(XML.MODEL.ALIGN);

	for (int k = 0; k < mROWS; k++) {
		sl.clear();

		for (int l = 0; l < mCOLS; l++)
			sl.append(QString::number((int)mTvm->mData.txtAlign[k][l], 16));

		xml.writeCharacters(sl.join(ROW_SEP) + COL_SEP);
	}
	xml.writeEndElement();
#endif

	/*! End XML.MODEL.ROOT */
	xml.writeEndElement();
	/*! End of XML document */
	xml.writeEndDocument();

	file.close();

	INFO << tr("XML exported to %1").arg(filePath);
	return true;
}
bool TabViewB::xmlToModel(QString filePath) {
	/* ======================================================================== */
	if (filePath.isEmpty()) {

		QDir dir(TabViewB::DEFAULT_XML_WRITE + tr("/xmlRecall"));

		if (! dir.exists())
			dir.filePath( QFileDialog::getOpenFileName(
								  this, tr("Open xml export dir"), "/home/") );

		/*! List and sort all xml files */
		dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
		dir.setSorting(QDir::Name | QDir::Reversed);

		/*!
		 * When a default import action occures, select the last modified xml file but check
		 * if the corresponding timestamp is uniq in applications mTbls vector. If not, select
		 * the next earlier xml file.AbortError.
		 */
		QFileInfoList fileList = dir.entryInfoList();

		if (fileList.isEmpty()) {
			INFO << tr("No xml files found in %1!").arg(dir.absolutePath());
			return false;
		}

//		filePath = fileList.last().absoluteFilePath();
		QString str = QFileDialog::getOpenFileName(this, tr("Open xml model file..."),
																 tr("/home/mainster/.config/delBassoDEV/xml/xmlRecall/"),
																 tr("*.xml"));
		filePath = str;

	}
	/* ======================================================================== */
	INFO << tr("XML import from %1").arg(filePath);
	QFile file(filePath);

	/*! Validate the file path */
	if (! file.exists())
		return false;

	/*! Handle import errors */
	if (! file.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(0, tr("XML import error"),
									 tr("The following error ocured while try to XML import:\n%1")
									 .arg(file.errorString()), QMessageBox::Ok);
		return false;
	}

	uID_t _uID;
	QStringList sl;

	int xmlRows = 0,
			xmlCols = 0;

	QXmlStreamReader *xml = new QXmlStreamReader();
	xml->setDevice(&file);

	while (!xml->atEnd() && !xml->hasError()) {
		/* Read next element.*/
		QXmlStreamReader::TokenType token = xml->readNext();

		/* If token is just StartDocument, we'll go to next.*/
		if (token == QXmlStreamReader::StartDocument)
			continue;

		/* If token is not a StartElement, loop read next.*/
		if (token != QXmlStreamReader::StartElement)
			continue;

		/*!
		 * Read identifiers
		 */
		if (xml->name() == XML.MODEL.ROOT) {
			token = xml->readNext();
			QString sr = xml->text().toString();
			//			INFO << tr("XML read in: Root element:") << sr;
			continue;
		}
		if (xml->name() == XML.IDENT.ID) {
			token = xml->readNext();
			_uID.id = xml->text().toString();
			continue;
		}
		if (xml->name() == XML.IDENT.TSTAMP) {
			token = xml->readNext();
			QDateTime dt = QDateTime::fromString(xml->text().toString(), Qt::ISODate);
			_uID.tstamp = dt;
			continue;
		}
		if (xml->name() == XML.IDENT.RSSI) {
			token = xml->readNext();
			quint8 rs = xml->text().toString().toInt();
			_uID.rssi = rs;
			continue;
		}

		/*!
		 * Read model dimension
		 */
		if (xml->name() == XML.MODEL.ROWS) {
			token = xml->readNext();
			xmlRows = xml->text().toString().toInt();
			continue;
		}
		if (xml->name() == XML.MODEL.COLS) {
			token = xml->readNext();
			xmlCols = xml->text().toString().toInt();
			continue;
		}

		/*!
		 * Set model dimension
		 */
		if (xmlRows * xmlCols) {
			getTm()->initModel(_uID, xmlRows, xmlCols);
			xmlRows = xmlCols = 0;
		}

		/*!
		 * Read fram from xml
		 */
		if (xml->name() == XML.MODEL.FRAM) {
			token = xml->readNext();
			sl = xml->text().toString().replace(ROW_SEP, COL_SEP).split(COL_SEP);
			sl.removeLast();
			bool ok;

			for (int k = 0; k<sl.length(); k++) {
				QString s = sl.at(k);
				mTvm->mSrc.mFram[k] = sl.at(k).toInt(&ok, 16);
			}
			while (token != QXmlStreamReader::EndElement)
				token = xml->readNext();

			token = xml->readNext();
			continue;
		}

		/*!
			 * Read bgColors from xml
			 */
		if (xml->name() == XML.MODEL.BGCOL) {
			token = xml->readNext();
			sl = xml->text().toString().replace(ROW_SEP, COL_SEP).split(COL_SEP);
			sl.removeLast();

			for (int k = 0; k<sl.length(); k++) {
				QString s = sl.at(k);
				QColor colorEl;
				colorEl.setNamedColor(s);

				if (colorEl.isValid())
					mTvm->mSrc.mBground[k].setPen(QPen(colorEl));
				else
					INFO << tr("Invalied color parsed?!!");
			}

			while (token != QXmlStreamReader::EndElement)
				token = xml->readNext();

			token = xml->readNext();
			continue;
		}

#ifdef XML_WRITE_TXT_ALIGN
		/*!
			 * Read txtAligns from xml
			 */
		if (xml->name() == XML.MODEL.ALIGN) {
			token = xml->readNext();
			sl = xml->text().toString().split(COL_SEP);
			sl.removeLast();

			for (int k = 0; k < sl.count(); k++) {
				QStringList slr = QString(sl[k]).split(ROW_SEP);

				if (mTvm->mData.txtAlign.count() <= k) {
					/*! If txtAligns end reached, append a new row */
					QList<Qt::Alignment> _al =
							Globals::seqAlign(Qt::AlignLeft, mCOLS);
					mTvm->mData.txtAlign.append( _al );
				}

				for (int l = 0; l < slr.count(); l++) {
					mTvm->mData.txtAlign[k][l] =
							QFlags<Qt::AlignmentFlag>( QString(slr[l]).toInt(&ok, 16) );
				}
			}

			while (token != QXmlStreamReader::EndElement)
				token = xml->readNext();

			token = xml->readNext();
			continue;
		}
#endif
	}

	if (xml->hasError())
		CRIT << tr("xml->hasError(): ") << xml->errorString();
	else {
		if (! xml->atEnd())
			INFO << tr("(! xml->atEnd())");
	}

	file.close();

	mTvm->setUId(_uID);
	mTvm->emit_modelHasChanged();
	Ctrls *ctrl = Ctrls::getInstance();

	if (ctrl)	ctrl->populateInfosProxy();

	return true;
}
void TabViewB::delegateSignals(const bool b) {
	Q_UNUSED(b);
	viewport()->update();
}
void TabViewB::setCustomDelegate(const bool b) {
	if (b)
		setItemDelegate(delegate);
	else
		setItemDelegate(defaultDelegate);
}
/**********************************************************************//**
 *		Event handlers
 **************************************************************************/
void TabViewB::keyPressEvent(QKeyEvent *e) {
	/*!
	 * Close event handler if event is not of type QEvent::keyPress.
	 */
	if (e->type() != QEvent::KeyPress)
		return;

	/*!
	 * Close parent Table:: instance if key escape was pressed.
	 */
	if (e->key() == Qt::Key_Escape) {
		parentWidget()->close();
		return;
	}

	if( e->modifiers() == ( Qt::ControlModifier |
									Qt::ShiftModifier ))  {
		/*!
		 * Ctrl + Shift modified shortcuts...
		 */
		if (e->key() == Qt::Key_R) {
			//			viewport()->update();

			setSizeAdjustPolicy(QTableView::AdjustIgnored);
			viewport()->update();

			getTm()->mShowAddr = true;
			getTm()->emit_modelHasChanged();
			//s			QTimer::singleShot(3000, this, SLOT(onTimeout()));
			return;
		}

		/*! Define further Ctrl + Shift modified shortcuts */

	}


}
void TabViewB::onTimeout() {
	getTm()->mShowAddr = false;
	setSizeAdjustPolicy(QTableView::AdjustToContents);
	getTm()->emit_modelHasChanged();
}
/*!
 * \brief TabViewB::paintEvent is invoked when a screen update is necessary.
 * This paint event handler is used to customize the "current selection".
 * The non-volatile boarder settings are handeld by the Delegate:: class.
 * \param e
 */
void TabViewB::paintEventM(QPaintEvent *e) {
	/*!
	 * Call base classes paint event first so we can draw over it.
	 */
	QTableView::paintEvent( e );
	viewport()->update();

	INFO << e->region().boundingRect();
	if (! selectedIndexes().isEmpty()) {
		/*! If this paintEvent is selection based: */
		QModelIndexList mil = selectedIndexes();

		QRect rect = visualRect(mil.at(0)) |
						 visualRect(mil.at(mil.size() - 1));

		QPen pen(Qt::red, 1);
		QPainter painter(viewport());
		painter.setPen(pen);
		painter.drawRect(rect);
	}
}
void TabViewB::keyReleaseEvent(QKeyEvent *e) {

	if (e->type() != QEvent::KeyRelease)
		QTableView::keyReleaseEvent(e);

	if (e->key() == Qt::Key_R) {
		setSizeAdjustPolicy(QTableView::AdjustToContents);
		viewport()->update();

		getTm()->mShowAddr = false;
		getTm()->emit_modelHasChanged();
		//				QTimer::singleShot(3000, this, SLOT(onTimeout()));
		return;
	}
}
