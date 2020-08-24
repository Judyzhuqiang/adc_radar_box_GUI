
#-------------------------------------------------
#
# Project created by QtCreator 2020-02-18T11:54:06
#
#-------------------------------------------------

QT       += core gui serialport charts network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#DEFINES += QT_NO_WARNING_OUTPUT
#DEFINES += QT_NO_DEBUG_OUTPUT

unix:CONFIG += c++11 console
unix:QMAKE_LFLAGS += -no-pie

QMAKE_CFLAGS_ISYSTEM = -I
TARGET = adc_radar_box_GUI
TEMPLATE = app
QMAKE_LFLAGS += -no-pie
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    radarwidget.cpp \
    radar.cpp \
    radarpaintwidget.cpp \
    can_driver.cpp \
    candriver.cpp

HEADERS += \
        mainwindow.h \
   radarwidget.h \
    radar.h \
   radarpaintwidget.h \
    can_driver.h \
    candriver.h \
    detectiondefines.h \
    include/controlcan.h

FORMS += \
        mainwindow.ui


unix: INCLUDEPATH += /usr/include


RESOURCES += \
    resource.qrc

DISTFILES += \
    lib/libcontrolcan.so

unix:!macx: LIBS += -L$$PWD/lib/ -lcontrolcan

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include



# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target
