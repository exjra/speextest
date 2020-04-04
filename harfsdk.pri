CONFIG += c++11
CONFIG += rtti

include($$PWD/common/path.pri)
include($$PWD/common/platform.pri)

SDKPATH = $$(HARFSDK)/$$OS/$$COMPILER

INCLUDEPATH += $$SDKPATH/include
DEPENDPATH += $$SDKPATH/include

LIBS += -L$$SDKPATH/lib

CONFIG += harfcore
CONFIG += harfarengine
CONFIG += harfconnection
CONFIG += harfstream

harfcore{
    CONFIG(debug, debug|release) {
        LIBS += -lharfcored
        ANDROID_EXTRA_LIBS += $$SDKPATH/lib/libharfcored.so
    } else {
        LIBS += -lharfcore
        ANDROID_EXTRA_LIBS += $$SDKPATH/lib/libharfcore.so
    }

    android {
        ANDROID_EXTRA_LIBS += \
            #arcore
            $$SDKPATH/3rdparty/arcore/lib/libarcore_sdk_c.so \
            $$SDKPATH/3rdparty/arcore/lib/libarcore_sdk_jni.so \
            #boost
            $$SDKPATH/3rdparty/boost/lib/libboost_context.so \
            $$SDKPATH/3rdparty/boost/lib/libboost_coroutine.so \
            $$SDKPATH/3rdparty/boost/lib/libboost_date_time.so \
            $$SDKPATH/3rdparty/boost/lib/libboost_filesystem.so \
            $$SDKPATH/3rdparty/boost/lib/libboost_iostreams.so \
            $$SDKPATH/3rdparty/boost/lib/libboost_system.so \
            $$SDKPATH/3rdparty/boost/lib/libboost_thread.so \
            $$SDKPATH/3rdparty/boost/lib/libboost_chrono.so \
            #openssl
            $$SDKPATH/3rdparty/openssl/lib/libcrypto.so \
            $$SDKPATH/3rdparty/openssl/lib/libssl.so \
    }
}

harfarengine{
    CONFIG(debug, debug|release) {
        LIBS += -larengined
        ANDROID_EXTRA_LIBS += $$SDKPATH/lib/libarengined.so
    } else {
        LIBS += -larengine
        ANDROID_EXTRA_LIBS += $$SDKPATH/lib/libarengine.so
    }

    INCLUDEPATH += $$SDKPATH/3rdparty/eigen/include
    DEPENDPATH += $$SDKPATH/3rdparty/eigen/include
    android {
        LIBS += -landroid -lEGL -lGLESv2

        ANDROID_EXTRA_LIBS += \
            #opencv
            $$SDKPATH/3rdparty/opencv/lib/libopencv_aruco.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_bgsegm.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_bioinspired.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_calib3d.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_ccalib.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_core.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_datasets.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_dnn.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_dnn_objdetect.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_dpm.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_face.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_features2d.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_flann.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_fuzzy.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_hfs.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_highgui.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_imgcodecs.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_imgproc.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_img_hash.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_line_descriptor.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_ml.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_objdetect.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_optflow.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_phase_unwrapping.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_photo.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_plot.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_reg.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_rgbd.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_saliency.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_shape.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_stereo.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_stitching.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_structured_light.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_superres.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_surface_matching.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_text.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_tracking.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_video.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_videoio.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_videostab.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_ximgproc.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_xobjdetect.so \
            $$SDKPATH/3rdparty/opencv/lib/libopencv_xphoto.so \
    }
}

harfconnection{
    CONFIG(debug, debug|release) {
        LIBS += -lharfconnectiond
        ANDROID_EXTRA_LIBS += $$SDKPATH/lib/libharfconnectiond.so
    } else {
        LIBS += -lharfconnection
        ANDROID_EXTRA_LIBS += $$SDKPATH/lib/libharfconnection.so
    }
}

harfstream{
    CONFIG(debug, debug|release) {
        LIBS += -lstreamd
        ANDROID_EXTRA_LIBS += $$SDKPATH/lib/libstreamd.so
    } else {
        LIBS += -lstream
        ANDROID_EXTRA_LIBS += $$SDKPATH/lib/libstream.so
    }


    android {
        ANDROID_EXTRA_LIBS += \
            #gstreamer
            $$SDKPATH/3rdparty/gstreamer/lib/libgstreamer_android.so \
    }
}

android {
    QMAKE_LFLAGS += -nostdlib++

    LIBS += -landroid #from android ndk

#    DISTFILES += \
#        android/libs/harfandroid-release.aar \
}
