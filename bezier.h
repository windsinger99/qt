#ifndef BEZIER_H
#define BEZIER_H

#define BZ_FILTER_USE
#define BZ_CUBIC_FILTER		1
#if (BZ_CUBIC_FILTER == 1)
#define BZ_CUBIC_FILTER_USE
#define BZ_MAX_CNT          3
#define BZ_MAX_REPORT_CNT   3
#else
#define BZ_QUADRATIC_FILTER_USE
#define BZ_MAX_CNT          2
#define BZ_MAX_REPORT_CNT   2
#endif

typedef struct {
    int step;
    int cnt;
    int reportCnt;
    #ifdef BZ_FIX_FILTER
        float prePosX[BZ_MAX_CNT+1];
        float prePosY[BZ_MAX_CNT+1];
        float curPosX[BZ_MAX_CNT];
        float curPosY[BZ_MAX_CNT];
    #else
        float prePosX[BZ_MAX_CNT+1];
        float prePosY[BZ_MAX_CNT+1];
        float curPosX[BZ_MAX_CNT+1];
        float curPosY[BZ_MAX_CNT+1];
    #endif
} mBz;

float beizerFilterAnalysis(int step, float *pos);
float beizerCurvedFilterAnalysis(float prePos, float curPos);

#endif // BEZIER_H
