#include "mdeventfilters.h"

/* ======================================================================== */
/*                      GlobalEventFilter::eventFilter                      */
/* ======================================================================== */
bool GlobalEventFilter::eventFilter(QObject *obj, QEvent *event) {
	/*!
	 * Event handler for global focus events.
	 */
	if ((event->type() == QEvent::FocusIn) ||
		 (event->type() == QEvent::FocusOut)) {

		if (mPrintDebug) {
			INFO << tr("Class:") << obj->metaObject()->className()
				  << tr("property ""hasFocus"":") << obj->property("hasFocus").toBool();
		}

		QWidget *wdg = qobject_cast<QWidget *>(obj);

		if (! wdg)
			return QObject::eventFilter(obj, event);

		/*!
		 * Validate obj if custom property "hasFocus" exists and set/clear the value
		 */
		if (wdg->property("hasFocus").isValid()) {
			wdg->setProperty("hasFocus",(event->type() == QEvent::FocusIn) ? true : false);
			wdg->style()->unpolish(wdg);
			wdg->style()->polish(wdg);
			wdg->update();
		}

		event->ignore();
		return QObject::eventFilter(obj, event);
	}

	if (event->type() == QEvent::KeyPress) {
		if (static_cast<QKeyEvent *>(event)->key() == Qt::Key_At)
			emit control_one_pressed();
	}

	/*!
	 * This wheel event has been moved from MdTable::wheelEvent(...) to this global event
	 * filter because wheel events are sent to the MdTables base classes scroll bars, if
	 * they exist. This means, the font could only be changed if the scroll bar reaches
	 * the upper or lower bound. By caching the wheel event globally by this event handler,
	 * the problem could be fixed.
	 */
	if (event->type() == QEvent::Wheel) {
		if (static_cast<QWheelEvent *>(event)) {
			/** query current keyboard modifiers
			 */

			/** Ctrl modifier + mouse wheel changes font size
			 * Do some range check of min/max pointsize
			 */
			if (qApp->queryKeyboardModifiers() == Qt::ControlModifier) {
				Browser::mTabs_t browserTables = Browser::instance()->mTabs;

				MdTable *mdTable = qobject_cast<MdTable *>(qApp->widgetAt(QCursor::pos())
																		 ->parent()->parent()->parent());
				if (! mdTable)
					return QObject::eventFilter(obj, event);

				if (browserTables.tvIds().join(',').contains(mdTable->tv()->objectName())) {
					/**< Delta size */
					int dsize = static_cast<QWheelEvent *>(event)->delta() /
									abs(static_cast<QWheelEvent *>(event)->delta());
					/** Add current step.
					 * event->delta() can be negative or positive.
					 */
					mdTable->mouseWheelCnt += dsize;

					if ( ((dsize < 0) && (mdTable->tv()->fontInfo().pointSize() > 6)) ||
						  ((dsize > 0) && (mdTable->tv()->fontInfo().pointSize() < 20)) ) {
						QFont cfont = QFont( mdTable->tv()->font() );
						cfont.setPointSize( cfont.pointSize() + dsize );
						mdTable->tv()->setFont( cfont );
						mdTable->tv()->storeFont( mdTable->tv()->font() );
					}
					mdTable->tv()->resizeColumnsToContents();
					mdTable->tv()->resizeRowsToContents();
				}
				return QObject::eventFilter(obj, event);
			}
		}
	}

	/*!
	 * Call superclass methode and pass the return value.
	 */
	return QObject::eventFilter(obj, event);
}

/* ======================================================================== */
/*                       SqlEventFilter::eventFilter                        */
/* ======================================================================== */
bool SqlEventFilter::eventFilter(QObject *obj, QEvent *event) {
	/*!
	 * standard event processing
	 */
	if ((event->type() != QEvent::MouseButtonPress) ||
		 (event->type() != QEvent::KeyPress))
		return QObject::eventFilter(obj, event);

	/*!
	 * Event type key pressed
	 */
	if(event->type() == QEvent::KeyPress) {
		switch (dynamic_cast<QKeyEvent *>(event)->key()) {
			case Qt::Key_Escape: {
				INFO << tr("reset focus to first tab widget");
				//				reset()
			}; break;
			case Qt::Key_Tab: {
				INFO << tr("Tab pressed");
			}; break;
			default: break;
		}
	}

	/*!
	 * Try to get InpFrm:: class instance pointer from QObject parent
	 */
	InpFrm *inpFrm = qobject_cast<InpFrm*>(obj->parent());

	if (! inpFrm)
		bReturn("qobject_cast<InpFrm *>(obj->parent())) fails");

	/*!
	 * Check event type wether to be a mouse button pressed
	 */
	if (dynamic_cast<QMouseEvent *>(event)->button() == Qt::LeftButton) {
		if (inpFrm->getChangeFocusFlag() == Qt::FocusChange_isRunning) {
			if (! inpFrm->mTabOrder.addWidgetToNext( qobject_cast<QWidget *>(obj) )) {
				inpFrm->setChangeFocusFlag( Qt::FocusChange_done );
				QString msg(tr("Input form tab order successfully changed!"));
				emit inpFrm->stateMessage(msg, 2000);
				INFO << msg;
			}
		}
	}
	return false;
}
