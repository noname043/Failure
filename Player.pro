#-------------------------------------------------
#
# Project created by QtCreator 2013-01-14T11:54:56
#
#-------------------------------------------------

QT       += core gui phonon xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Failure
TEMPLATE = app

LIBS += -ltag -llastfm

SOURCES += src/main.cpp \
    src/player.cpp \
    src/stringmanager.cpp \
    src/track.cpp \
    src/database.cpp \
    mime/freedesktopmime.cpp \
    src/progressdialog.cpp \
    src/lastfmauth.cpp \
    src/lastfmauthdialog.cpp \
    src/settings.cpp

HEADERS  += \
    src/player.h \
    src/stringmanager.h \
    src/track.h \
    src/database.h \
    mime/freedesktopmime.h \
    src/progressdialog.h \
    src/lastfmauth.h \
    src/lastfmauthdialog.h \
    src/settings.h \
    src/global.h

FORMS    += \
    ui/player.ui \
    ui/progressdialog.ui \
    ui/lastfmauthdialog.ui

RESOURCES += \
    mime/freedesktopmime.qrc \
    res/res.qrc

OTHER_FILES += \
    todo.txt \
    TODO \
    .gitignore
