#------------------------------------------------
QT       += core gui network qml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = iKunPlayer
TEMPLATE = app

# The following define makes yousr compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_APPLE_DEVICE_ARCHS=arm64

CONFIG += c++17

ICON=$$PWD/logo.icns

CONFIG(debug, debug|release){
    TARGET=$$join(TARGET,,,d)
    DESTDIR=$$PWD/../bin/debug
    OBJECTS_DIR=$$PWD/../build/debug/obj
    MOC_DIR=$$PWD/../build/debug/moc
    UI_DIR=$$PWD/../build/debug/ui
    RCC_DIR=$$PWD/../build/debug/rcc
}else{
    DESTDIR=$$PWD/../bin/release
    OBJECTS_DIR=$$PWD/../build/release/obj
    MOC_DIR=$$PWD/../build/release/moc
    UI_DIR=$$PWD/../build/release/ui
    RCC_DIR=$$PWD/../build/release/rcc
}

SOURCES += \
    comnetwork.cpp \
    controller.cpp \
    getdouyu.cpp \
    gethuya.cpp \
    getrealurl.cpp \
        main.cpp \
        mainwindow.cpp \
    mpvplayerwidget.cpp

HEADERS += \
    comnetwork.h \
    controller.h \
    getdouyu.h \
    gethuya.h \
    getrealurl.h \
        mainwindow.h \
    mpvplayerwidget.h

FORMS += \
        controller.ui \
        mainwindow.ui \
        mpvplayerwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


macx: LIBS += -L$$PWD/mpv/ -lmpv.2
win32: LIBS += -L$$PWD/mpv/ -llibmpv.dll

INCLUDEPATH += $$PWD/mpv/include
DEPENDPATH += $$PWD/mpv/include
