#-------------------------------------------------
#
# Project created by QtCreator 2015-10-28T13:53:49
#
#-------------------------------------------------

QT       += core gui
QT       += sql widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WorktimeManager_v1-35
TEMPLATE = app

PLATFORM = linux
#APP_NAME="WorktimeManager_v"
#APP_VERSION="1.35"


SOURCES += main.cpp\
        mainwindow.cpp \
    connectionwidget.cpp \
    browser.cpp \
    inpfrm.cpp \
    dbconndlg.cpp \
    tabledelegate.cpp \
    globals.cpp \
    tabview.cpp \
    form.cpp \
    mdstatebar.cpp \
    sortfiltproxymdl.cpp \
    filtform.cpp

HEADERS  += mainwindow.h \
    connectionwidget.h \
    browser.h \
    globals.h \
    inpfrm.h \
    initdb.h \
    dbconndlg.h \
    tabledelegate.h \
    tabview.h \
    form.h \
    mdstatebar.h \
    sortfiltproxymdl.h \
    filtform.h

FORMS    += mainwindow.ui \
    dbconndlg.ui \
    tablewindow.ui \
    form.ui \
    browser.ui \
    inpfrm.ui \
    tabview.ui \
    sortwindow.ui \
    filtform.ui


RESOURCES += \
    WorktimeManager_v1-35.qrc

OTHER_FILES +=

# install
#target.path = $$[QT_INSTALL_EXAMPLES]/sql/sqlbrowser
#INSTALLS += target


wince*: {
    DEPLOYMENT_PLUGIN += qsqlite
}
