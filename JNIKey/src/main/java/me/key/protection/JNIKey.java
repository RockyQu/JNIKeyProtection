package me.key.protection;

/**
 * 用来加载 DecryptKey.so 获取解密 Key，内含签名防盗机制
 * <p>
 * 生成 .h 文件，如果你用的是动态注册，这步可以省略
 * 例：javah -d jni -classpath D:\Android\Workspace\JNIKeyProtection\Key\build\intermediates\javac\debug\compileDebugJavaWithJavac\classes me.key.protection.JNIKey
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
     * @return return key
     */
    public static native String getKey();
}