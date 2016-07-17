#-------------------------------------------------
#
# Project created by QtCreator 2015-10-28T13:53:49
#
#-------------------------------------------------

QT       += core gui
QT       += sql widgets

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = WorktimeManager
QMAKE_CXX = ccache g++
TEMPLATE = app
CONFIG+=c++11
INCLUDEPATH += mdbQtStd


SOURCES +=	\
	browser.cpp \
	connectionwidget.cpp \
	dbconndlg.cpp \
	form.cpp \
	inpfrm.cpp \
	locals.cpp \
	main.cpp \
	mainwindow.cpp \
	mdbQtStd/debug.cpp \
	mdbQtStd/globals.cpp \
	mysortfilterproxymodel.cpp \
	sortwindow.cpp \
	tabledelegate.cpp \
	tablewindow.cpp \
	tabview.cpp \
	mdbQtStd/mdstatebar.cpp

HEADERS  += \
	browser.h \
	connectionwidget.h \
	dbconndlg.h \
	form.h \
	initdb.h \
	inpfrm.h \
	locals.h \
	mainwindow.h \
	mdbQtStd/debug.h \
	mdbQtStd/globals.h \
	mysortfilterproxymodel.h \
	sortwindow.h \
	tabledelegate.h \
	tablewindow.h \
	tabview.h \
	mdbQtStd/mdstatebar.h

FORMS    += \
	browser.ui \
	dbconndlg.ui \
	form.ui \
	inpfrm.ui \
	mainwindow.ui \
	sortwindow.ui \
	tablewindow.ui \
	tabview.ui


RESOURCES += \
    WorktimeManager.qrc

OTHER_FILES +=

# install
#target.path = $$[QT_INSTALL_EXAMPLES]/sql/sqlbrowser
#INSTALLS += target


wince*: {
    DEPLOYMENT_PLUGIN += qsqlite
}

DISTFILES += \
    NOTES
