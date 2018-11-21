package me.key.protection.demo;

import android.content.Context;
import android.content.pm.PackageManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import me.key.protection.JNIKey;
import me.key.protection.JNISignature;

/**
 * 使用 so 文件存储私密数据，并增加签名防盗机制
 * <p>
 * https://rockycoder.cn/android%20ndk/2018/11/18/Android-NDK-DecryptKey.html
 */
public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        TextView value = findViewById(R.id.value);

        try {
            Context context = this.createPackageContext("me.decrypt.key", CONTEXT_INCLUDE_CODE | CONTEXT_IGNORE_SECURITY);

            Log.e("MainActivity", JNISignature.getSignature(getApplicationContext()));
            boolean flag = JNIKey.init();
            Log.e("MainActivity", String.valueOf(flag));
            String key = JNIKey.getKey();
            Log.e("MainActivity", key);

            value.setText(String.format("%s%s", flag, key));
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
    }
}
