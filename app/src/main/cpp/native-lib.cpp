#include <jni.h>
#include <cstdint>

#include "rgba.h"
#include "imageProcess.h"

void *grayscaleProcedure(void *_threadJob);
extern "C" jintArray Java_me_ibrohim_clavaria_MainActivity_grayscale(JNIEnv* env, jobject me, jintArray image, jint width, jint height) {
    return imageProcess(env, me, image, width, height, grayscaleProcedure);
}

uint32_t grayscaleFunction(rgba* input) {
    return uint8_t((0.299 * input->getR())+ (0.587 * input->getG())+ (0.114 * input->getB()));
}

void *grayscaleProcedure(void *_threadJob) {

    jobElement* threadJob = (jobElement*)_threadJob;
    jint* srcRow = threadJob->src;
    jint* dstRow = threadJob->dst;
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