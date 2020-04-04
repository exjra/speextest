DEFINES += H_DEVTOOL_QT

win32 {
    DEFINES += H_PLATFORM_WIN
}

android {
    DEFINES += H_PLATFORM_ANDROID
}

ios {
    DEFINES += H_PLATFORM_IOS
}

macos {
    DEFINES += H_PLATFORM_MACOS
}

unix{
    DEFINES += H_PLATFORM_UNIX
}

unix:!android{
    DEFINES += H_PLATFORM_LINUX
}

CONFIG(debug, debug|release) {
    DEFINES += H_DEBUG
} else {
    DEFINES += H_RELEASE
}
