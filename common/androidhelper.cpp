#if defined(__ANDROID__)

#include "androidhelper.h"
#include "harfcore/core/device/hdevicefactory.h"
#include <QtAndroidExtras/QtAndroid>

void AndroidHelper::initialize(bool pStreamEnabled)
{
    QStringList permissionList;
    permissionList.append("android.permission.ACCESS_WIFI_STATE");
    permissionList.append("android.permission.WRITE_EXTERNAL_STORAGE");
    permissionList.append("android.permission.ACCESS_NETWORK_STATE");
    permissionList.append("android.permission.READ_EXTERNAL_STORAGE");
    permissionList.append("android.permission.INTERNET");
    permissionList.append("android.permission.READ_PHONE_STATE");
    permissionList.append("android.permission.RECORD_AUDIO");
    permissionList.append("android.permission.CAMERA");
    QtAndroid::requestPermissionsSync(permissionList);

    auto tEnv = HDeviceFactory::getJavaEnvironment();
    if (tEnv)
    {
        jobject tObjContext = tEnv->NewGlobalRef(QtAndroid::androidContext().object());
        jobject tObjActivity = tEnv->NewGlobalRef(QtAndroid::androidActivity().object());
        HDeviceFactory::setJNIContexAndActivity(tObjContext, tObjActivity, pStreamEnabled);
    }
}

#endif
