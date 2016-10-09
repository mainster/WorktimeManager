#-------------------------------------------------
#
# Project created by QtCreator 2015-10-28T13:53:49
#
#-------------------------------------------------

QT       += core gui sql widgets
QT			+= network
QT			+= script

qtHaveModule(printsupport): QT += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WorktimeManager
#QMAKE_CXX = ccache g++
TEMPLATE = app
CONFIG+=c++11
INCLUDEPATH += mdbQtStd

DEFINES += USE_QPOINTER

DISTFILES += \
	 NOTES \
    ../../../../.config/delBassoDEV/WorktimeManager.ini \


INCLUDEPATH += \
		delegates \
		headers \
		mdbQtStd \
		models \
		views \
		ui \
		textedit \


RESOURCES += \
    WorktimeManager.qrc

OTHER_FILES +=

# install
#target.path = $$[QT_INSTALL_EXAMPLES]/sql/sqlbrowser
#INSTALLS += target


HEADERS += \
	 browser.h \
	 connectionwidget.h \
	 dbconndlg.h \
	 dbcontroller.h \
	 delegates/sqlreldel.h \
	 delegates/tabledelegate.h \
	 filterform.h \
	 headers/locals.h \
	 inpfrm.h \
	 mainwindow.h \
	 mdbQtStd/debug.h \
	 mdbQtStd/globals.h \
	 mdbQtStd/mdeventfilters.h \
	 mdbQtStd/mdstatebar.h \
	 mdbQtStd/types.h \
	 mdcombobox.h \
	 mdmenu.h \
	 mdnotes.h \
	 models/editablesqlmdl.h \
	 mpushbutton.h \
	 sectionmask.h \
	 textedit/textedit.h \
	 views/mdtable.h \
    views/mdtabview.h \
    basedataform.h \
    delegates/nulldelegate.h \
    mdsplashscreen.h \
    mdplaintextedit.h \
    runtimetable.h \
    mdtableinfo.h \
    headeralias.h \
    datetimerangemask.h \
    models/sqlrtm.h \
    models/sfiltmdl.h


SOURCES += \
	 browser.cpp \
	 connectionwidget.cpp \
	 dbconndlg.cpp \
	 dbcontroller.cpp \
	 delegates/sqlreldel.cpp \
	 delegates/tabledelegate.cpp \
	 filterform.cpp \
	 headers/locals.cpp \
	 inpfrm.cpp \
	 main.cpp \
	 mainwindow.cpp \
	 mdbQtStd/debug.cpp \
	 mdbQtStd/globals.cpp \
	 mdbQtStd/mdeventfilters.cpp \
	 mdbQtStd/mdstatebar.cpp \
	 mdbQtStd/types.cpp \
	 mdcombobox.cpp \
	 mdmenu.cpp \
	 mdnotes.cpp \
	 models/editablesqlmdl.cpp \
	 mpushbutton.cpp \
	 sectionmask.cpp \
	 textedit/textedit.cpp \
	 views/mdtable.cpp \
    views/mdtabview.cpp \
    basedataform.cpp \
    delegates/nulldelegate.cpp \
    mdsplashscreen.cpp \
    mdplaintextedit.cpp \
    runtimetable.cpp \
    mdtableinfo.cpp \
    headeralias.cpp \
    datetimerangemask.cpp \
    models/sqlrtm.cpp \
    models/sfiltmdl.cpp


FORMS += \
	 uis/dbconndlg.ui \
	 uis/inpfrm.ui \
	 uis/mainwindow.ui \
	 uis/filterform.ui \
	 uis/mdnotes.ui \
    uis/inpfrmTRANS.ui \
    uis/mdtable.ui


