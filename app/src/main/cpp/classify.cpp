#include "classify.h"

bool compareAngle(shared_ptr<DtLine> dtLine1, shared_ptr<DtLine> dtLine2) {
    return dtLine1->getAngle() < dtLine2->getAngle();
}

bool compareRibLength(shared_ptr<Rib> rib1, shared_ptr<Rib> rib2) {
    return rib1->getLength() > rib2->getLength();
}

bool compareVertebraY(shared_ptr<Rib> rib1, shared_ptr<Rib> rib2) {
    return rib1->getVertebra()->getY() < rib2->getVertebra()->getY();
}

vector<vector<shared_ptr<DtLine>>> splitToGroupAngle(vector<shared_ptr<DtLine>> detectedLines) {
    vector<vector<shared_ptr<DtLine>>> out;
    shared_ptr<DtLine> dtLine;
    vector<shared_ptr<DtLine>> subVector = {detectedLines.front()};
    out.push_back(subVector);

    for (int i = 1; i < detectedLines.size(); i++) {
        dtLine = detectedLines.at(i);
        if ((fabs(dtLine->getAngle() - out.back().back()->getAngle()) > diffAngleInGroup)) {
            vector<shared_ptr<DtLine>> subVector = {dtLine};
            out.push_back(subVector);
            continue;
        }
        out.back().push_back(dtLine);
    }
    return out;
}

float calweightedLength(shared_ptr<Rib> preRib, shared_ptr<Rib> currentRib) {
    return (preRib->getLength() + currentRib->getLength());
}

float calWeightedDiffColor(shared_ptr<Rib> preRib, shared_ptr<Rib> currentRib) {
    int diffMean;
    float weightedDiffColorPre, weightedDiffColorCurrent;

    int diffPre = abs(preRib->getParallelLineAvgColor2() - preRib->getParallelLineAvgColor1()) -
                  minDiffColor;
    int diffCurrent =
            abs(currentRib->getParallelLineAvgColor2() - currentRib->getParallelLineAvgColor1()) -
            minDiffColor;

    if (diffPre < 0 || diffCurrent < 0) {
        cout << "diffPre < 0 " << endl;
        cout << "diffCurrent < 0 " << endl;
    }

    if (diffPre <= 40) weightedDiffColorPre = (40 - diffPre) * 0.05; // max: (40 - 0) * 0.05 = 2
    else weightedDiffColorPre = 2;

    if (diffCurrent <= 40) weightedDiffColorCurrent = (40 - diffCurrent) * 0.05; // 2
    else weightedDiffColorCurrent = 2;

    return weightedDiffColorPre + weightedDiffColorCurrent;
}

float calWeightedColorInside(shared_ptr<Rib> preRib, shared_ptr<Rib> currentRib) {
    float weightedColorInside = 0;
    string str;
    if (preRib->getGroup()) str = "asphalt: ";
    else str = "crosswalk: ";

    int diffInside = abs(
            currentRib->getParallelLineAvgColor2() - preRib->getParallelLineAvgColor1());

    if (diffInside <= 22) {
        weightedColorInside = (22 - diffInside) * 0.15; // max: (22 - 0) * 0.15 = 3.3
    }
    return weightedColorInside;
}

float calWeightedColorOutside(shared_ptr<Rib> preRib, shared_ptr<Rib> currentRib) {
    float weightedColorOutside = 0;
    int diffOutside = abs(
            currentRib->getParallelLineAvgColor1() - preRib->getParallelLineAvgColor2());

    string str;
    if (preRib->getGroup()) str = "asphalt: ";
    else str = "crosswalk: ";

    if (diffOutside <= 30)
        weightedColorOutside = (30 - diffOutside) * 0.08; // max: (30 - 0) * 0.08 = 2.4

    return weightedColorOutside;
}

float calWeightedScale(shared_ptr<Rib> preRib, shared_ptr<Rib> currentRib, shared_ptr<Rib> nextRib) {
    string str1 = "asphalt/crosswalk: ", str2 = "crosswalk/asphalt: ";
    float weightedScale = 0,
            scale = currentRib->distToRib(nextRib) / currentRib->distToRib(preRib);
    if (!currentRib->getGroup()) // blue: crosswalk/asphalt
    {
        if (0.7 <= scale && scale <= 0.95)
            weightedScale = (scale - 0.7) * 20; // max: (0.95 - 0.7) * 20 = 5
        else if (0.95 < scale && scale <= 1.2) weightedScale = (1.2 - scale) * 20; // 5
    } else // asphalt/crosswalk
    {
        if (1.6 <= scale && scale <= 2)
            weightedScale = (scale - 1.6) * 12.5; // 5
        else if (2 < scale && scale <= 2.4) weightedScale = (2.4 - scale) * 12.5; // 5
    }
    return weightedScale;
}

int giveScore(vector<shared_ptr<Rib>> ribs) {
    float weightedLength, weightedScale, weightedDiffColor, weightedColorInside, weightedColorOutside;
    int score = 0;

    vector<shared_ptr<Rib>>::iterator preRibIterator, nextRibIterator;

    for (vector<shared_ptr<Rib>>::iterator currentRibIterator = ribs.begin() + 1;
         currentRibIterator != ribs.end(); currentRibIterator++) {
        preRibIterator = currentRibIterator - 1;

        weightedLength = calweightedLength(*preRibIterator, *currentRibIterator);

        weightedDiffColor = calWeightedDiffColor(*preRibIterator, *currentRibIterator);

        weightedColorInside = calWeightedColorInside(*preRibIterator, *currentRibIterator);

        weightedColorOutside = calWeightedColorOutside(*preRibIterator, *currentRibIterator);

        if ((currentRibIterator + 1) != ribs.end()) {
            nextRibIterator = currentRibIterator + 1;
            weightedScale = calWeightedScale(*preRibIterator, *currentRibIterator,
                                             *nextRibIterator);
        } else weightedScale = 1;

        addScore(&score, weightedLength * weightedScale *
                         (weightedDiffColor + weightedColorInside + weightedColorOutside));
    }

    return score;
}

int isSatisfyWidthConstraint(shared_ptr<Rib> preRib, shared_ptr<Rib> currentRib) {
    float minWidth, maxWidth;
    float dist = currentRib->distToRib(preRib);

    if (preRib->getGroup()) // red<<asphat<<blue
    {
        // y=7x/13-10
        minWidth = preRib->getVertebra()->getY() * 7 / 13 - 10;
        // y=4x/7+80
        maxWidth = preRib->getVertebra()->getY() * 4 / 7 + 80;
        if (minWidth <= 20) minWidth = 20;

        if (dist > maxWidth) return 0;
        if (dist < minWidth) return 0;
    } else // blue<<crosswalk<<red
    {
        // y=1x/2-12
        minWidth = preRib->getVertebra()->getY() / 2 - 12;
        // y=8x/15+60
        maxWidth = preRib->getVertebra()->getY() * 8 / 15 + 60;
        if (minWidth <= 10) minWidth = 10;

        if (dist > maxWidth) return 0;
        if (dist < minWidth) return 0;
    }

    return true;
}

vector<shared_ptr<Rib>> getCandicateRibs(vector<shared_ptr<Rib>> ribs) {
    shared_ptr<Rib> currentRib;
    int currentRibIndex = 1;

    sort(ribs.begin(), ribs.end(), compareVertebraY);

    vector<shared_ptr<Rib>> candicateRibs = {ribs.front()};

    for (vector<shared_ptr<Rib>>::iterator it1 = ribs.begin() + 1; it1 != ribs.end(); it1++) {
        currentRib = ribs.at(currentRibIndex);
        if ((*it1)->getGroup() == candicateRibs.back()->getGroup()) continue;

        if (isSatisfyWidthConstraint(candicateRibs.back(), *it1)) candicateRibs.push_back(*it1);
    }
    return candicateRibs;
}

vector< vector<shared_ptr<Rib>> > makeTripleRibs(vector<shared_ptr<Rib>> ribs) {
    vector<bool> v(ribs.size());
    vector< vector<shared_ptr<Rib>> > splitted;

    fill(v.begin(), v.begin() + 3, true);
    do {
        vector<shared_ptr<Rib>> subRibs;
        for (int i = 0; i < ribs.size(); ++i) {
            if (v[i]) {
                subRibs.push_back(ribs.at(i));
            }
        }
        splitted.push_back(subRibs);
    } while (prev_permutation(v.begin(), v.end()));
    return splitted;
}

bool isCrosswalk(shared_ptr<Backbone> backbone) {
    int threshold, score;
    vector<shared_ptr<Rib>> candicateRibs;
    vector<shared_ptr<Rib>> ribsSortedByLength = backbone->getRibs();
    sort(ribsSortedByLength.begin(), ribsSortedByLength.end(), compareRibLength);

    for (vector<shared_ptr<Rib>>::iterator it1 = ribsSortedByLength.begin();
         it1 != ribsSortedByLength.end(); it1++) {
        candicateRibs.clear();
        candicateRibs.push_back(*it1);
        auto colorCentroid = make_shared<Point3D>((*it1)->getParallelLineAvgColorHigh(),
                                             (*it1)->getParallelLineAvgColorLow(),
                                             3 * (*it1)->getParallelLineDiffColor());

        for (vector<shared_ptr<Rib>>::iterator it2 = it1 + 1; it2 != ribsSortedByLength.end(); it2++) {
            auto colorNew = make_shared<Point3D>((*it2)->getParallelLineAvgColorHigh(),
                                            (*it2)->getParallelLineAvgColorLow(),
                                            3 * (*it2)->getParallelLineDiffColor());

            if (colorCentroid->near(colorNew, diffInColorSpace)) {
                candicateRibs.push_back(*it2);
                colorCentroid = updateColorCentroid(candicateRibs);
            }
        }
        if (candicateRibs.size() < 3) continue;

        if (candicateRibs.size() > 3)
        {
            vector<vector<shared_ptr<Rib>>> tripleRibs = makeTripleRibs(candicateRibs);
            for (vector<shared_ptr<Rib>> tripleRib : tripleRibs)
            {
                tripleRib = getCandicateRibs(tripleRib);
                if (tripleRib.size() < 3) continue;

                score = giveScore(tripleRib);
                if (score >= score_threshold[3]) {
                    globalCandicateRibs = tripleRib;
                    return true;
                }
            }
        }
        else if (candicateRibs.size() == 3)
        {
            candicateRibs = getCandicateRibs(candicateRibs);
            if (candicateRibs.size() < 3) continue;

            score = giveScore(candicateRibs);
            if (score >= score_threshold[3]) {
                globalCandicateRibs = candicateRibs;
                return true;
            }
        }
    }
    return false;
}

bool classify(vector<shared_ptr<DtLine>> detectedLines) {
    sort(detectedLines.begin(), detectedLines.end(), compareAngle);

    vector<vector<shared_ptr<DtLine>>> splittedByAngle = splitToGroupAngle(detectedLines);

    for (vector<shared_ptr<DtLine>> dtLines : splittedByAngle) {
        if (dtLines.size() < 3) continue;
        if (isCrosswalk(make_shared<Backbone>(dtLines))) return true;
    }
    return false;
}

shared_ptr<Point3D> updateColorCentroid(vector<shared_ptr<Rib>> ribs) {
    float sumX = 0, sumY = 0, sumZ = 0;
    for (shared_ptr<Rib> rib : ribs) {
        sumX += rib->getParallelLineAvgColorHigh();
        sumY += rib->getParallelLineAvgColorLow();
        sumZ += 3 * rib->getParallelLineDiffColor();
    }
    return make_shared<Point3D>(sumX / ribs.size(), sumY / ribs.size(), sumZ / ribs.size());
}

void addScore(int *score, int amount) {
    *score += amount;
}

void subScore(int *score, int amount) {
    *score -= amount;
}