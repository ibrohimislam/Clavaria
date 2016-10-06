#include <jni.h>
#include <cstdint>

#include "rgba.h"
#include "imageProcess.h"

void *grayscaleProcedure(void *_threadJob);
void *mapProcedure(void *_threadJob);
void *reduceProcedure(void *_threadJob);

extern "C" jint Java_me_ibrohim_clavaria_MainActivity_grayscale(JNIEnv* env, jobject, jobject bitmapDestination, jobject bitmapSource) {
    return processPixel(env, bitmapDestination, bitmapSource, grayscaleProcedure);
}

extern "C" jint Java_me_ibrohim_clavaria_MainActivity_mapReduceTest(JNIEnv* env, jobject, jobject bitmapDestination, jobject bitmapSource) {
    return processMapReduce(env, bitmapDestination, bitmapSource, 8, mapProcedure, reduceProcedure);
}

uint32_t grayscaleFunction(rgba* input) {
    return uint8_t((0.299 * input->getR())+ (0.587 * input->getG())+ (0.114 * input->getB()));
}

void *grayscaleProcedure(void *_threadJob) {

    jobElement* threadJob = (jobElement*)_threadJob;
    jint* srcRow = (jint*)threadJob->src;
    jint* dstRow = (jint*)threadJob->dst;
    int width = threadJob->width;
    int height = threadJob->height;

    rgba *c = new rgba();
    uint32_t gray;

    for (int j = 0; j < height; ++j, srcRow += width, dstRow += width) {

        for (int i = 0; i < width; ++i) {
            c->setRGBA(uint32_t(srcRow[i]));
            gray = grayscaleFunction(c);

            c->setAll(gray);
            dstRow[i] = c->to_int();
        }
    }

    delete c;

    return 0;
}


void* mapProcedure(void *_threadJob) {

    jobElement* threadJob = (jobElement*)_threadJob;
    jint* srcRow = (jint*)threadJob->src;
    jint* dstRow = (jint*)threadJob->dst;
    int width = threadJob->width;
    int height = threadJob->height;

    for (int j = 0; j < height; ++j, srcRow += width, dstRow += width) {
        for (int i = 0; i < width; ++i) {
            dstRow[i] = uint32_t(srcRow[i]);
        }
    }

    return 0;
}

void* reduceProcedure(void *_threadJob) {

    jobElement* threadJob = (jobElement*)_threadJob;
    jint** srcRow = (jint**)threadJob->src;
    jint* dstRow = (jint*)threadJob->dst;

    int* _n = (int*)threadJob->info;
    int n = *_n;

    int width = threadJob->width;
    int height = threadJob->height;

    for (int j = 0; j < height; ++j, dstRow += width) {
        for (int i = 0; i < width; ++i) {
            dstRow[i] = uint32_t(srcRow[0][(j*width) + i]);
        }
    }

    return 0;
}