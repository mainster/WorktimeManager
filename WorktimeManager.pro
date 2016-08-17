#-------------------------------------------------
#
# Project created by QtCreator 2015-10-28T13:53:49
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WorktimeManager
QMAKE_CXX = ccache g++
TEMPLATE = app
CONFIG+=c++11
INCLUDEPATH += mdbQtStd

DISTFILES += \
	 NOTES

INCLUDEPATH += \
		delegates \
		headers \
		mdbQtStd \
		models ui


RESOURCES += \
    WorktimeManager.qrc

OTHER_FILES +=

# install
#target.path = $$[QT_INSTALL_EXAMPLES]/sql/sqlbrowser
#INSTALLS += target


wince*: {
    DEPLOYMENT_PLUGIN += qsqlite
}

HEADERS += \
    delegates/tabledelegate.h \
    headers/locals.h \
    mdbQtStd/debug.h \
    mdbQtStd/globals.h \
    mdbQtStd/mdstatebar.h \
    mdbQtStd/types.h \
    models/editablesqlmdl.h \
    models/mysortfilterproxymodel.h \
    browser.h \
    connectionwidget.h \
    dbconndlg.h \
    dbcontroller.h \
    initdb.h \
    inpfrm.h \
    mainwindow.h \
    sortwindow.h \
    tabview.h \
    models/models.h \
	 delegates/sqlreldel.h \
    mdbQtStd/mdeventfilters.h \
	 mdcombobox.h

SOURCES += \
    delegates/tabledelegate.cpp \
    headers/locals.cpp \
    mdbQtStd/debug.cpp \
    mdbQtStd/globals.cpp \
    mdbQtStd/mdstatebar.cpp \
    mdbQtStd/types.cpp \
    models/editablesqlmdl.cpp \
    models/mysortfilterproxymodel.cpp \
    browser.cpp \
    connectionwidget.cpp \
    dbconndlg.cpp \
    dbcontroller.cpp \
    inpfrm.cpp \
    main.cpp \
    mainwindow.cpp \
    sortwindow.cpp \
    tabview.cpp \
    models/models.cpp \
	 delegates/sqlreldel.cpp \
    mdbQtStd/mdeventfilters.cpp \
	 mdcombobox.cpp

FORMS += \
	 uis/dbconndlg.ui \
	 uis/inpfrm.ui \
	 uis/mainwindow.ui \
	 uis/sortwindow.ui \
	 uis/tabview.ui \
    uis/_form.ui


