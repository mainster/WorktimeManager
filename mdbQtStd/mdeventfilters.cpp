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

		if (! qobject_cast<QWidget *>(obj))
			return QObject::eventFilter(obj, event);

		QWidget *wdg = qobject_cast<QWidget *>(obj);

		/*!
		 * Validate obj if custom property "hasFocus" exists and set/clear the value
		 */
		if (wdg->property("hasFocus").isValid()) {
			wdg->setProperty("hasFocus",(event->type() == QEvent::FocusIn) ? true : false);
			wdg->style()->unpolish(wdg);
			wdg->style()->polish(wdg);
			wdg->update();
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
