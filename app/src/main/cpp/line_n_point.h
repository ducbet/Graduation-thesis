#ifndef _POINT_AND_LINE_H_
#define _POINT_AND_LINE_H_

// accumulate
#include <numeric>
// __android_log_print
#include <android/log.h>
// pow,...
#include <cmath>
// setprecision
#include <iomanip>
#include <list>
#include <unordered_map>
// unique_ptr, shared_ptr
#include <memory>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

#define  LOG_TAG "MYTAG-native-lib"

#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

//template<typename T, typename... Args>
//std::unique_ptr<T> make_unique(Args&&... args)
//{
//    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
//}

using namespace std;

extern cv::Mat *openningImage;

extern int MAX_SLOPE;
extern int d;
extern int powD;
extern int diffOffsetInGroup;

class Point2D {
    cv::Point2d coor;
public:
    Point2D(cv::Point2d);

    Point2D(int, int);

    void setCoor(cv::Point2d);

    void print();

    int getX();

    int getY();

    float distToPoint(shared_ptr<Point2D>);

    bool equal(shared_ptr<Point2D>);
};

class Point3D : public Point2D {
    int z;
public:
    Point3D(int, int, int);

    void setZ(int);

    void print();

    float distToPoint(shared_ptr<Point3D>);

    bool near(shared_ptr<Point3D>, int);

    int getZ();
};

class Line {
    string imageName;
    shared_ptr<Point2D> A, B;
    float length, slope, angle, interceptY, distToO;

public:
    Line();

    Line(shared_ptr<Point2D>, shared_ptr<Point2D>, string);

    void setImageName(string);

    void setA(shared_ptr<Point2D>);

    void setB(shared_ptr<Point2D>);

    void setSlope(float slope);

    void calSlope();

    void calAngle();

    void calInterceptY();

    void calDistToO();

    void calLength();

    void setLength(float);

    void print();

    void getParallelPoints(shared_ptr<Point2D>, shared_ptr<Point2D>&, shared_ptr<Point2D>&, int);

    void getParallelPoints(shared_ptr<Point2D>, shared_ptr<Point2D>&, shared_ptr<Point2D>&);

    string getImageName();

    shared_ptr<Point2D> getA();

    shared_ptr<Point2D> getB();

    float getLength();

    float getSlope();

    float getAngle();

    float getInterceptY();

    float getDistToO();

    float distToPoint(shared_ptr<Point2D>);

    shared_ptr<Point2D> getIntersectionWith(shared_ptr<Line>);

    shared_ptr<Point2D> getMidpointt();
};

class DtLine;

class ParallelLine : public Line {
    shared_ptr<DtLine> baseLine;
    vector<uchar> eachPointColor;
    int avgColor;

    void calAvgColor();

public:
    ParallelLine(shared_ptr<DtLine>, shared_ptr<Point2D>, shared_ptr<Point2D>, string);

    void setBaseLine(shared_ptr<DtLine>);

    void setEachPointColorParallel();

    void setEachPointColorDiagonal();

    void setAvgColor(int);

    shared_ptr<DtLine> getBaseLine();

    int getAvgColor();

    vector<uchar> getEachPointColor();
};

typedef DtLine Rib;

class Backbone;

class DtLine : public Line, public enable_shared_from_this<DtLine> {
    shared_ptr<ParallelLine> parallel1, parallel2;
    bool group;
    shared_ptr<Backbone> backbone;
    shared_ptr<Point2D> vertebra;

public:
    DtLine(shared_ptr<Point2D>, shared_ptr<Point2D>, string);

    void print();

    void setVertebra(shared_ptr<Point2D>);

    float distToRib(shared_ptr<Rib>);

    void calParallels();

    void calGroup();

    shared_ptr<DtLine> getptr();

    shared_ptr<ParallelLine> getParallel1();

    shared_ptr<ParallelLine> getParallel2();

    shared_ptr<Point2D> getVertebra();

    int getParallelLineAvgColor1();

    int getParallelLineAvgColor2();

    int getParallelLineAvgColorHigh();

    int getParallelLineAvgColorLow();

    int getParallelLineDiffColor();

    bool getGroup();

    bool isGroup1();

    bool isGroup2();
};

class Backbone : public Line {
    vector<shared_ptr<Rib>> ribs;

public:
    Backbone(vector<shared_ptr<DtLine>>);

    void setRibs(vector<shared_ptr<DtLine>>);

    vector<shared_ptr<Rib>> getRibs();

    vector<shared_ptr<Point2D>> getVertebrae();
};

#endif