#-------------------------------------------------
#
# Project created by QtCreator 2015-10-28T13:53:49
#
#-------------------------------------------------

QT       += core gui sql widgets
QT			+= network
QT			+= script

#unix:LIBS += -lpq
#win32:LIBS += libpqdll.lib

qtHaveModule(printsupport): QT += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WorktimeManager
#QMAKE_CXX = ccache g++
TEMPLATE = app
CONFIG+=c++11
INCLUDEPATH += mdbQtStd

DEFINES +=	USE_QPOINTER \
				NO_SPLASH_SCREEN \

DISTFILES += \
	 NOTES \
	../../../../.config/delBassoDEV/WorktimeManager.ini \
	sqlite/olddb/delbassoSQL_2016-09-25_184903.db \
	sqlite/delbasso_SQLITE.db \
	sqlite/delbassoSQL.db \
	images/textedit_res/logo32.png \
	images/bigPencilNote.png \
	images/down_arrowb.png \
	images/notesBlackPencil.png \
	images/up_arrowb.png \
	textedit/images/logo32.png \
	images/greenCheck.xcf \
	splashScreen.xcf \
	WorktimeManager_customQuerys \
	apts_qt \
	WorktimeManager.ini \
	textedit/textedit.qdoc \
	sqlite/delBassoInitialDb_sheme_template.sql \
	sqlite/delBassoInitialDb_sheme_template09-May-2017_091353.sql


INCLUDEPATH += \
		delegates \
		headers \
		mdbQtStd \
		models \
		views \
		ui \
		textedit \
		filter \
		misc \
		inpfrm \


RESOURCES += \
	WorktimeManager.qrc

OTHER_FILES +=

# install
#target.path = $$[QT_INSTALL_EXAMPLES]/sql/sqlbrowser
#INSTALLS += target


HEADERS += \
	basedataform.h \
	browser.h \
	connectionwidget.h \
	datetimerangemask.h \
	dbconndlg.h \
	dbcontroller.h \
	delegates/nulldelegate.h \
	delegates/sqlreldel.h \
	delegates/tabledelegate.h \
	filter/basicsortfilt.h \
	filter/filterwidget.h \
	headers/headeralias.h \
	headers/locals.h \
	inpfrm/inpfrm4.h \
	inpfrm/inpfrm.h \
	mainwindow.h \
	mdbQtStd/debug.h \
	mdbQtStd/globals.h \
	mdbQtStd/mdeventfilters.h \
	mdbQtStd/mdstatebar.h \
	mdbQtStd/types.h \
	mdmenu.h \
	mdnotes.h \
	mdplaintextedit.h \
	mdtableinfo.h \
	misc/mdsplashscreen.h \
	misc/sectionmask.h \
	models/editablesqlmdl.h \
	models/mdstringlistmodel.h \
	models/sfiltmdl.h \
	models/sqlrtm.h \
	models/tbl2listmdl.h \
	mpushbutton.h \
	runtimetable.h \
	textedit/textedit.h \
	views/mdcombobox.h \
	views/mdecombo.h \
	views/mdtable.h \
	views/mdtabview.h \
    mdcompleter.h


SOURCES += \
	basedataform.cpp \
	browser.cpp \
	connectionwidget.cpp \
	datetimerangemask.cpp \
	dbconndlg.cpp \
	dbcontroller.cpp \
	delegates/nulldelegate.cpp \
	delegates/sqlreldel.cpp \
	delegates/tabledelegate.cpp \
	filter/basicsortfilt.cpp \
	filter/filterwidget.cpp \
	headers/headeralias.cpp \
	headers/locals.cpp \
	inpfrm/inpfrm4.cpp \
	inpfrm/inpfrm.cpp \
	main.cpp \
	mainwindow.cpp \
	mdbQtStd/debug.cpp \
	mdbQtStd/globals.cpp \
	mdbQtStd/mdeventfilters.cpp \
	mdbQtStd/mdstatebar.cpp \
	mdbQtStd/types.cpp \
	mdmenu.cpp \
	mdnotes.cpp \
	mdplaintextedit.cpp \
	mdtableinfo.cpp \
	misc/mdsplashscreen.cpp \
	misc/sectionmask.cpp \
	models/editablesqlmdl.cpp \
	models/mdstringlistmodel.cpp \
	models/sfiltmdl.cpp \
	models/sqlrtm.cpp \
	models/tbl2listmdl.cpp \
	mpushbutton.cpp \
	runtimetable.cpp \
	textedit/textedit.cpp \
	views/mdcombobox.cpp \
	views/mdecombo.cpp \
	views/mdtable.cpp \
	views/mdtabview.cpp \
    mdcompleter.cpp



FORMS += \
	 uis/dbconndlg.ui \
	 uis/inpfrm.ui \
	 uis/mainwindow.ui \
	 uis/mdnotes.ui \
	uis/inpfrmTRANS.ui \
	uis/mdtable.ui \
	uis/form.ui \
	uis/inpfrm2.ui \


