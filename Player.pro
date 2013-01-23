#-------------------------------------------------
#
# Project created by QtCreator 2013-01-14T11:54:56
#
#-------------------------------------------------

QT       += core gui phonon xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Failure
TEMPLATE = app

LIBS += -ltag -llastfm

SOURCES += main.cpp \
    player.cpp \
    stringmanager.cpp \
    track.cpp \
    database.cpp \
    mime/freedesktopmime.cpp \
    progressdialog.cpp

HEADERS  += \
    player.h \
    stringmanager.h \
    track.h \
    database.h \
    mime/freedesktopmime.h \
    progressdialog.h

FORMS    += \
    player.ui \
    progressdialog.ui

RESOURCES += \
    mime/freedesktopmime.qrc \
    res.qrc

OTHER_FILES += \
    todo.txt \
    TODO \
    .gitignore
