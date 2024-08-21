QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    codetextedit.cpp \
    databaseconnection.cpp \
    datasource.cpp \
    entrylist.cpp \
    linewidget.cpp \
    main.cpp \
    preferences.cpp \
    project.cpp \
    projectitem.cpp \
    queryresult.cpp \
    ringz.cpp \
    rz.cpp \
    syntaxhighlight.cpp \
    table.cpp \
    tablecolumn.cpp \
    tablekey.cpp \
    texteditor.cpp

HEADERS += \
    codetextedit.h \
    databaseconnection.h \
    datasource.h \
    entrylist.h \
    linewidget.h \
    preferences.h \
    project.h \
    projectitem.h \
    queryresult.h \
    ringz.h \
    rz.h \
    syntaxhighlight.h \
    table.h \
    tablecolumn.h \
    tablekey.h \
    texteditor.h

FORMS += \
    datasource.ui \
    entrylist.ui \
    preferences.ui \
    project.ui \
    ringz.ui \
    texteditor.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ringz.qrc
