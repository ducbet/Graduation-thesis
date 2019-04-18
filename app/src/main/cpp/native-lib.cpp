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
int powD = d*d;

int image_height = 300;
float roi_scale = 0.8;

cv::Mat* openningImage;

void preprocess(){
    cv::Mat src = *openningImage;
    LOGD("old size: width: %d, height: %d\n", src.size().width, src.size().height);

    if (src.size().width > src.size().height) cv::rotate(src, src, cv::ROTATE_90_CLOCKWISE);

    if (image_height == -1) image_height = src.size().height; // giu nguyen kich thuoc
    double ratio = (double) src.size().width / src.size().height;
    cv::Size target_size = cv::Size((int)(image_height * ratio), image_height);
    resize(src, src, target_size);

    // roi
    cv::Rect ROI = cv::Rect(0, 0, src.size().width, src.size().height * roi_scale);
    src = src(ROI).clone();
//    src = src(ROI);
    openningImage = &src;
//    if (kernel_size != 0) GaussianBlur( openningImage, openningImage, Size( kernel_size, kernel_size ), 0, 0 );
    LOGD("new size: width: %d, height: %d\n", (*openningImage).size().width, (*openningImage).size().height);
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_gr_ImageProcessingActivity_detectCrosswalk(JNIEnv *env, jobject instance,
                                                 jlong addrGray) {
    double start, duration_ms;
    start = double(cv::getTickCount());
    openningImage  = (cv::Mat*)addrGray;
    preprocess();
    vector<shared_ptr<DtLine>> detectedLines = detectbyEDLines("asdsd");
    if(detectedLines.size() < 3) return 0;
    detectedLines = postProcessDtLines(detectedLines);
    if(detectedLines.size() < 3) return 0;
    int numLines;
    do
    {
        numLines = detectedLines.size();
        detectedLines = connectLine(detectedLines);
        if (detectedLines.size() < 3 || detectedLines.size() == numLines) break;
    } while(1);
    if(detectedLines.size() < 3) return 0;
    duration_ms = (double(cv::getTickCount()) - start) * 1000 / cv::getTickFrequency();
    LOGD("duration_ms: %f", duration_ms);
    return classify(detectedLines);
}