#include <jni.h>
#include <cstdint>
#include <string>

#include <omp.h>

#include <android/log.h>

#include "rgba.h"

extern "C" jintArray Java_me_ibrohim_clavaria_MainActivity_grayscale(JNIEnv*, jobject, jintArray, jint, jint);
extern "C" jintArray Java_me_ibrohim_clavaria_MainActivity_histogram(JNIEnv*, jobject, jintArray, jint, jint);

uint32_t grayscale(rgba* input) {
    return uint8_t((0.299 * input->getR())+ (0.587 * input->getG())+ (0.114 * input->getB()));
}

jintArray Java_me_ibrohim_clavaria_MainActivity_grayscale(JNIEnv* env, jobject me, jintArray image, jint width, jint height) {

    int size = width*height;

#ifdef OPENMP
    __android_log_print(ANDROID_LOG_VERBOSE, "TEST", "OPENMP FOUND");
#else
    __android_log_print(ANDROID_LOG_VERBOSE, "TEST", "OPENMP NOT FOUND");
#endif

    jintArray result;
    result = env->NewIntArray(size);
    if (result == NULL) {
        return NULL; /* out of memory error thrown */
    }

    jint* src = env->GetIntArrayElements(image, NULL);
    jint* dst = new jint[size];

    rgba *c = new rgba();

    jint* srcRow = src;
    jint* dstRow = dst;

#pragma omp parallel for
    for (int j = 0; j < height; ++j, srcRow += width, dstRow += width) {

        for (int i = 0; i < width; ++i) {
            c->setRGBA(uint32_t(srcRow[i]));
            uint32_t gray = grayscale(c);

            c->setAll(gray);
            dstRow[i] = c->to_int();
        }
    }

    delete c;

    env->SetIntArrayRegion(result, 0, size, dst);
    return result;
}
