#-------------------------------------------------
#
# Project created by QtCreator 2020-05-16T00:47:33
#
#-------------------------------------------------

QT       += core gui
QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = XPlayerWidget
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        videowidget.cpp \
        xaudioplay.cpp \
        xffmpeg.cpp \
        xplayer.cpp \
        xslider.cpp \
        xvideothread.cpp

HEADERS += \
        videowidget.h \
        xaudioplay.h \
        xffmpeg.h \
        xplayer.h \
        xslider.h \
        xvideothread.h

FORMS += \
        xplayer.ui


#添加ffmpeg库
INCLUDEPATH += $$PWD/../../include
LIBS += -L$$PWD/../../lib/win32

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc
