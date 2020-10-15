package com.lxzh123.zipper;

import android.content.Context;

/**
 * description $
 * author      Created by lxzh
 * date        2020/10/16
 */
public class Zipper {
    public static String unzipFromAsset(Context context, String zipName, String fileName) {
        return Native.unzipFromAssets(context, zipName, fileName);
    }
}
