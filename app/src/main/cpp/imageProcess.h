#include <jni.h>
#include <pthread.h>

#ifndef CLAVARIA_IMAGEPROCESS_H
#define CLAVARIA_IMAGEPROCESS_H

typedef struct jobElement {
    jint* src;
    jint* dst;
    int width;
    int height;
    void* info;
};

jintArray imageProcess(JNIEnv* env, jobject me, jintArray image, jint width, jint height, void *(*procedure)(void*) ){
    const int size = width*height;

    jintArray result;
    result = env->NewIntArray(size);
    if (result == NULL) {
        return NULL; /* out of memory error thrown */
    }

    jint* src = env->GetIntArrayElements(image, NULL);
    jint* dst = new jint[size];

    const int nThread = 3;
    const int element_size = height/nThread;

    jobElement thJob[nThread];

    for (int i=0; i<nThread; i++) {
        thJob[i].dst = dst + i*element_size*width;
        thJob[i].src = src + i*element_size*width;
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

    env->SetIntArrayRegion(result, 0, size, dst);
    return result;
}

#endif //CLAVARIA_IMAGEPROCESS_H
