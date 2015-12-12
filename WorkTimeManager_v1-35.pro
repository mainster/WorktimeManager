#-------------------------------------------------
#
# Project created by QtCreator 2015-10-28T13:53:49
#
#-------------------------------------------------

QT       += core gui
QT       += sql widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WorkTimeManager_v1-35
TEMPLATE = app

PLATFORM = linux


SOURCES += main.cpp\
        mainwindow.cpp \
    connectionwidget.cpp \
    browser.cpp \
    inpfrm.cpp \
    dbconndlg.cpp \
    tabledelegate.cpp \
    tablewindow.cpp \
    globals.cpp \
    tabview.cpp \
    form.cpp \
    QSortFilterSqlQueryModel.cpp

HEADERS  += mainwindow.h \
    connectionwidget.h \
    browser.h \
    globals.h \
    inpfrm.h \
    initdb.h \
    dbconndlg.h \
    tabledelegate.h \
    tablewindow.h \
    tabview.h \
    form.h \
    QSortFilterSqlQueryModel.h

FORMS    += mainwindow.ui \
    dbconndlg.ui \
    tablewindow.ui \
    form.ui \
    browser.ui \
    inpfrm.ui \
    tabview.ui


RESOURCES += \
    WorkTimeManager_v1-35.qrc

OTHER_FILES +=

# install
#target.path = $$[QT_INSTALL_EXAMPLES]/sql/sqlbrowser
#INSTALLS += target


wince*: {
    DEPLOYMENT_PLUGIN += qsqlite
}
