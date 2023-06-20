#include "bezier.h"

float beizerFilterAnalysis(int step, float *pos) {

#ifdef BZ_CUBIC_FILTER_USE
    float t = 0.0f;
    float result = 0.0f;

    if (step == 0) {
    	t = 0;
    } else if (step == BZ_MAX_REPORT_CNT) {
    	t = 1;
    } else {
    	t = step  * 0.33f;
    }

    result = (float)((((1 - t) * (1 - t) * (1 - t)) * pos[0]) + \
             (3 * ((1 - t) * (1 - t)) * t * pos[1]) + \
             (3 * (1 - t) * (t * t) * pos[2]) + \
             ((t * t * t) * pos[3]));
#endif
#ifdef BZ_QUADRATIC_FILTER_USE
    float t = 0.0f;
    float result = 0.0f;

    if (step == 0) {
    	t = 0;
    } else if (step == BZ_MAX_REPORT_CNT) {
    	t = 1;
    } else {
    	t = step  * 0.5f;
    }

    result = (float)((((1 - t) * (1 - t)) * pos[0]) + \
             (2 * (1 - t) * t * pos[1]) + \
             ((t * t) * pos[2]));
#endif

    return result;
}

float beizerCurvedFilterAnalysis(float prePos, float curPos) {
    float result = 0.0f;

    result = ((prePos - curPos) / 2) + curPos;

    return result;
}
