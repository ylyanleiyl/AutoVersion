#-------------------------------------------------
#
# Project created by QtCreator 2018-08-20T18:46:00
#
#-------------------------------------------------

QT       += core

#pro for msvc utf8 code
msvc:QMAKE_CXXFLAGS += -execution-charset:utf-8
msvc:QMAKE_CXXFLAGS += -source-charset:utf-8
msvc:QMAKE_CXXFLAGS_WARN_ON += -wd4819
#cpp #pragma execution_character_set("utf-8")

TARGET = AutoVersion

TEMPLATE = app
DESTDIR = ./

CONFIG  += console
CONFIG  -= app_bundle
CONFIG  += stl warn_on qt

SOURCES += \
        useless.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

VERSION = 2018.11.21

##************************AutoVersion***************************#
win32-msvc* {
    #message(win32-msvc)
    copytobin.commands = copy $${TARGET}.exe $$(QTDIR)\\bin\\$${TARGET}.exe
    QMAKE_EXTRA_TARGETS += copytobin
    PRE_TARGETDEPS += copytobin
    version.commands = call "$$(QTDIR)\\bin\\$${TARGET}.exe" $$PWD/../version.h -pnWinServer
    custom.commands = call "$$(QTDIR)\\bin\\$${TARGET}.exe" $$PWD/../custom.pri \"-rDEFINES += NO_QMESSAGEBOX\" \
                                                                                 \"-rDEFINES += Debug_For_No_Public\" \
                                                                                 \"-rDEFINES += Compile_WinClient\"
}
else {
    #message(mingw linux)
    version.commands = $$DESTDIR$$TARGET $$PWD/../version.h -pnWinServer
    custom.commands = $$DESTDIR$$TARGET $$PWD/../custom.pri \"-rDEFINES += NO_QMESSAGEBOX\" \
                                                     \"-rDEFINES += Debug_For_No_Public\" \
                                                     \"-aDEFINES += Compile_WindServer\"
}

#version.depends = FORCE
#rcbuild.depends = version
QMAKE_EXTRA_TARGETS += version custom
PRE_TARGETDEPS += version custom

#win32:rcbuild.commands += $$DESTDIR$$TARGET $$PWD/../res.rc
#win32:QMAKE_EXTRA_TARGETS += rcbuild
#win32:PRE_TARGETDEPS += rcbuild

##1add sub pro->include ../version.h
##2add sub pro->win32:RC_FILE = pos.rc
##************************AutoVersion***************************#


