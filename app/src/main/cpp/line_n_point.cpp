#include "line_n_point.h"

// Point2D
Point2D::Point2D(cv::Point2d coor) {
    this->setCoor(coor);
}

Point2D::Point2D(int x, int y) {
    cv::Point2d coor(x, y);
    this->setCoor(coor);
}

void Point2D::setCoor(cv::Point2d coor) {
    this->coor = coor;
}

void Point2D::print() {
    LOGD("x: %3d; y: %3d\t", this->getX(), this->getY());
}

int Point2D::getX() {
    return (int)(this->coor.x);
}

int Point2D::getY() {
    return (int)(this->coor.y);
}

float Point2D::distToPoint(shared_ptr<Point2D> point) {
    return sqrt(pow(point->getX() - this->coor.x, 2) + pow(point->getY() - this->coor.y, 2));
}

bool Point2D::equal(shared_ptr<Point2D> point) {
    if (this->coor.x == point->getX() && this->coor.y == point->getY()) return true;
    return false;
}

// Point3D
Point3D::Point3D(int x, int y, int z) : Point2D(x, y) {
    this->setZ(z);
}

void Point3D::setZ(int z) {
    this->z = z;
}

void Point3D::print() {
//  printf("x: %3d; y: %3d; z: %3d\t", this->getX(), this->getY(), this->getZ());
}

float Point3D::distToPoint(shared_ptr<Point3D> point) {
    return sqrt(pow(point->getX() - this->getX(), 2) + pow(point->getY() - this->getY(), 2) +
                pow(point->getZ() - this->getZ(), 2));
}

bool Point3D::near(shared_ptr<Point3D> point, int dist) {
    if (this->distToPoint(point) < dist) return true;
    return false;
}

int Point3D::getZ() {
    return this->z;
}

// Line
Line::Line() {
}

Line::Line(shared_ptr<Point2D> A, shared_ptr<Point2D> B, string imageName) {
    this->setA(A);
    this->setB(B);
    this->setImageName(imageName);
}

void Line::setImageName(string imageName) {
    this->imageName = imageName;
}

void Line::setA(shared_ptr<Point2D> A) {
    this->A = A;
}

void Line::setB(shared_ptr<Point2D> B) {
    this->B = B;
    this->calLength();
    this->calSlope();
    this->calAngle();
    this->calInterceptY();
    this->calDistToO();
}

void Line::setSlope(float slope) {
    this->slope = slope;
}

void Line::calSlope() {
    if (this->A->getX() - this->B->getX() == 0) {
        this->slope = MAX_SLOPE;
        return;
    }
    this->slope = (float) (this->A->getY() - this->B->getY()) / (this->A->getX() - this->B->getX());
}

void Line::calAngle() {
    if (this->slope == MAX_SLOPE) {
        this->angle = 90;
        return;
    }
    this->angle = atan2(this->slope, 1) * (180.0 / 3.141592653589793238463);
}

void Line::calLength() {
    this->length = this->A->distToPoint(this->B);
}

void Line::calInterceptY() {
    this->interceptY = -(this->slope) * (this->A->getX()) + this->A->getY();
}

void Line::calDistToO() {
    this->distToO = this->distToPoint(make_shared<Point2D>(0, 0));
}

void Line::setLength(float length) {
    this->length = length;
}

void Line::print() {
    /*
    cout<<this->getImageName()<<"\t";
    cout<<"A:";
    if (this->getA() != NULL) this->getA()->print();
    else cout<<"point: NULL"<<"\t";

    cout<<"B:";
    if (this->getB() != NULL) this->getB()->print();
    else cout<<"point: NULL"<<"\t";

    cout<<"length:"<<setprecision(2)<<this->getLength()<<"\t";
    cout<<"width:"<<this->getWidth()<<"\t";
    cout<<"slope:"<<setprecision(3)<<this->getSlope()<<"\t";
    cout<<"angle:"<<this->getAngle()<<"\t";
    cout<<"interceptY:"<<setprecision(2)<<this->getInterceptY()<<"\t";
    cout<<"distToO:"<<setprecision(2)<<this->getDistToO()<<"\t";
    cout<<endl;
     */
}

void Line::getParallelPoints(shared_ptr<Point2D> point, shared_ptr<Point2D> &point1,
                             shared_ptr<Point2D> &point2) {
    this->getParallelPoints(point, point1, point2, d);
}

void Line::getParallelPoints(shared_ptr<Point2D> point, shared_ptr<Point2D> &point1,
                             shared_ptr<Point2D> &point2, int d) {
    if (fabs(this->getSlope()) <= 0.01) {
        point1 = make_shared<Point2D>(point->getX(), point->getY() + d);
        point2 = make_shared<Point2D>(point->getX(), point->getY() - d);
        return;
    } else if (this->getSlope() == MAX_SLOPE) {
        point1 = make_shared<Point2D>(point->getX() - d, point->getY());
        point2 = make_shared<Point2D>(point->getX() + d, point->getY());
        return;
    }
    float perpendicularSlope = -1 / (this->getSlope()),
            perpendicularInterceptY = point->getY() - perpendicularSlope * (point->getX());
    float a = 1 + pow(perpendicularSlope, 2),
            b = -2 * (point->getX()) +
                2 * perpendicularSlope * (perpendicularInterceptY - point->getY()),
            c = pow(point->getX(), 2) + pow(perpendicularInterceptY - point->getY(), 2) - powD;

    float discriminant = pow(b, 2) - 4 * a * c;
    float x1 = (-b - sqrt(discriminant)) / (2 * a),
            x2 = (-b + sqrt(discriminant)) / (2 * a);

    if (this->getSlope() > 0) {
        point1 = make_shared<Point2D>(x1, perpendicularSlope * x1 + perpendicularInterceptY);
        point2 = make_shared<Point2D>(x2, perpendicularSlope * x2 + perpendicularInterceptY);
    } else {
        point1 = make_shared<Point2D>(x2, perpendicularSlope * x2 + perpendicularInterceptY);
        point2 = make_shared<Point2D>(x1, perpendicularSlope * x1 + perpendicularInterceptY);
    }
}

string Line::getImageName() {
    return this->imageName;
}

shared_ptr<Point2D> Line::getA() {
    return this->A;
}

shared_ptr<Point2D> Line::getB() {
    return this->B;
}

float Line::getLength() {
    return this->length;
}

float Line::getSlope() {
    return this->slope;
}

float Line::getAngle() {
    return this->angle;
}

float Line::getInterceptY() {
    return this->interceptY;
}

float Line::getDistToO() {
    return this->distToO;
}

float Line::distToPoint(shared_ptr<Point2D> point) {
    int x0 = point->getX(),
            y0 = point->getY(),
            x1 = this->getA()->getX(),
            y1 = this->getA()->getY(),
            x2 = this->getB()->getX(),
            y2 = this->getB()->getY();
    return abs((y2 - y1) * x0 - (x2 - x1) * y0 + x2 * y1 - y2 * x1) / this->getLength();
}

shared_ptr<Point2D> Line::getIntersectionWith(shared_ptr<Line> line) {
    float a1 = this->getB()->getY() - this->getA()->getY(),
            b1 = this->getA()->getX() - this->getB()->getX(),
            c1 = a1 * (this->getA()->getX()) + b1 * (this->getA()->getY());

    float a2 = line->getB()->getY() - line->getA()->getY(),
            b2 = line->getA()->getX() - line->getB()->getX(),
            c2 = a2 * (line->getA()->getX()) + b2 * (line->getA()->getY());

    float determinant = a1 * b2 - a2 * b1;
    if (determinant == 0) {
        __android_log_print(ANDROID_LOG_INFO, "detect result",
                            "getIntersectionWith PARALLEL LINES");
        exit(1);
    } else {
        float x = (b2 * c1 - b1 * c2) / determinant,
                y = (a1 * c2 - a2 * c1) / determinant;
        return make_shared<Point2D>(x, y);
    }
}

shared_ptr<Point2D> Line::getMidpointt() {
    return make_shared<Point2D>(((this->getA()->getX() + this->getB()->getX()) / 2),
                                ((this->getA()->getY() + this->getB()->getY()) / 2));
}

//ParallelLine
ParallelLine::ParallelLine(shared_ptr<DtLine> baseLine, shared_ptr<Point2D> A,
                           shared_ptr<Point2D> B, string imageName) : Line(A, B, imageName) {
    this->setBaseLine(baseLine);
    this->setEachPointColorParallel();
}

void ParallelLine::setEachPointColorParallel() {
    cv::LineIterator it(*openningImage, cv::Point(this->getA()->getX(), this->getA()->getY()),
                        cv::Point(this->getB()->getX(), this->getB()->getY()), 8);
    for (int i = it.count * 0.1; i < it.count * 0.9; i++, ++it) {
        (this->eachPointColor).push_back((*openningImage).at<uchar>(it.pos()));
    }

    if (this->eachPointColor.empty()) {
        // cout<<"setEachPointColorParallel ERROR"<<endl;
        setEachPointColorDiagonal();
        return;
    }
    this->calAvgColor();
}

void ParallelLine::setEachPointColorDiagonal() {
    (this->eachPointColor).clear();
    shared_ptr<Point2D> midpointt = ((shared_ptr<Line>(this->getBaseLine()))->getMidpointt());

    cv::LineIterator it((*openningImage), cv::Point(this->getA()->getX(), this->getA()->getY()),
                        cv::Point(midpointt->getX(), midpointt->getY()), 8);
    cv::LineIterator it2((*openningImage), cv::Point(this->getB()->getX(), this->getB()->getY()),
                         cv::Point(midpointt->getX(), midpointt->getY()), 8);
    for (int i = 0; i < it.count; i++, ++it) {
        (this->eachPointColor).push_back((*openningImage).at<uchar>(it.pos()));
    }
    for (int i = 0; i < it2.count; i++, ++it2) {
        (this->eachPointColor).push_back((*openningImage).at<uchar>(it2.pos()));
    }

    if (this->eachPointColor.empty()) {
        __android_log_print(ANDROID_LOG_INFO, "detect result", "setEachPointColorDiagonal ERROR");
        exit(1);
    }
    this->calAvgColor();
}

void ParallelLine::calAvgColor() {
    this->setAvgColor(
            accumulate((this->eachPointColor).begin(), (this->eachPointColor).end(), 0.0) /
            (this->eachPointColor).size());
}

void ParallelLine::setAvgColor(int avgColor) {
    this->avgColor = avgColor;
}

void ParallelLine::setBaseLine(shared_ptr<DtLine> baseLine) {
    this->baseLine = baseLine;
}

shared_ptr<DtLine> ParallelLine::getBaseLine() {
    return this->baseLine;
}

int ParallelLine::getAvgColor() {
    return this->avgColor;
}

vector<uchar> ParallelLine::getEachPointColor() {
    return this->eachPointColor;
}

// DtLine
DtLine::DtLine(shared_ptr<Point2D> A, shared_ptr<Point2D> B, string imageName) : Line(A, B,
                                                                                      imageName) {
//    this->calParallels();
//    this->calGroup();
}

shared_ptr<DtLine> DtLine::getptr() {
    return shared_from_this();
}

void DtLine::calParallels() {
    shared_ptr<Point2D> point1, point2, point3, point4;
    Line::getParallelPoints(this->getA(), point1, point2);
    Line::getParallelPoints(this->getB(), point3, point4);
    shared_ptr<ParallelLine> line1 = make_shared<ParallelLine>(this->getptr(), point1, point3,
                                                               "parallel 1"),
            line2 = make_shared<ParallelLine>(this->getptr(), point1, point4, "parallel 1");
    if (fabs(line1->getAngle() - this->getAngle()) < fabs(line2->getAngle() - this->getAngle())) {
        this->parallel1 = line1;
        this->parallel2 = make_shared<ParallelLine>(this->getptr(), point2, point4, "parallel 2");
    } else {
        this->parallel1 = line2;
        this->parallel2 = make_shared<ParallelLine>(this->getptr(), point2, point3, "parallel 2");
    }
}

void DtLine::calGroup() {
    if (this->getParallelLineAvgColor1() > this->getParallelLineAvgColor2()) this->group = false;
    else this->group = true;
}

void DtLine::print() {
    /*
    cout<<"G: "<<this->getGroup()<<"  ";
    Line::print();
     */
}

void Rib::setVertebra(shared_ptr<Point2D> vertebra) {
    this->vertebra = vertebra;
}

float Rib::distToRib(shared_ptr<Rib> rib) {
    return this->vertebra->distToPoint(rib->getVertebra());
}

shared_ptr<ParallelLine> DtLine::getParallel1() {
    return this->parallel1;
}

shared_ptr<ParallelLine> DtLine::getParallel2() {
    return this->parallel2;
}

shared_ptr<Point2D> Rib::getVertebra() {
    return this->vertebra;
}

int DtLine::getParallelLineAvgColor1() {
    return this->getParallel1()->getAvgColor();
}

int DtLine::getParallelLineAvgColor2() {
    return this->getParallel2()->getAvgColor();
}

int DtLine::getParallelLineAvgColorHigh() {
    if (this->getParallelLineAvgColor1() > this->getParallelLineAvgColor2()) {
        return this->getParallelLineAvgColor1();
    }
    return this->getParallelLineAvgColor2();
}

int DtLine::getParallelLineAvgColorLow() {
    if (this->getParallelLineAvgColor1() < this->getParallelLineAvgColor2()) {
        return this->getParallelLineAvgColor1();
    }
    return this->getParallelLineAvgColor2();
}

int DtLine::getParallelLineDiffColor() {
    return abs(this->getParallelLineAvgColor1() - this->getParallelLineAvgColor2());;
}

bool DtLine::isGroup1() {
    return (this->group == true);
}

bool DtLine::isGroup2() {
    return (this->group == false);
}

bool DtLine::getGroup() {
    return this->group;
}

// Backbone
bool compareOffset(shared_ptr<DtLine> dtLine1, shared_ptr<DtLine> dtLine2) {
    return (int) dtLine1->getDistToO() < (int) dtLine2->getDistToO();
}

vector<vector<shared_ptr<DtLine>>> splitToGroupOffset(vector<shared_ptr<DtLine>> detectedLines) {
    vector<vector<shared_ptr<DtLine>>> splitted;
    shared_ptr<DtLine> dtLine;
    vector<shared_ptr<DtLine>> subVector = {detectedLines.front()};
    splitted.push_back(subVector);

    for (int i = 1; i < detectedLines.size(); i++) {
        dtLine = detectedLines.at(i);
        if ((dtLine->getGroup() != splitted.back().back()->getGroup()) ||
            (fabs(dtLine->getDistToO() - splitted.back().back()->getDistToO()) >
             diffOffsetInGroup)) {
            vector<shared_ptr<DtLine>> subVector = {dtLine};
            splitted.push_back(subVector);
            continue;
        }
        splitted.back().push_back(dtLine);
    }
    return splitted;
}

Backbone::Backbone(vector<shared_ptr<DtLine>> dtLines) : Line() {
    sort(dtLines.begin(), dtLines.end(), compareOffset);
    vector<vector<shared_ptr<DtLine>>> splitted = splitToGroupOffset(dtLines);
    float sumX = 0, sumY = 0, sumLength = 0, sumSlope = 0, sumLengthInGroup;
    int longestLineIndex;
    vector<shared_ptr<DtLine>> mergedDtLines;
    for (vector<shared_ptr<DtLine>> dtLines : splitted) {
        longestLineIndex = 0;
        sumLengthInGroup = 0;
        for (int i = 0; i < dtLines.size(); i++) {
            shared_ptr<DtLine> dtLine = dtLines.at(i);

            sumX += dtLine->getA()->getX() + dtLine->getB()->getX();
            sumY += dtLine->getA()->getY() + dtLine->getB()->getY();

            sumLength += dtLine->getLength();
            sumSlope += dtLine->getLength() * dtLine->getSlope();

            sumLengthInGroup += dtLine->getLength();

            if (dtLines.at(i)->getLength() >
                dtLines.at(longestLineIndex)->getLength())
                longestLineIndex = i;
        }

        if (dtLines.size() == 1) mergedDtLines.push_back(dtLines.front());
        else if (dtLines.size() > 1) {
            dtLines.at(longestLineIndex)->setLength(sumLengthInGroup);
            mergedDtLines.push_back(dtLines.at(longestLineIndex));
        }
    }
    float slope = sumSlope / sumLength;

    shared_ptr<Point2D> point1, point2;
    this->setSlope(slope);
    Line::getParallelPoints(
            make_shared<Point2D>(sumX / (2 * dtLines.size()), sumY / (2 * dtLines.size())),
            point1, point2, 30);
    Line::setA(point1);
    Line::setB(point2);
    this->setRibs(mergedDtLines);
}

void Backbone::setRibs(vector<shared_ptr<DtLine>> dtLines) {
    this->ribs = dtLines;
    for (shared_ptr<Rib> rib : this->ribs) rib->setVertebra(this->getIntersectionWith(rib));
}

vector<shared_ptr<Rib>> Backbone::getRibs() {
    return this->ribs;
}

vector<shared_ptr<Point2D>> Backbone::getVertebrae() {
    vector<shared_ptr<Point2D>> vertebrae;
    for (shared_ptr<Rib> rib : this->ribs) vertebrae.push_back(rib->getVertebra());
    return vertebrae;
}
