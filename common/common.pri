QT = gui qml quick

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/androidhelper.h \
    $$PWD/hqmlviewer.h \

SOURCES += \
    $$PWD/androidhelper.cpp \
    $$PWD/hqmlviewer.cpp \

android {
    QT += androidextras
}
