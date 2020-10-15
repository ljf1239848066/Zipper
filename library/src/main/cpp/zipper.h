//
// Created by lxzh on 2020/10/13.
//

#ifndef ZIPPERDEMO_GTZIP_H
#define ZIPPERDEMO_GTZIP_H

#include <jni.h>
#ifdef __cplusplus
extern "C" {
#endif


#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved);
JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved);

#ifdef __cplusplus
}
#endif

#endif //ZIPPERDEMO_GTZIP_H
