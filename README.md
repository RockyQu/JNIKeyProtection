# JNIKeyProtection
Android 使用 so 文件存储私密数据，并增加签名防盗机制

## 0x00 实际项目中引出的一些需求问题
> 有时你需要在客户端存放一些保密的数据，比如某些授权 Key ，如果直接写在 Java 中，会很容易被反编译看到，那么我们可以把这些数据存在 so 文件中，来增加反编译难度，并且增加 APP 签名防盗机制来防止别人盗用 so 文件。

## 0x01 一些准备工作
- 配置 NDK 开发环境，看这里 → [Android Studio NDK 开发安装配置](https://rockycoder.cn/android%20ndk/2018/01/18/Android-Studio-JNI-Exercise.html)  
- 有必要的了解一下 NDK 开发基础（此例子使用的是 CMake）、最好学习一下 C/C++ 基础，不然代码看起来很费劲

## 0x02 编写配置文件、Java、C/C++ 代码
1、新建 JNIKey.class 并声明 native 方法

```
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
```

2、Build → Rebuild Project 生成 class 文件，生成目录一般在如下目录

```
JNIKey\build\intermediates\javac\debug\compileDebugJavaWithJavac\classes\me\key\protection\
```

3、根据生成的 class 文件生成 .h 文件，进入 Terminal 命令窗口输入以下命令

```
javah -d jni -classpath D:\Android\Workspace\JNIKeyProtection\Key\build\intermediates\javac\debug\compileDebugJavaWithJavac\classes me.key.protection.JNIKey
```

> 注意路径不要写错了，生成的 .h 文件里包含自动生成的一些方法，方法名称一一应对 Java native 方法，如果在 .cpp 代码里用的是动态注册的方式，这步可以忽略不做，因为动态注册方法名可以随便写，具体看 .cpp 里的代码

4、在 src\main 目录下新建 cpp 目录，新建 Key.cpp 文件在这里编写 C++ 代码，.cpp 表示 C++ 文件, .c 表示 C 文件

![1](https://github.com/RockyQu/RockyQu.github.io/blob/master/assets/image/2018-11-18/2018-11-18_1.png)
![screenshots](https://github.com/DesignQu/Logg/blob/master/ImageFolder/screenshots.png "screenshots")

5、在 Module 根目录下新建 CmakeLists.txt 文件，配置 JNI 相关参数

```
# 指定编译器版本
cmake_minimum_required(VERSION 3.4.1)

# 存放生成 so 库的目录
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/${ANDROID_ABI})

# 配置 so 库信息
add_library(

        # 生成的 so 库名称，此处生成的 so 文件名称是libKey.so
        Key

        # STATIC：静态库，是目标文件的归档文件，在链接其它目标的时候使用
        # SHARED：动态库，会被动态链接，在运行时被加载
        # MODULE：模块库，是不会被链接到其它目标中的插件，但是可能会在运行时使用dlopen-系列的函数动态链接
        SHARED

        # 资源文件，可以多个，资源路径是相对路径，相对于本CMakeLists.txt所在目录
        src/main/cpp/Key.cpp)

# 依赖 NDK 中的 log 日志库
find_library(
        log-lib
        log)

# 关联 log 库到本地库。如果你本地的库（DecryptKey）想要调用log库的方法，那么就需要配置这个属性
target_link_libraries(

        # 目标库
        Key

        # 依赖库
        ${log-lib})
```

> 注意是 Module 根目录，不是 Project 根目录

6、修改对应 Module 的 build.gradle 文件添加配置参数

- defaultConfig 标签里添加如下配置

```
externalNativeBuild {
        cmake {
            cppFlags "-frtti -fexceptions"
        }
    }

    // 平台架构支持
    ndk {
        abiFilters 'arm64-v8a', 'armeabi', 'armeabi-v7a', 'x86', 'x86_64'
    }
```

- android 标签里添加如下配置

```
externalNativeBuild {
    cmake {
        path "CMakeLists.txt"
    }
}
```

7、在 cpp 文件夹下新建 Key.cpp 文件里面放 c++ 代码
> 完整代码 → [Key.cpp](https://github.com/RockyQu/JNIKeyProtection/blob/master/JNIKey/src/main/cpp/Key.cpp)  

## 0x03 调用生成的 .so 文件
- 默认 .so 文件生成目录，复制出来放到 libs 目录下，即可使用

```
{项目目录}\JNIKey\build\intermediates\cmake\debug\obj\
```

- 你可以对生成的库文件封装一个 Jar 包，这样 .so 就不必放在特定的包下，Jar 包的生成必须要 Module 里，默认 Jar 包生成目录

```
{项目目录}\JNIKey\build\intermediates\intermediate-jars\debug\classes.jar
```
