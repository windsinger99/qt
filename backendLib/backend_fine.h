/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */
#ifndef _BACKEND_FINE_H    /* Guard against multiple inclusion */
#define _BACKEND_FINE_H
#include "backend_common.h"
#include "backend_utils.h"

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

#define FINE_CLIPPING_NEW  //nsmoon@200121
#define FINE_CAL_MIN_MAX

#define FINE_MEM_ERROR			-1
#define FINE_ERROR              1
#define FINE_OK                 0

#define FINE_GROUP_FOUND		1
#define FINE_NO_GROUP			0
#define FINE_OUT_OF_INST		2
#define FINE_INST_OK			0

///////////////////////////////////////////////////////////////////////
// Tunning Parameters
///////////////////////////////////////////////////////////////////////
#if 1 //def FINE_TEST_RULE_1
#define MAX_FINE_INITIAL_CXP_PER_LINE   MAX_INITIAL_LINE_FINE_Y //max initial cross point number //nsmoon@211028 MAX_TOUCH_LIMIT_FINE=>MAX_INITIAL_LINE_FINE_Y
#if defined(GN65_TEST) //nsmoon@221116 for GN65
#define FINE_CAL_WIDTH_INIT      1.0f
#define FINE_GRID_WIDTH_INIT     8 //6 //5*CAL_WIDTH
#elif defined(N75_TEST) || defined(N65_TEST) //nsmoon@230411
#define FINE_CAL_WIDTH_INIT      0.6f //0.8f
#define FINE_GRID_WIDTH_INIT     8    //6
#else
#define FINE_CAL_WIDTH_INIT      0.6f
#define FINE_GRID_WIDTH_INIT     8
#endif
#define FINE_CAL_WIDTH_NORM      FINE_CAL_WIDTH_INIT
#define FINE_GRID_WIDTH_NORM     FINE_GRID_WIDTH_INIT

#if 0 //for test //nsmoon@230131
#define REMAINED_FINE_INITIAL_RANGE_X   (FINE_CAL_WIDTH_INIT*FINE_GRID_WIDTH_INIT) //3.6f //10.0f //10.0*2=20.0=1.0*20
#define REMAINED_FINE_INITIAL_RANGE_Y   REMAINED_FINE_INITIAL_RANGE_X //3.6f //10.0f
#define REMAINED_FINE_NORMAL_RANGE_X    (REMAINED_FINE_INITIAL_RANGE_X*0.5f) //3.6f //3.6*2=7.2=0.6*12
#define REMAINED_FINE_NORMAL_RANGE_Y    REMAINED_FINE_NORMAL_RANGE_X //3.6f
#endif
#if 1
#define REMAINED_FINE_INITIAL_RANGE_X   3.6f //10.0f //10.0*2=20.0=1.0*20
#define REMAINED_FINE_INITIAL_RANGE_Y   3.6f //10.0f
#define REMAINED_FINE_NORMAL_RANGE_X    3.6f    //3.6*2=7.2=0.6*12
#define REMAINED_FINE_NORMAL_RANGE_Y    3.6f
#endif

//#define FINE_MIN_CAL_SIZE 1.0f //nsmoon@210305 //nsmoon@211022 not-used

#define FINE_MAX_CNT_INIT        5
#define FINE_MAX_CNT_INIT_EDGE   3

#ifdef GN65_TEST //nsmoon@221206 for GN65
#define FINE_MIN_NUM_SLOT        2
#else
#define FINE_MIN_NUM_SLOT        3
#endif
#if defined(N75_TEST) || defined(N65_TEST)  //nsmoon@230510 1=>2
#define FINE_MIN_NUM_SLOT_EDGE   2
#else
#define FINE_MIN_NUM_SLOT_EDGE   1
#endif
#define FINE_MAX_DIFF_CNT        2
#define FINE_MAX_DIFF_EDGE_CNT   5
#define FINE_EDGE_DIST_FROM_AAREA       90.0f //50.0f //25.0f //30.0f  //nsmoon@200803 25=>50
#define FINE_EDGE_DIST_FROM_AAREA_INIT  90.0f //50.0f //nsmooon@200205
#define FINE_EDGE_DIST_FROM_TP          90.0f //50.0f //nsmooon@211019 50.0f=>90.0f
#define FINE_EDGE_PD_DIST_FROM_AAREA    40.0f //40mm

#define FINE_MIN_REMAINED_LINE_X2        4 //8 //nsmoon@200326 4=>8
#define FINE_MIN_REMAINED_LINE_Y2        6 //12 //nsmoon@200326 6=>12
#endif //1

#ifdef FINE_CLIPPING_NEW
#if defined(GN65_TEST) || defined(N75_TEST) || defined(N65_TEST) //nsmoon@230328o
#define FINE_INITIAL_GRP_DIST_SHADOW             1.5f
#define FINE_INITIAL_GRP_DIST_SHADOW_OPP         2.5f
#else
#define FINE_INITIAL_GRP_DIST_SHADOW    1.0f //5.0f //nsmoon@211022 //nsmoon@211028 5.0f=>1.0f
#define FINE_INITIAL_GRP_DIST_SHADOW_OPP 2.0f
#endif
#define FINE_INITIAL_GRP_DIST_SHADOW_ERASER      3.0f
#define FINE_INITIAL_GRP_DIST_SHADOW_OPP_ERASER  4.0f
#define FINE_INITIAL_GRP_DIST_BRUSH     3.0f
#define FINE_INITIAL_GRP_DIST_X         1.2f //1.3f //1.5f
#define FINE_INITIAL_GRP_DIST_Y         0.8f //1.0f //1.3f
#if defined(GN65_TEST) //nsmoon@221122
#define FINE_INITIAL_GRP_DIST_X_OLD     2.3f //1.5f
#define FINE_INITIAL_GRP_DIST_Y_OLD     1.5f //1.3f
#elif defined(N75_TEST) || defined(N65_TEST)//nsmoon@230322
#define FINE_INITIAL_GRP_DIST_X_OLD     1.5f //2.3f
#define FINE_INITIAL_GRP_DIST_Y_OLD     1.6f
#else
#define FINE_INITIAL_GRP_DIST_X_OLD     1.5f
#define FINE_INITIAL_GRP_DIST_Y_OLD     1.3f
#endif
#define FINE_INITIAL_GRP_DIST_X_APRX    3.0f
#define FINE_INITIAL_GRP_DIST_Y_APRX    2.0f
#define FINE_INITIAL_GRP_DIST_XX        3.0f
#define FINE_INITIAL_GRP_DIST_YY        2.5f
#define FINE_INITIAL_GRP_DENS_X         1.0f
#define FINE_INITIAL_GRP_DENS_Y         2.0f
#define FINE_INITIAL_GRP_EDGE_AREA		40.0f
#define FINE_INITIAL_GRP_MAX			3
#define FINE_INST_MARGIN                10.0f
#if 1 //nsmoon@200803
#if 0 //(MODEL_SPT == MODEL_CTSK_850_V100) //nsmoon@200910
#define FINE_GHOST_SAME_AXIS            5.0f
#define FINE_GHOST_NEAR_DIST            15.0f
#define FINE_GHOST_NEAR_DIST_EDGE       40.0f
#else
#define FINE_GHOST_SAME_AXIS            5.0f //10.0f //5.0f
#define FINE_GHOST_NEAR_DIST            15.0f
#if defined(N75_TEST) || defined(N65_TEST) //nsmoon@230419
#define FINE_GHOST_NEAR_DIST_SAME_AXIS  150.0f //n65_4p_n65_4p_230412b.dlt #32 //40*3=120
#else
#define FINE_GHOST_NEAR_DIST_SAME_AXIS  20.0f //30.0f //nsmoon@211021 //nsmoon@211029 30.0f=>20.0f
#endif
#define FINE_GHOST_NEAR_DIST_Y          20.0f
#define FINE_GHOST_NEAR_DIST_EDGE_X     20.0f //25.0f //40.0f //nsmoon@211029 25.0f=>20.0f
//#define FINE_GHOST_NEAR_DIST_EDGE_Y     900.0f //170.0f //150.0f //40.0f
#endif
#else
#define FINE_GHOST_NEAR_DISTSQ          225.0f //15*15 //nsmoon@200803
//#define FINE_GHOST_NEAR_DISTSQ_EDGE     500.0f //22.4f*22.4f, //nsmoon@200406
#define FINE_GHOST_NEAR_DISTSQ_EDGE     1600.0f //40.0f*40.0f, //nsmoon@200513
#endif
#define FINE_GHOST_MIN_GRID_WIDTH       2
#else
#define FINE_INITIAL_GRP_DIST_X         3.0f //3.0f //2.0f //nsmoon@190528 1.0=>1.5
#define FINE_INITIAL_GRP_DIST_Y         2.0f //3.0f //1.5f //f2p-0604d_4.dlt#500&#502#543
#endif
#define FINE_INITIAL_GRP_DIST_MERGED    3.0f //FIXME
#define FINE_INITIAL_GRP_MIN_DENS       10

#if (MODEL_TOUCH_SIZE==MODEL_TOUCH_SIZE_85)||(MODEL_TOUCH_SIZE==MODEL_TOUCH_SIZE_75)||(MODEL_TOUCH_SIZE==MODEL_TOUCH_SIZE_65)||(MODEL_TOUCH_SIZE==MODEL_TOUCH_SIZE_55) //nsmoon@211116
#ifdef FINE_CLIPPING_NEW
#if 0 //def GN65_TEST //nsmoon@221116 not-used
#define FINE_INITIAL_GRP_MIN_NUM        3
#else
#define FINE_INITIAL_GRP_MIN_NUM        5
#endif
#define FINE_INITIAL_GRP_MIN_NUM_SHADOW 3 //nsmoon@211027
#define FINE_INITIAL_GRP_MIN_NUM_EDGE   3 //FIXME
#define FINE_INITIAL_GRP_MIN_NUM_BRUSH  3
#else
#define FINE_INITIAL_GRP_MIN_NUM        3
#define FINE_INITIAL_GRP_MIN_NUM_EDGE   2 //FIXME
#endif
#define FINE_INITIAL_GRP_THRESHOLD		30
//#elif (MODEL_TOUCH_SIZE	== MODEL_TOUCH_SIZE_85)
//#define FINE_INITIAL_GRP_MIN_NUM        5
//#define FINE_INITIAL_GRP_MIN_NUM_EDGE   3 //FIXME
//#define FINE_INITIAL_GRP_MIN_NUM_BRUSH  3
//#define FINE_INITIAL_GRP_THRESHOLD		30
#elif (MODEL_SPT == MODEL_IRT_550)
#define FINE_INITIAL_GRP_MIN_NUM        3
#define FINE_INITIAL_GRP_MIN_NUM_EDGE   2 //FIXME
#define FINE_INITIAL_GRP_THRESHOLD		70 //50 //30
#elif (MODEL_SPT == MODEL_SPT_550)
#define FINE_INITIAL_GRP_MIN_NUM        4 //5
#define FINE_INITIAL_GRP_MIN_NUM_EDGE   4 //2
#define FINE_INITIAL_GRP_THRESHOLD		20
#else
#error "MODEL_TOUCH_SIZE is not defined!"
#endif

#define FINE_INITIAL_GRP_NUM_CHKTH      10
//#define FINE_INITIAL_GRP_TH50_RATIO		3 //2:50%, 3:33%
//#define FINE_INITIAL_GRP_MIN_NUM_EDGE	2 //2 is 3
//#define FINE_INITIAL_GRP_EDGE_POS_50   	50.0f
//#define FINE_INITIAL_GRP_EDGE_POS_120  	120.0f
//#define FINE_INITIAL_SAME_LINE_DIST		0.1f //0.1f //0.5f
#define FINE_BELONGED_LINE_SLOPE_DIFF	5
//#define FINE_INITIAL_LINE_DIST          30.0f
//#define FINE_MERGE_GRP_DIST				25.0f //20.0f=>15.0f debug@190419-2, 15.0f=>20.0f nsmoon@190604
//#define FINE_MERGE_GRP_DIST_X			25.0fz
//#define FINE_MERGE_GRP_DIST_STATE4		5.0f
//#define FINE_INITIAL_INST_X_RNG         50.0f //20.0f=>50.0f nsmoon@190419-1
#define FINE_MERGE_LINE_Y_DIST          3.0f //nsmoon@190618
#define FINE_CAL_MIN_MAX_BLACK_AREA     10.0f
//#define FINE_MIN_TOUCH_AREA           0.8f
#define FINE_MIN_TOUCH_WIDTH            0.8f
#ifdef FINE_DATA_CHECK //debug@190420-1
#define FINE_DATA_CHECK_DIST			(60.0f * 60.0f)
#define FINE_DATA_CHECK_AAREA_DIST		50.0f //25.0f
#endif

#if (MODEL_SPT == MODEL_SPT_550)
#define FINE_SHARED_LINE_REMOVE_AREA	50.0f //80.0f //80.0f
#else //(MODEL_TOUCH_SIZE == MODEL_TOUCH_SIZE_75) ||(MODEL_TOUCH_SIZE == MODEL_TOUCH_SIZE_85)
#define FINE_SHARED_LINE_REMOVE_AREA	80.0f
#endif

//#define FINE_SHARED_LINE_AREA			(FINE_SHARED_LINE_REMOVE_AREA)
#define FINE_SIZE_EDGE_AREA_X			100.0f
#define FINE_SIZE_EDGE_AREA_X2			30.0f
//#define FINE_SHARED_LINE_AREA_CORER		30.0f //50.0f
#define FINE_SHARED_LINE_SLOPE_X_VAL    20
#define FINE_SHARED_LINE_SLOPE_Y_VAL    20
#define FINE_INST_X_MARGIN              5.0f //10.0f //6.0f=>4.0f=>6.0f=>10.0f //nsmoon@190731
#define FINE_INST_Y_DISPLAY             20.0f
#define FINE_REMOVE_GRP_MAHT_DIST       80.0f //30.0f
//#define FINE_REMOVE_Y_DIST              80.0f //25.0f
//#define FINE_REMOVE_Y_DIST_CORNER       10.0f  //nsmoon@190613
//#define FINE_REMOVE_Y_DIST_EDGE         1.5f  //nsmoon@190613
#define FINE_REMOVE_TOUCH_POS           15.0f //10.0f //3.0f, nsmoon@190809
#define FINE_BELONG_MIN_SLOPE_DIFF      15
#define FINE_MIN_NEAR_LINE_SLOPE_VAL    5 //nsmoon@190617a
#define FINE_CENT2POS_X_MARGIN          40.0f //40mm, nsmoon@190808
#define FINE_MAX_NEAR_LINE_X_DIST       FINE_CENT2POS_X_MARGIN //20.0f //10 //nsmoon@190617a
#define FINE_MAX_NEAR_LINE_Y_DIST       3.0f //nsmoon@190617a
#define FINE_MAX_NEAR_LINE_X_DIST_NEAR  FINE_REMOVE_GRP_MAHT_DIST //nsmoon@190807
#define FINE_MAX_NEAR_LINE_DIST         6.0f //3.0f //nsmoon@190917a

#define FINE_MAX_OVERLAP_TP_MINMAX_GAP  0.5f
#if defined(N75_TEST) || defined(N65_TEST) //nsmoon@240426
#define FINE_MAX_CLOSED_TP_MINMAX_GAP   15.0f //min-dist for 2-touch
#define FINE_MAX_CLOSED_TOUCH_INFO_GAP  1.0f //nsmoon@230531 3.6f=>1.0f
#else
#define FINE_MAX_CLOSED_TP_MINMAX_GAP   10.0f //12.0f //15.0f, min/max, //nsmoon@0325 15=>12 //nsmoon@211028 12=>10
#endif
#if 1 //nsmoon@211102
#define FINE_MAX_CLOSED_TP_1P_SPEED_GAP       25.0f
#define FINE_MAX_CLOSED_TP_1P_SPEED_OPP_GAP   4.0f
#endif
#define FINE_MAX_CLOSED_TP_CENT_GAP     20.0f
#define FINE_MAX_BRUSH_NEAR_TP_GAP		100.0f
#if 1 //nsmoon@211014
#define FINE_MAX_BETWEEN_TP_MINMAX_GAP_X  50.0f
#define FINE_MAX_BETWEEN_TP_MINMAX_GAP_Y  20.0f
#define FINE_MAX_BETWEEN_TP_MINMAX_GAP_Y2 80.0f //nsmoon@211018
#endif

#define FINE_MIN_SLOPE_CNT_GRP          1 //2 //3  //nsmoon@200318
#define FINE_MIN_SLOPE_VAL              10 //5 //nsmoon@211022 //nsmoon@211028 5=>10

#define FINE_NOT_ALLOC_LINE             2 //5 //nsmoon@190619a, FIXME
#define FINE_XGRP_LEN_RATIO             4.0f //nsmoon@190808
///////////////////////////////////////////////////////////////////////

#if 1 //nsmoon@230410
#define MAX_INITIAL_LINE_FINE_X		((MAX_TOUCH_LIMIT_FINE+1)) //(MAX_TOUCH_LIMIT_FINE) //0~255
#define MAX_INITIAL_LINE_FINE_Y		((MAX_TOUCH_LIMIT_FINE+1) * (MAX_TOUCH_LIMIT_FINE+1) /* *2 */) //0~255, must check range!! //nsmoon@211119 *2 //nsmoon@220207 *2 not-used
#define MAX_INITIAL_LINE_FINE_XY    MAX_INITIAL_LINE_FINE_Y //nsmoon@230412
#define MAX_INITIAL_LINE_FINE_X2	MAX_INITIAL_LINE_FINE_Y //((MAX_TOUCH_LIMIT_FINE+1) * (MAX_TOUCH_LIMIT_FINE+1))
#else
#define MAX_INITIAL_LINE_FINE_X		(MAX_TOUCH_LIMIT_FINE) //(MAX_TOUCH_LIMIT_FINE) //0~255
#define MAX_INITIAL_LINE_FINE_Y		(MAX_TOUCH_LIMIT_FINE * MAX_TOUCH_LIMIT_FINE /* *2 */) //0~255, must check range!! //nsmoon@211119 *2 //nsmoon@220207 *2 not-used
#define MAX_INITIAL_LINE_FINE_X2	(MAX_TOUCH_LIMIT_FINE * MAX_TOUCH_LIMIT_FINE)
#endif
#define MAX_INITIAL_CXP_FINE        (MAX_INITIAL_LINE_FINE_X * MAX_INITIAL_LINE_FINE_Y)

#define FINE_INITIAL_SLOPE_MAX_VAL	(MAX_Y_SLOPE_VAL)
#define FINE_INITIAL_SLOPE_MAX_IDX	(MAX_Y_SLOPE_IDX)
#define NUM_INIT_MAX_Y_GRP          (MAX_TOUCH_LIMIT_FINE / 2)

#define MAX_NUM_INST_FINE           ((MAX_TOUCH_LIMIT_FINE*2) * MAX_INST_SORT_TBL_SIZE) //if change size, shoud check sort tbl //nsmoon@211125 *2
#define MAX_INITIAL_LINE_GRP        (MAX_TOUCH_LIMIT_FINE * 20) //(MAX_TOUCH_LIMIT_FINE * 6), FIXME
#define FINE_MAX_GRP_CNT		    10 //7 //MAX_TOUCH_LIMIT_FINE
#ifndef USE_CUST_MALLOC //nsmon@201012
#define MAX_FINE_REM_INIT_LINE		FINE_INITIAL_SLOPE_MAX_IDX //30 //nsmoon@201012
#endif

#define MAX_Y_LINE_FOR_FINE			100 //FIXME, nsmoon@190710
#define MAX_X_LINE_FOR_FINE			63	//100*40/63 //not-detect one finger

//mode of fine_get_inst2() for getting intersection
#define FINE_USE_CENTER_LINE        0
#define FINE_USE_INITIAL_LINE       1

#if 0 //for test
#define FINE_GRPSTAT_REMOVED_MASK	0x0F //removed mask
#define FINE_GRPSTAT_SKIP_MASK		0xF0 //info mask
#define FINE_GRPSTAT_INST			0x00 //set
#define FINE_GRPSTAT_NOINST			0x01 //init
#define FINE_GRPSTAT_GHOST			0x02
#define FINE_GRPSTAT_SMALL			0x04 //small len
#define FINE_GRPSTAT_NOOPP			0x08 //no opp axis
#define FINE_GRPSTAT_MISMATCH		0x10
#endif

#define FINE_CXP_ORI        0
#define FINE_CXP_CUR        1
#define FINE_CXP_CAL_INIT   0
#define FINE_CXP_CAL_NORM   1

#if 1 //nsmoon@201117
#define FINE_LOOP_FINE5         0
#define FINE_LOOP_XY_ST         (FINE_LOOP_FINE5+1) //1
#if defined(N75_TEST) || defined(N65_TEST) //nsmoon@230327 1=>3
#define FINE_LOOP_XY_ED         (FINE_LOOP_XY_ST+3) //TTR4_230321.dlt  #108/138
#else
#define FINE_LOOP_XY_ED         (FINE_LOOP_XY_ST+1) //2
#endif
#define FINE_LOOP_SHADOW_ST     (FINE_LOOP_XY_ED+1) //4
#define FINE_LOOP_SHADOW_ED     (FINE_LOOP_SHADOW_ST+2) //6
#define FINE_LOOP_EDGE_X        (FINE_LOOP_SHADOW_ED+1) //7
#define FINE_LOOP_BRUSH         (FINE_LOOP_EDGE_X+1)    //8
#endif

typedef struct {
    uint8_t len;
    uint8_t *instIdx;
} merged_group_t;

typedef struct {
    uint8_t len;
    uint8_t stat;
    uint8_t cxLineIdx;
    //uint8_t calMode;
    uint8_t diffSum;
    uint8_t maxCnt;
    uint8_t maxCntAll;
    uint8_t gridWidth;
    uint8_t *instIdx;
    pos_minMax2_t instPos;
    pos_minMax2_t instPos2;
    //float dens;
} initial_line_group_t;

typedef struct {
    uint8_t line;
    uint8_t grp;
} initial_line_idx_t;

#ifdef FINE_CLIPPING_NEW
typedef struct {
    uint8_t lineIdx;
    uint8_t grpIdx;
    uint8_t lineIdx_2;
    uint8_t grpIdx_2;
} initial_line_closed_idx_t;
#endif

#if 1 //reserved, do not remove
typedef struct {
    in_line_t line;
    int instLen;
    int sortLen; //nsmoon@190527
    vec_t *inst;
    uint8_t *sort;
    in_line_t *lineSen;
    initial_line_group_t *grp;
    vec_t cent;
    pos_minMax2_t linePos2;
    uint8_t grpCnt; //grp cnt
    uint8_t grpCntSav; //grp cnt tmp
    //uint8_t grpRealCnt; //grp cnt
    uint8_t merged;
    uint8_t orthShareId;
    uint16_t isEdgeArea; //nsmoon@200923
} initial_line_t;
#endif

typedef struct {
    vec_t cent; //nsmoon@211125 change position
    in_line_t line;
    uint8_t cxLineIdx; //cross line idx for y-axis
    uint8_t cxGrpIdx;  //cross grp idx for y-axis
    uint8_t cxpCntMax; //cross point cnt max
    uint8_t cxOppLine[MAX_FINE_INITIAL_CXP_PER_LINE];
    uint8_t cxTcIdx[MAX_FINE_INITIAL_CXP_PER_LINE];
    uint8_t score[MAX_FINE_INITIAL_CXP_PER_LINE]; //score for revival
    uint8_t score2[MAX_FINE_INITIAL_CXP_PER_LINE]; //score for revival
    uint16_t instLen_a[MAX_FINE_INITIAL_CXP_PER_LINE]; //nsmoon@211125 int=>uint16_t
    uint16_t sortLen_a[MAX_FINE_INITIAL_CXP_PER_LINE]; //nsmoon@190527
    uint16_t isEdgeArea[MAX_FINE_INITIAL_CXP_PER_LINE]; //nsmoon@200923
    vec_t cxpCent[MAX_FINE_INITIAL_CXP_PER_LINE];
    uint8_t *sort_a[MAX_FINE_INITIAL_CXP_PER_LINE];
    vec_t *inst_a[MAX_FINE_INITIAL_CXP_PER_LINE];
    in_line_t *lineSen_a[MAX_FINE_INITIAL_CXP_PER_LINE];
    initial_line_group_t *grp_a[MAX_FINE_INITIAL_CXP_PER_LINE];
} initial_line_a_t;

#if 0 //def FINE_TEST_RULE_1
typedef struct {
    in_line_t line;
    uint8_t cxpCntMax; //cross point cnt max
    uint8_t cxOppLine[MAX_FINE_INITIAL_CXP_PER_LINE];
    vec_t cxpCent[MAX_FINE_INITIAL_CXP_PER_LINE];
    int instLen_a[MAX_FINE_INITIAL_CXP_PER_LINE];
    int sortLen_a[MAX_FINE_INITIAL_CXP_PER_LINE]; //nsmoon@190527
    vec_t *inst_a[MAX_FINE_INITIAL_CXP_PER_LINE];
    uint8_t *sort_a[MAX_FINE_INITIAL_CXP_PER_LINE];
    uint8_t *lineStat_a[MAX_FINE_INITIAL_CXP_PER_LINE];
    initial_line_group_t *grp_a[MAX_FINE_INITIAL_CXP_PER_LINE];
} initial_line_cxp_t;
#endif

typedef struct {
    int slopeValMin;
    int slopeValMax;
    int pdNoMin;
    int pdNoMax;
    int lineIdxMin;
    int lineIdxMax;
} fine_cal_min_max_info_t;

#define MAX_TOUCH_INFO_FINE_SIZE	(MAX_TOUCH_LIMIT_FINE * 5)
typedef struct {
	uint8_t fineStat;
	uint8_t lineIdxX;
	uint8_t grpIdxX;
    uint8_t lineIdxXY;  //nsmoon@230412 lineIdxY=>lineIdxXY
	uint8_t grpIdxY;
	uint8_t th50CntX;
	uint8_t th10CntX;
	uint8_t th50CntY;
	uint8_t th10CntY;
    uint8_t diffSum;
    uint8_t maxCnt;
    uint8_t gridWidth;
    initial_line_a_t *lineX;
    initial_line_a_t *lineXY;  //nsmoon@230412 lineIdxY=>lineIdxXY
    //float weightFinal;
    //float weight;
    //float xyGrpDist;
    //float grpXposX;
    pos_min_max_t mM; //min,max
    pos_min_max_t mM2; //th10
    vec_t centerPos;
    uint8_t lineIdxX0;
#ifdef FINE_REMOVE_GHOST_NEW
    uint8_t lineIdxY0; //nsmoon@230419
    uint8_t tiXcnt; //nsmoon@230424
    uint8_t tiYcnt; //nsmoon@230515
#endif
} touch_info_fine_t;

#if 1 //nsmoon@211028
#define FINE_SHADOW_GET_SLOPE_LINE_FIRST    0
#define FINE_SHADOW_GET_SLOPE_LINE_MID      1
#define FINE_SHADOW_GET_SLOPE_LINE_LAST     2
typedef struct {
    uint16_t sortCnt;
    uint16_t inBufIdx;
    int16_t slopeVal;
    int8_t slopeSign;
    uint8_t min_max;
} find_shadow_slope_line_t;
#endif

#if 1 //nsmoon@230418
#define FINE_GHOST_MODE_X    0 //tp on same x-line
#define FINE_GHOST_MODE_Y    1 //tp on same y-line
#define FINE_GHOST_MODE_XY   2 //near tp
#define FINE_GHOST_MODE_END  FINE_GHOST_MODE_XY //end
#define FINE_REMOVE_GHOST_DENSE_DIFF_X    0.4f //0.6f
#define FINE_REMOVE_GHOST_DENSE_DIFF_Y    0.4f //0.6f
#define FINE_REMOVE_GHOST_DENSE_DIFF_X2   0.1f
#define FINE_NEAR_CXP_DIST  3.0f //2.0f //EPSILON //0.8f //1.5f //2.0f //nsmoon@230426 3.0f
#define FINE_EDGE_NEAR_CXP_DIST  2.0f //0.5f
#define FINE_NEAR_EDGE_SLOPE   15
#endif

extern initial_line_t BS_initial_line_x[MAX_INITIAL_LINE_FINE_X];
extern initial_line_t BS_initial_line_y[MAX_INITIAL_LINE_FINE_Y];
extern int BS_initial_line_x_cnt, BS_initial_line_y_cnt;

extern initial_line_a_t BS_initial_line_a_x[MAX_INITIAL_LINE_FINE_X];
extern initial_line_a_t BS_initial_line_a_x2[MAX_INITIAL_LINE_FINE_X2];
extern initial_line_a_t BS_initial_line_a_y[MAX_INITIAL_LINE_FINE_XY]; //nsmoon@230412 MAX_INITIAL_LINE_FINE_Y=>MAX_INITIAL_LINE_FINE_XY
extern int BS_initial_line_a_x_cnt, BS_initial_line_a_y_cnt; //BS_initial_line_a_x2_cnt, BS_initial_line_a_x_cnt_real
extern int BS_initial_line_a_x_slope, BS_initial_line_a_y_slope;
#ifdef N75_TEST //nsmoon@230425
extern vec_t BS_initial_cxp[MAX_INITIAL_LINE_FINE_XY];
#endif

extern touch_info_fine_t BS_touch_info_fine[MAX_TOUCH_INFO_FINE_SIZE];
extern int BS_touch_info_fine_cnt;
//extern int16_t *BS_rem_init_line_slope_idx; //nsmoon@201019

extern int BG_clipping_fine4(void);
extern int BG_clipping_fine5(int fineLoop);
//extern int BS_fine_add_remained_touch(int touchCntFine);
extern int BS_fine_add_remained_touch_xy(int fineLoop);
extern int BS_fine_add_remained_touch_shadow(int fineLoop);
#if (BRUSH_MODE_ENABLE == 1)
extern int BS_fine_add_remained_touch_brush(int fineLoop);
extern int BS_fine_add_remained_touch_brush2(int fineLoop);
#endif
extern int BS_fine_add_remained_touch_edge_x(); //nsmoon@201118
extern int BS_fine_add_remained_touch_edge_y(); //nsmoon@220127

#if 1 //nsmoon@230510
#define FINE_INIT_EP5_MIN_SLOPE    0
#define FINE_INIT_EP5_MAX_SLOPE    1
extern int BS_fine_get_initial_ep5(axis_t axis, uint16_t *remLine, int *remLineCnt_in, int mode);
#else
extern int BS_fine_get_initial_ep5(axis_t axis, uint16_t *remLine, int *remLineCnt_in);
#endif

#ifdef USE_CUST_MALLOC //nsmon@201012
extern int BS_max_fine_rem_init_line;
extern int BS_fine_malloc(void);
#endif

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif
#endif //_BACKEND_MULTI_H
/* *****************************************************************************
 End of File
 */
