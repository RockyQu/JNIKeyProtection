#include <jni.h>
#include <string.h>
#include <assert.h>
#include <cstdlib>
#include "log.h"

// 需要被验证应用的包名
const char *APP_PACKAGE_NAME = "me.key.protection.demo";
// 应用签名，通过 JNIDecryptKey.getSignature(getApplicationContext()) 获取，注意开发版和发布版的区别，发布版需要使用正式签名打包后获取
const char *SIGNATURE_KEY = "308201dd30820146020101300d06092a864886f70d010105050030373116301406035504030c0d416e64726f69642044656275673110300e060355040a0c07416e64726f6964310b3009060355040613025553301e170d3138313031353033343532325a170d3438313030373033343532325a30373116301406035504030c0d416e64726f69642044656275673110300e060355040a0c07416e64726f6964310b300906035504061302555330819f300d06092a864886f70d010101050003818d003081890281810083f9d6810d4f93d1a3bd882f77e123853221dda2a52fe916b41c6d88e07b49c7f36e53c2b7d8d7d6cecdf933236fd43b50dea0d8c01f726056c02d5f9f4a4dfc8470aeae0c4b393067dbe954c16ec18427763df1ccacac4ca81251663c77e91194945b4bde3cc9fe70ecbcb49313e217793254b9e8513813dbb290775b9a9be70203010001300d06092a864886f70d0101050500038181002372375ff84ce8652762ec23020313ba1953d1fb9e47214f9150c750ddd06fac4048353a34e2501c4e2aa44748053e296c93dda8e491eff13c863286b94df5528234291985eb19a203c734c342206147586bf82451680d13d562008ae68c00edee2a775efbf4b6e63d4d9499aeee172cb8cd6024aaa8a9c50a83005a74cebc30";
// 需要被保护的密钥，请修改成你自己的密钥
const char *DECRYPT_KEY = "successful return key!";

// native 方法所在类的路径
const char *NATIVE_CLASS_PATH = "me/key/protection/JNIKey";

// 验证是否通过
static jboolean auth = JNI_FALSE;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 获取全局 Application
 */
jobject getApplicationContext(JNIEnv *env) {
    jclass activityThread = env->FindClass("android/app/ActivityThread");
    jmethodID currentActivityThread = env->GetStaticMethodID(activityThread, "currentActivityThread", "()Landroid/app/ActivityThread;");
    jobject at = env->CallStaticObjectMethod(activityThread, currentActivityThread);
    jmethodID getApplication = env->GetMethodID(activityThread, "getApplication", "()Landroid/app/Application;");
    return env->CallObjectMethod(at, getApplication);
}

/*
 * 初始化并判断当前 APP 是否为合法应用，只需调用一次
 */
JNICALL jboolean init(JNIEnv *env, jclass) {

    jclass binderClass = env->FindClass("android/os/Binder");
    jclass contextClass = env->FindClass("android/content/Context");
    jclass signatureClass = env->FindClass("android/content/pm/Signature");
    jclass packageNameClass = env->FindClass("android/content/pm/PackageManager");
    jclass packageInfoClass = env->FindClass("android/content/pm/PackageInfo");

    jmethodID packageManager = env->GetMethodID(contextClass, "getPackageManager", "()Landroid/content/pm/PackageManager;");
    jmethodID packageName = env->GetMethodID(contextClass, "getPackageName", "()Ljava/lang/String;");
    jmethodID toCharsString = env->GetMethodID(signatureClass, "toCharsString", "()Ljava/lang/String;");
    jmethodID packageInfo = env->GetMethodID(packageNameClass, "getPackageInfo", "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    jmethodID nameForUid = env->GetMethodID(packageNameClass, "getNameForUid", "(I)Ljava/lang/String;");
    jmethodID callingUid = env->GetStaticMethodID(binderClass, "getCallingUid", "()I");

    jint uid = env->CallStaticIntMethod(binderClass, callingUid);

    // 获取全局 Application
    jobject context = getApplicationContext(env);

    jobject packageManagerObject = env->CallObjectMethod(context, packageManager);
    jstring packNameString = (jstring) env->CallObjectMethod(context, packageName);
    jobject packageInfoObject = env->CallObjectMethod(packageManagerObject, packageInfo, packNameString, 64);
    jfieldID signaturefieldID = env->GetFieldID(packageInfoClass, "signatures", "[Landroid/content/pm/Signature;");
    jobjectArray signatureArray = (jobjectArray) env->GetObjectField(packageInfoObject, signaturefieldID);
    jobject signatureObject = env->GetObjectArrayElement(signatureArray, 0);
    jstring runningPackageName = (jstring) env->CallObjectMethod(packageManagerObject, nameForUid, uid);

    if (runningPackageName) {// 正在运行应用的包名
        const char *charPackageName = env->GetStringUTFChars(runningPackageName, 0);
        if (strcmp(charPackageName, APP_PACKAGE_NAME) != 0) {
            return JNI_FALSE;
        }
        env->ReleaseStringUTFChars(runningPackageName, charPackageName);
    } else {
        return JNI_FALSE;
    }

    jstring signatureStr = (jstring) env->CallObjectMethod(signatureObject, toCharsString);
    const char *signature = env->GetStringUTFChars((jstring) env->CallObjectMethod(signatureObject, toCharsString), NULL);

    env->DeleteLocalRef(binderClass);
    env->DeleteLocalRef(contextClass);
    env->DeleteLocalRef(signatureClass);
    env->DeleteLocalRef(packageNameClass);
    env->DeleteLocalRef(packageInfoClass);

    LOGE("current apk signature %s", signature);
    LOGE("reserved signature %s", SIGNATURE_KEY);
    if (strcmp(signature, SIGNATURE_KEY) == 0) {
        LOGE("verification passed");
        env->ReleaseStringUTFChars(signatureStr, signature);
        auth = JNI_TRUE;
        return JNI_TRUE;
    } else {
        LOGE("verification failed");
        auth = JNI_FALSE;
        return JNI_FALSE;
    }
}

/*
 * 获取 Key
 */
JNIEXPORT jstring JNICALL getKey(JNIEnv *env, jclass) {
    if (auth) {
        return env->NewStringUTF(DECRYPT_KEY);
    } else {// 你没有权限，验证没有通过。
        return env->NewStringUTF("You don't have permission, the verification didn't pass.");
    }
}

/*
 * 动态注册 native 方法数组，可以不受方法名称的限制，与 Java native 方法一一对应
 */
static JNINativeMethod registerMethods[] = {
        {"init",   "()Z",                  (jboolean *) init},
        {"getKey", "()Ljava/lang/String;", (jstring *) getKey},
};

/*
 * 动态注册 native 方法
 */
static int registerNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *gMethods, int numMethods) {
    jclass clazz = env->FindClass(className);
    if (clazz == NULL) {
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

/*
 * 默认执行的初始化方法
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGE("JNI_OnLoad");

    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    LOGE("register native methods");
    if (!registerNativeMethods(env, NATIVE_CLASS_PATH, registerMethods, sizeof(registerMethods) / sizeof(registerMethods[0]))) {
        LOGE("register native methods failed");
        return -1;
    }

    LOGE("register native methods success");
    return JNI_VERSION_1_6;
}

#ifdef __cplusplus
}
#endif