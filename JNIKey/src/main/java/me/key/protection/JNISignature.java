package me.key.protection;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;

public class JNISignature {

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