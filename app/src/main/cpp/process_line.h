#ifndef _EDLINES_H_
#define _EDLINES_H_

#include "EDLib.h"
#include "line_n_point.h"

extern cv::Mat* openningImage;
extern int minLength;
extern int minDiffColor;
extern int maxOffset;
extern float maxDegrees;

vector<shared_ptr<DtLine>> detectbyEDLines(string);
vector<shared_ptr<DtLine>> postProcessDtLines(vector<shared_ptr<DtLine>>);
vector<shared_ptr<DtLine>> connectLine(vector<shared_ptr<DtLine>>);

#endif
