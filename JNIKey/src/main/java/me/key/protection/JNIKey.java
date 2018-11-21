package me.key.protection;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;

/**
 * 用来加载 DecryptKey.so 获取解密 Key，内含签名防盗机制
 * <p>
 * 生成 .h 文件
 * 例：javah -d jni -classpath D:\Android\Workspace\DecryptKey\Key\build\intermediates\javac\debug\compileDebugJavaWithJavac\classes com.ld.decrypt.key.JNIDecryptKey
 */
public class JNIKey {

    static {
        System.loadLibrary("Key");
    }

    /**
     * 初始化并判断当前 APP 是否为合法应用，只需调用一次
     *
     * @return 返回 true 则初始化成功并当前 APP 为合法应用
     */
    public static native boolean init();

    /**
     * 获取 Key
     *
     * @return 解密 Key
     */
    public static native String getKey();

    /**
     * 获取应用签名，获取签名后替换 Key.cpp 的 SIGNATURE_KEY 变量
     *
     * @param context
     * @return
     */
    public static String getSignature(Context context) {
        try {
            PackageInfo packageInfo = context.getPackageManager().getPackageInfo(context.getPackageName(), PackageManager.GET_SIGNATURES);
            Signature[] signatures = packageInfo.signatures;
            return signatures[0].toCharsString();
        } catch (Exception e) {
            e.printStackTrace();
        }

        return null;
    }
}