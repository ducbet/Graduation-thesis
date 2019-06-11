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

vector<shared_ptr<Rib>> globalCandicateRibs;
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

int eval_ms_count = 0;
double avg_ms_preprocess = 0;
double avg_ms_detectbyEDLines = 0;
double avg_ms_postProcessDtLines = 0;
double avg_ms_classify = 0;
double avg_ms_all = 0;
double max_ms_preprocess = 0, max_ms_detectbyEDLines = 0, max_ms_postProcessDtLines = 0, max_ms_classify = 0, max_ms_all = 0;

jintArray detectCrosswalk(JNIEnv *env, jobject instance, jlong addrGray) {
    double start_all, duration_ms_all;
    int firstLineIndex = 3;
    start_all = double(cv::getTickCount());
    openningImage = (cv::Mat *) addrGray;
    LOGD("old size: width: %d, height: %d\n", (*openningImage).size().width,
         (*openningImage).size().height);

    eval_ms_count++;
    double start_preprocess = double(cv::getTickCount());
    preprocess();
    double duration_ms_preprocess = (double(cv::getTickCount()) - start_preprocess) * 1000 / cv::getTickFrequency();
    avg_ms_preprocess += duration_ms_preprocess;
    LOGD("eval_ms_count: %d", eval_ms_count);
    if (max_ms_preprocess < duration_ms_preprocess) max_ms_preprocess = duration_ms_preprocess;
    LOGD("max_ms_preprocess: %f", max_ms_preprocess);
    LOGD("avg_ms_preprocess: %f", avg_ms_preprocess / eval_ms_count);

    LOGD("width: %d, height: %d\n", (*openningImage).size().width, (*openningImage).size().height);
    double start_detectbyEDLines = double(cv::getTickCount());
    vector<shared_ptr<DtLine>> detectedLines = detectbyEDLines("asdsd");
    double duration_ms_detectbyEDLines = (double(cv::getTickCount()) - start_detectbyEDLines) * 1000 / cv::getTickFrequency();
    avg_ms_detectbyEDLines += duration_ms_detectbyEDLines;
    if (max_ms_detectbyEDLines < duration_ms_detectbyEDLines) max_ms_detectbyEDLines = duration_ms_detectbyEDLines;
    LOGD("max_ms_detectbyEDLines: %f", max_ms_detectbyEDLines);
    LOGD("avg_ms_detectbyEDLines: %f", avg_ms_detectbyEDLines / eval_ms_count);

    LOGD("detectedLines.size(): %lu\n", detectedLines.size());
    int resultSize = (int) (detectedLines.size() * 4 + firstLineIndex);
    jintArray result = env->NewIntArray(resultSize + 3 * 4);// chua cho cho 3 valid line
    jint *fill = new jint[resultSize + 3 * 4];
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
    double start_postProcessDtLines = double(cv::getTickCount());
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
    double duration_ms_postProcessDtLines = (double(cv::getTickCount()) - start_postProcessDtLines) * 1000 / cv::getTickFrequency();
    avg_ms_postProcessDtLines += duration_ms_postProcessDtLines;
    if (max_ms_postProcessDtLines < duration_ms_postProcessDtLines) max_ms_postProcessDtLines = duration_ms_postProcessDtLines;
    LOGD("max_ms_postProcessDtLines: %f", max_ms_postProcessDtLines);
    LOGD("avg_ms_postProcessDtLines: %f", avg_ms_postProcessDtLines / eval_ms_count);

    double start_classify = double(cv::getTickCount());
    bool detectResult = classify(detectedLines);
    double duration_ms_classify = (double(cv::getTickCount()) - start_classify) * 1000 / cv::getTickFrequency();
    avg_ms_classify += duration_ms_classify;
    if (max_ms_classify < duration_ms_classify) max_ms_classify = duration_ms_classify;
    LOGD("max_ms_classify: %f", max_ms_classify);
    LOGD("avg_ms_classify: %f", avg_ms_classify / eval_ms_count);

    duration_ms_all = (double(cv::getTickCount()) - start_all) * 1000 / cv::getTickFrequency();
    avg_ms_all += duration_ms_all;
    if (max_ms_all < duration_ms_all) max_ms_all = duration_ms_all;
    LOGD("max_ms_all: %f", max_ms_all);
    LOGD("avg_ms_all all: %f", avg_ms_all / eval_ms_count);
    fill[2] = detectResult;
    if (detectResult) {
        i = resultSize; // first valid line index == resultSize (old)
        resultSize += 3 * 4;
        fill[0] = resultSize;// 3 line valid * 4 point per line
        for (const auto &dtLine : globalCandicateRibs) {
            fill[i] = (int) (dtLine->getA()->getY() * resizeRatio);
            fill[i + 1] = originWidth - (int) (dtLine->getA()->getX() * resizeRatio);
            fill[i + 2] = (int) (dtLine->getB()->getY() * resizeRatio);
            fill[i + 3] = originWidth - (int) (dtLine->getB()->getX() * resizeRatio);
            i += 4;
        }
    }
    env->SetIntArrayRegion(result, 0, resultSize, fill);
    free(fill);
    //chua free result
    return result;
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_com_example_gr_DemoCameraActivity_detectCrosswalk(JNIEnv *env, jobject instance,
                                                       jlong addrGray) {
    return detectCrosswalk(env, instance, addrGray);
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_com_example_gr_DemoVideoActivity_detectCrosswalk(JNIEnv *env, jobject instance,
                                                      jlong addrGray) {
    return detectCrosswalk(env, instance, addrGray);
}