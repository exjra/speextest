#-------------------------------------------------
#
# Project created by QtCreator 2020-03-25T16:22:20
#
#------------------------------------------------- 11

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = speextests
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

DEFINES += HAS_HARFSDK

SOURCES += \
        haecmanager.cpp \
        haudiomanager.cpp \
        hdevice.cpp \
        hearbuffer.cpp \
        hearbuffernetwork.cpp \
        hmicbuffer.cpp \
        hmicbuffernetwork.cpp \
        main.cpp \
        mainwindow.cpp

contains(DEFINES, HAS_HARFSDK) {
    win32 {
        SOURCES += haudioclient.cpp
    }
}

HEADERS += \
        haecmanager.h \
        haudiomanager.h \
        hdevice.h \
        hearbuffer.h \
        hearbuffernetwork.h \
        hmicbuffer.h \
        hmicbuffernetwork.h \
        mainwindow.h

contains(DEFINES, HAS_HARFSDK) {
    win32 {
        HEADERS += haudioclient.h
    }
}

FORMS += \
        mainwindow.ui

win32 {
    LIBS += -L$$PWD/../speex-qt -lspeex
    INCLUDEPATH += $$PWD/../speex-qt/include

    LIBS += -L$$PWD/../speexdsp-qt -lspeexdsp
    INCLUDEPATH += $$PWD/../speexdsp-qt/include
}

android {
    LIBS += -L$$PWD/../speex-qt/android -lspeex
    INCLUDEPATH += $$PWD/../speex-qt/include

    LIBS += -L$$PWD/../speexdsp-qt/android -lspeexdsp
    INCLUDEPATH += $$PWD/../speexdsp-qt/include
}

#include($$(HARF_DIR)/lib/include/path.pri)
#include($$(HARF_DIR)/lib/include/speex.pri)

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

ANDROID_PACKAGE_SOURCE_DIR = \
    $$PWD/android

contains(DEFINES, HAS_HARFSDK) {
    win32 {
        include(harfsdk.pri)
    }
}

#%HARFSDK%/win32/msvc2017_64/3rdparty/opencv/bin;%HARFSDK%/win32/msvc2017_64/3rdparty/openssl/bin;%HARFSDK%/win32/msvc2017_64/3rdparty/osg/bin;%HARFSDK%/win32/msvc2017_64/3rdparty/zlib/bin;%HARFSDK%/win32/msvc2017_64/bin;%HARFSDK%\win32\msvc2017_64\3rdparty\cpprestsdk\bin;%HARFSDK%\win32\msvc2017_64\3rdparty\boost\bin
