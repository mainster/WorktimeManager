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
	 basedataform.h \
	 browser.h \
	 connectionwidget.h \
	 datetimerangemask.h \
	 dbconndlg.h \
	 dbcontroller.h \
	 delegates/nulldelegate.h \
	 delegates/sqlreldel.h \
	 delegates/tabledelegate.h \
	 filterwidget.h \
	 headeralias.h \
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
	 mdplaintextedit.h \
	 mdsplashscreen.h \
	 mdtableinfo.h \
	 models/editablesqlmdl.h \
	 models/sfiltmdl.h \
	 models/sqlrtm.h \
	 mpushbutton.h \
	 runtimetable.h \
	 sectionmask.h \
	 textedit/textedit.h \
	 views/mdtable.h \
	 views/mdtabview.h \
    inpfrm4.h


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
	 filterwidget.cpp \
	 headeralias.cpp \
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
	 mdplaintextedit.cpp \
	 mdsplashscreen.cpp \
	 mdtableinfo.cpp \
	 models/editablesqlmdl.cpp \
	 models/sfiltmdl.cpp \
	 models/sqlrtm.cpp \
	 mpushbutton.cpp \
	 runtimetable.cpp \
	 sectionmask.cpp \
	 textedit/textedit.cpp \
	 views/mdtable.cpp \
	 views/mdtabview.cpp \
    inpfrm4.cpp


FORMS += \
	 uis/dbconndlg.ui \
	 uis/inpfrm.ui \
	 uis/mainwindow.ui \
	 uis/mdnotes.ui \
    uis/inpfrmTRANS.ui \
    uis/mdtable.ui \
    uis/form.ui


