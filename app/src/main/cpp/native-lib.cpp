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

cv::Mat* openningImage;

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_gr_ImageProcessingActivity_detectCrosswalk(JNIEnv *env, jobject instance,
                                                 jlong addrGray) {
    double start, duration_ms;
    start = double(cv::getTickCount());
    openningImage  = (cv::Mat*)addrGray;
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