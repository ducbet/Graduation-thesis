//
// Created by tmd on 15/04/2019.
//

#include <jni.h>
#include "native-lib.h"

// post process params
int minLength = 30;
int minDiffColor = 15;
int maxOffset = 6;
float maxDegrees = 1.35;

// classify
int diffOffsetInGroup = 12;
float diffAngleInGroup = 4.5;
int diffInColorSpace = 55;
vector<int> score_threshold = {1000000, 1000000, 1000000, 4000, 15000, 30000, 35000}; // 6

int MAX_SLOPE = 10000;
int d = 5;
int powD = d * d;

int image_height = 300;
float roi_scale = 0.8;

cv::Mat *openningImage;
cv::Mat src;

double resizeRatio;
int originWidth;

void preprocess() {
    src = (*openningImage).clone();
    if (src.size().width > src.size().height) cv::rotate(src, src, cv::ROTATE_90_CLOCKWISE);

    originWidth = src.size().width;
    resizeRatio = ((double) src.size().height) / image_height;

    double ratio = (double) src.size().width / src.size().height;
    cv::Size target_size = cv::Size((int) (image_height * ratio), image_height);
    resize(src, src, target_size);

    // roi
    cv::Rect ROI = cv::Rect(0, 0, src.size().width, (int) (src.size().height * roi_scale));
//    src = src(ROI).clone();
    src = src(ROI);
    openningImage = &src;
//    if (kernel_size != 0) GaussianBlur( openningImage, openningImage, Size( kernel_size, kernel_size ), 0, 0 );
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_com_example_gr_ImageProcessingActivity_detectCrosswalk(JNIEnv *env, jobject instance,
                                                            jlong addrGray) {
    double start, duration_ms;
    int firstLineIndex = 3;
    start = double(cv::getTickCount());
    openningImage = (cv::Mat *) addrGray;
    LOGD("old size: width: %d, height: %d\n", (*openningImage).size().width,
         (*openningImage).size().height);
    preprocess();
    LOGD("width: %d, height: %d\n", (*openningImage).size().width, (*openningImage).size().height);
    vector<shared_ptr<DtLine>> detectedLines = detectbyEDLines("asdsd");
    LOGD("detectedLines.size(): %lu\n", detectedLines.size());
    int resultSize = (int) (detectedLines.size() * 4 + firstLineIndex);
    jintArray result = env->NewIntArray(resultSize);
    jint *fill = new jint[resultSize];
    // 0: resultSize
    // 1: first line index
    // 2: detect result
    // 3: firstLineIndex
    fill[0] = resultSize;
    fill[1] = firstLineIndex;
    fill[2] = 0;
    int i = firstLineIndex;
    for (const auto &dtLine : detectedLines) {
        fill[i] = (int) (dtLine->getA()->getY() * resizeRatio);
        fill[i + 1] = originWidth - (int) (dtLine->getA()->getX() * resizeRatio);
        fill[i + 2] = (int) (dtLine->getB()->getY() * resizeRatio);
        fill[i + 3] = originWidth - (int) (dtLine->getB()->getX() * resizeRatio);
        i += 4;
    }
    if (detectedLines.size() < 3) {
        env->SetIntArrayRegion(result, 0, resultSize, fill);
        free(fill);
        return result;
    }
    detectedLines = postProcessDtLines(detectedLines);
    if (detectedLines.size() < 3) {
        env->SetIntArrayRegion(result, 0, resultSize, fill);
        free(fill);
        return result;
    }
    int numLines;
    do {
        numLines = (int) detectedLines.size();
        detectedLines = connectLine(detectedLines);
        if (detectedLines.size() < 3 || detectedLines.size() == numLines) break;
    } while (true);
    if (detectedLines.size() < 3) {
        env->SetIntArrayRegion(result, 0, resultSize, fill);
        return result;
    }
    duration_ms = (double(cv::getTickCount()) - start) * 1000 / cv::getTickFrequency();
    LOGD("duration_ms: %f", duration_ms);
    bool b = classify(detectedLines);
    fill[2] = b;
    env->SetIntArrayRegion(result, 0, resultSize, fill);
    free(fill);
    //chua free result
    return result;
}