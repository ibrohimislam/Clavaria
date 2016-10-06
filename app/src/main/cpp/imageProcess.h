#include <jni.h>
#include <pthread.h>
#include <android/bitmap.h>

#ifndef CLAVARIA_IMAGEPROCESS_H
#define CLAVARIA_IMAGEPROCESS_H

typedef struct jobElement {
    void* src;
    void* dst;
    int width;
    int height;
    void* info;
} jobElement;

typedef struct mapReduce {
    void* map;
    void* reduce;
    int n;
} mapReduce;

jint bitmapInterface(JNIEnv* env, jobject bitmapDestination, jobject bitmapSource, void (*procedure)(jint*, jint*, uint32_t, uint32_t, void*), void* callback){
    AndroidBitmapInfo  info;
    jint*              src;
    jint*              dst;

    AndroidBitmap_getInfo(env, bitmapSource, &info);
    if(info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        return -1;
    }

    const uint32_t height = info.height;
    const uint32_t width  = info.width;

    AndroidBitmap_lockPixels(env, bitmapDestination, reinterpret_cast<void **>(&dst));
    AndroidBitmap_lockPixels(env, bitmapSource, reinterpret_cast<void **>(&src));

    procedure(dst, src, width, height, callback);

    AndroidBitmap_unlockPixels(env, bitmapSource);
    AndroidBitmap_unlockPixels(env, bitmapDestination);

    return 0;
}

void pixelProcessing(jint* dst, jint* src, uint32_t width, uint32_t height, void* pixelFunction){
    const int nThread = 3;
    const int element_size = height/nThread;

    void* (*procedure)(void*) = (void* (*)(void*))pixelFunction;

    jobElement thJob[nThread];

    for (int i=0; i<nThread; i++) {
        thJob[i].dst = (void*)(dst + i*element_size*width);
        thJob[i].src = (void*)(src + i*element_size*width);
        thJob[i].width = width;
        thJob[i].height = element_size;
    }

    pthread_t th[nThread];

    void* status;

    for (int i = 0; i < nThread; ++i) {
        pthread_create(&th[i], NULL, procedure, (void*)&thJob[i]);
    }
    for (int i = 0; i < nThread; ++i) {
        pthread_join(th[i], &status);
    }

}

void mapReduceProcessing(jint* dst, jint* src, uint32_t width, uint32_t height, void* _mapReduceJob){
    mapReduce* mapReduceJob = (mapReduce*) _mapReduceJob;

    int nThread = mapReduceJob->n;

    void* (*mapProcedure)(void*) = (void* (*)(void*))mapReduceJob->map;
    void* (*reduceProcedure)(void*) = (void* (*)(void*))mapReduceJob->reduce;

    jobElement thJob[nThread+1];

    uint32_t size = width*height;

    // prepare map result
    jint** mapResult = new jint*[nThread];
    for (int i = 0; i < nThread; ++i) {
        mapResult[i] = new jint[size];
    }

    // prepare map jobs
    for (int i=1; i<= nThread; i++) {
        int* info = new int;
        *info = i;

        thJob[i].dst = (void*)mapResult[i-1];
        thJob[i].src = (void*)src;
        thJob[i].width = width;
        thJob[i].height = height;
        thJob[i].info = (void*)info;
    }

    pthread_t th[nThread+1];
    void* status;

    for (int i = 1; i <= nThread; ++i) {
        pthread_create(&th[i], NULL, mapProcedure, (void*)&thJob[i]);
    }
    for (int i = 1; i <= nThread; ++i) {
        pthread_join(th[i], &status);
    }

    // cleanup map jobs
    for (int i=1; i<= nThread; i++) {
        int* info = (int*)thJob[i].info;
        thJob[i].info = NULL;

        delete info;
    }

    int* info = new int;
    *info = mapReduceJob->n;

    thJob[0].dst = (void*)dst;
    thJob[0].src = (void*)mapResult;
    thJob[0].width = width;
    thJob[0].height = height;
    thJob[0].info = (void*)info;

    pthread_create(&th[0], NULL, reduceProcedure, (void*)&thJob[0]);
    pthread_join(th[0], &status);

    // cleanup map result
    for (int i = 0; i < nThread; ++i) {
        delete mapResult[i];
    }
    delete mapResult;
}

jint processPixel(JNIEnv* env, jobject bitmapDestination, jobject bitmapSource, void* (*pixelFunction)(void*) ){
    bitmapInterface(env, bitmapDestination, bitmapSource, pixelProcessing, (void*)pixelFunction);
    return 0;
}

jint processMapReduce(JNIEnv* env, jobject bitmapDestination, jobject bitmapSource, int n, void* (*map)(void*), void* (*reduce)(void*)){

    mapReduce* mapReduceJob = new mapReduce();
    mapReduceJob->n = n;
    mapReduceJob->map = (void*)map;
    mapReduceJob->reduce = (void*)reduce;

    bitmapInterface(env, bitmapDestination, bitmapSource, mapReduceProcessing, (void*) mapReduceJob);

    delete mapReduceJob;

    return 0;

}

#endif //CLAVARIA_IMAGEPROCESS_H
