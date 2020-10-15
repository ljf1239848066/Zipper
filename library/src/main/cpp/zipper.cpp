//
// Created by lxzh on 2020/10/13.
//
#include "zipper.h"
#include "utils/common.h"
#include "minizip/mz_zip_rw.h"
#include "minizip/mz.h"

#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/mman.h>


#define JNIREG_CLASS "com/lxzh123/zipper/Native"//指定要注册的类

#define MAGIC   "zip"
#define BUFFER_4096 4096

int gSdkInt = 0;
char gPrivDir[256] = {0};
const char *gFileDir = nullptr;

/**
 * 处理 java 报错
 * @param env
 */
void errorCatch(JNIEnv *env) {
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
}

/**
 * 解密 dex 文件
 *
 * @param env
 * @param ctx
 * @param szDexPath 解密文件存放路径
 * @param fileName  待解密源文件
 * @return
 */
int copyZipFile(JNIEnv *env, jobject ctx, const char *szDexPath, const char *fileName) {
    jclass applicationClass = env->GetObjectClass(ctx);
    if (applicationClass == NULL) {
        LOGE("[-] applicationClass failed");
        errorCatch(env);
        return 0;
    }
    jmethodID getAssetsMethod = env->GetMethodID(applicationClass, "getAssets",
                                                 "()Landroid/content/res/AssetManager;");
    jobject assetsObj = env->CallObjectMethod(ctx, getAssetsMethod);
    if (assetsObj == NULL) {
        LOGE("[-] assetsObj failed");
        errorCatch(env);
        return 0;
    }
    AAssetManager *mgr = AAssetManager_fromJava(env, assetsObj);
    if (mgr == NULL) {
        LOGE("[-] getAAssetManager failed");
        errorCatch(env);
        return 0;
    }
    AAsset *asset = AAssetManager_open(mgr, fileName, AASSET_MODE_STREAMING);
    if (asset == NULL) {
        LOGE("[-] AAssetManager_open file:%s failed", fileName);
        errorCatch(env);
        return 0;
    }

    int bufferSize = AAsset_getLength(asset);
    FILE *file = fopen(szDexPath, "wb");
    if (access(szDexPath, F_OK) == 0) {
        int dexSize = ftell(file);
        LOGI("[+] Assets file size %s, existed file size:%d", szDexPath, dexSize);
    }

    LOGI("[+] Asset FileName:%s, extract path:%s, size:%d\n", fileName, szDexPath, bufferSize);
    unsigned char *buffer = (unsigned char *) malloc(BUFFER_4096);
    if (buffer == NULL) {
        LOGE("[-] buffer malloc failed.");
        return 0;
    }

    while (true) {
        int size = AAsset_read(asset, buffer, BUFFER_4096);
        if (size <= 0) {
            break;
        }
        fwrite(buffer, size, 1, file);
    }
    free(buffer);
    fclose(file);
    AAsset_close(asset);
    env->DeleteLocalRef(assetsObj);
    env->DeleteLocalRef(applicationClass);
    chmod(szDexPath, 493);
    return 1;
}

jstring unzipFile(JNIEnv *env, jobject ctx, const char *zipPath) {
    jstring content = env->NewStringUTF("null");
    char *fileName = "$fileName";
    char *password = "$password";
    char *zipBuf = nullptr;

    void *reader = NULL;
    int32_t err = MZ_OK;
    int32_t err_close = MZ_OK;

    LOGI("[+] Archive %s", zipPath);
    char *destination = "/data/data/com.lxzh123.zipperdemo/files/zip/";

    /* Create zip reader */
    mz_zip_reader_create(&reader);
    mz_zip_reader_set_password(reader, password);
    err = mz_zip_reader_open_file(reader, zipPath);

    if (err != MZ_OK) {
        LOGE("[-] Error %" PRId32 " opening archive %s", err, zipPath);
    } else {
        LOGI("[+] Archive open success");
        err = mz_zip_reader_locate_entry(reader, fileName, 1);
        if (err != MZ_OK) {
            LOGE("[-] Error locate entry %s on opening archive %s error:%d", fileName, zipPath, err);
        } else {
            LOGI("[+] Archive locate entry success");
            int32_t buf_size = (int32_t)mz_zip_reader_entry_save_buffer_length(reader);
            int32_t str_size = buf_size;
            LOGI("[+] Archive entry save buffer length:%d", buf_size);
            zipBuf = (char *)malloc((size_t)str_size);
            memset(zipBuf, 0, (size_t)str_size);
            LOGI("[+] Archive init buffer success");
            err = mz_zip_reader_entry_save_buffer(reader, zipBuf, buf_size);
            if (err != MZ_OK) {
                LOGE("[-] Error save entry %s buffer to %p error:%d", fileName, zipBuf, err);
            } else {
                LOGI("[+] Archive entry save buffer success");
                content = env->NewStringUTF((char*)(zipBuf));
                free(zipBuf);
            }
        }
    }

    err_close = mz_zip_reader_close(reader);
    if (err_close != MZ_OK) {
        LOGE("[-] Error %" PRId32 " closing archive for reading", err_close);
        err = err_close;
    }
    LOGI("[+] Archive extract finish");
    mz_zip_reader_delete(&reader);

    return content;
}

jstring unzipFomeAssets(JNIEnv *env, jobject ctx, const char *zipName, const char *fileName, const char *password) {
    jclass applicationClass = env->GetObjectClass(ctx);
    if (applicationClass == NULL) {
        LOGE("[-] applicationClass failed");
        errorCatch(env);
        return 0;
    }
    jmethodID getAssetsMethod = env->GetMethodID(applicationClass, "getAssets",
                                                 "()Landroid/content/res/AssetManager;");
    jobject assetsObj = env->CallObjectMethod(ctx, getAssetsMethod);
    if (assetsObj == NULL) {
        LOGE("[-] assetsObj failed");
        errorCatch(env);
        return 0;
    }
    AAssetManager *mgr = AAssetManager_fromJava(env, assetsObj);
    if (mgr == NULL) {
        LOGE("[-] getAAssetManager failed");
        errorCatch(env);
        return 0;
    }
    AAsset *asset = AAssetManager_open(mgr, zipName, AASSET_MODE_STREAMING);
    if (asset == NULL) {
        LOGE("[-] AAssetManager_open file:%s failed", zipName);
        errorCatch(env);
        return 0;
    }

    int zipSize = AAsset_getLength(asset);
    unsigned char *zipBuff = (unsigned char *)malloc(zipSize);

    LOGI("[+] Asset FileName:%s, size:%d\n", zipName, zipSize);
    unsigned char *buffer = (unsigned char *) malloc(BUFFER_4096);
    if (buffer == NULL) {
        LOGE("[-] buffer malloc failed.");
        return 0;
    }

    int total = 0;
    while (true) {
        int size = AAsset_read(asset, buffer, BUFFER_4096);
        if (size <= 0) {
            break;
        }
        memcpy(zipBuff + total, buffer, size);
        total += size;
    }
    free(buffer);
    AAsset_close(asset);
    env->DeleteLocalRef(assetsObj);
    env->DeleteLocalRef(applicationClass);

    jstring content = env->NewStringUTF("null");
    char *zipBuf = nullptr;

    void *reader = NULL;
    int32_t err = MZ_OK;
    int32_t err_close = MZ_OK;

    LOGI("[+] Archive start");

    /* Create zip reader */
    mz_zip_reader_create(&reader);
    mz_zip_reader_set_password(reader, password);
    err = mz_zip_reader_open_buffer(reader, zipBuff, zipSize, 0);

    if (err != MZ_OK) {
        LOGE("[-] Error %" PRId32 " opening buffer archive %p", err, zipBuf);
    } else {
        LOGI("[+] Archive open success");
        err = mz_zip_reader_locate_entry(reader, fileName, 1);
        if (err != MZ_OK) {
            LOGE("[-] Error locate entry %s on opening buffer archive %p error:%d", fileName, zipBuf, err);
        } else {
            LOGI("[+] Archive locate entry success");
            int32_t buf_size = (int32_t)mz_zip_reader_entry_save_buffer_length(reader);
            int32_t str_size = buf_size + 1;// 字符串增加结束符
            LOGI("[+] Archive entry save buffer length:%d", buf_size);
            zipBuf = (char *)malloc((size_t)str_size);
            memset(zipBuf, 0, (size_t)str_size);
            LOGI("[+] Archive init buffer success");
            err = mz_zip_reader_entry_save_buffer(reader, zipBuf, buf_size);
            if (err != MZ_OK) {
                LOGE("[-] Error save entry %s buffer to %p error:%d", fileName, zipBuf, err);
            } else {
                LOGI("[+] Archive entry save buffer success");
                content = env->NewStringUTF((char*)(zipBuf));
                free(zipBuf);
            }
        }
    }

    err_close = mz_zip_reader_close(reader);
    if (err_close != MZ_OK) {
        LOGE("[-] Error %" PRId32 " closing archive for reading", err_close);
        err = err_close;
    }
    LOGI("[+] Archive extract finish");
    mz_zip_reader_delete(&reader);

    return content;
}

JNIEXPORT jstring JNICALL
unzip(JNIEnv *env, jclass clazz, jobject ctx, jstring zipFileName, jstring entryFileName) {
    jstring rst = env->NewStringUTF("");
    jclass applicationClass = env->GetObjectClass(ctx);
    if (applicationClass == NULL) {
        LOGE("[-] applicationClass failed");
        errorCatch(env);
        return rst;
    }
    jmethodID getFilesDir = env->GetMethodID(applicationClass, "getFilesDir", "()Ljava/io/File;");
    jobject fileObj = env->CallObjectMethod(ctx, getFilesDir);
    if (fileObj == NULL) {
        LOGE("[-] fileObj failed");
        errorCatch(env);
        return rst;
    }

    jclass FileClass = env->GetObjectClass(fileObj);
    jmethodID getAbsolutePathMethod = env->GetMethodID(FileClass, "getAbsolutePath",
                                                       "()Ljava/lang/String;");
    jstring dataFileDir = (jstring) (env->CallObjectMethod(fileObj,
                                                           getAbsolutePathMethod));
    if (dataFileDir == NULL) {
        LOGE("[-] dataFileDir failed");
        errorCatch(env);
        return rst;
    }

    gFileDir = env->GetStringUTFChars(dataFileDir, JNI_FALSE);
    LOGI("[+] FilesDir:%s", gFileDir);

    char privPath[256] = {0}; // 加密dex的存储路径

    const char *zipName = env->GetStringUTFChars(zipFileName, NULL);
    const char *fileName = env->GetStringUTFChars(entryFileName, NULL);
    sprintf(gPrivDir, "%s/%s", gFileDir, MAGIC);
    sprintf(privPath, "%s/%s", gPrivDir, zipName);
    LOGI("[+] Helper_init gPrivDir:%s, privPath:%s", gPrivDir, privPath);
    if (access(gPrivDir, F_OK) != 0) {
        if (mkdir(gPrivDir, 0755) == -1) {
            LOGE("[-] mkdir %s error:%s", gPrivDir, strerror(errno));
            return rst;
        }
    }

    LOGI("[+] start unzip");
    rst = unzipFomeAssets(env, ctx, zipName, fileName, "$password");
    LOGI("[+] unzip finished");

    errorCatch(env);

    env->DeleteLocalRef(dataFileDir);
    env->DeleteLocalRef(fileObj);
    env->DeleteLocalRef(FileClass);
    env->DeleteLocalRef(applicationClass);
    return rst;
}

static JNINativeMethod gMethods[] = {
        {"unzipFromAssets", "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", (void *) unzip}
};

int jniRegisterNativeMethods(JNIEnv *env, const char *className, const JNINativeMethod *gMethods,
                             int numMethods) {
    LOGI("[+] jniRegisterNativeMethods");
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        return -1;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        LOGE("[-] RegisterNatives failed");
        return -1;
    }
    return 0;
}

void initJni(JNIEnv *env) {
    LOGI("[+] init");
    jclass VersionClass = env->FindClass("android/os/Build$VERSION");
    jfieldID SDK_INT = env->GetStaticFieldID(VersionClass, "SDK_INT", "I");

    gSdkInt = env->GetStaticIntField(VersionClass, SDK_INT);
    LOGI("[+] init sdk_int:%d", gSdkInt);

    jniRegisterNativeMethods(env, JNIREG_CLASS, gMethods, NELEM(gMethods));
    env->DeleteLocalRef(VersionClass);
}

JNIEXPORT int JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGI("[+] JNI_OnLoad");
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    initJni(env);
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
    LOGI("[+] JNI_OnUnload");
}