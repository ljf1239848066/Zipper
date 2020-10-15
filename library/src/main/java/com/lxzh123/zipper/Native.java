package com.lxzh123.zipper;

import android.content.Context;

/**
 * description $
 * author      Created by lxzh
 * date        2020/10/16
 */
class Native {
    static {
        System.loadLibrary("zipper");
    }

    public static native String unzipFromAssets(Context context, String zipName, String fileName);
}
