#include "process_line.h"

vector<shared_ptr<DtLine>> detectbyEDLines(string imageName) {
    EDLines testEDLines = EDLines(*openningImage);
    vector<LS> lines = testEDLines.getLines();
    int noLines = testEDLines.getLinesNo();
    vector<shared_ptr<DtLine>> dtLines;
    for (int i = 0; i < noLines; i++) {
        auto A = make_shared<Point2D>(lines.at(i).start);
        auto B = make_shared<Point2D>(lines.at(i).end);
        if (A->distToPoint(B) <= minLength) continue;
        auto dtLine = make_shared<DtLine>(A, B, imageName);
        dtLine->calParallels();
        dtLine->calGroup();
        dtLines.push_back(dtLine);
    }
    return dtLines;
}

vector<shared_ptr<DtLine>> postProcessDtLines(vector<shared_ptr<DtLine>> dtLines) {
    vector<shared_ptr<DtLine>> postProcessedDtLines;
    for (const auto &dtLine : dtLines) {
        if (fabs(dtLine->getAngle()) < 45 && dtLine->getParallelLineDiffColor() >= minDiffColor) {
            postProcessedDtLines.push_back(dtLine);
        }
    }
    return postProcessedDtLines;
}

bool compareAngleAndOffset(shared_ptr<DtLine> dtLine1, shared_ptr<DtLine> dtLine2) {
    if (dtLine1->getGroup() == dtLine2->getGroup()) {
        if (abs((int) dtLine1->getDistToO() - (int) dtLine2->getDistToO()) < maxOffset) {
            if (fabs(dtLine1->getAngle() - dtLine2->getAngle()) < maxDegrees) {
                return dtLine1->getAngle() < dtLine2->getAngle();
            }
        }
        return ((int) dtLine1->getDistToO() < (int) dtLine2->getDistToO());
    }
    return dtLine1->getGroup() < dtLine2->getGroup();
}

vector<vector<shared_ptr<DtLine>>> splitByAngle(vector<shared_ptr<DtLine>> detectedLines) {
    vector<vector<shared_ptr<DtLine>>> output;
    shared_ptr<DtLine> dtLine;
    vector<shared_ptr<DtLine>> subVector = {detectedLines.front()};
    output.push_back(subVector);

    for (int i = 1; i < detectedLines.size(); i++) {
        dtLine = detectedLines.at(i);
        if ((fabs(dtLine->getAngle() - output.back().back()->getAngle()) > maxDegrees) ||
            ((int) (dtLine->getDistToO()) - (int) (output.back().back()->getDistToO()) >
             maxOffset) ||
            (dtLine->getGroup() != output.back().back()->getGroup())) {
            vector<shared_ptr<DtLine>> subVector = {dtLine};
            output.push_back(subVector);
            continue;
        }
        output.back().push_back(dtLine);
    }

    return output;
}

shared_ptr<DtLine> mergeLines(vector<shared_ptr<DtLine>> dtLines) {
    shared_ptr<DtLine> connectedLines = dtLines.front();
    shared_ptr<Point2D> minPoint = connectedLines->getA(),
            maxPoint = connectedLines->getA();

    int sumNumPixels1 = 0, sumNumPixels2 = 0, sumAvgColor1 = 0, sumAvgColor2 = 0;;
    for (const auto &dtLine : dtLines) {
        if (dtLine->getA()->getX() < minPoint->getX()) minPoint = dtLine->getA();
        else if (dtLine->getA()->getX() > maxPoint->getX()) maxPoint = dtLine->getA();

        if (dtLine->getB()->getX() < minPoint->getX()) minPoint = dtLine->getB();
        else if (dtLine->getB()->getX() > maxPoint->getX()) maxPoint = dtLine->getB();

        shared_ptr<ParallelLine> parallel1 = dtLine->getParallel1();
        sumAvgColor1 += parallel1->getEachPointColor().size() * parallel1->getAvgColor();
        sumNumPixels1 += parallel1->getEachPointColor().size();

        shared_ptr<ParallelLine> parallel2 = dtLine->getParallel2();
        sumAvgColor2 += parallel2->getEachPointColor().size() * parallel2->getAvgColor();
        sumNumPixels2 += parallel2->getEachPointColor().size();
    }

    connectedLines->setA(minPoint);
    connectedLines->setB(maxPoint);

    connectedLines->getParallel1()->setAvgColor(sumAvgColor1 / sumNumPixels1);
    connectedLines->getParallel2()->setAvgColor(sumAvgColor2 / sumNumPixels2);

    return connectedLines;
}

vector<shared_ptr<DtLine>> connectLine(vector<shared_ptr<DtLine>> detectedLines) {
    sort(detectedLines.begin(), detectedLines.end(), compareAngleAndOffset);

    vector<vector<shared_ptr<DtLine>>> splitted = splitByAngle(detectedLines);

    vector<shared_ptr<DtLine>> connectedLines;
    for (vector<shared_ptr<DtLine>> dtLines : splitted) {
        if (dtLines.size() == 1) connectedLines.push_back(dtLines.back());
        else connectedLines.push_back(mergeLines(dtLines));
    }
    return connectedLines;
}
