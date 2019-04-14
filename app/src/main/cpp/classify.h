#ifndef _CLASSIFY_H_
#define _CLASSIFY_H_

#include "process_line.h"

extern bool debugging;
extern float diffAngleInGroup;
extern int diffInColorSpace;
extern vector<int> score_threshold;

bool classify(vector<shared_ptr<DtLine>>);
shared_ptr<Point3D> updateColorCentroid(vector<shared_ptr<Rib>>);
void addScore(int *, int);
void subScore(int *, int);

#endif