/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Lispect

  @File Name
    fineTouch.c

  @Summary
    Fine Touch Routine

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */
#include "backend_main.h"
#include "backend_fine.h"
#include "backend_multi.h" //nsmoon@220125
#include "backend_utils.h"
#include "IRTP_layout.h"
#include "backend_debug.h"

////////////////////////////
// Local Define
////////////////////////////
#define FINE_MAX_HALF_WIDTH     10.0f //5.0f //3.0f //2.0f //nsmoon@200331 5=>10
#define FINE_MINMAX_WIDTH_MIN   1.0f

//////////////////////////////////////////
// Static Variables
//////////////////////////////////////////
#define MAX_NEAR_LINE    5
#define MAX_NEAR_GRP     10
//ATTR_BACKEND_RAM3 static int nearLine[MAX_NEAR_LINE];
//ATTR_BACKEND_RAM3 static float nearGrpDist[MAX_NEAR_GRP];

#if 1 //nsmoon@211027
#define FINE_CENT_RHT    0
#define FINE_CENT_LFT    1
#define FINE_CENT_TOP    2
#define FINE_CENT_BOT    3
#endif

//////////////////////////////////////////
// Static Functions
//////////////////////////////////////////
//static int chk_cont_epa_sen(sen_type_t senType, ep_no_t *epaX, int epaXCnt, int idx);
//static void adj_grp_min_max(initial_line_group_t *grp, vec_t *inst, pos_minMax2_t *instPos0);
//static int fine_select_grp_maxlen(axis_t axis1, int lineIdx);
//static int is_exist_grp_y(int initialLineIdxY, int grpIdx0, initial_line_idx_t *lineX);
//static int fine_remove_mismatch_grp(axis_t axis1, int lineIdx);
//static int fine_make_near_x_line_list(int lineIdx0);
//static int fine_remove_ghost_grp(axis_t axis1, int nearLineCnt);
//static int fine_swap_y_line(void);
//static int fine_swap_y_line2(void);
//static int is_shared_edge_line_x(int pdTmp, int ledTmp);
//static int fine_is_epf_used(int epfNo);
//static void fine_add_epf_used(int epfNo);
//static int fine_chk_grp5_y(int lineIdx, int grpCnt0);
//static int fine_cal_min_max3(axis_t axis1, int initialLineIdx, int initialGrpIdx, axis_t axis2, uint16_t *remLine2, pos_minMax2_t *min_max, vec_t * retMinPos, vec_t *retMaxPos);
#ifdef FINE_DATA_CHECK //debug@190420-1
static int is_fine_data_ignore(int maxCnt, int idx);
#endif
#define FINE_REMAINED_INIT_LINE_MAX  MAX_TOUCH_LIMIT_FINE //4 //nsmoon@211017 //nsmoon@230412 4=>MAX_TOUCH_LIMIT_FINE
#ifdef FINE_REMAINED_INIT_LINE_MAX
static int fine_get_center_cross_lines_remained(vec_t *cent, int centLenMax, int *centCnt); //nsmoon@211017
#else
static int fine_get_center_cross_lines_remained(vec_t *cent);
#endif
//static int fine_get_center_cross_lines_remainedsort(axis_t axis, int remainedSortCnt, vec_t *cent);
static int fine_get_initial_tp(int fineLoop);
static int fine_get_initial_cxp(void); //nsmoon@230524
int BS_fine_get_crossed_lines(axis_t axis, int side, vec_t *p0, vec_t *p1, vec_t *width);

//////////////////////////////////////////
// Global Variable
//////////////////////////////////////////
ATTR_BACKEND_RAM3 initial_line_t BS_initial_line_x[MAX_INITIAL_LINE_FINE_X];
ATTR_BACKEND_RAM3 initial_line_t BS_initial_line_y[MAX_INITIAL_LINE_FINE_Y];
ATTR_BACKEND_RAM3 int BS_initial_line_x_cnt, BS_initial_line_y_cnt;

ATTR_BACKEND_RAM3 initial_line_a_t BS_initial_line_a_x[MAX_INITIAL_LINE_FINE_X];
ATTR_BACKEND_RAM3 initial_line_a_t BS_initial_line_a_x2[MAX_INITIAL_LINE_FINE_X2];
ATTR_BACKEND_RAM3 initial_line_a_t BS_initial_line_a_y[MAX_INITIAL_LINE_FINE_XY]; //nsmoon@230412 MAX_INITIAL_LINE_FINE_Y=>MAX_INITIAL_LINE_FINE_XY
ATTR_BACKEND_RAM3 int BS_initial_line_a_x_cnt, BS_initial_line_a_y_cnt; //BS_initial_line_a_x2_cnt, BS_initial_line_a_x_cnt_real
ATTR_BACKEND_RAM3 int BS_initial_line_a_x_y; //nsmoon@230510
ATTR_BACKEND_RAM3 int BS_initial_line_a_x_slope, BS_initial_line_a_y_slope; //nsmoon@230425

#ifdef USE_CUST_MALLOC //nsmon@201012
ATTR_BACKEND_RAM2 int BS_max_fine_rem_init_line;
ATTR_BACKEND_RAM2 int16_t *BS_rem_init_line_slope_idx;
ATTR_BACKEND_RAM2 int16_t *BS_rem_init_line_idx_res;
#ifdef USE_CONT_EPA_CHK2
ATTR_BACKEND_RAM2 int8_t *BS_epa_x_buff;
#else
ATTR_BACKEND_RAM2 uint16_t *BS_epa_x_sort;
#endif
#else
ATTR_BACKEND_RAM2 int16_t BS_rem_init_line_slope_idx[MAX_FINE_REM_INIT_LINE];
ATTR_BACKEND_RAM2 int16_t BS_rem_init_line_idx_res[MAX_FINE_REM_INIT_LINE];
#ifdef USE_CONT_EPA_CHK2
ATTR_BACKEND_RAM2 int8_t BS_epa_x_buff[MAX_FINE_REM_INIT_LINE];
#else
ATTR_BACKEND_RAM2 int BS_epa_x_sort[MAX_FINE_REM_INIT_LINE];
#endif
#endif

ATTR_BACKEND_RAM vec_t BS_inst_xy[MAX_NUM_INST_FINE];
ATTR_BACKEND_RAM in_line_t BS_inst_xy_sen[MAX_NUM_INST_FINE];
ATTR_BACKEND_RAM uint8_t BS_inst_xy_sort[MAX_NUM_INST_FINE];
ATTR_BACKEND_RAM int BS_inst_xy_cnt;

ATTR_BACKEND_RAM2 uint8_t BS_inst_xy_grp[MAX_NUM_INST_FINE];
ATTR_BACKEND_RAM2 int BS_inst_xy_grp_cnt;
ATTR_BACKEND_RAM int BS_inst_xy_cnt_pre, BS_inst_xy_grp_cnt_pre;  //nsmoon@230613

ATTR_BACKEND_RAM2 initial_line_group_t BS_initial_line_grp[MAX_INITIAL_LINE_GRP];
ATTR_BACKEND_RAM2 int BS_initial_line_grp_cnt;

ATTR_BACKEND_RAM2 touch_info_fine_t BS_touch_info_fine[MAX_TOUCH_INFO_FINE_SIZE];
ATTR_BACKEND_RAM3 static uint8_t find_touch_info_fine_sort[MAX_TOUCH_INFO_FINE_SIZE]; //nsmoon@230517
ATTR_BACKEND_RAM2 int BS_touch_info_fine_cnt;

#if 1 //nsmoon@200128
#define MAX_GRID_LINE_CNT    32
ATTR_BACKEND_RAM2 uint8_t BS_grid_line_cur[MAX_GRID_LINE_CNT];
ATTR_BACKEND_RAM2 uint8_t BS_grid_line_org[MAX_GRID_LINE_CNT];
ATTR_BACKEND_RAM2 uint8_t BS_grid_line_start[MAX_GRID_LINE_CNT];
ATTR_BACKEND_RAM2 uint8_t BS_grid_line_end[MAX_GRID_LINE_CNT];
ATTR_BACKEND_RAM2 int BS_grid_line_cnt;
#endif

//////////////////////////////////////////////////////////////////
#define NUM_OF_SLOPE_MAX         4
ATTR_BACKEND_RAM2 int BS_slopePlus[NUM_OF_SLOPE_MAX];
ATTR_BACKEND_RAM2 int BS_slopePlusIdx[NUM_OF_SLOPE_MAX];
ATTR_BACKEND_RAM2 int BS_slopeMinus[NUM_OF_SLOPE_MAX];
ATTR_BACKEND_RAM2 int BS_slopeMinusIdx[NUM_OF_SLOPE_MAX];
ATTR_BACKEND_RAM2 int BS_slopePlusUncrossed[NUM_OF_SLOPE_MAX];
ATTR_BACKEND_RAM2 int BS_slopeMinusUncrossed[NUM_OF_SLOPE_MAX];
//////////////////////////////////////////////////////////////////

#if 1 //nsmoon@201118
#define MAX_NUM_PD_REMLINE      10
ATTR_BACKEND_RAM2 static uint8_t pdEdgeBuf[MAX_NUM_PD_REMLINE], ledEdgeBuf[MAX_NUM_PD_REMLINE];
#endif

//////////////////////////////////////////
// Global Functions
//////////////////////////////////////////
void BG_epaInit_fine(void);
int BG_clipping_fine5(int fineLoop);

#ifdef USE_CUST_MALLOC //nsmoon@201012
ATTR_BACKEND_RAMFUNC
int BS_fine_malloc(void)
{
    if (BS_offsetTblLenX < 10 || BS_offsetTblLenY < 10) {
        TRACE_ERROR("ERROR! BG_fine_malloc invalid BS_offsetTblLenX,BS_offsetTblLenY %d %d", BS_offsetTblLenX, BS_offsetTblLenY);
        return 1; //error
    }

    BS_max_fine_rem_init_line = GET_MAX(BS_offsetTblLenX, BS_offsetTblLenY); //init

    ////////////////////////////////////////////////
    /// int16_t *BS_rem_init_line_slope_idx
    ////////////////////////////////////////////////
    BS_rem_init_line_slope_idx = (int16_t *)BS_malloc(sizeof(int16_t) * BS_max_fine_rem_init_line);
    if (BS_rem_init_line_slope_idx == 0) {
        TRACE_ERROR("ERROR! BS_fine_malloc..BS_rem_init_line_slope_idx");
        return 1; //error
    }
    TRACE_MALLOC("BS_rem_init_line_slope_idx=,%x,%x,%x,", &BS_rem_init_line_slope_idx[0], sizeof(int16_t) * BS_max_fine_rem_init_line, BS_end_of_heap);

    ////////////////////////////////////////////////
    /// int16_t *BS_rem_init_line_idx_res
    ////////////////////////////////////////////////
    BS_rem_init_line_idx_res = (int16_t *)BS_malloc(sizeof(int16_t) * BS_max_fine_rem_init_line);
    if (BS_rem_init_line_idx_res == 0) {
        TRACE_ERROR("ERROR! BS_fine_malloc..BS_rem_init_line_idx_res");
        return 1; //error
    }
    TRACE_MALLOC("BS_rem_init_line_idx_res=,%x,%x,%x,", &BS_rem_init_line_idx_res[0], sizeof(int16_t) * BS_max_fine_rem_init_line, BS_end_of_heap);

#ifdef USE_CONT_EPA_CHK2
    ////////////////////////////////////////////////
    /// int8_t *BS_epa_x_buff
    ////////////////////////////////////////////////
    BS_epa_x_buff = (int8_t *)BS_malloc(sizeof(int8_t) * BS_max_fine_rem_init_line);
    if (BS_epa_x_buff == 0) {
        TRACE_ERROR("ERROR! BS_fine_malloc..BS_epa_x_buff");
        return 1; //error
    }
    TRACE_MALLOC("BS_epa_x_buff=,%x,%x,%x,", &BS_epa_x_buff[0], sizeof(int8_t) * BS_max_fine_rem_init_line, BS_end_of_heap);
#else
    ////////////////////////////////////////////////
    /// uint16_t *BS_epa_x_sort
    ////////////////////////////////////////////////
    BS_epa_x_sort = (uint16_t *)BS_malloc(sizeof(uint16_t) * BS_max_fine_rem_init_line);
    if (BS_epa_x_sort == 0) {
        TRACE_ERROR("ERROR! BS_fine_malloc..BS_epa_x_sort");
        return 1; //error
    }
    TRACE_MALLOC("BS_epa_x_sort=,%x,%x,%x,", &BS_epa_x_sort[0], sizeof(uint16_t) * BS_max_fine_rem_init_line, BS_end_of_heap);
#endif

    return 0; //no-error
}
#endif

//////////////////////////////////////////
//#define DEBUG_DUMP_INITIAL_LINE_INFO__
#ifdef DEBUG_DUMP_INITIAL_LINE_INFO__
ATTR_BACKEND_RAMFUNC
static void DEBUG_dump_initial_line_info(int cnt)
{
    int initialLineIdx;
    initial_line_group_t *grpX; //, *grpY;
    int grpCntX; //, grpCntY;
    //pos_min_max_t minMaxTmp;
    //int cxLineIdx;
    vec_t *inst;
    int minInstIdx, maxInstIdx;
    int i;

    TRACE("DEBUG_dump_initial_line_info: %d", cnt);
    for (initialLineIdx = 0; initialLineIdx < BS_initial_line_x_cnt; initialLineIdx++)
    {
        if (BS_initial_line_x[initialLineIdx].instLen < 0) {
            continue;
        }
        inst = BS_initial_line_x[initialLineIdx].inst;
        grpX = BS_initial_line_x[initialLineIdx].grp;
        grpCntX = BS_initial_line_x[initialLineIdx].grpCnt;
        for (i = 0; i < grpCntX; i++) {
            if (grpX[i].stat != 1) continue;
            minInstIdx = grpX[i].instIdx[0];
            maxInstIdx = grpX[i].instIdx[grpX[i].len - 1];
            TRACE("..Y(%d/%d) %d(%0.2f,%0.2f) %d(%0.2f,%0.2f)", initialLineIdx, i, minInstIdx, inst[minInstIdx].x, inst[minInstIdx].y, maxInstIdx, inst[maxInstIdx].x, inst[maxInstIdx].y);
        }
    }
}
#endif

ATTR_BACKEND_RAMFUNC
static int fine_bs_mem_init(void)
{
    BS_inst_xy_cnt = 0; //init
    BS_inst_xy_grp_cnt = 0; //init
    BS_initial_line_grp_cnt = 0; //init
    BS_initial_line_a_x_cnt = 0; //init
    //BS_initial_line_a_x2_cnt = 0; //init
    BS_initial_line_a_y_cnt = 0; //init
    BS_initial_line_x_cnt = 0; //init
    BS_initial_line_y_cnt = 0; //init
#if 1  //nsmoon@230613
    BS_initial_line_a_x_y = 0;
    BS_initial_line_a_x_slope = 0;
    BS_initial_line_a_y_slope = 0;
#endif
    return 0;
}

static int fine_add_used_line(axis_t axis1, initial_line_group_t *grp, in_line_t *lineSen)
{
    int j;
    int pd, led;
    for (j = 0; j < grp->len; j++) {
        int tmpIdx2 = grp->instIdx[j]; //instIdx==instIdx
        led = lineSen[tmpIdx2].led;
        pd = lineSen[tmpIdx2].pd;
        BS_set_unset2_used_led_pd(axis1, led, pd, 1); //1:set
    } //for (j = 0; j < grp->len; j++)

    return 0; //no-error
}

#ifdef FRONTEND_LINE_THRESHOLD
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_fine_getThresholdCnt	0
#endif
#if (DEBUG_fine_getThresholdCnt > 0) //defined(DEBUG_FUNCTION_ENABLE_RELEASE)  //def DRAW_POLYGON_TEST
#define TRACE_FGTC(...)		TRACE(__VA_ARGS__)
#else
#define TRACE_FGTC(...)
#endif
ATTR_BACKEND_RAMFUNC
//static int fine_getThresholdCnt(axis_t axis1, initial_line_group_t *grp, in_line_t *lineSen, pos_minMax2_t *instPos2, tp_line_cnt_t *thCnt)
static int fine_getThresholdCnt(axis_t axis, pos_min_max_t *minMaxIn, initial_line_group_t *grp, in_line_t *lineSen, tp_line_cnt_t *thCnt)
{
    int j;
    int pd, ofstIdx, th10Cnt = 0;
	int led, slopeVal, slopeValMax; //maxPdNum;
    pos_min_max_t minMaxPos2;
    int ret;

    if (axis == ENUM_HOR_X) {
        slopeValMax = BS_slopeValMaxX;
		//maxPdNum = BS_maxHorPdNum - 1;
    }
    else {
        slopeValMax = BS_slopeValMaxY;
		//maxPdNum = BS_maxVerPdNum - 1;
    }
    minMaxPos2.minX = MIN_INITIAL_VAL;
    minMaxPos2.maxX = 0;
        minMaxPos2.minY = MIN_INITIAL_VAL;
        minMaxPos2.maxY = 0;
#if 0 //nsmoon@200924 not-use
    fine_add_used_line(axis, grp, lineSen);
#endif
#if (DEBUG_fine_getThresholdCnt > 1)
    IS_DEBUG_FLAG{TRACE("  grp->len=(%d) %d", axis, grp->len);};
#endif

    for (j = 0; j < grp->len; j++) {
        int tmpIdx2 = grp->instIdx[j]; //instIdx==instIdx
        led = lineSen[tmpIdx2].led;
        pd = lineSen[tmpIdx2].pd;
        slopeVal = led - pd;
        ofstIdx = slopeVal + slopeValMax;
#if (DEBUG_fine_getThresholdCnt > 1)
        DEBUG_SHOW_LINE_PD_LED(axis, pd, led, 3);
#endif
        ret = BS_is_set_threshold(axis, pd, ofstIdx);
        if (ret) {
            //BS_calMinMaxSen(axis, led, pd, &minMaxPos, &minMaxPos2);
            BS_calMinMaxSen(axis, led, pd, minMaxIn, &minMaxPos2);
            th10Cnt++;
        }
    } //for (j = 0; j < grp->len; j++)

#define FINE_TH10_WIDTH_MIN    0.01f //nsmoon@230612
    if (axis == ENUM_HOR_X) {
        thCnt->th50CntX = grp->len;
        thCnt->th10CntX = (uint8_t)GET_MIN(th10Cnt, UINT8_MAX);
        if (minMaxPos2.minX == MIN_INITIAL_VAL) {
            minMaxPos2.minX = 0.0f; //nsmoon@200302
        }
        if (th10Cnt > 1) { //nsmoon@200602
            thCnt->th10WidthX = GET_ABS(minMaxPos2.maxX - minMaxPos2.minX);
        }
        else {
            thCnt->th10WidthX = 0;
        }
        thCnt->th10WidthX = GET_MAX(thCnt->th10WidthX, FINE_TH10_WIDTH_MIN); //nsmoon@230612
#if (DEBUG_fine_getThresholdCnt > 0) //defined(DEBUG_FUNCTION_ENABLE_RELEASE)  //def DRAW_POLYGON_TEST
        IS_DEBUG_FLAG{
            TRACE("  (F)th50CntX,th10CntX= %d,%d (%0.2f,%0.2f)", thCnt->th50CntX, thCnt->th10CntX, minMaxIn->maxX-minMaxIn->minX, thCnt->th10WidthX);
            if (thCnt->th10WidthX < FINE_TH10_WIDTH_MIN) {
                TRACE("  minMaxPos2.maxX= %f %f", minMaxPos2.maxX, minMaxPos2.minX);
            }
        };
#endif
    }
    else {
        thCnt->th50CntY = grp->len;
        thCnt->th10CntY = (uint8_t)GET_MIN(th10Cnt, UINT8_MAX);
        if (minMaxPos2.minY == MIN_INITIAL_VAL) {
            minMaxPos2.minY = 0.0f; //nsmoon@200302
        }
        if (th10Cnt > 1) { //nsmoon@200602
            thCnt->th10WidthY = GET_ABS(minMaxPos2.maxY - minMaxPos2.minY);
        }
        else {
            thCnt->th10WidthY = 0;
        }
        thCnt->th10WidthY = GET_MAX(thCnt->th10WidthY, FINE_TH10_WIDTH_MIN); //nsmoon@230612
#if (DEBUG_fine_getThresholdCnt > 0) //defined(DEBUG_FUNCTION_ENABLE_RELEASE)  //def DRAW_POLYGON_TEST
        IS_DEBUG_FLAG{
            TRACE("  (F)th50CntY,th10CntY= %d,%d (%0.2f,%0.2f)", thCnt->th50CntY, thCnt->th10CntY, minMaxIn->maxY-minMaxIn->minY, thCnt->th10WidthY);
            if (thCnt->th10WidthY < FINE_TH10_WIDTH_MIN) {
                TRACE("  minMaxPos2.maxY= %f %f", minMaxPos2.maxY, minMaxPos2.minY);
            }
        };
#endif
    }
    return th10Cnt;
}
#endif

#ifdef FINE_DATA_CHECK //debug@190420-1
ATTR_BACKEND_RAMFUNC
static int is_fine_data_ignore(int maxCnt, int idx)
{
    int i;
    float distSq, diffTmp;
    vec_t p1, p2;

    p1.x = BG_touch_data_fine[idx].x;
    p1.y = BG_touch_data_fine[idx].y;

    diffTmp = GET_ABS(p1.x - BS_aarea_zero_x);
    if (diffTmp > FINE_DATA_CHECK_AAREA_DIST) {
        diffTmp = GET_ABS(p1.x - BS_aarea_end_x);
        if (diffTmp > FINE_DATA_CHECK_AAREA_DIST) {
            diffTmp = GET_ABS(p1.y - BS_aarea_zero_y);
            if (diffTmp > FINE_DATA_CHECK_AAREA_DIST) {
                diffTmp = GET_ABS(p1.y - BS_aarea_end_y);
                if (diffTmp > FINE_DATA_CHECK_AAREA_DIST) {
                    return 0;
                }
            }
        }
    }

    for (i = 0; i < maxCnt; i++) {
        if (i == idx) continue;
        p2.x = BG_touch_data_fine[i].x;
        p2.y = BG_touch_data_fine[i].y;
        distSq = BG_calDistSquare(p1, p2);
        if (distSq <= FINE_DATA_CHECK_DIST) {
            return 1; //ignore
        }
    }
    return 0; //not-ignore
}
#endif

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_BG_clipping_fine2         1
#endif
#if (DEBUG_BG_clipping_fine2 > 1)
#define TRACE_BCF2(...)      TRACE(__VA_ARGS__)
#else
#define TRACE_BCF2(...)
#endif
#if 0 //for test
ATTR_BACKEND_RAMFUNC
static int fine_remove_closed_touch(int touchCntLocal, vec_t centerPoint0)
{
    int i;
    float distX, distY;
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };
    for (i = 0; i < touchCntLocal; i++) {
        distX = GET_ABS(centerPoint0.x - BG_touch_data[i].x);
        distY = GET_ABS(centerPoint0.y - BG_touch_data[i].y);
        if (distX < FINE_REMOVE_TOUCH_POS && distY < FINE_REMOVE_TOUCH_POS) {
            IS_DEBUG_FLAG { TRACE_BCF2("distX,distY: %0.2f %0.2f", distX, distY);};
            return 1; //too closed
        }
    }
    return 0;
}
#endif //0

#if defined(DEBUG_FUNCTION_ENABLE_ALL) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
#define DEBUG_fine_get_initial_ep	1 //0~3
#endif
#if (DEBUG_fine_get_initial_ep > 1)
#define TRACENR_FGIE(...)	TRACENR(__VA_ARGS__)
#define TRACE_FGIE(...)		TRACE(__VA_ARGS__)
#else
#define TRACENR_FGIE(...)
#define TRACE_FGIE(...)
#endif

static int isCrossOtheLineX(int epaXCnt, int pd0, int led0)
{
    ep_no_t *epaX = &BS_edge_pattern_ep[0];
    int i, pd1, led1;
    //int width = 0;
    //int slopeVal0 = led0 - pd0;
    //int slopeVal1;

    for (i = 0; i < epaXCnt; i++) {
        pd1 = epaX[i].e1;
        led1 = epaX[i].s1;
        //slopeVal1 = led1 - pd1;
        if ((pd0 >= pd1 && led0 >= led1) || (pd0 <= pd1 && led0 <= led1)) {
            if (pd0 == pd1 && led0 == led1) {
                return 1; //crossed
            }
#if 0 //do not use
            else if (slopeVal1 == slopeVal0) {
                //ignore closed parallel line
                width = GET_ABS(pd0 - pd1);
                if (width == 1) {
                    return 1; //crossed
                }
            }
#endif
        }
        else {
            return 1; //crossed
        }
    }
    return 0; //un-crossed
}

ATTR_BACKEND_RAMFUNC
static int save_epa_max(ep_no_t *epaX, int epaXCnt, ep_no_t *epaXmax)
{
    int j, epaXmaxCnt = 0;
    for (j = 0; j < epaXCnt; j++) {
        if (epaX[j].e2 == 0) {
            //not-continuous
            epaXmax[epaXmaxCnt++] = epaX[j];
        }
    }
    return epaXmaxCnt;
}

#if 1 //nsmoon@220124
static int is_cont_sen(axis_t axis, uint8_t *inlineBuf, uint8_t pd, uint8_t start0, uint8_t end0)
{
    int i, start, end, lineBufIdx;
    int maxInLineBufLed = (axis == ENUM_HOR_X) ? BS_max_in_line_buf_led_x : BS_max_in_line_buf_led_y;
    uint8_t senMask;
    if (start0 > end0) {
        start = end0;
        end = start0;
    }
    else if (start0 < end0) {
        start = start0;
        end = end0;
    }
    else {
        return 0; //same
    }

    for (i = start; i < end; i++) {
        lineBufIdx = GET_IN_LINE_IDX(pd, i);
        senMask = GET_LED_BIT_MASK(i);
        if ((inlineBuf[lineBufIdx] & senMask) == 0) {
            return 0; //not continuous
        }
    }

    return 1; //continuous
}

#if 1 //for test
static int is_cont_sen_same_slope(axis_t axis, uint8_t *inlineBuf, uint8_t pd0, uint8_t led0, int pdDiff)
{
    int i, senMax, pd, led;
    int maxInLineBufLed, lineBufIdx;
    uint8_t senMask;
    int inc = (pdDiff < 0) ? -1 : 1; //pd0 < pd1

    if (pdDiff == 0) {
        return 1; //same
    }
    else if (GET_ABS(pdDiff) == 1) {
        return 1; //continuous
    }

    if (axis == ENUM_HOR_X) {
        maxInLineBufLed = BS_max_in_line_buf_led_x;
        senMax = BS_maxHorPdNum;
    }
    else {
        maxInLineBufLed = BS_max_in_line_buf_led_y;
        senMax = BS_maxVerPdNum;
    }

    for (i = (1*inc); (i*inc) < (pdDiff*inc); i += (1*inc)) {
        pd = pd0 + i;
        led = led0 + i;
        if (pd >= senMax || pd < 0 || led >= senMax || led < 0) {
            TRACE_ERROR("ERROR! is_cont_sen_same_slope..invalid pd0 %d %d %d", pd0, led0, pdDiff);
            continue;
        }
        lineBufIdx = GET_IN_LINE_IDX(pd, led);
        senMask = GET_LED_BIT_MASK(led);
        if ((inlineBuf[lineBufIdx] & senMask) == 0) {
            return 0; //not continuous
        }
    }

    return 1; //continuous
}
#endif

static int is_cont_line(axis_t axis, uint8_t pd0, uint8_t led0, uint8_t pd1, uint8_t led1)
{
    uint8_t *inlineBuf;
    int pdDiff, slope0, slope1;

    if (pd0 == pd1 && led0 == led1) {
        return 1; //same
    }
    else if (pd0 == pd1) {
        inlineBuf = (axis == ENUM_HOR_X) ? IN_LINE_BUF_ADDR_X_PD : IN_LINE_BUF_ADDR_Y_PD;
        if (is_cont_sen(axis, inlineBuf, pd0, led0, led1)) {
            return 1; //continuous
        }
    }
    else if (led0 == led1) {
        inlineBuf = (axis == ENUM_HOR_X) ? IN_LINE_BUF_ADDR_X_LED : IN_LINE_BUF_ADDR_Y_LED;
        if (is_cont_sen(axis, inlineBuf, led0, pd0, pd1)) {
            return 1; //continuous
        }
    }
    else
    {
        inlineBuf = (axis == ENUM_HOR_X) ? IN_LINE_BUF_ADDR_X_PD : IN_LINE_BUF_ADDR_Y_PD;
        slope0 = led0 - pd0;
        slope1 = led1 - pd1;
        if (slope0 == slope1) {
            pdDiff = pd1 - pd0;
            if (is_cont_sen_same_slope(axis, inlineBuf, pd0, led0, pdDiff)) {
                return 1; //continuous
            }
        }
    }
    return 0;
}
#endif //1

ATTR_BACKEND_RAMFUNC
int BS_fine_get_initial_ep5(axis_t axis, uint16_t *remLine, int *remLineCnt_in, int mode)
{
    //mode: FINE_INIT_EP5_MIN_SLOPE(min slope first), FINE_INIT_EP5_MAX_SLOPE(max slope first)
    int i, j, k, ret;
    int remLineIdx0, remLineIdx;
    initial_line_a_t *initial_line;
    int initialLineCnt, initialLineCntMax;
    int epaXCnt, epaXmaxCnt;
    int pd0, led0, pd1, led1;
    int tmpInBufIdx;
    int slopeVal, slopeIdx;
    int slopeSum, slopeSumMin; //senSum;
    int remLineSlopeIdxCnt;
    int remLineCnt = *remLineCnt_in;
    int slopeValMax;
    int initial_line_slope = 0; //nsmoon@230425
#if (DEBUG_fine_get_initial_ep > 1)
    IS_DEBUG_FLAG{
        TRACE("*BS_fine_get_initial_ep5...%d %d", axis, mode);
        TRACE_NOP;
    };
#endif

    ep_no_t *epaX = &BS_edge_pattern_ep[0];
#ifdef USE_CUST_MALLOC //nsmon@201012
    ep_no_t *epaXmax = &BS_edge_pattern_ep[BS_max_ep_count / 2]; //nsmoon@201014
#else
    ep_no_t *epaXmax = &BS_edge_pattern_ep[MAX_EP_COUNT / 2];
#endif
    int16_t *remLineSlopeIdx = &BS_rem_init_line_slope_idx[0];
    int16_t *remInitLineIdxRes = &BS_rem_init_line_idx_res[0];

    //TRACE("remLineSlopeIdx= %x %x", remLineSlopeIdx, &BS_rem_init_line_slope_idx[0]);
    if (remLineSlopeIdx == 0) {
        TRACE_NOP;
    }

    if (axis == ENUM_HOR_X) {
        //x-axis, remained line
        initial_line = &BS_initial_line_a_x[0];
        initialLineCntMax = MAX_INITIAL_LINE_FINE_X;
        slopeValMax = BS_slopeValMaxX;
        remLineSlopeIdxCnt = BS_offsetTblLenX;
    }
    else {
        //y-axis, remained line
        initial_line = &BS_initial_line_a_y[0];
        initialLineCntMax = MAX_INITIAL_LINE_FINE_Y;
        slopeValMax = BS_slopeValMaxY;
        remLineSlopeIdxCnt = BS_offsetTblLenY;
    }

#if 1 //nsmoon@201110
    if (remLineCnt > MAX_REMAINED_LINE) {
        TRACE_ERROR("ERROR! BS_fine_get_initial_ep5..invalid remLineCnt: %d", remLineCnt);
		remLineCnt = MAX_REMAINED_LINE;
        //FIXME, no-error
    }
#endif
#if (DEBUG_fine_get_initial_ep > 3)
    IS_DEBUG_FLAG{
        TRACENR_FGIE("remLine[]: %d %d", axis, remLineCnt);
        for (i = 0; i < remLineCnt; i++) {
            tmpInBufIdx = remLine[i];
            if (!BS_getSenInBuf(axis, tmpInBufIdx, &pd0, &led0)) {
                TRACE_FGIE(" %d-%d", pd0, led0);
            }
        }
    };
#endif

    //init
#ifdef USE_CUST_MALLOC //nsmon@201012
    for (i = 0; i < BS_max_fine_rem_init_line; i++) {
#else
    for (i = 0; i < MAX_FINE_REM_INIT_LINE; i++) {
#endif
        remLineSlopeIdx[i] = -1;
    }

    //////////////////////////////////////////////
    //find presentative line of each slope
    //remLineSlopeIdxCnt = slopeValMax * 2 + 1; //x-axis only
    //remLineIdxMinSlopeVal = -1;
    for (remLineIdx = 0; remLineIdx < remLineCnt; remLineIdx++)
    {
        tmpInBufIdx = remLine[remLineIdx];
        if (BS_getSenInBuf(axis, tmpInBufIdx, &pd0, &led0)) {
            //TRACE_ERROR("ERROR! BS_fine_get_initial_ep5..invalid inBufIdx: %d", tmpInBufIdx);
            continue;
        }
        slopeVal = led0 - pd0;
#if 1 //nsmoon@230425
        if (GET_ABS(slopeVal) > initial_line_slope) {
            initial_line_slope = GET_ABS(slopeVal);
        }
#endif
#if 0 //order-0: -10~0~+10
        slopeIdx = slopeValMax + slopeVal;
#endif
        if (mode == FINE_INIT_EP5_MAX_SLOPE) {
#if 1 //def FINE_MAX_SLOPE_FIRST //order-1: +10,-10~1,-1,0 (0530)
        if (slopeVal > 0) {
            slopeIdx = (slopeValMax - slopeVal) * 2;
        }
        else if (slopeVal < 0) {
            int tmpIdx = (slopeValMax + slopeVal + 1);
            slopeIdx = (tmpIdx * 2) - 1;
        }
        else {
            slopeIdx = slopeValMax * 2;
        }
#endif
        }
        else {
#if 1 //def FINE_MIN_SLOPE_FIRST //order-2: 0,1,-1,~ (0613)
        if (slopeVal > 0) {
            slopeIdx = (slopeVal * 2) - 1;
        }
        else if (slopeVal < 0) {
            slopeIdx = (-slopeVal) * 2;
        }
        else {
            slopeIdx = 0;
        }
#endif
        }
#if (DEBUG_fine_get_initial_ep > 2)
        IS_DEBUG_FLAG{ TRACE_FGIE("..slopeIdx:,%d,%d,%d,:,%d,%d", slopeIdx, slopeVal, tmpInBufIdx, led0, pd0); };
#endif
        if (slopeIdx < 0 || slopeIdx >= remLineSlopeIdxCnt) {
            TRACE_ERROR("ERROR! BS_fine_get_initial_ep5..invalid slopeIdx: %d", slopeIdx);
            //slopeIdx = 0;
            continue;
        }
        remLineSlopeIdx[slopeIdx] = (int16_t)remLineIdx;
        TRACE_NOP;
    } //for (remLineIdx = 0; remLineIdx < remLineCnt; remLineIdx++)

#if (DEBUG_fine_get_initial_ep > 3)
    IS_DEBUG_FLAG{
        TRACE_FGIE("remLineSlopeIdx[]: %d", remLineSlopeIdxCnt);
        for (i = 0; i < remLineSlopeIdxCnt; i++) {
            if (remLineSlopeIdx[i] < 0) continue;
            tmpInBufIdx = remLine[remLineSlopeIdx[i]];
            if (!BS_getSenInBuf(axis, tmpInBufIdx, &pd0, &led0)) {
                TRACE_FGIE(" %d,%d-%d", remLineSlopeIdx[i], pd0, led0);
            }
        }
    };
#endif
#if (DEBUG_fine_get_initial_ep > 1)
    IS_DEBUG_FLAG {
    for (i = 0; i < remLineSlopeIdxCnt; i++) {
        if (remLineSlopeIdx[i] < 0) continue;
        remLineIdx0 = remLineSlopeIdx[i];
        tmpInBufIdx = remLine[remLineIdx0];
        if (!BS_getSenInBuf(axis, tmpInBufIdx, &pd0, &led0)) {
                int slopeVal = led0 - pd0;
            //IS_DEBUG_FLAG{ DEBUG_SHOW_LINE_PD_LED(axis, pd0, led0, MY_COLOR); };
                TRACE_FGIE("  remLineSlopeIdxCnt=%d-%d %d-%d %d", axis, i, pd0, led0, slopeVal);
                //break;
        }
    }
    };
#endif

    /////////////////////////////////////
    //find uncrossed line
    epaXCnt = epaXmaxCnt = 0;
    slopeSumMin = MIN_INITIAL_VAL;
    //senSumMax = 0;
    for (i = 0; i < remLineSlopeIdxCnt; i++) {
        if (remLineSlopeIdx[i] < 0) continue;
#if (DEBUG_fine_get_initial_ep > 0)
        IS_DEBUG_FLAG {
            if (i == 6) {
                TRACE_NOP;
            }
        };
#endif
        remLineIdx0 = remLineSlopeIdx[i];
        tmpInBufIdx = remLine[remLineIdx0];
        if (!BS_getSenInBuf(axis, tmpInBufIdx, &pd0, &led0)) {
            //init epaX[0]
            epaX[0].e1 = (uint8_t)pd0;
            epaX[0].s1 = (uint8_t)led0;
            epaX[0].e2 = 0; //not crossed
            epaX[0].s2 = 0; //continuous
            epaXCnt = 1;
        }
        else {
            TRACE_ERROR("ERROR! BS_fine_get_initial_ep5.2.invalid inBufIdx: %d", tmpInBufIdx);
            continue;
        }

        slopeVal = led0 - pd0;
        for (remLineIdx = 0; remLineIdx < remLineCnt; remLineIdx++)
        {
            if (remLineIdx == remLineIdx0) continue;
            tmpInBufIdx = remLine[remLineIdx];
            if (!BS_getSenInBuf(axis, tmpInBufIdx, &pd1, &led1)) {
#if 1 //nsmoon@230426
                int slopeVal1 = led1 - pd1;
                if ((slopeVal > 0 && slopeVal1 < 0) ||
                        (slopeVal < 0 && slopeVal1 > 0) ) {
                    continue;
                }
#endif
#if 0 //(DEBUG_fine_get_initial_ep > 1)
                IS_DEBUG_FLAG {
                    if (i == 17 && remLineIdx == 64) {
                        TRACE_NOP;
                    }
                };
#endif
#if 0 //for test
                IS_DEBUG_FLAG{
                    if (axis == ENUM_VER_Y) {
                        if (pd0 == pd1 && pd1 == 24 && led1 == 34) {
                            TRACE_NOP;
                        }
                    }
                };
#endif
                ret = isCrossOtheLineX(epaXCnt, pd1, led1);
                if (ret == 0) {
                    //unclossed
#ifdef USE_CUST_MALLOC //nsmon@201012
                    if (epaXCnt < (BS_max_ep_count / 2)) { //nsmoon@201014
#else
                    if (epaXCnt < (MAX_EP_COUNT / 2)) {
#endif
                        epaX[epaXCnt].e1 = (uint8_t)pd1;
                        epaX[epaXCnt].s1 = (uint8_t)led1;
                        epaX[epaXCnt].e2 = 0; //not crossed
                        epaX[epaXCnt].s2 = 0; //continuous
                        epaXCnt++;
                    }
                    else {
                        TRACE_ERROR("ERROR! BS_fine_get_initial_ep5..invalid epaXCnt");
                        break;
                    }
                }
            }
            else {
                TRACE_ERROR("ERROR! BS_fine_get_initial_ep5..invalid inBufIdx: %d", tmpInBufIdx);
                continue;
            }
        }
    #if (DEBUG_fine_get_initial_ep > 2)
        IS_DEBUG_FLAG {
            if (i == 6) {
                uint8_t epaXcnt_str[100], idx = 0;
                TRACE_FGIE("epaXCnt[]:(%d-%d/%d)", axis, i, epaXCnt);
            for (j = 0; j < epaXCnt; j++) {
                    snprintf(&epaXcnt_str[idx], (100-idx), " %03d-%03d", epaX[j].e1, epaX[j].s1);
                    idx += 8;
                }
                epaXcnt_str[idx] = '\0';
                TRACE_FGIE("%s", epaXcnt_str);
            }
        };
#endif

#if 1 //nsmoon@220126
        //remove continous and adjacent parallel line
        uint8_t *inlineBuf = (axis == ENUM_HOR_X) ? IN_LINE_BUF_ADDR_X_PD : IN_LINE_BUF_ADDR_Y_PD;
        for (k = 0; k < epaXCnt; k++) {
            if (epaX[k].e2 != 0) continue;
            uint8_t pd0 = epaX[k].e1;
            uint8_t led0 = epaX[k].s1;
            int slope0 = led0 - pd0;
            for (j = 0; j < epaXCnt; j++) {
                if (k == j) continue;
                if (epaX[j].e2 != 0) continue;
                uint8_t pd1 = epaX[j].e1;
                uint8_t led1 = epaX[j].s1;
                int slope1 = led1 - pd1;
#if 0 //for test
                IS_DEBUG_FLAG{
                    if (axis == ENUM_VER_Y && i == 17) {
                        if (pd0 == pd1 && pd1 == 24 && led1 == 34) {
                            TRACE_NOP;
                        }
                    }
                };
#endif
                if (slope0 == slope1) {
                    int pdDiff = pd1 - pd0;
                    if (is_cont_sen_same_slope(axis, inlineBuf, pd0, led0, pdDiff)) {
                        epaX[j].e2 = 1; //continuous
                    }
                }
                else {
                    if (is_cont_line(axis, pd0, led0, pd1, led1)) {
#if 1 //nsmoon@230410 //TestData_N65ve\\4pen bot.dlt #36
                        int slope0a = GET_ABS(slope0);
                        int slope1a = GET_ABS(slope1);
                        if (slope0a > slope1a) {
                            epaX[i].e2 = 1; //continuous
                        }
                        else {
                        epaX[j].e2 = 1; //continuous
                    }
#else
                        epaX[j].e2 = 1; //continuous
#endif
                    }
                }
            }
        }
#else
        //check continous line
#ifdef USE_CONT_EPA_CHK2 //remove continuous line
        chk_cont_epa_sen2(&epaX[0], epaXCnt, 1); //pd
        chk_cont_epa_sen2(&epaX[0], epaXCnt, 0); //led
#else
        for (k = 0; k < epaXCnt; k++) {
            if (epaX[k].e2 != 0) continue;
#if (DEBUG_fine_get_initial_ep > 1)
            IS_DEBUG_FLAG { TRACE_FGIE("epaX:(%d/%d) %d %d %d %d", i, k, epaX[k].e1, epaX[k].s1, epaX[k].e2, epaX[k].s2); };
#endif
            for (j = 0; j < epaXCnt; j++) {
                if (epaX[j].e2 != 0) continue;
                if (k == j) continue;
                if (epaX[k].e1 == epaX[j].e1) {
                    chk_cont_epa_sen(ENUM_PD, &epaX[0], epaXCnt, k); //pd
                }
                else if (epaX[k].s1 == epaX[j].s1) {
                    chk_cont_epa_sen(ENUM_LED, &epaX[0], epaXCnt, k); //led
                }
            } //for (j = 0; j < epaXCnt; j++)
        } //for (k = 1; k < epaXCnt; k++)
#endif

        //remove adjacent parallel line, nsmoon@190628a
        for (k = 0; k < epaXCnt; k++) {
            if (epaX[k].e2 != 0) continue;
            for (j = 0; j < epaXCnt; j++) {
                if (epaX[j].e2 != 0) continue;
                if (k == j) continue;
                if (epaX[k].e1 == (epaX[j].e1 + 1) && epaX[k].s1 == (epaX[j].s1 + 1)) {
                    epaX[j].e2 = 1;
                    break;
                }
                else if (epaX[k].e1 == (epaX[j].e1 - 1) && epaX[k].s1 == (epaX[j].s1 - 1)) {
                    epaX[j].e2 = 1;
                    break;
                }
            }
        }
#endif

        //count uncrossed line
        remInitLineIdxRes[i] = 0; //init
        slopeSum = 0;
        for (j = 0; j < epaXCnt; j++) {
            if (epaX[j].e2 == 0) {
                //un-crossed
                remInitLineIdxRes[i]++;
                slopeSum += GET_ABS(epaX[j].s1 - epaX[j].e1);
            }
        }
#if 0 //nsmoon@210304
        int senSum = 0;
        for (j = 0; j < (epaXCnt - 1); j++) {
            if (epaX[j].e2 == 0) {
                for (k = (j+1); k < epaXCnt; k++) {
                    if (epaX[k].e2 == 0) {
                        if (epaX[j].s1 == epaX[k].s1 || epaX[j].e1 == epaX[k].e1) {
                            senSum++;
                        }
                    }
                }
            }
        }
#endif
#if (DEBUG_fine_get_initial_ep > 2)
        IS_DEBUG_FLAG { 
            TRACE_FGIE("=>remInitLineIdxRes[]: (%d) %d %d", i, remInitLineIdxRes[i], slopeSum);
        };
#endif

        //save epaXmax
        //if (epaXmaxCnt < remInitLineIdxRes[i])
        if (epaXmaxCnt < remInitLineIdxRes[i]) {
            epaXmaxCnt = save_epa_max(&epaX[0], epaXCnt, &epaXmax[0]);
            //remInitLineIdxMax = i;
        }
        else if (epaXmaxCnt == remInitLineIdxRes[i]) {
            if ((mode == FINE_INIT_EP5_MIN_SLOPE && slopeSum < slopeSumMin) ||
                (mode == FINE_INIT_EP5_MAX_SLOPE && slopeSum > slopeSumMin)) //nsmoon@230509
            {
                slopeSumMin = slopeSum;
                epaXmaxCnt = save_epa_max(&epaX[0], epaXCnt, &epaXmax[0]);
                //remInitLineIdxMax = i;
            }
        }
    } //for (i = 0; i < remLineSlopeIdxCnt; i++)

#if (DEBUG_fine_get_initial_ep > 3)
    IS_DEBUG_FLAG{
        TRACE_FGIE("remInitLineIdxRes[]:(%d)..",remLineSlopeIdxCnt);
        int maxIdxTmp = -1, maxResTmp = 0;
        for (i = 0; i < remLineSlopeIdxCnt; i++) {
            TRACE_FGIE("%03d,%03d ", remLineSlopeIdx[i], remInitLineIdxRes[i]);
            if (remInitLineIdxRes[i] > maxResTmp) {
                maxResTmp = remInitLineIdxRes[i];
                maxIdxTmp = remLineSlopeIdx[i];
            }
        }
        if (maxIdxTmp >= 0) {
            TRACE_FGIE("=>%d(%d)", maxIdxTmp, maxResTmp);
        }
    };
#endif

    if (epaXmaxCnt > MAX_INITIAL_LINE_FINE_Y) { //nsmoon@211028 MAX_INITIAL_LINE_FINE_X=>MAX_INITIAL_LINE_FINE_Y
        TRACE_ERROR("ERROR! invalid epaXmaxCnt(%d) %d", axis, epaXmaxCnt);
        epaXmaxCnt = MAX_INITIAL_LINE_FINE_Y;
    }
    initialLineCnt = 0;
    for (i = 0; i < epaXmaxCnt; i++) {
#if (DEBUG_fine_get_initial_ep > 1)
        IS_DEBUG_FLAG { TRACE_FGIE("  epaXmax:(%d) %d-%d %d", i, epaXmax[i].e1, epaXmax[i].s1, epaXmax[i].e2); };
#endif
        if (epaXmax[i].e2 == 0) {
#if (DEBUG_fine_get_initial_ep > 1)
            IS_DEBUG_FLAG{ DEBUG_SHOW_LINE_PD_LED(axis, epaXmax[i].e1, epaXmax[i].s1, 1); }; //initial line
#endif
            //if (i < MAX_INITIAL_LINE_FINE_X) {
            //if (i < MAX_INITIAL_LINE_FINE_X && initialLineCnt < initialLineCntMax) {
            if (initialLineCnt < initialLineCntMax) {
                initial_line[initialLineCnt].line.pd = epaXmax[i].e1;
                initial_line[initialLineCnt].line.led = epaXmax[i].s1;
#if (DEBUG_fine_get_initial_ep > 1)
                IS_DEBUG_FLAG{ DEBUG_SHOW_LINE_PD_LED(axis, epaXmax[i].e1, epaXmax[i].s1, 0); }; //initial line
#elif defined(INITIAL_LINE_TEST_MULTI)
                IS_DEBUG_FLAG{ DEBUG_SHOW_LINE_PD_LED(axis, epaXmax[i].e1, epaXmax[i].s1, 0); }; //initial line
#endif
                initialLineCnt++;
            }
            else {
                TRACE_ERROR("ERROR! BS_fine_get_initial_ep5..invalid initialLineCnt %d %d", initialLineCnt, initialLineCntMax);
                break;
                //return FINE_MEM_ERROR; //mem-error
            }
        }
    }
    if (axis == ENUM_HOR_X) {
        BS_initial_line_a_x_cnt = initialLineCnt;
        BS_initial_line_a_x_slope = initial_line_slope; //nsmoon@230425
    }
    else {
        BS_initial_line_a_y_cnt = initialLineCnt;
        BS_initial_line_a_y_slope = initial_line_slope; //nsmoon@230425
    }
#if (DEBUG_fine_get_initial_ep > 1)
    IS_DEBUG_FLAG{ TRACE_FGIE("  initialLineCnt: %d %d", axis, initialLineCnt); };
#endif
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };

    *remLineCnt_in = remLineCnt; //save
    return 0; //no-error
}

#ifdef FINE_INITIAL_LINE_CENT //nsmoon@230418
int BS_fine_get_initial_ep5_cent()
{
    int i, stopLoop;
    int remLineIdx0;
    initial_line_a_t *initial_line;
    int initLineIdx, initialLineCntDiff;
    int initialLineCnt0, initialLineCnt, initialLineCntMax;
    int pd0, led0, pd1, led1;
    int tmpInBufIdx;
    int slopeVal0, slopeVal1;
    int remLineSlopeIdxCnt;
    int remLineCnt;
    uint16_t *remLine;
#ifdef FINE_MAX_SLOPE_FIRST
    int slopeValMax;
#endif
    int16_t *remLineSlopeIdx = &BS_rem_init_line_slope_idx[0];
    axis_t axis;

    if (BS_initial_line_a_x_cnt > BS_initial_line_a_y_cnt && BS_initial_line_a_x_cnt <= MAX_TOUCH_LIMIT_FINE) {
        axis = ENUM_VER_Y;
        initialLineCnt = BS_initial_line_a_y_cnt;
    }
    else if (BS_initial_line_a_x_cnt < BS_initial_line_a_y_cnt && BS_initial_line_a_y_cnt <= MAX_TOUCH_LIMIT_FINE) {
        axis = ENUM_HOR_X;
        initialLineCnt = BS_initial_line_a_x_cnt;
    }
    else {
        return 0; //no-error
    }
    initialLineCnt0 = initialLineCnt;
    initialLineCntDiff = GET_ABS(BS_initial_line_a_x_cnt - BS_initial_line_a_y_cnt);

#if (DEBUG_fine_get_initial_ep > 1)
    IS_DEBUG_FLAG{
        TRACE_FGIE("***BS_fine_get_initial_ep5_cent...%d %d %d", axis, initialLineCnt, initialLineCntDiff);
        TRACE_NOP;
    };
#endif
    //TRACE("remLineSlopeIdx= %x %x", remLineSlopeIdx, &BS_rem_init_line_slope_idx[0]);
    if (remLineSlopeIdx == 0) {
        TRACE_NOP;
    }

    if (axis == ENUM_HOR_X) {
        //x-axis, remained line
        initial_line = &BS_initial_line_a_x[0];
        initialLineCntMax = MAX_INITIAL_LINE_FINE_X;
        remLine = &BS_remained_x[0];
        remLineCnt = BS_remained_x_cnt;
        remLineSlopeIdxCnt = BS_offsetTblLenX;
#ifdef FINE_MAX_SLOPE_FIRST
        slopeValMax = BS_slopeValMaxX;
#endif
    }
    else {
        //y-axis, remained line
        initial_line = &BS_initial_line_a_y[0];
        initialLineCntMax = MAX_INITIAL_LINE_FINE_Y;
        remLine = &BS_remained_y[0];
        remLineCnt = BS_remained_y_cnt;
        remLineSlopeIdxCnt = BS_offsetTblLenY;
#ifdef FINE_MAX_SLOPE_FIRST
        slopeValMax = BS_slopeValMaxY;
#endif
    }

#if 1 //nsmoon@201110
    if (remLineCnt > MAX_REMAINED_LINE) {
        TRACE_ERROR("ERROR! BS_fine_get_initial_ep5..invalid remLineCnt: %d", remLineCnt);
        remLineCnt = MAX_REMAINED_LINE;
        //FIXME, no-error
    }
#endif
#if 1 //remained line index by slopeVal
    int slopeVal, slopeIdx, remLineIdx;

    //init
    for (i = 0; i < BS_max_fine_rem_init_line; i++) {
        remLineSlopeIdx[i] = -1;
    }

    //////////////////////////////////////////////
    //find presentative line of each slope
    //remLineSlopeIdxCnt = slopeValMax * 2 + 1; //x-axis only
    //remLineIdxMinSlopeVal = -1;
    for (remLineIdx = 0; remLineIdx < remLineCnt; remLineIdx++)
    {
        tmpInBufIdx = remLine[remLineIdx];
        if (BS_getSenInBuf(axis, tmpInBufIdx, &pd0, &led0)) {
            //TRACE_ERROR("ERROR! BS_fine_get_initial_ep5..invalid inBufIdx: %d", tmpInBufIdx);
            continue;
        }
        slopeVal = led0 - pd0;
#if 0 //order-0: -10~0~+10
        slopeIdx = slopeValMax + slopeVal;
#endif
#ifdef FINE_MAX_SLOPE_FIRST //order-1: +10,-10~1,-1,0 (0530)
        if (slopeVal > 0) {
            slopeIdx = (slopeValMax - slopeVal) * 2;
        }
        else if (slopeVal < 0) {
            int tmpIdx = (slopeValMax + slopeVal + 1);
            slopeIdx = (tmpIdx * 2) - 1;
        }
        else {
            slopeIdx = slopeValMax * 2;
        }
#endif
#ifdef FINE_MIN_SLOPE_FIRST //order-2: 0,1,-1,~ (0613)
        if (slopeVal > 0) {
            slopeIdx = (slopeVal * 2) - 1;
        }
        else if (slopeVal < 0) {
            slopeIdx = (-slopeVal) * 2;
        }
        else {
            slopeIdx = 0;
        }
#endif
#if (DEBUG_fine_get_initial_ep > 2)
        IS_DEBUG_FLAG{ TRACE_FGIE("  ..slopeIdx:,%d,%d,%d,:,%d,%d", slopeIdx, slopeVal, tmpInBufIdx, led0, pd0); };
#endif
        if (slopeIdx < 0 || slopeIdx >= remLineSlopeIdxCnt) {
            TRACE_ERROR("ERROR! BS_fine_get_initial_ep5..invalid slopeIdx: %d", slopeIdx);
            //slopeIdx = 0;
            continue;
        }
        remLineSlopeIdx[slopeIdx] = (int16_t)remLineIdx;
        TRACE_NOP;
    } //for (remLineIdx = 0; remLineIdx < remLineCnt; remLineIdx++)
#endif //1

    stopLoop = 0;
    for (i = 0; i < remLineSlopeIdxCnt; i++) {
        if (remLineSlopeIdx[i] < 0) continue;
        remLineIdx0 = remLineSlopeIdx[i];
        tmpInBufIdx = remLine[remLineIdx0];
        if (!BS_getSenInBuf(axis, tmpInBufIdx, &pd0, &led0)) {
            TRACE_FGIE("  pd0=%d %d", pd0, led0);
            for (initLineIdx = 0; initLineIdx < initialLineCnt0; initLineIdx++) {
                pd1 = initial_line[initLineIdx].line.pd;
                led1 = initial_line[initLineIdx].line.led;
                if (pd0 == pd1 && led0 == led1) {
                    continue;
                }
                slopeVal0 = led0 - pd0;
                slopeVal1 = led1 - pd0;
                if (GET_ABS(slopeVal0) > GET_ABS(slopeVal1)) {
                    break;
                }
#if (DEBUG_fine_get_initial_ep > 0)
                TRACE_FGIE("  **pd0=%d %d", pd0, led0);
                IS_DEBUG_FLAG{ DEBUG_SHOW_LINE_PD_LED(axis, pd0, led0, MY_COLOR); };
#endif
                if (initialLineCnt < initialLineCntMax) {
                    initial_line[initialLineCnt].line.pd = pd0;
                    initial_line[initialLineCnt].line.led = led0;
                    initialLineCnt++;
                    initialLineCntDiff--;
                }
                else {
                    TRACE_ERROR("ERROR! BS_fine_get_initial_ep5_cent..invalid initialLineCnt %d %d", initialLineCnt, initialLineCntMax);
                    stopLoop = 1;
                    break;
                }
                if (initialLineCntDiff < 1) {
                    stopLoop = 1;
                    break;
                }
            }
            if (stopLoop) {
                break;
            }
        }
    }

    if (axis == ENUM_HOR_X) {
        BS_initial_line_a_x_cnt = initialLineCnt;
    }
    else {
        BS_initial_line_a_y_cnt = initialLineCnt;
    }
#if (DEBUG_fine_get_initial_ep > 1)
    IS_DEBUG_FLAG{ TRACE_FGIE("  initialLineCnt.2. %d %d", axis, initialLineCnt); };
#endif
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };

    return 0; //no-error
}
#endif

#if 1 //nsmoon@220124
static int is_cont_line_epaX(axis_t axis, int epaXCnt, uint8_t pd1, uint8_t led1)
{
    ep_no_t *epaX = &BS_edge_pattern_ep[0];
    uint8_t pd0, led0;
    int i;

    for (i = 0; i < epaXCnt; i++) {
        pd0 = epaX[i].e1;
        led0 = epaX[i].s1;
        if (is_cont_line(axis, pd0, led0, pd1, led1)) {
            return 1; //continuous
        }
    }

    return 0; //not-continuous
}

static int add_sen_epaX(int *epaXCnt0, uint8_t pd, uint8_t led)
{
    ep_no_t *epaX = &BS_edge_pattern_ep[0];
    int epaXCnt = *epaXCnt0;
#if 0 //(DEBUG_fine_get_initial_ep > 1)
    IS_DEBUG_FLAG{
        TRACE("..add_sen_epaX..(%d) %d/%d", epaXCnt, pd, led);
    };
#endif
#ifdef USE_CUST_MALLOC //nsmon@201012
    if (epaXCnt < (BS_max_ep_count / 2)) { //nsmoon@201014
#else
    if (epaXCnt < (MAX_EP_COUNT / 2)) {
#endif
        epaX[epaXCnt].e1 = pd;
        epaX[epaXCnt].s1 = led;
        epaX[epaXCnt].e2 = 0; //not crossed
        epaX[epaXCnt].s2 = 0; //continuous
        epaXCnt++;
        (*epaXCnt0) = epaXCnt;
        return 0; //no-error
    }
    else {
        TRACE_ERROR("ERROR! add_sen_epaX..invalid epaXCnt %d", epaXCnt);
        return 1; //error
    }
}

static int fine_get_initial_ep5_remained(axis_t axis, uint16_t *remLine, int *remLineCnt_in)
{
    int remLineIdx;
    initial_line_a_t *initial_line;
    int initialLineCnt, initialLineCntMax, initLineIdx;
    int epaXCnt, initialLineCntOrg;  //nsmoon@220126
    int pd1, led1;
    int tmpInBufIdx;
    int remLineCnt = *remLineCnt_in;
#if 0 //nsmoon@211119
    int slopeValMax;
#endif
    //ep_no_t *epaX = &BS_edge_pattern_ep[0];

    if (axis == ENUM_HOR_X) {
        //x-axis, remained line
        initial_line = &BS_initial_line_a_x[0];
#if 0 //nsmoon@211119
        slopeValMax = BS_slopeValMaxX;
#endif
        initialLineCnt = BS_initial_line_a_x_cnt;
        initialLineCntMax = MAX_INITIAL_LINE_FINE_X;
    }
    else {
        //y-axis, remained line
        initial_line = &BS_initial_line_a_y[0];
#if 0 //nsmoon@211119
        slopeValMax = BS_slopeValMaxY;
#endif
        initialLineCnt = BS_initial_line_a_y_cnt;
        initialLineCntMax = MAX_INITIAL_LINE_FINE_Y;
    }
    if (initialLineCnt >= MAX_TOUCH_LIMIT_FINE) { //nsmoon@230418
        return 0;
    }

    initialLineCntOrg = initialLineCnt;  //nsmoon@220126
#if (DEBUG_fine_get_initial_ep > 1)
    IS_DEBUG_FLAG{
        TRACE_FGIE("**fine_get_initial_ep5_remained...%d %d", axis, initialLineCnt);
        TRACE_NOP;
    };
#endif
#if 1 //nsmoon@201110
    if (remLineCnt > MAX_REMAINED_LINE) {
        TRACE_ERROR("ERROR! fine_get_initial_ep5_remained..invalid remLineCnt: %d", remLineCnt);
        remLineCnt = MAX_REMAINED_LINE;
        //FIXME, no-error
    }
#endif

    /////////////////////////////////////
    //get initial line
    epaXCnt = 0;
    for (initLineIdx = 0; initLineIdx < initialLineCnt; initLineIdx++) {
        pd1 = initial_line[initLineIdx].line.pd;
        led1 = initial_line[initLineIdx].line.led;
        if (add_sen_epaX(&epaXCnt, pd1, led1)) {
            break;
        }
    }

    /////////////////////////////////////
    //find uncrossed line
    for (remLineIdx = 0; remLineIdx < remLineCnt; remLineIdx++)
    {
        tmpInBufIdx = remLine[remLineIdx];
        if (!BS_getSenInBuf(axis, tmpInBufIdx, &pd1, &led1)) {
#if 0 //for test
            IS_DEBUG_FLAG{
                if (axis == ENUM_VER_Y) {
                    if (pd1 == 24 && led1 == 34) {
                        TRACE_NOP;
                    }
                }
            };
#endif
            int ret = isCrossOtheLineX(epaXCnt, pd1, led1);
            if (ret == 0) {
                if (is_cont_line_epaX(axis, epaXCnt, pd1, led1) == 0) {
                    //unclossed
                    if (initialLineCnt < initialLineCntMax) {
                        initial_line[initialLineCnt].line.pd = pd1;
                        initial_line[initialLineCnt].line.led = led1;
        #if (DEBUG_fine_get_initial_ep > 0)
                        IS_DEBUG_FLAG{ DEBUG_SHOW_LINE_PD_LED(axis, pd1, led1, 0); }; //initial line
        #elif defined(INITIAL_LINE_TEST_MULTI)
                        IS_DEBUG_FLAG{ DEBUG_SHOW_LINE_PD_LED(axis, pd1, led1, 0); }; //initial line
        #endif
                        initialLineCnt++;
                        add_sen_epaX(&epaXCnt, pd1, led1);
                    }
                    else {
                        TRACE_ERROR("ERROR! fine_get_initial_ep5_remained..invalid initialLineCnt %d %d", initialLineCnt, initialLineCntMax);
                        break;
                    }
                }
            }
        }
        else {
                TRACE_ERROR("ERROR! fine_get_initial_ep5_remained..invalid inBufIdx: %d", tmpInBufIdx);
                continue;
        }
    } //for (remLineIdx = 0; remLineIdx < remLineCnt; remLineIdx++)

    if (axis == ENUM_HOR_X) {
        BS_initial_line_a_x_cnt = initialLineCnt;
    }
    else {
        BS_initial_line_a_y_cnt = initialLineCnt;
    }
#if (DEBUG_fine_get_initial_ep > 1)
    IS_DEBUG_FLAG{ TRACE_FGIE("  ..initialLineCnt: %d", initialLineCnt); };
#endif
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };

    *remLineCnt_in = remLineCnt; //save
    return (initialLineCnt - initialLineCntOrg); //no-error //nsmoon@220126
}
#endif

#if defined(DEBUG_FUNCTION_ENABLE_ALL)
#define DEBUG_fine_get_inst     1
#endif
#if (DEBUG_fine_get_inst > 0)
#define TRACE_FGI0(...)   TRACE(__VA_ARGS__)
#else
#define TRACE_FGI0(...)
#endif
#if (DEBUG_fine_get_inst > 1)
#define TRACE_FGI(...)   TRACE(__VA_ARGS__)
#else
#define TRACE_FGI(...)
#endif

static int fine_get_clipping_info(axis_t axis)
{
    clipping_range_info_t *clipping_info = &BS_clipping_range_info[(int)axis];
    //bd_type_t bd, bd2;
    //sen_type_t sensor, sensor2;
    float *senPos2 = 0; //opp sensor
    float *senPosX = &BS_pd_pos_x[0];
    float *senPosY = &BS_pd_pos_y[0];

    if (axis == ENUM_HOR_X) {
        senPos2 = &BS_led_pos_x[0];
    }
    else {
        senPos2 = &BS_led_pos_y[0];
    }

    clipping_info->bd = ENUM_TOP_BD;
    clipping_info->bd2 = ENUM_TOP_BD;
    clipping_info->sensor = ENUM_PD;
    clipping_info->sensor2 = ENUM_PD; //opp axis pos
    clipping_info->senPosX = senPosX;
    clipping_info->senPosY = senPosY;
    clipping_info->senPos2 = senPos2; //opp sensor pos
    clipping_info->sideIdx = 0;
    clipping_info->polyIdx = 0;

    return 0;
}

static int fine_get_inst5(axis_t axis1, initial_line_a_t *initial_line, int initialLineIdx, int mode, axis_t axis2, uint16_t *remLine2, int remLineCnt2, axis_t axis3, int calMode)
{
    int tmpInBufIdx;
    int pd, led;
    vec_t p0, p1, p2, p3, pR;
    int instCnt, sortCnt, instCurCnt;
    int remLineIdx;
    float minPos2 = 0, maxPos2 = 0; //cent;
    int cxpIdx, cxpSt, cxpEd;
    ep_min_max_t minMaxEp;
    pos_min_max_t minMaxPos = {0, 0, 0, 0};
    int remSt, remEd;
    int senStOp, senEdOp;
    float instRange;
    int i, j;
    //vec_t *inst_a[MAX_FINE_INITIAL_CXP_PER_LINE];
    //uint8_t *sort_a[MAX_FINE_INITIAL_CXP_PER_LINE];
    //in_line_t *lineSen_a[MAX_FINE_INITIAL_CXP_PER_LINE];
    vec_t *inst_a; //nsmoon@211125
    uint8_t *sort_a;
    in_line_t *lineSen_a;

#if 0 //nsmoon@230412 not-used
    if (initialLineIdx >= MAX_INITIAL_LINE_FINE_Y) {
        TRACE_ERROR("ERROR! invalid initialLineIdx:(%d) %d / %d", axis1, initialLineIdx, initialLineIdxMax);
        return FINE_MEM_ERROR; //mem-error
    }
#endif
    IS_DEBUG_FLAG{
        //SEND_POLY(0, 0x21, 0);
        TRACE_NOP;
    };

    //get cxp range
    if (calMode == FINE_CXP_CAL_INIT) {
        if (axis1 == ENUM_HOR_X) {
            cxpSt = 0;
            cxpEd = initial_line[initialLineIdx].cxpCntMax;
            if (cxpEd > MAX_FINE_INITIAL_CXP_PER_LINE) {
                TRACE_ERROR("ERROR! invalid cxpEd %d", cxpEd);
                cxpEd = MAX_FINE_INITIAL_CXP_PER_LINE;
            }
        }
        else {
            TRACE_ERROR("ERROR! invald calMode,axis1: %d %d", calMode, axis1);
            return FINE_OUT_OF_INST;
        }
    }
    else {
        //(calMode == FINE_CXP_CAL_NORM)
        cxpSt = FINE_CXP_ORI; //0
        cxpEd = FINE_CXP_ORI + 1; //1
    }

    //get inst range
    if (calMode == FINE_CXP_CAL_INIT) {
        if (axis3 == ENUM_HOR_X) {
            instRange = REMAINED_FINE_INITIAL_RANGE_X;
        }
        else {
            instRange = REMAINED_FINE_INITIAL_RANGE_Y;
        }
    }
    else {
        //(calMode == FINE_CXP_CAL_NORM)
        if (axis3 == ENUM_HOR_X) {
            instRange = REMAINED_FINE_NORMAL_RANGE_X;
        }
        else {
            instRange = REMAINED_FINE_NORMAL_RANGE_Y;
        }
    }

    //get p0 and p1 lines for inst
    //if (initial_line[initialLineIdx].instLen < 0) {
    //    return FINE_OUT_OF_INST; //out-of-inst
    //}
    if (mode == FINE_USE_INITIAL_LINE) {
        //using initial line
        pd = initial_line[initialLineIdx].line.pd;
        led = initial_line[initialLineIdx].line.led;
        if (axis1 == ENUM_HOR_X) {
            p0.x = BS_led_pos_x[led];
            p0.y = BS_sensor_zero_y;
            p1.x = BS_pd_pos_x[pd];
            p1.y = BS_sensor_end_y;
        }
        else {
            p0.x = BS_sensor_end_x;
            p0.y = BS_led_pos_y[led];
            p1.x = BS_sensor_zero_x;
            p1.y = BS_pd_pos_y[pd];
        }
    }
    else {
        //(mode == FINE_USE_CENTER_LINE)
        //using center line
        if (axis1 == ENUM_HOR_X) {
            p0.x = initial_line[initialLineIdx].cent.x;
            p0.y = initial_line[initialLineIdx].cent.y - FINE_INST_Y_DISPLAY;
            p1.x = initial_line[initialLineIdx].cent.x;
            p1.y = initial_line[initialLineIdx].cent.y + FINE_INST_Y_DISPLAY;
        }
        else {
            p0.x = initial_line[initialLineIdx].cent.x - FINE_INST_Y_DISPLAY;
            p0.y = initial_line[initialLineIdx].cent.y;
            p1.x = initial_line[initialLineIdx].cent.x + FINE_INST_Y_DISPLAY;
            p1.y = initial_line[initialLineIdx].cent.y;
        }
    }
#if 0 //(DEBUG_fine_get_inst > 0)
    if (calMode == FINE_CXP_CAL_NORM) {
        DEBUG_SHOW_LINE_POS(&p0, &p1, initialLineIdx);
    }
#endif

    //make inst range for FINE_USE_CENTER_LINE
    if (mode == FINE_USE_CENTER_LINE) {
        //cent of opp line
        if (axis3 == ENUM_HOR_X) {
            minPos2 = initial_line[initialLineIdx].cent.x - instRange - EPSILON;
            maxPos2 = initial_line[initialLineIdx].cent.x + instRange + EPSILON;
            minMaxPos.minY = initial_line[initialLineIdx].cent.y;
            minMaxPos.maxY = minMaxPos.minY;
            minMaxPos.minX = minPos2;
            minMaxPos.maxX = maxPos2;
        }
        else {
            //(axis3 == ENUM_VER_Y)
            minPos2 = initial_line[initialLineIdx].cent.y - instRange - EPSILON;
            maxPos2 = initial_line[initialLineIdx].cent.y + instRange + EPSILON;
            minMaxPos.minX = initial_line[initialLineIdx].cent.x;
            minMaxPos.maxX = minMaxPos.minX;
            minMaxPos.minY = minPos2;
            minMaxPos.maxY = maxPos2;
        }
#if 0 //(DEBUG_fine_get_inst > 0) || defined(FINE_RULE_1A_TEST)
        if (calMode == FINE_CXP_CAL_NORM) {
            if (axis3 == ENUM_HOR_X) {
                p2.x = minPos2;
                p2.y = initial_line[initialLineIdx].cent.y - instRange - EPSILON;
                p3.x = minPos2;
                p3.y = initial_line[initialLineIdx].cent.y + instRange + EPSILON;
                DEBUG_SHOW_LINE_POS(&p2, &p3, initialLineIdx /*MY_COLOR*/);
                p2.x = maxPos2;
                p2.y = initial_line[initialLineIdx].cent.y - instRange - EPSILON;
                p3.x = maxPos2;
                p3.y = initial_line[initialLineIdx].cent.y + instRange + EPSILON;
                DEBUG_SHOW_LINE_POS(&p2, &p3, initialLineIdx /*MY_COLOR*/);
            }
            else {
                //(axis3 == ENUM_VER_Y)
                p2.y = minPos2;
                p2.x = initial_line[initialLineIdx].cent.x - instRange - EPSILON;
                p3.y = minPos2;
                p3.x = initial_line[initialLineIdx].cent.x + instRange + EPSILON;
                DEBUG_SHOW_LINE_POS(&p2, &p3, initialLineIdx /*MY_COLOR*/);
                p2.y = maxPos2;
                p2.x = initial_line[initialLineIdx].cent.x - instRange - EPSILON;
                p3.y = maxPos2;
                p3.x = initial_line[initialLineIdx].cent.x + instRange + EPSILON;
                DEBUG_SHOW_LINE_POS(&p2, &p3, initialLineIdx /*MY_COLOR*/);
            }
        }
#endif
    }

    //init min/max index of group
    for (cxpIdx = cxpSt; cxpIdx < cxpEd; cxpIdx++)
    {
        //assign inst buf
        initial_line[initialLineIdx].inst_a[cxpIdx] = &BS_inst_xy[BS_inst_xy_cnt];
        initial_line[initialLineIdx].sort_a[cxpIdx] = &BS_inst_xy_sort[BS_inst_xy_cnt];
        initial_line[initialLineIdx].lineSen_a[cxpIdx] = &BS_inst_xy_sen[BS_inst_xy_cnt];
        initial_line[initialLineIdx].sortLen_a[cxpIdx] = 0; //init
        initial_line[initialLineIdx].instLen_a[cxpIdx] = 0; //init //nsmoon@220127
        //inst_a[cxpIdx] = initial_line[initialLineIdx].inst_a[cxpIdx];
        //sort_a[cxpIdx] = initial_line[initialLineIdx].sort_a[cxpIdx];
        //lineSen_a[cxpIdx] = initial_line[initialLineIdx].lineSen_a[cxpIdx];
        inst_a = initial_line[initialLineIdx].inst_a[cxpIdx]; //nsmoon@211125
        sort_a = initial_line[initialLineIdx].sort_a[cxpIdx];
        lineSen_a = initial_line[initialLineIdx].lineSen_a[cxpIdx];
        TRACE_FGI("fine_get_inst5...: (%d/%d) %d-%d %d %d %x %x", axis1, calMode, initialLineIdx, cxpIdx, BS_inst_xy_cnt, remLineCnt2, inst_a, &BS_inst_xy[BS_inst_xy_cnt]);

        //make inst range for FINE_USE_INITIAL_LINE
        if (mode == FINE_USE_INITIAL_LINE) {
            if (axis3 == ENUM_HOR_X) {
                minPos2 = initial_line[initialLineIdx].cxpCent[cxpIdx].x - instRange - EPSILON;
                maxPos2 = initial_line[initialLineIdx].cxpCent[cxpIdx].x + instRange + EPSILON;
                minMaxPos.minY = initial_line[initialLineIdx].cxpCent[cxpIdx].y;
                minMaxPos.maxY = minMaxPos.minY;
                minMaxPos.minX = minPos2;
                minMaxPos.maxX = maxPos2;
            }
            else {
                //(axis3 == ENUM_VER_Y)
                minPos2 = initial_line[initialLineIdx].cxpCent[cxpIdx].y - instRange - EPSILON;
                maxPos2 = initial_line[initialLineIdx].cxpCent[cxpIdx].y + instRange + EPSILON;
                minMaxPos.minX = initial_line[initialLineIdx].cxpCent[cxpIdx].x;
                minMaxPos.maxX = minMaxPos.minX;
                minMaxPos.minY = minPos2;
                minMaxPos.maxY = maxPos2;
            }
#if 0 //(DEBUG_fine_get_inst > 0) || defined(FINE_RULE_1A_TEST)
            if (calMode == FINE_CXP_CAL_NORM) {
                if (axis3 == ENUM_HOR_X) {
                    p2.x = minPos2;
                    p2.y = initial_line[initialLineIdx].cxpCent[cxpIdx].y - instRange;
                    p3.x = minPos2;
                    p3.y = initial_line[initialLineIdx].cxpCent[cxpIdx].y + instRange;
                    DEBUG_SHOW_LINE_POS(&p2, &p3, initialLineIdx /*MY_COLOR*/);
                    p2.x = maxPos2;
                    p2.y = initial_line[initialLineIdx].cxpCent[cxpIdx].y - instRange;
                    p3.x = maxPos2;
                    p3.y = initial_line[initialLineIdx].cxpCent[cxpIdx].y + instRange;
                    DEBUG_SHOW_LINE_POS(&p2, &p3, initialLineIdx /*MY_COLOR*/);
                }
                else {
                    p2.y = minPos2;
                    p2.x = initial_line[initialLineIdx].cxpCent[cxpIdx].x - instRange;
                    p3.y = minPos2;
                    p3.x = initial_line[initialLineIdx].cxpCent[cxpIdx].x + instRange;
                    DEBUG_SHOW_LINE_POS(&p2, &p3, initialLineIdx /*MY_COLOR*/);
                    p2.y = maxPos2;
                    p2.x = initial_line[initialLineIdx].cxpCent[cxpIdx].x - instRange;
                    p3.y = maxPos2;
                    p3.x = initial_line[initialLineIdx].cxpCent[cxpIdx].x + instRange;
                    DEBUG_SHOW_LINE_POS(&p2, &p3, initialLineIdx /*MY_COLOR*/);
                }
            }
#endif
        }

        minMaxEp.minY = (uint8_t)BS_getSenNumPos(axis3, ENUM_PD, minMaxPos.minY, 0); //0:before
        minMaxEp.maxY = (uint8_t)BS_getSenNumPos(axis3, ENUM_PD, minMaxPos.maxY, 1); //1:after
        minMaxEp.minX = (uint8_t)BS_getSenNumPos(axis3, ENUM_PD, minMaxPos.minX, 0); //0:before
        minMaxEp.maxX = (uint8_t)BS_getSenNumPos(axis3, ENUM_PD, minMaxPos.maxX, 1); //1:after

        int minSenlim, maxSenlim;
        BS_get_clipping_range(axis3, &minMaxEp, &minMaxPos, &minSenlim, &maxSenlim);
#if (DEBUG_fine_get_inst > 1)
        IS_DEBUG_FLAG{
            int center = GET_LINE_CENT_INT(minSenlim, maxSenlim);
            TRACE("center,minSenlim,maxSenlim: %d (%d %d)", center, minSenlim, maxSenlim);
        };
#endif

        if (calMode == FINE_CXP_CAL_INIT) {
            remSt = 0;
            remEd = remLineCnt2;
        }
        else {
            //(calMode == FINE_CXP_CAL_NORM)
            remSt = minSenlim;
            remEd = maxSenlim;
        }
		instCnt = instCurCnt = 0; //reset

        for (remLineIdx = remSt; remLineIdx < remEd; remLineIdx++)
        {
            pd = 0, led = 0;
            //get p2 and p3 lines for inst
            if (calMode == FINE_CXP_CAL_INIT) {
                tmpInBufIdx = remLine2[remLineIdx];
                if (BS_getSenInBuf(axis2, tmpInBufIdx, &pd, &led) < 0) {
                    TRACE_ERROR("ERROR! DEBUG_show_line_idx_..BS_getSenInBuf() [%d]", BG_frame_no);
                    continue;
                }
#if 0 //(DEBUG_fine_get_inst > 1)
                if (BS_is_set_line_buffer(axis2, pd, led)) {
                    TRACE_NOP;
                }
#endif
                senStOp = BS_get_virtual_range(axis3, ENUM_PD, &minMaxEp, &minMaxPos, pd, 0); //start
                senEdOp = BS_get_virtual_range(axis3, ENUM_PD, &minMaxEp, &minMaxPos, pd, 1); //end
                if (led < senStOp || led > senEdOp) continue;
                if (BS_getLinePosInBuf(axis2, tmpInBufIdx, &p2, &p3)) {
                    TRACE_ERROR("ERROR! fine_get_inst..BS_getLinePosInBuf() [%d]", BG_frame_no);
                    break;
                }
                if (BS_is_set_line_buffer(axis3, pd, led)) {
                    instCurCnt++;
                }
				senStOp = senEdOp = 0;
            }
            else {
                //(calMode == FINE_CXP_CAL_NORM)
                int slopeVal, adjVal;
                int maxSlopeVal = (axis3 == ENUM_HOR_X) ? BS_slopeValMaxX : BS_slopeValMaxY;
                senStOp = BS_get_virtual_range(axis3, ENUM_PD, &minMaxEp, &minMaxPos, remLineIdx, 0); //start
                //TRACE("senStOp=%d-%d", senStOp, remLineIdx);
                slopeVal = GET_ABS(senStOp - remLineIdx);
                if (slopeVal > maxSlopeVal) {
                    adjVal = slopeVal - maxSlopeVal;
                    senStOp = (senStOp > remLineIdx) ? senStOp - adjVal : senStOp + adjVal;
                }

                senEdOp = BS_get_virtual_range(axis3, ENUM_PD, &minMaxEp, &minMaxPos, remLineIdx, 1); //end
                //TRACE("senEdOp=%d-%d", senEdOp, remLineIdx);
                slopeVal = GET_ABS(senEdOp - remLineIdx);
                if (slopeVal > maxSlopeVal) {
                    adjVal = slopeVal - maxSlopeVal;
                    senEdOp = (senEdOp > remLineIdx) ? senEdOp - adjVal : senEdOp + adjVal;
                }
#if 0 //(DEBUG_fine_get_inst > 1)
                //TRACE("=>senStOp=%d-%d (%d)", senStOp, remLineIdx, senStOp - remLineIdx);
                slopeVal = GET_ABS(senStOp - remLineIdx);
                if (slopeVal > maxSlopeVal) {
                    DEBUG_SHOW_LINE_PD_LED(axis3, remLineIdx, senStOp, MY_COLOR);
                    TRACE_NOP;
                }
                //TRACE("=>senEdOp=%d-%d (%d)", senEdOp, remLineIdx, senEdOp - remLineIdx);
                slopeVal = GET_ABS(senEdOp - remLineIdx);
                if (slopeVal > maxSlopeVal) {
                    DEBUG_SHOW_LINE_PD_LED(axis3, remLineIdx, senEdOp, MY_COLOR);
                    TRACE_NOP;
                }
#endif
                //if (remLineIdx == 83) TRACE("axis1,pd,led=(%d) %d %d-%d", axis1, remLineIdx, senStOp, senEdOp);
                TRACE_NOP;
            }

            int senIdx;
			for (senIdx = senStOp; senIdx <= senEdOp; senIdx++) 
            {
                //get p2 and p3 lines for inst
                if (calMode == FINE_CXP_CAL_NORM) {
                    if (axis3 == ENUM_HOR_X) {
                        p2.x = BS_pd_pos_x[remLineIdx]; //pd
                        p2.y = BS_sensor_end_y;
                        p3.x = BS_led_pos_x[senIdx]; //led
                        p3.y = BS_sensor_zero_y;
                    }
                    else {
                        p2.y = BS_pd_pos_y[remLineIdx];
                        p2.x = BS_sensor_zero_x;
                        p3.y = BS_led_pos_y[senIdx];
                        p3.x = BS_sensor_end_x;
                    }
                }
#if 0 //(DEBUG_fine_get_inst > 1)
                if (calMode == FINE_CXP_CAL_NORM) {
                    if (BS_is_set_line_buffer(axis2, remLineIdx, senIdx)) {
                        //TRACE("LED-PD=%d-%d", senIdx, remLineIdx);
                        //DEBUG_SHOW_LINE_POS(&p2, &p3, (MY_COLOR - 4)/*initialLineIdx*/);
                    }
                    else {
                        //TRACE("led-pd=%d-%d", senIdx, remLineIdx);
                        DEBUG_SHOW_LINE_POS(&p2, &p3, (MY_COLOR - 4)/*initialLineIdx*/);
                    }
                    TRACE_NOP;
                }
#endif
                pR.x = -1; pR.y = -1;
                if (BS_line_intersection(&p0, &p1, &p2, &p3, &pR)) {
#if 0 //(DEBUG_fine_get_inst > 0)
                    int cxLineIdx = initial_line[initialLineIdx].cxLineIdx;
                    DEBUG_SHOW_POS(&pR, 0.1f, 0.1f, cxLineIdx/*MY_COLOR - 4*/); //gray
                    if (axis1 == ENUM_HOR_X) {
                        DEBUG_SHOW_POS(&pR, 1.05f, 1.05f, MY_COLOR - 2);
                    }
#endif
                    if ((BS_inst_xy_cnt + instCnt) < MAX_NUM_INST_FINE) { //nsmoon@181218z
                        if (axis3 == ENUM_HOR_X) {
                            if (pR.x > minPos2 && pR.x < maxPos2) {
#if (defined(DEBUG_FUNCTION_ENABLE_ALL) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)) && defined(DRAW_POLYGON_TEST)
                                int cxLineIdx = initial_line[initialLineIdx].cxLineIdx;
                                if (calMode == FINE_CXP_CAL_INIT) {
                                    DEBUG_SHOW_POS(&pR, 0.05f, 0.05f, MY_COLOR - 4); //gray
                                }
                                else {
                                    DEBUG_SHOW_POS(&pR, 0.05f, 0.05f, cxLineIdx);
                                }
#endif
                                if (instCnt < UINT8_SIZE) {
                                    //inst_a[cxpIdx][instCnt] = pR;
                                    inst_a[instCnt] = pR; //nsmoon@211125
                                    if (calMode == FINE_CXP_CAL_NORM) {
                                        //lineSen_a[cxpIdx][instCnt].pd = (uint8_t)remLineIdx;
                                        //lineSen_a[cxpIdx][instCnt].led = (uint8_t)senIdx;
                                        lineSen_a[instCnt].pd = (uint8_t)remLineIdx; //nsmoon@211125
                                        lineSen_a[instCnt].led = (uint8_t)senIdx;
                                    }
                                    else {
                                        //(calMode == FINE_CXP_CAL_INIT)
                                        //lineStat_a[cxpIdx][instCnt] = remLineIdx;
                                        //lineSen_a[cxpIdx][instCnt].led = (uint8_t)led;
                                        //lineSen_a[cxpIdx][instCnt].pd = (uint8_t)pd;
                                        lineSen_a[instCnt].led = (uint8_t)led; //nsmoon@211125
                                        lineSen_a[instCnt].pd = (uint8_t)pd;
                                    }
                                    instCnt++;
                                }
                                else {
                                    TRACE_ERROR("ERROR! fine_get_inst5..invalid instCnt %d", instCnt);
                                }
                            }
                        }
                        else {
                            if (pR.y > minPos2 && pR.y < maxPos2) {
#if (defined(DEBUG_FUNCTION_ENABLE_ALL) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)) && defined(DRAW_POLYGON_TEST)
                                int cxLineIdx = initial_line[initialLineIdx].cxLineIdx;
                                if (calMode == FINE_CXP_CAL_INIT) {
                                    DEBUG_SHOW_POS(&pR, 0.05f, 0.05f, MY_COLOR - 4); //gray
                                }
                                else {
                                    DEBUG_SHOW_POS(&pR, 0.05f, 0.05f, cxLineIdx);
                                }
#endif
                                if (instCnt < UINT8_SIZE) {
                                    //inst_a[cxpIdx][instCnt] = pR;
                                    inst_a[instCnt] = pR; //nsmoon@211125
                                    if (calMode == FINE_CXP_CAL_NORM) {
                                        //lineSen_a[cxpIdx][instCnt].pd = (uint8_t)remLineIdx;
                                        //lineSen_a[cxpIdx][instCnt].led = (uint8_t)senIdx;
                                        lineSen_a[instCnt].pd = (uint8_t)remLineIdx; //nsmoon@211125
                                        lineSen_a[instCnt].led = (uint8_t)senIdx;
                                    }
                                    else {
                                        //(calMode == FINE_CXP_CAL_INIT)
                                        //lineStat_a[cxpIdx][instCnt] = remLineIdx;
                                        //lineSen_a[cxpIdx][instCnt].led = (uint8_t)led;
                                        //lineSen_a[cxpIdx][instCnt].pd = (uint8_t)pd;
                                        lineSen_a[instCnt].led = (uint8_t)led; //nsmoon@211125
                                        lineSen_a[instCnt].pd = (uint8_t)pd;
                                    }
                                    instCnt++;
                                }
                                else {
                                    TRACE_ERROR("ERROR! fine_get_inst5..invalid instCnt %d", instCnt);
                                }
                            }
                        }
                    }
                    else {
                        TRACE_ERROR_MEM("ERROR_MEM! fine_get_inst..invalid instCnt: (%d)%d+%d %d", axis1, BS_inst_xy_cnt, instCnt, MAX_NUM_INST_FINE);
                        return FINE_MEM_ERROR; //mem-error
                    }
#if 0 //for testing
                    if (axis1 == ENUM_VER_Y) {
                        TRACE("inst[].x: %d %0.1f", instCnt - 1, inst[instCnt - 1].x);
                    }
#endif
                }
                else {
                    //TRACE_ERROR("ERROR! fine_get_inst..no inst");
                    TRACE_NOP;
                }
            } //for (senIdx = senStOp; senIdx < senEdOp; senIdx++)
        } //for (remLineIdx = 0; remLineIdx < remLineCnt; remLineIdx++)

        //save instLen
        initial_line[initialLineIdx].instLen_a[cxpIdx] = instCnt; //num of instersect

        int isEdgeArea = 0;
#if 1 //nsmoon@200922
        if (instCnt >= FINE_INITIAL_GRP_MIN_NUM_EDGE) {
            //int edgeX = (BS_is_edge(ENUM_HOR_X, inst_a[cxpIdx][0].x) | BS_is_edge(ENUM_HOR_X, inst_a[cxpIdx][instCnt - 1].x));
            //int edgeY = (BS_is_edge(ENUM_VER_Y, inst_a[cxpIdx][0].y) | BS_is_edge(ENUM_VER_Y, inst_a[cxpIdx][instCnt - 1].y));
            int edgeX = (BS_is_edge(ENUM_HOR_X, inst_a[0].x) | BS_is_edge(ENUM_HOR_X, inst_a[instCnt - 1].x)); //nsmoon@211125
            int edgeY = (BS_is_edge(ENUM_VER_Y, inst_a[0].y) | BS_is_edge(ENUM_VER_Y, inst_a[instCnt - 1].y));
            isEdgeArea = edgeX | edgeY;
            //IS_DEBUG_FLAG{ TRACE_FGI("...isEdgeArea1: (%d/%d) %04x (%0.1f,%0.1f)(%0.1f,%0.1f)", initialLineIdx, cxpIdx, isEdgeArea, inst_a[cxpIdx][0].x, inst_a[cxpIdx][0].y, inst_a[cxpIdx][instCnt - 1].x, inst_a[cxpIdx][instCnt - 1].y);};
            IS_DEBUG_FLAG{ TRACE_FGI("...isEdgeArea1: (%d/%d) %04x (%0.1f,%0.1f)(%0.1f,%0.1f)", initialLineIdx, cxpIdx, isEdgeArea, inst_a[0].x, inst_a[0].y, inst_a[instCnt - 1].x, inst_a[instCnt - 1].y);}; //nsmoon@211125
        }
        initial_line[initialLineIdx].isEdgeArea[cxpIdx] = (uint16_t)isEdgeArea;
#endif
#if 0
        //remove ghost
        if ((isEdgeArea == 0 && instCnt < FINE_INITIAL_GRP_MIN_NUM) ||
            (isEdgeArea && instCnt < FINE_INITIAL_GRP_MIN_NUM_EDGE)) {
            continue;
        }
#endif

        ///////////////////////////////////////
        //make sort table
        int instIdx_i, instIdx_j;
        float inst_i, inst_j;

        sortCnt = 0;
        for (i = 0; i < instCnt; i++) {
            //sort_a[cxpIdx][sortCnt++] = (uint8_t)i; //nsmoon@190527
            sort_a[sortCnt++] = (uint8_t)i; //nsmoon@211125
        }
        //save sortCnt
        initial_line[initialLineIdx].sortLen_a[cxpIdx] = (uint16_t)sortCnt; //num of instersect
        IS_DEBUG_FLAG{ TRACE_FGI("&initial_line[initialLineIdx].sortLen_a[cxpIdx]=%x %d %d %d", &initial_line[initialLineIdx].sortLen_a[cxpIdx], initialLineIdx, cxpIdx, initial_line[initialLineIdx].sortLen_a[cxpIdx]);}; //nsmoon@211125
        IS_DEBUG_FLAG{
            if (sort_a != initial_line[initialLineIdx].sort_a[cxpIdx]) {
                TRACE_ERROR("ERROR! invalid sort_a 0x%x 0x%x", sort_a, initial_line[initialLineIdx].sort_a[cxpIdx]);
            }
            if (inst_a != initial_line[initialLineIdx].inst_a[cxpIdx]) {
                TRACE_ERROR("ERROR! invalid inst_a 0x%x 0x%x", sort_a, initial_line[initialLineIdx].inst_a[cxpIdx]);
            }
        }
#if 0 //(DEBUG_fine_get_inst > 1) //for testing
        for (i = 0; i < sortCnt; i++) {
            /TRACENR("%d ", sort_a[cxpIdx][i]);
            TRACENR("%d ", sort_a[i]); //nsmoon@211125
            if ((i + 1) % 20 == 0) TRACENR("\r\n");
        }
        TRACE("=>before(%d, %d)", axis2, initialLineIdx);
        for (i = 0; i < sortCnt; i++) {
            //inst_i = (axis2 == ENUM_HOR_X) ? inst_a[cxpIdx][sort[i]].x : inst_a[cxpIdx][sort[i]].y;
            inst_i = (axis2 == ENUM_HOR_X) ? inst_a[sort[i]].x : inst_a[sort[i]].y; //nsmoon@211125
            TRACENR("%0.1f ", inst_i);
            if ((i + 1) % 20 == 0) TRACENR("\r\n");
        }
        TRACE("=>before(%d, %d)", axis2, initialLineIdx);
#endif

        //for (i = 0; i < instCnt - 1; i++)
        for (i = 0; i < sortCnt - 1; i++)
        {
            //for (j = (i + 1); j < instCnt; j++)
            for (j = (i + 1); j < sortCnt; j++)
            {
                //instIdx_i = sort_a[cxpIdx][i];
                //instIdx_j = sort_a[cxpIdx][j];
                instIdx_i = sort_a[i]; //nsmoon@211125
                instIdx_j = sort_a[j];
                //inst_i = (axis3 == ENUM_HOR_X) ? inst_a[cxpIdx][instIdx_i].x : inst_a[cxpIdx][instIdx_i].y;
                //inst_j = (axis3 == ENUM_HOR_X) ? inst_a[cxpIdx][instIdx_j].x : inst_a[cxpIdx][instIdx_j].y;
                inst_i = (axis3 == ENUM_HOR_X) ? inst_a[instIdx_i].x : inst_a[instIdx_i].y; //nsmoon@211125
                inst_j = (axis3 == ENUM_HOR_X) ? inst_a[instIdx_j].x : inst_a[instIdx_j].y;
                if (inst_i > inst_j)
                {
                    //sort_a[cxpIdx][i] = (uint8_t)instIdx_j;
                    //sort_a[cxpIdx][j] = (uint8_t)instIdx_i;
                    sort_a[i] = (uint8_t)instIdx_j; //nsmoon@211125
                    sort_a[j] = (uint8_t)instIdx_i;
                }
            }
        }
#if 0 //(DEBUG_fine_get_inst > 1) //for testing
        IS_DEBUG_FLAG
        {
            for (i = 0; i < sortCnt; i++) {
                //TRACENR("%d ", sort_a[cxpIdx][i]);
                TRACENR("%d ", sort_a[i]); //nsmoon@211125
                if ((i + 1) % 20 == 0) TRACENR("\r\n");
            }
            TRACE("=>after idx(%d, %d, %d)", axis3, initialLineIdx, sortCnt);
            for (i = 0; i < sortCnt; i++) {
                //inst_i = (axis3 == ENUM_HOR_X) ? inst_a[cxpIdx][sort_a[cxpIdx][i]].x : inst[sort_a[cxpIdx][i]].y;
                inst_i = (axis3 == ENUM_HOR_X) ? inst_a[sort_a[i]].x : inst[sort_a[i]].y; //nsmoon@211125
                TRACENR("%0.1f ", inst_i);
                if ((i + 1) % 20 == 0) TRACENR("\r\n");
            }
            TRACE("=>after pos(%d, %d, %d)", axis3, initialLineIdx, sortCnt);
        };
#endif
        TRACE_FGI("...instCurCnt,instCnt,sortCnt: %d %d %d", instCurCnt, instCnt, sortCnt);
        TRACE_FGI("initial_line[].inst:3: %d %d %x %x", initialLineIdx, instCnt, inst_a, initial_line[initialLineIdx].inst_a[cxpIdx]);
        IS_DEBUG_FLAG{
            TRACE_NOP;
        };
        IS_DEBUG_FLAG{
            if (sort_a != initial_line[initialLineIdx].sort_a[cxpIdx]) {
                TRACE_ERROR("ERROR! invalid sort_a 0x%x 0x%x", sort_a, initial_line[initialLineIdx].sort_a[cxpIdx]);
            }
            if (inst_a != initial_line[initialLineIdx].inst_a[cxpIdx]) {
                TRACE_ERROR("ERROR! invalid inst_a 0x%x 0x%x", sort_a, initial_line[initialLineIdx].inst_a[cxpIdx]);
            }
        }
        if ((BS_inst_xy_cnt + instCnt) < MAX_NUM_INST_FINE) {
            BS_inst_xy_cnt += instCnt;
            IS_DEBUG_FLAG {
                //TRACE("BS_inst_xy_cnt=%d", BS_inst_xy_cnt);
                TRACE_NOP;
            };
        }
        else {
            TRACE_ERROR("ERROR! fine_get_inst.2.invalid instCnt: (%d-%d)%d %d (%d)", axis1, initialLineIdx, instCnt, BS_inst_xy_cnt);
            return FINE_MEM_ERROR; //mem-error
        }
    } //for (cxpIdx = cxpSt; cxpIdx < cxpEd; cxpIdx++)

#if 0 //for test
    if (axis1 == ENUM_VER_Y && mode == FINE_USE_CENTER_LINE) {
        //just check
        if (GET_ABS(initial_line[initialLineIdx].cent.y - inst[0].y) > EPSILON) {
            TRACE_ERROR("ERROR! invalid initial_line[].cent.y and inst[].y: %0.2f, %0.2f", initial_line[initialLineIdx].cent.y, inst[0].y);
        }
    }
#endif

    IS_DEBUG_FLAG{
        //if (axis1 == ENUM_VER_Y)
        {
            TRACE_NOP;
        };
    };

    for (cxpIdx = cxpSt; cxpIdx < cxpEd; cxpIdx++) {
        if (initial_line[initialLineIdx].sortLen_a[cxpIdx] > 0) {
            return FINE_INST_OK; //no-error
        }
    }
    return FINE_OUT_OF_INST; //no-inst
}

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_fine_make_group       1
#define DEBUG_test_clipping         0
#endif
#if (DEBUG_test_clipping == 1)
static void test_clipping_fine_init(axis_t axis);
static int test_clipping_initial(void);
#endif
#if (DEBUG_fine_make_group > 1)
static void DEBUG_dump_grp_info(axis_t axis1, int idNo);
#endif

#if (DEBUG_fine_make_group > 0)
#define TRACE_FMG(...)		TRACE(__VA_ARGS__)
#define TRACE_FMG2(...)		TRACE(__VA_ARGS__)
#define TRACE_FMG3(...)		//TRACE(__VA_ARGS__)
#define TRACENR_FMG(...)     TRACENR(__VA_ARGS__)
#else
#define TRACE_FMG(...)
#define TRACE_FMG2(...)
#define TRACE_FMG3(...)
#define TRACENR_FMG(...)
#endif

#define FINE_ADJ_Y_GRP_MIN_MAX
#if 0 //(0613)
#define FINE_REMOVE_Y_GROUP //(0613)
#define FINE_SHARED_INITIAL_X_LINE //(0613)
#endif

#if (defined(DEBUG_FUNCTION_ENABLE_ALL) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)) && defined(DRAW_POLYGON_TEST)
/*FINE_TEST_RULE_1  fine test rule-1*/

static int DEBUG_show_grid(axis_t axis2, vec_t *inst, uint8_t *sort, int gridIdx, float calWidth, float instPosStart, int calMode)
{
    vec_t p0, p1;
    if (calMode == FINE_CXP_CAL_NORM)
    //if (calMode == FINE_CXP_CAL_INIT)
    {
        int color = MY_COLOR - 4; //MY_COLOR -2;
        if (axis2 == ENUM_HOR_X) {
            p0.y = inst[sort[0]].y - 3.0f;
            p0.x = instPosStart + (gridIdx * calWidth);
            p1.y = inst[sort[0]].y + 3.0f;
            p1.x = instPosStart + (gridIdx * calWidth);
        }
        else {
            p0.x = inst[sort[0]].x - 3.0f;
            p0.y = instPosStart + (gridIdx * calWidth);
            p1.x = inst[sort[0]].x + 3.0f;
            p1.y = instPosStart + (gridIdx * calWidth);
        }
        DEBUG_SHOW_LINE_POS(&p0, &p1, color);
    }
    return 0;
}

#if 0 //for test
static int save_cal_slot(axis_t axis2, int initialLineIdx, int cxpIdx, uint8_t *grid_line, int gridCnt, int original, int calMode)
{
    uint8_t *slot = 0;
    cal_slot_t *cal_slot;
    int cal_cnt;
    int i;

    if (grid_line == (uint8_t *)0) {
        TRACE_ERROR("ERROR! invalid grid_line %x", grid_line);
        return 1; //error
    }

    if (original == FINE_CXP_ORI) {
        cal_slot = &BG_cal_slot_org[0];
        cal_cnt = BG_cal_slot_org_cnt;
    }
    else {
        cal_slot = &BG_cal_slot_cur[0];
        cal_cnt = BG_cal_slot_cur_cnt;
    }

    if (cal_cnt < MAX_GRID_CAL_SLOT) {
        cal_slot[cal_cnt].slot = &BG_cal_slot_buf[BG_cal_slot_buf_idx];
        slot = cal_slot[cal_cnt].slot;
        cal_slot[cal_cnt].axis = axis2;
        cal_slot[cal_cnt].line = (uint8_t)initialLineIdx;
        cal_slot[cal_cnt].cxp = (uint8_t)cxpIdx;
        cal_slot[cal_cnt].slotCnt = (uint8_t)gridCnt;
        cal_slot[cal_cnt].calMode = (uint8_t)calMode;
        cal_cnt++;
    }
    else {
        TRACE_ERROR("ERROR! save_cal_slot..invalid cal_cnt %d", cal_cnt);
    }
    for (i = 0; i < gridCnt; i++) {
        if ((BG_cal_slot_buf_idx + i) < MAX_LINE_CAL_SLOT) {
            if (slot != (uint8_t *)0) {
                slot[i] = grid_line[i];
                IS_DEBUG_FLAG{ TRACE_FMG("grid_line..%d %d", i, grid_line[i]); };
            }
            else {
                TRACE_ERROR("ERROR! uninitialized slot: %x", slot);
            }
        }
        else {
            TRACE_ERROR("ERROR! invalid BG_cal_slot_buf_idx %d", BG_cal_slot_buf_idx + i);
            return 1; //error //nsmoon@220126
        }
    }
    if (original == FINE_CXP_ORI) {
        BG_cal_slot_org_cnt = cal_cnt;
    }
    else {
        BG_cal_slot_cur_cnt = cal_cnt;
    }
    BG_cal_slot_buf_idx += gridCnt;

    return 0; //no-error
}
#endif

static int save_cal_slot2(axis_t axis2, int initialLineIdx, int cxpIdx, int original, int calMode, uint8_t maxCnt, int diffSumMin, int gridWidth)
{
    //uint8_t *slot = 0;
    cal_slot_t *cal_slot;
    int cal_cnt;
    //int i;

    if (original == FINE_CXP_ORI) {
        cal_slot = &BG_cal_slot_org[0];
        cal_cnt = BG_cal_slot_org_cnt;
    }
    else {
        cal_slot = &BG_cal_slot_cur[0];
        cal_cnt = BG_cal_slot_cur_cnt;
    }

    if (cal_cnt < MAX_GRID_CAL_SLOT) {
        cal_slot[cal_cnt].slot = &BG_cal_slot_buf[BG_cal_slot_buf_idx];
        //slot = cal_slot[cal_cnt].slot;
        cal_slot[cal_cnt].axis = axis2;
        cal_slot[cal_cnt].line = (uint8_t)initialLineIdx;
        cal_slot[cal_cnt].cxp = (uint8_t)cxpIdx;
        cal_slot[cal_cnt].calMode = (uint8_t)calMode;
        cal_slot[cal_cnt].maxCnt = maxCnt;
        cal_slot[cal_cnt].diffSumMin = (uint8_t)diffSumMin;
        cal_slot[cal_cnt].gridWidth = (uint8_t)gridWidth;
        cal_cnt++;
    }
    else {
        TRACE_ERROR("ERROR! save_cal_slot2..invalid cal_cnt %d %d", cal_cnt, MAX_GRID_CAL_SLOT);
    }

    if (original == FINE_CXP_ORI) {
        BG_cal_slot_org_cnt = cal_cnt;
    }
    else {
        BG_cal_slot_cur_cnt = cal_cnt;
    }

    return 0; //no-error
}
#endif

#if (DEBUG_fine_make_group > 0)
static int DEBUG_dump_grid_line_diff(uint8_t *grid_line, uint8_t *grid_line_cur, uint8_t *grid_line_start, uint8_t *grid_line_end, int gridCnt)
{
    int i, maxCntAll = 0;
    for (i = 0; i < gridCnt; i++) {
        maxCntAll += grid_line_cur[i];
        IS_DEBUG_FLAG{ 
        int diffTmp = GET_ABS(grid_line[i] - grid_line_cur[i]);
            TRACE_FMG2("diff-all: (%d) %d %d %d (%d,%d)", i, diffTmp, grid_line[i], grid_line_cur[i], grid_line_start[i], grid_line_end[i]);
        };
    }
    IS_DEBUG_FLAG{ TRACE_FMG2("maxCntAll: %d", maxCntAll);};
    return maxCntAll;
}
#endif

static int fine_make_group5_A(axis_t axis1, initial_line_a_t *initial_line, int initialLineIdx, int cxpIdx,
                              axis_t axis2, float calWidth, int calCount, int calMode)
{
    vec_t *inst;
    uint8_t *sort;
    int sortLen;
    in_line_t *lineSen;
    initial_line_group_t *grp;
    float instPosStart, instPosEnd;
    //vec_t p0, p1, p2, p3;
    float width;
    int gridPosIdx;
    float gridPosEnd = 0;
    int gridCnt, gridStart, gridEnd = 0;
    uint8_t *grid_line;
    uint8_t *grid_line_cur = 0;
    uint8_t *grid_line_start = &BS_grid_line_start[0];
    uint8_t *grid_line_end = &BS_grid_line_end[0];
    int i, j;
    int tmp_start_i, max_grid_width, max_grid_idx;
    int sortSt, sortEd;
    int stLineCnt, max_diff_cnt;
    int maxIdx, maxCnt;
#define MAX_TMP_START_END   3
    int tmpStart[MAX_TMP_START_END], tmpEnd[MAX_TMP_START_END];
    int tmpStartCnt, maxCntAll;
    int min_num_slot; //, isEdgeArea = 0;
    int gridWidth;
    //int min_rem_adj; //nsmoon@230531
#if 0 //def FINE_ADJ_GRP_5A //nsmoon@230509
    int sortIdxStart, sortIdxEnd;
    int sortIdxStartGrid, sortIdxEndGrid;
    int sortIdxLastGrid, gridLast;
    int gridPosIdxPrev;
#endif
    if (axis1 == ENUM_HOR_X) {
        IS_DEBUG_FLAG{
            TRACE_NOP;
        }
    }
    else {
        //(axis1 == ENUM_VER_Y)
        IS_DEBUG_FLAG{
            TRACE_NOP;
        }
    }

    //instLen = initial_line[initialLineIdx].instLen;
    sortLen = (int)initial_line[initialLineIdx].sortLen_a[cxpIdx];
    sort = initial_line[initialLineIdx].sort_a[cxpIdx];
    //lineStat = initial_line[initialLineIdx].lineStat_a[cxpIdx];
    lineSen = initial_line[initialLineIdx].lineSen_a[cxpIdx];
    inst = initial_line[initialLineIdx].inst_a[cxpIdx];
    IS_DEBUG_FLAG{ TRACE_FMG2("..fine_make_group5_A...%d %d-%d %d %x (%0.1f %d %d)", axis1, initialLineIdx, cxpIdx, sortLen, inst, calWidth, calCount, calMode); };

    //alloc grp buf
    if (BS_initial_line_grp_cnt < MAX_INITIAL_LINE_GRP) {
        initial_line[initialLineIdx].grp_a[cxpIdx] = &BS_initial_line_grp[BS_initial_line_grp_cnt];
        grp = initial_line[initialLineIdx].grp_a[cxpIdx];
        grp->stat = 0; //init
        grp->len = 0; //init
        grp->maxCnt = 0; //init
        grp->maxCntAll = 0; //init
        grp->gridWidth = 0; //init
        grp->diffSum = UINT8_MAX;
        grp->instIdx = &BS_inst_xy_grp[BS_inst_xy_grp_cnt];
        IS_DEBUG_FLAG{ TRACE_FMG2("...grp->instIdx..%x %d-%d %d %d", grp->instIdx, initialLineIdx, cxpIdx, BS_initial_line_grp_cnt, BS_inst_xy_grp_cnt); };
        BS_initial_line_grp_cnt++;
    }
    else {
        TRACE_ERROR("ERROR! invalid BS_initial_line_grp_cnt %d %d", BS_initial_line_grp_cnt, MAX_INITIAL_LINE_GRP);
        return FINE_MEM_ERROR; //mem-error
    }

    //start and end pos of inst
    instPosStart = (axis2 == ENUM_HOR_X) ? inst[sort[0]].x : inst[sort[0]].y;
    instPosEnd = (axis2 == ENUM_HOR_X) ? inst[sort[sortLen - 1]].x : inst[sort[sortLen - 1]].y;
    IS_DEBUG_FLAG{ TRACE_FMG2("...instPosStart,instPosEnd(%d) %0.2f %0.2f", calMode, instPosStart, instPosEnd); };

    int isEdgeArea = initial_line[initialLineIdx].isEdgeArea[cxpIdx];
#if (DEBUG_fine_make_group > 0) //nsmoon@200922
    IS_DEBUG_FLAG{ TRACE_FMG2("...isEdgeArea2: %04x", isEdgeArea);};
#endif
#if 0 //for test
    float edgeDistMax;
    if (calMode == FINE_CXP_CAL_INIT) {
        edgeDistMax = FINE_EDGE_DIST_FROM_AAREA_INIT;
    }
    else {
        edgeDistMax = FINE_EDGE_DIST_FROM_AAREA;
    }
    int isEdgeArea = 0;
    if (sortLen >= FINE_INITIAL_GRP_MIN_NUM_EDGE) {
        if (inst[sort[0]].x < (BS_aarea_zero_x + edgeDistMax) ||
                inst[sort[sortLen - 1]].x > (BS_aarea_end_x - edgeDistMax) ||
                inst[sort[0]].y < (BS_aarea_zero_y + edgeDistMax) ||
                inst[sort[sortLen - 1]].y > (BS_aarea_end_y - edgeDistMax)) {
            isEdgeArea = 1;
        }
        IS_DEBUG_FLAG{ TRACE_FMG2("...isEdgeArea: %d (%0.1f,%0.1f)(%0.1f,%0.1f)", isEdgeArea, inst[sort[0]].x, inst[sort[0]].y, inst[sort[sortLen - 1]].x, inst[sort[sortLen - 1]].y);};
    }
#endif
#if 0 //for test
    if (axis2 == ENUM_HOR_X) {
        if (instPosStart < (BS_aarea_zero_x + FINE_EDGE_DIST_FROM_AAREA) ||
                instPosEnd > (BS_aarea_end_x - FINE_EDGE_DIST_FROM_AAREA)) {
            isEdgeArea = 1;
        }
    }
    else {
        if (instPosStart < (BS_aarea_zero_y + FINE_EDGE_DIST_FROM_AAREA) ||
                instPosEnd > (BS_aarea_end_y - FINE_EDGE_DIST_FROM_AAREA)) {
            isEdgeArea = 1;
        }
    }
    IS_DEBUG_FLAG{ TRACE_FMG2("...isEdgeArea: %d", isEdgeArea);};
#endif
#if 0 //for test
    //start and end pos of inst in opp axis
    float instPos2;
    if (axis1 == ENUM_HOR_X) {
        instPos2 = inst[sort[0]].x;
        if (instPos2 < (BS_aarea_zero_x + FINE_EDGE_PD_DIST_FROM_AAREA)) {
            isEdgeArea = 1;
        }
    }
    else {
        instPos2 = inst[sort[0]].y;
        if (instPos2 > (BS_aarea_end_y - FINE_EDGE_PD_DIST_FROM_AAREA)) {
            isEdgeArea = 1;
        }
    }
    TRACE("=>isEdgeArea: %d", isEdgeArea);
#endif

    if ((axis1 == ENUM_HOR_X && IS_NEAR_Y(isEdgeArea)) || (axis1 == ENUM_VER_Y && IS_NEAR_X(isEdgeArea))) {
        min_num_slot = FINE_MIN_NUM_SLOT_EDGE;
    }
    else {
        min_num_slot = FINE_MIN_NUM_SLOT;
    }

    //cal grid of inst
    width = GET_ABS(instPosEnd - instPosStart);
    gridCnt = (int)((width + calWidth - 0.001f) / calWidth); //FIXME!! 0.001f
    IS_DEBUG_FLAG{ TRACE_FMG2("...width,gridCnt: %0.2f,%d,%0.2f", width, gridCnt, calWidth); };
    if (calMode == FINE_CXP_CAL_INIT) {
         grid_line = &BS_grid_line_cur[0];
    }
    else {
        grid_line = &BS_grid_line_org[0];
        grid_line_cur = &BS_grid_line_cur[0];
    }

    //init grid
    for (i = 0; i < gridCnt; i++) {
        if (i < MAX_GRID_LINE_CNT) {
            grid_line[i] = 0;
            grid_line_start[i] = UINT8_MAX;
            grid_line_end[i] = UINT8_MAX;
            if (calMode == FINE_CXP_CAL_NORM) {
                grid_line_cur[i] = 0;
            }
        }
        else {
            TRACE_ERROR("ERROR! invalid gridCnt %d", gridCnt);
            gridCnt = MAX_GRID_LINE_CNT;
            break;
        }
#if (defined(DEBUG_FUNCTION_ENABLE_ALL) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)) && defined(DRAW_POLYGON_TEST)
        DEBUG_show_grid(axis2, inst, sort, i, calWidth, instPosStart, calMode);
#endif
    }
#if (defined(DEBUG_FUNCTION_ENABLE_ALL) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)) && defined(DRAW_POLYGON_TEST)
    DEBUG_show_grid(axis2, inst, sort, i, calWidth, instPosStart, calMode);
#endif

#ifdef DEBUG_UNBLOCKED_LINE //nsmoon@230403
    int debug_blocked_line_idx[2] = { -1, -1 };
#endif
    //count inst in grid
    gridPosIdx = -1; //nsmoon@211025 0=>-1
    for (i = 0; i < sortLen && i < UINT8_MAX; i++)
    {
        int instIdxTmp = sort[i];
        float tmpPos = (axis2 == ENUM_HOR_X) ? inst[instIdxTmp].x : inst[instIdxTmp].y;

        for (j = gridPosIdx; j < gridCnt; j++) {
            gridPosEnd = instPosStart + ((j + 1) * calWidth);
            IS_DEBUG_FLAG{ TRACE_FMG3("...grpPosEnd..%0.3f/%0.3f %d/%d", gridPosEnd, tmpPos, i, j); };
            if (tmpPos < gridPosEnd + EPSILON) {
                gridPosIdx = j;
                break;
            }
        }
        if (j >= gridCnt) {
            //TRACE_ERROR("ERROR! out of range slot j=%d/%d %d/%d %0.3f/%0.3f", j, gridCnt, i, sortLen, tmpPos, gridPosEnd);
            break;
        }
        if (gridPosIdx < 0) continue; //nsmoon@230131

        if (calMode == FINE_CXP_CAL_NORM) {
            if (grid_line[gridPosIdx] < UINT8_MAX) {
                //increase inst cnt in grid
                grid_line[gridPosIdx]++;
            }
            if (grid_line_cur[gridPosIdx] < UINT8_MAX) {
                int pd = lineSen[instIdxTmp].pd;
                int led = lineSen[instIdxTmp].led;
                //TRACE("...fine_make_group5_A..pd,led=%d,%d", pd, led);
                if (BS_is_set_line_buffer(axis2, pd, led)) {
                    if (grid_line_cur[gridPosIdx] == 0) {
                        //save start sortIdx of grid
                        grid_line_start[gridPosIdx] = (uint8_t)i;
                    }
                    grid_line_end[gridPosIdx] = (uint8_t)i;
                    //increase inst cnt in grid
                    grid_line_cur[gridPosIdx]++;
#ifdef DEBUG_UNBLOCKED_LINE //nsmoon@230403
                    if (debug_blocked_line_idx[0] < 0) {
                        debug_blocked_line_idx[0] = i;
                    }
                    debug_blocked_line_idx[1] = i;
#endif
                }
            }
        }
        else {
            //(calMode == FINE_CXP_CAL_INIT)
            if (grid_line[gridPosIdx] < UINT8_MAX) {
                if (grid_line[gridPosIdx] == 0) {
                    //save start sortIdx of grid
                    grid_line_start[gridPosIdx] = (uint8_t)i;
                }
                grid_line_end[gridPosIdx] = (uint8_t)i;
                //increase inst cnt in grid
                grid_line[gridPosIdx]++;
            }
        }
        IS_DEBUG_FLAG{ TRACE_FMG3("...tmpPos,..(%d)%0.3f/%0.3f %d %d", i, tmpPos, gridPosEnd, gridPosIdx, grid_line[gridPosIdx]); };
    } //for (i = 0; i < sortLen; i++)
#ifdef DEBUG_UNBLOCKED_LINE //nsmoon@230403
    if (calMode == FINE_CXP_CAL_NORM) {
        TRACE_RELEASE("debug_blocked_line_idx= %d %d %d", sortLen, debug_blocked_line_idx[0], debug_blocked_line_idx[1]);
        for (i = 0; i < sortLen && i < UINT8_MAX; i++)
        {
            if (i >= debug_blocked_line_idx[0] && i <= debug_blocked_line_idx[1]) {
                int instIdxTmp = sort[i];
                int pd = lineSen[instIdxTmp].pd;
                int led = lineSen[instIdxTmp].led;
                if (BS_is_set_line_buffer(axis2, pd, led) == 0) {
                    TRACE_RELEASE("****unblocked line=%d %d %d", i, led, pd);
                    DEBUG_SHOW_LINE_PD_LED(axis2, pd, led, MY_COLOR);
                }
            }
        } //for (i = 0; i < sortLen; i++)
    }
#endif
    if (gridPosIdx < 0) { //nsmoon@211025
        TRACE_ERROR("ERROR! no inst in grid");
        return FINE_NO_GROUP;
    }
#if (DEBUG_fine_make_group > 0) //for test
    IS_DEBUG_FLAG{
        for (i = 0; i <= gridPosIdx; i++) {
            TRACE_FMG3("..gridPosIdx,..%d %d %d %d", i, grid_line[i], grid_line_start[i], grid_line_end[i]);
        }
    }
#endif
#if 0 //defined(FINE_RULE_1A_TEST) //for debugging
    save_cal_slot(axis2, initialLineIdx, cxpIdx, grid_line, gridCnt, FINE_CXP_ORI, calMode);
    if (calMode == FINE_CXP_CAL_NORM) {
        save_cal_slot(axis2, initialLineIdx, cxpIdx, grid_line_cur, gridCnt, FINE_CXP_CUR, calMode);
    }
#endif

    //calculate center of grp
    maxCnt = 0;
    tmpStartCnt = 0;
    maxCntAll = 0;
    if (calMode == FINE_CXP_CAL_INIT) {
        //get max maxCnt
        maxIdx = maxCnt = 0;
        for (i = 0; i < gridCnt; i++)
        {
            maxCntAll += grid_line[i];
            int totTmp = 0;
            for (j = i; (j < (i + calCount) && j < gridCnt); j++) {
                totTmp += grid_line[j];
            }
            IS_DEBUG_FLAG{ TRACE_FMG3("...totTmp..%d %d (%d,%d)", i, totTmp, grid_line_start[i], grid_line_end[i]); };
            if (totTmp > maxCnt && grid_line_start[i] != UINT8_MAX) {
                maxCnt = totTmp;
                maxIdx = i;
            }
        }
        gridStart = maxIdx;
        gridEnd = GET_MIN(gridStart + calCount, gridCnt);
        grp->maxCntAll = (uint8_t)GET_MIN(maxCntAll, UINT8_MAX);
        IS_DEBUG_FLAG{ TRACE_FMG3("...gridStart,..%d %d %d", gridStart, gridEnd, maxCntAll); };
    }
#if 1 //step-1
    else {
        //(calMode == FINE_CXP_CAL_NORM)
        if (gridCnt < FINE_MIN_NUM_SLOT) {
            IS_DEBUG_FLAG{ TRACE_FMG("...FINE_NO_GROUP.1.gridCnt= %d", gridCnt); };
#if (DEBUG_fine_make_group > 0)
            DEBUG_dump_grid_line_diff(grid_line, grid_line_cur, grid_line_start, grid_line_end, gridCnt);
#endif
            return FINE_NO_GROUP;
        }
        maxIdx = -1;
        gridEnd = 0;

        //diffSum = maxCnt = 0;
        for (i = 0; i < gridCnt; i++) {
            maxCntAll += grid_line_cur[i];
#if 0 //defined(DEBUG_FUNCTION_ENABLE_ALL) //for debugging
            int diffTmp = GET_ABS(grid_line[i] - grid_line_cur[i]);
            IS_DEBUG_FLAG{ TRACE_FMG2("diff-all: (%d) %d %d %d (%d,%d)", i, diffTmp, grid_line[i], grid_line_cur[i], grid_line_start[i], grid_line_end[i]);};
#endif
        }
        grp->maxCntAll = (uint8_t)GET_MIN(maxCntAll, UINT8_MAX);
        stLineCnt = 0;
        if ((axis1 == ENUM_HOR_X && IS_NEAR_Y(isEdgeArea)) || (axis1 == ENUM_VER_Y && IS_NEAR_X(isEdgeArea))) {
            max_diff_cnt = FINE_MAX_DIFF_EDGE_CNT;
        }
        else {
            max_diff_cnt = FINE_MAX_DIFF_CNT;
        }

        for (i = 0; i < gridCnt; i++) {
            int diffTmp = GET_ABS(grid_line[i] - grid_line_cur[i]);
#if defined(N75_TEST) || defined(N65_TEST) //nsmoon@230328 //TTR4_230321.dlt #75 //FIXME!!
            if (grid_line[i] > 1 && grid_line_cur[i] == 0) {
                diffTmp = 0xFF;
            }
#endif
            int over5 = (grid_line_cur[i] >= 5); //5-lines
            IS_DEBUG_FLAG{ TRACE_FMG2("...diff: (%d/%d) %d (%d/%d) (%d,%d)", i, maxIdx, diffTmp, grid_line[i], grid_line_cur[i], grid_line_start[i], grid_line_end[i]);};
            if (maxIdx < 0) {
                //for start
                if (grid_line_cur[i] == 0) continue;
                if (diffTmp > max_diff_cnt) continue;
                IS_DEBUG_FLAG{ TRACE_FMG2("...start: %d, %d/%d", stLineCnt, i, maxIdx);};
            }
            else {
                //for stop
                if (over5) {
                    if (grid_line[i] > 0) {
                        float diffRateTmp = (float)grid_line_cur[i] / (float)grid_line[i];
                        if (diffRateTmp < 0.5f) { //50%
                            IS_DEBUG_FLAG{ TRACE_FMG2(".2.stop: %d, %d/%d %d/%d", stLineCnt, i, maxIdx, grid_line_cur[i], grid_line[i]);};
                            //if ((i - maxIdx) < FINE_MIN_NUM_SLOT || stLineCnt == 0) {
                            if ((i - maxIdx) < FINE_MIN_NUM_SLOT || stLineCnt < 4) { //nsmoon@200803
                                //restart
                                if (tmpStartCnt < MAX_TMP_START_END) {
                                    tmpStart[tmpStartCnt] = maxIdx; //nsmoon@200803
                                    tmpEnd[tmpStartCnt] = i;
                                    tmpStartCnt++;
                                }
                                maxIdx = -1;
                                stLineCnt = 0;
                                continue;
                            }
                            else {
                                break; //stop
                            }
                        }
                    }
                }
                else {
                    //if (diffTmp > max_diff_cnt || (isEdgeArea == 0 && grid_line_cur[i] == 0))
                    if (diffTmp > max_diff_cnt) //nsmoon@200513
                    {
                        IS_DEBUG_FLAG{ TRACE_FMG2(".1.stop: %d, %d/%d %d/%d", stLineCnt, i, maxIdx, diffTmp, max_diff_cnt);};
                        //if ((i - maxIdx) < FINE_MIN_NUM_SLOT || stLineCnt == 0) {
                        if ((i - maxIdx) < FINE_MIN_NUM_SLOT || stLineCnt < 4) { //nsmoon@200803
                            //restart
                            if (tmpStartCnt < MAX_TMP_START_END) {
                                tmpStart[tmpStartCnt] = maxIdx; //nsmoon@200803
                                tmpEnd[tmpStartCnt] = i;
                                tmpStartCnt++;
                            }
                            maxIdx = -1;
                            stLineCnt = 0;
                            continue;
                        }
                        else {
                            break; //stop
                        }
                    }
                }
            }
            if (maxIdx < 0) {
                maxIdx = i;
            }
            if (grid_line_start[i] != UINT8_MAX) {
                stLineCnt++;
            }
        }
        if (maxIdx < 0) {
            IS_DEBUG_FLAG{ TRACE_FMG2("...maxIdx= %d", maxIdx); };
#if 0 //defined(DEBUG_FUNCTION_ENABLE_ALL) //for debugging
            DEBUG_dump_grid_line_diff(grid_line, grid_line_cur, grid_line_start, grid_line_end, gridCnt);
#endif
            //return FINE_NO_GROUP;
            gridStart = gridEnd = -1;
        }
        else {
        gridStart = maxIdx;
        gridEnd = GET_MAX(i, maxIdx);
    }
    }
#endif
    //cal gridWidth-1
    gridWidth = gridEnd - gridStart;
#if 1 //nsmoon@200803 => nsmoon@200922
    if (gridStart < 0 && tmpStartCnt == 0) {
        IS_DEBUG_FLAG{ TRACE_FMG2("...FINE_NO_GROUP.2.gridStart= %d", gridStart); };
        return FINE_NO_GROUP;
    }
    max_grid_width = gridWidth;
    max_grid_idx = -1;
    for (tmp_start_i = 0; tmp_start_i < tmpStartCnt; tmp_start_i++) {
        int gridWidth2 = tmpEnd[tmp_start_i] - tmpStart[tmp_start_i];
        if (gridWidth2 > max_grid_width) {
            max_grid_width = gridWidth2;
            max_grid_idx = tmp_start_i;
        }
    }
    if (max_grid_idx >= 0) {
        gridStart = tmpStart[max_grid_idx];
        gridEnd = tmpEnd[max_grid_idx];
        gridWidth = max_grid_width;
    }
#endif
    IS_DEBUG_FLAG{ TRACE_FMG2("...gridWidth=1 %d(%d,%d)", gridWidth, gridStart, gridEnd); };
    if (gridWidth < min_num_slot) {
        IS_DEBUG_FLAG{ TRACE_FMG2("...FINE_NO_GROUP.3.gridWidth=1 %d(%04x,%d)", gridWidth, isEdgeArea, min_num_slot); };
#if defined(DEBUG_FUNCTION_ENABLE_ALL) && defined(DRAW_POLYGON_TEST) //for debugging
        if (calMode == FINE_CXP_CAL_NORM) {
            DEBUG_dump_grid_line_diff(grid_line, grid_line_cur, grid_line_start, grid_line_end, gridCnt);
        }
#endif
        return FINE_NO_GROUP;
    }

#if 1 //step-2
    if (calMode == FINE_CXP_CAL_NORM) {
        //get diffSumMin
        int diffSumMin = MIN_INITIAL_VAL;
        maxIdx = gridStart;
        if (gridWidth > calCount) {
            int shiftCnt = gridCnt - calCount;
            IS_DEBUG_FLAG{ TRACE_FMG2("...shiftCnt..%d %d %d %d", shiftCnt, gridCnt, calCount, gridStart); };
            for (i = gridStart; i <= shiftCnt; i++)
            {
                int diffSum = 0;
                int totCnt = 0;
                for (j = i; (j < (i + calCount) && j < gridCnt); j++) {
                    int diff = GET_ABS(grid_line[j] - grid_line_cur[j]);
                    totCnt += grid_line[j];
                    diffSum += diff;
                }
                IS_DEBUG_FLAG{ TRACE_FMG2("...totCnt,diffSum..%d %d %d (%d,%d)", i, totCnt, diffSum, grid_line_start[i], grid_line_end[i]); };
                if ((diffSum < diffSumMin && grid_line_start[i] != UINT8_MAX) ||
                        (diffSum == diffSumMin && totCnt > maxCnt && grid_line_start[i] != UINT8_MAX)) {
                    diffSumMin = diffSum;
                    maxCnt = totCnt;
                    maxIdx = i;
                }
            }
        }
        gridStart = maxIdx;
        gridEnd = GET_MIN(gridStart + calCount, gridEnd);
    } //(calMode == FINE_CXP_CAL_NORM)
#endif //1

    IS_DEBUG_FLAG{ TRACE_FMG2("...gridStart,gridEnd: %d %d", gridStart, gridEnd);};
#if 0 //defined(DEBUG_FUNCTION_ENABLE_ALL) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
    uint8_t maxCnt_debug01 = (uint8_t)GET_MIN(maxCnt, UINT8_MAX);
    save_cal_slot2(axis2, initialLineIdx, cxpIdx, FINE_CXP_ORI, calMode, maxCnt_debug01, diffSum, gridWidth);
#endif

    //make cal start and end Idx of grid
    sortSt = grid_line_start[gridStart];
    sortEd = grid_line_end[gridEnd - 1];
    if (sortSt == UINT8_MAX) {
        IS_DEBUG_FLAG{ TRACE_FMG2("...invalid sortSt: %d (%d)", sortSt, gridStart);};
        for (i = gridStart; i < gridEnd; i++) {
            if (grid_line_start[i] != UINT8_MAX) {
                sortSt = grid_line_start[i];
                gridStart = i;
                break;
            }
        }
    }
    if (sortEd == UINT8_MAX) {
        IS_DEBUG_FLAG{ TRACE_FMG2("...invalid sortEd: %d (%d)", sortEd, gridEnd);};
        for (i = (gridEnd - 1); i >= gridStart; i--) {
            if (grid_line_start[i] != UINT8_MAX) {
                sortEd = grid_line_end[i];
                gridEnd = i + 1;
                break;
            }
        }
    }
    IS_DEBUG_FLAG{ TRACE_FMG2("...sortSt,sortEd..(%d) %d %d (%d, %d)", maxIdx, sortSt, sortEd, gridStart, gridEnd);};
    if (sortSt == UINT8_MAX || sortEd == UINT8_MAX) {
        TRACE_ERROR("ERROR! invalid sortSt,sortEd: %d %d", sortSt, sortEd);
        return FINE_NO_GROUP;
    }

    //cal diffSum and recal maxCnt for FINE_CXP_ORI
    int diffSum = 0;
    if (calMode == FINE_CXP_CAL_NORM) {
        maxCnt = diffSum = 0;
        for (i = gridStart; i < gridEnd; i++) {
            int diffTmp = GET_ABS(grid_line[i] - grid_line_cur[i]);
            diffSum += diffTmp;
            maxCnt += grid_line_cur[i];
        }
        IS_DEBUG_FLAG{ TRACE_FMG2("...diffSum,maxCnt:%d %d", diffSum, maxCnt);};
    }
    if (calMode == FINE_CXP_CAL_INIT)
    {
        if ((((axis1 == ENUM_HOR_X && IS_NOT_NEAR_Y(isEdgeArea)) ||
              (axis1 == ENUM_VER_Y && IS_NOT_NEAR_X(isEdgeArea)))
               && maxCnt < FINE_MAX_CNT_INIT) ||
            (((axis1 == ENUM_HOR_X && IS_NEAR_Y(isEdgeArea)) ||
              (axis1 == ENUM_VER_Y && IS_NEAR_X(isEdgeArea)))
              && maxCnt < FINE_MAX_CNT_INIT_EDGE)) //20
        {
            IS_DEBUG_FLAG{ TRACE_FMG("...FINE_NO_GROUP.4.maxCnt= %d", maxCnt); };
            return FINE_NO_GROUP;
        }
    }

    //cal gridWidth-2
    gridWidth = gridEnd - gridStart;
    IS_DEBUG_FLAG{ TRACE_FMG2("...gridWidth=2 %d(%d %d)", gridWidth, gridStart, gridEnd); };
    if (gridWidth < min_num_slot) {
        IS_DEBUG_FLAG{ TRACE_FMG("...FINE_NO_GROUP.5.gridWidth=2 %d(%04x,%d)", gridWidth, isEdgeArea, min_num_slot); };
#if defined(DEBUG_FUNCTION_ENABLE_ALL) && defined(DRAW_POLYGON_TEST) //for debugging
        if (calMode == FINE_CXP_CAL_NORM) {
            DEBUG_dump_grid_line_diff(grid_line, grid_line_cur, grid_line_start, grid_line_end, gridCnt);
        }
#endif
        return FINE_NO_GROUP;
    }

#if (defined(DEBUG_FUNCTION_ENABLE_ALL) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)) && defined(DRAW_POLYGON_TEST)
    uint8_t maxCnt_debug02 = (uint8_t)GET_MIN(maxCnt, UINT8_MAX);
    save_cal_slot2(axis2, initialLineIdx, cxpIdx, FINE_CXP_ORI, calMode, maxCnt_debug02, diffSum, gridWidth);
#endif

    //make grp of inst
    int cnt = 0;
    for (i = sortSt; i <= sortEd; i++)
    {
        uint8_t instIdxTmp = sort[i];
        if (calMode == FINE_CXP_CAL_NORM) {
            int pd = lineSen[instIdxTmp].pd;
            int led = lineSen[instIdxTmp].led;
            if (BS_is_set_line_buffer(axis2, pd, led) == 0) {
                IS_DEBUG_FLAG2{ TRACE_FMG3("...no in buff~ %d %d %d (%d)", axis2, pd, led, i);};
                //DEBUG_SHOW_LINE_PD_LED(axis2, pd, led, MY_COLOR);
                continue;
            }
        }

        //add in grp->instIdx
            if ((BS_inst_xy_grp_cnt + cnt) < MAX_NUM_INST_FINE && cnt < MAX_INST_SORT_TBL_SIZE) {
#if (DEBUG_fine_make_group > 0) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
                if (calMode == FINE_CXP_CAL_NORM) {
                    DEBUG_SHOW_POS(&inst[instIdxTmp], 0.1f, 0.1f, initialLineIdx);
                }
                else {
                    DEBUG_SHOW_POS(&inst[instIdxTmp], 0.1f, 0.1f, MY_COLOR -4); //gray
                }
#endif
                grp->instIdx[cnt] = instIdxTmp;
                IS_DEBUG_FLAG{ TRACE_FMG3("...add grp->instIdx[]: (%d) %d-%d %d", i, BS_inst_xy_grp_cnt, cnt, instIdxTmp); };
                cnt++;
            }
            else {
                TRACE_ERROR("ERROR! 1 invalid cnt >= MAX_NUM_INST_FINE: %d/%d", cnt, BS_inst_xy_grp_cnt);
                return FINE_MEM_ERROR; //mem-error
            }
        } //for (i = sortSt; i <= sortEd; i++)

    if (cnt > UINT8_MAX) {
        TRACE_ERROR("ERROR! fine_make_group5_A..invalid cnt %d", cnt);
        cnt = UINT8_MAX;
    }
    if (diffSum > UINT8_MAX) {
        TRACE_ERROR("ERROR! fine_make_group5_A..invalid diffSum %d", diffSum);
        diffSum = UINT8_MAX;
    }
    if (maxCnt > UINT8_MAX) {
        TRACE_ERROR("ERROR! fine_make_group5_A..invalid maxCnt %d", maxCnt);
        maxCnt = UINT8_MAX;
    }
    if (gridWidth > UINT8_MAX) {
        TRACE_ERROR("ERROR! fine_make_group5_A..invalid gridWidth %d", gridWidth);
        gridWidth = UINT8_MAX;
    }
    grp->stat = 1; //grp
    grp->len = (uint8_t)cnt;
    //grp->calMode = calMode;
    grp->diffSum = (uint8_t)diffSum;
    grp->maxCnt = (uint8_t)maxCnt;
    grp->gridWidth = (uint8_t)gridWidth;
    BS_inst_xy_grp_cnt += cnt; //increase buf

    return FINE_GROUP_FOUND; //found grp
}

#if 0 //for test //nsmoon@211018 1=>0
static int fine_make_group5_B(axis_t axis1, initial_line_a_t *initial_line, int initialLineIdx, int cxpIdx, axis_t axis2)
{
    uint8_t *sort;
    int sortLen;
    in_line_t *lineSen;
    initial_line_group_t *grp;
    int i;

    if (axis1 == ENUM_HOR_X) {
        IS_DEBUG_FLAG{
            TRACE_NOP;
        }
    }
    else {
        //(axis1 == ENUM_VER_Y)
        IS_DEBUG_FLAG{
            TRACE_NOP;
        }
    }

    //instLen = initial_line[initialLineIdx].instLen;
    sortLen = (int)initial_line[initialLineIdx].sortLen_a[cxpIdx];
    sort = initial_line[initialLineIdx].sort_a[cxpIdx];
    //lineStat = initial_line[initialLineIdx].lineStat_a[cxpIdx];
    lineSen = initial_line[initialLineIdx].lineSen_a[cxpIdx];
    grp = initial_line[initialLineIdx].grp_a[cxpIdx];
    IS_DEBUG_FLAG{ TRACE_FMG2("fine_make_group5_A...%d %d-%d %d", axis1, initialLineIdx, cxpIdx, sortLen); };

    #if 0 //test
    //alloc grp buf
    if (BS_initial_line_grp_cnt < MAX_INITIAL_LINE_GRP) {
        initial_line[initialLineIdx].grp_a[cxpIdx] = &BS_initial_line_grp[BS_initial_line_grp_cnt];
        grp = initial_line[initialLineIdx].grp_a[cxpIdx];
        grp->stat = 0; //init
        grp->len = 0; //init
        grp->maxCnt = 0; //init
        grp->maxCntAll = 0; //init
        grp->gridWidth = 0; //init
        grp->diffSum = UINT8_MAX;
        grp->instIdx = &BS_inst_xy_grp[BS_inst_xy_grp_cnt];
        IS_DEBUG_FLAG{ TRACE_FMG2("grp->instIdx..%x %d-%d %d %d", grp->instIdx, initialLineIdx, cxpIdx, BS_initial_line_grp_cnt, BS_inst_xy_grp_cnt); };
        BS_initial_line_grp_cnt++;
    }
    else {
        TRACE_ERROR("ERROR! invalid BS_initial_line_grp_cnt %d ", BS_initial_line_grp_cnt);
        return FINE_MEM_ERROR; //mem-error
    }
    #endif

#if 0 //reserved, do not remove
    //start and end pos of inst
    vec_t *inst = initial_line[initialLineIdx].inst_a[cxpIdx];
    float instPosStart = (axis2 == ENUM_HOR_X) ? inst[sort[0]].x : inst[sort[0]].y;
    float instPosEnd = (axis2 == ENUM_HOR_X) ? inst[sort[sortLen - 1]].x : inst[sort[sortLen - 1]].y;
    IS_DEBUG_FLAG{ TRACE_FMG2("..instPosStart,instPosEnd(%d) %0.2f %0.2f", calMode, instPosStart, instPosEnd); };

    int isEdgeArea = 0;
    if (axis2 == ENUM_HOR_X) {
        if (instPosStart < (BS_aarea_zero_x + FINE_EDGE_DIST_FROM_AAREA) ||
                instPosEnd > (BS_aarea_end_x - FINE_EDGE_DIST_FROM_AAREA)) {
            isEdgeArea = 1;
        }
    }
    else {
        if (instPosStart < (BS_aarea_zero_y + FINE_EDGE_DIST_FROM_AAREA) ||
                instPosEnd > (BS_aarea_end_y - FINE_EDGE_DIST_FROM_AAREA)) {
            isEdgeArea = 1;
        }
    }
    IS_DEBUG_FLAG{ TRACE_FMG2("..isEdgeArea: %d", isEdgeArea);};
#endif
#if 0 //for test
    //start and end pos of inst in opp axis
    float instPos2;
    if (axis1 == ENUM_HOR_X) {
        instPos2 = inst[sort[0]].x;
        if (instPos2 < (BS_aarea_zero_x + FINE_EDGE_PD_DIST_FROM_AAREA)) {
            isEdgeArea = 1;
        }
    }
    else {
        instPos2 = inst[sort[0]].y;
        if (instPos2 > (BS_aarea_end_y - FINE_EDGE_PD_DIST_FROM_AAREA)) {
            isEdgeArea = 1;
        }
    }
    TRACE("=>isEdgeArea: %d", isEdgeArea);
#endif

#if 0 //reserved
    int min_num_slot;
    if (isEdgeArea == 0) {
        min_num_slot = FINE_MIN_NUM_SLOT;
    }
    else {
        min_num_slot = FINE_MIN_NUM_SLOT_EDGE;
    }
#endif

    //make grp of inst
    int cnt = 0;
    for (i = 0; i <= sortLen; i++)
    {
        uint8_t instIdxTmp = sort[i];
        int pd = lineSen[instIdxTmp].pd;
        int led = lineSen[instIdxTmp].led;
        if (BS_is_set_line_buffer(axis2, pd, led) == 0) {
            IS_DEBUG_FLAG2{ TRACE_FMG3("no in buff~~ %d %d %d (%d)", axis2, pd, led, i);};
            //DEBUG_SHOW_LINE_PD_LED(axis2, pd, led, MY_COLOR);
            continue;
        }

        //add in grp->instIdx
        if ((BS_inst_xy_grp_cnt + cnt) < MAX_NUM_INST_FINE && cnt < MAX_INST_SORT_TBL_SIZE) {
#if (DEBUG_fine_make_group > 0) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
            vec_t *inst = initial_line[initialLineIdx].inst_a[cxpIdx];
            DEBUG_SHOW_POS(&inst[instIdxTmp], 0.1f, 0.1f, initialLineIdx);
#endif
            grp->instIdx[cnt] = instIdxTmp;
            IS_DEBUG_FLAG{ TRACE_FMG2(" ..add grp->instIdx[]: (%d) %d-%d %d", i, BS_inst_xy_grp_cnt, cnt, instIdxTmp); };
            cnt++;
        }
        else {
            TRACE_ERROR("ERROR! 1 invalid cnt >= MAX_NUM_INST_FINE: %d/%d", cnt, BS_inst_xy_grp_cnt);
            return FINE_MEM_ERROR; //mem-error
        }
    } //for (i = sortSt; i <= sortEd; i++)

    if (cnt > UINT8_MAX) {
        TRACE_ERROR("ERROR! fine_make_group5_B..invalid cnt %d", cnt);
        cnt = UINT8_MAX;
    }
    grp->stat = 1; //grp
    grp->len = (uint8_t)cnt;
    grp->maxCnt = (uint8_t)cnt;
    BS_inst_xy_grp_cnt += cnt; //increase buf

    return FINE_GROUP_FOUND; //found grp
}
#endif

static int fine_make_grp_min_max5(axis_t axis1, initial_line_group_t *grp, vec_t *inst)
{
    int minIdx, maxIdx;
    float minPos, maxPos, minPos2, maxPos2;

    minIdx = grp->instIdx[0];
    maxIdx = grp->instIdx[grp->len - 1];
    if (axis1 == ENUM_HOR_X) {
        if (inst[minIdx].y < inst[maxIdx].y) {
            minPos = inst[minIdx].y;
            maxPos = inst[maxIdx].y;
        }
        else {
            maxPos = inst[minIdx].y;
            minPos = inst[maxIdx].y;
        }
        if (inst[minIdx].x < inst[maxIdx].x) {
            minPos2 = inst[minIdx].x;
            maxPos2 = inst[maxIdx].x;
        }
        else {
            maxPos2 = inst[minIdx].x;
            minPos2 = inst[maxIdx].x;
        }
    }
    else {
        if (inst[minIdx].x < inst[maxIdx].x) {
            minPos = inst[minIdx].x;
            maxPos = inst[maxIdx].x;
        }
        else {
            maxPos = inst[minIdx].x;
            minPos = inst[maxIdx].x;
        }
        if (inst[minIdx].y < inst[maxIdx].y) {
            minPos2 = inst[minIdx].y;
            maxPos2 = inst[maxIdx].y;
        }
        else {
            maxPos2 = inst[minIdx].y;
            minPos2 = inst[maxIdx].y;
        }
    }
    grp->instPos.min = minPos;
    grp->instPos.max = maxPos;
    grp->instPos2.min = minPos2;
    grp->instPos2.max = maxPos2;
    return 0;
}

static int fine_make_y_grp_x_line_cxp(initial_line_a_t *initial_line, int initialLineIdx, int cxpIdx, initial_line_a_t *initial_line_2)
{
    initial_line_group_t *grp;
    vec_t *inst;
    int initialLineIdx_2 = initial_line[initialLineIdx].cxOppLine[cxpIdx];
    int ret;

    IS_DEBUG_FLAG{
        TRACE_FMG2("(a)fine_make_y_grp_x_line_cxp..%d %d-%d %0.2f %0.2f", ENUM_HOR_X, initialLineIdx, cxpIdx, initial_line[initialLineIdx].cxpCent[cxpIdx].x, initial_line[initialLineIdx].cxpCent[cxpIdx].y);
        TRACE_NOP;
    };

    ///////////////////////////////////////////////////
    //make groups
    ///////////////////////////////////////////////////
    ret = fine_make_group5_A(ENUM_HOR_X, initial_line, initialLineIdx, cxpIdx,
                             ENUM_VER_Y, FINE_CAL_WIDTH_INIT, FINE_GRID_WIDTH_INIT, FINE_CXP_CAL_INIT);
#if 1 //nsmoon@220126 move
    if (ret == FINE_MEM_ERROR) {
        return -1; //mem-error
    }
    else if (ret == FINE_NO_GROUP) {
        return FINE_NO_GROUP;
    }
#endif

    inst = initial_line[initialLineIdx].inst_a[cxpIdx];
    grp = initial_line[initialLineIdx].grp_a[cxpIdx];
    grp->cxLineIdx = UINT8_MAX;
    if (grp->len == 0) {
        TRACE_ERROR("ERROR! fine_make_y_grp_x_line_cxp..invalid grp->len");
        return FINE_NO_GROUP;
    }

    fine_make_grp_min_max5(ENUM_HOR_X, grp, inst); //instPos, instPos2

    initial_line[initialLineIdx].cent.x = (grp->instPos2.min + grp->instPos2.max) * 0.5f; //x-line x-cent
    initial_line[initialLineIdx].cent.y = -1; //see grp center pos
    grp->cxLineIdx = (uint8_t)initialLineIdx_2;

#if 0 //(DEBUG_fine_make_group > 0)
    TRACE("fine_make_y_grp_x_line_cxp..grp->len=%d", grp->len);
    int minIdx = grp->instIdx[0];
    int maxIdx = grp->instIdx[grp->len - 1];
    DEBUG_SHOW_MIN_MAX_POS(ENUM_VER_Y, &inst[0], minIdx, maxIdx, 1);
#endif

    //make Y line
    initial_line_2[initialLineIdx_2].cxpCntMax = 2; //must be set
    initial_line_2[initialLineIdx_2].cxLineIdx = (uint8_t)initialLineIdx; //initialLineIdx of x-line
    initial_line_2[initialLineIdx_2].cxGrpIdx = (uint8_t)cxpIdx; //grpIdx of x-line
    initial_line_2[initialLineIdx_2].cent.x = initial_line[initialLineIdx].cent.x; //x-line x-cent, not y-line x-cent!!!
    initial_line_2[initialLineIdx_2].cent.y = (grp->instPos.min + grp->instPos.max) * 0.5f; //x-line y-cent is same as y-line y-cent
    //initial_line_2[initialLineIdx_2].linePos2.min = grp->instPos2.min;
    //initial_line_2[initialLineIdx_2].linePos2.max = grp->instPos2.max;
    //initial_line_2[initialLineIdx_2].instLen = 0; //reset
    //initial_line_2[initialLineIdx_2].sortLen = 0; //reset
    IS_DEBUG_FLAG{ TRACE_FMG2("..y-cent-line: %d/%d %d/%d-%d %d (%0.2f %0.2f)", ENUM_VER_Y, initialLineIdx_2, ENUM_HOR_X, initialLineIdx, cxpIdx, grp->len, initial_line_2[initialLineIdx_2].cent.x, initial_line_2[initialLineIdx_2].cent.y); };
    return FINE_GROUP_FOUND;
}

static int fine_make_x_grp_y_cent_line(initial_line_a_t *initial_line, int initialLineIdx,
                                       initial_line_a_t *initial_line_2, int initialLineIdx_2)
{
    initial_line_group_t *grp;
    vec_t *inst;
    int ret;

    IS_DEBUG_FLAG{
        TRACE_FMG2("(b)fine_make_x_grp_y_cent_line..%d-%d", initialLineIdx, initialLineIdx_2);
        TRACE_NOP;
    };

    ///////////////////////////////////////////////////
    //make groups
    ///////////////////////////////////////////////////
    ret = fine_make_group5_A(ENUM_VER_Y, initial_line, initialLineIdx, FINE_CXP_ORI,
                             ENUM_HOR_X, FINE_CAL_WIDTH_NORM, FINE_GRID_WIDTH_NORM, FINE_CXP_CAL_NORM);
#if 1 //nsmoon@220126
    if (ret == FINE_MEM_ERROR) {
        return FINE_MEM_ERROR; //mem-error
    }
    else if (ret == FINE_NO_GROUP) {
        return FINE_NO_GROUP;
    }
#endif

    inst = initial_line[initialLineIdx].inst_a[FINE_CXP_ORI];
    grp = initial_line[initialLineIdx].grp_a[FINE_CXP_ORI];
    grp->cxLineIdx = UINT8_MAX; //no-grp
    if (grp->len == 0) {
        TRACE_ERROR("ERROR! fine_make_x_grp_y_cent_line..invalid grp->len");
        return FINE_NO_GROUP;
    }

    fine_make_grp_min_max5(ENUM_VER_Y, grp, inst); //instPos, instPos2
    grp->cxLineIdx = (uint8_t)initialLineIdx_2; //found grp

#if 0 //(DEBUG_fine_make_group > 0)
    TRACE("fine_make_x_grp_y_cent_line..grp->len=%d", grp->len);
    int minIdx = grp->instIdx[0];
    int maxIdx = grp->instIdx[grp->len - 1];
    DEBUG_SHOW_MIN_MAX_POS(ENUM_VER_Y, &inst[0], minIdx, maxIdx, 1);
#endif

    //make initial line of opposite axis
    initial_line_2[initialLineIdx_2].cxpCntMax = 2; //must be set
    initial_line_2[initialLineIdx_2].cxLineIdx = (uint8_t)initialLineIdx; //initialLineIdx of x-line
    initial_line_2[initialLineIdx_2].cxGrpIdx = FINE_CXP_ORI; //grpIdx of x-line
    initial_line_2[initialLineIdx_2].cent.x = (grp->instPos.min + grp->instPos.max) * 0.5f;
    initial_line_2[initialLineIdx_2].cent.y = (grp->instPos2.min + grp->instPos2.max) * 0.5f;
    //initial_line_2[initialLineIdx_2].linePos2.min = grp->instPos2.min; //x-line x-min
    //initial_line_2[initialLineIdx_2].linePos2.max = grp->instPos2.max; //x-line y-min
    //initial_line_2[initialLineIdx_2].instLen = 0; //reset
    //initial_line_2[initialLineIdx_2].sortLen = 0; //reset
    IS_DEBUG_FLAG{ TRACE_FMG2("..x-cent-line: %d/%d %d/%d %d (%0.2f %0.2f)", ENUM_HOR_X, initialLineIdx_2, ENUM_VER_Y, initialLineIdx, grp->len, initial_line_2[initialLineIdx_2].cent.x, initial_line_2[initialLineIdx_2].cent.y); };
    return FINE_GROUP_FOUND;
}

#if 0 //for test //nsmoon@211018 1=>0
static int fine_make_x_grp_y_cent_line_tmp(initial_line_a_t *initial_line, int initialLineIdx)
{
    initial_line_group_t *grp;
    vec_t *inst;
    int ret;

    IS_DEBUG_FLAG{
        TRACE_FMG2("(b2)fine_make_x_grp_y_cent_line_tmp..%d-%d", initialLineIdx, initialLineIdx);
        TRACE_NOP;
    };

    ///////////////////////////////////////////////////
    //make groups
    ///////////////////////////////////////////////////
    ret = fine_make_group5_B(ENUM_VER_Y, initial_line, initialLineIdx, FINE_CXP_ORI, ENUM_HOR_X);
#if 1 //nsmoon@220126
    if (ret == FINE_MEM_ERROR) {
        return -1; //mem-error
    }
    else if (ret == FINE_NO_GROUP) {
        return FINE_NO_GROUP;
    }
#endif

    inst = initial_line[initialLineIdx].inst_a[FINE_CXP_ORI];
    grp = initial_line[initialLineIdx].grp_a[FINE_CXP_ORI];
    grp->cxLineIdx = UINT8_MAX; //no-grp
    if (grp->len == 0) {
        TRACE_ERROR("ERROR! fine_make_x_grp_y_cent_line..invalid grp->len");
        return FINE_NO_GROUP;
    }

    fine_make_grp_min_max5(ENUM_VER_Y, grp, inst); //instPos, instPos2

#if (DEBUG_fine_make_group > 0)
    TRACE(".fine_make_x_grp_y_cent_line_tmp..grp->len=%d", grp->len);
    int minIdx = grp->instIdx[0];
    int maxIdx = grp->instIdx[grp->len - 1];
    DEBUG_SHOW_MIN_MAX_POS(ENUM_VER_Y, &inst[0], minIdx, maxIdx, 1);
#endif

    return FINE_GROUP_FOUND;
}
#endif

static int fine_make_y_grp_x_line(initial_line_a_t *initial_line, int initialLineIdx)
{
    initial_line_group_t *grp;
    vec_t *inst;
    int ret;

    //initial_line = &BS_initial_line_y[0];
    IS_DEBUG_FLAG{
        TRACE_FMG2("(c)fine_make_y_grp_x_line..%d", initialLineIdx);
        TRACE_NOP;
    };

    ///////////////////////////////////////////////////
    //make groups
    ///////////////////////////////////////////////////
    ret = fine_make_group5_A(ENUM_HOR_X, initial_line, initialLineIdx, FINE_CXP_ORI,
                             ENUM_VER_Y, FINE_CAL_WIDTH_NORM, FINE_GRID_WIDTH_NORM, FINE_CXP_CAL_NORM);
#if 1 //nsmoon@220126
    if (ret == FINE_MEM_ERROR) {
        return -1; //mem-error
    }
    else if (ret == FINE_NO_GROUP) {
        return FINE_NO_GROUP;
    }
#endif

    inst = initial_line[initialLineIdx].inst_a[FINE_CXP_ORI];
    grp = initial_line[initialLineIdx].grp_a[FINE_CXP_ORI];
    grp->cxLineIdx = UINT8_MAX;
    if (grp->len == 0) {
        TRACE_ERROR("ERROR! fine_make_x_grp_y_cent_line..invalid grp->len");
        return FINE_NO_GROUP;
    }

    fine_make_grp_min_max5(ENUM_HOR_X, grp, inst); //instPos, instPos2
    grp->cxLineIdx = 0;
    IS_DEBUG_FLAG{ TRACE_FMG2("..y-cent-line-2: %d %d %d (%0.2f %0.2f)", ENUM_HOR_X, initialLineIdx, grp->len, initial_line[initialLineIdx].cent.x, initial_line[initialLineIdx].cent.y); };

#if 0 //(DEBUG_fine_make_group > 0)
    TRACE("fine_make_y_grp_x_line..grp->len=%d", grp->len);
    int minIdx = grp->instIdx[0];
    int maxIdx = grp->instIdx[grp->len - 1];
    DEBUG_SHOW_MIN_MAX_POS(ENUM_VER_Y, &inst[0], minIdx, maxIdx, 1);
#endif

    return FINE_GROUP_FOUND;
}

#ifdef FINE_CAL_MIN_MAX //fine_cal_min_max3
ATTR_BACKEND_RAMFUNC
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_fine_minmax_inst_idx     0
#endif
#if (DEBUG_fine_minmax_inst_idx > 0)
#define TRACE_FCMII(...)		TRACE(__VA_ARGS__)
#define TRACENR_FCMII(...)	TRACENR(__VA_ARGS__)
#else
#define TRACE_FCMII(...)
#define TRACENR_FCMII(...)
#endif

static int fine_minmax_inst_idx(axis_t axis1, int instLen, float orgVal, float orgVal2, int modeCnt, int loopCnt)
{
    int i;
    vec_t *instNew = (vec_t *)&BS_minmax_inst_xy[0];
    float tmpVal, tmpVal2, diffVal, diffVal2;
    float minMaxdiffVal = MIN_INITIAL_VAL;
    int minMaxDiffIdx = -1;
    float manDist;

#if (DEBUG_fine_minmax_inst_idx > 0)
    IS_DEBUG_FLAG{ TRACE_FCMII("fine_minmax_inst_idx..: (%d-%d/%d), %0.2f, %0.2f", axis1, modeCnt, instLen, orgVal, orgVal2); };
#endif

    tmpVal2 = 0;
    for (i = 0; i < instLen; i++) {
        if (axis1 == ENUM_HOR_X) {
            tmpVal = instNew[i].y;
            tmpVal2 = instNew[i].x;
        }
        else {
            tmpVal = instNew[i].x;
            tmpVal2 = instNew[i].y;
        }
#if (DEBUG_fine_minmax_inst_idx > 0)
        if (loopCnt == 1) {
            IS_DEBUG_FLAG{ TRACE_FCMII("..loopCnt,tmpVal,orgVal: %d %0.2f %0.2f", loopCnt, tmpVal,orgVal);};
        }
#endif
#if 0
        if (loopCnt == 1) {
            //should be less than orgVal at first loop
            if (modeCnt == 0) {
                //min
                if (tmpVal < orgVal) continue;
            }
            else {
                //max
                if (tmpVal > orgVal) continue;
            }
        }
#endif
        //find closed intersection of min diff
        if (axis1 == ENUM_HOR_X) {
            diffVal2 = GET_ABS(tmpVal2 + EPSILON - orgVal2); //x-pos:0=>max
            if (modeCnt) {
                //max:max=>0
                diffVal = GET_ABS(orgVal + EPSILON - tmpVal); //nsmoon@211024 GE_ABS
            }
            else {
                //min:0=>max
                diffVal = GET_ABS(tmpVal + EPSILON - orgVal);
            }
        }
        else {
            diffVal2 = GET_ABS(orgVal2 + EPSILON - tmpVal2); //y-pos:max=>0
            if (modeCnt) {
                //max:max=>0
                diffVal = GET_ABS(orgVal + EPSILON - tmpVal);
            }
            else {
                //min:0=>max
                diffVal = GET_ABS(tmpVal + EPSILON - orgVal);
            }
        }
        manDist = diffVal2 + diffVal;

#if (DEBUG_fine_minmax_inst_idx > 0)
        IS_DEBUG_FLAG{ TRACE_FCMII("..i,diffVal,tmpVal,orgVal: (%d) %0.2f,%0.2f,%0.2f %0.2f,%0.2f,%0.2f", i, diffVal, tmpVal, orgVal, diffVal2, tmpVal2, orgVal2);};
        IS_DEBUG_FLAG{ TRACE_FCMII("..manDist,minMaxdiffVal: %f %f", manDist, minMaxdiffVal);};
#endif
        if (diffVal > 0 && diffVal2 > 0) {
            if (manDist < minMaxdiffVal) {
                minMaxdiffVal = manDist;
                minMaxDiffIdx = i;
            }
        }

    }
#if (DEBUG_fine_minmax_inst_idx > 0)
    IS_DEBUG_FLAG{ TRACE_FCMII(".minMaxDiffIdx,minMaxdiffVal: %d %0.2f", minMaxDiffIdx, minMaxdiffVal);};
#endif

    return minMaxDiffIdx; //-1:not-found
}

#ifdef ADJUST_MIN_MAX_FINE_NEW
static int fine_minmax_inst_idx2(axis_t axis1, int instLen, vec_t *pSav0, vec_t *pSav1, float orgVal, float orgVal2, int modeCnt, int loopCnt)
{
    int i;
    vec_t *instNew = (vec_t *)&BS_minmax_inst_xy[0];
    float tmpVal2; //tmpVal
    float minMaxdiffVal = MIN_INITIAL_VAL;
    int minMaxDiffIdx = -1;
    float manDist;
    int right, calFinish;

#if (DEBUG_fine_minmax_inst_idx > 0)
    IS_DEBUG_FLAG{ TRACE_FCMII("fine_minmax_inst_idx..: (%d-%d/%d), %0.2f, %0.2f", axis1, modeCnt, instLen, orgVal, orgVal2); };
#endif

    tmpVal2 = 0;
    for (i = 0; i < instLen; i++) {
        right = BS_left_of(pSav0, pSav1, &instNew[i]);
        calFinish = ((modeCnt == 0 && right == -1) || (modeCnt == 1 && right == 1));
        if (calFinish) {
            continue;
        }
        if (axis1 == ENUM_HOR_X) {
            //tmpVal = instNew[i].y;
            tmpVal2 = instNew[i].x;
        }
        else {
            //tmpVal = instNew[i].x;
            tmpVal2 = instNew[i].y;
        }
#if (DEBUG_fine_minmax_inst_idx > 0)
        if (loopCnt == 1) {
            IS_DEBUG_FLAG{ TRACE_FCMII("..loopCnt,tmpVal2,orgVal2: %d %0.2f %0.2f", loopCnt, tmpVal2, orgVal2);};
        }
#endif
#if 0
        if (loopCnt == 1) {
            //should be less than orgVal at first loop
            if (modeCnt == 0) {
                //min
                if (tmpVal < orgVal) continue;
            }
            else {
                //max
                if (tmpVal > orgVal) continue;
            }
        }
#endif
        //find closed intersection of min diff
        if (axis1 == ENUM_HOR_X) {
            manDist = GET_ABS(tmpVal2 + EPSILON - orgVal2); //x-pos:0=>max
        }
        else {
            manDist = GET_ABS(orgVal2 + EPSILON - tmpVal2); //y-pos:max=>0
        }

#if (DEBUG_fine_minmax_inst_idx > 0)
        IS_DEBUG_FLAG{ TRACE_FCMII("..i,manDist,tmpVal2,orgVal2,minMaxdiffVal: (%d) %0.2f,%0.2f, %0.2f,%0.2f", i, manDist, minMaxdiffVal, tmpVal2, orgVal2);};
#endif
        if (manDist < minMaxdiffVal) {
            minMaxdiffVal = manDist;
            minMaxDiffIdx = i;
        }
    }
#if (DEBUG_fine_minmax_inst_idx > 0)
    IS_DEBUG_FLAG{ TRACE_FCMII(".minMaxDiffIdx,minMaxdiffVal: %d %0.2f", minMaxDiffIdx, minMaxdiffVal);};
#endif

    return minMaxDiffIdx; //-1:not-found
}
#endif

ATTR_BACKEND_RAMFUNC
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define fineDEBUG_cal_min_max3     0
#endif
#if (fineDEBUG_cal_min_max3 > 0)
#define TRACE_FCMM3(...)		TRACE(__VA_ARGS__)
#define TRACE_FCMM32(...)		TRACE(__VA_ARGS__)
#define TRACENR_FCMM3(...)	TRACENR(__VA_ARGS__)
#else
#define TRACE_FCMM3(...)
#define TRACE_FCMM32(...)
#define TRACENR_FCMM3(...)
#endif
#if 0 //for test
#define CAL_MIN_MAX3_LOOP_CNT	5 //15 //nsmoon@211025 15=>5
#define CAL_FINISH_INIT      0
#define CAL_FINISH_NOINST    1
#define CAL_FINISH_NOMINIDX  2
#define CAL_FINISH_NEARERASE 3
#define CAL_FINISH_INVANGLE  4
#define CAL_FINISH_SMALLCURR 5
#define CAL_FINISH_SMALLPREV 6
#define CAL_FINISH_OUTOFRNG  7
ATTR_BACKEND_RAMFUNC
static int fine_cal_min_max3(axis_t axis1, int initialLineIdx, int initialGrpIdx,
                             axis_t axis2, pos_minMax2_t *min_max,
                             vec_t *retMinPos, vec_t *retMaxPos,
                             int chkSlope, int fineLoop)
{
    initial_line_t *initial_line;
    initial_line_group_t *grp;
    vec_t *instNew;
    uint16_t *statNew;
    vec_t p0, p1, p2, p3, pRet, pSav0, pSav1;
    float instStartPos_min, instStartPos_max, instStartPos2_min, instStartPos2_max;
    int instStartIdx_min, instStartIdx_max;
    float intSectPosTmp, intSectPosTmp2, intSectPosOrg, intSectPosOrg2, intSectNewPos, intSectNewPos2;
    float instRange2Min, instRange2Max, instRange;
    int intSectCnt = 0, intSectTmpIdx = -1, calFinish; //calFinish0;
    int instIdxMinMax = 0; //instIdxTmp
    int instStartLineIdxMin = -1, instStartLineIdxMax = -1;
    //int slopeValIdxMin, slopeValIdxMax;
    int j, loopCnt, modeCnt;
    vec_t *newPos, *inst; //*newMinPos, *newMaxPos;
    in_line_t *lineSen;
    int pd, led, pdMin, pdMax, ledMin, ledMax;
    float angleNew, angleMin;
    vec_t newSavPos;
    int newSavPosCnt;
    //float min_range, max_range;

#if (fineDEBUG_cal_min_max3 > 0)
    int debugColor;
#define DEBUG_COLOR_X_MIN (MY_COLOR - 1) //y-min, lime
#define DEBUG_COLOR_X_MAX (MY_COLOR - 2) //y-max, cyan
#define DEBUG_COLOR_Y_MIN (0) //(MY_COLOR - 3) //x-min, yellow
#define DEBUG_COLOR_Y_MAX (1) //(MY_COLOR - 5) //x-max, light coral
#endif
    IS_DEBUG_FLAG{
        TRACE_FCMM3("fine_cal_min_max3...(%d-%d-%d)(%d)%d(%0.1f,%0.1f)", axis1, initialLineIdx, initialGrpIdx, axis2, chkSlope, min_max->min, min_max->max);
        TRACE_NOP;
    }

    if (axis1 == ENUM_HOR_X) {
        initial_line = &BS_initial_line_x[0];
    }
    else {
        initial_line = &BS_initial_line_y[0];
    }
    inst = initial_line[initialLineIdx].inst;
    grp = initial_line[initialLineIdx].grp;
    lineSen = initial_line[initialLineIdx].lineSen;

#if 1 //nsmoon@191230
    pdMin = ledMin = MIN_INITIAL_VAL;
    pdMax = ledMax = 0;
    for (j = 0; j < grp[initialGrpIdx].len; j++) {
        int tmpIdx = grp[initialGrpIdx].instIdx[j];
        pd = lineSen[tmpIdx].pd;
        led = lineSen[tmpIdx].led;
        if (pd < pdMin || (pd == pdMin && led < ledMin)) {
            pdMin = pd;
            ledMin = led;
            instStartLineIdxMin = tmpIdx;
        }
        if (pd > pdMax || (pd == pdMax && led > ledMax)) {
            pdMax = pd;
            ledMax = led;
            instStartLineIdxMax = tmpIdx;
        }
    }
#endif

    //inst min/max idx of grp
    instStartIdx_min = grp[initialGrpIdx].instIdx[0];
    instStartIdx_max = grp[initialGrpIdx].instIdx[grp[initialGrpIdx].len - 1];

    //inst min/max pos of grp
    instStartPos_min = grp[initialGrpIdx].instPos.min;
    instStartPos_max = grp[initialGrpIdx].instPos.max;
    instStartPos2_min = grp[initialGrpIdx].instPos2.min;
    instStartPos2_max = grp[initialGrpIdx].instPos2.max;
#if (fineDEBUG_cal_min_max3 > 1)
    IS_DEBUG_FLAG{ TRACE_FCMM32(" ..instStartIdx_min,instStartIdx_max=%d,%d", instStartIdx_min, instStartIdx_max);};
    IS_DEBUG_FLAG{ TRACE_FCMM32(" ..instStartPos_min,instStartPos_max=%0.1f,%0.1f", instStartPos_min, instStartPos_max);};
    IS_DEBUG_FLAG{ TRACE_FCMM32(" ..instStartPos2_min,instStartPos2_max=%0.1f,%0.1f", instStartPos2_min, instStartPos2_max);};
#endif

    //init return value first
    if (min_max->min != instStartPos_min || min_max->max != instStartPos_max) {
        IS_DEBUG_FLAG{ TRACE_ERROR("ERROR! min_max= %0.2f/%0.2f, %0.2f/%0.2f", min_max->min, min_max->max, instStartPos_min, instStartPos_max); };
        //return 1; //error //nsmoon@211022 not-return
        min_max->min = instStartPos_min;
        min_max->max = instStartPos_max;
    }

    if (instStartLineIdxMin < 0 || instStartLineIdxMax < 0) {
        TRACE_ERROR("ERROR! invalid instStartLineIdxMin,instStartLineIdxMax");
        return 1; //error
    }
    if (instStartPos_min > instStartPos_max || instStartPos2_min > instStartPos2_max) {
        TRACE_ERROR("ERROR! invalid instStartPos_min,instStartPos_max,instStartPos2_min,instStartPos2_ma: %f,%f %f,%f", instStartPos_min, instStartPos_max, instStartPos2_min, instStartPos2_max);
        return 1; //error
    }

    //init return value
    (*retMinPos) = inst[instStartIdx_min];
    (*retMaxPos) = inst[instStartIdx_max];

#if 1  //nsmoon@211023
    if (chkSlope) {
        //check plus/minus slope
        int slopeVal, slopeDiff;
        int slopePlusMax = 0, slopeMinusMax = 0;
        int slopePlusMin = MIN_INITIAL_VAL, slopeMinusMin = MIN_INITIAL_VAL;
        for (j = 0; j < grp[initialGrpIdx].len; j++) {
            int tmpIdx = grp[initialGrpIdx].instIdx[j]; //org instIdx
            pd = lineSen[tmpIdx].pd;
            led = lineSen[tmpIdx].led;
            slopeVal = led - pd;
            if (slopeVal > 0) {
                if (slopeVal > slopePlusMax) {
                    slopePlusMax = slopeVal;
                }
                if (slopeVal < slopePlusMin) {
                    slopePlusMin = slopeVal;
                }
            }
            else if (slopeVal < 0) {
                if ((-slopeVal) > slopeMinusMax) {
                    slopeMinusMax = (-slopeVal);
                }
                if ((-slopeVal) < slopeMinusMin) {
                    slopeMinusMin = (-slopeVal);
                }
            }
        }
        if (slopePlusMax > 0 && slopeMinusMax > 0) {
            slopeDiff = slopePlusMax + slopeMinusMax;
        }
        else if (slopePlusMax == 0 && slopeMinusMax > 0) {
            slopeDiff = slopeMinusMax - slopeMinusMin;
        }
        else if (slopePlusMax > 0 && slopeMinusMax == 0) {
            slopeDiff = slopePlusMax - slopePlusMin;
        }
        else {
            IS_DEBUG_FLAG{ TRACE_SIZE_ADJ(" .no grp member****"); };
            return 1; //error
        }
        IS_DEBUG_FLAG{ TRACE_SIZE_ADJ(" .slopeDiff= %d (%d %d)", slopeDiff, slopePlusMax, slopeMinusMax); };

        if (slopeDiff < FINE_MIN_SLOPE_VAL) {
            IS_DEBUG_FLAG{ TRACE_SIZE_ADJ(" .flat slope**** %d", slopeDiff); };
            return 1; //error
        }
    }
#endif //1

#if (fineDEBUG_cal_min_max3 > 0)
    int pdTmp, ledTmp;
    pdTmp = lineSen[instStartLineIdxMin].pd;
    ledTmp = lineSen[instStartLineIdxMin].led;
    DEBUG_SHOW_LINE_PD_LED(axis2, pdTmp, ledTmp, 0/*MY_COLOR*//*2*/);
    //DEBUG_SHOW_POS(&inst[instStartLineIdxMin], 0.05f, 0.05f, MY_COLOR);

    pdTmp = lineSen[instStartLineIdxMax].pd;
    ledTmp = lineSen[instStartLineIdxMax].led;
    DEBUG_SHOW_LINE_PD_LED(axis2, pdTmp, ledTmp, 0/*MY_COLOR*//*2*/);
    //DEBUG_SHOW_POS(&inst[instStartLineIdxMax], 0.05f, 0.05f, MY_COLOR);
    IS_DEBUG_FLAG{
        //if (axis1 == ENUM_HOR_X)
        TRACE_NOP;
    };
#endif

    instNew = (vec_t *)&BS_minmax_inst_xy[0];
    statNew = (uint16_t *)&BS_minmax_inst_stat[0];

    if (axis1 == ENUM_HOR_X) {
        instRange2Min = BS_aarea_zero_x;
        instRange2Max = BS_aarea_end_x;
    }
    else {
        instRange2Min = BS_aarea_zero_y;
        instRange2Max = BS_aarea_end_y;
    }

    /////////////////////////////////////////////////////////////
    //cal min/max
    /////////////////////////////////////////////////////////////
    for (modeCnt = 0; modeCnt < 2; modeCnt++)
    {
        if (modeCnt == 0) {
            //cal min
            instIdxMinMax = instStartLineIdxMin;
            intSectPosOrg = instStartPos_min;
            intSectPosOrg2 = instStartPos2_min;
            newSavPos = inst[instStartIdx_min];
            if (axis1 == ENUM_HOR_X) {
                instRange = inst[instStartIdx_max].y;
            }
            else {
                instRange = inst[instStartIdx_max].x;
            }
            DEBUG_SHOW_POS(&inst[instStartIdx_max], 0.05f, 0.05f, MY_COLOR);
        }
        else {
            //cal max
            instIdxMinMax = instStartLineIdxMax;
            intSectPosOrg = instStartPos_max;
            intSectPosOrg2 = instStartPos2_max;
            newSavPos = inst[instStartIdx_max];
            if (axis1 == ENUM_HOR_X) {
                instRange = inst[instStartIdx_min].y;
            }
            else {
                instRange = inst[instStartIdx_min].x;
            }
            DEBUG_SHOW_POS(&inst[instStartIdx_min], 0.07f, 0.07f, MY_COLOR);
        }
        intSectNewPos = intSectNewPos2 = 0; //nsmoon@200602
        calFinish = CAL_FINISH_INIT;

    #if (fineDEBUG_cal_min_max3 > 0)
        if (axis1 == ENUM_HOR_X) {
            if (modeCnt == 0) {
                debugColor = DEBUG_COLOR_X_MIN;
            }
            else {
                debugColor = DEBUG_COLOR_X_MAX;
            }
        }
        else {
            if (modeCnt == 0) {
                debugColor = DEBUG_COLOR_Y_MIN;
            }
            else {
                debugColor = DEBUG_COLOR_Y_MAX;
            }
        }
    #endif
    #if (fineDEBUG_cal_min_max3 > 0)
        //IS_DEBUG_FLAG{ TRACE_FCMM32(" axis1,mode,instIdxMinMax,slopeValMinMax: (%d/%d)(%d,%d)(%0.1f,%0.1f)", axis1, modeCnt, instIdxMinMax, slopeValMinMax, instStartPosMin, instStartPosMax);};
        IS_DEBUG_FLAG{ TRACE_FCMM32(" ..mode,instIdxMinMax: %d %d %0.1f", modeCnt, instIdxMinMax, instRange);};
    #endif

        newSavPosCnt = 0;
        angleNew = angleMin = MIN_INITIAL_VAL_SQ; //nsmoon@211024
        BS_int_sect_group2_epf_used_cnt = 0; //reset
        loopCnt = 0;
        while (loopCnt++ < CAL_MIN_MAX3_LOOP_CNT) {
            BS_add_epf_used((uint16_t)instIdxMinMax);
            pd = lineSen[instIdxMinMax].pd;
            led = lineSen[instIdxMinMax].led;
            BS_getLinePosInBufSen(axis2, led, pd, &p0, &p1);
            if (loopCnt == 1) {
                pSav0 = p0;
                pSav1 = p1;
            }
            IS_DEBUG_FLAG{ TRACE_FCMM32("  loopCnt=(%d) %d(%d,%d)", loopCnt, instIdxMinMax, pd, led);};
    #if 0 //(fineDEBUG_cal_min_max3 > 0)
            if (loopCnt > 1) {
                DEBUG_SHOW_LINE_POS(&p0, &p1, debugColor);
            }
    #endif
            IS_DEBUG_FLAG{
                //if (axis1 == ENUM_HOR_X)
                TRACE_NOP;
            };

            ////////////////////////////////////////////////////////////
            // make new inst with belonged lines
            ////////////////////////////////////////////////////////////
            intSectCnt = 0;
            for (j = 0; j < grp[initialGrpIdx].len; j++) {
                int tmpIdx = grp[initialGrpIdx].instIdx[j]; //org instIdx
                if (BS_is_epf_used((uint16_t)tmpIdx)) continue;
                pd = lineSen[tmpIdx].pd;
                led = lineSen[tmpIdx].led;
                BS_getLinePosInBufSen(axis2, led, pd, &p2, &p3);
                if (BS_line_intersection(&p0, &p1, &p2, &p3, &pRet)) {
                    if (axis1 == ENUM_HOR_X) {
                        intSectPosTmp = pRet.y;
                        intSectPosTmp2 = pRet.x;
                    }
                    else {
                        intSectPosTmp = pRet.x;
                        intSectPosTmp2 = pRet.y;
                    }
#if 1 //nsmoon@211024
                    if (loopCnt > 1) {
                        int isRight = BS_left_of(&pSav0, &pSav1, &pRet); //0:colinear,-1:left,1:right
                        if (modeCnt == 0 && isRight < 0) {
                            continue;
                        }
                        else if (modeCnt == 1 && isRight > 0){
                            continue;
                        }
                    }
#endif
#if (fineDEBUG_cal_min_max3 > 2)
                    IS_DEBUG_FLAG{ TRACE_FCMM32("    ...intSectPosTmp2: %0.1f %0.1f %0.1f %0.1f %0.1f", intSectPosTmp2,instRange2Min,instRange2Max,intSectPosTmp,instRange);};
#endif
                    if ((intSectPosTmp2 + EPSILON) > instRange2Min &&
                            (intSectPosTmp2 - EPSILON) < instRange2Max) {
                        if ((modeCnt == 0 && (intSectPosTmp - EPSILON) < instRange) ||
                                (modeCnt == 1 && (intSectPosTmp + EPSILON) > instRange)) {
                            IS_DEBUG_FLAG{
                                TRACE_NOP;
                            };
#if (fineDEBUG_cal_min_max3 > 2)
                            //if (loopCnt == 2)
                            DEBUG_SHOW_POS(&pRet, 0.05f, 0.05f, MY_COLOR - 4); //gray
#endif
                            if (intSectCnt < MAX_MINMAX_INST_TBL)
                            {
                                instNew[intSectCnt] = pRet;
                                statNew[intSectCnt] = (uint16_t)tmpIdx; //org instIdx
                                intSectCnt++;
                            }
                            else {
                                TRACE_ERROR("ERROR! fine_cal_min_max2.1.invalid intSectCnt %d [%d]", intSectCnt, BG_frame_no);
                                continue;
                            }
                        }

                    }
                } //if (BS_line_intersection(&p0, &p1, &p2, &p3, &pRet))
            } //for (j = 0; j < grp[initialGrpIdx].len; j++)
            IS_DEBUG_FLAG{
                //if (axis1 == ENUM_HOR_X)
                TRACE_NOP;
            };

            intSectTmpIdx = -1;
            if (intSectCnt == 0)
            {
                //no intersect
                calFinish = CAL_FINISH_NOINST; //no-intsect
#if (fineDEBUG_cal_min_max3 > 0)
                IS_DEBUG_FLAG{ TRACE_FCMM32("   no-intsect~~(%d)", calFinish);};
#endif
                break;
            }

            ////////////////////////////////////////////////////////////
            // find min/max inst pos in new inst
            ////////////////////////////////////////////////////////////
            calFinish = CAL_FINISH_INIT;
            if (loopCnt > 1) {
                intSectTmpIdx = fine_minmax_inst_idx(axis1, intSectCnt, intSectPosOrg, intSectPosOrg2, modeCnt, loopCnt); //new instIdx
            }
            else {
                float tmpPos, tmpPos2;
                if (axis1 == ENUM_HOR_X) {
                    tmpPos = p1.y;
                    tmpPos2 = p1.x;
                }
                else {
                    tmpPos = p1.x;
                    tmpPos2 = p1.y;
                }
                intSectTmpIdx = fine_minmax_inst_idx(axis1, intSectCnt, tmpPos, tmpPos2, modeCnt, loopCnt); //new instIdx
            }
            if (intSectTmpIdx < 0) {
                calFinish = CAL_FINISH_NOMINIDX; //no-minIdx //nsmoon@211023 1=>2
#if (fineDEBUG_cal_min_max3 > 0)
                IS_DEBUG_FLAG{ TRACE_FCMM32("   no-minIdx~~(%d)", calFinish);};
#endif
                break;
            }

            //get newPos
            newPos = &instNew[intSectTmpIdx];
            if (axis1 == ENUM_HOR_X) {
                intSectNewPos = newPos->y;
                intSectNewPos2 = newPos->x;
            }
            else {
                intSectNewPos = newPos->x;
                intSectNewPos2 = newPos->y;
            }

            intSectPosOrg = intSectNewPos;
            intSectPosOrg2 = intSectNewPos2;
            pSav0 = p0;
            pSav1 = p1;

            //cal angle
            instIdxMinMax = statNew[intSectTmpIdx];
            pd = lineSen[instIdxMinMax].pd;
            led = lineSen[instIdxMinMax].led;
            BS_getLinePosInBufSen(axis2, led, pd, &p2, &p3);
            BS_3point_angle(&p1, &p2, newPos, &angleNew);
            if (angleNew < angleMin) {
                if ((modeCnt == 0 && intSectPosOrg > min_max->min) ||
                        (modeCnt == 1 && intSectPosOrg < min_max->max)) {
                    if (modeCnt == 0) {
                        min_max->min = intSectPosOrg;
                    }
                    else {
                        min_max->max = intSectPosOrg;
                    }
                    angleMin = angleNew;
                    newSavPos = (*newPos);
                    newSavPosCnt++;
                }
            }
#if (fineDEBUG_cal_min_max3 > 0)
            //if (loopCnt == 2)
            DEBUG_SHOW_POS(newPos, 0.3f, 0.3f, debugColor);
            IS_DEBUG_FLAG{
                TRACE_FCMM32("   ..newPos=(%d/%d/%d)(%0.1f,%0.1f)(%0.1f,%0.1f)", axis1, pd, led, newPos->x, newPos->y, angleNew, angleMin);
            };
#endif
        } //while (loopCnt++ < CAL_MIN_MAX3_LOOP_CNT)
        IS_DEBUG_FLAG {
            TRACE_NOP;
        };
        if (newSavPosCnt) {
            if (axis1 == ENUM_HOR_X) {
                intSectPosOrg = newSavPos.y;
                intSectPosOrg2 = newSavPos.x;
            }
            else {
                intSectPosOrg = newSavPos.x;
                intSectPosOrg2 = newSavPos.y;
            }
            if (modeCnt == 0) {
                (*retMinPos) = newSavPos;
            }
            else {
                (*retMaxPos) = newSavPos;
            }
        }

        //save min/max, nsmoon@200223
        if (modeCnt == 0) {
            min_max->min = intSectPosOrg;
        }
        else {
            min_max->max = intSectPosOrg;
        }
    } //for (modeCnt = 0; modeCnt < 2; modeCnt++)

    IS_DEBUG_FLAG{
        TRACE_NOP;
    };
    return 0; //no-error
}
#else
#define CAL_MIN_MAX3_LOOP_CNT	15 //FIXME
ATTR_BACKEND_RAMFUNC
static int fine_cal_min_max3(axis_t axis1, int initialLineIdx, int initialGrpIdx,
                             axis_t axis2, pos_minMax2_t *min_max,
                             vec_t *retMinPos, vec_t *retMaxPos,
                             int chkSlope, int fineLoop/*nearErase*/)
{
    initial_line_t *initial_line;
    initial_line_group_t *grp;
    vec_t *instNew;
    uint16_t *statNew;
    vec_t p0, p1, p2, p3, pRet, pSav0, pSav1;
    float instStartPos_min, instStartPos_max, instStartPos2_min, instStartPos2_max;
    int instStartIdx_min, instStartIdx_max;
    float intSectPosTmp, intSectPosTmp2, intSectPosOrg, intSectPosOrg2, intSectNewPos, intSectNewPos2;
    float instRange2Min, instRange2Max, instRange;
    int intSectCnt = 0, intSectTmpIdx = -1, calFinish;
    int tmpIdx, instIdxMinMax = 0;
    int instStartLineIdxMin = -1, instStartLineIdxMax = -1;
    //int slopeValIdxMin, slopeValIdxMax;
    int j, loopCnt, modeCnt;
    vec_t *newMinPos, *newMaxPos, *inst, newSavPos;
    in_line_t *lineSen;
    int pd, led, pdMin, pdMax, ledMin, ledMax;

#if (fineDEBUG_cal_min_max3 > 0)
    int debugColor;
#define DEBUG_COLOR_X_MIN (MY_COLOR - 1) //y-min, lime
#define DEBUG_COLOR_X_MAX (MY_COLOR - 2) //y-max, cyan
#define DEBUG_COLOR_Y_MIN (MY_COLOR - 4) //x-min, yellow
#define DEBUG_COLOR_Y_MAX (MY_COLOR - 5) //x-max, light coral
#endif
    IS_DEBUG_FLAG{
        TRACE_FCMM3("fine_cal_min_max3...(%d-%d-%d)(%d)%d", axis1, initialLineIdx, initialGrpIdx, axis2, chkSlope);
        TRACE_NOP;
    }

    if (axis1 == ENUM_HOR_X) {
        initial_line = &BS_initial_line_x[0];
    }
    else {
        initial_line = &BS_initial_line_y[0];
    }
    inst = initial_line[initialLineIdx].inst;
    grp = initial_line[initialLineIdx].grp;
    lineSen = initial_line[initialLineIdx].lineSen;

#if 0 //nsmoon@200309, FIXME!!!
    float diffTmp = GET_ABS(min_max->max - min_max->min);
    if (diffTmp < FINE_MINMAX_WIDTH_MIN) {
        IS_DEBUG_FLAG{ TRACE_FCMM3(".skip~diffTmp: (%d) %0.1f", axis2, diffTmp); };
        return 1; //error
    }
#endif

#if 1 //nsmoon@200228
    if (chkSlope) { //nsmoon@200309
        //check plus/minus slope
        int slopeVal, slopePlusCnt = 0, slopeMinusCnt = 0;
        int slopeMin = MIN_INITIAL_VAL;
        int slopeMax = 0;
        for (j = 0; j < grp[initialGrpIdx].len; j++) {
            tmpIdx = grp[initialGrpIdx].instIdx[j]; //org instIdx
            pd = lineSen[tmpIdx].pd;
            led = lineSen[tmpIdx].led;
            slopeVal = led - pd;
            //TRACE(" slopeVal=%d %d %d", slopeVal, led, pd);
            if (slopeVal > 0) {
                slopePlusCnt++;
            }
            else if (slopeVal < 0) {
                slopeMinusCnt++;
            }
            if (slopeVal < slopeMin) {
                slopeMin = slopeVal;
        }
            if (slopeVal > slopeMax) {
                slopeMax = slopeVal;
            }
        }
        IS_DEBUG_FLAG{ 
            TRACE_FCMM3(".slope= %d,%d (%d,%d) (%d)", slopePlusCnt, slopeMinusCnt, slopeMin, slopeMax, initial_line[initialLineIdx].sortLen);
        };
        //if (slopePlusCnt < FINE_MIN_SLOPE_CNT_GRP || slopeMinusCnt < FINE_MIN_SLOPE_CNT_GRP)
        if (GET_ABS(slopeMax - slopeMin) < FINE_MIN_SLOPE_VAL) //nsmoon@211028
        {
            IS_DEBUG_FLAG{ TRACE_FCMM3(".no slope****"); };
            //TRACE_RELEASE(".noslope= %d,%d (%d)", slopePlusCnt, slopeMinusCnt, initial_line[initialLineIdx].sortLen);
                return 1; //error //nsmoon@211026, if ignore this error, occurs pen size error
        }
    }
#endif //1

#if 1 //nsmoon@191230
    pdMin = ledMin = MIN_INITIAL_VAL;
    pdMax = ledMax = 0;
    for (j = 0; j < grp[initialGrpIdx].len; j++) {
        tmpIdx = grp[initialGrpIdx].instIdx[j];
        pd = lineSen[tmpIdx].pd;
        led = lineSen[tmpIdx].led;
        if (pd < pdMin || (pd == pdMin && led < ledMin)) {
            pdMin = pd;
            ledMin = led;
            instStartLineIdxMin = tmpIdx;
        }
        if (pd > pdMax || (pd == pdMax && led > ledMax)) {
            pdMax = pd;
            ledMax = led;
            instStartLineIdxMax = tmpIdx;
        }
    }
#endif
#if 0 //for test
    if (instStartLineIdxMin >= 0) {
        slopeValIdxMin = GET_ABS(lineSen[instStartLineIdxMin].led - lineSen[instStartLineIdxMin].pd);
    }
    else {
        slopeValIdxMin = 0;
    }
    if (instStartLineIdxMin >= 0) {
        slopeValIdxMax = GET_ABS(lineSen[instStartLineIdxMax].led - lineSen[instStartLineIdxMax].pd);
    }
    else {
        slopeValIdxMax = 0;
    }
    IS_DEBUG_FLAG{ TRACE_FCMM3("..pdMin,pdMax=%d,%d (%d,%d)", pdMin, pdMax, slopeValIdxMin, slopeValIdxMax);};
#endif

    //inst min/max idx of grp
    instStartIdx_min = grp[initialGrpIdx].instIdx[0];
    instStartIdx_max = grp[initialGrpIdx].instIdx[grp[initialGrpIdx].len - 1];

    //inst min/max pos of grp
    instStartPos_min = grp[initialGrpIdx].instPos.min;
    instStartPos_max = grp[initialGrpIdx].instPos.max;
    instStartPos2_min = grp[initialGrpIdx].instPos2.min;
    instStartPos2_max = grp[initialGrpIdx].instPos2.max;
#if (fineDEBUG_cal_min_max3 > 1)
    IS_DEBUG_FLAG{ TRACE_FCMM32("..instStartIdx_min,instStartIdx_max=%d,%d", instStartIdx_min, instStartIdx_max);};
    IS_DEBUG_FLAG{ TRACE_FCMM32("..instStartPos_min,instStartPos_max=%0.1f,%0.1f", instStartPos_min, instStartPos_max);};
    IS_DEBUG_FLAG{ TRACE_FCMM32("..instStartPos2_min,instStartPos2_max=%0.1f,%0.1f", instStartPos2_min, instStartPos2_max);};
#endif

    //init return value first
    if (min_max->min != instStartPos_min || min_max->max != instStartPos_max) {
        IS_DEBUG_FLAG{ TRACE_ERROR("ERROR! fine_cal_min_max3..invalid min_max %0.2f/%0.2f, %0.2f/%0.2f", min_max->min, min_max->max, instStartPos_min, instStartPos_max); };
        return 1; //error
    }

    if (instStartLineIdxMin < 0 || instStartLineIdxMax < 0) {
#if (fineDEBUG_cal_min_max3 > 0)
        TRACE_ERROR("ERROR! invalid instStartLineIdxMin,instStartLineIdxMax");
#endif
        return 1; //error
    }
    if (instStartPos_min > instStartPos_max || instStartPos2_min > instStartPos2_max) {
#if (fineDEBUG_cal_min_max3 > 0)
        TRACE_ERROR("ERROR! invalid instStartPos_min,instStartPos_max,instStartPos2_min,instStartPos2_ma: %f,%f %f,%f", instStartPos_min, instStartPos_max, instStartPos2_min, instStartPos2_max);
#endif
        return 1; //error
    }

    newMinPos = &inst[instStartIdx_min];
    newMaxPos = &inst[instStartIdx_max];
#if 0 //nsmoon@200223
    float tmp_pos;
    if (newMinPos->x > newMaxPos->x) {
        tmp_pos = newMaxPos->x;
        newMaxPos->x = newMinPos->x;
        newMinPos->x = tmp_pos;
    }
    if (newMinPos->y > newMaxPos->y) {
        tmp_pos = newMaxPos->y;
        newMaxPos->y = newMinPos->y;
        newMinPos->y = tmp_pos;
    }
#endif
    *retMinPos = *newMinPos;
    *retMaxPos = *newMaxPos;
#if (fineDEBUG_cal_min_max3 > 1)
    IS_DEBUG_FLAG{ TRACE_FCMM32("..retMinPos->x,retMaxPos->x=%0.1f,%0.1f", newMinPos->x, newMaxPos->x);};
    IS_DEBUG_FLAG{ TRACE_FCMM32("..retMinPos->y,retMaxPos->y=%0.1f,%0.1f", newMinPos->y, newMaxPos->y);};
    float centTmpX = (newMinPos->x + newMaxPos->x) * 0.5f;
    float centTmpY = (newMinPos->y + newMaxPos->y) * 0.5f;
    IS_DEBUG_FLAG{ TRACE_FCMM32("..centTmpX,centTmpY=%0.1f,%0.1f", centTmpX, centTmpY); };
#endif

#if (fineDEBUG_cal_min_max3 > 0)
    int pdTmp, ledTmp;
    pdTmp = lineSen[instStartLineIdxMin].pd;
    ledTmp = lineSen[instStartLineIdxMin].led;
    DEBUG_SHOW_LINE_PD_LED(axis2, pdTmp, ledTmp, MY_COLOR/*2*/);
    DEBUG_SHOW_POS(&inst[instStartLineIdxMin], 0.05f, 0.05f, MY_COLOR);

    pdTmp = lineSen[instStartLineIdxMax].pd;
    ledTmp = lineSen[instStartLineIdxMax].led;
    DEBUG_SHOW_LINE_PD_LED(axis2, pdTmp, ledTmp, MY_COLOR/*2*/);
    DEBUG_SHOW_POS(&inst[instStartLineIdxMax], 0.05f, 0.05f, MY_COLOR);
    IS_DEBUG_FLAG{
        //if (axis1 == ENUM_HOR_X)
        TRACE_NOP;
    };
#endif

    instNew = (vec_t *)&BS_minmax_inst_xy[0];
    statNew = (uint16_t *)&BS_minmax_inst_stat[0];

    if (axis1 == ENUM_HOR_X) {
        instRange2Min = BS_aarea_zero_x;
        instRange2Max = BS_aarea_end_x;
    }
    else {
        instRange2Min = BS_aarea_zero_y;
        instRange2Max = BS_aarea_end_y;
    }

    /////////////////////////////////////////////////////////////
    //cal min/max
    /////////////////////////////////////////////////////////////
    for (modeCnt = 0; modeCnt < 2; modeCnt++) {
        if (modeCnt == 0) {
            //cal min
            instIdxMinMax = instStartLineIdxMin;
            intSectPosOrg = instStartPos_min;
            intSectPosOrg2 = instStartPos2_min;
            if (axis1 == ENUM_HOR_X) {
                instRange = inst[instStartIdx_max].y;
            }
            else {
                instRange = inst[instStartIdx_max].x;
            }
            newSavPos = *newMinPos; //nsmoon@200223
        }
        else {
            //cal max
            instIdxMinMax = instStartLineIdxMax;
            intSectPosOrg = instStartPos_max;
            intSectPosOrg2 = instStartPos2_max;
            if (axis1 == ENUM_HOR_X) {
                instRange = inst[instStartIdx_min].y;
            }
            else {
                instRange = inst[instStartIdx_min].x;
            }
            newSavPos = *newMaxPos; //nsmoon@200223
        }
        intSectNewPos = intSectNewPos2 = 0; //nsmoon@200602
        calFinish = 0;

    #if (fineDEBUG_cal_min_max3 > 0)
        if (axis1 == ENUM_HOR_X) {
            if (modeCnt == 0) {
                debugColor = DEBUG_COLOR_X_MIN;
            }
            else {
                debugColor = DEBUG_COLOR_X_MAX;
            }
        }
        else {
            if (modeCnt == 0) {
                debugColor = DEBUG_COLOR_Y_MIN;
            }
            else {
                debugColor = DEBUG_COLOR_Y_MAX;
            }
        }
    #endif
    #if (fineDEBUG_cal_min_max3 > 1)
        //IS_DEBUG_FLAG{ TRACE_FCMM32("axis1,mode,instIdxMinMax,slopeValMinMax: (%d/%d)(%d,%d)(%0.1f,%0.1f)", axis1, modeCnt, instIdxMinMax, slopeValMinMax, instStartPosMin, instStartPosMax);};
        IS_DEBUG_FLAG{ TRACE_FCMM32("..mode,instIdxMinMax: %d %d %0.1f", modeCnt, instIdxMinMax, instRange);};
    #endif
        BS_int_sect_group2_epf_used_cnt = 0; //reset
        loopCnt = 0;
        while (loopCnt++ < CAL_MIN_MAX3_LOOP_CNT)
        {
            //IS_DEBUG_FLAG{ TRACE_FCMM32("..: %d", instIdxMinMax);};
            BS_add_epf_used((uint16_t)instIdxMinMax);
            pd = lineSen[instIdxMinMax].pd;
            led = lineSen[instIdxMinMax].led;
            BS_getLinePosInBufSen(axis2, led, pd, &p0, &p1);
            if (loopCnt == 1) {
                pSav0 = p0;
                pSav1 = p1;
            }
    #if (fineDEBUG_cal_min_max3 > 0)
            if (loopCnt > 1) {
                DEBUG_SHOW_LINE_POS(&p0, &p1, debugColor);
            }
    #endif
            IS_DEBUG_FLAG{
                //if (axis1 == ENUM_HOR_X)
                TRACE_NOP;
            };

            ////////////////////////////////////////////////////////////
            // make new inst with belonged lines
            ////////////////////////////////////////////////////////////
            intSectCnt = 0;
            for (j = 0; j < grp[initialGrpIdx].len; j++) {
                tmpIdx = grp[initialGrpIdx].instIdx[j]; //org instIdx
                if (BS_is_epf_used((uint16_t)tmpIdx)) continue;
                pd = lineSen[tmpIdx].pd;
                led = lineSen[tmpIdx].led;
                BS_getLinePosInBufSen(axis2, led, pd, &p2, &p3);
                if (BS_line_intersection(&p0, &p1, &p2, &p3, &pRet)) {
                    if (axis1 == ENUM_HOR_X) {
                        intSectPosTmp = pRet.y;
                        intSectPosTmp2 = pRet.x;
                    }
                    else {
                        intSectPosTmp = pRet.x;
                        intSectPosTmp2 = pRet.y;
                    }
#if (fineDEBUG_cal_min_max3 > 2)
                    IS_DEBUG_FLAG{ TRACE_FCMM32("..intSectPosTmp2: %0.1f %0.1f %0.1f %0.1f %0.1f", intSectPosTmp2,instRange2Min,instRange2Max,intSectPosTmp,instRange);};
#endif
                    if ((intSectPosTmp2 + EPSILON) > instRange2Min && (intSectPosTmp2 - EPSILON) < instRange2Max)
                    {
                        if ((modeCnt == 0 && (intSectPosTmp - EPSILON) < instRange) || (modeCnt == 1 && (intSectPosTmp + EPSILON) > instRange))
                        {
                            IS_DEBUG_FLAG{
                                TRACE_NOP;
                            };
#if (DEBUG_fine_cal_min_max > 2)
                            DEBUG_SHOW_POS(&pRet, 0.05f, 0.05f, MY_COLOR - 4); //gray
#endif
                            if (intSectCnt < MAX_MINMAX_INST_TBL)
                            {
                                instNew[intSectCnt] = pRet;
                                statNew[intSectCnt] = (uint16_t)tmpIdx; //org instIdx
                                intSectCnt++;
                            }
                            else {
                                TRACE_ERROR("ERROR! fine_cal_min_max2.1.invalid intSectCnt %d [%d]", intSectCnt, BG_frame_no);
                                continue;
                            }
                        }

                    }
                } //if (BS_line_intersection(&p0, &p1, &p2, &p3, &pRet))
            } //for (j = 0; j < grp[initialGrpIdx].len; j++)
            IS_DEBUG_FLAG{
                //if (axis1 == ENUM_HOR_X)
                TRACE_NOP;
            };
            if (intSectCnt == 0)
            {
                //no intersect
    #if (fineDEBUG_cal_min_max3 > 1)
                if (modeCnt == 0) {
                    IS_DEBUG_FLAG{ TRACE_FCMM32(".==>min_max->min: %0.2f (%0.2f)", min_max->min, intSectPosOrg);};
                }
                else {
                    IS_DEBUG_FLAG{ TRACE_FCMM32(".==>min_max->max: %0.2f (%0.2f)", min_max->max, intSectPosOrg);};
                }
    #endif
                calFinish = 1;
                intSectTmpIdx = -1;
#if (fineDEBUG_cal_min_max3 > 0)
                IS_DEBUG_FLAG{ TRACE_FCMM32("  no-intersect~~(%d)");};
    #endif
            }
            else {
                ////////////////////////////////////////////////////////////
                // find min/max inst pos in new inst
                ////////////////////////////////////////////////////////////
                calFinish = 0;
                if (loopCnt > 1) {
#ifdef ADJUST_MIN_MAX_FINE_NEW //nsmoon@220308a
                    intSectTmpIdx = fine_minmax_inst_idx2(axis1, intSectCnt, &pSav0, &pSav1, intSectPosOrg, intSectPosOrg2, modeCnt, loopCnt); //new instIdx
#else
                    intSectTmpIdx = fine_minmax_inst_idx(axis1, intSectCnt, intSectPosOrg, intSectPosOrg2, modeCnt, loopCnt); //new instIdx
#endif
                }
                else {
                    float tmpPos, tmpPos2;
                    if (axis1 == ENUM_HOR_X) {
                            tmpPos = p1.y;
                            tmpPos2 = p1.x;
                        }
                        else {
                            tmpPos = p1.x;
                            tmpPos2 = p1.y;
                        }
                    intSectTmpIdx = fine_minmax_inst_idx(axis1, intSectCnt, tmpPos, tmpPos2, modeCnt, loopCnt); //new instIdx
                }
                if (intSectTmpIdx < 0) {
                    calFinish = 1;
#if (fineDEBUG_cal_min_max3 > 0)
                    IS_DEBUG_FLAG{ TRACE_FCMM32("  no-minIdx~~(%d)", calFinish);};
#endif
                }
                else {
                    //float opAxisDist;
                    if (axis1 == ENUM_HOR_X) {
                        //opAxisDist = (intSectNewPos2 == 0) ? 0 : GET_ABS(instNew[intSectTmpIdx].x - intSectNewPos2);
                        //IS_DEBUG_FLAG{ TRACE_FCMM3("opAxisDist= %0.1f (%0.1f %0.1f)", opAxisDist, instNew[intSectTmpIdx].x, intSectNewPos2);};
                        intSectNewPos = instNew[intSectTmpIdx].y;
                        intSectNewPos2 = instNew[intSectTmpIdx].x;
                    }
                    else {
                        //opAxisDist = (intSectNewPos2 == 0) ? 0 : GET_ABS(instNew[intSectTmpIdx].y - intSectNewPos2);
                        //IS_DEBUG_FLAG{ TRACE_FCMM3("opAxisDist= %0.1f (%0.1f %0.1f)", opAxisDist, instNew[intSectTmpIdx].y, intSectNewPos2);};
                        intSectNewPos = instNew[intSectTmpIdx].x;
                        intSectNewPos2 = instNew[intSectTmpIdx].y;
                    }

                    if (modeCnt == 0) {
                        newMinPos = &instNew[intSectTmpIdx];
                    }
                    else {
                        newMaxPos = &instNew[intSectTmpIdx];
                    }
#if (fineDEBUG_cal_min_max3 > 0)
                    DEBUG_SHOW_POS(&instNew[intSectTmpIdx], 0.3f, 0.3f, debugColor);
#endif

                    int right = 0; //0:colinear,-1:left,1:right
#ifndef ADJUST_MIN_MAX_FINE_NEW //nsmoon@220308a
                    if (loopCnt > 1) {
                        right = BS_left_of(&pSav0, &pSav1, &instNew[intSectTmpIdx]);
                    }
#endif
#if (fineDEBUG_cal_min_max3 > 1)
                    IS_DEBUG_FLAG{
                        TRACE_FCMM32("..intSectNewPos=(%d-%d)(%d) %0.2f/%0.2f (%d)", modeCnt, loopCnt, intSectTmpIdx, intSectNewPos, intSectPosOrg, right);
                    };
#endif
                    //calculation finished
#if 0
                    if (loopCnt == 1) {
                        calFinish = ((modeCnt == 0 && (intSectNewPos - EPSILON) < intSectPosOrg) ||
                            (modeCnt == 1 && (intSectNewPos + EPSILON) > intSectPosOrg));
                    }
                    else
#endif
                    {
                        //calFinish = ((axis1 == ENUM_HOR_X && (modeCnt == 0 && right == -1) || (modeCnt == 1 && right == 1)) ||
                        //             (axis1 == ENUM_VER_Y && (modeCnt == 0 && right == -1) || (modeCnt == 1 && right == 1)));
                        calFinish = ((modeCnt == 0 && right == -1) || (modeCnt == 1 && right == 1));
                    }
#if 0  //nsmoon@200602 //nsmoon@211026 not-used
                    #define MAX_OPAXIS_DIST 10.0f //200.0f
                    if (nearErase && opAxisDist > MAX_OPAXIS_DIST) {
                        calFinish = 1;
                    }
#endif
                }
#if (fineDEBUG_cal_min_max3 > 0)
                IS_DEBUG_FLAG{
                    TRACE_NOP;
                };
                if (calFinish) {
                    if (modeCnt == 0) {
                        IS_DEBUG_FLAG{ TRACE_FCMM32("..=>min_max->min: %0.2f (%0.2f)", min_max->min, intSectPosOrg); };
                    }
                    else {
                        IS_DEBUG_FLAG{ TRACE_FCMM32("..=>min_max->max: %0.2f (%0.2f)", min_max->max, intSectPosOrg); };
                    }
                }
#endif
            }
            IS_DEBUG_FLAG{
                if (modeCnt == 0) {
                    IS_DEBUG_FLAG{ TRACE_FCMM32("..intSectPosOrg=(%d) %0.2f/%0.2f (%0.2f,%0.2f)", modeCnt, intSectPosOrg, min_max->min, newSavPos.x, newSavPos.y);};
                }
                else {
                    IS_DEBUG_FLAG{ TRACE_FCMM32("..intSectPosOrg=(%d) %0.2f/%0.2f (%0.2f,%0.2f)", modeCnt, intSectPosOrg, min_max->max, newSavPos.x, newSavPos.y);};
                }
                TRACE_NOP;
            };
            //save min/max, nsmoon@200223
            if (modeCnt == 0) {
                if (min_max->min < intSectPosOrg) {
                    min_max->min = intSectPosOrg;
                    *retMinPos = newSavPos;
                }
            }
            else {
                if (min_max->max > intSectPosOrg) {
                    min_max->max = intSectPosOrg;
                    *retMaxPos = newSavPos;
                }
            }

            if (calFinish)
            {
                break;
            }
            else {
                if (intSectTmpIdx >= 0 && intSectTmpIdx < MAX_MINMAX_INST_TBL) {
                //replace posOrg and instIdxMinMax
                intSectPosOrg = intSectNewPos;
                intSectPosOrg2 = intSectNewPos2;
                instIdxMinMax = statNew[intSectTmpIdx];
                pSav0 = p0;
                pSav1 = p1;
                newSavPos = (modeCnt) ? *newMaxPos : *newMinPos; //nsmoon@200223
            }
                else {
                    TRACE_ERROR("ERROR! invalid intSectTmpIdx %d", intSectTmpIdx);
                    break;
                }
            }
        } //while (loopCnt++ < CAL_MIN_MAX3_LOOP_CNT)
        if (loopCnt >= CAL_MIN_MAX3_LOOP_CNT) {
            TRACE_ERROR("ERROR! fine_cal_min_max2..could not find minIdx, loopCnt %d", loopCnt);
        }
    } //for (modeCnt = 0; modeCnt < 2; modeCnt++)

    IS_DEBUG_FLAG{
        TRACE_NOP;
    };
    return 0; //no-error
}
#endif

ATTR_BACKEND_RAMFUNC
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_fine_cal_min_max5   1
#endif
#if (DEBUG_fine_cal_min_max5 > 1)
#define TRACE_FCMM5(...)		TRACE(__VA_ARGS__)
#define TRACENR_FCMM5(...)	TRACENR(__VA_ARGS__)
#else
#define TRACE_FCMM5(...)
#define TRACENR_FCMM5(...)
#endif

#if 0 //for test //nsmoon@211018 1=>0
#define CAL_MIN_MAX5_LOOP_CNT	15 //FIXME
ATTR_BACKEND_RAMFUNC
static int fine_cal_min_max5(axis_t axis1, initial_line_a_t *initial_line, int initialLineIdx, int cxpIdx, 
                             axis_t axis2, pos_minMax2_t *min_max, 
                             vec_t *retMinPos, vec_t *retMaxPos)
{
    //initial_line_a_t *initial_line;
    initial_line_group_t *grp;
    vec_t *instNew;
    uint16_t *statNew;
    vec_t p0, p1, p2, p3, pRet, pSav0, pSav1;
    float instStartPos_min, instStartPos_max, instStartPos2_min, instStartPos2_max;
    int instStartIdx_min, instStartIdx_max;
    float intSectPosTmp, intSectPosTmp2, intSectPosOrg, intSectPosOrg2;
    float intSectNewPos = 0, intSectNewPos2 = 0;
    float instRange2Min, instRange2Max, instRange;
    int intSectCnt = 0, intSectTmpIdx, calFinish;
    int tmpIdx, instIdxMinMax = 0;
    int instStartLineIdxMin = -1, instStartLineIdxMax = -1;
    int j, loopCnt, modeCnt;
    vec_t *newMinPos, *newMaxPos, *inst;
    float tmp_pos;
    in_line_t *lineSen;
    //axis_t axis2;
    //uint16_t *remLine2;
    //int remLine2Idx;
    //uint8_t *lineStat;

#if (DEBUG_fine_cal_min_max5 > 0)
    int debugColor;
#define DEBUG_COLOR_X_MIN (MY_COLOR - 1) //y-min, lime
#define DEBUG_COLOR_X_MAX (MY_COLOR - 2) //y-max, cyan
#define DEBUG_COLOR_Y_MIN (MY_COLOR - 3) //x-min, yellow
#define DEBUG_COLOR_Y_MAX (MY_COLOR - 5) //x-max, light coral
#endif
    IS_DEBUG_FLAG{
        TRACE_NOP;
    }

    inst = initial_line[initialLineIdx].inst_a[cxpIdx];
    grp = initial_line[initialLineIdx].grp_a[cxpIdx];
#ifdef FINE_CLIPPING_NEW
    //lineStat = initial_line[initialLineIdx].lineStat
    lineSen = initial_line[initialLineIdx].lineSen_a[cxpIdx];
#endif

#if 1 //nsmoon@191230
    int pd, led, pdMin, pdMax, ledMin, ledMax;
    pdMin = ledMin = MIN_INITIAL_VAL;
    pdMax = ledMax = 0;
    for (j = 0; j < grp->len; j++) {
        tmpIdx = grp->instIdx[j];
        pd = lineSen[tmpIdx].pd;
        led = lineSen[tmpIdx].led;
        //IS_DEBUG_FLAG{ TRACE_FCMM5("tmpIdx=%d(%d,%d)", tmpIdx, pd, led);};
        if (pd < pdMin || (pd == pdMin && led < ledMin)) {
            pdMin = pd;
            ledMin = led;
            instStartLineIdxMin = tmpIdx;
        }
        if (pd > pdMax || (pd == pdMax && led > ledMax)) {
            pdMax = pd;
            ledMax = led;
            instStartLineIdxMax = tmpIdx;
        }
    }
    //IS_DEBUG_FLAG{ TRACE_FCMM5("pdMin,pdMax=(%d)%d,%d", axis2, pdMin, pdMax);};
#endif

    //inst min/max idx of grp
    instStartIdx_min = grp->instIdx[0];
    instStartIdx_max = grp->instIdx[grp->len - 1];

    //inst min/max pos of grp
    instStartPos_min = grp->instPos.min;
    instStartPos_max = grp->instPos.max;
    instStartPos2_min = grp->instPos2.min;
    instStartPos2_max = grp->instPos2.max;

    //init return value first
    min_max->min = instStartPos_min;
    min_max->max = instStartPos_max;

    if (instStartLineIdxMin < 0 || instStartLineIdxMax < 0) {
        TRACE_ERROR("ERROR! invalid instStartLineIdxMin,instStartLineIdxMax");
        return 1; //error
    }
    if (instStartPos_min > instStartPos_max || instStartPos2_min > instStartPos2_max) {
        TRACE_ERROR("ERROR! invalid instStartPos_min,instStartPos_max,instStartPos2_min,instStartPos2_ma: %f,%f %f,%f", instStartPos_min, instStartPos_max, instStartPos2_min, instStartPos2_max);
        return 1; //error
    }

    //newMinPos = &inst[remLine2[instStartIdx_min]];
    //newMaxPos = &inst[remLine2[instStartIdx_max]];
    newMinPos = &inst[instStartIdx_min];
    newMaxPos = &inst[instStartIdx_max];
    if (newMinPos->x > newMaxPos->x) {
        tmp_pos = newMaxPos->x;
        newMaxPos->x = newMinPos->x;
        newMinPos->x = tmp_pos;
    }
    if (newMinPos->y > newMaxPos->y) {
        tmp_pos = newMaxPos->y;
        newMaxPos->y = newMinPos->y;
        newMinPos->y = tmp_pos;
    }
    *retMinPos = *newMinPos;
    *retMaxPos = *newMaxPos;

#if 1 //nsmoon@200228, FIXME
    //check plus/minus slope
    int slopeVal, slopePlusCnt = 0, slopeMinusCnt = 0;
    for (j = 0; j < grp->len; j++) {
        tmpIdx = grp->instIdx[j]; //org instIdx
        pd = lineSen[tmpIdx].pd;
        led = lineSen[tmpIdx].led;
        slopeVal = led - pd;
        if (slopeVal > 0) {
            slopePlusCnt++;
        }
        else if (slopeVal < 0) {
            slopeMinusCnt++;
        }
    }
    IS_DEBUG_FLAG{ TRACE_FCMM5("slopePlusCnt,slopeMinusCnt= %d ,%d", slopePlusCnt, slopeMinusCnt);};
    if (slopePlusCnt < FINE_MIN_SLOPE_CNT_GRP || slopeMinusCnt < FINE_MIN_SLOPE_CNT_GRP) {
        return 1; //error
    }
#endif

#if (DEBUG_fine_cal_min_max5 > 0)
    int pdTmp = lineSen[instStartLineIdxMin].pd;
    int ledTmp = lineSen[instStartLineIdxMin].led;
    DEBUG_SHOW_LINE_PD_LED(axis2, pdTmp, ledTmp, MY_COLOR/*2*/);
    DEBUG_SHOW_POS(&inst[instStartLineIdxMin], 0.05f, 0.05f, MY_COLOR);

    pdTmp = lineSen[instStartLineIdxMax].pd;
    ledTmp = lineSen[instStartLineIdxMax].led;
    DEBUG_SHOW_LINE_PD_LED(axis2, pdTmp, ledTmp, MY_COLOR/*2*/);
    DEBUG_SHOW_POS(&inst[instStartLineIdxMax], 0.05f, 0.05f, MY_COLOR);
    IS_DEBUG_FLAG{
        //if (axis1 == ENUM_HOR_X)
        TRACE_NOP;
    };
#endif
#if (DEBUG_fine_cal_min_max5 > 1)
    instStartCent = (instStartPos_min + instStartPos_max) * 0.5f;
    instStartCent2 = (axis1 == ENUM_HOR_X) ? initial_line[initialLineIdx].cent.x : initial_line[initialLineIdx].cent.y;
    IS_DEBUG_FLAG{ TRACE_FCMM5("fine_cal_min_max3.. (%d) %d,%d, %d,%d, %0.1f,%0.1f", axis1, instStartLineIdxMin, instStartLineIdxMax, instStartIdx_min, instStartIdx_max, instStartCent, instStartCent2); };
    IS_DEBUG_FLAG{ TRACE_FCMM5("..%0.2f,%0.2f %0.2f,%0.2f", instStartPos_min, instStartPos_max, instStartPos2_min, instStartPos2_max); };
    IS_DEBUG_FLAG{ TRACE_FCMM5("..%0.2f,%0.2f %0.2f,%0.2f", newMinPos->x, newMaxPos->x, newMinPos->y, newMaxPos->y); };
#endif

    instNew = (vec_t *)&BS_minmax_inst_xy[0];
    statNew = (uint16_t *)&BS_minmax_inst_stat[0];

    if (axis1 == ENUM_HOR_X) {
        instRange2Min = BS_aarea_zero_x;
        instRange2Max = BS_aarea_end_x;
    }
    else {
        instRange2Min = BS_aarea_zero_y;
        instRange2Max = BS_aarea_end_y;
        //instRange2Min = newMinPos->y - REMAINED_FINE_NORMAL_RANGE_Y;
        //instRange2Max = newMinPos->y + REMAINED_FINE_NORMAL_RANGE_Y;
    }

    /////////////////////////////////////////////////////////////
    //cal min/max
    /////////////////////////////////////////////////////////////
    for (modeCnt = 0; modeCnt < 2; modeCnt++) {
        if (modeCnt == 0) {
            //cal min
            instIdxMinMax = instStartLineIdxMin;
            intSectPosOrg = instStartPos_min;
            intSectPosOrg2 = instStartPos2_min;
            if (axis1 == ENUM_HOR_X) {
                instRange = inst[instStartIdx_max].y;
            }
            else {
                instRange = inst[instStartIdx_max].x;
            }
        }
        else {
            //cal max
            instIdxMinMax = instStartLineIdxMax;
            intSectPosOrg = instStartPos_max;
            intSectPosOrg2 = instStartPos2_max;
            if (axis1 == ENUM_HOR_X) {
                instRange = inst[instStartIdx_min].y;
            }
            else {
                instRange = inst[instStartIdx_min].x;
            }
        }
#if (DEBUG_fine_cal_min_max5 > 0)
        if (axis1 == ENUM_HOR_X) {
            if (modeCnt == 0) {
                debugColor = DEBUG_COLOR_X_MIN;
            }
            else {
                debugColor = DEBUG_COLOR_X_MAX;
            }
        }
        else {
            if (modeCnt == 0) {
                debugColor = DEBUG_COLOR_Y_MIN;
            }
            else {
                debugColor = DEBUG_COLOR_Y_MAX;
            }
        }
#endif
#if (DEBUG_fine_cal_min_max5 > 1)
        //IS_DEBUG_FLAG{ TRACE_FCMM5("axis1,mode,instIdxMinMax,slopeValMinMax: (%d/%d)(%d,%d)(%0.1f,%0.1f)", axis1, modeCnt, instIdxMinMax, slopeValMinMax, instStartPosMin, instStartPosMax);};
        IS_DEBUG_FLAG{ TRACE_FCMM5("..mode,instIdxMinMax: %d %d %0.1f", modeCnt, instIdxMinMax, instRange); };
#endif
        BS_int_sect_group2_epf_used_cnt = 0; //reset
        loopCnt = 0;
        while (loopCnt++ < CAL_MIN_MAX5_LOOP_CNT)
        {
            //IS_DEBUG_FLAG{ TRACE_FCMM5("..: %d", instIdxMinMax);};
            BS_add_epf_used((uint16_t)instIdxMinMax);
            pd = lineSen[instIdxMinMax].pd;
            led = lineSen[instIdxMinMax].led;
            BS_getLinePosInBufSen(axis2, led, pd, &p0, &p1);
            if (loopCnt == 1) {
                pSav0 = p0;
                pSav1 = p1;
            }
#if (DEBUG_fine_cal_min_max5 > 0)
            if (loopCnt > 1) {
                DEBUG_SHOW_LINE_POS(&p0, &p1, debugColor);
                TRACE_NOP;
            }
#endif
            IS_DEBUG_FLAG{
                //if (axis1 == ENUM_HOR_X)
                TRACE_NOP;
            };

            ////////////////////////////////////////////////////////////
            // make new inst with belonged lines
            ////////////////////////////////////////////////////////////
            intSectCnt = 0;
            intSectTmpIdx = -1;
            for (j = 0; j < grp->len; j++) {
                tmpIdx = grp->instIdx[j]; //org instIdx
                if (BS_is_epf_used((uint16_t)tmpIdx)) continue;
                pd = lineSen[tmpIdx].pd;
                led = lineSen[tmpIdx].led;
                BS_getLinePosInBufSen(axis2, led, pd, &p2, &p3);
                if (BS_line_intersection(&p0, &p1, &p2, &p3, &pRet)) {
                    if (axis1 == ENUM_HOR_X) {
                        intSectPosTmp = pRet.y;
                        intSectPosTmp2 = pRet.x;
                    }
                    else {
                        intSectPosTmp = pRet.x;
                        intSectPosTmp2 = pRet.y;
                    }
#if (DEBUG_fine_cal_min_max5 > 2)
                    IS_DEBUG_FLAG{ TRACE_FCMM5("..intSectPosTmp2: %0.1f %0.1f %0.1f %0.1f %0.1f", intSectPosTmp2,instRange2Min,instRange2Max,intSectPosTmp,instRange); };
#endif
                    if ((intSectPosTmp2 + EPSILON) > instRange2Min && (intSectPosTmp2 - EPSILON) < instRange2Max)
                    {
                        if ((modeCnt == 0 && intSectPosTmp < instRange) || (modeCnt == 1 && intSectPosTmp > instRange))
                        {
                            IS_DEBUG_FLAG{
                                TRACE_NOP;
                            };
#if (DEBUG_fine_cal_min_max5 > 2)
                            DEBUG_SHOW_POS(&pRet, 0.05f, 0.05f, MY_COLOR - 4); //gray
#endif
                            if (intSectCnt < MAX_MINMAX_INST_TBL)
                            {
                                instNew[intSectCnt] = pRet;
                                statNew[intSectCnt] = (uint16_t)tmpIdx; //org instIdx
                                intSectCnt++;
                            }
                            else {
                                TRACE_ERROR("ERROR! fine_cal_min_max2.1.invalid intSectCnt %d [%d]", intSectCnt, BG_frame_no);
                                continue;
                            }
                        }

                    }
                } //if (BS_line_intersection(&p0, &p1, &p2, &p3, &pRet))
            } //for (j = 0; j < grp->len; j++)
            IS_DEBUG_FLAG{
                //if (axis1 == ENUM_HOR_X)
                TRACE_NOP;
            };
            if (intSectCnt == 0)
            {
                //no intersect
#if (DEBUG_fine_cal_min_max5 > 1)
                if (modeCnt == 0) {
                    IS_DEBUG_FLAG{ TRACE_FCMM5(".==>min_max->min: %0.2f", min_max->min); };
                }
                else {
                    IS_DEBUG_FLAG{ TRACE_FCMM5(".==>min_max->max: %0.2f", min_max->max); };
                }
#endif
                calFinish = 1;
            }
            else {
                ////////////////////////////////////////////////////////////
                // find min/max inst pos in new inst
                ////////////////////////////////////////////////////////////
                calFinish = 0;
                if (loopCnt > 1) {
                    intSectTmpIdx = fine_minmax_inst_idx(axis1, intSectCnt, intSectPosOrg, intSectPosOrg2, modeCnt, loopCnt); //new instIdx
                }
                else {
                    float tmpPos, tmpPos2;
                    if (axis1 == ENUM_HOR_X) {
                        tmpPos = p1.y;
                        tmpPos2 = p1.x;
                    }
                    else {
                        tmpPos = p1.x;
                        tmpPos2 = p1.y;
                    }
                    intSectTmpIdx = fine_minmax_inst_idx(axis1, intSectCnt, tmpPos, tmpPos2, modeCnt, loopCnt); //new instIdx
                }
                if (intSectTmpIdx < 0) {
                    calFinish = 1;
                }
                else {
                    if (axis1 == ENUM_HOR_X) {
                        intSectNewPos = instNew[intSectTmpIdx].y;
                        intSectNewPos2 = instNew[intSectTmpIdx].x;
                    }
                    else {
                        intSectNewPos = instNew[intSectTmpIdx].x;
                        intSectNewPos2 = instNew[intSectTmpIdx].y;
                    }
                    if (modeCnt == 0) {
                        newMinPos = &instNew[intSectTmpIdx];
                    }
                    else {
                        newMaxPos = &instNew[intSectTmpIdx];
                    }
#if (DEBUG_fine_cal_min_max5 > 0)
                    DEBUG_SHOW_POS(&instNew[intSectTmpIdx], 0.3f, 0.3f, debugColor);
#endif

                    int right = 0; //0:colinear,-1:left,1:right
                    if (loopCnt > 1) {
                        right = BS_left_of(&pSav0, &pSav1, &instNew[intSectTmpIdx]);
                    }
#if (DEBUG_fine_cal_min_max5 > 1)
                    IS_DEBUG_FLAG{
                        TRACE_FCMM5("...:(%d) %d %0.2f/%0.2f %d", modeCnt, intSectTmpIdx, intSectNewPos, intSectPosOrg, right);
                    };
#endif
                    //calculation finished
#if 0
                    if (loopCnt == 1) {
                        calFinish = ((modeCnt == 0 && (intSectNewPos - EPSILON) < intSectPosOrg) ||
                            (modeCnt == 1 && (intSectNewPos + EPSILON) > intSectPosOrg));
                    }
                    else
#endif
                    {
                        //calFinish = ((axis1 == ENUM_HOR_X && (modeCnt == 0 && right == -1) || (modeCnt == 1 && right == 1)) ||
                        //             (axis1 == ENUM_VER_Y && (modeCnt == 0 && right == -1) || (modeCnt == 1 && right == 1)));
                        calFinish = ((modeCnt == 0 && right == -1) || (modeCnt == 1 && right == 1));
                    }
                }
#if (DEBUG_fine_cal_min_max5 > 0)
                IS_DEBUG_FLAG{
                    TRACE_NOP;
                };
                if (calFinish) {
                    if (modeCnt == 0) {
                        IS_DEBUG_FLAG{ TRACE_FCMM5("..=>min_max->min: %0.2f", min_max->min); };
                    }
                    else {
                        IS_DEBUG_FLAG{ TRACE_FCMM5("..=>min_max->max: %0.2f", min_max->max); };
                    }
                }
#endif
            }

            //save min/max
            if (modeCnt == 0) {
                min_max->min = GET_MAX(intSectPosOrg, min_max->min);
                if (min_max->min <= intSectPosOrg) {
                    *retMinPos = *newMinPos;
                }
            }
            else {
                min_max->max = GET_MIN(intSectPosOrg, min_max->max);
                if (min_max->max >= intSectPosOrg) {
                    *retMaxPos = *newMaxPos;
                }
            }

            if (calFinish)
            {
                break;
            }
            else {
                //replave posOrg and instIdxMinMax
                if (intSectTmpIdx >= 0 && intSectTmpIdx < MAX_MINMAX_INST_TBL) {
                intSectPosOrg = intSectNewPos;
                intSectPosOrg2 = intSectNewPos2;
                instIdxMinMax = statNew[intSectTmpIdx];
                pSav0 = p0;
                pSav1 = p1;
            }
                else {
                    TRACE_ERROR("ERROR! invalid intSectTmpIdx %d", intSectTmpIdx);
                }
            }
        } //while (loopCnt++ < CAL_MIN_MAX5_LOOP_CNT)
        if (loopCnt >= CAL_MIN_MAX5_LOOP_CNT) {
            TRACE_ERROR("ERROR! fine_cal_min_max2..could not find minIdx, loopCnt %d", loopCnt);
        }
    } //for (modeCnt = 0; modeCnt < 2; modeCnt++)
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };
    return 0; //no-error
}
#endif //0
#endif //1

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_fine_add_remained_touch   1
#endif
#if (DEBUG_fine_add_remained_touch > 0)
#define TRACE_BFART(...) TRACE(__VA_ARGS__)
#else
#define TRACE_BFART(...)
#endif

#if 0 //reserved do not remove
#define DEBUG_is_near_large_touch	0
#if (DEBUG_is_near_large_touch > 0)
#define TRACE_INLT(...)		TRACE(__VA_ARGS__);
#else
#define TRACE_INLT(...)	
#endif
static int is_near_large_touch(axis_t axis, pos_minMax2_t *min_max)
{
    int i;
    float min, max, min2, max2;
    int large_touch_idx_cnt = BS_large_touch_idx_cnt; //nsmoon@191227
    pos_min_max_t *large_touch_idx = &BS_large_touch_idx[0];

    for (i = 0; i < large_touch_idx_cnt; i++) {
        if (axis == ENUM_HOR_X) {
            min = large_touch_idx[i].minX - FINE_MULTI_MAX_LARGE_ERASE_DIST;
            max = large_touch_idx[i].maxX + FINE_MULTI_MAX_LARGE_ERASE_DIST;
            min2 = large_touch_idx[i].minX - FINE_MULTI_MIN_LARGE_ERASE_DIST;
            max2 = large_touch_idx[i].maxX + FINE_MULTI_MIN_LARGE_ERASE_DIST;
        }
        else {
            min = large_touch_idx[i].minY - FINE_MULTI_MAX_LARGE_ERASE_DIST;
            max = large_touch_idx[i].maxY + FINE_MULTI_MAX_LARGE_ERASE_DIST;
            min2 = large_touch_idx[i].minY - FINE_MULTI_MIN_LARGE_ERASE_DIST;
            max2 = large_touch_idx[i].maxY + FINE_MULTI_MIN_LARGE_ERASE_DIST;
        }
#if (DEBUG_is_near_large_touch > 0)
        IS_DEBUG_FLAG{
            TRACE_INLT("min,max::%d(%0.1f,%0.1f)-(%0.1f,%0.1f)(%0.1f,%0.1f)", axis, min_max->min, min_max->max, min, max, min2, max2);
        };
#endif
        if ((min_max->min >= min2 && min_max->min <= max2) ||
                (min_max->max >= min2 && min_max->max <= max2)) {
            TRACE_INLT("near large_touch~~skip");
            continue; //skip
        }
        if ((min_max->min >= min && min_max->min <= max) ||
                (min_max->max >= min && min_max->max <= max)) {
            TRACE_INLT("near large_touch~~cal");
            return 1; //found
        }
    }
    return 0; //not-found
}
#endif //1

#if 0 //for test
static int is_near_large_touch_prev(pos_min_max_t *min_max)
{
    int i, j;
    float minX, maxX, minY, maxY;
    int large_touch_idx_cnt;
    pos_min_max_t *large_touch_idx;
#if (DEBUG_is_near_large_touch > 0)
    IS_DEBUG_FLAG{
        TRACE_INLT("is_near_large_touch_prev..(%0.1f,%0.1f)(%0.1f,%0.1f)", min_max->minX, min_max->maxX, min_max->minY, min_max->maxY);
    };
#endif
    for (j = 0; j < 2; j++) {
        large_touch_idx = &BS_large_touch_idx_prev[j][0];
        large_touch_idx_cnt = BS_large_touch_idx_cnt_prev[j];
        for (i = 0; i < large_touch_idx_cnt; i++) {
            minX = large_touch_idx[i].minX - FINE_MULTI_MIN_LARGE_ERASE_DIST;
            maxX = large_touch_idx[i].maxX + FINE_MULTI_MIN_LARGE_ERASE_DIST;
            minY = large_touch_idx[i].minY - FINE_MULTI_MIN_LARGE_ERASE_DIST;
            maxY = large_touch_idx[i].maxY + FINE_MULTI_MIN_LARGE_ERASE_DIST;
#if (DEBUG_is_near_large_touch > 0)
            IS_DEBUG_FLAG{
                DEBUG_SHOW_MIN_MAX(&large_touch_idx[i], MY_COLOR, 0);
                TRACE_INLT("..(%0.1f,%0.1f)(%0.1f,%0.1f)", minX, maxX, minY, maxY);
            };
#endif
            if ((min_max->minX >= minX && min_max->minX <= maxX) ||
                    (min_max->maxX >= minX && min_max->maxX <= maxX)) {
                if ((min_max->minY >= minY && min_max->minY <= maxY) ||
                        (min_max->maxY >= minY && min_max->maxY <= maxY)) {
                    return 1; //found
                }
            }
        }
    }
    return 0; //not-found
}
#endif //0

#if 1 //old fine, do not remove
#define FINE_GET_INST_LINE_CNT_SMALL    15
static int fine_get_inst3(axis_t axis1, vec_t p0, vec_t p1, axis_t axis2, uint16_t *remLine2, int remLineCnt2, find_shadow_slope_line_t *slopeLine)
{
    int tmpInBufIdx;
    int pd, led, pd0 = 0, led0 = 0;
    vec_t p2, p3, pR;
    vec_t *inst;
    int instCnt, sortCnt;
    uint8_t *sort;
    in_line_t *lineSen;
    int remLineIdx;
    initial_line_t *initial_line;
    int initial_line_cnt, initialLineMax;

    if (slopeLine != 0) {
        if (BS_getSenInBuf(axis2, slopeLine->inBufIdx, &pd0, &led0) < 0) {
            TRACE_ERROR_MEM("ERROR_MEM! fine_get_inst3.0.invalid initial_line_cnt: (%d)%d", axis2, remLineCnt2);
            return FINE_MEM_ERROR; //mem-error
        }
    }

    if (axis1 == ENUM_HOR_X) {
        initial_line = &BS_initial_line_x[0];
        initial_line_cnt = BS_initial_line_x_cnt;
        initialLineMax = MAX_INITIAL_LINE_FINE_X;
    }
    else {
        //(axis1 == ENUM_VER_Y)
        initial_line = &BS_initial_line_y[0];
        initial_line_cnt = BS_initial_line_y_cnt;
        initialLineMax = MAX_INITIAL_LINE_FINE_Y;
    }
    IS_DEBUG_FLAG{
        //SEND_POLY(0, 0x21, 0);
        TRACE_NOP;
    }

    //assign inst buf
    if (initial_line_cnt < initialLineMax) {
        initial_line[initial_line_cnt].inst = &BS_inst_xy[BS_inst_xy_cnt];
        initial_line[initial_line_cnt].sort = &BS_inst_xy_sort[BS_inst_xy_cnt];
        initial_line[initial_line_cnt].lineSen = &BS_inst_xy_sen[BS_inst_xy_cnt];
        inst = initial_line[initial_line_cnt].inst;
        sort = initial_line[initial_line_cnt].sort;
        lineSen = initial_line[initial_line_cnt].lineSen;
            initial_line[initial_line_cnt].instLen = 0; //reset
            initial_line[initial_line_cnt].sortLen = 0; //reset
        if (axis1 == ENUM_HOR_X) {
            BS_initial_line_x_cnt++;
        }
        else {
            BS_initial_line_y_cnt++;
        }
    }
    else {
        TRACE_ERROR_MEM("ERROR_MEM! fine_get_inst3.1.invalid initial_line_cnt: (%d)%d", axis1, initial_line_cnt);
        return FINE_MEM_ERROR; //mem-error
    }
#if (DEBUG_fine_get_inst > 0)
    //if (axis1 == ENUM_VER_Y && initialLineIdx == 1)
    DEBUG_SHOW_LINE_POS(&p0, &p1, initial_line_cnt);
#endif

    instCnt = 0; //reset
    for (remLineIdx = 0; remLineIdx < remLineCnt2; remLineIdx++)
    {
        tmpInBufIdx = remLine2[remLineIdx];
        if (BS_getSenInBuf(axis2, tmpInBufIdx, &pd, &led) < 0) {
            TRACE_ERROR("ERROR! DEBUG_show_line_idx_..BS_getSenInBuf() [%d]", BG_frame_no);
            continue;
        }
#if 1 //nsmoon@211028
            if (slopeLine != 0) {
                if (pd0 == pd || led0 == led) {
                    continue;
                }
                if (remLineCnt2 > FINE_GET_INST_LINE_CNT_SMALL) { //nsmoon@211029
            int slopeVal = led - pd;
                if ((slopeVal > 0 && slopeLine->slopeSign < 0) || 
                    (slopeVal < 0 && slopeLine->slopeSign > 0)) {
                continue;
            }
                if (slopeLine->slopeSign == 0) {
                    if (slopeVal > 0 && slopeVal > slopeLine->slopeVal) {
                continue;
            }
                    else if (slopeVal < 0 && slopeVal < slopeLine->slopeVal) {
                continue;
            }
                }
            }
            }
#endif
        BS_getLinePosInBufSen(axis2, led, pd, &p2, &p3);
#if (DEBUG_fine_get_inst > 1)
        //if (axis1 == ENUM_VER_Y && initialLineIdx == 1)
        DEBUG_SHOW_LINE_POS(&p2, &p3, initial_line_cnt);
#endif
        pR.x = -1; pR.y = -1;
        if (BS_line_intersection(&p0, &p1, &p2, &p3, &pR)) {
#if (DEBUG_fine_get_inst > 0)
            DEBUG_SHOW_POS(&pR, 0.1f, 0.1f, MY_COLOR - 4); //gray
#endif
			if ((BS_inst_xy_cnt + instCnt) < MAX_NUM_INST_FINE) {
	            if (instCnt < UINT8_SIZE) { //nsmoon@230410
					inst[instCnt] = pR;
					lineSen[instCnt].led = (uint8_t)led;
					lineSen[instCnt].pd = (uint8_t)pd;
					instCnt++;
	            }
	            else {
	                TRACE_ERROR("ERROR! fine_get_inst3.2.invalid instCnt %d", instCnt);
	            }
			}
			else {
				TRACE_ERROR_MEM("ERROR_MEM! fine_get_inst3.1.invalid instCnt: (%d)%d", axis1, (BS_inst_xy_cnt + instCnt));
				return FINE_MEM_ERROR; //mem-error
			}
        }
        else {
            //TRACE_ERROR("ERROR! fine_get_inst3..no inst");
            TRACE_NOP;
        }
    } //for (remLineIdx = 0; remLineIdx < remLineCnt; remLineIdx++)

    //save instLen
    initial_line[initial_line_cnt].instLen = instCnt; //num of instersect
#if 0 //for test
    if (axis1 == ENUM_VER_Y && mode == 0) {
        //just check
        if (GET_ABS(initial_line[initialLineIdx].cent.y - inst[0].y) > EPSILON) {
            TRACE_ERROR("ERROR! invalid initial_line[].cent.y and inst[].y: %0.2f, %0.2f", initial_line[initialLineIdx].cent.y, inst[0].y);
        }
    }
#endif

#if 1 //nsmoon@200922
    int isEdgeArea = 0;
    if (instCnt >= FINE_INITIAL_GRP_MIN_NUM_EDGE) {
        int edgeX = (BS_is_edge(ENUM_HOR_X, inst[0].x) | BS_is_edge(ENUM_HOR_X, inst[instCnt - 1].x));
        int edgeY = (BS_is_edge(ENUM_VER_Y, inst[0].y) | BS_is_edge(ENUM_VER_Y, inst[instCnt - 1].y));
        isEdgeArea = edgeX | edgeY;
        IS_DEBUG_FLAG{ TRACE_FGI("...isEdgeArea3: %04x (%0.1f,%0.1f)(%0.1f,%0.1f)", isEdgeArea, inst[0].x, inst[0].y, inst[instCnt - 1].x, inst[instCnt - 1].y);};
    }
    initial_line[initial_line_cnt].isEdgeArea = (uint16_t)isEdgeArea;
#endif

    ///////////////////////////////////////
    //make sort table
    int i, j, instIdx_i, instIdx_j;
    float inst_i, inst_j;

    sortCnt = 0;
    for (i = 0; i < instCnt; i++) {
        sort[sortCnt++] = (uint8_t)i; //nsmoon@190527
    }
    //save sortCnt
    initial_line[initial_line_cnt].sortLen = sortCnt; //num of instersect
    if (sortCnt != instCnt) {
        TRACE_ERROR_MEM("ERROR_MEM! invalid sortLen and instLen: %d, %d", sortCnt, instCnt);
        return FINE_MEM_ERROR; //mem-error
    }
#if 0 //(DEBUG_fine_get_inst > 1) //for testing
    for (i = 0; i < sortCnt; i++) {
        TRACENR("%d ", sort[i]);
        if ((i + 1) % 20 == 0) TRACENR("\r\n");
    }
    TRACE("=>before(%d, %d)", axis2, initial_line_cnt);
    for (i = 0; i < sortCnt; i++) {
        inst_i = (axis2 == ENUM_HOR_X) ? inst[sort[i]].x : inst[sort[i]].y;
        TRACENR("%0.1f ", inst_i);
        if ((i + 1) % 20 == 0) TRACENR("\r\n");
    }
    TRACE("=>before(%d, %d)", axis2, initial_line_cnt);
#endif

    //for (i = 0; i < instCnt - 1; i++)
    for (i = 0; i < sortCnt - 1; i++)
    {
        //for (j = (i + 1); j < instCnt; j++)
        for (j = (i + 1); j < sortCnt; j++)
        {
            instIdx_i = sort[i];
            instIdx_j = sort[j];
            inst_i = (axis2 == ENUM_HOR_X) ? inst[instIdx_i].x : inst[instIdx_i].y;
            inst_j = (axis2 == ENUM_HOR_X) ? inst[instIdx_j].x : inst[instIdx_j].y;
            if (inst_i > inst_j)
            {
                sort[i] = (uint8_t)instIdx_j;
                sort[j] = (uint8_t)instIdx_i;
            }
        }
    }
#if 0 //(DEBUG_fine_get_inst > 1) //for testing
    IS_DEBUG_FLAG
    {
        for (i = 0; i < sortCnt; i++) {
            TRACENR("%d ", sort[i]);
            if ((i + 1) % 20 == 0) TRACENR("\r\n");
        }
    TRACE("=>after idx(%d, %d, %d)", axis2, initial_line_cnt, sortCnt);
    for (i = 0; i < sortCnt; i++) {
        inst_i = (axis2 == ENUM_HOR_X) ? inst[sort[i]].x : inst[sort[i]].y;
        TRACENR("%0.1f ", inst_i);
        if ((i + 1) % 20 == 0) TRACENR("\r\n");
    }
    TRACE("=>after pos(%d, %d, %d)", axis2, initial_line_cnt, sortCnt);
    };
#endif
    TRACE_FGI("initial_line[].inst:1: %d %d 0x%x", initial_line_cnt, instCnt, initial_line[initial_line_cnt].inst);
    if ((BS_inst_xy_cnt + instCnt) < MAX_NUM_INST_FINE) {
        BS_inst_xy_cnt += instCnt;
    }
    else {
        TRACE_ERROR("ERROR! fine_get_inst.2.invalid instCnt: (%d)%d %d %d", axis1, instCnt, BS_inst_xy_cnt, initial_line_cnt);
        return FINE_MEM_ERROR; //mem-error
    }
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };
    return initial_line_cnt; //no-error
}

#if 0 //reserved
ATTR_BACKEND_RAMFUNC
static int fine_conv_idx2(uint8_t *sort, uint8_t *stat, int sortLen, int idx)
{
    int sortIdx, instIdx;
    for (sortIdx = 0; sortIdx < sortLen; sortIdx++) {
        instIdx = sort[sortIdx];
        if (stat[instIdx] == 2) {
            continue; //out of range
        }
        if (instIdx == idx)
        {
            return sortIdx;
        }
    }
    return -1; //error
}
#endif //1

static int fine_make_group3(axis_t axis1, int initialLineIdx, axis_t axis2, int fineLoop)
{
    //mode: 0(normal), 1(approximate)
    initial_line_t *initial_line;
    initial_line_group_t *grp;
    int grpCnt, cnt;
    float initialGrpDist = 0;
    int sortLen;
    uint8_t *sort;
    vec_t *inst;
    float tmpPos[2], tmpDist = MIN_INITIAL_VAL;
    int inBufIdx[2];
    int initialGrpCntMin = FINE_INITIAL_GRP_MIN_NUM;
    int i;

    if (axis1 == ENUM_VER_Y) {
        initial_line = &BS_initial_line_y[0];
        IS_DEBUG_FLAG{
            TRACE_NOP;
        }
    }
    else {
        //(axis1 == ENUM_HOR_X);
        initial_line = &BS_initial_line_x[0];
    }
#if 0 //for test
    if (axis2 == ENUM_VER_Y) {
        initialGrpDist = (mode) ? FINE_INITIAL_GRP_DIST_Y_APRX : FINE_INITIAL_GRP_DIST_Y; //y-grp
    }
    else {
        //(axis2 == ENUM_HOR_X);
        initialGrpDist = (mode) ? FINE_INITIAL_GRP_DIST_X_APRX : FINE_INITIAL_GRP_DIST_X; //x-grp
    }
#else
    if (axis2 == ENUM_VER_Y) {
        initialGrpDist = FINE_INITIAL_GRP_DIST_Y_OLD; //FINE_INITIAL_GRP_DIST_Y; //y-grp
    }
    else {
        //(axis2 == ENUM_HOR_X);
        initialGrpDist = FINE_INITIAL_GRP_DIST_X_OLD; //FINE_INITIAL_GRP_DIST_X; //x-grp
    }
#endif //0
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };

    initial_line[initialLineIdx].grpCnt = 0; //init

    //sortLen = initial_line[initialLineIdx].instLen;
    sortLen = initial_line[initialLineIdx].sortLen;
    if (sortLen <= 0) {
        return FINE_NO_GROUP;
    }

    sort = initial_line[initialLineIdx].sort;
    inst = initial_line[initialLineIdx].inst;
    grpCnt = 0; //reset
    initial_line[initialLineIdx].grp = &BS_initial_line_grp[BS_initial_line_grp_cnt];
    grp = initial_line[initialLineIdx].grp;
    grp[grpCnt].instIdx = &BS_inst_xy_grp[BS_inst_xy_grp_cnt];
#if 1 //nsmoon@200923
    int isEdgeArea = initial_line[initialLineIdx].isEdgeArea;
    if ((axis1 == ENUM_HOR_X && IS_NEAR_Y(isEdgeArea)) ||
        (axis1 == ENUM_VER_Y && IS_NEAR_X(isEdgeArea))) {
        initialGrpCntMin = FINE_INITIAL_GRP_MIN_NUM_EDGE;
    }
#endif
#if 1 //nsmoon@201203
    else if (axis1 == ENUM_VER_Y && IS_INNER_Y(isEdgeArea)) {
        initialGrpCntMin = FINE_INITIAL_GRP_MIN_NUM_EDGE;
    }
#endif
#if (BRUSH_MODE_ENABLE == 1)
    if (BS_brushMode) { //nsmoon@200513
        initialGrpCntMin = FINE_INITIAL_GRP_MIN_NUM_BRUSH;
        initialGrpDist = FINE_INITIAL_GRP_DIST_BRUSH;
    }
#endif
#if 1 //nsmoon@211022
    if (fineLoop >= FINE_LOOP_SHADOW_ST && fineLoop <= FINE_LOOP_SHADOW_ED) {
        initialGrpDist = FINE_INITIAL_GRP_DIST_SHADOW;
        initialGrpCntMin = FINE_INITIAL_GRP_MIN_NUM_SHADOW; //nsmoon@211027
    }
#endif
    IS_DEBUG_FLAG{ TRACE_FMG2(".initialLineIdx,sortLen: (%d/%x) %d %d", axis1, isEdgeArea, initialLineIdx, sortLen); };
    IS_DEBUG_FLAG{ TRACE_FMG2("initialGrpDist,initialGrpCntMin= %0.1f %d 0x%x", initialGrpDist, initialGrpCntMin, isEdgeArea);};

#if 0 //nsmoon@200408
    int edgeX = (BS_is_edge(ENUM_HOR_X, inst[sort[0]].x) || BS_is_edge(ENUM_HOR_X, inst[sort[sortLen - 1]].x));
    int edgeY = (BS_is_edge(ENUM_VER_Y, inst[sort[0]].y) || BS_is_edge(ENUM_VER_Y, inst[sort[sortLen - 1]].y));
    IS_DEBUG_FLAG{ TRACE_FMG2("edgeX,edgeY= %d %d (%0.1f %0.1f)(%0.1f %0.1f)", edgeX, edgeY, inst[sort[0]].x, inst[sort[sortLen - 1]].x, inst[sort[0]].y, inst[sort[sortLen - 1]].y);};
    if (edgeX || edgeY) {
        initialGrpCntMin = FINE_INITIAL_GRP_MIN_NUM_EDGE;
#if 0 //not use for test, nsmoon@200918
        if (axis2 == ENUM_VER_Y) {
            initialGrpDist = FINE_INITIAL_GRP_DIST_Y_APRX; //y-grp
        }
        else {
            //(axis2 == ENUM_HOR_X);
            initialGrpDist = FINE_INITIAL_GRP_DIST_X_APRX; //x-grp
        }
        IS_DEBUG_FLAG{ TRACE_FMG2("initialGrpDist= %0.1f", initialGrpDist);};
#endif
    }
#endif
#if 0 //nsmoon@200408
    if (inst[sort[0]].x < (BS_aarea_zero_x + FINE_EDGE_DIST_FROM_AAREA) ||
            inst[sort[sortLen - 1]].x > (BS_aarea_end_x - FINE_EDGE_DIST_FROM_AAREA) ||
            inst[sort[0]].y < (BS_aarea_zero_y + FINE_EDGE_DIST_FROM_AAREA) ||
            inst[sort[sortLen - 1]].y > (BS_aarea_end_y - FINE_EDGE_DIST_FROM_AAREA)) {
        initialGrpCntMin = FINE_INITIAL_GRP_MIN_NUM_EDGE;
        if (axis2 == ENUM_VER_Y) {
            initialGrpDist = FINE_INITIAL_GRP_DIST_Y_APRX; //y-grp
        }
        else {
            //(axis2 == ENUM_HOR_X);
            initialGrpDist = FINE_INITIAL_GRP_DIST_X_APRX; //x-grp
        }
    }
#endif

    cnt = 0; //reset;
    tmpPos[0] = MIN_INITIAL_VAL;
    inBufIdx[0] = 0;
    for (i = 0; i < sortLen; i++)
    {
        inBufIdx[1] = sort[i];
#if (DEBUG_fine_make_group > 1)
        DEBUG_SHOW_POS(&initial_line[initialLineIdx].inst[inBufIdx[1]], 0.05f, 0.05f, initialLineIdx);
#endif
        tmpPos[1] = (axis2 == ENUM_HOR_X) ? inst[inBufIdx[1]].x : inst[inBufIdx[1]].y;
#if 0 //(BRUSH_MODE_ENABLE == 0) //not brush mode, nsmoon@200309, FIXME!!!
        if (BS_is_edge(axis2, tmpPos[1])) {
            initialGrpCntMin = FINE_INITIAL_GRP_MIN_NUM_EDGE;
        }
#endif

        if (tmpPos[0] == MIN_INITIAL_VAL) {
            //save previous idx & pos
            inBufIdx[0] = inBufIdx[1];
            tmpPos[0] = tmpPos[1];
            continue;
        }
        else {
            tmpDist = tmpPos[1] - tmpPos[0];
            IS_DEBUG_FLAG{ TRACE_FMG3("..tmpDist,tmpPos[1],tmpPos[0]: %0.2f %0.2f(%d) %0.2f(%d)", tmpDist, tmpPos[1], inBufIdx[1], tmpPos[0], inBufIdx[0]); };
            if (tmpDist < 0) {
                TRACE_ERROR("ERROR! invalid tmpDist: %0.3f (%0.2f %0.2f)", tmpDist, tmpPos[1], tmpPos[0]);
                //tmpDist = -tmpDist;
                    return FINE_NO_GROUP; //error
            }
        }

        if (tmpDist < initialGrpDist) {
            //add in grp
            if (BS_inst_xy_grp_cnt < MAX_NUM_INST_FINE && cnt < MAX_INST_SORT_TBL_SIZE) {
                BS_inst_xy_grp[BS_inst_xy_grp_cnt++] = (uint8_t)inBufIdx[0]; cnt++;
                IS_DEBUG_FLAG{ TRACE_FMG3(" ..add BS_inst_xy_grp[]: %d %d %d", cnt - 1, BS_inst_xy_grp_cnt - 1, BS_inst_xy_grp[BS_inst_xy_grp_cnt - 1]); };
            }
            else {
                TRACE_ERROR("ERROR! 1 invalid cnt >= MAX_NUM_INST_FINE: %d/%d", cnt, BS_inst_xy_grp_cnt);
                    return FINE_MEM_ERROR; //mem-error
            }
        }
        else {
            //end of grp
            if ((cnt+1) >= initialGrpCntMin) { //include first one
                if (BS_inst_xy_grp_cnt < MAX_NUM_INST_FINE && cnt < MAX_INST_SORT_TBL_SIZE) {
                    BS_inst_xy_grp[BS_inst_xy_grp_cnt++] = (uint8_t)inBufIdx[0]; cnt++;
                    IS_DEBUG_FLAG{ TRACE_FMG3(" ..end BS_inst_xy_grp[]: %d %d %d", (cnt - 1), BS_inst_xy_grp_cnt - 1, BS_inst_xy_grp[BS_inst_xy_grp_cnt - 1]); };
                    if (grpCnt < FINE_MAX_GRP_CNT) {
                        grp[grpCnt].stat = 0; //init
                        //grp[grpCnt].mergedCnt = 0; //init
                        if (cnt > UINT8_MAX) {
                        	TRACE_ERROR("ERROR! invlaid cnt %d", cnt);
                            cnt = UINT8_MAX;
                        }
                        grp[grpCnt].len = (uint8_t)cnt; //nsmoon@191211=>1230
                        IS_DEBUG_FLAG{ TRACE_FMG2(" .1.grpCnt,grp[].len: %d %d", grpCnt, grp[grpCnt].len); };
                        //assign new grp
                        grpCnt++;
                        grp[grpCnt].instIdx = &BS_inst_xy_grp[BS_inst_xy_grp_cnt];
                    }
                    else {
                        //TRACE_ERROR("ERROR! 1 invalid grpCnt");
                        TRACE_ERROR("&1:");
                            return FINE_MEM_ERROR; //mem-error
                    }
                }
                else {
                    TRACE_ERROR("ERROR! 2 invalid cnt >= MAX_NUM_INST_FINE %d", cnt);
                        return FINE_MEM_ERROR; //mem-error
                }
            } //if (cnt >= initialGrpCntMin)
            else {
                BS_inst_xy_grp_cnt -= cnt;
                IS_DEBUG_FLAG{ TRACE_FMG3(" ..del cnt,BS_inst_xy_grp_cnt: %d %d", cnt, BS_inst_xy_grp_cnt); };
                if (BS_inst_xy_grp_cnt < 0) {
                    TRACE_ERROR_MEM("ERROR_MEM! invalid BS_inst_xy_grp_cnt: %d", BS_inst_xy_grp_cnt);
                    BS_inst_xy_grp_cnt = 0;
                }
            }
            cnt = 0; //reset
        }
        //save previous idx & pos
        inBufIdx[0] = inBufIdx[1];
        tmpPos[0] = tmpPos[1];
        if (BS_inst_xy_grp_cnt >= MAX_NUM_INST_FINE) {
            break;
        }
    } //for (i = 0; i < sortLen; i++)
#if (BRUSH_MODE_ENABLE == 1)
    if ((cnt+1) >= initialGrpCntMin || (BS_brushMode && i > 0))
#else
    if ((cnt+1) >= initialGrpCntMin)
#endif
    { //include first one
        if (BS_inst_xy_grp_cnt < MAX_NUM_INST_FINE && cnt < MAX_INST_SORT_TBL_SIZE) {
            BS_inst_xy_grp[BS_inst_xy_grp_cnt++] = (uint8_t)inBufIdx[0]; cnt++;
            IS_DEBUG_FLAG{ TRACE_FMG3(" ...end BS_inst_xy_grp[]: %d %d %d", (cnt - 1), BS_inst_xy_grp_cnt - 1, BS_inst_xy_grp[BS_inst_xy_grp_cnt - 1]); };
            if (grpCnt < FINE_MAX_GRP_CNT) {
                grp[grpCnt].stat = 0; //init
                                      //grp[grpCnt].mergedCnt = 0; //init
                if (cnt > UINT8_MAX) {
                	TRACE_ERROR("ERROR! invlaid cnt %d", cnt);
                    cnt = UINT8_MAX;
                }
                grp[grpCnt].len = (uint8_t)cnt; //nsmoon@191211=>1230
                IS_DEBUG_FLAG{ TRACE_FMG2(" .2.grpCnt,grp[].len: %d %d", grpCnt, grp[grpCnt].len); };
                //assign new grp
                grpCnt++;
                grp[grpCnt].instIdx = &BS_inst_xy_grp[BS_inst_xy_grp_cnt]; //instIdx
            }
            else {
                //TRACE_ERROR("ERROR! 2 invalid grpCnt");
                TRACE_ERROR("&2:");
                return FINE_MEM_ERROR; //mem-error
            }
            TRACE_NOP;
        }
        else {
            TRACE_ERROR("ERROR! 3 invalid cnt >= MAX_NUM_INST_FINE %d %d", cnt, BS_inst_xy_grp_cnt);
            return FINE_MEM_ERROR; //mem-error
        }
    } //if (cnt >= initialGrpCntMin)
    else {
        BS_inst_xy_grp_cnt -= cnt;
        IS_DEBUG_FLAG{ TRACE_FMG3(" ..del cnt,BS_inst_xy_grp_cnt: %d %d", cnt, BS_inst_xy_grp_cnt); };
        if (BS_inst_xy_grp_cnt < 0) {
            TRACE_ERROR_MEM("ERROR_MEM! invalid BS_inst_xy_grp_cnt: %d", BS_inst_xy_grp_cnt);
            BS_inst_xy_grp_cnt = 0;
        }
    }
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };

    //update & save count
    if (grpCnt == 0) {
        //BS_initial_line[initialLineIdx].instLen = -1;
        initial_line[initialLineIdx].sortLen = 0;
        return FINE_NO_GROUP; //no-grp
    }
    BS_initial_line_grp_cnt += grpCnt; //inclease buffer index
    initial_line[initialLineIdx].grpCnt = (uint8_t)grpCnt;

    return FINE_GROUP_FOUND; //found grp
}

#if 1 //nsmoon@200602
#define FINE_ERASER_DIST_SQ      (600.0f * 600.0f) //(1000.0f * 1000.0f)
static int fine_is_near_eraser(vec_t centPos)
{
    int i;
    for (i = 0; i < BG_touch_count; i++) {
        //TRACE("..BG_touch_area= (%d)%0.1f", i, BG_touch_area[i]);
        if (BG_touch_area[i] > TOUCH_ERASER_SIZESQ_XLARGE) {
            float distSq = BG_calDistSquare(BG_touch_data[i], centPos);
            //TRACE("..distSq= (%d)%0.1f/%0.1f", i, distSq, FINE_ERASER_DIST_SQ);
            if (distSq < FINE_ERASER_DIST_SQ) {
                //TRACE("=>near eraser(center)");
                return 1; //found
            }
        }
    }
    return 0; //not found
}
#endif

#if 1 //nsmoon@211028
#define fineDEBUG_make_group_shadow_cent 0
#if (fineDEBUG_make_group_shadow_cent > 0)
#define TRACE_FMGSC(...)		TRACE(__VA_ARGS__)
#else
#define TRACE_FMGSC(...)
#endif
//#define FINE_INITIAL_GRP_DIST_SHADOW_OPP_MAX    10.0f
static int fine_make_group_shadow_cent(axis_t axis1, int initialLineIdx, axis_t axis2, int fineLoop)
{
    //mode: 0(normal), 1(approximate)
    initial_line_t *initial_line;
    initial_line_group_t *grp;
    int grpCnt, cnt;
    int sortLen;
    uint8_t *sort;
    vec_t *inst;
    float tmpPos[2], tmpPosOpp[2];
    float tmpDist = MIN_INITIAL_VAL;
    float tmpDistOpp = MIN_INITIAL_VAL;
    int inBufIdx[2];
    float initialGrpDist = FINE_INITIAL_GRP_DIST_SHADOW;
    float initialGrpDistOpp = FINE_INITIAL_GRP_DIST_SHADOW_OPP;
    int initialGrpCntMin = FINE_INITIAL_GRP_MIN_NUM_SHADOW;
    int i;

    if (axis1 == ENUM_VER_Y) {
        initial_line = &BS_initial_line_y[0];
        IS_DEBUG_FLAG{
            TRACE_NOP;
        }
    }
    else {
        //(axis1 == ENUM_HOR_X);
        initial_line = &BS_initial_line_x[0];
    }
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };

    initial_line[initialLineIdx].grpCnt = 0; //init
    sortLen = initial_line[initialLineIdx].sortLen;
    if (sortLen <= 0) {
        return FINE_NO_GROUP;
    }

    sort = initial_line[initialLineIdx].sort;
    inst = initial_line[initialLineIdx].inst;
    grpCnt = 0; //reset
    initial_line[initialLineIdx].grp = &BS_initial_line_grp[BS_initial_line_grp_cnt];
    grp = initial_line[initialLineIdx].grp;
    grp[grpCnt].instIdx = &BS_inst_xy_grp[BS_inst_xy_grp_cnt];
#if 1 //nsmoon@200923
    int isEdgeArea = initial_line[initialLineIdx].isEdgeArea;
    if ((axis1 == ENUM_HOR_X && IS_NEAR_Y(isEdgeArea)) ||
        (axis1 == ENUM_VER_Y && IS_NEAR_X(isEdgeArea))) {
        //initialGrpCntMin = FINE_INITIAL_GRP_MIN_NUM_EDGE;
        initialGrpDist = FINE_INITIAL_GRP_DIST_SHADOW_ERASER;
        initialGrpDistOpp = FINE_INITIAL_GRP_DIST_SHADOW_OPP_ERASER;
    }
#endif
#if 0 //nsmoon@201203
    else if (axis1 == ENUM_VER_Y && IS_INNER_Y(isEdgeArea)) {
        //initialGrpCntMin = FINE_INITIAL_GRP_MIN_NUM_EDGE;
    }
#endif
#if 1 //nsmoon@211029
    int nearErase = fine_is_near_eraser(inst[sort[0]]);
    if (nearErase) {
        initialGrpDist = FINE_INITIAL_GRP_DIST_SHADOW_ERASER;
        initialGrpDistOpp = FINE_INITIAL_GRP_DIST_SHADOW_OPP_ERASER;
    }
#endif

    IS_DEBUG_FLAG{ TRACE_FMGSC(".initialLineIdx,sortLen: (%d/%x) %d %d", axis1, isEdgeArea, initialLineIdx, sortLen); };
    //IS_DEBUG_FLAG{ TRACE_FMGSC("initialGrpDist,initialGrpCntMin= %0.1f %d 0x%x", initialGrpDist, initialGrpCntMin, isEdgeArea); };

    cnt = 0; //reset;
    tmpPos[0] = MIN_INITIAL_VAL;
    tmpPosOpp[0] = MIN_INITIAL_VAL;
    inBufIdx[0] = 0;
    for (i = 0; i < sortLen; i++)
    {
        inBufIdx[1] = sort[i];
#if (fineDEBUG_make_group_shadow_cent > 1)
        DEBUG_SHOW_POS(&initial_line[initialLineIdx].inst[inBufIdx[1]], 0.05f, 0.05f, initialLineIdx);
#endif
        tmpPos[1] = (axis2 == ENUM_HOR_X) ? inst[inBufIdx[1]].x : inst[inBufIdx[1]].y;
        tmpPosOpp[1] = (axis2 == ENUM_HOR_X) ? inst[inBufIdx[1]].y : inst[inBufIdx[1]].x;

        if (tmpPos[0] == MIN_INITIAL_VAL) {
            //save previous idx & pos
            inBufIdx[0] = inBufIdx[1];
            tmpPos[0] = tmpPos[1];
            tmpPosOpp[0] = tmpPosOpp[1];
            continue;
        }
        else {
            tmpDist = tmpPos[1] - tmpPos[0];
            tmpDistOpp = GET_ABS(tmpPosOpp[1] - tmpPosOpp[0]);
            IS_DEBUG_FLAG{ TRACE_FMGSC("..tmpDist,tmpPos[1],tmpPos[0]: %0.2f %0.2f(%d) %0.2f(%d)", tmpDist, tmpPos[1], inBufIdx[1], tmpPos[0], inBufIdx[0]); };
            IS_DEBUG_FLAG{ TRACE_FMGSC("..tmpDistOpp,tmpPosOpp[1],tmpPosOpp[0]: %0.2f %0.2f %0.2f", tmpDistOpp, tmpPosOpp[1], tmpPosOpp[0]); };
            if (tmpDist < 0) {
                TRACE_ERROR("ERROR! invalid tmpDist: %0.3f (%0.2f %0.2f)", tmpDist, tmpPos[1], tmpPos[0]);
                //tmpDist = -tmpDist;
                return FINE_NO_GROUP; //error
            }
        }

        if (tmpDist < initialGrpDist && tmpDistOpp < initialGrpDistOpp)
        {
            //add in grp
            if (BS_inst_xy_grp_cnt < MAX_NUM_INST_FINE && cnt < MAX_INST_SORT_TBL_SIZE) {
                BS_inst_xy_grp[BS_inst_xy_grp_cnt++] = (uint8_t)inBufIdx[0]; cnt++;
                IS_DEBUG_FLAG{ TRACE_FMGSC(" ..add BS_inst_xy_grp[]: %d %d %d", cnt - 1, BS_inst_xy_grp_cnt - 1, BS_inst_xy_grp[BS_inst_xy_grp_cnt - 1]); };
            }
            else {
                TRACE_ERROR("ERROR! 1 invalid cnt >= MAX_NUM_INST_FINE: %d/%d", cnt, BS_inst_xy_grp_cnt);
                return FINE_MEM_ERROR; //mem-error
            }
        }
        else {
            //end of grp
            if ((cnt + 1) >= initialGrpCntMin)
            { //include first one
                if (BS_inst_xy_grp_cnt < MAX_NUM_INST_FINE && cnt < MAX_INST_SORT_TBL_SIZE) {
                    BS_inst_xy_grp[BS_inst_xy_grp_cnt++] = (uint8_t)inBufIdx[0]; cnt++;
                    IS_DEBUG_FLAG{ TRACE_FMGSC(" ..end BS_inst_xy_grp[]: %d %d %d", (cnt - 1), BS_inst_xy_grp_cnt - 1, BS_inst_xy_grp[BS_inst_xy_grp_cnt - 1]); };
                    if (grpCnt < FINE_MAX_GRP_CNT) {
                        grp[grpCnt].stat = 0; //init
                                              //grp[grpCnt].mergedCnt = 0; //init
                        if (cnt > UINT8_MAX) {
                            TRACE_ERROR("ERROR! invlaid cnt %d", cnt);
                            cnt = UINT8_MAX;
                        }
                        grp[grpCnt].len = (uint8_t)cnt; //nsmoon@191211=>1230
                        IS_DEBUG_FLAG{ TRACE_FMGSC(" .1.grpCnt,grp[].len: %d %d", grpCnt, grp[grpCnt].len); };
                        //assign new grp
                        grpCnt++;
                        grp[grpCnt].instIdx = &BS_inst_xy_grp[BS_inst_xy_grp_cnt];
                    }
                    else {
                        //TRACE_ERROR("ERROR! 1 invalid grpCnt");
                        TRACE_ERROR("&1:");
                        return FINE_MEM_ERROR; //mem-error
                    }
                }
                else {
                    TRACE_ERROR("ERROR! 2 invalid cnt >= MAX_NUM_INST_FINE %d", cnt);
                    return FINE_MEM_ERROR; //mem-error
                }
            } //if (cnt >= initialGrpCntMin)
            else {
                BS_inst_xy_grp_cnt -= cnt;
                IS_DEBUG_FLAG{ TRACE_FMGSC(" ..del cnt,BS_inst_xy_grp_cnt: %d %d", cnt, BS_inst_xy_grp_cnt); };
                if (BS_inst_xy_grp_cnt < 0) {
                    TRACE_ERROR_MEM("ERROR_MEM! invalid BS_inst_xy_grp_cnt: %d", BS_inst_xy_grp_cnt);
                    BS_inst_xy_grp_cnt = 0;
                }
            }
            cnt = 0; //reset
        }
        //save previous idx & pos
        inBufIdx[0] = inBufIdx[1];
        tmpPos[0] = tmpPos[1];
        tmpPosOpp[0] = tmpPosOpp[1];
        if (BS_inst_xy_grp_cnt >= MAX_NUM_INST_FINE) {
            break;
        }
    } //for (i = 0; i < sortLen; i++)

    if ((cnt + 1) >= initialGrpCntMin)
    { //include first one
        if (BS_inst_xy_grp_cnt < MAX_NUM_INST_FINE && cnt < MAX_INST_SORT_TBL_SIZE) {
            BS_inst_xy_grp[BS_inst_xy_grp_cnt++] = (uint8_t)inBufIdx[0]; cnt++;
            IS_DEBUG_FLAG{ TRACE_FMGSC(" ...end BS_inst_xy_grp[]: %d %d %d", (cnt - 1), BS_inst_xy_grp_cnt - 1, BS_inst_xy_grp[BS_inst_xy_grp_cnt - 1]); };
            if (grpCnt < FINE_MAX_GRP_CNT) {
                grp[grpCnt].stat = 0; //init
                                      //grp[grpCnt].mergedCnt = 0; //init
                if (cnt > UINT8_MAX) {
                    TRACE_ERROR("ERROR! invlaid cnt %d", cnt);
                    cnt = UINT8_MAX;
                }
                grp[grpCnt].len = (uint8_t)cnt; //nsmoon@191211=>1230
                IS_DEBUG_FLAG{ TRACE_FMGSC(" .2.grpCnt,grp[].len: %d %d", grpCnt, grp[grpCnt].len); };
                //assign new grp
                grpCnt++;
                grp[grpCnt].instIdx = &BS_inst_xy_grp[BS_inst_xy_grp_cnt]; //instIdx
            }
            else {
                //TRACE_ERROR("ERROR! 2 invalid grpCnt");
                TRACE_ERROR("&2:");
                return FINE_MEM_ERROR; //mem-error
            }
            TRACE_NOP;
        }
        else {
            TRACE_ERROR("ERROR! 3 invalid cnt >= MAX_NUM_INST_FINE %d %d", cnt, BS_inst_xy_grp_cnt);
            return FINE_MEM_ERROR; //mem-error
        }
    } //if (cnt >= initialGrpCntMin)
    else {
        BS_inst_xy_grp_cnt -= cnt;
        IS_DEBUG_FLAG{ TRACE_FMG3(" ..del cnt,BS_inst_xy_grp_cnt: %d %d", cnt, BS_inst_xy_grp_cnt); };
        if (BS_inst_xy_grp_cnt < 0) {
            TRACE_ERROR_MEM("ERROR_MEM! invalid BS_inst_xy_grp_cnt: %d", BS_inst_xy_grp_cnt);
            BS_inst_xy_grp_cnt = 0;
        }
    }
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };

    //update & save count
    if (grpCnt == 0) {
        //BS_initial_line[initialLineIdx].instLen = -1;
        initial_line[initialLineIdx].sortLen = 0;
        return FINE_NO_GROUP; //no-grp
    }
    BS_initial_line_grp_cnt += grpCnt; //inclease buffer index
    initial_line[initialLineIdx].grpCnt = (uint8_t)grpCnt;

    return FINE_GROUP_FOUND; //found grp
}
#endif

static int fine_get_maxlen_grp(initial_line_group_t *grp, int grpCnt)
{
    int grpIdx, grpCntMaxIdx = 0, grpCntMax = 0;
    for (grpIdx = 0; grpIdx < grpCnt; grpIdx++) {
        if (grp[grpIdx].len >= grpCntMax) {
            grpCntMax = grp[grpIdx].len;
            grpCntMaxIdx = grpIdx;
        }
    }
    grp[grpCntMaxIdx].stat = 1; //set
    return grpCntMaxIdx;
}

#if 0 //for test
static int fine_get_max_density_grp(axis_t axis1, initial_line_group_t *grp, int grpCnt, vec_t *inst)
{
    int minIdx, maxIdx;
    int grpIdx, grpCntMaxIdx, grpCntMax = 0;
    float grpWidth, grpDensity, grpDenMax = 0;
    for (grpIdx = 0; grpIdx < grpCnt; grpIdx++) {
        minIdx = grp[grpIdx].instIdx[0];
        maxIdx = grp[grpIdx].instIdx[grp[grpIdx].len - 1];
        if (axis1 == ENUM_HOR_X) {
            grpWidth = GET_ABS(inst[maxIdx].y - inst[minIdx].y);
        }
        else {
            grpWidth = GET_ABS(inst[maxIdx].x - inst[minIdx].x);
        }
        grpDensity = grp[grpIdx].len / grpWidth;
        IS_DEBUG_FLAG { TRACE("grpDensity=%0.2f %d %0.1f", grpDensity, grp[grpIdx].len, grpWidth);};
        if (grpDensity > grpDenMax || (GET_ABS(grpDensity - grpDenMax) < EPSILON && grp[grpIdx].len >= grpCntMax)) {
            grpDenMax = grpDensity;
            grpCntMax = grp[grpIdx].len;
            grpCntMaxIdx = grpIdx;
        }
    }
    grp[grpCntMaxIdx].stat = 1; //set
    return grpCntMaxIdx;
}
#endif

static int fine_make_grp_min_max(axis_t axis1, initial_line_group_t *grp, int grpIdx, vec_t *inst)
{
    int minIdx, maxIdx;
    float minPos, maxPos, minPos2, maxPos2;

    minIdx = grp[grpIdx].instIdx[0];
    maxIdx = grp[grpIdx].instIdx[grp[grpIdx].len - 1];
    if (axis1 == ENUM_HOR_X) {
        if (inst[minIdx].y < inst[maxIdx].y) {
            minPos = inst[minIdx].y;
            maxPos = inst[maxIdx].y;
        }
        else {
            maxPos = inst[minIdx].y;
            minPos = inst[maxIdx].y;
        }
        if (inst[minIdx].x < inst[maxIdx].x) {
            minPos2 = inst[minIdx].x;
            maxPos2 = inst[maxIdx].x;
        }
        else {
            maxPos2 = inst[minIdx].x;
            minPos2 = inst[maxIdx].x;
        }
    }
    else {
        if (inst[minIdx].x < inst[maxIdx].x) {
            minPos = inst[minIdx].x;
            maxPos = inst[maxIdx].x;
        }
        else {
            maxPos = inst[minIdx].x;
            minPos = inst[maxIdx].x;
        }
        if (inst[minIdx].y < inst[maxIdx].y) {
            minPos2 = inst[minIdx].y;
            maxPos2 = inst[maxIdx].y;
        }
        else {
            maxPos2 = inst[minIdx].y;
            minPos2 = inst[maxIdx].y;
        }
    }
    grp[grpIdx].instPos.min = minPos;
    grp[grpIdx].instPos.max = maxPos;
    grp[grpIdx].instPos2.min = minPos2;
    grp[grpIdx].instPos2.max = maxPos2;
    return 0;
}

#if (DEBUG_fine_make_group > 0)
ATTR_BACKEND_RAMFUNC
static void DEBUG_dump_grp_info(axis_t axis1, int idNo)
{
    int i, k, lineIdx;
    float cent_2; //cent,
    initial_line_t *initial_line;
    int initial_line_cnt;

    TRACE("DEBUG_dump_grp_info.. %d-%d", axis1, idNo);

    k = (axis1 == ENUM_VER_Y);
    initial_line = (k) ? BS_initial_line_y : BS_initial_line_x;
    initial_line_cnt = (k) ? BS_initial_line_y_cnt : BS_initial_line_x_cnt;

    for (lineIdx = 0; lineIdx < initial_line_cnt; lineIdx++)
    {
        if (initial_line[lineIdx].sortLen <= 0) {
            TRACENR("~*~ sortLen : %d %d", lineIdx, initial_line[lineIdx].sortLen);
            continue;
        }
        int grpCnt = initial_line[lineIdx].grpCnt;
        initial_line_group_t *grp = initial_line[lineIdx].grp;
        //cent = (axis1 == ENUM_HOR_X) ? initial_line[lineIdx].cent.x : initial_line[lineIdx].cent.y;
        if (axis1 == ENUM_HOR_X) {
            TRACENR("~X~ %d(%d)(%0.3f,--):", lineIdx, grpCnt, initial_line[lineIdx].cent.x);
        }
        else {
            TRACENR("~Y~ %d(%d)(%0.3f,%0.3f):", lineIdx, grpCnt, initial_line[lineIdx].cent.x, initial_line[lineIdx].cent.y);
        }
        //if (initial_line[lineIdx].sortLen <= 0) {
        //    TRACE(" ~del(%d)", grpCnt);
        //    continue;
        //}
        for (i = 0; i < grpCnt; i++) {
            //TRACENR(" %d(%d/%d)%x", i, grp[i].len, grp[i].stat, &grp[i]);
            cent_2 = (grp[i].instPos.min + grp[i].instPos.max) * 0.5f;
            TRACENR(" %d(%d/%d)(%d/%0.2f)(%0.1f/%0.1f)", i, grp[i].len, grp[i].stat, grp[i].cxLineIdx, cent_2, grp[i].instPos.min, grp[i].instPos.max);
#if (DEBUG_fine_make_group > 0)
            IS_DEBUG_FLAG {
                //if (grp[i].stat == 1)
                {
                    vec_t *inst = initial_line[lineIdx].inst;
                    int m;
                    for (m = 0 ; m < grp[i].len; m++) {
                        int instIdx = grp[i].instIdx[m];
                        if (axis1 == ENUM_HOR_X) {
                            DEBUG_SHOW_POS(&inst[instIdx], 0.10f, 0.10f, grp[i].cxLineIdx/*(lineIdx+1) * i*/);
                        }
                        else {
                            DEBUG_SHOW_POS(&inst[instIdx], 0.10f, 0.10f, lineIdx/*(lineIdx+1) * i*/);
                        }
                        //TRACENR(".(%0.3f/%0.3f)", inst[instIdx].x, inst[instIdx].y);
                    }
                }
            };
#endif
        }
        TRACE(".");
    }
}
#endif

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_closed_min_max    1
#endif
#if (DEBUG_closed_min_max == 1)
#define TRACE_CDMM(...)  //TRACE(__VA_ARGS__)
#else
#define TRACE_CDMM(...)
#endif

#if 0 //def FINE_REMAINED_INIT_LINE_MAX //nsmoon@211014
static int fine_is_closed_tp_min_max2(pos_min_max_t *mM0, int startIdx, int endIdx, int endIdxOrg)
{
    int tIdx;
    pos_min_max_t mM;
    vec_t centerPoint, touchHalfWidth;
    float diffDistX, diffDistY;
    int skipCnt = 0;//nsmoon@211014
    IS_DEBUG_FLAG{ TRACE_CDMM("mM0=%0.1f/%0.1f %0.1f/%0.1f (%d %d)", mM0->minX, mM0->maxX, mM0->minY, mM0->maxY, startIdx, endIdx); };

    for (tIdx = startIdx; tIdx < endIdx; tIdx++) {
        centerPoint.x = BG_touch_data[tIdx].x;
        centerPoint.y = BG_touch_data[tIdx].y;
        touchHalfWidth.x = BG_touch_size[tIdx].xSize * 0.5f;
        touchHalfWidth.y = BG_touch_size[tIdx].ySize * 0.5f;
        mM.minX = centerPoint.x - touchHalfWidth.x;
        mM.maxX = centerPoint.x + touchHalfWidth.x;
        mM.minY = centerPoint.y - touchHalfWidth.y;
        mM.maxY = centerPoint.y + touchHalfWidth.y;
        diffDistX = GET_MIN(GET_ABS(mM.minX - mM0->maxX), GET_ABS(mM.maxX - mM0->minX));
        diffDistY = GET_MIN(GET_ABS(mM.minY - mM0->maxY), GET_ABS(mM.maxY - mM0->minY));
        IS_DEBUG_FLAG {TRACE_CDMM(" mM:(%d)%0.1f,%0.1f(%0.1f/%0.1f,1%0.1f/%0.1f)", skipCnt, diffDistX, diffDistY, mM.minX, mM.maxX, mM.minY, mM.maxY);};

        if (diffDistY < FINE_MAX_CLOSED_TP_MINMAX_GAP && diffDistX < FINE_MAX_CLOSED_TP_MINMAX_GAP) {
            IS_DEBUG_FLAG {TRACE_CDMM(" =>CLOSED TP~~");};
            return 1; //closed y
        }
        if (tIdx < endIdxOrg) {
            if (diffDistY < FINE_MAX_BETWEEN_TP_MINMAX_GAP_Y && diffDistX < FINE_MAX_BETWEEN_TP_MINMAX_GAP_X) {
                IS_DEBUG_FLAG {TRACE_CDMM(" =>SAME Y-AXIS~1~");};
                return 1; //same y-axis
            }
            if (diffDistY < FINE_MAX_BETWEEN_TP_MINMAX_GAP_Y2 && diffDistX < FINE_MAX_BETWEEN_TP_MINMAX_GAP_X) {
                skipCnt++;
                if (skipCnt > 1) {
                    IS_DEBUG_FLAG {TRACE_CDMM(" =>SAME Y-AXIS~2~");};
                    return 1; //same y-axis
                }
            }
        }
    } //for (tIdx = 0; tIdx < BG_touch_count; tIdx++)

    return 0; //not-closed
}
#else
static int fine_is_closed_tp_min_max(pos_min_max_t *mM0, int startIdx, int endIdx)
{
    int tIdx;
    pos_min_max_t mM;
    vec_t centerPoint, touchHalfWidth;
    float diffDistX, diffDistY;
    IS_DEBUG_FLAG{ TRACE_CDMM(" fine_is_closed_tp_min_max..(%d,%d) %0.1f/%0.1f %0.1f/%0.1f",
                              startIdx, endIdx, mM0->minX, mM0->maxX, mM0->minY, mM0->maxY); };

#if 1 //nsmoon@200312
    for (tIdx = startIdx; tIdx < endIdx; tIdx++) {
        centerPoint.x = BG_touch_data[tIdx].x;
        centerPoint.y = BG_touch_data[tIdx].y;
        touchHalfWidth.x = BG_touch_size[tIdx].xSize * 0.5f;
        touchHalfWidth.y = BG_touch_size[tIdx].ySize * 0.5f;
        mM.minX = centerPoint.x - touchHalfWidth.x;
        mM.maxX = centerPoint.x + touchHalfWidth.x;
        mM.minY = centerPoint.y - touchHalfWidth.y;
        mM.maxY = centerPoint.y + touchHalfWidth.y;
        diffDistX = GET_MIN(GET_ABS(mM.minX - mM0->minX), GET_ABS(mM.maxX - mM0->maxX));
        diffDistX = GET_MIN(diffDistX, GET_ABS(mM.minX - mM0->maxX));
        diffDistY = GET_MIN(GET_ABS(mM.minY - mM0->minY), GET_ABS(mM.maxY - mM0->maxY));
        diffDistY = GET_MIN(diffDistY, GET_ABS(mM.minY - mM0->maxY));
        IS_DEBUG_FLAG {TRACE_CDMM(" mM: (%d) %0.1f %0.1f (%0.1f/%0.1f %0.1f/%0.1f)", tIdx,
                                  diffDistX, diffDistY, mM.minX, mM.maxX, mM.minY, mM.maxY);};

        if (diffDistY < FINE_MAX_CLOSED_TP_MINMAX_GAP && diffDistX < FINE_MAX_CLOSED_TP_MINMAX_GAP) {
            IS_DEBUG_FLAG {TRACE_CDMM("CLOSED TP~~,%0.1f,%0.1f,", diffDistX, diffDistY);};
            return 1; //closed y
        }
    } //for (tIdx = 0; tIdx < BG_touch_count; tIdx++)
#else
    int ol_x1, ol_x2, ol_x3, ol_x4, ol_y1, ol_y2, ol_y3, ol_y4;
    for (tIdx = startIdx; tIdx < endIdx; tIdx++) {
        centerPoint.x = BG_touch_data[tIdx].x;
        centerPoint.y = BG_touch_data[tIdx].y;
        touchHalfWidth.x = BG_touch_size[tIdx].xSize * 0.5f;
        touchHalfWidth.y = BG_touch_size[tIdx].ySize * 0.5f;
        mM.minX = centerPoint.x - touchHalfWidth.x;
        mM.maxX = centerPoint.x + touchHalfWidth.x;
        mM.minY = centerPoint.y - touchHalfWidth.y;
        mM.maxY = centerPoint.y + touchHalfWidth.y;
        IS_DEBUG_FLAG {TRACE_CDMM("mM: %0.1f/%0.1f %0.1f/%0.1f", mM.minX, mM.maxX, mM.minY, mM.maxY);};

        ol_x1 = (mM.minX > (mM0->minX - FINE_MAX_OVERLAP_TP_MINMAX_GAP) && mM.minX < (mM0->maxX + FINE_MAX_OVERLAP_TP_MINMAX_GAP));
        ol_x2 = (mM.maxX > (mM0->minX - FINE_MAX_OVERLAP_TP_MINMAX_GAP) && mM.maxX < (mM0->maxX + FINE_MAX_OVERLAP_TP_MINMAX_GAP));
        ol_x3 = (mM0->minX > (mM.minX - FINE_MAX_OVERLAP_TP_MINMAX_GAP) && mM0->minX < (mM.maxX + FINE_MAX_OVERLAP_TP_MINMAX_GAP));
        ol_x4 = (mM0->maxX > (mM.minX - FINE_MAX_OVERLAP_TP_MINMAX_GAP) && mM0->maxX < (mM.maxX + FINE_MAX_OVERLAP_TP_MINMAX_GAP));
        if (ol_x1 || ol_x2 || ol_x3 || ol_x4) {
            //same x-axis
            diffDistY = GET_MIN(GET_ABS(mM.minY - mM0->maxY), GET_ABS(mM.maxY - mM0->minY));
            if (diffDistY < FINE_MAX_CLOSED_TP_MINMAX_GAP) {
                IS_DEBUG_FLAG {TRACE_CDMM("closed y~ %0.1f", diffDistX);};
                return 1; //closed y
            }
        }

        ol_y1 = (mM.minY > (mM0->minY - FINE_MAX_OVERLAP_TP_MINMAX_GAP) && mM.minY < (mM0->maxY + FINE_MAX_OVERLAP_TP_MINMAX_GAP));
        ol_y2 = (mM.maxY > (mM0->minY - FINE_MAX_OVERLAP_TP_MINMAX_GAP) && mM.maxY < (mM0->maxY + FINE_MAX_OVERLAP_TP_MINMAX_GAP));
        ol_y3 = (mM0->minY > (mM.minY - FINE_MAX_OVERLAP_TP_MINMAX_GAP) && mM0->minY < (mM.maxY + FINE_MAX_OVERLAP_TP_MINMAX_GAP));
        ol_y4 = (mM0->maxY > (mM.minY - FINE_MAX_OVERLAP_TP_MINMAX_GAP) && mM0->maxY < (mM.maxY + FINE_MAX_OVERLAP_TP_MINMAX_GAP));
        if (ol_y1 || ol_y2 || ol_y3 || ol_y4) {
            //same y-axis
            diffDistX = GET_MIN(GET_ABS(mM.minX - mM0->maxX), GET_ABS(mM.maxX - mM0->minX));
            if (diffDistX < FINE_MAX_CLOSED_TP_MINMAX_GAP) {
                IS_DEBUG_FLAG {TRACE_CDMM("closed x~ %0.1f", diffDistX);};
                return 2; //closed x
            }
        }
    } //for (tIdx = 0; tIdx < BG_touch_count; tIdx++)
#endif
    return 0; //not-closed
}
#endif

#if 0 //nsmoon@220117 //for test
#define TRACE_FIGMM(...)    TRACE(__VA_ARGS__)
#define FINE_XY_GHOST_DIST_MAX      10.0f
#define FINE_XY_GHOST_DIST2_MAX     60.0f
#define FINE_XY_GHOST_TP_SIZE_MAX   6.0f
static int fine_is_xy_ghost_min_max(pos_min_max_t *mM0, int startIdx, int endIdx)
{
    int i, j;
    vec_t centerPoint_0;
    float diffDistX, diffDistY;
    axis_t axis2;

    centerPoint_0.x = (mM0->minX + mM0->maxX) * 0.5f;
    centerPoint_0.y = (mM0->minY + mM0->maxY) * 0.5f;
    IS_DEBUG_FLAG{ TRACE_FIGMM("fine_is_ghost_min_max..(%d,%d) %0.1f/%0.1f", startIdx, endIdx, centerPoint_0.x, centerPoint_0.y); };

    for (i = startIdx; i < endIdx; i++) {
        if (BG_touch_size[i].xSize > FINE_XY_GHOST_TP_SIZE_MAX || BG_touch_size[i].ySize > FINE_XY_GHOST_TP_SIZE_MAX) {
            continue;
        }
        axis2 = ENUM_AXIS_END;
        diffDistX = GET_ABS(BG_touch_data[i].x - centerPoint_0.x);
        diffDistY = GET_ABS(BG_touch_data[i].y - centerPoint_0.y);
        IS_DEBUG_FLAG{ TRACE_FIGMM(" ..%0.1f/%0.1f %0.1f/%0.1f", BG_touch_data[i].x, BG_touch_data[i].y, diffDistX, diffDistY); };
        if (diffDistX < FINE_XY_GHOST_DIST_MAX && diffDistY < FINE_XY_GHOST_DIST2_MAX) {
            axis2 = ENUM_VER_Y;
        }
        if (diffDistX < FINE_XY_GHOST_DIST2_MAX && diffDistY < FINE_XY_GHOST_DIST_MAX) {
            axis2 = ENUM_HOR_X;
        }
        if (axis2 == ENUM_AXIS_END) {
            continue;
        }

        for (j = startIdx; j < endIdx; j++) {
            if (i == j) continue;
            if (BG_touch_size[j].xSize > FINE_XY_GHOST_TP_SIZE_MAX || BG_touch_size[j].ySize > FINE_XY_GHOST_TP_SIZE_MAX) {
                continue;
            }
            diffDistX = GET_ABS(BG_touch_data[j].x - centerPoint_0.x);
            diffDistY = GET_ABS(BG_touch_data[j].y - centerPoint_0.y);
            IS_DEBUG_FLAG{ TRACE_FIGMM(" ...%d %0.1f/%0.1f %0.1f/%0.1f", axis2, BG_touch_data[j].x, BG_touch_data[j].y, diffDistX, diffDistY); };
            if (axis2 == ENUM_HOR_X && diffDistX < FINE_XY_GHOST_DIST_MAX && diffDistY < FINE_XY_GHOST_DIST2_MAX) {
                return 1;
            }
            if (axis2 == ENUM_VER_Y && diffDistX < FINE_XY_GHOST_DIST2_MAX && diffDistY < FINE_XY_GHOST_DIST_MAX) {
                return 1;
            }
        } //for (j = startIdx; j < endIdx; j++)
    } //for (i = startIdx; i < endIdx; i++)
    return 0; //not-closed
}
#endif

#if 1 //nsmoon@211102
static int fine_is_closed_tp_shadow(axis_t axis2, pos_min_max_t *mM0, int startIdx, int endIdx)
{
    int tIdx;
    pos_min_max_t mM;
    vec_t centerPoint, touchHalfWidth;
    float diffDistX, diffDistY;
    IS_DEBUG_FLAG{
        TRACE_CDMM("fine_is_closed_tp_shadow..(%d,%d) %0.1f/%0.1f %0.1f/%0.1f", startIdx, endIdx, mM0->minX, mM0->maxX, mM0->minY, mM0->maxY);
    };

    for (tIdx = startIdx; tIdx < endIdx; tIdx++) {
        centerPoint.x = BG_touch_data[tIdx].x;
        centerPoint.y = BG_touch_data[tIdx].y;
        touchHalfWidth.x = BG_touch_size[tIdx].xSize * 0.5f;
        touchHalfWidth.y = BG_touch_size[tIdx].ySize * 0.5f;
        mM.minX = centerPoint.x - touchHalfWidth.x;
        mM.maxX = centerPoint.x + touchHalfWidth.x;
        mM.minY = centerPoint.y - touchHalfWidth.y;
        mM.maxY = centerPoint.y + touchHalfWidth.y;
        diffDistX = GET_MIN(GET_ABS(mM.minX - mM0->minX), GET_ABS(mM.maxX - mM0->maxX));
        diffDistX = GET_MIN(diffDistX, GET_ABS(mM.minX - mM0->maxX));
        diffDistY = GET_MIN(GET_ABS(mM.minY - mM0->minY), GET_ABS(mM.maxY - mM0->maxY));
        diffDistY = GET_MIN(diffDistY, GET_ABS(mM.minY - mM0->maxY));
        IS_DEBUG_FLAG {
            TRACE_CDMM("mM: %0.1f %0.1f (%0.1f/%0.1f %0.1f/%0.1f)", diffDistX, diffDistY, mM.minX, mM.maxX, mM.minY, mM.maxY);
        };

        if (diffDistY < FINE_MAX_CLOSED_TP_MINMAX_GAP && diffDistX < FINE_MAX_CLOSED_TP_MINMAX_GAP) {
            IS_DEBUG_FLAG {TRACE_CDMM("CLOSED TP~~,%0.1f,%0.1f,", diffDistX, diffDistY);};
            return 1; //closed x and y
        }
        if (axis2 == ENUM_VER_Y) {
            if (diffDistX < FINE_MAX_CLOSED_TP_1P_SPEED_GAP && diffDistY < FINE_MAX_CLOSED_TP_1P_SPEED_OPP_GAP) {
                IS_DEBUG_FLAG {TRACE_CDMM("CLOSED TP SAME Y~~,%0.1f,%0.1f,", diffDistX, diffDistY);};
                return 1; //closed x
            }
        }
        else {
            if (diffDistY < FINE_MAX_CLOSED_TP_1P_SPEED_GAP && diffDistX < FINE_MAX_CLOSED_TP_1P_SPEED_OPP_GAP) {
                IS_DEBUG_FLAG {TRACE_CDMM("CLOSED TP SAME X~~,%0.1f,%0.1f,", diffDistX, diffDistY);};
                return 1; //closed y
            }
        }
    } //for (tIdx = 0; tIdx < BG_touch_count; tIdx++)

    return 0; //not-closed
}
#endif

#if 0 //for test //nsmoon@200312
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_fine_shadow_cal_cent_line_size    0
#endif
#if (DEBUG_fine_shadow_cal_cent_line_size > 0)
#define TRACE_FSCCLS(...)  TRACE(__VA_ARGS__)
#else
#define TRACE_FSCCLS(...)
#endif
#define FINE_CLOSED_TP_CENT_AXIS_MARGIN    50.0f //nsmoon@211025 50.0f=>100.0f
static int fine_shadow_cal_cent_line_size(axis_t axis, int side, vec_t *cent, int tIdxIn, float *gap)
{
    int tIdx, ret = 0;
    float minPos, maxPos;
    float centerPoint, centerPointOpp, touchHalfWidth;
    float diffDist, diffDistOpp, diffDistMin;
    int diffDistMinIdx;
    int touchCntOrg = BG_touch_count;
    float diffDistMax = (*gap);
    IS_DEBUG_FLAG{ TRACE_FSCCLS("fine_shadow_cal_cent_line_size..(%d/%d/%d) %0.1f(%0.1f,%0.1f)", tIdxIn, axis, side, diffDistMax, cent->x, cent->y); };

    diffDistMin = MIN_INITIAL_VAL;
    diffDistMinIdx = -1;
    for (tIdx = 0; tIdx < touchCntOrg; tIdx++)
    {
        if (tIdx == tIdxIn) continue;
        if (axis == ENUM_HOR_X) {
            centerPoint = BG_touch_data[tIdx].x;
            touchHalfWidth = BG_touch_size[tIdx].xSize * 0.5f;
            minPos = centerPoint - touchHalfWidth;
            maxPos = centerPoint + touchHalfWidth;
            if (side == FINE_CENT_LFT) {
                if (minPos < cent->x) {
                    continue;
                }
                diffDist = minPos - cent->x;
            }
            else {
                //FINE_CENT_RHT
                if (maxPos > cent->x) {
                    continue;
                }
                diffDist = cent->x - maxPos;
            }
            centerPointOpp = BG_touch_data[tIdx].y;
            diffDistOpp = GET_ABS(centerPointOpp - cent->y);
        }
        else {
            centerPoint = BG_touch_data[tIdx].y;
            touchHalfWidth = BG_touch_size[tIdx].ySize * 0.5f;
            minPos = centerPoint - touchHalfWidth;
            maxPos = centerPoint + touchHalfWidth;
            diffDist = GET_MIN(GET_ABS(minPos - cent->y), GET_ABS(maxPos - cent->y));
            if (side == FINE_CENT_TOP) {
                if (minPos < cent->y) {
                    continue;
                }
                diffDist = minPos - cent->y;
            }
            else {
                //FINE_CENT_BOT
                if (maxPos > cent->y) {
                    continue;
                }
                diffDist = cent->y - maxPos;
            }
            centerPointOpp = BG_touch_data[tIdx].x;
            diffDistOpp = GET_ABS(centerPointOpp - cent->x);
        }

        //IS_DEBUG_FLAG {TRACE_FSCCLS("  mM: %0.1f(%0.1f,%0.1f) %0.1f ", diffDist, minPos, maxPos, diffDistOpp);};
        //if (diffDist > 0 && diffDist < diffDistMax && diffDistOpp < FINE_CLOSED_TP_CENT_AXIS_MARGIN)
        if (diffDist > 0 && diffDistOpp < FINE_CLOSED_TP_CENT_AXIS_MARGIN) //nsmoon@211025
        {
            //IS_DEBUG_FLAG {TRACE_FSCCLS("  =>CLOSED tp~~~");};
            if (diffDist < diffDistMin) {
                diffDistMin = diffDist;
                diffDistMinIdx = tIdx;
            }
            //return 1; //closed
        }
    } //for (tIdx = 0; tIdx < BG_touch_count; tIdx++)

    if (diffDistMinIdx < 0) {
    if (axis == ENUM_HOR_X) {
        if (side == FINE_CENT_LFT) {
            (*gap) = BS_aarea_end_x - cent->x;
        }
        else {
            //FINE_CENT_RHT
            (*gap) = cent->x - BS_aarea_zero_x;
        }
    }
    else {
        //ENUM_VER_Y
        if (side == FINE_CENT_TOP) {
            (*gap) = BS_aarea_end_y - cent->y;
        }
        else {
            //FINE_CENT_BOT
            (*gap) = cent->y - BS_aarea_zero_y;
        }
    }
        return ret; //not-closed
    }

    IS_DEBUG_FLAG{ TRACE_FSCCLS(" =>diffDistMin=%0.1f %0.1f", diffDistMin, diffDistMax); };
    if (diffDistMin < diffDistMax) {
        ret = 1;
    }
    (*gap) = diffDistMin;
    return ret; //closed
}
#endif

#if 0 //reserved, nsmoon@200309
static int fine_is_closed_tp_cent(vec_t *cent)
{
    int tIdx;
    vec_t centerPoint;
    float diffDistX, diffDistY;

    for (tIdx = 0; tIdx < BG_touch_count; tIdx++) {
        centerPoint.x = BG_touch_data[tIdx].x;
        centerPoint.y = BG_touch_data[tIdx].y;
        diffDistX = GET_ABS(cent->x - centerPoint.x);
        diffDistY = GET_ABS(cent->y - centerPoint.y);
        IS_DEBUG_FLAG{ TRACE_CDMM("diffDistX,diffDistY: %0.1f/%0.1f", diffDistX, diffDistY); };
        if (diffDistX < FINE_MAX_CLOSED_TP_CENT_GAP && diffDistY < FINE_MAX_CLOSED_TP_CENT_GAP) {
            IS_DEBUG_FLAG{ TRACE_CDMM("closed~ %0.1f/%0.1f", diffDistX, diffDistY); };
            return 1; //closed
        }
    } //for (tIdx = 0; tIdx < BG_touch_count; tIdx++)

    return 0; //not-closed
}
#endif

#if 0 //reserved, nsmoon@200307
static int fine_is_closed_one_tp_min_max(axis_t axis, pos_minMax2_t *mM0, int tIdx)
{
    pos_min_max_t mM;
    vec_t centerPoint, touchHalfWidth;
    float diffDistX, diffDistY;
    IS_DEBUG_FLAG{ TRACE_CDMM("mM0: %0.1f/%0.1f", mM0->min, mM0->max); };

    centerPoint.x = BG_touch_data[tIdx].x;
    centerPoint.y = BG_touch_data[tIdx].y;
    touchHalfWidth.x = BG_touch_size[tIdx].xSize * 0.5f;
    touchHalfWidth.y = BG_touch_size[tIdx].ySize * 0.5f;
    mM.minX = centerPoint.x - touchHalfWidth.x;
    mM.maxX = centerPoint.x + touchHalfWidth.x;
    mM.minY = centerPoint.y - touchHalfWidth.y;
    mM.maxY = centerPoint.y + touchHalfWidth.y;
    IS_DEBUG_FLAG{ TRACE_CDMM("mM: %0.1f/%0.1f %0.1f/%0.1f", mM.minX, mM.maxX, mM.minY, mM.maxY); };

    if (axis == ENUM_HOR_X) {
        //same x-axis
        diffDistY = GET_MIN(GET_ABS(mM.minY - mM0->max), GET_ABS(mM.maxY - mM0->min));
        if (diffDistY < FINE_MAX_CLOSED_TP_MINMAX_GAP) {
            IS_DEBUG_FLAG{ TRACE_CDMM("closed y~~ %0.1f", diffDistY); };
            return 1; //closed y
        }
    }
    else { //(axis == ENUM_HOR_X)
           //same y-axis
        diffDistX = GET_MIN(GET_ABS(mM.minX - mM0->max), GET_ABS(mM.maxX - mM0->min));
        if (diffDistX < FINE_MAX_CLOSED_TP_MINMAX_GAP) {
            IS_DEBUG_FLAG{ TRACE_CDMM("closed x~~ %0.1f", diffDistX); };
            return 2; //closed x
        }
    }

    return 0; //not-closed
}
#endif
#if (BRUSH_MODE_ENABLE == 1)
static int fine_is_overlap_tp_min_max(pos_min_max_t *mM0, int startIdx, int endIdx)
{
    int tIdx;
    pos_min_max_t mM;
    vec_t centerPoint, touchHalfWidth;
    int ol_x1, ol_x2, ol_x3, ol_x4, ol_y1, ol_y2, ol_y3, ol_y4;
    float max_overlap_gap = FINE_MAX_OVERLAP_TP_MINMAX_GAP;
#if (BRUSH_MODE_ENABLE == 1)
    if (BS_brushMode) {
        max_overlap_gap = FINE_MAX_BRUSH_NEAR_TP_GAP;
    }
#endif
    IS_DEBUG_FLAG {TRACE_CDMM("mM0::=%0.1f/%0.1f,%0.1f/%0.1f", mM0->minX, mM0->maxX, mM0->minY, mM0->maxY);};

    for (tIdx = startIdx; tIdx < endIdx; tIdx++) {
        centerPoint.x = BG_touch_data[tIdx].x;
        centerPoint.y = BG_touch_data[tIdx].y;
        touchHalfWidth.x = BG_touch_size[tIdx].xSize * 0.5f;
        touchHalfWidth.y = BG_touch_size[tIdx].ySize * 0.5f;
        mM.minX = centerPoint.x - touchHalfWidth.x;
        mM.maxX = centerPoint.x + touchHalfWidth.x;
        mM.minY = centerPoint.y - touchHalfWidth.y;
        mM.maxY = centerPoint.y + touchHalfWidth.y;
        IS_DEBUG_FLAG {TRACE_CDMM("mM: %0.1f/%0.1f %0.1f/%0.1f", mM.minX, mM.maxX, mM.minY, mM.maxY);};
        ol_x1 = (mM.minX > (mM0->minX - max_overlap_gap) && mM.minX < (mM0->maxX + max_overlap_gap));
        ol_x2 = (mM.maxX > (mM0->minX - max_overlap_gap) && mM.maxX < (mM0->maxX + max_overlap_gap));
        ol_x3 = (mM0->minX > (mM.minX - max_overlap_gap) && mM0->minX < (mM.maxX + max_overlap_gap));
        ol_x4 = (mM0->maxX > (mM.minX - max_overlap_gap) && mM0->maxX < (mM.maxX + max_overlap_gap));
        ol_y1 = (mM.minY > (mM0->minY - max_overlap_gap) && mM.minY < (mM0->maxY + max_overlap_gap));
        ol_y2 = (mM.maxY > (mM0->minY - max_overlap_gap) && mM.maxY < (mM0->maxY + max_overlap_gap));
        ol_y3 = (mM0->minY > (mM.minY - max_overlap_gap) && mM0->minY < (mM.maxY + max_overlap_gap));
        ol_y4 = (mM0->maxY > (mM.minY - max_overlap_gap) && mM0->maxY < (mM.maxY + max_overlap_gap));
        if ((ol_x1 || ol_x2 || ol_x3 || ol_x4) && (ol_y1 || ol_y2 || ol_y3 || ol_y4)) {
            IS_DEBUG_FLAG {TRACE_CDMM(" =>overlapped..");};
            return 1; //overlap
        }
    } //for (tIdx = 0; tIdx < BG_touch_count; tIdx++)

    return 0; //not-overlapped
}
#endif //1

#if 0 //for test //nsmoon@211018 1=>0
static int fine_is_overlap_tinfo_min_max(pos_min_max_t *mM0, int endIdx)
{
    int tIdx;
    pos_min_max_t mM;
    float max_overlap_gap = FINE_MAX_OVERLAP_TP_MINMAX_GAP;
    touch_info_fine_t *touch_info_fine = &BS_touch_info_fine[0];
    int ol_x1, ol_x2, ol_x3, ol_x4, ol_y1, ol_y2, ol_y3, ol_y4;
    IS_DEBUG_FLAG {TRACE_CDMM("mM0:=%0.1f/%0.1f,%0.1f/%0.1f", mM0->minX, mM0->maxX, mM0->minY, mM0->maxY);};

    for (tIdx = 0; tIdx < endIdx; tIdx++) {
        //centerPoint = touch_info_fine[tIdx].centerPos;
        mM = touch_info_fine[tIdx].mM;
        IS_DEBUG_FLAG {TRACE_CDMM("mM: %0.1f/%0.1f %0.1f/%0.1f", mM.minX, mM.maxX, mM.minY, mM.maxY);};
        ol_x1 = (mM.minX > (mM0->minX - max_overlap_gap) && mM.minX < (mM0->maxX + max_overlap_gap));
        ol_x2 = (mM.maxX > (mM0->minX - max_overlap_gap) && mM.maxX < (mM0->maxX + max_overlap_gap));
        ol_x3 = (mM0->minX > (mM.minX - max_overlap_gap) && mM0->minX < (mM.maxX + max_overlap_gap));
        ol_x4 = (mM0->maxX > (mM.minX - max_overlap_gap) && mM0->maxX < (mM.maxX + max_overlap_gap));
        ol_y1 = (mM.minY > (mM0->minY - max_overlap_gap) && mM.minY < (mM0->maxY + max_overlap_gap));
        ol_y2 = (mM.maxY > (mM0->minY - max_overlap_gap) && mM.maxY < (mM0->maxY + max_overlap_gap));
        ol_y3 = (mM0->minY > (mM.minY - max_overlap_gap) && mM0->minY < (mM.maxY + max_overlap_gap));
        ol_y4 = (mM0->maxY > (mM.minY - max_overlap_gap) && mM0->maxY < (mM.maxY + max_overlap_gap));
        if ((ol_x1 || ol_x2 || ol_x3 || ol_x4) && (ol_y1 || ol_y2 || ol_y3 || ol_y4)) {
            IS_DEBUG_FLAG {TRACE_CDMM(" =>overlapped,,");};
            return 1; //overlap
        }
    } //for (tIdx = 0; tIdx < BG_touch_count; tIdx++)

    return 0; //not-overlapped
}
#endif

#if 0 //for test
static int fine_get_center_line(axis_t axis0, sen_type_t sen0, int tIdx0, int touchCntOrg, vec_t *p0, vec_t *p1)
{
    int tIdx;
    int axis, sen;
    vec_t centerPoint, touchWidth, p2, p3;
    pos_minMax2_t min_max0, min_max;

    centerPoint.x = BG_touch_data[tIdx0].x;
    centerPoint.y = BG_touch_data[tIdx0].y;
    touchWidth.x = BG_touch_size[tIdx0].xSize;
    touchWidth.y = BG_touch_size[tIdx0].ySize;
    if (axis0 == ENUM_VER_Y) {
        if (sen0 == ENUM_LED) {
            p0->x = centerPoint.x + FINE_EDGE_DIST_FROM_TP;
            p1->x = centerPoint.x + (touchWidth.x * 0.5f);
        }
        else { //(sen == ENUM_PD)
            p0->x = centerPoint.x - FINE_EDGE_DIST_FROM_TP;
            p1->x = centerPoint.x - (touchWidth.x * 0.5f);
        }
        p0->y = centerPoint.y;
        p1->y = centerPoint.y;
    }
    else { //(axis0 == ENUM_HOR_X)
        if (sen0 == ENUM_LED) {
            p2->y = centerPoint.y - FINE_EDGE_DIST_FROM_TP;
            p3->y = centerPoint.y - (touchWidth.y * 0.5f);
        }
        else { //(sen == ENUM_PD)
            p2->y = centerPoint.y + FINE_EDGE_DIST_FROM_TP;
            p3->y = centerPoint.y + (touchWidth.y * 0.5f);
        }
        p0->x = centerPoint.x;
        p1->x = centerPoint.x;
    }

    for (tIdx = 0; tIdx < touchCntOrg; tIdx++) {
        if (tIdx == tIdx0) continue;
        centerPoint.x = BG_touch_data[tIdx].x;
        centerPoint.y = BG_touch_data[tIdx].y;
        touchWidth.x = BG_touch_size[tIdx].xSize;
        touchWidth.y = BG_touch_size[tIdx].ySize;

        for (axis = 0; axis < (int)ENUM_AXIS_END; axis++) {
            //0:X(y-grp), 1:Y(x-grp)
            for (sen = 0; sen < (int)ENUM_SEN_END; sen++) {
                //0:PD, 1:LED
                if (axis == (int)ENUM_VER_Y) {
                    if (sen == (int)ENUM_LED) {
                        p2.x = centerPoint.x + FINE_EDGE_DIST_FROM_TP;
                        p3.x = centerPoint.x + (touchWidth.x * 0.5f);
                    }
                    else { //(sen == ENUM_PD)
                        p2.x = centerPoint.x - FINE_EDGE_DIST_FROM_TP;
                        p3.x = centerPoint.x - (touchWidth.x * 0.5f);
                    }
                    p2.y = centerPoint.y;
                    p3.y = centerPoint.y;
                }
                else { //(axis == ENUM_HOR_X)
                    if (sen == (int)ENUM_LED) {
                        p2.y = centerPoint.y - FINE_EDGE_DIST_FROM_TP;
                        p3.y = centerPoint.y - (touchWidth.y * 0.5f);
                    }
                    else { //(sen == ENUM_PD)
                        p2.y = centerPoint.y + FINE_EDGE_DIST_FROM_TP;
                        p3.y = centerPoint.y + (touchWidth.y * 0.5f);
                    }
                    p2.x = centerPoint.x;
                    p3.x = centerPoint.x;
                }

            } //for (sen = 0; sen < (int)ENUM_SEN_END; sen++)
        } //for (axis = 0; axis < (int)ENUM_AXIS_END; axis++)
    }
}
#endif

#if 1 //nsmoon@200206
#define TRACE_FICE(...)    //TRACE(__VA_ARGS__)
#define FINE_ERASE_INCLUDE_GAP			1.0f
static int fine_is_closed_eraser_min_max(axis_t axis, pos_min_max_t mM, float mmGap)
{
    int i;
    pos_min_max_t mM2;
    float halfSizeX, halfSizeY;
    int ol_x1 = 0, ol_x2 = 0, ic_y1 = 0, ic_y2 = 0;
    TRACE_FICE("fine_is_closed_eraser_min_max..mM=(%d) %0.1f(%0.1f,%0.1f)(%0.1f,%0.1f)", axis, mmGap, mM.minX, mM.maxX, mM.minY, mM.maxY);

    for (i = 0; i < BG_touch_count; i++) {
        if (BG_touch_area[i] > TOUCH_ERASER_SIZESQ_XLARGE) {
        halfSizeX = BG_touch_size[i].xSize * 0.5f;
        halfSizeY = BG_touch_size[i].ySize * 0.5f;
        mM2.minX = BG_touch_data[i].x - halfSizeX;
        mM2.maxX = BG_touch_data[i].x + halfSizeX;
        mM2.minY = BG_touch_data[i].y - halfSizeY;
        mM2.maxY = BG_touch_data[i].y + halfSizeY;
            TRACE_FICE("..mM2= (%d)%0.1f(%0.1f,%0.1f)(%0.1f,%0.1f)", i, BG_touch_area[i], mM2.minX, mM2.maxX, mM2.minY, mM2.maxY);

            if (axis == ENUM_HOR_X) {
                ol_x1 = (mM.minX > (mM2.minX - mmGap) && mM.minX < (mM2.maxX + mmGap));
                ol_x2 = (mM.maxX > (mM2.minX - mmGap) && mM.maxX < (mM2.maxX + mmGap));
                ic_y1 = (mM.minY > (mM2.minY - FINE_ERASE_INCLUDE_GAP) && mM.minY < (mM2.maxY + FINE_ERASE_INCLUDE_GAP));
                ic_y2 = (mM.maxY > (mM2.minY - FINE_ERASE_INCLUDE_GAP) && mM.maxY < (mM2.maxY + FINE_ERASE_INCLUDE_GAP));
                TRACE_FICE(".x.%d(%0.1f,%0.1f)(%0.1f,%0.1f)", axis, (mM2.minX - mmGap), (mM.maxX + mmGap), (mM2.minY - FINE_ERASE_INCLUDE_GAP), (mM2.maxY + FINE_ERASE_INCLUDE_GAP));
            }
            else {
                ol_x1 = (mM.minY > (mM2.minY - mmGap) && mM.minY < (mM2.maxY + mmGap));
                ol_x2 = (mM.maxY > (mM2.minY - mmGap) && mM.maxY < (mM2.maxY + mmGap));
                ic_y1 = (mM.minX > (mM2.minX - FINE_ERASE_INCLUDE_GAP) && mM.minX < (mM2.maxX + FINE_ERASE_INCLUDE_GAP));
                ic_y2 = (mM.maxX > (mM2.minX - FINE_ERASE_INCLUDE_GAP) && mM.maxX < (mM2.maxX + FINE_ERASE_INCLUDE_GAP));
                TRACE_FICE(".y.%d(%0.1f,%0.1f)(%0.1f,%0.1f)", axis, (mM2.minY - mmGap), (mM2.maxY + mmGap), (mM2.minX - FINE_ERASE_INCLUDE_GAP), (mM2.maxX + FINE_ERASE_INCLUDE_GAP));
            }
            if ((ol_x1 || ol_x2) && (ic_y1 || ic_y2)) {
                TRACE_FICE("=>ol_x1: (%d) %d %d %d %d", i, ol_x1, ol_x2, ic_y1, ic_y2);
                return 1; //overlapped
            }
        }
    }
    return 0; //not overlapped
}
#endif

#if 1 //nsmoon@200306
#if defined(DEBUG_FUNCTION_ENABLE_ALL) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
#define DEBUG_fine_add_touch_data    0
#endif
#if (DEBUG_fine_add_touch_data > 0)
#define TRACE_FATD(...)     //TRACE(__VA_ARGS__)
#else
#define TRACE_FATD(...)
#endif
//#define MAX_RECAL_MINMAX_WIDTH  25.0f
#define MAX_RECAL_MAX_WIDTH  25.0f
#define MAX_RECAL_MIN_WIDTH  2.0f
#ifdef FINE_REMAINED_INIT_LINE_MAX //nsmoon@211017
static int fine_add_touch_data(axis_t axis2, vec_t *centIn, int centInCnt, int chkSlope, int fineLoop)
{
    int touchCntOrg = BG_touch_count;
    int touchCntFineOrg = BG_touch_count_fine;
    int touchCnt = touchCntOrg;
    int touchCntFine = touchCntFineOrg;
    int touchCntLocal;
    vec_t centerPoint, touchWidth;
    float halfWidthTmp;
    float touchArea;
    pos_min_max_t minMaxTmp;
    pos_minMax2_t min_max, instPos2;
    vec_t p2, p3;
    axis_t axis1;
    int initial_line_cnt;
    initial_line_t *initial_line;
    uint16_t *remained;
    int remainedCnt;
    initial_line_group_t *grp;
    int grp_cnt, grp_max_idx;
    in_line_t *lineSen;
    vec_t retMinPos, retMaxPos;
    float retWidth;
    int ret;
    int k;
    int iniLineCntTmp[FINE_REMAINED_INIT_LINE_MAX];
    int grpMaxIdxTmp[FINE_REMAINED_INIT_LINE_MAX];
    float grpDensity, grpDensityMin;
    int sortLen, isEdgeArea;
    float width;
    IS_DEBUG_FLAG{ 
        TRACE_FATD("fine_add_touch_data...%d %d %d %d", axis2, chkSlope, fineLoop, centInCnt);
        for (k = 0; k < centInCnt; k++) {
            TRACE_FATD("  (%d)(%0.1f %0.1f)", k, centIn[k].x, centIn[k].y);
        }
        TRACE_NOP;
    };

#if 1 //nsmoon@201202
    float maxHalfWidth = FINE_MAX_HALF_WIDTH;
    if (fineLoop == FINE_LOOP_EDGE_X) {
        maxHalfWidth = FINE_MAX_HALF_WIDTH * 2.0f;
    }
#endif

    tp_line_cnt_t thCnt;
    thCnt.th10CntX = thCnt.th50CntX = 0;
    thCnt.th10WidthX = 0;
    thCnt.th10CntY = thCnt.th50CntY = 0;
    thCnt.th10WidthY = 0;

    if (axis2 == ENUM_HOR_X) {
        remained = &BS_remained_x[0];
        remainedCnt = BS_remained_x_cnt;
        axis1 = ENUM_VER_Y;
        initial_line = &BS_initial_line_y[0];
    }
    else { //(axis2 == ENUM_VER_Y)
        remained = &BS_remained_y[0];
        remainedCnt = BS_remained_y_cnt;
        axis1 = ENUM_HOR_X;
        initial_line = &BS_initial_line_x[0];
    }
    if (centInCnt > FINE_REMAINED_INIT_LINE_MAX) {
        TRACE_ERROR("ERROR! fine_add_touch_data..invalid centInCnt %d", centInCnt);
        centInCnt = FINE_REMAINED_INIT_LINE_MAX;
    }

    int centIdx = -1;
    grpDensityMin = MIN_INITIAL_VAL;
    for (k = 0; k < centInCnt; k++) {
        if (axis2 == ENUM_HOR_X) {
            p2.x = centIn[k].x - maxHalfWidth;
            p3.x = centIn[k].x + maxHalfWidth;
            p2.y = centIn[k].y;
            p3.y = p2.y;
        }
        else { //(axis2 == ENUM_VER_Y)
            p2.y = centIn[k].y - maxHalfWidth;
            p3.y = centIn[k].y + maxHalfWidth;
            p2.x = centIn[k].x;
            p3.x = p2.x;
        }
    #if (DEBUG_fine_add_touch_data > 0)
        IS_DEBUG_FLAG{
            DEBUG_SHOW_LINE_POS(&p2, &p3, MY_COLOR);
            TRACE_NOP;
        };
    #endif

        iniLineCntTmp[k] = fine_get_inst3(axis1, p2, p3, axis2, remained, remainedCnt, 0);
        if (iniLineCntTmp[k] == FINE_MEM_ERROR) {
            IS_DEBUG_FLAG{ TRACE_FATD("iniLineCntTmp[k]..FINE_MEM_ERROR");};
            continue; //FINE_MEM_ERROR
        }
        sortLen = initial_line[iniLineCntTmp[k]].sortLen;
        isEdgeArea = initial_line[iniLineCntTmp[k]].isEdgeArea;
    #if 1 //nsmoon@201203
        if (fineLoop == FINE_LOOP_EDGE_X) {
            if ((axis1 == ENUM_VER_Y && IS_INNER_Y(isEdgeArea))
                 && sortLen < FINE_INITIAL_GRP_MIN_NUM_EDGE) {
                IS_DEBUG_FLAG{ TRACE_FATD("  no inst-4a %04x, %d", isEdgeArea, sortLen);};
                continue; //no-grp
            }
        }
        else
    #endif
        if ((((axis1 == ENUM_HOR_X && IS_NOT_NEAR_Y(isEdgeArea)) ||
              (axis1 == ENUM_VER_Y && IS_NOT_NEAR_X(isEdgeArea)))
             && sortLen < FINE_INITIAL_GRP_MIN_NUM) ||
            (((axis1 == ENUM_HOR_X && IS_NEAR_Y(isEdgeArea)) ||
             (axis1 == ENUM_VER_Y && IS_NEAR_X(isEdgeArea)))
             && sortLen < FINE_INITIAL_GRP_MIN_NUM_EDGE)) { //nsmoon@200408
            IS_DEBUG_FLAG{ TRACE_FATD("  no inst-4b %04x, %d", isEdgeArea, sortLen);};
            continue; //no-grp
        }

        ret = fine_make_group3(axis1, iniLineCntTmp[k], axis2, fineLoop);
        if (ret == FINE_MEM_ERROR) {
            IS_DEBUG_FLAG{ TRACE_FATD("  ret..FINE_MEM_ERROR");};
            continue; //FINE_MEM_ERROR
        }
        if (ret != FINE_GROUP_FOUND) {
            IS_DEBUG_FLAG{ TRACE_FATD("no group~1~");};
            continue; //no-grp
        }

        grp = initial_line[iniLineCntTmp[k]].grp;
        grp_cnt = initial_line[iniLineCntTmp[k]].grpCnt;
        if (grp_cnt <= 0) {
            IS_DEBUG_FLAG{ TRACE_FATD("  no group~2~");};
            continue; //not-found
        }
        grpMaxIdxTmp[k] = fine_get_maxlen_grp(grp, grp_cnt);
        fine_make_grp_min_max(axis1, grp, grpMaxIdxTmp[k], initial_line[iniLineCntTmp[k]].inst);
        min_max.min = grp[grpMaxIdxTmp[k]].instPos.min;
        min_max.max = grp[grpMaxIdxTmp[k]].instPos.max;
        width = GET_ABS(min_max.max - min_max.min);
        if (grp[grpMaxIdxTmp[k]].len > 0) {
            grpDensity = width / grp[grpMaxIdxTmp[k]].len;
            IS_DEBUG_FLAG{ TRACE_FATD("  grpDensity=(%d)%0.1f(%0.1f,%d)", k, grpDensity, width, grp[grpMaxIdxTmp[k]].len);};
        }
        else {
            continue; //no-grp
        }
        if (grpDensity < grpDensityMin) {
            grpDensityMin = grpDensity;
                centIdx = k;
            }
    } //for (k = 0; k < centInCnt; k++)
    if (centIdx < 0 || iniLineCntTmp[centIdx] < 0) {
        return 0; //not-found
    }

    initial_line_cnt = iniLineCntTmp[centIdx];
    grp_max_idx = grpMaxIdxTmp[centIdx];
    grp = initial_line[initial_line_cnt].grp;
    grp_cnt = initial_line[initial_line_cnt].grpCnt;
    lineSen = initial_line[initial_line_cnt].lineSen;
    isEdgeArea = initial_line[initial_line_cnt].isEdgeArea;

#if (DEBUG_fine_add_touch_data > 0) && defined(DRAW_POLYGON_TEST)
    int minIdx = grp[grp_max_idx].instIdx[0];
    int maxIdx = grp[grp_max_idx].instIdx[grp[grp_max_idx].len - 1];
    DEBUG_SHOW_MIN_MAX_POS(axis2, &initial_line[initial_line_cnt].inst[0], minIdx, maxIdx, 1);
#endif

    fine_make_grp_min_max(axis1, grp, grp_max_idx, initial_line[initial_line_cnt].inst);
    min_max.min = grp[grp_max_idx].instPos.min;
    min_max.max = grp[grp_max_idx].instPos.max;
    retMinPos.x = 0; retMaxPos.x = 0;
    retMinPos.y = 0; retMaxPos.y = 0;

#ifdef FINE_MIN_CAL_SIZE //nsmoon@210305
    width = GET_ABS(min_max.max - min_max.min);
    if (width > FINE_MIN_CAL_SIZE)
#endif
    {
        ret = fine_cal_min_max3(axis1, initial_line_cnt, grp_max_idx, axis2, &min_max, &retMinPos, &retMaxPos, 1, fineLoop); //1: check slope always, nsmoon@200313
        IS_DEBUG_FLAG{ TRACE_FATD("..min_max=(%d)(%0.1f,%0.1f)(%0.1f,%0.1f)=>(%0.1f,%0.1f) (%0.1f,%0.1f)(%0.1f,%0.1f)", ret, grp[grp_max_idx].instPos.min, grp[grp_max_idx].instPos.max, grp[grp_max_idx].instPos2.min, grp[grp_max_idx].instPos2.max, min_max.min, min_max.max, retMinPos.x, retMaxPos.x, retMinPos.y, retMaxPos.y); };
#if 0 //nsmoon@211018
        if (ret) {
            if (chkSlope) { //nsmoon@200313
                IS_DEBUG_FLAG{ TRACE_FATD("no +/- slope~");};
                return 0; //not-found
            }
        }
#endif
    }

    if (touchCntFine  <= MAX_TOUCH_LIMIT_FINE && touchCnt <= ALLOWABLE_TOUCH_BE) {
        if (axis2 == ENUM_HOR_X) {
            centerPoint.x = (min_max.min + min_max.max) * 0.5f;
            retWidth = GET_ABS(retMaxPos.y - retMinPos.y);
            //IS_DEBUG_FLAG{ TRACE_FATD("-retWidth= %0.1f", retWidth);};
            //if (chkSlope && retWidth < MAX_RECAL_MINMAX_WIDTH) { //nsmoon@200313, nsmoon@200403 FIXME!!!
            IS_DEBUG_FLAG { TRACE_FATD("(X)retWidth=(%d) %0.1f (%0.1f,%0.1f)", chkSlope, retWidth, retMinPos.y, retMaxPos.y); };
#ifdef FINE_MIN_CAL_SIZE //nsmoon@210305
            if (chkSlope && retWidth > MAX_RECAL_MIN_WIDTH && retWidth < MAX_RECAL_MAX_WIDTH) { //nsmoon@200529
#else
            //if (chkSlope && retWidth < MAX_RECAL_MAX_WIDTH) { //nsmoon@211022
            if ((chkSlope && retWidth < MAX_RECAL_MAX_WIDTH) &&
                    (fineLoop < FINE_LOOP_XY_ST && fineLoop > FINE_LOOP_XY_ED)) { //nsmoon@211026
#endif
                centerPoint.y = (retMinPos.y + retMaxPos.y) * 0.5f;
            }
            else {
                centerPoint.y = (grp[grp_max_idx].instPos2.min + grp[grp_max_idx].instPos2.min) * 0.5f;
            }
            touchWidth.x = GET_ABS(min_max.max - min_max.min);
            touchWidth.y = touchWidth.x;
            halfWidthTmp = touchWidth.y *0.5f;
            instPos2.min = centerPoint.y - halfWidthTmp;
            instPos2.max = centerPoint.y + halfWidthTmp;
            minMaxTmp.minX = min_max.min;
            minMaxTmp.maxX = min_max.max;
            minMaxTmp.minY = instPos2.min;
            minMaxTmp.maxY = instPos2.max;
        }
        else { //(axis2 == ENUM_VER_Y)
            centerPoint.y = (min_max.min + min_max.max) * 0.5f;
            retWidth = GET_ABS(retMaxPos.x - retMinPos.x);
            //IS_DEBUG_FLAG{ TRACE_FATD("--retWidth= %0.1f", retWidth);};
            IS_DEBUG_FLAG { TRACE_FATD("(Y)retWidth=(%d) %0.1f (%0.1f,%0.1f)", chkSlope, retWidth, retMinPos.x, retMaxPos.x); };
#ifdef FINE_MIN_CAL_SIZE //nsmoon@210305
            //if (chkSlope && retWidth < MAX_RECAL_MINMAX_WIDTH) { //nsmoon@200313, nsmoon@200403 FIXME!!!
            if (chkSlope && retWidth > MAX_RECAL_MIN_WIDTH && retWidth < MAX_RECAL_MAX_WIDTH) //nsmoon@200529
#else
            //if (chkSlope && retWidth < MAX_RECAL_MAX_WIDTH) { //nsmoon@211022
            if ((chkSlope && retWidth < MAX_RECAL_MAX_WIDTH) &&
                    (fineLoop < FINE_LOOP_XY_ST && fineLoop > FINE_LOOP_XY_ED)) { //nsmoon@211026
#endif
                centerPoint.x = (retMinPos.x + retMaxPos.x) * 0.5f;
            }
            else {
                centerPoint.x = (grp[grp_max_idx].instPos2.min + grp[grp_max_idx].instPos2.min) * 0.5f;
            }
            touchWidth.y = GET_ABS(min_max.max - min_max.min);
            touchWidth.x = touchWidth.y;
            halfWidthTmp = touchWidth.x *0.5f;
            instPos2.min = centerPoint.x - halfWidthTmp;
            instPos2.max = centerPoint.x + halfWidthTmp;
            minMaxTmp.minY = min_max.min;
            minMaxTmp.maxY = min_max.max;
            minMaxTmp.minX = instPos2.min;
            minMaxTmp.maxX = instPos2.max;
        }
#if (DEBUG_fine_add_touch_data > 0) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
            DEBUG_SHOW_POS(&centerPoint, (touchWidth.x*0.5f), (touchWidth.y*0.5f), MY_COLOR);
#endif
        if (IS_NOT_ACTIVE_AREA(centerPoint.x, centerPoint.y)) {
            IS_DEBUG_FLAG { TRACE("fine_add_touch_data..out of black area~1~ %0.1f %0.1f", centerPoint.x, centerPoint.y); };
            fine_add_used_line(axis2, &grp[grp_max_idx], &lineSen[grp_max_idx]);
#ifdef HOR_EDGE_TOUCH_ENABLE
            if (BG_touch_data_edge.x == 0) {
                BG_touch_data_edge.x = centerPoint.x;
                BG_touch_data_edge.y = centerPoint.y;
            }
#endif
            return 0; //not-found
        }
        //if (fine_is_closed_tp_cent(&centerPoint))
        //if (fine_is_closed_tp_min_max2(&minMaxTmp, 0, touchCnt, 0)) //nsmoon@211015
        if (fine_is_closed_tp_min_max(&minMaxTmp, 0, touchCnt))
        {
            IS_DEBUG_FLAG{ TRACE("closed~1~"); };
#if 0 //nsmoon@200924 not-use
            fine_add_used_line(axis2, &grp[grp_max_idx], &lineSen[grp_max_idx]);
#endif
            //FIXME!!! should re-cal size!!!
            return 0; //not-found
        }
#if 0 //nsmoon@220117 for test
        else if (fine_is_xy_ghost_min_max(&minMaxTmp, 0, touchCnt))
        {
            IS_DEBUG_FLAG{ TRACE("closed~1a~"); };
            return 0; //not-found
        }
#endif
#if 1 //nsmoon@200602
        //if (fineLoop == 3 || fineLoop == 4) {
        if (fineLoop == FINE_LOOP_SHADOW_ST || fineLoop == FINE_LOOP_SHADOW_ED) {
            if (fine_is_closed_eraser_min_max(axis2, minMaxTmp, 50.0f)) {
                return 0; //not-found
            }
        }
#endif

#ifdef FRONTEND_LINE_THRESHOLD //nsmoon@191226
        fine_getThresholdCnt(axis2, &minMaxTmp, &grp[grp_max_idx], &lineSen[grp_max_idx], &thCnt); //must be use grp[] with opp axis
#ifdef DUPLICATE_TH10 //0 is for new size rule //nsmoon@200511
        if (axis2 == ENUM_HOR_X) {
            thCnt.th10CntY = thCnt.th10CntX;
            thCnt.th50CntY = thCnt.th50CntX;
            thCnt.th10WidthY = thCnt.th10WidthX; //nsmoon@200602
        }
        else { //(axis2 == ENUM_VER_Y)
            thCnt.th10CntX = thCnt.th10CntY;
            thCnt.th50CntX = thCnt.th50CntY;
            thCnt.th10WidthX = thCnt.th10WidthY; //nsmoon@200602
            }
#endif

#if 1 //(MODEL_SPT == MODEL_CTSO_430_V100) //nsmoon@210304, for test
#define SHADOW_MARKER_MIN   3.0f
#define SHADOW_MARKER_MAX   4.5f
#define FINE_MAX_ERASER_DIST    150.0f
#define FINE_NEAR_ERASER_ADJ_SIZE 1.5f
        IS_DEBUG_FLAG{TRACE_SIZE_ADJ("fine_add_touch_data..isEdgeArea= %x", isEdgeArea);};
        if (isEdgeArea) {
            if (fine_is_closed_eraser_min_max(axis2, minMaxTmp, FINE_MAX_ERASER_DIST)) {
                //float maxSize = GET_MAX(touchWidth.x, touchWidth.y);
                //float widthRatio = (touchWidth.x > touchWidth.y) ? (touchWidth.x / touchWidth.y) : (touchWidth.x / touchWidth.y);
                //TRACE_SIZE_ADJ("..widthRatio= (%d/%d) %0.1f (%0.1f %0.1f)(%0.1f %0.1f)", fineLoop, touchCnt, widthRatio, touchWidth.x, touchWidth.y, thCnt.th10WidthX, thCnt.th10WidthY);
                if (touchWidth.x > SHADOW_MARKER_MIN && touchWidth.x < SHADOW_MARKER_MAX) {
                    touchWidth.x += FINE_NEAR_ERASER_ADJ_SIZE;
                    thCnt.th10WidthX += FINE_NEAR_ERASER_ADJ_SIZE; //nsmoon@210304
                    IS_DEBUG_FLAG{TRACE_SIZE_ADJ(" =>touchWidth.x= %0.1f (%0.1f)", touchWidth.x, thCnt.th10WidthX);};
                }
                if (touchWidth.y > SHADOW_MARKER_MIN && touchWidth.y < SHADOW_MARKER_MAX) {
                    touchWidth.y += FINE_NEAR_ERASER_ADJ_SIZE;
                    thCnt.th10WidthY += FINE_NEAR_ERASER_ADJ_SIZE; //nsmoon@210304
                    IS_DEBUG_FLAG{TRACE_SIZE_ADJ(" =>touchWidth.y= %0.1f (%0.1f)", touchWidth.y, thCnt.th10WidthY);};
                }
            }
        }
#endif
#if 0 //nsmoon@200602
#define SHADOW_MARKER_MIN   3.0f
#define SHADOW_MARKER_MAX   4.0f
#define FINE_MAX_ERASER_DIST    100.0f
#define FINE_NEAR_ERASER_ADJ_SIZE 1.0f
        //if (fineLoop == 3 || fineLoop == 4) {
        if (fineLoop == FINE_LOOP_SHADOW_ST || fineLoop == FINE_LOOP_SHADOW_ED) {
                if (fine_is_closed_eraser_min_max(axis2, minMaxTmp, FINE_MAX_ERASER_DIST)) {
                    //float widthRatio = (touchWidth.x > touchWidth.y) ? (touchWidth.x / touchWidth.y) : (touchWidth.x / touchWidth.y);
                    //TRACE("...widthRatio= (%d) %0.1f (%0.1f %0.1f)(%0.1f %0.1f)", touchCnt, widthRatio, touchWidth.x, touchWidth.y, thCnt.th10WidthX, thCnt.th10WidthY);
                    if (touchWidth.x > SHADOW_MARKER_MIN && touchWidth.x < SHADOW_MARKER_MAX) {
                        touchWidth.x += FINE_NEAR_ERASER_ADJ_SIZE;
                        thCnt.th10WidthX += FINE_NEAR_ERASER_ADJ_SIZE; //nsmoon@210304
                }
                    if (touchWidth.y > SHADOW_MARKER_MIN && touchWidth.y < SHADOW_MARKER_MAX) {
                        touchWidth.y += FINE_NEAR_ERASER_ADJ_SIZE;
                        thCnt.th10WidthY += FINE_NEAR_ERASER_ADJ_SIZE; //nsmoon@210304
                }
            }
        }
#endif

        touchArea = touchWidth.x * touchWidth.y;
        BG_touch_data[touchCnt] = centerPoint;
        BG_touch_area[touchCnt] = touchArea;
        BG_touch_size[touchCnt].xSize = touchWidth.x;
        BG_touch_size[touchCnt].ySize = touchWidth.y;
        BG_multi_fine[touchCnt] = 0; //for debugging
        BG_touch_lineCnt[touchCnt] = thCnt;
        IS_DEBUG_FLAG{ TRACE_FATD("X:th10Cnt,th50Cnt=%d,%d", thCnt.th10CntX, thCnt.th50CntX); };
        IS_DEBUG_FLAG{ TRACE_FATD("Y:th10Cnt,th50Cnt=%d,%d", thCnt.th10CntY, thCnt.th50CntY); };
#endif
#if (DEBUG_fine_add_touch_data > 0) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
        //DEBUG_SHOW_MIN_MAX(&minMaxTmp, MY_COLOR, 1);
        DEBUG_SHOW_POS(&centerPoint, (touchWidth.x*0.5f), (touchWidth.y*0.5f), MY_COLOR);
#endif
        touchCnt++; touchCntFine++;
    }
    else {
        TRACE_ERROR_MEM("ERROR_MEM! fine_add_touch_data..invalid touchCnt,touchCntFine.. %d %d", touchCnt, touchCntFine);
        //return FINE_MEM_ERROR; //nsmoon@210317
    }
#if 0 //nsmoon@200924 not-use
    BS_adj_used_lines(touchCntOrg, touchCnt, ADJUST_USED_LINE_NORMAL); //adjust used line, nsmoon@200331
#endif

    touchCntLocal = touchCntFine - touchCntFineOrg;
    BG_touch_count_fine = touchCntFine;
    BG_touch_count = touchCnt;
    return (touchCntLocal);
}
#else
static int fine_add_touch_data(axis_t axis2, vec_t *centIn, int chkSlope, int fineLoop)
{
    int touchCntOrg = BG_touch_count;
    int touchCntFineOrg = BG_touch_count_fine;
    int touchCnt = touchCntOrg;
    int touchCntFine = touchCntFineOrg;
    int touchCntLocal;
    vec_t centerPoint, touchWidth;
    float halfWidthTmp;
    float touchArea;
    pos_min_max_t minMaxTmp;
    pos_minMax2_t min_max, instPos2;
    vec_t p2, p3;
    axis_t axis1;
    int initial_line_cnt;
    initial_line_t *initial_line;
    uint16_t *remained;
    int remainedCnt;
    initial_line_group_t *grp;
    int grp_cnt, grp_max_idx;
    in_line_t *lineSen;
    vec_t retMinPos, retMaxPos;
    float retWidth;
    int ret;
    IS_DEBUG_FLAG{ 
        TRACE_FATD("fine_add_touch_data...%d %d %d (%0.1f %0.1f)", axis2, chkSlope, fineLoop, centIn->x, centIn->y);
        TRACE_NOP;
    };
#if 1 //nsmoon@201202
    float maxHalfWidth = FINE_MAX_HALF_WIDTH;
    if (fineLoop == FINE_LOOP_EDGE_X) {
        maxHalfWidth = FINE_MAX_HALF_WIDTH * 2.0f;
    }
#endif
    tp_line_cnt_t thCnt;
    thCnt.th10CntX = thCnt.th50CntX = 0;
    thCnt.th10WidthX = 0;
    thCnt.th10CntY = thCnt.th50CntY = 0;
    thCnt.th10WidthY = 0;

    if (axis2 == ENUM_HOR_X) {
        remained = &BS_remained_x[0];
        remainedCnt = BS_remained_x_cnt;
        p2.x = centIn->x - maxHalfWidth;
        p3.x = centIn->x + maxHalfWidth;
        p2.y = centIn->y;
        p3.y = p2.y;
        axis1 = ENUM_VER_Y;
        initial_line = &BS_initial_line_y[0];
    }
    else { //(axis2 == ENUM_VER_Y)
        remained = &BS_remained_y[0];
        remainedCnt = BS_remained_y_cnt;
        p2.y = centIn->y - maxHalfWidth;
        p3.y = centIn->y + maxHalfWidth;
        p2.x = centIn->x;
        p3.x = p2.x;
        axis1 = ENUM_HOR_X;
        initial_line = &BS_initial_line_x[0];
    }
#if (DEBUG_fine_add_touch_data > 0)
    IS_DEBUG_FLAG{
        DEBUG_SHOW_LINE_POS(&p2, &p3, MY_COLOR);
        TRACE_NOP;
    };
#endif

    initial_line_cnt = fine_get_inst3(axis1, p2, p3, axis2, remained, remainedCnt);
    if (initial_line_cnt == FINE_MEM_ERROR) {
        return FINE_MEM_ERROR;
    }
    int sortLen = initial_line[initial_line_cnt].sortLen;
    int isEdgeArea = initial_line[initial_line_cnt].isEdgeArea;
#if 1 //nsmoon@201203
    if (fineLoop == FINE_LOOP_EDGE_X) {
        if ((axis1 == ENUM_VER_Y && IS_INNER_Y(isEdgeArea))
             && sortLen < FINE_INITIAL_GRP_MIN_NUM_EDGE) {
            IS_DEBUG_FLAG{ TRACE_FATD("no inst-4a %04x, %d", isEdgeArea, sortLen);};
            return 0; //no-grp
        }
    }
    else
#endif
    if ((((axis1 == ENUM_HOR_X && IS_NOT_NEAR_Y(isEdgeArea)) ||
          (axis1 == ENUM_VER_Y && IS_NOT_NEAR_X(isEdgeArea)))
         && sortLen < FINE_INITIAL_GRP_MIN_NUM) ||
        (((axis1 == ENUM_HOR_X && IS_NEAR_Y(isEdgeArea)) ||
         (axis1 == ENUM_VER_Y && IS_NEAR_X(isEdgeArea)))
         && sortLen < FINE_INITIAL_GRP_MIN_NUM_EDGE)) { //nsmoon@200408
        IS_DEBUG_FLAG{ TRACE_FATD("no inst-4 %04x, %d", isEdgeArea, sortLen);};
        return 0; //no-grp
    }
    lineSen = initial_line[initial_line_cnt].lineSen;

    ret = fine_make_group3(axis1, initial_line_cnt, axis2);
    if (ret == FINE_MEM_ERROR) {
        return FINE_MEM_ERROR;
    }
    if (ret != FINE_GROUP_FOUND) {
        IS_DEBUG_FLAG{ TRACE_FATD("no group~1~");};
        return 0; //no-grp
    }

    grp = initial_line[initial_line_cnt].grp;
    grp_cnt = initial_line[initial_line_cnt].grpCnt;
    if (grp_cnt <= 0) {
        IS_DEBUG_FLAG{ TRACE_FATD("no group~2~");};
        return 0; //not-found
    }

    grp_max_idx = fine_get_maxlen_grp(grp, grp_cnt);
#if (DEBUG_fine_add_touch_data > 0)
    int minIdx = grp[grp_max_idx].instIdx[0];
    int maxIdx = grp[grp_max_idx].instIdx[grp[grp_max_idx].len - 1];
    DEBUG_SHOW_MIN_MAX_POS(axis2, &initial_line[initial_line_cnt].inst[0], minIdx, maxIdx, 1);
#endif
    fine_make_grp_min_max(axis1, grp, grp_max_idx, initial_line[initial_line_cnt].inst);
    min_max.min = grp[grp_max_idx].instPos.min;
    min_max.max = grp[grp_max_idx].instPos.max;
    retMinPos.x = 0; retMaxPos.x = 0;
    retMinPos.y = 0; retMaxPos.y = 0;

#ifdef FINE_MIN_CAL_SIZE //nsmoon@210305
    float width = GET_ABS(min_max.max - min_max.min);
    if (width > FINE_MIN_CAL_SIZE)
#endif
    {
    ret = fine_cal_min_max3(axis1, initial_line_cnt, grp_max_idx, axis2, &min_max, &retMinPos, &retMaxPos, 1, fineLoop); //1: check slope always, nsmoon@200313
    IS_DEBUG_FLAG{ TRACE_FATD("..min_max=(%d)(%0.1f,%0.1f)(%0.1f,%0.1f)=>(%0.1f,%0.1f) (%0.1f,%0.1f)(%0.1f,%0.1f)", ret, grp[grp_max_idx].instPos.min, grp[grp_max_idx].instPos.max, grp[grp_max_idx].instPos2.min, grp[grp_max_idx].instPos2.max, min_max.min, min_max.max, retMinPos.x, retMaxPos.x, retMinPos.y, retMaxPos.y); };
    if (ret) {
        if (chkSlope) { //nsmoon@200313
            IS_DEBUG_FLAG{ TRACE_FATD("no +/- slope~");};
            return 0; //not-found
        }
    }
    }

    if (touchCntFine  < MAX_TOUCH_LIMIT_FINE && touchCnt < ALLOWABLE_TOUCH_BE) {
        if (axis2 == ENUM_HOR_X) {
            centerPoint.x = (min_max.min + min_max.max) * 0.5f;
            retWidth = GET_ABS(retMaxPos.y - retMinPos.y);
            IS_DEBUG_FLAG{ TRACE_FATD("-retWidth= %0.1f", retWidth);};
            //if (chkSlope && retWidth < MAX_RECAL_MINMAX_WIDTH) { //nsmoon@200313, nsmoon@200403 FIXME!!!
            if (chkSlope && retWidth > MAX_RECAL_MIN_WIDTH && retWidth < MAX_RECAL_MAX_WIDTH) { //nsmoon@200529
                centerPoint.y = (retMinPos.y + retMaxPos.y) * 0.5f;
            }
            else {
                centerPoint.y = (grp[grp_max_idx].instPos2.min + grp[grp_max_idx].instPos2.min) * 0.5f;
            }
            touchWidth.x = GET_ABS(min_max.max - min_max.min);
            touchWidth.y = touchWidth.x;
            halfWidthTmp = touchWidth.y *0.5f;
            instPos2.min = centerPoint.y - halfWidthTmp;
            instPos2.max = centerPoint.y + halfWidthTmp;
            minMaxTmp.minX = min_max.min;
            minMaxTmp.maxX = min_max.max;
            minMaxTmp.minY = instPos2.min;
            minMaxTmp.maxY = instPos2.max;
        }
        else { //(axis2 == ENUM_VER_Y)
            centerPoint.y = (min_max.min + min_max.max) * 0.5f;
            retWidth = GET_ABS(retMaxPos.x - retMinPos.x);
            IS_DEBUG_FLAG{ TRACE_FATD("--retWidth= %0.1f", retWidth);};
            //if (chkSlope && retWidth < MAX_RECAL_MINMAX_WIDTH) { //nsmoon@200313, nsmoon@200403 FIXME!!!
            if (chkSlope && retWidth > MAX_RECAL_MIN_WIDTH && retWidth < MAX_RECAL_MAX_WIDTH) { //nsmoon@200529
                centerPoint.x = (retMinPos.x + retMaxPos.x) * 0.5f;
            }
            else {
                centerPoint.x = (grp[grp_max_idx].instPos2.min + grp[grp_max_idx].instPos2.min) * 0.5f;
            }
            touchWidth.y = GET_ABS(min_max.max - min_max.min);
            touchWidth.x = touchWidth.y;
            halfWidthTmp = touchWidth.x *0.5f;
            instPos2.min = centerPoint.x - halfWidthTmp;
            instPos2.max = centerPoint.x + halfWidthTmp;
            minMaxTmp.minY = min_max.min;
            minMaxTmp.maxY = min_max.max;
            minMaxTmp.minX = instPos2.min;
            minMaxTmp.maxX = instPos2.max;
        }

        if (IS_NOT_ACTIVE_AREA(centerPoint.x, centerPoint.y)) {
            IS_DEBUG_FLAG { TRACE("fine_add_touch_data..out of black area~1~ %0.1f %0.1f", centerPoint.x, centerPoint.y); };
            fine_add_used_line(axis2, &grp[grp_max_idx], &lineSen[grp_max_idx]);
#ifdef HOR_EDGE_TOUCH_ENABLE
            if (BG_touch_data_edge.x == 0) {
                BG_touch_data_edge.x = centerPoint.x;
                BG_touch_data_edge.y = centerPoint.y;
            }
#endif
            return 0; //not-found
        }
        if (fine_is_closed_tp_min_max(&minMaxTmp, 0, touchCnt))
        //if (fine_is_closed_tp_cent(&centerPoint))
        {
            IS_DEBUG_FLAG{ TRACE("closed~1~"); };
#if 0 //nsmoon@200924 not-use
            fine_add_used_line(axis2, &grp[grp_max_idx], &lineSen[grp_max_idx]);
#endif
            //FIXME!!! should re-cal size!!!
            return 0; //not-found
        }
#if 1 //nsmoon@200602
        //if (fineLoop == 3 || fineLoop == 4) {
        if (fineLoop == FINE_LOOP_SHADOW_ST || fineLoop == FINE_LOOP_SHADOW_ED) {
            if (fine_is_closed_eraser_min_max(axis2, minMaxTmp, 50.0f)) {
                return 0; //not-found
            }
        }
#endif

#ifdef FRONTEND_LINE_THRESHOLD //nsmoon@191226
        fine_getThresholdCnt(axis2, &minMaxTmp, &grp[grp_max_idx], &lineSen[grp_max_idx], &thCnt); //must be use grp[] with opp axis
#ifdef DUPLICATE_TH10 //0 is for new size rule //nsmoon@200511
        if (axis2 == ENUM_HOR_X) {
            thCnt.th10CntY = thCnt.th10CntX;
            thCnt.th50CntY = thCnt.th50CntX;
            thCnt.th10WidthY = thCnt.th10WidthX; //nsmoon@200602
        }
        else { //(axis2 == ENUM_VER_Y)
            thCnt.th10CntX = thCnt.th10CntY;
            thCnt.th50CntX = thCnt.th50CntY;
            thCnt.th10WidthX = thCnt.th10WidthY; //nsmoon@200602
            }
#endif

#if 1 //(MODEL_SPT == MODEL_CTSO_430_V100) //nsmoon@210304, for test
#define SHADOW_MARKER_MIN   3.0f
#define SHADOW_MARKER_MAX   4.5f
#define FINE_MAX_ERASER_DIST    150.0f
#define FINE_NEAR_ERASER_ADJ_SIZE 1.5f
        TRACE_SIZE_ADJ("fine_add_touch_data..isEdgeArea= %d", isEdgeArea);
        if (isEdgeArea) {
            if (fine_is_closed_eraser_min_max(axis2, minMaxTmp, FINE_MAX_ERASER_DIST)) {
                //float maxSize = GET_MAX(touchWidth.x, touchWidth.y);
                //float widthRatio = (touchWidth.x > touchWidth.y) ? (touchWidth.x / touchWidth.y) : (touchWidth.x / touchWidth.y);
                //TRACE_SIZE_ADJ("..widthRatio= (%d/%d) %0.1f (%0.1f %0.1f)(%0.1f %0.1f)", fineLoop, touchCnt, widthRatio, touchWidth.x, touchWidth.y, thCnt.th10WidthX, thCnt.th10WidthY);
                if (touchWidth.x > SHADOW_MARKER_MIN && touchWidth.x < SHADOW_MARKER_MAX) {
                    touchWidth.x += FINE_NEAR_ERASER_ADJ_SIZE;
                    thCnt.th10WidthX += FINE_NEAR_ERASER_ADJ_SIZE; //nsmoon@210304
                    TRACE_SIZE_ADJ("=>touchWidth.x= %0.1f (%0.1f)", touchWidth.x, thCnt.th10WidthX);
                }
                if (touchWidth.y > SHADOW_MARKER_MIN && touchWidth.y < SHADOW_MARKER_MAX) {
                    touchWidth.y += FINE_NEAR_ERASER_ADJ_SIZE;
                    thCnt.th10WidthY += FINE_NEAR_ERASER_ADJ_SIZE; //nsmoon@210304
                    TRACE_SIZE_ADJ("=>touchWidth.y= %0.1f (%0.1f)", touchWidth.y, thCnt.th10WidthY);
                }
            }
        }
#endif
#if 0 //nsmoon@200602
#define SHADOW_MARKER_MIN   3.0f
#define SHADOW_MARKER_MAX   4.0f
#define FINE_MAX_ERASER_DIST    100.0f
#define FINE_NEAR_ERASER_ADJ_SIZE 1.0f
        //if (fineLoop == 3 || fineLoop == 4) {
        if (fineLoop == FINE_LOOP_SHADOW_ST || fineLoop == FINE_LOOP_SHADOW_ED) {
                if (fine_is_closed_eraser_min_max(axis2, minMaxTmp, FINE_MAX_ERASER_DIST)) {
                    //float widthRatio = (touchWidth.x > touchWidth.y) ? (touchWidth.x / touchWidth.y) : (touchWidth.x / touchWidth.y);
                    //TRACE("...widthRatio= (%d) %0.1f (%0.1f %0.1f)(%0.1f %0.1f)", touchCnt, widthRatio, touchWidth.x, touchWidth.y, thCnt.th10WidthX, thCnt.th10WidthY);
                    if (touchWidth.x > SHADOW_MARKER_MIN && touchWidth.x < SHADOW_MARKER_MAX) {
                        touchWidth.x += FINE_NEAR_ERASER_ADJ_SIZE;
                        thCnt.th10WidthX += FINE_NEAR_ERASER_ADJ_SIZE; //nsmoon@210304
                }
                    if (touchWidth.y > SHADOW_MARKER_MIN && touchWidth.y < SHADOW_MARKER_MAX) {
                        touchWidth.y += FINE_NEAR_ERASER_ADJ_SIZE;
                        thCnt.th10WidthY += FINE_NEAR_ERASER_ADJ_SIZE; //nsmoon@210304
                }
            }
        }
#endif

        touchArea = touchWidth.x * touchWidth.y;
        BG_touch_data[touchCnt] = centerPoint;
        BG_touch_area[touchCnt] = touchArea;
        BG_touch_size[touchCnt].xSize = touchWidth.x;
        BG_touch_size[touchCnt].ySize = touchWidth.y;
        BG_multi_fine[touchCnt] = 0; //for debugging
        BG_touch_lineCnt[touchCnt] = thCnt;
        IS_DEBUG_FLAG{ TRACE_FATD("X:th10Cnt,th50Cnt=%d,%d", thCnt.th10CntX, thCnt.th50CntX); };
        IS_DEBUG_FLAG{ TRACE_FATD("Y:th10Cnt,th50Cnt=%d,%d", thCnt.th10CntY, thCnt.th50CntY); };
#endif
#if (DEBUG_fine_add_touch_data > 0) //for test
        DEBUG_SHOW_MIN_MAX(&minMaxTmp, MY_COLOR, 1);
        TRACE_NOP;
#endif
        touchCnt++; touchCntFine++;
    }
    else {
        TRACE_ERROR_MEM("ERROR_MEM! fine_add_touch_data..invalid touchCnt,touchCntFine.. %d %d", touchCnt, touchCntFine);
        //return FINE_MEM_ERROR; //nsmoon@210317
    }
#if 0 //nsmoon@200924 not-use
    BS_adj_used_lines(touchCntOrg, touchCnt, ADJUST_USED_LINE_NORMAL); //adjust used line, nsmoon@200331
#endif

    touchCntLocal = touchCntFine - touchCntFineOrg;
    BG_touch_count_fine = touchCntFine;
    BG_touch_count = touchCnt;
    return (touchCntLocal);
}
#endif

#if 0 //nsmoon@211022
static int fine_add_touch_data_shadow(axis_t axis2, int side, vec_t *p2, vec_t *p3, vec_t *tHalfWidth, int tpClosed, int fineLoop)
{
    int touchCntOrg = BG_touch_count;
    int touchCntFineOrg = BG_touch_count_fine;
    int touchCnt = touchCntOrg;
    int touchCntFine = touchCntFineOrg;
    int touchCntLocal;
    vec_t centerPoint, touchWidth;
    float halfWidthTmp;
    float touchArea;
    pos_min_max_t minMaxTmp;
    pos_minMax2_t min_max, instPos2;
    axis_t axis1;
    int initial_line_cnt;
    initial_line_t *initial_line;
    uint16_t *remained;
    int remainedCnt;
    initial_line_group_t *grp;
    int grp_cnt, grp_max_idx;
    in_line_t *lineSen;
    vec_t retMinPos, retMaxPos;
    float retWidth;
    int ret;
    int sortLen, isEdgeArea;
    int k;
    float distMin, distTmp;
    //float grpDensity;

    IS_DEBUG_FLAG{
        TRACE_FATD("fine_add_touch_data_shadow...%d %d %d %d", axis2, side, tpClosed, fineLoop);
        //DEBUG_SHOW_LINE_POS(p2, p3, MY_COLOR);
    TRACE_NOP;
    };

    tp_line_cnt_t thCnt;
    thCnt.th10CntX = thCnt.th50CntX = 0;
    thCnt.th10WidthX = 0;
    thCnt.th10CntY = thCnt.th50CntY = 0;
    thCnt.th10WidthY = 0;

    if (axis2 == ENUM_HOR_X) {
        remained = &BS_remained_x[0];
        remainedCnt = BS_remained_x_cnt;
        axis1 = ENUM_VER_Y;
        initial_line = &BS_initial_line_y[0];
    }
    else { //(axis2 == ENUM_VER_Y)
        remained = &BS_remained_y[0];
        remainedCnt = BS_remained_y_cnt;
        axis1 = ENUM_HOR_X;
        initial_line = &BS_initial_line_x[0];
    }

    initial_line_cnt = fine_get_inst3(axis1, *p2, *p3, axis2, remained, remainedCnt, 0);
    if (initial_line_cnt == FINE_MEM_ERROR) {
        IS_DEBUG_FLAG{ TRACE_FATD("initial_line_cnt..FINE_MEM_ERROR"); };
        return FINE_MEM_ERROR;
    }
    sortLen = initial_line[initial_line_cnt].sortLen;
    isEdgeArea = initial_line[initial_line_cnt].isEdgeArea;
    if ((((axis1 == ENUM_HOR_X && IS_NOT_NEAR_Y(isEdgeArea)) ||
        (axis1 == ENUM_VER_Y && IS_NOT_NEAR_X(isEdgeArea)))
        && sortLen < FINE_INITIAL_GRP_MIN_NUM) ||
        (((axis1 == ENUM_HOR_X && IS_NEAR_Y(isEdgeArea)) ||
        (axis1 == ENUM_VER_Y && IS_NEAR_X(isEdgeArea)))
            && sortLen < FINE_INITIAL_GRP_MIN_NUM_EDGE)) { //nsmoon@200408
        IS_DEBUG_FLAG{ TRACE_FATD("  no inst-4 %04x, %d", isEdgeArea, sortLen); };
        return 0; //no-grp
    }
    lineSen = initial_line[initial_line_cnt].lineSen;

    ret = fine_make_group3(axis1, initial_line_cnt, axis2, fineLoop);
    if (ret == FINE_MEM_ERROR) {
        IS_DEBUG_FLAG{ TRACE_FATD("  ret..FINE_MEM_ERROR"); };
        return FINE_MEM_ERROR;
    }
    if (ret != FINE_GROUP_FOUND) {
        IS_DEBUG_FLAG{ TRACE_FATD("no group~1~"); };
        return 0; //no-grp
    }

    grp = initial_line[initial_line_cnt].grp;
    grp_cnt = initial_line[initial_line_cnt].grpCnt;
    if (grp_cnt < 1)
    {
        IS_DEBUG_FLAG{ TRACE_FATD("  no group~2~"); };
        return 0; //not-found
    }
    else if (tpClosed && grp_cnt > 1) { //nsmoon@211025
        IS_DEBUG_FLAG{ TRACE_FATD("  no group~3~"); };
        return 0; //not-found
    }
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };

#if 1 //nsmoon@211022
    distMin = MIN_INITIAL_VAL;
    grp_max_idx = 0;
    if (grp_cnt > 1) {
    for (k = 0; k < grp_cnt; k++) {
        if (side == FINE_CENT_LFT || side == FINE_CENT_RHT) {
            distTmp = GET_MIN(GET_ABS(grp[grp_max_idx].instPos.min - p2->x), GET_ABS(grp[grp_max_idx].instPos.max - p2->x));
        }
        else {
            //TOP,BOT
            distTmp = GET_MIN(GET_ABS(grp[grp_max_idx].instPos.min - p2->y), GET_ABS(grp[grp_max_idx].instPos.max - p2->y));
        }
        if (distTmp < distMin) {
            distMin = distTmp;
            grp_max_idx = k;
        }
    }
    }
#else
    grp_max_idx = fine_get_maxlen_grp(grp, grp_cnt);
#endif
#if 0 //(DEBUG_fine_add_touch_data > 0)
    int minIdx = grp[grp_max_idx].instIdx[0];
    int maxIdx = grp[grp_max_idx].instIdx[grp[grp_max_idx].len - 1];
    DEBUG_SHOW_MIN_MAX_POS(axis2, &initial_line[initial_line_cnt].inst[0], minIdx, maxIdx, 1);
#endif
    fine_make_grp_min_max(axis1, grp, grp_max_idx, initial_line[initial_line_cnt].inst);
    min_max.min = grp[grp_max_idx].instPos.min;
    min_max.max = grp[grp_max_idx].instPos.max;
    retMinPos.x = 0; retMaxPos.x = 0;
    retMinPos.y = 0; retMaxPos.y = 0;

#ifdef FINE_MIN_CAL_SIZE //nsmoon@210305
    float widthTmp = GET_ABS(min_max.max - min_max.min);
    if (widthTmp > FINE_MIN_CAL_SIZE)
#endif
    {
        ret = fine_cal_min_max3(axis1, initial_line_cnt, grp_max_idx, axis2, &min_max, &retMinPos, &retMaxPos, 1, fineLoop); //1: check slope always, nsmoon@200313 //do-not-use near_eraser
        IS_DEBUG_FLAG{ TRACE_FATD("..min_max=(%d)(%0.1f,%0.1f)(%0.1f,%0.1f)=>(%0.1f,%0.1f) (%0.1f,%0.1f)(%0.1f,%0.1f)", ret, grp[grp_max_idx].instPos.min, grp[grp_max_idx].instPos.max, grp[grp_max_idx].instPos2.min, grp[grp_max_idx].instPos2.max, min_max.min, min_max.max, retMinPos.x, retMaxPos.x, retMinPos.y, retMaxPos.y); };
#if 0 //nsmoon@211018 not-used
        if (ret) {
            if (chkSlope) { //nsmoon@200313
                IS_DEBUG_FLAG{ TRACE_FATD("no +/- slope~"); };
                return 0; //not-found
            }
        }
#endif
    }

    if (touchCntFine  < MAX_TOUCH_LIMIT_FINE && touchCnt < ALLOWABLE_TOUCH_BE) {
        if (axis2 == ENUM_HOR_X) {
            centerPoint.x = (min_max.min + min_max.max) * 0.5f;
            retWidth = GET_ABS(retMaxPos.y - retMinPos.y);
            IS_DEBUG_FLAG{ TRACE_FATD("-retWidth= %0.1f", retWidth); };
            //if (chkSlope && retWidth < MAX_RECAL_MINMAX_WIDTH) { //nsmoon@200313, nsmoon@200403 FIXME!!!
            IS_DEBUG_FLAG { TRACE_FATD("{X}retWidth= %0.1f (%0.1f,%0.1f)", retWidth, retMinPos.y, retMaxPos.y, p2->y-tHalfWidth->y, p2->y+tHalfWidth->y); };
#if 1 //for test //nsmoon@211024
            IS_DEBUG_FLAG { TRACE_FATD(" (%0.1f,%0.1f)", p2->y-tHalfWidth->y, p2->y+tHalfWidth->y); };
#endif
#ifdef FINE_MIN_CAL_SIZE //nsmoon@210305
            if (chkSlope && retWidth > MAX_RECAL_MIN_WIDTH && retWidth < MAX_RECAL_MAX_WIDTH) { //nsmoon@200529
#else
            if (/*chkSlope && */retWidth < MAX_RECAL_MAX_WIDTH)  { //nsmoon@211022
                centerPoint.y = (retMinPos.y + retMaxPos.y) * 0.5f;
#endif
            }
            else {
                centerPoint.y = (grp[grp_max_idx].instPos2.min + grp[grp_max_idx].instPos2.min) * 0.5f;
            }
            touchWidth.x = GET_ABS(min_max.max - min_max.min);
            touchWidth.y = touchWidth.x;
            halfWidthTmp = touchWidth.y *0.5f;
            instPos2.min = centerPoint.y - halfWidthTmp;
            instPos2.max = centerPoint.y + halfWidthTmp;
            minMaxTmp.minX = min_max.min;
            minMaxTmp.maxX = min_max.max;
            minMaxTmp.minY = instPos2.min;
            minMaxTmp.maxY = instPos2.max;
        }
        else { //(axis2 == ENUM_VER_Y)
            centerPoint.y = (min_max.min + min_max.max) * 0.5f;
            retWidth = GET_ABS(retMaxPos.x - retMinPos.x);
            IS_DEBUG_FLAG{ TRACE_FATD("--retWidth= %0.1f", retWidth); };
            //if (chkSlope && retWidth < MAX_RECAL_MINMAX_WIDTH) { //nsmoon@200313, nsmoon@200403 FIXME!!!
            //if (chkSlope && retWidth > MAX_RECAL_MIN_WIDTH && retWidth < MAX_RECAL_MAX_WIDTH) //nsmoon@200529
            IS_DEBUG_FLAG { TRACE_FATD("{Y}retWidth= %0.1f (%0.1f,%0.1f)", retWidth, retMinPos.x, retMaxPos.x); };
#if 1 //for test //nsmoon@211024
            IS_DEBUG_FLAG { TRACE_FATD(" (%0.1f,%0.1f)", p2->x-tHalfWidth->x, p2->x+tHalfWidth->x); };
#endif
            if (/*chkSlope && */retWidth < MAX_RECAL_MAX_WIDTH) //nsmoon@211022
            {
                centerPoint.x = (retMinPos.x + retMaxPos.x) * 0.5f;
            }
            else {
                centerPoint.x = (grp[grp_max_idx].instPos2.min + grp[grp_max_idx].instPos2.min) * 0.5f;
            }
            touchWidth.y = GET_ABS(min_max.max - min_max.min);
            touchWidth.x = touchWidth.y;
            halfWidthTmp = touchWidth.x *0.5f;
            instPos2.min = centerPoint.x - halfWidthTmp;
            instPos2.max = centerPoint.x + halfWidthTmp;
            minMaxTmp.minY = min_max.min;
            minMaxTmp.maxY = min_max.max;
            minMaxTmp.minX = instPos2.min;
            minMaxTmp.maxX = instPos2.max;
        }

        if (IS_NOT_ACTIVE_AREA(centerPoint.x, centerPoint.y)) {
            IS_DEBUG_FLAG{ TRACE("fine_add_touch_data..out of black area~2~ %0.1f %0.1f", centerPoint.x, centerPoint.y); };
            fine_add_used_line(axis2, &grp[grp_max_idx], &lineSen[grp_max_idx]);
#ifdef HOR_EDGE_TOUCH_ENABLE
            if (BG_touch_data_edge.x == 0) {
                BG_touch_data_edge.x = centerPoint.x;
                BG_touch_data_edge.y = centerPoint.y;
            }
#endif
            return 0; //not-found
        }
        //if (fine_is_closed_tp_cent(&centerPoint))
        //if (fine_is_closed_tp_min_max2(&minMaxTmp, 0, touchCnt, 0)) //nsmoon@211015
        if (fine_is_closed_tp_min_max(&minMaxTmp, 0, touchCnt))
        {
            IS_DEBUG_FLAG{ TRACE("closed~1~"); };
#if 0 //nsmoon@200924 not-use
            fine_add_used_line(axis2, &grp[grp_max_idx], &lineSen[grp_max_idx]);
#endif
            //FIXME!!! should re-cal size!!!
            return 0; //not-found
        }
#if 1 //nsmoon@200602
        //if (fineLoop == 3 || fineLoop == 4) {
        if (fineLoop == FINE_LOOP_SHADOW_ST || fineLoop == FINE_LOOP_SHADOW_ED) {
            if (fine_is_closed_eraser_min_max(axis2, minMaxTmp, 20.0f)) { //nsmoon@211022 50.0f 20.0f
                return 0; //not-found
            }
        }
#endif

#ifdef FRONTEND_LINE_THRESHOLD //nsmoon@191226
        fine_getThresholdCnt(axis2, &minMaxTmp, &grp[grp_max_idx], &lineSen[grp_max_idx], &thCnt); //must be use grp[] with opp axis
#ifdef DUPLICATE_TH10 //0 is for new size rule //nsmoon@200511
        if (axis2 == ENUM_HOR_X) {
            thCnt.th10CntY = thCnt.th10CntX;
            thCnt.th50CntY = thCnt.th50CntX;
            thCnt.th10WidthY = thCnt.th10WidthX; //nsmoon@200602
        }
        else { //(axis2 == ENUM_VER_Y)
            thCnt.th10CntX = thCnt.th10CntY;
            thCnt.th50CntX = thCnt.th50CntY;
            thCnt.th10WidthX = thCnt.th10WidthY; //nsmoon@200602
        }
#endif

#if 1 //(MODEL_SPT == MODEL_CTSO_430_V100) //nsmoon@210304, for test
#define SHADOW_MARKER_MIN   3.0f
#define SHADOW_MARKER_MAX   4.5f
#define FINE_MAX_ERASER_DIST    150.0f
#define FINE_NEAR_ERASER_ADJ_SIZE 1.5f
        TRACE_SIZE_ADJ("fine_add_touch_data..isEdgeArea= %x", isEdgeArea);
        if (isEdgeArea) {
            if (fine_is_closed_eraser_min_max(axis2, minMaxTmp, FINE_MAX_ERASER_DIST)) {
                //float maxSize = GET_MAX(touchWidth.x, touchWidth.y);
                //float widthRatio = (touchWidth.x > touchWidth.y) ? (touchWidth.x / touchWidth.y) : (touchWidth.x / touchWidth.y);
                //TRACE_SIZE_ADJ("..widthRatio= (%d/%d) %0.1f (%0.1f %0.1f)(%0.1f %0.1f)", fineLoop, touchCnt, widthRatio, touchWidth.x, touchWidth.y, thCnt.th10WidthX, thCnt.th10WidthY);
                if (touchWidth.x > SHADOW_MARKER_MIN && touchWidth.x < SHADOW_MARKER_MAX) {
                    touchWidth.x += FINE_NEAR_ERASER_ADJ_SIZE;
                    thCnt.th10WidthX += FINE_NEAR_ERASER_ADJ_SIZE; //nsmoon@210304
                    TRACE_SIZE_ADJ("=>touchWidth.x= %0.1f (%0.1f)", touchWidth.x, thCnt.th10WidthX);
                }
                if (touchWidth.y > SHADOW_MARKER_MIN && touchWidth.y < SHADOW_MARKER_MAX) {
                    touchWidth.y += FINE_NEAR_ERASER_ADJ_SIZE;
                    thCnt.th10WidthY += FINE_NEAR_ERASER_ADJ_SIZE; //nsmoon@210304
                    TRACE_SIZE_ADJ("=>touchWidth.y= %0.1f (%0.1f)", touchWidth.y, thCnt.th10WidthY);
                }
            }
        }
#endif
#if 0 //nsmoon@200602
#define SHADOW_MARKER_MIN   3.0f
#define SHADOW_MARKER_MAX   4.0f
#define FINE_MAX_ERASER_DIST    100.0f
#define FINE_NEAR_ERASER_ADJ_SIZE 1.0f
        //if (fineLoop == 3 || fineLoop == 4) {
        if (fineLoop == FINE_LOOP_SHADOW_ST || fineLoop == FINE_LOOP_SHADOW_ED) {
            if (fine_is_closed_eraser_min_max(axis2, minMaxTmp, FINE_MAX_ERASER_DIST)) {
                //float widthRatio = (touchWidth.x > touchWidth.y) ? (touchWidth.x / touchWidth.y) : (touchWidth.x / touchWidth.y);
                //TRACE("...widthRatio= (%d) %0.1f (%0.1f %0.1f)(%0.1f %0.1f)", touchCnt, widthRatio, touchWidth.x, touchWidth.y, thCnt.th10WidthX, thCnt.th10WidthY);
                if (touchWidth.x > SHADOW_MARKER_MIN && touchWidth.x < SHADOW_MARKER_MAX) {
                    touchWidth.x += FINE_NEAR_ERASER_ADJ_SIZE;
                    thCnt.th10WidthX += FINE_NEAR_ERASER_ADJ_SIZE; //nsmoon@210304
                }
                if (touchWidth.y > SHADOW_MARKER_MIN && touchWidth.y < SHADOW_MARKER_MAX) {
                    touchWidth.y += FINE_NEAR_ERASER_ADJ_SIZE;
                    thCnt.th10WidthY += FINE_NEAR_ERASER_ADJ_SIZE; //nsmoon@210304
                }
            }
        }
#endif

        touchArea = touchWidth.x * touchWidth.y;
        BG_touch_data[touchCnt] = centerPoint;
        BG_touch_area[touchCnt] = touchArea;
        BG_touch_size[touchCnt].xSize = touchWidth.x;
        BG_touch_size[touchCnt].ySize = touchWidth.y;
        BG_multi_fine[touchCnt] = 0; //for debugging
        BG_touch_lineCnt[touchCnt] = thCnt;
        IS_DEBUG_FLAG{ TRACE_FATD("X:th10Cnt,th50Cnt=%d,%d", thCnt.th10CntX, thCnt.th50CntX); };
        IS_DEBUG_FLAG{ TRACE_FATD("Y:th10Cnt,th50Cnt=%d,%d", thCnt.th10CntY, thCnt.th50CntY); };
#endif
#if (DEBUG_fine_add_touch_data > 0) //for test
        DEBUG_SHOW_MIN_MAX(&minMaxTmp, MY_COLOR, 1);
        TRACE_NOP;
#endif
        touchCnt++; touchCntFine++;
        }
    else {
        TRACE_ERROR_MEM("ERROR_MEM! fine_add_touch_data..invalid touchCnt,touchCntFine.. %d %d", touchCnt, touchCntFine);
        //return FINE_MEM_ERROR; //nsmoon@210317
    }
#if 0 //nsmoon@200924 not-use
    BS_adj_used_lines(touchCntOrg, touchCnt, ADJUST_USED_LINE_NORMAL); //adjust used line, nsmoon@200331
#endif

    touchCntLocal = touchCntFine - touchCntFineOrg;
    BG_touch_count_fine = touchCntFine;
    BG_touch_count = touchCnt;
    return (touchCntLocal);
}
#endif

#if 1 //nsmoon@211022
static int fine_add_touch_data_shadow2(axis_t axis2, vec_t *p2, vec_t *p3, int fineLoop)
{
    int touchCntOrg = BG_touch_count;
    int touchCntFineOrg = BG_touch_count_fine;
    int touchCnt = touchCntOrg;
    int touchCntFine = touchCntFineOrg;
    int touchCntLocal;
    vec_t centerPoint, touchWidth;
    float halfWidthTmp;
    float touchArea;
    pos_min_max_t minMaxTmp;
    pos_minMax2_t min_max, instPos2;
    axis_t axis1;
    int initial_line_cnt;
    initial_line_t *initial_line;
    uint16_t *remained;
    int remainedCnt;
    initial_line_group_t *grp;
    int grp_cnt, grp_max_idx;
    in_line_t *lineSen;
    vec_t retMinPos, retMaxPos;
    float retWidth;
    int ret;
    int sortLen, isEdgeArea;
    //int k;
    //float distMin, distTmp;
    //float grpDensity;

    IS_DEBUG_FLAG{
        TRACE_FATD("fine_add_touch_data_shadow2...%d %d", axis2, fineLoop);
        //DEBUG_SHOW_LINE_POS(p2, p3, MY_COLOR);
    TRACE_NOP;
    };

    tp_line_cnt_t thCnt;
    thCnt.th10CntX = thCnt.th50CntX = 0;
    thCnt.th10WidthX = 0;
    thCnt.th10CntY = thCnt.th50CntY = 0;
    thCnt.th10WidthY = 0;

    if (axis2 == ENUM_HOR_X) {
        remained = &BS_remained_x[0];
        remainedCnt = BS_remained_x_cnt;
        axis1 = ENUM_VER_Y;
        initial_line = &BS_initial_line_y[0];
    }
    else { //(axis2 == ENUM_VER_Y)
        remained = &BS_remained_y[0];
        remainedCnt = BS_remained_y_cnt;
        axis1 = ENUM_HOR_X;
        initial_line = &BS_initial_line_x[0];
    }

    initial_line_cnt = fine_get_inst3(axis1, *p2, *p3, axis2, remained, remainedCnt, 0);
    if (initial_line_cnt == FINE_MEM_ERROR) {
        IS_DEBUG_FLAG{ TRACE_FATD("initial_line_cnt~~FINE_MEM_ERROR"); };
        return FINE_MEM_ERROR;
    }
    sortLen = initial_line[initial_line_cnt].sortLen;
    isEdgeArea = initial_line[initial_line_cnt].isEdgeArea;
    if ((((axis1 == ENUM_HOR_X && IS_NOT_NEAR_Y(isEdgeArea)) ||
        (axis1 == ENUM_VER_Y && IS_NOT_NEAR_X(isEdgeArea)))
        && sortLen < FINE_INITIAL_GRP_MIN_NUM_SHADOW) ||
        (((axis1 == ENUM_HOR_X && IS_NEAR_Y(isEdgeArea)) ||
        (axis1 == ENUM_VER_Y && IS_NEAR_X(isEdgeArea)))
            && sortLen < FINE_INITIAL_GRP_MIN_NUM_SHADOW)) { //nsmoon@211027
        IS_DEBUG_FLAG{ TRACE_FATD("  no inst~~4 %04x, %d", isEdgeArea, sortLen); };
        return 0; //no-grp
    }
    lineSen = initial_line[initial_line_cnt].lineSen;

    ret = fine_make_group3(axis1, initial_line_cnt, axis2, fineLoop);
    if (ret == FINE_MEM_ERROR) {
        IS_DEBUG_FLAG{ TRACE_FATD("  ret~~FINE_MEM_ERROR"); };
        return FINE_MEM_ERROR;
    }
    if (ret != FINE_GROUP_FOUND) {
        IS_DEBUG_FLAG{ TRACE_FATD("  no group~~3~"); };
        return 0; //no-grp
    }

    grp = initial_line[initial_line_cnt].grp;
    grp_cnt = initial_line[initial_line_cnt].grpCnt;
    if (grp_cnt < 1)
    {
        IS_DEBUG_FLAG{ TRACE_FATD("  no group~~4~"); };
        return 0; //not-found
    }
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };

    grp_max_idx = fine_get_maxlen_grp(grp, grp_cnt);

#if 0 //(DEBUG_fine_add_touch_data > 0)
    int minIdx = grp[grp_max_idx].instIdx[0];
    int maxIdx = grp[grp_max_idx].instIdx[grp[grp_max_idx].len - 1];
    DEBUG_SHOW_MIN_MAX_POS(axis2, &initial_line[initial_line_cnt].inst[0], minIdx, maxIdx, 1);
#endif
    fine_make_grp_min_max(axis1, grp, grp_max_idx, initial_line[initial_line_cnt].inst);
    min_max.min = grp[grp_max_idx].instPos.min;
    min_max.max = grp[grp_max_idx].instPos.max;
    retMinPos.x = 0; retMaxPos.x = 0;
    retMinPos.y = 0; retMaxPos.y = 0;

#ifdef FINE_MIN_CAL_SIZE //nsmoon@210305
    float widthTmp = GET_ABS(min_max.max - min_max.min);
    if (widthTmp > FINE_MIN_CAL_SIZE)
#endif
    {
        ret = fine_cal_min_max3(axis1, initial_line_cnt, grp_max_idx, axis2, &min_max, &retMinPos, &retMaxPos, 1, fineLoop); //1: check slope always, nsmoon@200313 //do-not-use near_eraser
        IS_DEBUG_FLAG{ TRACE_FATD("..min_max~~(%d)(%0.1f,%0.1f)(%0.1f,%0.1f)=>(%0.1f,%0.1f) (%0.1f,%0.1f)(%0.1f,%0.1f)", ret, grp[grp_max_idx].instPos.min, grp[grp_max_idx].instPos.max, grp[grp_max_idx].instPos2.min, grp[grp_max_idx].instPos2.max, min_max.min, min_max.max, retMinPos.x, retMaxPos.x, retMinPos.y, retMaxPos.y); };
        if (ret) {
#if 0 //nsmoon@211018 not-used
            if (chkSlope) { //nsmoon@200313
                IS_DEBUG_FLAG{ TRACE_FATD("no +/- slope~"); };
                return 0; //not-found
            }
#endif
        }
    }

    if (touchCntFine  < MAX_TOUCH_LIMIT_FINE && touchCnt < ALLOWABLE_TOUCH_BE) {
        if (axis2 == ENUM_HOR_X) {
            centerPoint.x = (min_max.min + min_max.max) * 0.5f;
            retWidth = GET_ABS(retMaxPos.y - retMinPos.y);
            //IS_DEBUG_FLAG{ TRACE_FATD("-retWidth~~ %0.1f", retWidth); };
            //if (chkSlope && retWidth < MAX_RECAL_MINMAX_WIDTH) { //nsmoon@200313, nsmoon@200403 FIXME!!!
            IS_DEBUG_FLAG { TRACE_FATD("{X}retWidth~~ %0.1f (%0.1f,%0.1f)", retWidth, retMinPos.y, retMaxPos.y); };
#ifdef FINE_MIN_CAL_SIZE //nsmoon@210305
            if (chkSlope && retWidth > MAX_RECAL_MIN_WIDTH && retWidth < MAX_RECAL_MAX_WIDTH) { //nsmoon@200529
#else
            if (/*chkSlope && */retWidth > 0 && retWidth < MAX_RECAL_MAX_WIDTH)  { //nsmoon@211022
                centerPoint.y = (retMinPos.y + retMaxPos.y) * 0.5f;
#endif
            }
            else {
                centerPoint.y = (grp[grp_max_idx].instPos2.min + grp[grp_max_idx].instPos2.min) * 0.5f;
            }
            touchWidth.x = GET_ABS(min_max.max - min_max.min);
            touchWidth.y = touchWidth.x;
            halfWidthTmp = touchWidth.y *0.5f;
            instPos2.min = centerPoint.y - halfWidthTmp;
            instPos2.max = centerPoint.y + halfWidthTmp;
            minMaxTmp.minX = min_max.min;
            minMaxTmp.maxX = min_max.max;
            minMaxTmp.minY = instPos2.min;
            minMaxTmp.maxY = instPos2.max;
        }
        else { //(axis2 == ENUM_VER_Y)
            centerPoint.y = (min_max.min + min_max.max) * 0.5f;
            retWidth = GET_ABS(retMaxPos.x - retMinPos.x);
            //IS_DEBUG_FLAG{ TRACE_FATD("--retWidth~~ %0.1f", retWidth); };
            //if (chkSlope && retWidth < MAX_RECAL_MINMAX_WIDTH) { //nsmoon@200313, nsmoon@200403 FIXME!!!
            //if (chkSlope && retWidth > MAX_RECAL_MIN_WIDTH && retWidth < MAX_RECAL_MAX_WIDTH) //nsmoon@200529
            IS_DEBUG_FLAG { TRACE_FATD("{Y}retWidth~~ %0.1f (%0.1f,%0.1f)", retWidth, retMinPos.x, retMaxPos.x); };
            if (/*chkSlope && */retWidth > 0 && retWidth < MAX_RECAL_MAX_WIDTH) //nsmoon@211022
            {
                centerPoint.x = (retMinPos.x + retMaxPos.x) * 0.5f;
            }
            else {
                centerPoint.x = (grp[grp_max_idx].instPos2.min + grp[grp_max_idx].instPos2.min) * 0.5f;
            }
            touchWidth.y = GET_ABS(min_max.max - min_max.min);
            touchWidth.x = touchWidth.y;
            halfWidthTmp = touchWidth.x *0.5f;
            instPos2.min = centerPoint.x - halfWidthTmp;
            instPos2.max = centerPoint.x + halfWidthTmp;
            minMaxTmp.minY = min_max.min;
            minMaxTmp.maxY = min_max.max;
            minMaxTmp.minX = instPos2.min;
            minMaxTmp.maxX = instPos2.max;
        }
#if (DEBUG_fine_add_touch_data > 0) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
            DEBUG_SHOW_POS(&centerPoint, (touchWidth.x*0.5f), (touchWidth.y*0.5f), MY_COLOR);
#endif
        if (IS_NOT_ACTIVE_AREA(centerPoint.x, centerPoint.y)) {
            IS_DEBUG_FLAG{ TRACE("fine_add_touch_data..out of black area~~3~ %0.1f %0.1f", centerPoint.x, centerPoint.y); };
            fine_add_used_line(axis2, &grp[grp_max_idx], &lineSen[grp_max_idx]);
#ifdef HOR_EDGE_TOUCH_ENABLE
            if (BG_touch_data_edge.x == 0) {
                BG_touch_data_edge.x = centerPoint.x;
                BG_touch_data_edge.y = centerPoint.y;
            }
#endif
            return 0; //not-found
        }
        //if (fine_is_closed_tp_cent(&centerPoint))
        //if (fine_is_closed_tp_min_max2(&minMaxTmp, 0, touchCnt, 0)) //nsmoon@211015
        if (fine_is_closed_tp_min_max(&minMaxTmp, 0, touchCnt))
        {
            IS_DEBUG_FLAG{ TRACE("closed~~1~"); };
#if 0 //nsmoon@200924 not-use
            fine_add_used_line(axis2, &grp[grp_max_idx], &lineSen[grp_max_idx]);
#endif
            //FIXME!!! should re-cal size!!!
            return 0; //not-found
        }
        if (fine_is_closed_tp_shadow(axis2, &minMaxTmp, 0, touchCnt)) //nsmoon@211102
        {
            IS_DEBUG_FLAG{ TRACE("closed~~1a~"); };
            return 0; //not-found
        }
#if 1 //nsmoon@200602
        //if (fineLoop == 3 || fineLoop == 4) {
        if (fineLoop >= FINE_LOOP_SHADOW_ST && fineLoop <= FINE_LOOP_SHADOW_ED) {
            if (fine_is_closed_eraser_min_max(axis2, minMaxTmp, 20.0f)) { //nsmoon@211022 50.0f=>20.0f
                return 0; //not-found
            }
        }
#endif

#ifdef FRONTEND_LINE_THRESHOLD //nsmoon@191226
        fine_getThresholdCnt(axis2, &minMaxTmp, &grp[grp_max_idx], &lineSen[grp_max_idx], &thCnt); //must be use grp[] with opp axis
#ifdef DUPLICATE_TH10 //0 is for new size rule //nsmoon@200511
        if (axis2 == ENUM_HOR_X) {
            thCnt.th10CntY = thCnt.th10CntX;
            thCnt.th50CntY = thCnt.th50CntX;
            thCnt.th10WidthY = thCnt.th10WidthX; //nsmoon@200602
        }
        else { //(axis2 == ENUM_VER_Y)
            thCnt.th10CntX = thCnt.th10CntY;
            thCnt.th50CntX = thCnt.th50CntY;
            thCnt.th10WidthX = thCnt.th10WidthY; //nsmoon@200602
        }
#endif

#if 1 //(MODEL_SPT == MODEL_CTSO_430_V100) //nsmoon@210304, for test
#define SHADOW_MARKER_MIN   3.0f
#define SHADOW_MARKER_MAX   4.5f
#define FINE_MAX_ERASER_DIST    150.0f
#define FINE_NEAR_ERASER_ADJ_SIZE 1.5f
        TRACE_SIZE_ADJ("fine_add_touch_data_shadow2..isEdgeArea= %x", isEdgeArea);
        if (isEdgeArea) {
            if (fine_is_closed_eraser_min_max(axis2, minMaxTmp, FINE_MAX_ERASER_DIST)) {
                //float maxSize = GET_MAX(touchWidth.x, touchWidth.y);
                //float widthRatio = (touchWidth.x > touchWidth.y) ? (touchWidth.x / touchWidth.y) : (touchWidth.x / touchWidth.y);
                //TRACE_SIZE_ADJ("..widthRatio= (%d/%d) %0.1f (%0.1f %0.1f)(%0.1f %0.1f)", fineLoop, touchCnt, widthRatio, touchWidth.x, touchWidth.y, thCnt.th10WidthX, thCnt.th10WidthY);
                if (touchWidth.x > SHADOW_MARKER_MIN && touchWidth.x < SHADOW_MARKER_MAX) {
                    touchWidth.x += FINE_NEAR_ERASER_ADJ_SIZE;
                    thCnt.th10WidthX += FINE_NEAR_ERASER_ADJ_SIZE; //nsmoon@210304
                    TRACE_SIZE_ADJ("=>touchWidth.x~~ %0.1f (%0.1f)", touchWidth.x, thCnt.th10WidthX);
                }
                if (touchWidth.y > SHADOW_MARKER_MIN && touchWidth.y < SHADOW_MARKER_MAX) {
                    touchWidth.y += FINE_NEAR_ERASER_ADJ_SIZE;
                    thCnt.th10WidthY += FINE_NEAR_ERASER_ADJ_SIZE; //nsmoon@210304
                    TRACE_SIZE_ADJ("=>touchWidth.y~~ %0.1f (%0.1f)", touchWidth.y, thCnt.th10WidthY);
                }
            }
        }
#endif
#if 0 //nsmoon@200602
#define SHADOW_MARKER_MIN   3.0f
#define SHADOW_MARKER_MAX   4.0f
#define FINE_MAX_ERASER_DIST    100.0f
#define FINE_NEAR_ERASER_ADJ_SIZE 1.0f
        //if (fineLoop == 3 || fineLoop == 4) {
        if (fineLoop == FINE_LOOP_SHADOW_ST || fineLoop == FINE_LOOP_SHADOW_ED) {
            if (fine_is_closed_eraser_min_max(axis2, minMaxTmp, FINE_MAX_ERASER_DIST)) {
                //float widthRatio = (touchWidth.x > touchWidth.y) ? (touchWidth.x / touchWidth.y) : (touchWidth.x / touchWidth.y);
                //TRACE("...widthRatio= (%d) %0.1f (%0.1f %0.1f)(%0.1f %0.1f)", touchCnt, widthRatio, touchWidth.x, touchWidth.y, thCnt.th10WidthX, thCnt.th10WidthY);
                if (touchWidth.x > SHADOW_MARKER_MIN && touchWidth.x < SHADOW_MARKER_MAX) {
                    touchWidth.x += FINE_NEAR_ERASER_ADJ_SIZE;
                    thCnt.th10WidthX += FINE_NEAR_ERASER_ADJ_SIZE; //nsmoon@210304
                }
                if (touchWidth.y > SHADOW_MARKER_MIN && touchWidth.y < SHADOW_MARKER_MAX) {
                    touchWidth.y += FINE_NEAR_ERASER_ADJ_SIZE;
                    thCnt.th10WidthY += FINE_NEAR_ERASER_ADJ_SIZE; //nsmoon@210304
                }
            }
        }
#endif

        touchArea = touchWidth.x * touchWidth.y;
        BG_touch_data[touchCnt] = centerPoint;
        BG_touch_area[touchCnt] = touchArea;
        BG_touch_size[touchCnt].xSize = touchWidth.x;
        BG_touch_size[touchCnt].ySize = touchWidth.y;
        BG_multi_fine[touchCnt] = 0; //for debugging
        BG_touch_lineCnt[touchCnt] = thCnt;
        IS_DEBUG_FLAG{ TRACE_FATD("X:th10Cnt,th50Cnt~~%d,%d", thCnt.th10CntX, thCnt.th50CntX); };
        IS_DEBUG_FLAG{ TRACE_FATD("Y:th10Cnt,th50Cnt~~%d,%d", thCnt.th10CntY, thCnt.th50CntY); };
#endif
#if (DEBUG_fine_add_touch_data > 0) //for test
        DEBUG_SHOW_MIN_MAX(&minMaxTmp, MY_COLOR, 1);
        TRACE_NOP;
#endif
        touchCnt++; touchCntFine++;
        }
    else {
        TRACE_ERROR_MEM("ERROR_MEM! fine_add_touch_data..invalid touchCnt,touchCntFine~~ %d %d", touchCnt, touchCntFine);
        //return FINE_MEM_ERROR; //nsmoon@210317
    }
#if 0 //nsmoon@200924 not-use
    BS_adj_used_lines(touchCntOrg, touchCnt, ADJUST_USED_LINE_NORMAL); //adjust used line, nsmoon@200331
#endif

    touchCntLocal = touchCntFine - touchCntFineOrg;
    BG_touch_count_fine = touchCntFine;
    BG_touch_count = touchCnt;
    return (touchCntLocal);
}
#endif

#ifdef FINE_REMAINED_INIT_LINE_MAX //nsmoon@211015
#define DEBUG_fine_get_center_cross_lines_remained 1
#if (DEBUG_fine_get_center_cross_lines_remained > 0)
#define TRACE_FGCCLR(...)   //TRACE(__VA_ARGS__)
#else
#define TRACE_FGCCLR(...)
#endif
static int fine_get_center_cross_lines_remained(vec_t *cent, int centLenMax, int *centCntIn)
{
    int slopeVal;
    int slopeValMin, slopeValMinIdx;
    int tmpInBufIdx;
    int pd, led;
    vec_t p0, p1, p2, p3, pR;
    uint16_t *remained;
    int remainedCnt;
    int i, k;
    axis_t saveAxis;
    initial_line_a_t *initial_line;
    int initial_line_a_cnt = 0;
    axis_t saveAxisOpp;
    IS_DEBUG_FLAG {
        TRACE_NOP;
    };

    int centCnt = 0;
    (*centCntIn) = 0; //init

#if 0 //nsmoon@211025
    int remLineIdx, axisIdx;
    int slopePlusMax = 0, slopeMinusMax = 0;
    int slopePlusMin = MIN_INITIAL_VAL, slopeMinusMin = MIN_INITIAL_VAL;
    int slopeDiff, slopeDiffX, slopeDiffY;

    for (axisIdx = (int)ENUM_HOR_X; axisIdx < (int)ENUM_AXIS_END; axisIdx++)
    {
        if (axisIdx == (int)ENUM_HOR_X) {
            remained = &BS_remained_x[0];
            remainedCnt = BS_remained_x_cnt;
        }
        else {
            remained = &BS_remained_y[0];
            remainedCnt = BS_remained_y_cnt;
        }

        for (remLineIdx = 0; remLineIdx < remainedCnt; remLineIdx++)
        {
            tmpInBufIdx = remained[remLineIdx];
            if (BS_getSenInBuf((axis_t)axisIdx, tmpInBufIdx, &pd, &led) < 0) {
                TRACE_ERROR("ERROR! DEBUG_show_line_idx_..BS_getSenInBuf() [%d]", BG_frame_no);
                continue;
            }
            slopeVal = led - pd;
            if (slopeVal > 0) {
                if (slopeVal > slopePlusMax) {
                    slopePlusMax = slopeVal;
                }
                if (slopeVal < slopePlusMin) {
                    slopePlusMin = slopeVal;
                }
            }
            else if (slopeVal < 0) {
                if ((-slopeVal) > slopeMinusMax) {
                    slopeMinusMax = (-slopeVal);
                }
                if ((-slopeVal) < slopeMinusMin) {
                    slopeMinusMin = (-slopeVal);
                }
            }
        }
        if (slopePlusMax > 0 && slopeMinusMax > 0) {
            slopeDiff = slopePlusMax + slopeMinusMax;
        }
        else if (slopePlusMax == 0 && slopeMinusMax > 0) {
            slopeDiff = slopeMinusMax - slopeMinusMin;
        }
        else if (slopePlusMax > 0 && slopeMinusMax == 0) {
            slopeDiff = slopePlusMax - slopePlusMin;
        }
        else {
            slopeDiff = 0;
        }
        if (axisIdx == (int)ENUM_HOR_X) {
            slopeDiffX = slopeDiff;
        }
        else {
            slopeDiffY = slopeDiff;
        }
    }
#endif

    //#define REMAINED_LINE_MARGIN  20
    //if ((BS_remained_y_cnt > (REMAINED_LINE_MARGIN + MIN_NUM_IN_LINES)) && //nsmoon@211021 FIXME!!
    //    ((BS_remained_x_cnt + REMAINED_LINE_MARGIN) > BS_remained_y_cnt)) 
    if (BS_remained_x_cnt > BS_remained_y_cnt) //nsmoon@211022
    //if (slopeDiffX > slopeDiffY) //nsmoon@211025
    {
        saveAxis = ENUM_HOR_X;
        remained = &BS_remained_x[0];
        remainedCnt = BS_remained_x_cnt;
        initial_line = &BS_initial_line_a_x[0];
    }
    else {
        saveAxis = ENUM_VER_Y;
        remained = &BS_remained_y[0];
        remainedCnt = BS_remained_y_cnt;
        initial_line = &BS_initial_line_a_y[0];
    }

    int ret = BS_fine_get_initial_ep5(saveAxis, remained, &remainedCnt, FINE_INIT_EP5_MIN_SLOPE);
    if (ret) {
        TRACE_ERROR("ERROR! fine_get_center_cross_lines_remained..0 mem over flow");
        return -1; //mem error
    }
    initial_line_a_cnt = (saveAxis == ENUM_HOR_X) ? BS_initial_line_a_x_cnt : BS_initial_line_a_y_cnt;
    if (initial_line_a_cnt <= 0) {
        TRACE_ERROR("ERROR! fine_get_center_cross_linesBS_getLinePosInBufSen(axis_t axis, int led, int pd, vec_t *p0, vec_t *p1)_remained..no initial_line_a_cnt");
        return 0;
    }
    IS_DEBUG_FLAG {
        TRACE_FGCCLR("saveAxis=%d %d %d", saveAxis, remainedCnt, initial_line_a_cnt);
    }
    //TRACE("&cent[0]=%x %x", &cent[0], &cent[1]);

    p0.x = 0; p0.y = 0; p1.x = 0; p1.y = 0;
    p2.x = 0; p2.y = 0; p3.x = 0; p3.y = 0;

    if (saveAxis == ENUM_HOR_X) {
        saveAxisOpp = ENUM_VER_Y;
        remained = &BS_remained_y[0];
        remainedCnt = BS_remained_y_cnt;
    }
    else {
        saveAxisOpp = ENUM_HOR_X;
        remained = &BS_remained_x[0];
        remainedCnt = BS_remained_x_cnt;
    }

    for (k = 0; k < initial_line_a_cnt; k++) {
        pd = initial_line[k].line.pd;
        led = initial_line[k].line.led;
        IS_DEBUG_FLAG {
            TRACE_FGCCLR("  (%d)(%d,%d)", k, pd, led);
        }
        if (BS_getLinePosInBufSen(saveAxis, led, pd, &p0, &p1)) {
            continue; //error
        }

        //fined min slope line
        slopeValMin = MIN_INITIAL_VAL;
        slopeValMinIdx = -1;
        for (i = 0; i < remainedCnt; i++) {
            tmpInBufIdx = remained[i];
            if (BS_getSenInBuf(saveAxisOpp, tmpInBufIdx, &pd, &led)) {
                continue;
            }
            slopeVal = GET_ABS(led - pd);
            if (slopeVal < slopeValMin) {
                slopeValMinIdx = i;
                slopeValMin = slopeVal;
            }
        }
        if (slopeValMinIdx < 0) {
             continue; //error //not-found
        }
        tmpInBufIdx = remained[slopeValMinIdx];
        if (BS_getLinePosInBuf(saveAxisOpp, tmpInBufIdx, &p2, &p3)) {
            continue; //error
        }
#if (DEBUG_fine_get_center_cross_lines_remained > 0)
        IS_DEBUG_FLAG{
            DEBUG_SHOW_LINE_POS(&p0, &p1, MY_COLOR);
            DEBUG_SHOW_LINE_POS(&p2, &p3, MY_COLOR);
            TRACE_NOP;
        };
#endif

        //calculate center
        if (BS_line_intersection(&p0, &p1, &p2, &p3, &pR)) {
            if (centCnt < centLenMax) {
                cent[centCnt] = pR;
                centCnt++;
                IS_DEBUG_FLAG {
                    TRACE_FGCCLR("  (%0.1f,%0.1f) ", pR.x, pR.y);
                }
            }
            else {
                TRACE_ERROR("ERROR! fine_get_center_cross_lines_remained..invalid centCnt %d", centCnt);
                break; //continue; //nsmoon@211201
            }
        }
        else {
            TRACE_FGCCLR("  (not-found)");
            continue;
        }
    } //for (k = 0; k < initial_line_a_cnt; k++)

#if 1 //nsmoon@211019
    //remove unnecessary cent
#define FINE_CENT_UNNESSARY_MARGIN  50.0f
    if (centCnt > FINE_REMAINED_INIT_LINE_MAX) {
        for (k = 0; k < centCnt; k++) {
            IS_DEBUG_FLAG { TRACE_FGCCLR("  center(%d-%d) %0.1f,%0.1f", saveAxis, k, cent[k].x, cent[k].y);};
            if ((saveAxis == ENUM_HOR_X &&
                    (cent[k].x < (BS_aarea_zero_x+FINE_CENT_UNNESSARY_MARGIN) ||
                    cent[k].x > (BS_aarea_end_x-FINE_CENT_UNNESSARY_MARGIN))) ||
                (saveAxis == ENUM_VER_Y &&
                    (cent[k].y < (BS_aarea_zero_y+FINE_CENT_UNNESSARY_MARGIN) ||
                    cent[k].y > (BS_aarea_end_y-FINE_CENT_UNNESSARY_MARGIN))) ) {
                for (i = k; i < (centCnt-1); i++) {
                    cent[i] = cent[i+1];
                }
                centCnt--; k--;
                IS_DEBUG_FLAG { TRACE_FGCCLR("  (*)centCnt=%d", centCnt);};
            }
            if (centCnt == FINE_REMAINED_INIT_LINE_MAX) {
                break;
            }
        }
    }
#endif

    if (centCnt > 0) {
        (*centCntIn) = centCnt;
        return saveAxis;
    }
    return ENUM_AXIS_END;
}
#else
//#defined RETRY_CNT_FGCCLR_ENABLE
#ifdef RETRY_CNT_FGCCLR_ENABLE
#define RETRY_CNT_FGCCLR    3
#else
#define RETRY_CNT_FGCCLR    1
#endif
static int fine_get_center_cross_lines_remained(vec_t *cent)
{
    int axis;
    int slopeVal;
    int slopeValMin, slopeValMinIdx;
    int tmpInBufIdx;
    int pd, led;
    vec_t p0, p1, p2, p3, pR;
    uint16_t *remained;
    int remainedCnt;
    int i; //k;
    int saveAxis = (BS_remained_x_cnt > BS_remained_y_cnt) ? (int)ENUM_HOR_X : (int)ENUM_VER_Y;
#ifdef RETRY_CNT_FGCCLR_ENABLE
    uint8_t minIdxSav[RETRY_CNT_FGCCLR];
    uint8_t minIdxSavCnt = 0;
    int m;
#endif
    IS_DEBUG_FLAG {
        TRACE_NOP;
    };

    p0.x = 0; p0.y = 0; p1.x = 0; p1.y = 0;
    p2.x = 0; p2.y = 0; p3.x = 0; p3.y = 0;
#if (RETRY_CNT_FGCCLR > 1)
    int k;
    for (k = 0; k < RETRY_CNT_FGCCLR; k++)
#endif
    {
        //retry for avoiding reflection
        for (axis = 0; axis < (int)ENUM_AXIS_END; axis++) {
            if (axis == (int)ENUM_HOR_X) {
                remained = &BS_remained_x[0];
                remainedCnt = BS_remained_x_cnt;
            }
            else { //(axis == (int)ENUM_VER_Y)
                remained = &BS_remained_y[0];
                remainedCnt = BS_remained_y_cnt;
            }

            //fined min slope line
            slopeValMin = MIN_INITIAL_VAL;
            slopeValMinIdx = -1;
            for (i = 0; i < remainedCnt; i++) {
#ifdef RETRY_CNT_FGCCLR_ENABLE
                //check skip slopeValMinIdx
                if (saveAxis == axis) {
                    for (m = 0; m < minIdxSavCnt; m++) {
                        if (minIdxSav[m] == i) break;
                    }
                    if (minIdxSavCnt > 0 && m < minIdxSavCnt) continue;
                }
#endif
                tmpInBufIdx = remained[i];
            if (BS_getSenInBuf((axis_t)axis, tmpInBufIdx, &pd, &led)) {
                    continue;
                }
                slopeVal = GET_ABS(led - pd);
                if (slopeVal < slopeValMin) {
                    slopeValMinIdx = i;
                    slopeValMin = slopeVal;
                }
            }
            if (slopeValMinIdx < 0) {
#if (RETRY_CNT_FGCCLR > 1)
                break; //not found
#else
                return ENUM_AXIS_END; //not-found
#endif
            }
#ifdef RETRY_CNT_FGCCLR_ENABLE
            //save slopeValMinIdx
            if (saveAxis == axis) {
                minIdxSav[minIdxSavCnt++] = slopeValMinIdx;
            }
#endif

            tmpInBufIdx = remained[slopeValMinIdx];
            if (axis == (int)ENUM_HOR_X) {
                if (BS_getLinePosInBuf((axis_t)axis, tmpInBufIdx, &p0, &p1)) {
#if (RETRY_CNT_FGCCLR > 1)
                    break; //error
#else
                    return ENUM_AXIS_END; //error
#endif
                }
            }
            else { //(axis == (int)ENUM_VER_Y)
                if (BS_getLinePosInBuf((axis_t)axis, tmpInBufIdx, &p2, &p3)) {
#if (RETRY_CNT_FGCCLR > 1)
                    break; //error
#else
                    return ENUM_AXIS_END; //error
#endif
                }
            }
        } //for (axis = 0; axis < (int)ENUM_AXIS_END; axis++)
        if (slopeValMinIdx < 0) {
#if (RETRY_CNT_FGCCLR > 1)
            break; //not found
#else
            return ENUM_AXIS_END; //not found
#endif
        }
#if 0 //ndef MULTI_SKIP_USED_VB //for test //nsmoon@210218
        IS_DEBUG_FLAG{
            DEBUG_SHOW_LINE_POS(&p0, &p1, 0);
            DEBUG_SHOW_LINE_POS(&p2, &p3, 0);
            TRACE_NOP;
        };
#endif
        if (BS_line_intersection(&p0, &p1, &p2, &p3, &pR)) {
            *cent = pR;
#ifdef RETRY_CNT_FGCCLR_ENABLE
            if (fine_is_closed_tp_cent(&pR)) {
                IS_DEBUG_FLAG{ TRACE("closed~2~"); };
                continue;
            }
#endif
            return saveAxis; //found
        }
        else {
#if (RETRY_CNT_FGCCLR > 1)
            break; //no inst
#else
            return ENUM_AXIS_END; //no inst
#endif
        }
    } //for (k = 0; k < RETRY_CNT_FGCCLR; k++)

#if (RETRY_CNT_FGCCLR > 1)
    return ENUM_AXIS_END; //not-found
#endif
}
#endif

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_BS_fine_add_remained_touch    1
#endif
#ifdef FINE_REMAINED_INIT_LINE_MAX //nsmoon@211017
int BS_fine_add_remained_touch_xy(int fineLoop)
{
    int ret, centCnt = 0;
    int touchCntOrg = BG_touch_count;
#define CENT_TMP_LEN (FINE_REMAINED_INIT_LINE_MAX+3)
    vec_t centTmp[CENT_TMP_LEN];
    int i;
    for (i = 0; i < CENT_TMP_LEN; i++) {
        centTmp[i].x = centTmp[i].y = 0;
    }
    //TRACE("&cent[0]=%x %x", &centTmp[0], &centTmp[1]);
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };

    //reset mem
    fine_bs_mem_init();

    //fine_get_initial_tp(fineLoop); //for test
    ret = fine_get_center_cross_lines_remained(&centTmp[0], CENT_TMP_LEN, &centCnt);
    if (ret == ENUM_AXIS_END || centCnt < 1) {
        return 0; //not found
    }

    //reset mem
    fine_bs_mem_init();

    ret = fine_add_touch_data((axis_t)ret, &centTmp[0], centCnt, 1, fineLoop); //1: check slope //nsmoon@211024 0=>1
#if 1 //nsmoon@200924
    if (ret > 0) { //nsmoon@220118
    int touchCnt = BG_touch_count;
    int adjust_mode = (centCnt > 1) ? ADJUST_USED_LINE_MULTI : ADJUST_USED_LINE_NORMAL;  //nsmoon@211018
    BS_adj_used_lines_tp(touchCntOrg, touchCnt, adjust_mode); //adjust used line, nsmoon@200331 //BS_fine_add_remained_touch_xy
    }
#endif
    return (ret);
}
#else
int BS_fine_add_remained_touch_xy(int fineLoop)
{
    vec_t centTmp;
    int ret;
    int touchCntOrg = BG_touch_count;
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };

    //reset mem
    fine_bs_mem_init();

    //fine_get_initial_tp(fineLoop); //for test
    ret = fine_get_center_cross_lines_remained(&centTmp);
    if (ret == ENUM_AXIS_END) {
        return 0; //not found
    }

    ret = fine_add_touch_data((axis_t)ret, &centTmp, 0, fineLoop); //0: not check slope
#if 1 //nsmoon@200924
    int touchCnt = BG_touch_count;
    BS_adj_used_lines(touchCntOrg, touchCnt, ADJUST_USED_LINE_NORMAL); //adjust used line, nsmoon@200331
#endif
    return (ret);
}
#endif //1
#endif //1

#ifdef FINE_OLD_METHOD_2PEN //old method for 2-pen
static int fine_cal_coordinates3(int fineLoop)
{
    int touchCntOrg = BG_touch_count;
    int touchCntFineOrg = BG_touch_count_fine;
    int touchCnt = touchCntOrg;
    int touchCntFine = touchCntFineOrg;
    int touchCntLocal;
    vec_t centerPoint, touchWidth;
    float touchArea;
    pos_min_max_t minMaxTmp;
    pos_minMax2_t min_max;
    vec_t p2, p3;
    axis_t axis1;
    int axis2, axis_start, axis_end, axis_inc;
    initial_line_a_t *initial_line_a;
    int initial_line_a_cnt;
    int initial_line_a_cnt_max, initial_line_a_y_first;
    initial_line_t *initial_line;
    int initial_line_cnt;
    uint16_t *remained;
    int remainedCnt;
    initial_line_group_t *grp = 0, *grpOp = 0;
    int grp_cnt, grp_max_idx = 0, grp_max_idx_op = 0;
    in_line_t *lineSen = 0, *lineSenOp = 0;
    vec_t retMinPos, retMaxPos;
    int pd, led;
    float *pdXpos = &BS_pd_pos_x[0];
    float *ledXpos = &BS_led_pos_x[0];
    float *pdYpos = &BS_pd_pos_y[0];
    float *ledYpos = &BS_led_pos_y[0];
    int skip;
    int ret;
    int nearErase = 0;
    IS_DEBUG_FLAG{
        //TRACE_FATD("fine_cal_coordinates3...");
        TRACE_NOP;
    };

    retMinPos.x = 0; retMaxPos.x = 0;
    retMinPos.y = 0; retMaxPos.y = 0;
    min_max.min = 0; min_max.max = 0;
    initial_line_cnt = 0;
    initial_line_a_y_first = 0;

    tp_line_cnt_t thCnt;
    thCnt.th10CntX = thCnt.th50CntX = 0;
    thCnt.th10WidthX = 0;
    thCnt.th10CntY = thCnt.th50CntY = 0;
    thCnt.th10WidthY = 0;

#if 1 //nsmoon@230613
    if (fine_get_initial_cxp() == 0) {
        return 0; //no-touch-point
    }
#endif

    if (BS_initial_line_a_y_cnt > BS_initial_line_a_x_cnt)
    {
        initial_line_a_y_first++;
        initial_line_a_cnt_max = BS_initial_line_a_y_cnt;
            axis_start = (int)ENUM_HOR_X;
            axis_end = (int)ENUM_VER_Y;
            axis_inc = 1;
        }
        else {
        initial_line_a_cnt_max = BS_initial_line_a_x_cnt;
            axis_start = (int)ENUM_VER_Y;
            axis_end = (int)ENUM_HOR_X;
            axis_inc = -1;
    }
    TRACE_FATD("fine_cal_coordinates3...initial_line_a_y_first= %d (%d)", initial_line_a_y_first, axis_start);
    //TRACE_RELEASE("fine_cal_coordinates3...initial_line_a_y_first= %d", initial_line_a_y_first);

    for (initial_line_a_cnt = 0; initial_line_a_cnt < initial_line_a_cnt_max; initial_line_a_cnt++)
    {
        skip = 0;
        for (axis2 = axis_start; (axis2 * axis_inc) <= (axis_end * axis_inc); axis2 += axis_inc)
        {
            if (axis2 == (int)ENUM_VER_Y) {
                remained = &BS_remained_y[0];
                remainedCnt = BS_remained_y_cnt;
                }
                else {
                remained = &BS_remained_x[0];
                remainedCnt = BS_remained_x_cnt;
            }

            if (axis2 == axis_start) {
                axis1 = (axis_t)axis_end;
                if (axis1 == (int)ENUM_HOR_X) {
                    initial_line = &BS_initial_line_x[0];
                    initial_line_a = &BS_initial_line_a_x[0];
                    pd = initial_line_a[initial_line_a_cnt].line.pd;
                    led = initial_line_a[initial_line_a_cnt].line.led;
                    p2.x = ledXpos[led];
                    p2.y = BS_sensor_zero_y;
                    p3.x = pdXpos[pd];
                    p3.y = BS_sensor_end_y;
                }
                else {
                initial_line = &BS_initial_line_y[0];
                    initial_line_a = &BS_initial_line_a_y[0];
                    pd = initial_line_a[initial_line_a_cnt].line.pd;
                    led = initial_line_a[initial_line_a_cnt].line.led;
                    p2.y = ledYpos[led];
                    p2.x = BS_sensor_end_x;
                    p3.y = pdYpos[pd];
                    p3.x = BS_sensor_zero_x;
                }
            }
            else { //(axis2 == axis_end)
                //save opp axis info
                    grpOp = grp;
                    grp_max_idx_op = grp_max_idx;
                    lineSenOp = lineSen;
                axis1 = (axis_t)axis_start;
                if (axis1 == ENUM_HOR_X) {
                    initial_line = &BS_initial_line_x[0];
                    p2.y = BS_initial_line_y[initial_line_cnt].cent.y - FINE_MAX_HALF_WIDTH;
                    p3.y = BS_initial_line_y[initial_line_cnt].cent.y + FINE_MAX_HALF_WIDTH;
                    p2.x = BS_initial_line_y[initial_line_cnt].cent.x;
                    p3.x = p2.x;
                }
                else {
                    initial_line = &BS_initial_line_y[0];
                    p2.x = BS_initial_line_x[initial_line_cnt].cent.x - FINE_MAX_HALF_WIDTH;
                    p3.x = BS_initial_line_x[initial_line_cnt].cent.x + FINE_MAX_HALF_WIDTH;
                    p2.y = BS_initial_line_x[initial_line_cnt].cent.y;
                    p3.y = p2.y;
                }
            }
    #if (DEBUG_fine_add_touch_data > 0)
            IS_DEBUG_FLAG{
                DEBUG_SHOW_LINE_POS(&p2, &p3, MY_COLOR);
                TRACE_NOP;
            };
    #endif
            initial_line_cnt = fine_get_inst3(axis1, p2, p3, (axis_t)axis2, remained, remainedCnt, 0);
            if (initial_line_cnt == FINE_MEM_ERROR) {
                return FINE_MEM_ERROR;
            }
            int sortLen = initial_line[initial_line_cnt].sortLen;
            int isEdgeArea = initial_line[initial_line_cnt].isEdgeArea;
            if ((((axis1 == ENUM_HOR_X && IS_NOT_NEAR_Y(isEdgeArea)) ||
                  (axis1 == ENUM_VER_Y && IS_NOT_NEAR_X(isEdgeArea)))
                 && sortLen < FINE_INITIAL_GRP_MIN_NUM) ||
                (((axis1 == ENUM_HOR_X && IS_NEAR_Y(isEdgeArea)) ||
                  (axis1 == ENUM_VER_Y && IS_NEAR_X(isEdgeArea)))
                   && sortLen < FINE_INITIAL_GRP_MIN_NUM_EDGE)) {
                IS_DEBUG_FLAG{ TRACE_FATD("no inst-5 %04x %d", isEdgeArea, sortLen);};
                skip++;
                break; //no-inst
            }
            lineSen = initial_line[initial_line_cnt].lineSen;

            ret = fine_make_group3(axis1, initial_line_cnt, (axis_t)axis2, fineLoop);
            if (ret == FINE_MEM_ERROR) {
                return FINE_MEM_ERROR;
            }
            if (ret != FINE_GROUP_FOUND) {
                IS_DEBUG_FLAG{ TRACE_FATD("  no group~5~");};
                skip++;
                break; //no-grp
            }

            grp = initial_line[initial_line_cnt].grp;
            grp_cnt = initial_line[initial_line_cnt].grpCnt;
            if (grp_cnt <= 0) {
                IS_DEBUG_FLAG{ TRACE_FATD("no group~6~");};
                skip++;
                break; //not-found
            }

            grp_max_idx = fine_get_maxlen_grp(grp, grp_cnt);
            //grp_max_idx = fine_get_max_density_grp(axis1, grp, grp_cnt, initial_line[initial_line_cnt].inst);
        #if (DEBUG_fine_add_touch_data > 0) && defined(DRAW_POLYGON_TEST)
            IS_DEBUG_FLAG{
            int minIdx = grp[grp_max_idx].instIdx[0];
            int maxIdx = grp[grp_max_idx].instIdx[grp[grp_max_idx].len - 1];
            DEBUG_SHOW_MIN_MAX_POS((axis_t)axis2, &initial_line[initial_line_cnt].inst[0], minIdx, maxIdx, 1);
            TRACE_NOP;
            };
        #endif
            fine_make_grp_min_max(axis1, grp, grp_max_idx, initial_line[initial_line_cnt].inst);
            min_max.min = grp[grp_max_idx].instPos.min;
            min_max.max = grp[grp_max_idx].instPos.max;
            if (axis2 == axis_start) {
                if (axis2 == (int)ENUM_VER_Y) {
                initial_line[initial_line_cnt].cent.y = (grp[grp_max_idx].instPos.min + grp[grp_max_idx].instPos.max) * 0.5f;
                initial_line[initial_line_cnt].cent.x = (grp[grp_max_idx].instPos2.min + grp[grp_max_idx].instPos2.max) * 0.5f;
            }
                else {
                    initial_line[initial_line_cnt].cent.x = (grp[grp_max_idx].instPos.min + grp[grp_max_idx].instPos.max) * 0.5f;
                    initial_line[initial_line_cnt].cent.y = (grp[grp_max_idx].instPos2.min + grp[grp_max_idx].instPos2.max) * 0.5f;
                }
            }
            vec_t centPos;
            if (initial_line_a_y_first) {
                centPos.y = (grp[grp_max_idx].instPos.min + grp[grp_max_idx].instPos.max) * 0.5f;
                centPos.x = (grp[grp_max_idx].instPos2.min + grp[grp_max_idx].instPos2.max) * 0.5f;
            }
            else {
                centPos.x = (grp[grp_max_idx].instPos.min + grp[grp_max_idx].instPos.max) * 0.5f;
                centPos.y = (grp[grp_max_idx].instPos2.min + grp[grp_max_idx].instPos2.max) * 0.5f;
            }

            retMinPos.x = 0; retMaxPos.x = 0;
            retMinPos.y = 0; retMaxPos.y = 0;

            nearErase = fine_is_near_eraser(centPos);

#ifdef FINE_MIN_CAL_SIZE //nsmoon@210305
            float width = GET_ABS(min_max.max - min_max.min);
            if (width > FINE_MIN_CAL_SIZE)
#endif
            {
                ret = fine_cal_min_max3(axis1, initial_line_cnt, grp_max_idx, (axis_t)axis2, &min_max, &retMinPos, &retMaxPos, 1, fineLoop); //1: check slope always, nsmoon@200313
                IS_DEBUG_FLAG{ TRACE_FATD(">>min_max=(%d)%d(%0.1f,%0.1f)=>(%0.1f,%0.1f) (%0.1f,%0.1f)(%0.1f,%0.1f)", axis1, ret, grp[grp_max_idx].instPos.min, grp[grp_max_idx].instPos.max, min_max.min, min_max.max, retMinPos.x, retMaxPos.x, retMinPos.y, retMaxPos.y); };
            //float retWidthX = GET_ABS(retMaxPos.x - retMinPos.x);
            //float retWidthY = GET_ABS(retMaxPos.y - retMinPos.y);
            //TRACE(".retWidthX,retWidthY= %0.1f %0.1f", retWidthX, retWidthY);
            if (ret) {
                IS_DEBUG_FLAG{ TRACE_FATD("no +/- slope~~");};
                //return 0; //not-found
            }
            else {
                grp[grp_max_idx].instPos.min = min_max.min;
                grp[grp_max_idx].instPos.max = min_max.max;
            }
            }
        } //for (axis2 = (int)ENUM_VER_Y; axis2 >= (int)ENUM_HOR_X; axis2--)
        if (skip) continue;

        if (initial_line_a_y_first) {
            minMaxTmp.minY = grp[grp_max_idx].instPos.min;
            minMaxTmp.maxY = grp[grp_max_idx].instPos.max;
            minMaxTmp.minX = grpOp[grp_max_idx_op].instPos.min;
            minMaxTmp.maxX = grpOp[grp_max_idx_op].instPos.max;
        }
        else {
            minMaxTmp.minX = grp[grp_max_idx].instPos.min;
            minMaxTmp.maxX = grp[grp_max_idx].instPos.max;
            minMaxTmp.minY = grpOp[grp_max_idx_op].instPos.min;
            minMaxTmp.maxY = grpOp[grp_max_idx_op].instPos.max;
        }
    #if (DEBUG_fine_add_touch_data > 0) //for test
        IS_DEBUG_FLAG {
        DEBUG_SHOW_MIN_MAX(&minMaxTmp, MY_COLOR, 1);
        TRACE_NOP;
        };
    #endif

        if (touchCntFine  < MAX_TOUCH_LIMIT_FINE && touchCnt < ALLOWABLE_TOUCH_BE) {
            centerPoint.x = (minMaxTmp.minX + minMaxTmp.maxX) * 0.5f;
            centerPoint.y = (minMaxTmp.minY + minMaxTmp.maxY) * 0.5f;
            touchWidth.x = GET_ABS(minMaxTmp.maxX - minMaxTmp.minX);
            touchWidth.y = GET_ABS(minMaxTmp.maxY - minMaxTmp.minY);
            if (IS_NOT_ACTIVE_AREA(centerPoint.x, centerPoint.y)) {
                IS_DEBUG_FLAG { TRACE("fine_cal_coordinates3..out of black area: %0.1f %0.1f", centerPoint.x, centerPoint.y); };
                if (initial_line_a_y_first) {
                    fine_add_used_line(ENUM_VER_Y, &grp[grp_max_idx], &lineSen[grp_max_idx]);
                    fine_add_used_line(ENUM_HOR_X, &grpOp[grp_max_idx_op], &lineSenOp[grp_max_idx_op]);
                }
                else {
                    fine_add_used_line(ENUM_HOR_X, &grp[grp_max_idx], &lineSen[grp_max_idx]);
                    fine_add_used_line(ENUM_VER_Y, &grpOp[grp_max_idx_op], &lineSenOp[grp_max_idx_op]);
                }
#ifdef HOR_EDGE_TOUCH_ENABLE
                if (BG_touch_data_edge.x == 0) {
                    BG_touch_data_edge.x = centerPoint.x;
                    BG_touch_data_edge.y = centerPoint.y;
                }
#endif
                break; //not-found
                //return 0; //not-found
            }

#if 0 //def FINE_REMAINED_INIT_LINE_MAX
            if (fine_is_closed_tp_min_max2(&minMaxTmp, 0, touchCnt, 0/*touchCntOrg*/)) //nsmoon@211018
#else
            if (fine_is_closed_tp_min_max(&minMaxTmp, 0, touchCnt))
            //if (fine_is_closed_tp_cent(&centerPoint))
#endif
            {
                IS_DEBUG_FLAG{ TRACE("closed~3~"); };
#if 0 //nsmoon@200924 not-use
                if (initial_line_a_y_first) {
                    fine_add_used_line(ENUM_VER_Y, &grp[grp_max_idx], &lineSen[grp_max_idx]);
                    fine_add_used_line(ENUM_HOR_X, &grpOp[grp_max_idx_op], &lineSenOp[grp_max_idx_op]);
                }
                else {
                    fine_add_used_line(ENUM_HOR_X, &grp[grp_max_idx], &lineSen[grp_max_idx]);
                    fine_add_used_line(ENUM_VER_Y, &grpOp[grp_max_idx_op], &lineSenOp[grp_max_idx_op]);
                }
#endif
                //FIXME!!! should re-cal size!!!
                break; //not-found
                //return 0; //not-found
            }
#if (DEBUG_BG_clipping_fine2 > 0) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
            DEBUG_SHOW_POS(&centerPoint, (touchWidth.x*0.5f), (touchWidth.y*0.5f), MY_COLOR);
#endif
    #ifdef FRONTEND_LINE_THRESHOLD //nsmoon@191226
            if (initial_line_a_y_first) {
                fine_getThresholdCnt(ENUM_VER_Y, &minMaxTmp, &grp[grp_max_idx], &lineSen[grp_max_idx], &thCnt); //must be use grp[] with opp axis
                fine_getThresholdCnt(ENUM_HOR_X, &minMaxTmp, &grpOp[grp_max_idx_op], &lineSenOp[grp_max_idx_op], &thCnt); //must be use grp[] with opp axis
            }
            else {
                fine_getThresholdCnt(ENUM_HOR_X, &minMaxTmp, &grp[grp_max_idx], &lineSen[grp_max_idx], &thCnt); //must be use grp[] with opp axis
                fine_getThresholdCnt(ENUM_VER_Y, &minMaxTmp, &grpOp[grp_max_idx_op], &lineSenOp[grp_max_idx_op], &thCnt); //must be use grp[] with opp axis
            }
    #endif

#if 1 //nsmoon@210309
#if 0 //for test
#define FINE_PEN_WIDTH_MIN         3.0f
#define FINE_PEN_WIDTH_MAX         4.2f
#define FINE_MARKER_WIDTH_MIN      4.0f
#define FINE_MARKER_WIDTH_MAX      8.0f
#define FINE_PEN_SIZE_RATIO        1.1f
#define FINE_PEN_ADJ_SIZE_RATIO    0.9f
            int edgeX = BS_is_edge(ENUM_HOR_X, centerPoint.x);
            int edgeY = BS_is_edge(ENUM_VER_Y, centerPoint.y);
            int isEdgeArea = edgeX | edgeY;
            float minSize = GET_MIN(touchWidth.x, touchWidth.y);
            float maxSize = GET_MAX(touchWidth.x, touchWidth.y);
#endif //0
            //TRACE_SIZE_ADJ("fine_cal_coordinates3..nearErase=(%d)(0x%x)%0.2f,%0.2f(%0.1f,%0.1f)", nearErase, isEdgeAreaAll, touchWidth.x, touchWidth.y, thCnt.th10WidthX, thCnt.th10WidthY);
            TRACE_SIZE_ADJ("fine_cal_coordinates3..nearErase=(%d)%0.2f,%0.2f(%0.1f,%0.1f)", nearErase, touchWidth.x, touchWidth.y, thCnt.th10WidthX, thCnt.th10WidthY);
            if (nearErase) {
#define FINE_MARKER_SIZE_RATIO     1.5f
#define FINE_MARKER_ADJ_SIZE_RATIO 0.9f
                float widthRatio = (touchWidth.x > touchWidth.y) ? (touchWidth.x / touchWidth.y) : (touchWidth.y / touchWidth.x);
                TRACE_SIZE_ADJ("..widthRatio= %0.2f/%0.1f", widthRatio, FINE_MARKER_SIZE_RATIO);
                if (widthRatio > FINE_MARKER_SIZE_RATIO) {
                    //might be marker
                    if (touchWidth.x > touchWidth.y) {
                        touchWidth.y = touchWidth.x * FINE_MARKER_ADJ_SIZE_RATIO;
                        thCnt.th10WidthY = thCnt.th10WidthX * FINE_MARKER_ADJ_SIZE_RATIO;
                    }
                    else {
                        touchWidth.x = touchWidth.y * FINE_MARKER_ADJ_SIZE_RATIO;
                        thCnt.th10WidthX = thCnt.th10WidthY * FINE_MARKER_ADJ_SIZE_RATIO;
                    }
                    TRACE_SIZE_ADJ("=>adjust size: %0.1f,%0.1f %0.1f,%0.1f", touchWidth.x, touchWidth.y, thCnt.th10WidthX, thCnt.th10WidthY);
                }
            }
#if 0 //nsmoon@210310, check asymatric size for marker at edge
            else if (isEdgeAreaAll) {
#define FINE_ASYM_SIZE_RATIO       1.2f
#define FINE_ASYM_ADJ_SIZE_RATIO   0.9f
                float widthRatio = (touchWidth.x > touchWidth.y) ? (touchWidth.x / touchWidth.y) : (touchWidth.y / touchWidth.x);
                TRACE("..maxSize,widthRatio= %0.2f %0.2f/%0.1f", maxSize, widthRatio, FINE_ASYM_SIZE_RATIO);
                if (maxSize > FINE_MARKER_WIDTH_MIN && widthRatio > FINE_ASYM_SIZE_RATIO) {
                    //asymatric size
                    if (touchWidth.x < touchWidth.y) {
                        touchWidth.x = touchWidth.y * FINE_ASYM_ADJ_SIZE_RATIO;
                        thCnt.th10WidthX = thCnt.th10WidthY * FINE_ASYM_ADJ_SIZE_RATIO;
                    }
                    else {
                        touchWidth.y = touchWidth.x * FINE_ASYM_ADJ_SIZE_RATIO;
                        thCnt.th10WidthY = thCnt.th10WidthX * FINE_ASYM_ADJ_SIZE_RATIO;
                    }
                    TRACE("=>adjust asym size: %0.1f,%0.1f %0.1f,%0.1f", touchWidth.x, touchWidth.y, thCnt.th10WidthX, thCnt.th10WidthY);
                }
            }
#endif //0
#endif
#if 0 //nsmoon@200602
#define FINE_PEN_WIDTH_MIN      2.0f
#define FINE_PEN_WIDTH_MAX      3.0f
#define FINE_PEN_ADJ_SIZE_RATIO 1.2f
#define FINE_MARKER_WIDTH_MIN      4.0f
#define FINE_MARKER_ADJ_SIZE_RATIO 0.90f
            float widthRatio = (touchWidth.x > touchWidth.y) ? (touchWidth.x / touchWidth.y) : (touchWidth.y / touchWidth.x);
            float minSize = GET_MIN(touchWidth.x, touchWidth.y);
            float maxSize = GET_MAX(touchWidth.x, touchWidth.y);
            //TRACE("widthRatio= %0.1f(%0.1f,%0.1f)", widthRatio, touchWidth.x, touchWidth.y);
            //if (BS_touch_info_multi[i].multiArea < MULTI_MARKER_SIZE_MAX)
            if (minSize > FINE_PEN_WIDTH_MIN && minSize < FINE_PEN_WIDTH_MAX)
            {
                //might be pen
                if (widthRatio > FINE_PEN_ADJ_SIZE_RATIO) {
                    if (touchWidth.x > touchWidth.y) {
                        touchWidth.x = touchWidth.y * FINE_PEN_ADJ_SIZE_RATIO;
                        thCnt.th10WidthX = thCnt.th10WidthY * FINE_PEN_ADJ_SIZE_RATIO;
                    }
                    else {
                        touchWidth.y = touchWidth.x * FINE_PEN_ADJ_SIZE_RATIO;
                        thCnt.th10WidthY = thCnt.th10WidthX * FINE_PEN_ADJ_SIZE_RATIO;
                    }
                }
                //BG_touch_lineCnt[touchCnt] = thCnt;
            }
            else if (nearErase && minSize < FINE_PEN_WIDTH_MIN && maxSize > FINE_MARKER_WIDTH_MIN) {
                //might be marker
                if (touchWidth.x < touchWidth.y) {
                    touchWidth.x = touchWidth.y * FINE_MARKER_ADJ_SIZE_RATIO;
                    thCnt.th10WidthX = thCnt.th10WidthY * FINE_MARKER_ADJ_SIZE_RATIO;
                }
                else {
                    touchWidth.y = touchWidth.x * FINE_MARKER_ADJ_SIZE_RATIO;
                    thCnt.th10WidthY = thCnt.th10WidthX * FINE_MARKER_ADJ_SIZE_RATIO;
                }
                //BG_touch_lineCnt[touchCnt] = thCnt;
            }
#endif
            touchArea = touchWidth.x * touchWidth.y;
            BG_touch_data[touchCnt] = centerPoint;
            BG_touch_area[touchCnt] = touchArea;
            BG_touch_size[touchCnt].xSize = touchWidth.x;
            BG_touch_size[touchCnt].ySize = touchWidth.y;
            BG_multi_fine[touchCnt] = 0; //for debugging
            BG_touch_lineCnt[touchCnt] = thCnt;
            IS_DEBUG_FLAG{ TRACE_FATD("X:th10Cnt,th50Cnt=%d,%d", thCnt.th10CntX, thCnt.th50CntX); };
            IS_DEBUG_FLAG{ TRACE_FATD("Y:th10Cnt,th50Cnt=%d,%d", thCnt.th10CntY, thCnt.th50CntY); };
            touchCnt++; touchCntFine++;
        }
        else {
            TRACE_ERROR_MEM("ERROR_MEM! fine_cal_coordinates3..invalid touchCnt,touchCntFine.. %d %d", touchCnt, touchCntFine);
            //return FINE_MEM_ERROR; //nsmoon@210317
        }
    } //for (initial_line_a_cnt = 0; initial_line_a_cnt < initial_line_a_cnt_max; initial_line_a_cnt++)
#if 0 //nsmoon@200924 not-use
    BS_adj_used_lines(touchCntOrg, touchCnt, ADJUST_USED_LINE_NORMAL); //adjust used line, nsmoon@200331
#endif

    touchCntLocal = touchCntFine - touchCntFineOrg;
    BG_touch_count_fine = touchCntFine;
    BG_touch_count = touchCnt;
    IS_DEBUG_FLAG{
    TRACE_NOP;
    };
    return (touchCntLocal);
}
#endif

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_fine_get_center_cross_lines_remainedsort    1
#endif
#if (DEBUG_fine_get_center_cross_lines_remainedsort > 0)
#define TRACE_FGCCLRS(...)    TRACE(__VA_ARGS__)
#else
#define TRACE_FGCCLRS(...)
#endif

ATTR_BACKEND_RAMFUNC
int BS_fine_get_crossed_lines(axis_t axis, int side, vec_t *p0, vec_t *p1, vec_t *halfWidth)
{
    int remLineIdx;
    int pd0, led0;
    int tmpInBufIdx;
    vec_t p2, p3, pR;
    int remainedSortCnt = 0;
    uint16_t *remainedSort = (uint16_t *)&BS_minmax_inst_stat[0];
    uint16_t *remainedSortTmp = (uint16_t *)&BS_minmax_inst_stat[MAX_MINMAX_INST_TBL/2];
    int remLineCnt;
    uint16_t *remLine;
#if 1 //nsmoon@211020
    uint8_t *sort = &BS_inst_xy_sort[0];
    vec_t *inst = &BS_inst_xy[0];
#endif

    IS_DEBUG_FLAG{
        //TRACE("BS_fine_get_crossed_lines...");
        TRACE_NOP;
    };

    if (axis == ENUM_HOR_X) {
        remLineCnt = BS_remained_x_cnt;
        remLine = &BS_remained_x[0];
    }
    else {
        remLineCnt = BS_remained_y_cnt;
        remLine = &BS_remained_y[0];
    }

    for (remLineIdx = 0; remLineIdx < remLineCnt; remLineIdx++)
    {
        tmpInBufIdx = remLine[remLineIdx];
        if (BS_getSenInBuf(axis, tmpInBufIdx, &pd0, &led0)) {
            //TRACE_ERROR("ERROR! BS_fine_get_crossed_lines..invalid inBufIdx: %d", tmpInBufIdx);
            continue;
        }
        if (axis == ENUM_HOR_X) {
            p2.x = BS_led_pos_x[led0];
            p3.x = BS_pd_pos_x[pd0];
            p2.y = BS_sensor_zero_y;
            p3.y = BS_sensor_end_y;
        }
        else {
            p2.y = BS_led_pos_y[led0];
            p3.y = BS_pd_pos_y[pd0];
            p2.x = BS_sensor_end_x;
            p3.x = BS_sensor_zero_x;
        }
        if (BS_is_used_line_buffer(axis, pd0, led0) == 0) { //nsmoon@200401-bugfix
            continue; //used
        }
        //if (fine_is_cross_line_tp(axis, tpStart, tpEnd, &p2, &p3)) {
        //    //set used line
        //    BS_set_unset2_used_led_pd(axis, led0, pd0, 1); //1:set
        //    continue;
        //}
        if (BS_line_intersection(p0, p1, &p2, &p3, &pR)) {
#if (DEBUG_get_shadow_line > 0)
            IS_DEBUG_FLAG{
            DEBUG_SHOW_POS(&pR, 0.5f, 0.5f, MY_COLOR);
            };
#endif
#if 1 //nsmoon@211021
            if (axis == ENUM_HOR_X) {
                if (pR.y > (p0->y + halfWidth->y + EPSILON) || pR.y < (p0->y - halfWidth->y - EPSILON)) {
                    continue;
                }
            }
            else {
                if (pR.x > (p0->x + halfWidth->x + EPSILON) || pR.x < (p0->x - halfWidth->x - EPSILON)) {
                    continue;
                }
            }
#endif
            if (remainedSortCnt < (MAX_MINMAX_INST_TBL/2) && remainedSortCnt < MAX_NUM_INST_FINE) {
                inst[remainedSortCnt] = pR; //nsmoon@211020
                remainedSortTmp[remainedSortCnt++] = (uint16_t)remLineIdx;
            }
            else {
                TRACE_ERROR("ERROR! BS_fine_get_crossed_lines...invalid remainedSortCnt %d", remainedSortCnt);
                break;
            }
        }
    } //for (remLineIdx = 0; remLineIdx < remLineCnt; remLineIdx++)


    ///////////////////////////////////////
    //make sort table
    int i, j, instIdx_i, instIdx_j;
    float inst_i, inst_j;
    int sortCnt = remainedSortCnt;

    for (i = 0; i < sortCnt; i++) {
        sort[i] = (uint8_t)i;
    }

#if 0 //(DEBUG_fine_get_inst > 1) //for testing
    IS_DEBUG_FLAG
    {
        for (i = 0; i < sortCnt; i++) {
            TRACENR("%d ", sort[i]);
            if ((i + 1) % 20 == 0) TRACENR("\r\n");
        }
        TRACE("=>before(%d)", axis);
        for (i = 0; i < sortCnt; i++) {
            inst_i = (axis == ENUM_HOR_X) ? inst[sort[i]].x : inst[sort[i]].y;
            TRACENR("%0.1f ", inst_i);
            if ((i + 1) % 20 == 0) TRACENR("\r\n");
        }
        TRACE("=>before(%d)", axis);
    };
#endif

    for (i = 0; i < sortCnt - 1; i++)
    {
        for (j = (i + 1); j < sortCnt; j++)
        {
            instIdx_i = sort[i];
            instIdx_j = sort[j];
            inst_i = (axis == ENUM_HOR_X) ? inst[instIdx_i].x : inst[instIdx_i].y;
            inst_j = (axis == ENUM_HOR_X) ? inst[instIdx_j].x : inst[instIdx_j].y;
            if (inst_i > inst_j)
            {
                sort[i] = (uint8_t)instIdx_j;
                sort[j] = (uint8_t)instIdx_i;
            }
        }
    }

#if 0 //(DEBUG_fine_get_inst > 1) //for testing
    IS_DEBUG_FLAG
    {
        for (i = 0; i < sortCnt; i++) {
            TRACENR("%d ", sort[i]);
            if ((i + 1) % 20 == 0) TRACENR("\r\n");
        }
        TRACE("=>after idx(%d, %d)", axis, sortCnt);
        for (i = 0; i < sortCnt; i++) {
            inst_i = (axis == ENUM_HOR_X) ? inst[sort[i]].x : inst[sort[i]].y;
            TRACENR("%0.1f ", inst_i);
            if ((i + 1) % 20 == 0) TRACENR("\r\n");
        }
        TRACE("=>after pos(%d, %d)", axis, sortCnt);
    };
#endif

    int sortCntSt, sortCntEd, inc;
    //float xLinePos = (axis == ENUM_HOR_X) ? p0->x : p0->x;
    float currPos, nextPos, posDist;
    if (side == FINE_CENT_BOT || side == FINE_CENT_RHT) {
        inc = -1;
        sortCntSt = sortCnt - 1;
        sortCntEd = 0;
    }
    else {
        inc = 1;
        sortCntSt = 0;
        sortCntEd = sortCnt - 1;
    }
#define FINE_SHADOW_GRP_DIST_X    10.0f //15.0f
#define FINE_SHADOW_GRP_DIST_Y    8.0f  //13.0f
    float maxDist = (axis == ENUM_HOR_X) ? FINE_SHADOW_GRP_DIST_X : FINE_SHADOW_GRP_DIST_Y;
    for (i = sortCntSt; (i*inc) < sortCntEd; i+=inc) {
        currPos = (axis == ENUM_HOR_X) ? inst[sort[i]].x : inst[sort[i]].y;
        nextPos = (axis == ENUM_HOR_X) ? inst[sort[i+inc]].x : inst[sort[i+inc]].y;
        posDist = GET_ABS(currPos - nextPos);
        if (posDist > maxDist) {
            break;
        }
    }
    sortCntEd = i;
    remainedSortCnt = 0;

#if 0 //(DEBUG_fine_get_inst > 1) //for testing
    IS_DEBUG_FLAG
    {
        for (i = 0; i < sortCnt; i++) {
            TRACENR("%d ", remainedSortTmp[i]);
        }
        TRACE("=>before remainedSortTmp %d", sortCnt);
    };
#endif
    for (i = sortCntSt; (i*inc) < (sortCntEd*inc); i += inc) {
        remainedSort[remainedSortCnt++] = remainedSortTmp[sort[i]];
    }
#if 0 //(DEBUG_fine_get_inst > 1) //for testing
    IS_DEBUG_FLAG
    {
        for (i = 0; i < remainedSortCnt; i++) {
            TRACENR("%d ", remainedSortTmp[i]);
        }
        TRACE("=>after remainedSort %d", remainedSortCnt);
    }
#endif

    return remainedSortCnt; //not-found
}

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_BS_fine_add_remained_touch_shadow    1
#endif
#if (DEBUG_BS_fine_add_remained_touch_shadow > 0)
#define TRACE_BFARTS(...) TRACE(__VA_ARGS__)
#else
#define TRACE_BFARTS(...)
#endif

#if 1 //nsmoon@211028
#define TRACE_FSGDCT(...)    //TRACE(__VA_ARGS__)
static int fine_shadow_get_dist_closed_tp(axis_t axis, vec_t *cent, float *dist, float *distOpp)
{
    int tIdx; //side;
    int touchCntOrg = BG_touch_count;
    float centerPoint, centerPointOpp, touchHalfWidth;
    float minPos, maxPos, diffDist, diffDistOpp;
    float diffDistMin = MIN_INITIAL_VAL;
    float diffDistOppMin = MIN_INITIAL_VAL;
    IS_DEBUG_FLAG {TRACE_FSGDCT("  cent=(%d)(%0.1f,%0.1f)", axis, cent->x, cent->y);};

    int tIdxMin = -1; //, tIdxOppMin = -1;
    for (tIdx = 0; tIdx < touchCntOrg; tIdx++)
    {
        if (axis == ENUM_HOR_X) {
            centerPoint = BG_touch_data[tIdx].x;
            centerPointOpp = BG_touch_data[tIdx].y;
            touchHalfWidth = BG_touch_size[tIdx].xSize * 0.5f;
            minPos = centerPoint - touchHalfWidth;
            maxPos = centerPoint + touchHalfWidth;
            diffDist = GET_MIN(GET_ABS(minPos - cent->x), GET_ABS(maxPos - cent->x));
            diffDistOpp = GET_ABS(centerPointOpp - cent->y);
        }
        else {
            centerPoint = BG_touch_data[tIdx].y;
            centerPointOpp = BG_touch_data[tIdx].x;
            touchHalfWidth = BG_touch_size[tIdx].ySize * 0.5f;
            minPos = centerPoint - touchHalfWidth;
            maxPos = centerPoint + touchHalfWidth;
            diffDist = GET_MIN(GET_ABS(minPos - cent->y), GET_ABS(maxPos - cent->y));
            diffDistOpp = GET_ABS(centerPointOpp - cent->x);
        }
        IS_DEBUG_FLAG {TRACE_FSGDCT("   diffDist=(%d)%0.1f(%0.1f,%0.1f)%0.1f ", tIdx, diffDist, minPos, maxPos, diffDistOpp);};
        if (diffDist < diffDistMin) {
            diffDistMin = diffDist;
            tIdxMin = tIdx;
        }
        if (diffDistOpp < diffDistOppMin) {
            diffDistOppMin = diffDistOpp;
            //tIdxOppMin = tIdx;
        }
    } //for (tIdx = 0; tIdx < BG_touch_count; tIdx++)

    IS_DEBUG_FLAG {TRACE_FSGDCT("   =>diffDist=%0.1f %0.1f ", diffDist, diffDistOpp);};
    (*dist) = diffDistMin;
    (*distOpp) = diffDistOppMin;

    return tIdxMin; //no-error
}
#endif

#if 1 //nsmoon@211027
#define FINE_SHADOW_CENTLINE_HALFWIDTH    10.0f
static int fine_shadow_cal_cent_pos2(axis_t axis2, int tmpInBufIdx, find_shadow_slope_line_t *slopeLine, vec_t *p0, vec_t *p1, int tryCnt, int fineLoop)
{
    uint16_t *remained;
    int remainedCnt;
    initial_line_t *initial_line;
    int initial_line_cnt;
    initial_line_group_t *grp;
    int grp_cnt, grp_max_idx;
    //in_line_t *lineSen;
    int sortLen, isEdgeArea;
    int minIdx, maxIdx;
    vec_t p2, p3, centPos;
    axis_t axis1;
    int ret;
#if 0
    pos_min_max_t minMaxTmp;
    pos_minMax2_t min_max, instPos2;
    vec_t retMinPos, retMaxPos;
    float retWidth;
    int k;
#endif
    IS_DEBUG_FLAG{
        TRACE_FATD("fine_shadow_cal_cent_pos2...%d %d %d %d %d", axis2, tmpInBufIdx, slopeLine->slopeSign, slopeLine->slopeVal, fineLoop);
    TRACE_NOP;
    };

    tp_line_cnt_t thCnt;
    thCnt.th10CntX = thCnt.th50CntX = 0;
    thCnt.th10WidthX = 0;
    thCnt.th10CntY = thCnt.th50CntY = 0;
    thCnt.th10WidthY = 0;

    if (axis2 == ENUM_HOR_X) {
        remained = &BS_remained_x[0];
        remainedCnt = BS_remained_x_cnt;
        axis1 = ENUM_VER_Y;
        initial_line = &BS_initial_line_y[0];
    }
    else { //(axis2 == ENUM_VER_Y)
        remained = &BS_remained_y[0];
        remainedCnt = BS_remained_y_cnt;
        axis1 = ENUM_HOR_X;
        initial_line = &BS_initial_line_x[0];
    }

    if (BS_getLinePosInBuf(axis2, tmpInBufIdx, &p2, &p3)) {
        TRACE_ERROR("ERROR! fine_shadow_cal_cent_pos2..BS_getLinePosInBuf() [%d]", BG_frame_no);
        return 0; //error
    }
    IS_DEBUG_FLAG{
        DEBUG_SHOW_LINE_POS(&p2, &p3, MY_COLOR/* -2*/);
        TRACE_NOP;
    };
    slopeLine->inBufIdx = tmpInBufIdx;
    initial_line_cnt = fine_get_inst3(axis1, p2, p3, axis2, remained, remainedCnt, slopeLine);
    if (initial_line_cnt == FINE_MEM_ERROR) {
        IS_DEBUG_FLAG{ TRACE_FATD("initial_line_cnt..FINE_MEM_ERROR"); };
        return FINE_MEM_ERROR;
    }
    sortLen = initial_line[initial_line_cnt].sortLen;
    isEdgeArea = initial_line[initial_line_cnt].isEdgeArea;
    if ((((axis1 == ENUM_HOR_X && IS_NOT_NEAR_Y(isEdgeArea)) ||
        (axis1 == ENUM_VER_Y && IS_NOT_NEAR_X(isEdgeArea)))
        && sortLen < FINE_INITIAL_GRP_MIN_NUM_SHADOW) ||
        (((axis1 == ENUM_HOR_X && IS_NEAR_Y(isEdgeArea)) ||
        (axis1 == ENUM_VER_Y && IS_NEAR_X(isEdgeArea)))
            && sortLen < FINE_INITIAL_GRP_MIN_NUM_SHADOW)) { //nsmoon@211027
        IS_DEBUG_FLAG{ TRACE_FATD("  no inst-4c %04x, %d", isEdgeArea, sortLen); };
        return 0; //no-grp
    }
    //lineSen = initial_line[initial_line_cnt].lineSen;
    IS_DEBUG_FLAG{
        TRACE_FATD("  sortLen=%d 0x%x", sortLen, isEdgeArea);
    TRACE_NOP;
    };

    ret = fine_make_group_shadow_cent(axis1, initial_line_cnt, axis2, fineLoop);
    if (ret == FINE_MEM_ERROR) {
        IS_DEBUG_FLAG{ TRACE_FATD("  ret..FINE_MEM_ERROR"); };
        return FINE_MEM_ERROR;
    }
    if (ret != FINE_GROUP_FOUND) {
        goto L_fine_shadow_cal_cent_pos2_no_grp;
        //return 0; //no group
    }

    grp = initial_line[initial_line_cnt].grp;
    grp_cnt = initial_line[initial_line_cnt].grpCnt;
    if (grp_cnt < 1)
    {
        IS_DEBUG_FLAG{ TRACE_FATD("  no group~8~"); };
        goto L_fine_shadow_cal_cent_pos2_no_grp;
        //return 0; //not-found
    }

    float dist, distOpp, distOppMin;
    vec_t cent;
    grp_max_idx = 0;
    if (grp_cnt > 1) {
        distOppMin = MIN_INITIAL_VAL;
        //grp_max_idx = -1;
        int i;
        for (i = 0; i < grp_cnt; i++) {
            if (grp[i].len < 1) continue;
            minIdx = grp[i].instIdx[0];
            maxIdx = grp[i].instIdx[grp[i].len - 1];
            cent.x = (initial_line[initial_line_cnt].inst[minIdx].x + initial_line[initial_line_cnt].inst[maxIdx].x) * 0.5f;
            cent.y = (initial_line[initial_line_cnt].inst[minIdx].y + initial_line[initial_line_cnt].inst[maxIdx].y) * 0.5f;
            if (fine_shadow_get_dist_closed_tp(axis2, &cent, &dist, &distOpp) < 0) {
                IS_DEBUG_FLAG{ TRACE_FATD("ERROR! fine_shadow_cal_cent_pos2..fine_shadow_get_dist_closed_tp");};
                break;
            }
            if (dist > 0 && distOpp > 0 && distOpp < distOppMin) {
                distOppMin = distOpp;
                grp_max_idx = i;
            }
        }
    }

    minIdx = grp[grp_max_idx].instIdx[0];
    maxIdx = grp[grp_max_idx].instIdx[grp[grp_max_idx].len - 1];
#if (DEBUG_fine_add_touch_data > 0)
    DEBUG_SHOW_MIN_MAX_POS(axis2, &initial_line[initial_line_cnt].inst[0], minIdx, maxIdx, 1);
#endif
    centPos.x = (initial_line[initial_line_cnt].inst[minIdx].x + initial_line[initial_line_cnt].inst[maxIdx].x) * 0.5f;
    centPos.y = (initial_line[initial_line_cnt].inst[minIdx].y + initial_line[initial_line_cnt].inst[maxIdx].y) * 0.5f;
    if (axis1 == ENUM_HOR_X) {
        p0->x = p1->x = centPos.x;
        p1->y = centPos.y - FINE_SHADOW_CENTLINE_HALFWIDTH;
        p0->y = centPos.y + FINE_SHADOW_CENTLINE_HALFWIDTH;
    }
    else {
        p1->x = centPos.x - FINE_SHADOW_CENTLINE_HALFWIDTH;
        p0->x = centPos.x + FINE_SHADOW_CENTLINE_HALFWIDTH;
        p0->y = p1->y = centPos.y;
    }

    return 1; //no-error

L_fine_shadow_cal_cent_pos2_no_grp:
    if (tryCnt < 1) {
        return 0; //no-grp
    }
#define FINE_SHADOW_CLOSED_TP_EDGE    50.0f
    cent.x = (initial_line[initial_line_cnt].inst[0].x + initial_line[initial_line_cnt].inst[0].x) * 0.5f;
    cent.y = (initial_line[initial_line_cnt].inst[0].y + initial_line[initial_line_cnt].inst[0].y) * 0.5f;
    int tpIdx = fine_shadow_get_dist_closed_tp(axis2, &cent, &dist, &distOpp);
    IS_DEBUG_FLAG{ TRACE_FATD("  no group~7~dist=%0.1f %0.1f (%d/%d)(%0.1f,%0.1f)", dist, distOpp, axis2, tpIdx, cent.x, cent.y);};
    if (isEdgeArea != 0 && tpIdx >= 0 && dist < FINE_SHADOW_CLOSED_TP_EDGE) {
        centPos.x = BG_touch_data[tpIdx].x;
        centPos.y = BG_touch_data[tpIdx].y;
        if (axis2 == ENUM_VER_Y) {
            p0->x = p1->x = centPos.x;
            if (centPos.y < cent.y && IS_EDGE_PD_Y(isEdgeArea)) {
                //top-side: p0 is led(bot)
                p0->y = centPos.y;
                p1->y = centPos.y + FINE_SHADOW_CLOSED_TP_EDGE;
                p1->y = GET_MIN(p1->y, BS_sensor_end_y);
            }
            else if (centPos.y > cent.y && IS_EDGE_LED_Y(isEdgeArea)){
                //bottom-side: p1 is pd(top)
                p1->y = centPos.y;
                p0->y = centPos.y - FINE_SHADOW_CLOSED_TP_EDGE;
                p0->y = GET_MAX(p0->y, BS_sensor_zero_y);
            }
        }
        else {
            p0->y = p1->y = centPos.y;
            if (centPos.x > cent.x && IS_EDGE_PD_X(isEdgeArea)) {
                //right-side: p0 is led(left)
                p0->x = centPos.x;
                p1->x = centPos.x - FINE_SHADOW_CLOSED_TP_EDGE;
                p1->x = GET_MAX(p1->x, BS_sensor_zero_x);
            }
            else if (centPos.x < cent.x && IS_EDGE_LED_X(isEdgeArea)) {
                //left-side: p1 is pd(right)
                p0->x = centPos.x + FINE_SHADOW_CLOSED_TP_EDGE;
                p0->x = GET_MIN(p0->x, BS_sensor_end_x);
                p1->x = centPos.x;
            }
        }
        return 1; //no-error
    }
    return 0; //no-grp
}
#endif

static int fine_shadow_get_slope_line(axis_t axis2, uint16_t *remLine, int remLineCnt, find_shadow_slope_line_t *slopeLine)
{
    //check plus/minus slope
    int remLineIdx, sortCnt;
    int tmpInBufIdx, tmpInBufIdx_i, tmpInBufIdx_j;
    int pd = 0, led = 0;
    int slopeVal, slopeVal_i, slopeVal_j;
    uint16_t *remSort = &BS_remained_sort[0];
    int i, j, ret;

    tmpInBufIdx = slopeVal = sortCnt = 0;
    for (remLineIdx = 0; remLineIdx < remLineCnt; remLineIdx++) {
        tmpInBufIdx = remLine[remLineIdx];
        if (BS_getSenInBuf(axis2, tmpInBufIdx, &pd, &led) < 0) {
            TRACE_ERROR("ERROR! fine_shadow_get_slope_line..BS_getSenInBuf() [%d]", BG_frame_no);
            continue;
        }
        slopeVal = led - pd;
        if ((slopeVal > 0 && slopeLine->slopeSign < 0) ||
            (slopeVal < 0 && slopeLine->slopeSign > 0)) {
            continue;
        }
        if (sortCnt < MAX_REMAINED_LINE_SORT) {
            //IS_DEBUG_FLAG{ TRACENR("%d ", slopeVal); };
            remSort[sortCnt++] = tmpInBufIdx;
        }
        else {
            TRACE_ERROR("ERROR! fine_shadow_get_slope_line..invalid sortCnt %d", sortCnt);
            return 1; //nsmoon@211201 FIXME!! //break;
        }
    } //for (remLineIdx = 0; remLineIdx < remLineCnt; remLineIdx++)
    //IS_DEBUG_FLAG{ TRACENR("\r\n"); };

    for (i = 0; i < sortCnt - 1; i++) {
        for (j = (i + 1); j < sortCnt; j++) {
            tmpInBufIdx_i = remSort[i];
            tmpInBufIdx_j = remSort[j];
            if (BS_getSenInBuf(axis2, tmpInBufIdx_i, &pd, &led) < 0) {
                TRACE_ERROR("ERROR! fine_shadow_get_slope_line..BS_getSenInBuf() [%d]", BG_frame_no);
                continue;
            }
            slopeVal_i = led - pd;
            if (BS_getSenInBuf(axis2, tmpInBufIdx_j, &pd, &led) < 0) {
                TRACE_ERROR("ERROR! fine_shadow_get_slope_line.1.BS_getSenInBuf() [%d]", BG_frame_no);
                continue;
            }
            slopeVal_j = led - pd;
            if (slopeVal_i > slopeVal_j) {
                remSort[i] = tmpInBufIdx_j;
                remSort[j] = tmpInBufIdx_i;
            }
        }
    }
#if 0 //for test
    IS_DEBUG_FLAG{
        for (i = 0; i < sortCnt; i++) {
            tmpInBufIdx = remSort[i];
            if (BS_getSenInBuf(axis2, tmpInBufIdx, &pd, &led) < 0) {
                TRACE_ERROR("ERROR! fine_shadow_get_slope_line..BS_getSenInBuf() [%d]", BG_frame_no);
            }
            slopeVal = led - pd;
            TRACENR("%d ", slopeVal);
        }
        TRACENR("\r\n");
    };
#endif
    if (sortCnt > 2) {
        int remSortIdx = 0;
        if (slopeLine->min_max == FINE_SHADOW_GET_SLOPE_LINE_FIRST) {
            remSortIdx = 0;
        }
        else if (slopeLine->min_max == FINE_SHADOW_GET_SLOPE_LINE_LAST) {
            remSortIdx = sortCnt - 1;
        }
        else {
            remSortIdx = sortCnt / 2;
        }
        tmpInBufIdx = remSort[remSortIdx];
        if (BS_getSenInBuf(axis2, tmpInBufIdx, &pd, &led) < 0) {
            TRACE_ERROR("ERROR! fine_shadow_get_slope_line..BS_getSenInBuf() [%d]", BG_frame_no);
        }
        slopeVal = led - pd;
        ret = 0; //no-error
    }
    else {
        TRACE_ERROR("ERROR! fine_shadow_get_slope_line.2.invalid sortCnt %d", sortCnt);
        ret = 1; //error
    }

    slopeLine->sortCnt = (uint16_t)sortCnt;
    slopeLine->slopeVal = (int16_t)slopeVal;
    slopeLine->inBufIdx = (uint16_t)tmpInBufIdx;
    return ret;
}

#define FINE_SHADOW_TP_DIST    300.0f
#define FINE_SHADOW_REMLINE_MIN           5
#define FINE_SHADOW_REMLINE_SLOPE_MAX     10
//#define FINE_SHADOW_SLOPE_DIFF_MARGIN     5

int BS_fine_add_remained_touch_shadow(int fineLoop)
{
    axis_t axis2;
    int ret;
    int touchCntOrg = BG_touch_count;
    uint16_t *remLine;
    int remLineCnt, remLineIdx;
    int tmpInBufIdx, tmpInBufIdx_p, tmpInBufIdx_m;
    int pd, led;
    int slopeVal, slopeVal_p, slopeVal_m, slopeVal_p_cnt, slopeVal_m_cnt;
    find_shadow_slope_line_t findShadowSlopeLine;
    vec_t p0, p1;

    TRACE_BFARTS("BS_fine_add_remained_touch_shadow..(%d)", fineLoop);

    if (BS_remained_x_cnt > BS_remained_y_cnt) {
        remLine = &BS_remained_x[0];
        remLineCnt = BS_remained_x_cnt;
        axis2 = ENUM_HOR_X;
    }
    else {
        remLine = &BS_remained_y[0];
        remLineCnt = BS_remained_y_cnt;
        axis2 = ENUM_VER_Y;
    }
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };

    //check plus/minus slope
    tmpInBufIdx_p = tmpInBufIdx_m = -1;
    slopeVal_p = -1; slopeVal_m = 1;
    slopeVal_p_cnt = slopeVal_m_cnt = 0;
    for (remLineIdx = 0; remLineIdx < remLineCnt; remLineIdx++) {
        tmpInBufIdx = remLine[remLineIdx];
        if (BS_getSenInBuf(axis2, tmpInBufIdx, &pd, &led) < 0) {
            TRACE_ERROR("ERROR! fine_shadow_cal_cent_pos..BS_getSenInBuf() [%d]", BG_frame_no);
            continue;
        }
        slopeVal = led - pd;
        if (slopeVal >= 0) {
            slopeVal_p_cnt++;
            if (slopeVal > slopeVal_p) {
            slopeVal_p = slopeVal;
            tmpInBufIdx_p = tmpInBufIdx;
        }
        }
        if (slopeVal <= 0) {
            slopeVal_m_cnt++;
            if (slopeVal < slopeVal_m) {
            slopeVal_m = slopeVal;
            tmpInBufIdx_m = tmpInBufIdx;
            }
        }
    } //for (remLineIdx = 0; remLineIdx < remLineCnt; remLineIdx++)
#if (DEBUG_BS_fine_add_remained_touch_shadow > 0)
    IS_DEBUG_FLAG{
        TRACE("slopeVal_p=%d,%d %d,%d", slopeVal_p, slopeVal_m, slopeVal_p_cnt, slopeVal_m_cnt);
    }
#endif

    if (slopeVal_p != -1 && slopeVal_m != 1) {
#if 1 //nsmoon@211029
        //plus and minus slope
        if (slopeVal_p_cnt < slopeVal_m_cnt && slopeVal_p >= GET_ABS(slopeVal_m)) {
            tmpInBufIdx = tmpInBufIdx_p; //plus max slope line
            findShadowSlopeLine.slopeSign = findShadowSlopeLine.slopeVal = -1; //minus lines
        }
        else if (slopeVal_p_cnt >= slopeVal_m_cnt && slopeVal_p < GET_ABS(slopeVal_m)) {
            tmpInBufIdx = tmpInBufIdx_m; //minus max slope line
            findShadowSlopeLine.slopeSign = findShadowSlopeLine.slopeVal = 1; //plus lines
        }
        else if (slopeVal_p_cnt >= slopeVal_m_cnt) {
            tmpInBufIdx = tmpInBufIdx_m; //minus max slope line
            findShadowSlopeLine.slopeSign = findShadowSlopeLine.slopeVal = 1; //plus lines
        }
        else {
            tmpInBufIdx = tmpInBufIdx_p; //plus max slope line
            findShadowSlopeLine.slopeSign = findShadowSlopeLine.slopeVal = -1; //minus lines
        }
#else
        if (slopeVal_p_cnt > FINE_SHADOW_REMLINE_MIN && slopeVal_m_cnt > FINE_SHADOW_REMLINE_MIN) {
        if (slopeVal_p >= GET_ABS(slopeVal_m)) {
                tmpInBufIdx = tmpInBufIdx_p; //plus max slope line
                findShadowSlopeLine.slopeSign = findShadowSlopeLine.slopeVal = -1; //minus lines
            }
            else if (slopeVal_p <= GET_ABS(slopeVal_m)) {
                tmpInBufIdx = tmpInBufIdx_m; //minus max slope line
                findShadowSlopeLine.slopeSign = findShadowSlopeLine.slopeVal = 1; //plus lines
            }
        }
        else {
            if (slopeVal_p_cnt > FINE_SHADOW_REMLINE_MIN) {
                tmpInBufIdx = tmpInBufIdx_m; //minus max slope line
                findShadowSlopeLine.slopeSign = findShadowSlopeLine.slopeVal = 1; //plus lines
            }
            else {
                tmpInBufIdx = tmpInBufIdx_p; //minus max slope line
                findShadowSlopeLine.slopeSign = findShadowSlopeLine.slopeVal = -1; //minus lines
            }
        }
#endif
    }
    else if (slopeVal_p == -1) {
        //no plus slope
        tmpInBufIdx = tmpInBufIdx_m; //minus max slope line
        findShadowSlopeLine.slopeSign = -1; //minus
        findShadowSlopeLine.min_max = FINE_SHADOW_GET_SLOPE_LINE_MID;
        if (fine_shadow_get_slope_line(axis2, remLine, remLineCnt, &findShadowSlopeLine)) {
            TRACE("ERROR! BS_fine_add_remained_touch_shadow..fine_shadow_get_slope_line~1~");
            return 0; //no-touch
        }
        findShadowSlopeLine.slopeSign = 0; //same slope line
    }
    else if (slopeVal_m == 1) {
        //no minus slope
        tmpInBufIdx = tmpInBufIdx_p; //plus max slope line
        findShadowSlopeLine.slopeSign = 1;
        findShadowSlopeLine.min_max = FINE_SHADOW_GET_SLOPE_LINE_MID;
        if (fine_shadow_get_slope_line(axis2, remLine, remLineCnt, &findShadowSlopeLine)) {
            TRACE("ERROR! BS_fine_add_remained_touch_shadow..fine_shadow_get_slope_line~2~");
            return 0; //no-touch
        }
        findShadowSlopeLine.slopeSign = 0; //same slope line
    }
    else {
        TRACE_ERROR("ERROR! fine_shadow_cal_cent_pos..invalid tmpInBufIdxLed");
        return 1; //error
    }
#if (DEBUG_BS_fine_add_remained_touch_shadow > 0)
    IS_DEBUG_FLAG{
        if (BS_getLinePosInBuf(axis2, tmpInBufIdx, &p0, &p1)) {
            TRACE_ERROR("ERROR! fine_get_inst..BS_getLinePosInBuf() [%d]", BG_frame_no);
            return 1; //error
        }
        DEBUG_SHOW_LINE_POS(&p0, &p1, MY_COLOR/* -2*/);
        TRACE_NOP;
    };
#endif

    //reset mem
    fine_bs_mem_init();

    //calculate center line
    int centNotFound = 0;
    if (fine_shadow_cal_cent_pos2(axis2, tmpInBufIdx, &findShadowSlopeLine, &p0, &p1, 0, fineLoop) < 1) {
        centNotFound++;
    }
#if 1 //nsmoon@211030
    if (centNotFound) {
        if (findShadowSlopeLine.slopeSign == 1 && findShadowSlopeLine.slopeVal == 1) {
            if (slopeVal_m_cnt > FINE_SHADOW_REMLINE_MIN) {
                findShadowSlopeLine.slopeSign = 1; //plus
                findShadowSlopeLine.min_max = FINE_SHADOW_GET_SLOPE_LINE_LAST; //max:1+last
                if (fine_shadow_get_slope_line(axis2, remLine, remLineCnt, &findShadowSlopeLine)) {
                    TRACE("ERROR! BS_fine_add_remained_touch_shadow..fine_shadow_get_slope_line~4~");
                    return 0; //no-touch
                }
                tmpInBufIdx = findShadowSlopeLine.inBufIdx;
                findShadowSlopeLine.slopeSign = findShadowSlopeLine.slopeVal = -1; //minus
            }
            else {
                findShadowSlopeLine.slopeSign = -1; //minus
                findShadowSlopeLine.min_max = FINE_SHADOW_GET_SLOPE_LINE_LAST; //min:-1+last
                if (fine_shadow_get_slope_line(axis2, remLine, remLineCnt, &findShadowSlopeLine)) {
                    TRACE("ERROR! BS_fine_add_remained_touch_shadow..fine_shadow_get_slope_line~4~");
                    return 0; //no-touch
                }
                tmpInBufIdx = findShadowSlopeLine.inBufIdx;
                findShadowSlopeLine.min_max = FINE_SHADOW_GET_SLOPE_LINE_MID;
                if (fine_shadow_get_slope_line(axis2, remLine, remLineCnt, &findShadowSlopeLine)) {
                    TRACE("ERROR! BS_fine_add_remained_touch_shadow..fine_shadow_get_slope_line~4~");
                    return 0; //no-touch
                }
                findShadowSlopeLine.slopeSign = 0; //same slope line
            }
        }
        else if (findShadowSlopeLine.slopeSign == -1 && findShadowSlopeLine.slopeVal == -1) {
            if (slopeVal_p_cnt > FINE_SHADOW_REMLINE_MIN) {
                findShadowSlopeLine.slopeSign = -1; //minus
                findShadowSlopeLine.min_max = FINE_SHADOW_GET_SLOPE_LINE_FIRST; //max:-1+first
                if (fine_shadow_get_slope_line(axis2, remLine, remLineCnt, &findShadowSlopeLine)) {
                    TRACE("ERROR! BS_fine_add_remained_touch_shadow..fine_shadow_get_slope_line~4~");
                    return 0; //no-touch
                }
                tmpInBufIdx = findShadowSlopeLine.inBufIdx;
                findShadowSlopeLine.slopeSign = findShadowSlopeLine.slopeVal = 1; //plus
            }
            else {
                findShadowSlopeLine.slopeSign = 1; //plus
                findShadowSlopeLine.min_max = FINE_SHADOW_GET_SLOPE_LINE_LAST; //min:1+first
                if (fine_shadow_get_slope_line(axis2, remLine, remLineCnt, &findShadowSlopeLine)) {
                    TRACE("ERROR! BS_fine_add_remained_touch_shadow..fine_shadow_get_slope_line~4~");
                    return 0; //no-touch
                }
                tmpInBufIdx = findShadowSlopeLine.inBufIdx;
                findShadowSlopeLine.min_max = FINE_SHADOW_GET_SLOPE_LINE_MID;
                if (fine_shadow_get_slope_line(axis2, remLine, remLineCnt, &findShadowSlopeLine)) {
                    TRACE("ERROR! BS_fine_add_remained_touch_shadow..fine_shadow_get_slope_line~4~");
                    return 0; //no-touch
                }
                findShadowSlopeLine.slopeSign = 0; //same slope line
            }
        }
        else {
            TRACE("  fine_shadow_cal_cent_pos2~1~<1");
            return 0; //no-touch
        }
        if (fine_shadow_cal_cent_pos2(axis2, tmpInBufIdx, &findShadowSlopeLine, &p0, &p1, 1, fineLoop) < 1) {
            TRACE("  fine_shadow_cal_cent_pos2~2~<1");
            return 0; //no-touch
        }
    }
#else
    if (centNotFound) {
        if (findShadowSlopeLine.slopeSign == -1 || findShadowSlopeLine.slopeSign == 1) {
            if (findShadowSlopeLine.slopeSign == 1) {
                if (slopeVal_m_cnt > FINE_SHADOW_REMLINE_MIN) {
                    findShadowSlopeLine.slopeSign = -1; //minus
                    findShadowSlopeLine.min_max = FINE_SHADOW_GET_SLOPE_LINE_FIRST; //max:-1+first
                    if (fine_shadow_get_slope_line(axis2, remLine, remLineCnt, &findShadowSlopeLine)) {
                        TRACE("ERROR! BS_fine_add_remained_touch_shadow..fine_shadow_get_slope_line~4~");
                        return 0; //no-touch
                    }
                    tmpInBufIdx = findShadowSlopeLine.inBufIdx;
                    findShadowSlopeLine.slopeSign = findShadowSlopeLine.slopeVal = 1; //plus
                }
                else {
                    findShadowSlopeLine.min_max = FINE_SHADOW_GET_SLOPE_LINE_LAST; //max:1+last
                    if (fine_shadow_get_slope_line(axis2, remLine, remLineCnt, &findShadowSlopeLine)) {
                        TRACE("ERROR! BS_fine_add_remained_touch_shadow..fine_shadow_get_slope_line~4~");
                        return 0; //no-touch
                    }
                    tmpInBufIdx = findShadowSlopeLine.inBufIdx;
                    findShadowSlopeLine.min_max = FINE_SHADOW_GET_SLOPE_LINE_MID;
                    if (fine_shadow_get_slope_line(axis2, remLine, remLineCnt, &findShadowSlopeLine)) {
                        TRACE("ERROR! BS_fine_add_remained_touch_shadow..fine_shadow_get_slope_line~4~");
                        return 0; //no-touch
                    }
                    findShadowSlopeLine.slopeSign = 0;
                }
            }
            else {
                if (slopeVal_p_cnt > FINE_SHADOW_REMLINE_MIN) {
                    findShadowSlopeLine.slopeSign = 1; //plus
                    findShadowSlopeLine.min_max = FINE_SHADOW_GET_SLOPE_LINE_LAST; //max:1+last
                    if (fine_shadow_get_slope_line(axis2, remLine, remLineCnt, &findShadowSlopeLine)) {
                        TRACE("ERROR! BS_fine_add_remained_touch_shadow..fine_shadow_get_slope_line~4~");
                        return 0; //no-touch
    }
                    tmpInBufIdx = findShadowSlopeLine.inBufIdx;
                    findShadowSlopeLine.slopeSign = findShadowSlopeLine.slopeVal = -1; //minus
            }
            else {
                    findShadowSlopeLine.min_max = FINE_SHADOW_GET_SLOPE_LINE_FIRST; //max:-1+first
                    if (fine_shadow_get_slope_line(axis2, remLine, remLineCnt, &findShadowSlopeLine)) {
                        TRACE("ERROR! BS_fine_add_remained_touch_shadow..fine_shadow_get_slope_line~4~");
                        return 0; //no-touch
                    }
                    tmpInBufIdx = findShadowSlopeLine.inBufIdx;
                    findShadowSlopeLine.min_max = FINE_SHADOW_GET_SLOPE_LINE_MID;
                    if (fine_shadow_get_slope_line(axis2, remLine, remLineCnt, &findShadowSlopeLine)) {
                        TRACE("ERROR! BS_fine_add_remained_touch_shadow..fine_shadow_get_slope_line~4~");
                        return 0; //no-touch
                    }
                    findShadowSlopeLine.slopeSign = 0;
                }
            }
            if (fine_shadow_cal_cent_pos2(axis2, tmpInBufIdx, &findShadowSlopeLine, &p0, &p1, fineLoop) < 1) {
                TRACE(" fine_shadow_cal_cent_pos2~2~ < 1");
                return 0; //no-touch
            }
        }
        else {
            TRACE(" fine_shadow_cal_cent_pos2~1~ < 1");
            return 0; //no-touch
        }
    }
#endif
    IS_DEBUG_FLAG{
        DEBUG_SHOW_LINE_POS(&p0, &p1, MY_COLOR);
        TRACE_NOP;
    };

    //reset mem
    fine_bs_mem_init();

    ret = fine_add_touch_data_shadow2(axis2, &p0, &p1, fineLoop);
    if (ret > 0) { //nsmoon@220118 >0
        int touchCnt = BG_touch_count;
        BS_adj_used_lines_tp(touchCntOrg, touchCnt, ADJUST_USED_LINE_MULTI/*ADJUST_USED_LINE_NORMAL*/); //adjust used line, nsmoon@211019 ADJUST_USED_LINE_NORMAL=>ADJUST_USED_LINE_MULTI //BS_fine_add_remained_touch_shadow
        return 1; //touch
    }

    TRACE(" fine_add_touch_data_shadow2 ret %d", ret);
    return 0; //no-touch
}
#endif

#if 1 //nsmoon@201118
#define TRACE_FARTEX(...)    //TRACE(__VA_ARGS__)
static int add_edge_sen_buff(uint8_t *senBuf, int senCnt, int senNo)
{
    int i;
    //TRACE_FARTEX("add_edge_sen_buff..%d,%d ", senCnt, senNo);
    for (i = 0; i < senCnt; i++) {
        //TRACE_FARTEX(" senBuf..%d,%d ", i, senBuf[i]);
        if (senBuf[i] == (uint8_t)senNo) {
            return 0; //exist
        }
    }
    if (i < MAX_NUM_PD_REMLINE) {
        senBuf[i] = (uint8_t)senNo;
        TRACE_FARTEX("=>new..%d,%d ", i, senBuf[i]);
        return 1; //new
    }
    return 0; //no-buff
}

#define TOUCH_DATA_EDGE_OFFSET  2.0f
int BS_fine_add_remained_touch_edge_x()
{
    int remLineIdx, tmpInBufIdx;
    int pd, led, pdCnt = 0, ledCnt = 0;

    //reset mem
    fine_bs_mem_init(); //nsmoon@211201

    for (remLineIdx = 0; remLineIdx < BS_remained_x_cnt; remLineIdx++) {
        tmpInBufIdx = BS_remained_x[remLineIdx];
        if (BS_getSenInBuf(ENUM_HOR_X, tmpInBufIdx, &pd, &led)) {
            continue;
        }
        //int slopeVal = GET_ABS(led - pd);
        //int slopeSign = BS_get_slope_sign(led, pd);
        TRACE_FARTEX("pd,led=%d,%d ", pd, led);
        if (add_edge_sen_buff((uint8_t *)&pdEdgeBuf[0], pdCnt, pd)) {
            pdCnt++;
        }
        if (add_edge_sen_buff((uint8_t *)&ledEdgeBuf[0], ledCnt, led)) {
            ledCnt++;
        }
    }
#if 1 //nsmoon@201202
    vec_t centTmp;
    if ((pdCnt > 0 && pdCnt < 3) && ledCnt >= 3) { //nsmoon@220118 >=
        TRACE_FARTEX("top!!%d,%d", pdCnt, ledCnt);
        if (pdCnt > 1) {
            centTmp.x = (BS_pd_pos_x[pdEdgeBuf[0]] + BS_pd_pos_x[pdEdgeBuf[1]]) * 0.5f;
        }
        else {
            centTmp.x = BS_pd_pos_x[pdEdgeBuf[0]];
        }
        centTmp.y = (BS_sensor_end_y + BS_aarea_end_y) * 0.5f;
#ifdef FINE_REMAINED_INIT_LINE_MAX
        int centTmpCnt = 1;
        fine_add_touch_data(ENUM_HOR_X, &centTmp, centTmpCnt, 0, FINE_LOOP_EDGE_X); //1:check slope
#else
        fine_add_touch_data(ENUM_HOR_X, &centTmp, 0, FINE_LOOP_EDGE_X); //1:check slope
#endif
    }
    else if (pdCnt >= 3 && (ledCnt > 0 && ledCnt < 3)) {  //nsmoon@220118 >=
        TRACE_FARTEX("bot!!%d,%d", pdCnt, ledCnt);
        if (ledCnt > 1) {
            centTmp.x = (BS_led_pos_x[ledEdgeBuf[0]] + BS_led_pos_x[ledEdgeBuf[1]]) * 0.5f;
        }
        else {
            centTmp.x = BS_led_pos_x[ledEdgeBuf[0]];
        }
        centTmp.y = (BS_sensor_zero_y + BS_aarea_zero_y) * 0.5f;
#ifdef FINE_REMAINED_INIT_LINE_MAX
        int centTmpCnt = 1;
        fine_add_touch_data(ENUM_HOR_X, &centTmp, centTmpCnt, 0, FINE_LOOP_EDGE_X); //1:check slope
#else
        fine_add_touch_data(ENUM_HOR_X, &centTmp, 0, FINE_LOOP_EDGE_X); //1:check slope
#endif
    }
#else
    if (pdCnt > 0 && pdCnt < 3 && ledCnt > 3) {
        TRACE_FARTEX("top!!%d,%d", pdCnt, ledCnt);
        if (pdCnt > 1) {
            BG_touch_data_edge.x = (BS_pd_pos_x[pdEdgeBuf[0]] + BS_pd_pos_x[pdEdgeBuf[1]]) * 0.5f;
        }
        else {
            BG_touch_data_edge.x = BS_pd_pos_x[pdEdgeBuf[0]];
        }
        BG_touch_data_edge.y = BS_sensor_end_y - TOUCH_DATA_EDGE_OFFSET;
    }
    else if (pdCnt > 3 && ledCnt > 0 && ledCnt < 3) {
        TRACE_FARTEX("bot!!%d,%d", pdCnt, ledCnt);
        if (ledCnt > 1) {
            BG_touch_data_edge.x = (BS_led_pos_x[ledEdgeBuf[0]] + BS_led_pos_x[ledEdgeBuf[1]]) * 0.5f;
        }
        else {
            BG_touch_data_edge.x = BS_led_pos_x[ledEdgeBuf[0]];
        }
        BG_touch_data_edge.y = BS_sensor_zero_y + TOUCH_DATA_EDGE_OFFSET;
    }
#endif
    return 0;
}
#endif
#if 1 //nsmoon@220127
int BS_fine_add_remained_touch_edge_y()
{
    int remLineIdx, tmpInBufIdx;
    int pd, led, pdCnt = 0, ledCnt = 0;

    //reset mem
    fine_bs_mem_init(); //nsmoon@211201

    for (remLineIdx = 0; remLineIdx < BS_remained_y_cnt; remLineIdx++) {
        tmpInBufIdx = BS_remained_y[remLineIdx];
        if (BS_getSenInBuf(ENUM_VER_Y, tmpInBufIdx, &pd, &led)) {
            continue;
        }
        //int slopeVal = GET_ABS(led - pd);
        //int slopeSign = BS_get_slope_sign(led, pd);
        TRACE_FARTEX("pd,led=%d,%d ", pd, led);
        if (add_edge_sen_buff((uint8_t *)&pdEdgeBuf[0], pdCnt, pd)) {
            pdCnt++;
        }
        if (add_edge_sen_buff((uint8_t *)&ledEdgeBuf[0], ledCnt, led)) {
            ledCnt++;
        }
    }

    vec_t centTmp;
    if ((pdCnt > 0 && pdCnt < 3) && ledCnt >= 3) { //nsmoon@220118 >=
        TRACE_FARTEX("left!!%d,%d", pdCnt, ledCnt);
        if (pdCnt > 1) {
            centTmp.y = (BS_pd_pos_y[pdEdgeBuf[0]] + BS_pd_pos_y[pdEdgeBuf[1]]) * 0.5f;
        }
        else {
            centTmp.y = BS_pd_pos_y[pdEdgeBuf[0]];
        }
        centTmp.x = (BS_sensor_end_x + BS_aarea_end_x) * 0.5f;
#ifdef FINE_REMAINED_INIT_LINE_MAX
        int centTmpCnt = 1;
        fine_add_touch_data(ENUM_VER_Y, &centTmp, centTmpCnt, 0, FINE_LOOP_EDGE_X); //1:check slope
#else
        fine_add_touch_data(ENUM_VER_Y, &centTmp, 0, FINE_LOOP_EDGE_X); //1:check slope
#endif
    }
    else if (pdCnt >= 3 && (ledCnt > 0 && ledCnt < 3)) {  //nsmoon@220118 >=
        TRACE_FARTEX("right!!%d,%d", pdCnt, ledCnt);
        if (ledCnt > 1) {
            centTmp.y = (BS_led_pos_y[ledEdgeBuf[0]] + BS_led_pos_y[ledEdgeBuf[1]]) * 0.5f;
        }
        else {
            centTmp.y = BS_led_pos_y[ledEdgeBuf[0]];
        }
        centTmp.x = (BS_sensor_zero_x + BS_aarea_zero_x) * 0.5f;
#ifdef FINE_REMAINED_INIT_LINE_MAX
        int centTmpCnt = 1;
        fine_add_touch_data(ENUM_VER_Y, &centTmp, centTmpCnt, 0, FINE_LOOP_EDGE_X); //1:check slope
#else
        fine_add_touch_data(ENUM_VER_Y, &centTmp, 0, FINE_LOOP_EDGE_X); //1:check slope
#endif
    }
    return 0;
}
#endif

#if (BRUSH_MODE_ENABLE == 1)
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_BS_fine_add_remained_touch_brush  1
#endif
#if (DEBUG_BS_fine_add_remained_touch_brush > 0)
#define TRACE_BFARB(...) TRACE(__VA_ARGS__)
#else
#define TRACE_BFARB(...)
#endif
int BS_fine_add_remained_touch_brush(int fineLoop)
{
    float touchArea;
    int remLineIdx, tmpInBufIdx;
    int slopeValMin, slopeValMinIdx;
    //uint16_t *remained ;
    initial_line_t *initial_line_x, *initial_line_y;
    int initial_line_x_cnt, initial_line_y_cnt;
    initial_line_group_t *grpX, *grpY;
    int grpX_cnt, grpY_cnt;
    int grpX_max_idx, grpY_max_idx;
    in_line_t *lineSenX, *lineSenY;
    vec_t p0, p1, p2, p3;
    int touchCntOrg = BG_touch_count;
    int touchCntFineOrg = BG_touch_count_fine;
    int touchCnt = touchCntOrg;
    int touchCntFine = touchCntFineOrg;
    //pos_minMax2_t min_max;
    pos_min_max_t minMaxAll;
    int pd, led, slopeVal;
    int ret;
    //int i,
#if (DEBUG_BS_fine_add_remained_touch_brush > 0) && defined(DRAW_POLYGON_TEST)
    int minIdx, maxIdx;
#endif

    tp_line_cnt_t thCnt;
    thCnt.th10CntX = thCnt.th50CntX = 0;
    thCnt.th10WidthX = 0;
    thCnt.th10CntY = thCnt.th50CntY = 0;
    thCnt.th10WidthY = 0;

    IS_DEBUG_FLAG{
        TRACE_NOP;
    };

    //reset mem
    fine_bs_mem_init();

    //find min slope line
    slopeValMin = MIN_INITIAL_VAL;
    slopeValMinIdx = -1;
    for (remLineIdx = 0; remLineIdx < BS_remained_x_cnt; remLineIdx++) {
        tmpInBufIdx = BS_remained_x[remLineIdx];
        if (BS_getSenInBuf(ENUM_HOR_X, tmpInBufIdx, &pd, &led)) {
            continue;
        }
        slopeVal = GET_ABS(led - pd);
        //slopeSign = BS_get_slope_sign(led, pd);
        if (slopeVal < slopeValMin) {
            slopeValMinIdx = remLineIdx;
            slopeValMin = slopeVal;
        }
    }
    if (slopeValMinIdx < 0) {
        return 0; //not found
    }
    tmpInBufIdx = BS_remained_x[slopeValMinIdx];
    if (BS_getLinePosInBuf(ENUM_HOR_X, tmpInBufIdx, &p0, &p1)) {
        return 0; //not found
    }
#if (DEBUG_BS_fine_add_remained_touch_brush > 0)
    IS_DEBUG_FLAG{
        DEBUG_SHOW_LINE_POS(&p0, &p1, 0);
        TRACE_NOP;
    };
#endif

    //get y-grp
    initial_line_x = &BS_initial_line_x[0];
    initial_line_x_cnt = fine_get_inst3(ENUM_HOR_X, p0, p1, ENUM_VER_Y, &BS_remained_y[0], BS_remained_y_cnt, 0);
    if (initial_line_x_cnt == FINE_MEM_ERROR) {
        return FINE_MEM_ERROR;
    }
    if (initial_line_x[initial_line_x_cnt].sortLen < FINE_INITIAL_GRP_MIN_NUM_BRUSH) {
        return 0; //no-inst
    }

    ret = fine_make_group3(ENUM_HOR_X, initial_line_x_cnt, ENUM_VER_Y, fineLoop);
    if (ret == FINE_MEM_ERROR) {
        return FINE_MEM_ERROR;
    }
    if (ret != FINE_GROUP_FOUND) {
        return 0; //no-grp
    }

    grpX = initial_line_x[initial_line_x_cnt].grp;
    grpX_cnt = initial_line_x[initial_line_x_cnt].grpCnt;
    if (grpX_cnt <= 0) return 0; //no-grp

    grpX_max_idx = fine_get_maxlen_grp(grpX, grpX_cnt);
#if (DEBUG_BS_fine_add_remained_touch_brush > 0) && defined(DRAW_POLYGON_TEST)
    minIdx = grpX[grpX_max_idx].instIdx[0];
    maxIdx = grpX[grpX_max_idx].instIdx[grpX[grpX_max_idx].len - 1];
    DEBUG_SHOW_MIN_MAX_POS(ENUM_HOR_X, &initial_line_x[initial_line_x_cnt].inst[0], minIdx, maxIdx, 1);
#endif
    fine_make_grp_min_max(ENUM_HOR_X, grpX, grpX_max_idx, initial_line_x[initial_line_x_cnt].inst);
    minMaxAll.minY = grpX[grpX_max_idx].instPos.min;
    minMaxAll.maxY = grpX[grpX_max_idx].instPos.max;
    p2.x = BS_sensor_zero_x;
    p3.x = BS_sensor_end_x;
    p2.y = minMaxAll.minY;
    p3.y = minMaxAll.maxY;
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };

    //get x-grp
    initial_line_y = &BS_initial_line_y[0];
    initial_line_y_cnt = fine_get_inst3(ENUM_VER_Y, p2, p3, ENUM_HOR_X, &BS_remained_x[0], BS_remained_x_cnt, 0);
    if (initial_line_y_cnt == FINE_MEM_ERROR) {
        return FINE_MEM_ERROR;
    }
    if (initial_line_y[initial_line_y_cnt].sortLen < FINE_INITIAL_GRP_MIN_NUM_BRUSH) {
        return 0; //no-inst
    }

    ret = fine_make_group3(ENUM_VER_Y, initial_line_y_cnt, ENUM_HOR_X, fineLoop);
    if (ret == FINE_MEM_ERROR) {
        return FINE_MEM_ERROR;
    }
    if (ret != FINE_GROUP_FOUND) {
        return 0; //no-grp
    }

    grpY = initial_line_y[initial_line_y_cnt].grp;
    grpY_cnt = initial_line_y[initial_line_y_cnt].grpCnt;
    if (grpY_cnt <= 0) return 0; //no-grp

    grpY_max_idx = fine_get_maxlen_grp(grpY, grpY_cnt);
#if (DEBUG_BS_fine_add_remained_touch_brush > 0) && defined(DRAW_POLYGON_TEST)
    minIdx = grpY[grpY_max_idx].instIdx[0];
    maxIdx = grpY[grpY_max_idx].instIdx[grpY[grpY_max_idx].len - 1];
    DEBUG_SHOW_MIN_MAX_POS(ENUM_VER_Y, &initial_line_y[initial_line_y_cnt].inst[0], minIdx, maxIdx, 1);
#endif
    fine_make_grp_min_max(ENUM_VER_Y, grpY, grpY_max_idx, initial_line_y[initial_line_y_cnt].inst);
    minMaxAll.minX = grpY[grpY_max_idx].instPos.min;
    minMaxAll.maxX = grpY[grpY_max_idx].instPos.max;
    if (fine_is_overlap_tp_min_max(&minMaxAll, 0, touchCntOrg)) {
        return 0; //overlapped;
    }
#define BRUSH_MIN_SIZE  0.1f
    if (touchCntFine < MAX_TOUCH_LIMIT_FINE && touchCnt < ALLOWABLE_TOUCH_BE) {
        vec_t centerPoint, touchWidth;
        centerPoint.x = (minMaxAll.minX + minMaxAll.maxX) * 0.5f;
        centerPoint.y = (minMaxAll.minY + minMaxAll.maxY) * 0.5f;
        touchWidth.x = GET_MAX(minMaxAll.maxX - minMaxAll.minX, BRUSH_MIN_SIZE);
        touchWidth.y = GET_MAX(minMaxAll.maxY - minMaxAll.minY, BRUSH_MIN_SIZE);
        touchArea = touchWidth.x * touchWidth.y;

#if 1 //nsmoon@200630
        if (IS_NOT_ACTIVE_AREA(centerPoint.x, centerPoint.y)) {
            IS_DEBUG_FLAG { TRACE("BS_fine_add_remained_touch_brush..out of black area: %0.1f %0.1f", centerPoint.x, centerPoint.y); };
            return 0; //not-found
        }
#endif

        BG_touch_data[touchCnt] = centerPoint;
        BG_touch_area[touchCnt] = touchArea;
        BG_touch_size[touchCnt].xSize = touchWidth.x;
        BG_touch_size[touchCnt].ySize = touchWidth.y;
        BG_multi_fine[touchCnt] = 0; //for debugging

#ifdef FRONTEND_LINE_THRESHOLD //nsmoon@191226
        lineSenX = initial_line_x[initial_line_x_cnt].lineSen;
        lineSenY = initial_line_y[initial_line_y_cnt].lineSen;
        fine_getThresholdCnt(ENUM_VER_Y, &minMaxAll, &grpX[grpX_max_idx], &lineSenX[grpX_max_idx], &thCnt); //must be use grp[] with opp axis
        fine_getThresholdCnt(ENUM_HOR_X, &minMaxAll, &grpY[grpY_max_idx], &lineSenY[grpY_max_idx], &thCnt); //must be use grp[] with opp axis
        BG_touch_lineCnt[touchCnt] = thCnt;
        IS_DEBUG_FLAG{ TRACE("X:th10Cnt,th50Cnt=%d,%d", thCnt.th10CntX, thCnt.th50CntX); };
        IS_DEBUG_FLAG{ TRACE("Y:th10Cnt,th50Cnt=%d,%d", thCnt.th10CntY, thCnt.th50CntY); };
#endif
#if (DEBUG_BS_fine_add_remained_touch_brush > 0)
        DEBUG_SHOW_MIN_MAX(&minMaxAll, MY_COLOR, 1);
        TRACE_NOP;
#endif
        touchCnt++; touchCntFine++;
    }
    else {
        TRACE_ERROR_MEM("ERROR_MEM! invalid touchCnt,touchCntFine.3. %d %d", touchCnt, touchCntFine);
        return FINE_MEM_ERROR;
    }
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };

    int touchCntLocal = touchCntFine - touchCntFineOrg;
    BG_touch_count_fine = touchCntFine;
    BG_touch_count = touchCnt;
    return (touchCntLocal);
}

static int fine_get_inst_slope(axis_t axis1, axis_t axis2, uint16_t *remLine2, int remLineCnt2, axis_t axis3)
{
    int inBufIdxPl, inBufIdxMi;
    int pdPl, ledPl, pdMi, ledMi;
    vec_t p0, p1, p2, p3, pR;
    vec_t *inst;
    int instCnt, sortCnt;
    uint8_t *sort;
    in_line_t *lineSen;
    int remLineIdxPl, remLineIdxMi;
    initial_line_t *initial_line;
    int initial_line_cnt, initialLineMax;
    int slopeValPl, slopeValMi, slopeVal_limitPl, slopeVal_limitMi;
    int slopeValMaxPl, slopeValMaxMi, slopeValMax;
    uint16_t *remainedSort = (uint16_t *)&BS_minmax_inst_stat[0];
    int remLineCntMi;
    int i, j, instIdx_i, instIdx_j;
    float inst_i, inst_j;
    int cnt;
    if (remLineCnt2 > UINT16_SIZE) {
        TRACE_ERROR("ERROR! fine_get_inst_slope..invalid remLineCnt2 %d", remLineCnt2);
        remLineCnt2 = UINT16_SIZE;
    }

    if (axis1 == ENUM_HOR_X) {
        initial_line = &BS_initial_line_x[0];
        initial_line_cnt = BS_initial_line_x_cnt;
        initialLineMax = MAX_INITIAL_LINE_FINE_X;
    }
    else {
        //(axis1 == ENUM_VER_Y)
        initial_line = &BS_initial_line_y[0];
        initial_line_cnt = BS_initial_line_y_cnt;
        initialLineMax = MAX_INITIAL_LINE_FINE_Y;
    }
    if (axis2 == ENUM_HOR_X) {
        slopeValMax = BS_slopeValMaxX;
    }
    else {
        //(axis2 == ENUM_VER_Y)
        slopeValMax = BS_slopeValMaxY;
    }

    IS_DEBUG_FLAG{
        //SEND_POLY(0, 0x21, 0);
        TRACE_NOP;
    };

    //assign inst buf
    if (initial_line_cnt < initialLineMax) {
        initial_line[initial_line_cnt].inst = &BS_inst_xy[BS_inst_xy_cnt];
        initial_line[initial_line_cnt].sort = &BS_inst_xy_sort[BS_inst_xy_cnt];
        initial_line[initial_line_cnt].lineSen = &BS_inst_xy_sen[BS_inst_xy_cnt];
        inst = initial_line[initial_line_cnt].inst;
        sort = initial_line[initial_line_cnt].sort;
        lineSen = initial_line[initial_line_cnt].lineSen;
        initial_line[initial_line_cnt].instLen = 0; //reset
        initial_line[initial_line_cnt].sortLen = 0; //reset
        if (axis1 == ENUM_HOR_X) {
            BS_initial_line_x_cnt++;
        }
        else {
            BS_initial_line_y_cnt++;
        }
    }
    else {
        TRACE_ERROR_MEM("ERROR_MEM! fine_get_inst3.2.invalid initial_line_cnt: (%d)%d", axis1, initial_line_cnt);
        return FINE_MEM_ERROR; //mem-error
    }

    //fined min slope line
    slopeValMaxPl = -1;
    slopeValMaxMi = -1;
    for (i = 0; i < remLineCnt2; i++) {
        inBufIdxMi = remLine2[i];
        if (BS_getSenInBuf(axis2, inBufIdxMi, &pdMi, &ledMi)) {
            continue;
        }
        slopeValMi = ledMi - pdMi;
        if (slopeValMi < 0) {
            if (GET_ABS(slopeValMi) > slopeValMaxMi) {
                slopeValMaxMi = GET_ABS(slopeValMi);
            }
        }
        else if (slopeValMi > 0) {
            if (GET_ABS(slopeValMi) > slopeValMaxPl) {
                slopeValMaxPl = GET_ABS(slopeValMi);
            }
        }
        if (slopeValMaxMi == slopeValMax && slopeValMaxPl == slopeValMax) {
            break;
        }
    }
#if (DEBUG_fine_get_center_cross_lines_remainedsort > 1)
    IS_DEBUG_FLAG{
        TRACE_FGCCLRS("slopeValMin= %d (%d)", slopeValMin, slopeValMinIdx);
    for (i = 0; i < NUM_OF_SLOPE_MAX; i++) {
        TRACE_FGCCLRS("slopePlus= %d (%d)", BS_slopePlus[i], BS_slopePlusIdx[i]);
    }
    for (i = 0; i < NUM_OF_SLOPE_MAX; i++) {
        TRACE_FGCCLRS("slopeMinus= %d (%d)", BS_slopeMinus[i], BS_slopeMinusIdx[i]);
    }
    };
#endif
    if (slopeValMaxMi < 0 || slopeValMaxPl < 0) {
        TRACE_FGCCLRS("slopeValMaxMi,slopeValMaxPl= %d %d", slopeValMaxMi, slopeValMaxPl);
        return 0; //not found
    }

//#define MAX_REMAINED_LINE_BRUSH 10
#define MAX_SLOPE_NUM_BRUSH     10
#define MIN_SLOPE_NUM_BRUSH     3
#define MAX_INITLINE_NUM_BRUSH  5
#define MAX_INST_NUM_BRUSH      (MAX_INITLINE_NUM_BRUSH * MAX_INITLINE_NUM_BRUSH)
    if (slopeValMaxPl == slopeValMax) {
        slopeVal_limitPl = slopeValMaxPl - MAX_SLOPE_NUM_BRUSH;
    }
    else {
        slopeVal_limitPl = MIN_SLOPE_NUM_BRUSH;
    }

    if (slopeValMaxMi == slopeValMax) {
        slopeVal_limitMi = slopeValMaxMi - MAX_SLOPE_NUM_BRUSH;
    }
    else {
        slopeVal_limitMi = MIN_SLOPE_NUM_BRUSH;
    }

    //get minus slope remained lines
    cnt = remLineCntMi = 0;
    for (i = 0; i < remLineCnt2; i++)
    {
        inBufIdxMi = remLine2[i];
        if (BS_getSenInBuf(axis2, inBufIdxMi, &pdMi, &ledMi) < 0) {
            TRACE_ERROR("ERROR! DEBUG_show_line_idx_..BS_getSenInBuf() [%d]", BG_frame_no);
            continue;
        }
        slopeValMi = ledMi - pdMi;
        if (slopeValMi > 0 || GET_ABS(slopeValMi) < slopeVal_limitMi) {
            continue;
        }
        if (cnt < MAX_INITLINE_NUM_BRUSH && cnt < MAX_MINMAX_INST_TBL) {
            remainedSort[cnt++] = (uint16_t)i;
        }
    }
    remLineCntMi = cnt;

    cnt = instCnt = 0; //reset
    for (remLineIdxPl = 0; remLineIdxPl < remLineCnt2; remLineIdxPl++)
    {
        inBufIdxPl = remLine2[remLineIdxPl];
        if (BS_getSenInBuf(axis2, inBufIdxPl, &pdPl, &ledPl) < 0) {
            TRACE_ERROR("ERROR! DEBUG_show_line_idx_..BS_getSenInBuf() [%d]", BG_frame_no);
            continue;
        }
        slopeValPl = ledPl - pdPl;
        if (slopeValPl < 0 || GET_ABS(slopeValPl) < slopeVal_limitPl) {
            continue;
        }

        BS_getLinePosInBufSen(axis2, ledPl, pdPl, &p0, &p1);
    #if (DEBUG_BS_fine_add_remained_touch_brush > 1)
        IS_DEBUG_FLAG{
        DEBUG_SHOW_LINE_POS(&p0, &p1, 0);
            TRACE_NOP;
        };
    #endif
        for (i = 0; i < remLineCntMi; i++)
        {
            remLineIdxMi = remainedSort[i];
            inBufIdxMi = remLine2[remLineIdxMi];
            if (BS_getSenInBuf(axis2, inBufIdxMi, &pdMi, &ledMi) < 0) {
                TRACE_ERROR("ERROR! DEBUG_show_line_idx_..BS_getSenInBuf() [%d]", BG_frame_no);
                continue;
            }
            BS_getLinePosInBufSen(axis2, ledMi, pdMi, &p2, &p3);
    #if (DEBUG_BS_fine_add_remained_touch_brush > 1)
            IS_DEBUG_FLAG{
            DEBUG_SHOW_LINE_POS(&p2, &p3, 1);
                TRACE_NOP;
            }
    #endif

            pR.x = -1; pR.y = -1;
            if (BS_line_intersection(&p0, &p1, &p2, &p3, &pR)) {
    #if (DEBUG_BS_fine_add_remained_touch_brush > 0)
                IS_DEBUG_FLAG{
                DEBUG_SHOW_POS(&pR, 0.1f, 0.1f, MY_COLOR - 4); //gray
                }
    #endif
                if (cnt++ > MAX_INST_NUM_BRUSH) break;
                if ((BS_inst_xy_cnt + instCnt) < MAX_NUM_INST_FINE) {
                    if (instCnt < UINT8_SIZE) {
                        inst[instCnt] = pR;
                        lineSen[instCnt].led = (uint8_t)ledPl;
                        lineSen[instCnt].pd = (uint8_t)pdPl;
                        instCnt++;
                    }
                    else {
                        TRACE_ERROR("ERROR! fine_get_inst_slope..invalid instCnt %d", instCnt);
                    }
                }
                else {
                    TRACE_ERROR_MEM("ERROR_MEM! fine_get_inst3..invalid instCnt: (%d)%d", axis1, (BS_inst_xy_cnt + instCnt));
                    return FINE_MEM_ERROR; //mem-error
                }
            }
            else {
                //TRACE_ERROR("ERROR! fine_get_inst3..no inst");
                TRACE_NOP;
            }
        } //for (remLineIdxMi = 0; remLineIdxMi < remLineCnt2; remLineIdxMi++)
        if (cnt++ > MAX_INST_NUM_BRUSH) break;
    } //for (remLineIdxPl = 0; remLineIdxPl < remLineCnt2; remLineIdxPl++)
    if (instCnt < FINE_INITIAL_GRP_MIN_NUM_BRUSH) {
        TRACE_NOP;
        return 0;
    }

    //save instLen
    initial_line[initial_line_cnt].instLen = instCnt; //num of instersect
#if 0 //for test
    if (axis1 == ENUM_VER_Y && mode == 0) {
        //just check
        if (GET_ABS(initial_line[initialLineIdx].cent.y - inst[0].y) > EPSILON) {
            TRACE_ERROR("ERROR! invalid initial_line[].cent.y and inst[].y: %0.2f, %0.2f", initial_line[initialLineIdx].cent.y, inst[0].y);
        }
    }
#endif

    ///////////////////////////////////////
    //make sort table
    sortCnt = 0;
    for (i = 0; i < instCnt; i++) {
        sort[sortCnt++] = (uint8_t)i; //nsmoon@190527
    }
    //save sortCnt
    initial_line[initial_line_cnt].sortLen = sortCnt; //num of instersect
    if (sortCnt != instCnt) {
        TRACE_ERROR_MEM("ERROR_MEM! invalid sortLen and instLen: %d, %d", sortCnt, instCnt);
        return FINE_MEM_ERROR; //mem-error
    }
#if 0 //(DEBUG_fine_get_inst > 1) //for testing
    for (i = 0; i < sortCnt; i++) {
        TRACENR("%d ", sort[i]);
        if ((i + 1) % 20 == 0) TRACENR("\r\n");
    }
    TRACE("=>before(%d, %d)", axis2, initial_line_cnt);
    for (i = 0; i < sortCnt; i++) {
        inst_i = (axis3 == ENUM_HOR_X) ? inst[sort[i]].x : inst[sort[i]].y;
        TRACENR("%0.1f ", inst_i);
        if ((i + 1) % 20 == 0) TRACENR("\r\n");
    }
    TRACE("=>before(%d, %d)", axis2, initial_line_cnt);
#endif

    //for (i = 0; i < instCnt - 1; i++)
    for (i = 0; i < sortCnt - 1; i++)
    {
        //for (j = (i + 1); j < instCnt; j++)
        for (j = (i + 1); j < sortCnt; j++)
        {
            instIdx_i = sort[i];
            instIdx_j = sort[j];
            inst_i = (axis3 == ENUM_HOR_X) ? inst[instIdx_i].x : inst[instIdx_i].y;
            inst_j = (axis3 == ENUM_HOR_X) ? inst[instIdx_j].x : inst[instIdx_j].y;
            if (inst_i > inst_j)
            {
                sort[i] = (uint8_t)instIdx_j;
                sort[j] = (uint8_t)instIdx_i;
            }
        }
    }
#if 0 //(DEBUG_fine_get_inst > 1) //for testing
    IS_DEBUG_FLAG
    {
        for (i = 0; i < sortCnt; i++) {
            TRACENR("%d ", sort[i]);
            if ((i + 1) % 20 == 0) TRACENR("\r\n");
        }
    TRACE("=>after idx(%d, %d, %d)", axis2, initial_line_cnt, sortCnt);
    for (i = 0; i < sortCnt; i++) {
        inst_i = (axis3 == ENUM_HOR_X) ? inst[sort[i]].x : inst[sort[i]].y;
        TRACENR("%0.1f ", inst_i);
        if ((i + 1) % 20 == 0) TRACENR("\r\n");
    }
    TRACE("=>after pos(%d, %d, %d)", axis2, initial_line_cnt, sortCnt);
    };
#endif
    TRACE_FGI("initial_line[].inst:2: %d %d 0x%x", initial_line_cnt, instCnt, initial_line[initial_line_cnt].inst);
    if ((BS_inst_xy_cnt + instCnt) < MAX_NUM_INST_FINE) {
        BS_inst_xy_cnt += instCnt;
    }
    else {
        TRACE_ERROR("ERROR! fine_get_inst.2.invalid instCnt: (%d)%d %d %d", axis1, instCnt, BS_inst_xy_cnt, initial_line_cnt);
        return FINE_MEM_ERROR; //mem-error
    }
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };
    return initial_line_cnt; //no-error
}

int BS_fine_add_remained_touch_brush2(int fineLoop)
{
    float touchArea;
    initial_line_t *initial_line;
    int initial_line_cnt;
    initial_line_group_t *grp;
    int grp_cnt;
    int grp_max_idx;
    in_line_t *lineSen;
    int touchCntOrg = BG_touch_count;
    int touchCntFineOrg = BG_touch_count_fine;
    int touchCnt = touchCntOrg;
    int touchCntFine = touchCntFineOrg;
    pos_minMax2_t minMax;
    pos_min_max_t minMaxPos;
    vec_t retMinPos, retMaxPos;
    int ret;
    uint16_t *remained;
    int remained_cnt;
    axis_t axis1, axis2;
    vec_t centerPoint, touchWidth;
    float retWidth, cent0; //halfWidth
    vec_t p0, p1;
#if (DEBUG_BS_fine_add_remained_touch_brush > 0) && defined(DRAW_POLYGON_TEST)
    int minIdx, maxIdx;
#endif
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };

    tp_line_cnt_t thCnt;
    thCnt.th10CntX = thCnt.th50CntX = 0;
    thCnt.th10WidthX = 0;
    thCnt.th10CntY = thCnt.th50CntY = 0;
    thCnt.th10WidthY = 0;
    touchWidth.x = touchWidth.y = 0; //nsmoon@210310

    //reset mem
    fine_bs_mem_init();

    if (BS_remained_x_cnt > BS_remained_y_cnt) {
        axis1 = ENUM_VER_Y;
        initial_line = &BS_initial_line_y[0];
        axis2 = ENUM_HOR_X;
        remained = &BS_remained_x[0];
        remained_cnt = BS_remained_x_cnt;
    }
    else {
        axis1 = ENUM_HOR_X;
        initial_line = &BS_initial_line_x[0];
        axis2 = ENUM_VER_Y;
        remained = &BS_remained_y[0];
        remained_cnt = BS_remained_y_cnt;
    }

    initial_line_cnt = fine_get_inst_slope(axis1, axis2, remained, remained_cnt, axis1);
    if (initial_line_cnt == FINE_MEM_ERROR) {
        return FINE_MEM_ERROR;
    }
    if (initial_line[initial_line_cnt].sortLen < FINE_INITIAL_GRP_MIN_NUM_BRUSH) {
        return 0; //no-inst
    }

    ret = fine_make_group3(axis1, initial_line_cnt, axis1, fineLoop);
    if (ret == FINE_MEM_ERROR) {
        return FINE_MEM_ERROR;
    }
    if (ret != FINE_GROUP_FOUND) {
        return 0; //no-grp
    }

    grp = initial_line[initial_line_cnt].grp;
    grp_cnt = initial_line[initial_line_cnt].grpCnt;
    if (grp_cnt <= 0) return 0; //no-grp

    grp_max_idx = fine_get_maxlen_grp(grp, grp_cnt);
#if (DEBUG_BS_fine_add_remained_touch_brush > 0) && defined(DRAW_POLYGON_TEST)
    minIdx = grp[grp_max_idx].instIdx[0];
    maxIdx = grp[grp_max_idx].instIdx[grp[grp_max_idx].len - 1];
    DEBUG_SHOW_MIN_MAX_POS(axis1, &initial_line[initial_line_cnt].inst[0], minIdx, maxIdx, 1);
#endif
    fine_make_grp_min_max(axis1, grp, grp_max_idx, initial_line[initial_line_cnt].inst);

#define MAX_INST_WIDTH_BRUSH    20.0f
#define MAX_MINMAX_WIDTH_BRUSH  10.0f
    if (axis1 == ENUM_HOR_X) {
        p0.x = (grp[grp_max_idx].instPos2.min + grp[grp_max_idx].instPos2.max) * 0.5f;
        cent0 = p1.x = p0.x;
        p0.y = (grp[grp_max_idx].instPos.min + grp[grp_max_idx].instPos.max) * 0.5f;
        p1.y = p0.y + MAX_INST_WIDTH_BRUSH;
        p0.y = p0.y - MAX_INST_WIDTH_BRUSH;
    }
    else {
        p0.y = (grp[grp_max_idx].instPos2.min + grp[grp_max_idx].instPos2.max) * 0.5f;
        cent0 = p1.y = p0.y;
        p0.x = (grp[grp_max_idx].instPos.min + grp[grp_max_idx].instPos.max) * 0.5f;
        p1.x = p0.x + MAX_INST_WIDTH_BRUSH;
        p0.x = p0.x - MAX_INST_WIDTH_BRUSH;
    }
#if (DEBUG_BS_fine_add_remained_touch_brush > 0)
    DEBUG_SHOW_LINE_POS(&p0, &p1, 0);
#endif

    initial_line_cnt = fine_get_inst3(axis1, p0, p1, axis2, remained, remained_cnt, 0);
    if (initial_line_cnt == FINE_MEM_ERROR) {
        return FINE_MEM_ERROR;
    }
    if (initial_line[initial_line_cnt].sortLen < FINE_INITIAL_GRP_MIN_NUM_BRUSH) {
        return 0; //no-inst
    }

    ret = fine_make_group3(axis1, initial_line_cnt, axis2, fineLoop);
    if (ret == FINE_MEM_ERROR) {
        return FINE_MEM_ERROR;
    }
    if (ret != FINE_GROUP_FOUND) {
        return 0; //no-grp
    }

    grp = initial_line[initial_line_cnt].grp;
    grp_cnt = initial_line[initial_line_cnt].grpCnt;
    if (grp_cnt <= 0) return 0; //no-grp

    grp_max_idx = fine_get_maxlen_grp(grp, grp_cnt);
#if (DEBUG_BS_fine_add_remained_touch_brush > 0) && defined(DRAW_POLYGON_TEST)
    minIdx = grp[grp_max_idx].instIdx[0];
    maxIdx = grp[grp_max_idx].instIdx[grp[grp_max_idx].len - 1];
    DEBUG_SHOW_MIN_MAX_POS(axis2, &initial_line[initial_line_cnt].inst[0], minIdx, maxIdx, 1);
#endif
    fine_make_grp_min_max(axis1, grp, grp_max_idx, initial_line[initial_line_cnt].inst);
    minMax.min = grp[grp_max_idx].instPos.min;
    minMax.max = grp[grp_max_idx].instPos.max;
    retMinPos.x = 0; retMaxPos.x = 0;
    retMinPos.y = 0; retMaxPos.y = 0;

#ifdef FINE_MIN_CAL_SIZE //nsmoon@210305
    int isEdgeArea = initial_line[initial_line_cnt].isEdgeArea;
    float width = GET_ABS(minMax.max - minMax.min);
    if (width > FINE_MIN_CAL_SIZE && isEdgeArea == 0)
#endif
    {
    ret = fine_cal_min_max3(axis1, initial_line_cnt, grp_max_idx, axis2, &minMax, &retMinPos, &retMaxPos, 1, fineLoop); //1: check slope
    IS_DEBUG_FLAG{ TRACE_FATD(".min_max=(%d)(%0.1f,%0.1f)(%0.1f,%0.1f)=>(%0.1f,%0.1f) (%0.1f,%0.1f)(%0.1f,%0.1f)", ret, grp[grp_max_idx].instPos.min, grp[grp_max_idx].instPos.max, grp[grp_max_idx].instPos2.min, grp[grp_max_idx].instPos2.max, minMax.min, minMax.max, retMinPos.x, retMaxPos.x, retMinPos.y, retMaxPos.y); };
    if (ret) {
        IS_DEBUG_FLAG{ TRACE_FATD("no +/- slope~~~");};
        return 0; //not-found
    }
    else {
#define BRUSH_MIN_SIZE  0.1f
        //update min/.max
        if(axis2 == ENUM_HOR_X) {
            minMaxPos.minX = minMax.min;
            minMaxPos.maxX = minMax.max;
            touchWidth.x = GET_MAX(minMaxPos.maxX - minMaxPos.minX, BRUSH_MIN_SIZE);
            centerPoint.x = (minMaxPos.minX + minMaxPos.maxX) * 0.5f;
            //halfWidth = touchWidth.x * 0.5f;
            retWidth = GET_ABS(retMaxPos.y - retMinPos.y);
            TRACE(".retWidth= %0.1f", retWidth);
            if (retWidth > MAX_MINMAX_WIDTH_BRUSH) {
                centerPoint.y = cent0;
            }
            else {
                centerPoint.y = (retMinPos.y + retMaxPos.y) * 0.5f;
            }
            minMaxPos.minY = centerPoint.y - BRUSH_MIN_SIZE;
            minMaxPos.maxY = centerPoint.y + BRUSH_MIN_SIZE;
            touchWidth.y = GET_MAX(minMaxPos.maxY - minMaxPos.minY, BRUSH_MIN_SIZE);
        }
        else {
            minMaxPos.minY = minMax.min;
            minMaxPos.maxY = minMax.max;
            touchWidth.y = GET_MAX(minMaxPos.maxY - minMaxPos.minY, BRUSH_MIN_SIZE);
            centerPoint.y = (minMaxPos.minY + minMaxPos.maxY) * 0.5f;
            //halfWidth = touchWidth.y * 0.5f;
            retWidth = GET_ABS(retMaxPos.x - retMinPos.x);
            TRACE("..retWidth= %0.1f", retWidth);
            if (retWidth > MAX_MINMAX_WIDTH_BRUSH) {
                centerPoint.x = cent0;
            }
            else {
                centerPoint.x = (retMinPos.x + retMaxPos.x) * 0.5f;
            }
            minMaxPos.minX = centerPoint.x - BRUSH_MIN_SIZE;
            minMaxPos.maxX = centerPoint.x + BRUSH_MIN_SIZE;
            touchWidth.x = GET_MAX(minMaxPos.maxX - minMaxPos.minX, BRUSH_MIN_SIZE);
        }
    }
    }
#if 1 //nsmoon@200928
    if (IS_NEAR_BLACK_AREA(centerPoint.x, centerPoint.y, FINE_EDGE_DIST_FROM_TP)) {
        IS_DEBUG_FLAG { TRACE("BS_fine_add_remained_touch_brush2..out of black area: %0.1f %0.1f", centerPoint.x, centerPoint.y); };
        return 0; //not-found
    }
#endif

    if (touchCntFine < MAX_TOUCH_LIMIT_FINE && touchCnt < ALLOWABLE_TOUCH_BE) {
        touchArea = touchWidth.x * touchWidth.y;
        BG_touch_data[touchCnt] = centerPoint;
        BG_touch_area[touchCnt] = touchArea;
        BG_touch_size[touchCnt].xSize = touchWidth.x;
        BG_touch_size[touchCnt].ySize = touchWidth.y;
        BG_multi_fine[touchCnt] = 0; //for debugging

#ifdef FRONTEND_LINE_THRESHOLD //nsmoon@191226
        lineSen = initial_line[initial_line_cnt].lineSen;
        fine_getThresholdCnt(axis2, &minMaxPos, &grp[grp_max_idx], &lineSen[grp_max_idx], &thCnt); //must be use grp[] with opp axis
        BG_touch_lineCnt[touchCnt] = thCnt;
#if 0 //def DUPLICATE_TH10 //nsmoon@200511
        if (axis2 == ENUM_HOR_X) {
            thCnt.th10CntY = thCnt.th10CntX;
            thCnt.th50CntY = thCnt.th50CntX;
            thCnt.th10WidthY = touchWidth.x;
        }
        else { //(axis2 == ENUM_VER_Y)
            thCnt.th10CntX = thCnt.th10CntY;
            thCnt.th50CntX = thCnt.th50CntY;
            thCnt.th10WidthX = touchWidth.y;
        }
#endif
        IS_DEBUG_FLAG{ TRACE("X:th10Cnt,th50Cnt=%d,%d", thCnt.th10CntX, thCnt.th50CntX); };
        IS_DEBUG_FLAG{ TRACE("Y:th10Cnt,th50Cnt=%d,%d", thCnt.th10CntY, thCnt.th50CntY); };
#endif
#if (DEBUG_BS_fine_add_remained_touch_brush > 0)
        DEBUG_SHOW_MIN_MAX(&minMaxPos, MY_COLOR, 1);
        TRACE_NOP;
#endif
        touchCnt++; touchCntFine++;
    }
    else {
        TRACE_ERROR_MEM("ERROR_MEM! invalid touchCnt,touchCntFine.3. %d %d", touchCnt, touchCntFine);
        return FINE_MEM_ERROR;
    }
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };

    int touchCntLocal = touchCntFine - touchCntFineOrg;
    BG_touch_count_fine = touchCntFine;
    BG_touch_count = touchCnt;
    return (touchCntLocal);
}
#endif //1

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_BG_clipping_fine5   1
#endif
#if (DEBUG_BG_clipping_fine5 > 0)
#define TRACE_BCF5(...)     TRACE(__VA_ARGS__)
#define TRACE_BCF52(...)    TRACE(__VA_ARGS__)
#else
#define TRACE_BCF5(...) 
#define TRACE_BCF52(...)
#endif

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_fine_get_initial_tp    2
#endif
#if (DEBUG_fine_get_initial_tp > 0)
#define TRACE_FGIT(...)    TRACE(__VA_ARGS__)
#else
#define TRACE_FGIT(...)
#endif
#if (DEBUG_fine_get_initial_tp > 1)
#define TRACE_FGIT2(...)   TRACE(__VA_ARGS__)
#else
#define TRACE_FGIT2(...)
#endif

#define FINE_COORD3_LINE_CNT    2
static int fine_get_initial_tp(int fineLoop)
{
    int ret;
    uint16_t *remLineX = &BS_remained_x[0];
    int remLineCntX = BS_remained_x_cnt;
    uint16_t *remLineY = &BS_remained_y[0];
    int remLineCntY = BS_remained_y_cnt;
    IS_DEBUG_FLAG{
        TRACE_NOP;
    }

    fine_bs_mem_init();
    fine_get_clipping_info(ENUM_HOR_X);
    fine_get_clipping_info(ENUM_VER_Y);

    ret = BS_fine_get_initial_ep5(ENUM_HOR_X, remLineX, &remLineCntX, FINE_INIT_EP5_MIN_SLOPE);
    if (ret || BS_initial_line_a_x_cnt <= 0) {
        if (ret == FINE_MEM_ERROR) {
        TRACE_ERROR("ERROR! fine_get_initial_tp..0 mem over flow");
            return ret; //mem error
    }
        IS_DEBUG_FLAG{TRACE("ERROR fine_get_initial_tp..no BS_initial_line_a_x_cnt (%d)", ret);};
        return 1; //error
    }

    ret = BS_fine_get_initial_ep5(ENUM_VER_Y, remLineY, &remLineCntY, FINE_INIT_EP5_MIN_SLOPE);
    if (ret || BS_initial_line_a_y_cnt <= 0) {
        if (ret == FINE_MEM_ERROR) {
        TRACE_ERROR("ERROR! BS_fine_get_initial_ep5..0 mem over flow");
            return ret; //mem error
    }
        IS_DEBUG_FLAG{TRACE("ERROR fine_get_initial_tp..no BS_initial_line_a_y_cnt (%d)", ret);};
        return 1; //error
    }
    IS_DEBUG_FLAG {
        TRACE("BS_initial_line_a_x_cnt= %d, %d (%d)", BS_initial_line_a_x_cnt, BS_initial_line_a_y_cnt, FINE_INIT_EP5_MIN_SLOPE);
    };

    fine_get_initial_ep5_remained(ENUM_HOR_X, remLineX, &remLineCntX); //nsmoon@220126
    fine_get_initial_ep5_remained(ENUM_VER_Y, remLineY, &remLineCntY); //nsmoon@220126
    IS_DEBUG_FLAG {
        TRACE("..BS_initial_line_a_x_cnt= %d %d", BS_initial_line_a_x_cnt, BS_initial_line_a_y_cnt);
        if (BS_initial_line_a_x_cnt != BS_initial_line_a_y_cnt) {
            TRACE_NOP;
        }
    };
#ifdef FINE_INITIAL_LINE_CENT //nsmoon@230418
    BS_fine_get_initial_ep5_cent();
    IS_DEBUG_FLAG {
        TRACE("...BS_initial_line_a_x_cnt=%d, %d", BS_initial_line_a_x_cnt, BS_initial_line_a_y_cnt);
    }
#endif

    return 0; //no-error
}

#if (DEBUG_fine_get_initial_ep > 0) //nsmoon@230523
static void DEBUG_fine_show_initial_line(void)
{
    initial_line_a_t *initial_line;
    int axis, pd, led;
    int line_idx, initial_line_cnt;
    vec_t p0, p1;

    for (axis = 0; axis < (int)ENUM_AXIS_END; axis++) {
        if (axis == (int)ENUM_HOR_X) {
            initial_line = &BS_initial_line_a_x[0];
            initial_line_cnt = BS_initial_line_a_x_cnt;
        }
        else {
            initial_line = &BS_initial_line_a_y[0];
            initial_line_cnt = BS_initial_line_a_y_cnt;
        }
        for (line_idx = 0; line_idx < initial_line_cnt; line_idx++)
        {
            pd = initial_line[line_idx].line.pd;
            led = initial_line[line_idx].line.led;
            if (axis == (int)ENUM_HOR_X) {
                p0.x = BS_led_pos_x[led];
                p0.y = BS_sensor_zero_y;
                p1.x = BS_pd_pos_x[pd];
                p1.y = BS_sensor_end_y;
            }
            else {
                p0.y = BS_led_pos_y[led];
                p0.x = BS_sensor_end_x;
                p1.y = BS_pd_pos_y[pd];
                p1.x = BS_sensor_zero_x;
            }
            DEBUG_SHOW_LINE_POS(&p0, &p1, line_idx);
        }
    }
}
#endif

#if 1 //nsmoon@230425
static int fine_get_initial_cxp(void)
{
    initial_line_a_t *initial_line_x = &BS_initial_line_a_x[0];
    initial_line_a_t *initial_line_y = &BS_initial_line_a_y[0];
    int line_idx_x_y, line_idx_x, line_idx_y;
    float *pdXpos = &BS_pd_pos_x[0];
    float *ledXpos = &BS_led_pos_x[0];
    float *pdYpos = &BS_pd_pos_y[0];
    float *ledYpos = &BS_led_pos_y[0];
    vec_t p0, p1, p2, p3, pR;
    int pdX, ledX, pdY, ledY;
    IS_DEBUG_FLAG{
        TRACE_FGIT(" fine_get_initial_cxp.. %d %d", BS_initial_line_a_x_cnt, BS_initial_line_a_y_cnt);
        TRACE_NOP;
    }

    line_idx_x_y = 0;
    for (line_idx_x = 0; line_idx_x < BS_initial_line_a_x_cnt; line_idx_x++)
    {
        pdX = initial_line_x[line_idx_x].line.pd;
        ledX = initial_line_x[line_idx_x].line.led;
        p0.x = ledXpos[ledX];
        p0.y = BS_sensor_zero_y;
        p1.x = pdXpos[pdX];
        p1.y = BS_sensor_end_y;
#if (DEBUG_fine_get_initial_tp > 2) //nsmoon@210216
        DEBUG_SHOW_LINE_POS(&p0, &p1, line_idx_x);
#endif
        int cxpCntX = 0;
        initial_line_x[line_idx_x].cxpCntMax = 0; //init
        for (line_idx_y = 0; line_idx_y < BS_initial_line_a_y_cnt; line_idx_y++)
        {
            pdY = initial_line_y[line_idx_y].line.pd;
            ledY = initial_line_y[line_idx_y].line.led;
            p2.y = ledYpos[ledY];
            p2.x = BS_sensor_end_x;
            p3.y = pdYpos[pdY];
            p3.x = BS_sensor_zero_x;
#if (DEBUG_fine_get_initial_tp > 2) //nsmoon@210216
            DEBUG_SHOW_LINE_POS(&p2, &p3, line_idx_y);
#endif
            if (BS_line_intersection(&p0, &p1, &p2, &p3, &pR)) {
                if (cxpCntX < MAX_FINE_INITIAL_CXP_PER_LINE) {
                    if (line_idx_x_y < MAX_INITIAL_LINE_FINE_XY) { //nsmoon@230412 MAX_INITIAL_LINE_FINE_Y=>MAX_INITIAL_LINE_FINE_XY
                        IS_DEBUG_FLAG{ TRACE_FGIT(" cxpCent1=%d-%d/%d,%d,%0.2f %0.2f,%d-%d,%d-%d",
                                                   line_idx_x, line_idx_x_y, line_idx_y, cxpCntX, pR.x, pR.y, pdX, ledX, pdY, ledY); };
                        //if (BG_touch_count == 0 && fine_is_closed_initial_tp_cent(cxpCntX, &pR)) //nsmoon@200924, not-use
                        {
                            initial_line_x[line_idx_x].cxpCent[cxpCntX] = pR;
                            initial_line_x[line_idx_x].cxOppLine[cxpCntX] = (uint8_t)line_idx_x_y;
                            initial_line_x[line_idx_x].cxTcIdx[cxpCntX] = UINT8_MAX; //init
                            initial_line_x[line_idx_x].sortLen_a[cxpCntX] = 0; //init
                            initial_line_x[line_idx_x].score[cxpCntX] = 0; //init
                            initial_line_x[line_idx_x].score2[cxpCntX] = 0; //init
                            line_idx_x_y++;
                            cxpCntX++;
                        }
                    }
                    else {
                        TRACE_ERROR("ERROR! fine_get_initial_cxp..invalid line_idx_x_y %d", line_idx_x_y);
                        break;
                    }
                }
                else {
                    TRACE_ERROR("ERROR! fine_get_initial_cxp..invalid cxpCntX %d", cxpCntX);
                    break;
                }
            }
        } //for (line_idx_y = 0; line_idx_y < BS_initial_line_a_y_cnt; line_idx_y++)
        initial_line_x[line_idx_x].cxpCntMax = (uint8_t)cxpCntX;
    } //for (line_idx_x = 0; line_idx_x < BS_initial_line_a_x_cnt; line_idx_x++)

    //init BS_initial_line_a_y[]
    for (line_idx_y = 0; line_idx_y < line_idx_x_y; line_idx_y++)
    {
        initial_line_y[line_idx_y].cxpCntMax = 0; //init
        initial_line_y[line_idx_y].cxLineIdx = UINT8_MAX; //init
        initial_line_y[line_idx_y].cxGrpIdx = UINT8_MAX; //init
        initial_line_y[line_idx_y].sortLen_a[0] = 0; //init //nsmoon@201019-bugfix
    }

#if (DEBUG_fine_get_initial_ep > 0) //nsmoon@230523
    DEBUG_fine_show_initial_line();
#endif
    return line_idx_x_y;
}
#endif

static int chk_touch_info_cnt(int touch_info_fine_cnt)
{
    int i, cnt = 0;
    touch_info_fine_t *touch_info_fine = &BS_touch_info_fine[0];
    for (i = 0; i < touch_info_fine_cnt; i++)
    {
        if (touch_info_fine[i].fineStat == 1) continue;
        cnt++;
    }
    return cnt;
}

#ifdef FINE_REMOVE_GHOST_NEW //nsmoon@230613
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_fine_remove_ghost     1
#endif
#if (DEBUG_fine_remove_ghost > 0)
#define TRACE_FRG(...)    TRACE(__VA_ARGS__)
#else
#define TRACE_FRG(...)
#endif

#if 1 //nsmoon@230424
static void fine_get_one_touch_info(int touch_info_fine_cnt)
{
    int i, j;
    uint8_t lineIdxX0_i, lineIdxX0_j;
    uint8_t lineIdxY0_i, lineIdxY0_j; //nsmoon@230515
    touch_info_fine_t *touch_info_fine = &BS_touch_info_fine[0];
    for (i = 0; i < touch_info_fine_cnt; i++) {
        if (touch_info_fine[i].fineStat == 1) continue;
        touch_info_fine[i].tiXcnt = 0; //init
        touch_info_fine[i].tiYcnt = 0; //nsmoon@230515
    }
#if 0 //for test
    if (BS_initial_line_a_x_cnt != BS_initial_line_a_y_cnt) {
        return; //not-check tiXcnt
    }
#endif

    for (i = 0; i < touch_info_fine_cnt; i++) {
        if (touch_info_fine[i].fineStat == 1) continue;
        if (touch_info_fine[i].tiXcnt > 0) continue;

        lineIdxX0_i = touch_info_fine[i].lineIdxX0;
        lineIdxY0_i = touch_info_fine[i].lineIdxY0; //nsmoon@230515
        touch_info_fine[i].tiXcnt = 1;

        for (j = i+1; j < touch_info_fine_cnt; j++) {
            if (touch_info_fine[j].fineStat == 1) continue;
            lineIdxX0_j = touch_info_fine[j].lineIdxX0;
            if (lineIdxX0_i == lineIdxX0_j) {
                touch_info_fine[i].tiXcnt++;
            }
        }
        for (j = i+1; j < touch_info_fine_cnt; j++) {
            if (touch_info_fine[j].fineStat == 1) continue;
            lineIdxX0_j = touch_info_fine[j].lineIdxX0;
            if (lineIdxX0_i == lineIdxX0_j) {
                touch_info_fine[j].tiXcnt = touch_info_fine[i].tiXcnt;
            }
        }
    }

#if 1 //nsmoon@230515
    for (i = 0; i < touch_info_fine_cnt; i++) {
        if (touch_info_fine[i].fineStat == 1) continue;
        if (touch_info_fine[i].tiYcnt > 0) continue;

        lineIdxY0_i = touch_info_fine[i].lineIdxY0;
        touch_info_fine[i].tiYcnt = 1;

        for (j = i+1; j < touch_info_fine_cnt; j++) {
            if (touch_info_fine[j].fineStat == 1) continue;
            lineIdxY0_j = touch_info_fine[j].lineIdxY0;
            if (lineIdxY0_i == lineIdxY0_j) {
                touch_info_fine[i].tiYcnt++;
            }
        }
        for (j = i+1; j < touch_info_fine_cnt; j++) {
            if (touch_info_fine[j].fineStat == 1) continue;
            lineIdxY0_j = touch_info_fine[j].lineIdxY0;
            if (lineIdxY0_i == lineIdxY0_j) {
                touch_info_fine[j].tiYcnt = touch_info_fine[i].tiYcnt;
            }
        }
    }
#endif

#if (DEBUG_fine_remove_ghost > 0)
    IS_DEBUG_FLAG {
        for (i = 0; i < touch_info_fine_cnt; i++) {
            if (touch_info_fine[i].fineStat == 1) continue;
            uint8_t lineIdxX0 = touch_info_fine[i].lineIdxX0;
            uint8_t lineIdxXY = touch_info_fine[i].lineIdxXY;
            uint8_t lineIdxY0 = touch_info_fine[i].lineIdxY0;
            uint8_t tiXcnt = touch_info_fine[i].tiXcnt;
            uint8_t tiYcnt = touch_info_fine[i].tiYcnt;
            TRACE_FRG("  tiCnt=%d-%d/%d (%d) %d %d", (int)lineIdxX0, (int)lineIdxXY, (int)lineIdxY0, i, (int)tiXcnt, (int)tiYcnt);
        };
    }
#endif
}

static void fine_decrease_ti_cnt(int touch_info_fine_cnt, int touch_info_fine_idx)
{
    int i;
    touch_info_fine_t *touch_info_fine = &BS_touch_info_fine[0];
    uint8_t lineIdxX0, lineIdxX0_in = touch_info_fine[touch_info_fine_idx].lineIdxX0;
    uint8_t lineIdxY0, lineIdxY0_in = touch_info_fine[touch_info_fine_idx].lineIdxY0; //nsmoon@230515

    for (i = 0; i < touch_info_fine_cnt; i++)
    {
        if (touch_info_fine[i].fineStat == 1) continue;
        lineIdxX0 = touch_info_fine[i].lineIdxX0;
        if (lineIdxX0_in == lineIdxX0) {
            if (touch_info_fine[i].tiXcnt > 1) {
                touch_info_fine[i].tiXcnt--;
            }
        }
#if 1 //nsmoon@230515
        lineIdxY0 = touch_info_fine[i].lineIdxY0;
        if (lineIdxY0_in == lineIdxY0) {
            if (touch_info_fine[i].tiYcnt > 1) {
                touch_info_fine[i].tiYcnt--;
            }
        }
#endif
    }
}
#endif

static int fine_remove_ghost(int touch_info_fine_cnt, int mode)
{
    int i, j;
    vec_t cent_i, cent_j;
    int diffSum_i, diffSum_j;
    int maxCnt_i, maxCnt_j;
    int gridWidth_i, gridWidth_j;
    float distX, distY;
    touch_info_fine_t *touch_info_fine = &BS_touch_info_fine[0];
    float near_dist_x, near_dist_y;
    float densDiffSum_i, densDiffSum_j;
    float densDiffSum, densDiffSumMargin = 0;
    uint8_t lineIdxX0_i, lineIdxX0_j;
    uint8_t lineIdxY0_i, lineIdxY0_j;
    int tCnt = chk_touch_info_cnt(touch_info_fine_cnt);
    //int is_x_small = (BS_initial_line_a_y_cnt <= MAX_TOUCH_LIMIT_FINE && BS_initial_line_a_x_cnt < BS_initial_line_a_y_cnt);

    IS_DEBUG_FLAG {TRACE_FRG("fine_remove_ghost..%d %d/%d", mode, tCnt, touch_info_fine_cnt);};

    if (mode == FINE_GHOST_MODE_X) {
        near_dist_x = FINE_GHOST_NEAR_DIST;
        near_dist_y = BS_aarea_end_y;
        densDiffSumMargin = FINE_REMOVE_GHOST_DENSE_DIFF_X;
    }
    else if (mode == FINE_GHOST_MODE_Y) {
        near_dist_y = FINE_GHOST_NEAR_DIST;
        near_dist_x = BS_aarea_end_x;
        if (BS_initial_line_a_y_cnt <= MAX_TOUCH_LIMIT_FINE && BS_initial_line_a_x_cnt < BS_initial_line_a_y_cnt) {
            densDiffSumMargin = FINE_REMOVE_GHOST_DENSE_DIFF_Y;
        }
        else {
            densDiffSumMargin = EPSILON; //nsmoon@2304515
        }
    }
    else if (mode == FINE_GHOST_MODE_XY) {
        near_dist_x = FINE_GHOST_NEAR_DIST; //near_dist_sq
        near_dist_y = FINE_GHOST_NEAR_DIST;
        densDiffSumMargin = EPSILON;
    }
    else {
        TRACE_ERROR("ERROR! fine_remove_ghost..invalid mode %d", mode);
        return 0;
    }

    for (i = 0; i < touch_info_fine_cnt; i++)
    {
        if (touch_info_fine[i].fineStat == 1) continue;
        cent_i = touch_info_fine[i].centerPos;
        diffSum_i = touch_info_fine[i].diffSum;
        maxCnt_i = touch_info_fine[i].maxCnt;
        gridWidth_i = touch_info_fine[i].gridWidth;
        lineIdxX0_i = touch_info_fine[i].lineIdxX0;
        lineIdxY0_i = touch_info_fine[i].lineIdxY0;
#if (DEBUG_fine_remove_ghost > 0)
    IS_DEBUG_FLAG {
        int lineIdxXY_i = touch_info_fine[i].lineIdxXY;
        //int lineIdxX_i = touch_info_fine[i].lineIdxX;
        TRACE_FRG("  lineIdxX0_i=(%d-%d/%d)(%d %d %d)", lineIdxX0_i, lineIdxXY_i, lineIdxY0_i, diffSum_i, maxCnt_i, gridWidth_i);
    };
#endif

        for (j = (i+1); j < touch_info_fine_cnt; j++) //nsmoon@230426 0=>(i+1)
        {
            if (i == j) continue;
            if (touch_info_fine[j].fineStat == 1) continue;
            lineIdxX0_j = touch_info_fine[j].lineIdxX0;
            if (mode == FINE_GHOST_MODE_X && lineIdxX0_i != lineIdxX0_j) {
                continue;
            }
            lineIdxY0_j = touch_info_fine[j].lineIdxY0;
            if (mode == FINE_GHOST_MODE_Y && lineIdxY0_i != lineIdxY0_j) {
                continue;
            }
            cent_j = touch_info_fine[j].centerPos;
            diffSum_j = touch_info_fine[j].diffSum;
            maxCnt_j = touch_info_fine[j].maxCnt;
            gridWidth_j = touch_info_fine[j].gridWidth;
            distX = GET_ABS(cent_i.x - cent_j.x);
            distY = GET_ABS(cent_i.y - cent_j.y);

#if (DEBUG_fine_remove_ghost > 0)
            IS_DEBUG_FLAG {
                int lineIdxXY_j = touch_info_fine[j].lineIdxXY;
                //int lineIdxX_j = touch_info_fine[j].lineIdxX;
                TRACE_FRG("    lineIdxX0_j=(%d-%d/%d)(%d %d %d)(%0.2f %0.2f)", lineIdxX0_j, lineIdxXY_j, lineIdxY0_j,
                           diffSum_j, maxCnt_j, gridWidth_j, distX, distY);
            };
#endif
            if ( (mode == FINE_GHOST_MODE_X) || (mode == FINE_GHOST_MODE_Y) ||
                 (mode == FINE_GHOST_MODE_XY && distX < near_dist_x && distY < near_dist_y) )
            {
                densDiffSum_i = (gridWidth_i) ? ((float)diffSum_i / gridWidth_i) : (diffSum_i * 10);
                densDiffSum_j = (gridWidth_j) ? ((float)diffSum_j / gridWidth_j) : (diffSum_j * 10);
                densDiffSum = GET_ABS(densDiffSum_i - densDiffSum_j);
                if (densDiffSum < densDiffSumMargin) {
                    densDiffSum_i = densDiffSum_j;
                }
#if (DEBUG_fine_remove_ghost > 0)
                IS_DEBUG_FLAG {
                    TRACE_FRG("    ~~distX,distY: %0.2f %0.2f (%0.2f)(%0.2f,%0.2f)(%d/%d)(%d/%d)(%d/%d)", distX, distY, densDiffSum,
                                    densDiffSum_i, densDiffSum_j, diffSum_i, diffSum_j, maxCnt_i, maxCnt_j, gridWidth_i, gridWidth_j);
                };
#endif
                if ( (diffSum_i != UINT8_MAX && diffSum_j == UINT8_MAX) ||
                     (densDiffSum_i < densDiffSum_j && diffSum_i != UINT8_MAX) ||
                     (mode == FINE_GHOST_MODE_XY && (densDiffSum_i == densDiffSum_j && maxCnt_i >= maxCnt_j)) )
                {
                    //if (touch_info_fine[j].tiXcnt > 1) //nsmoon@230424
                    if ((mode == FINE_GHOST_MODE_Y && touch_info_fine[j].tiXcnt > 1) ||
                            (mode == FINE_GHOST_MODE_X && touch_info_fine[j].tiYcnt > 1) ) //nsmoon@230516 //FIXME!!
                    {
                        IS_DEBUG_FLAG {TRACE_FRG("    =>ghost~j~ %d %d", touch_info_fine[j].tiXcnt, touch_info_fine[j].tiYcnt);};
                        fine_decrease_ti_cnt(touch_info_fine_cnt, j); //nsmoon@230424
                        touch_info_fine[j].fineStat = 1; //ghost
                        if ((--tCnt) < 3) {
                            return 1;
                        }
                    }
                    else if (mode == FINE_GHOST_MODE_XY) { //nsmoon@230424
                        if (densDiffSum < FINE_REMOVE_GHOST_DENSE_DIFF_X) {
                            densDiffSum_i = densDiffSum_j;
                        }
                    }
                }
                if ( (diffSum_j != UINT8_MAX && diffSum_i == UINT8_MAX) ||
                          (densDiffSum_j < densDiffSum_i && diffSum_j != UINT8_MAX) ||
                          (mode == FINE_GHOST_MODE_XY && (densDiffSum_j == densDiffSum_i && maxCnt_j >= maxCnt_i)) )
                {
                    //if (touch_info_fine[i].tiXcnt > 1) //nsmoon@230424
                    if ((mode == FINE_GHOST_MODE_Y && touch_info_fine[i].tiXcnt > 1) ||
                            (mode == FINE_GHOST_MODE_X && touch_info_fine[i].tiYcnt > 1) ) //nsmoon@230516 //FIXME!!
                    {
                        IS_DEBUG_FLAG {TRACE_FRG("    =>ghost~i~ %d %d", touch_info_fine[i].tiXcnt, touch_info_fine[i].tiYcnt);};
                        fine_decrease_ti_cnt(touch_info_fine_cnt, i); //nsmoon@230424
                        touch_info_fine[i].fineStat = 1; //ghost
                        if ((--tCnt) < 3) {
                            return 1;
                        }
                    }
                }
            }
        } //for (j = 0; j < touch_info_fine_cnt; j++)
    } //for (i = 0; i < touch_info_fine_cnt; i++)
    return 0;
}

static int fine_is_exist_one_tiYcnt(int touch_info_fine_cnt, int idx)
{
    int i;
    touch_info_fine_t *touch_info_fine = &BS_touch_info_fine[0];
    uint8_t lineIdxX0, lineIdxX0_in;
    uint8_t tiYcnt;

    lineIdxX0_in = BS_touch_info_fine[idx].lineIdxX0;

    for (i = 0; i < touch_info_fine_cnt; i++)
    {
        if (touch_info_fine[i].fineStat == 1) continue;
        if (i == idx) continue;
        lineIdxX0 = BS_touch_info_fine[i].lineIdxX0;
        if (lineIdxX0_in == lineIdxX0) {
            tiYcnt = touch_info_fine[i].tiYcnt;
            if (tiYcnt == 1) {
                return 1; //found
            }
        }
    }
    return 0; //not-found
}

static int fine_remove_ghost_pre(int touch_info_fine_cnt, int rmCnt)
{
    int i;
    touch_info_fine_t *touch_info_fine = &BS_touch_info_fine[0];
    uint8_t tiXcnt, tiYcnt;
    IS_DEBUG_FLAG {TRACE_FRG("fine_remove_ghost_pre..%d %d", touch_info_fine_cnt, rmCnt);};

    for (i = 0; i < touch_info_fine_cnt; i++)
    {
        if (touch_info_fine[i].fineStat == 1) continue;
        tiXcnt = touch_info_fine[i].tiXcnt;
        tiYcnt = touch_info_fine[i].tiYcnt;
        if (tiXcnt > 1 && tiYcnt > 1) {
            if (fine_is_exist_one_tiYcnt(touch_info_fine_cnt, i)) {
                IS_DEBUG_FLAG {TRACE_FRG("    =>ghost~~~~(%d) %d %d", i, touch_info_fine[i].tiXcnt, touch_info_fine[i].tiYcnt);};
                fine_decrease_ti_cnt(touch_info_fine_cnt, i); //nsmoon@230424
                touch_info_fine[i].fineStat = 1; //ghost
                rmCnt++;
            }
        }
    }
    if (rmCnt) {
        fine_remove_ghost_pre(touch_info_fine_cnt, 0);
    }

    return 0;
}

#if 1 //nsmoon@230412
static float fine_cal_densDiffSum(int idx)
{
    float densDiffSum;
    touch_info_fine_t *touch_info_fine = &BS_touch_info_fine[0];
    uint8_t diffSum = touch_info_fine[idx].diffSum;
    uint8_t gridWidth = touch_info_fine[idx].gridWidth;
    if (diffSum == UINT8_MAX || gridWidth == 0) {
        densDiffSum = (float)UINT8_MAX;
    }
    else {
        densDiffSum = (float)diffSum / gridWidth;
    }
    return densDiffSum;
}

#if (DEBUG_fine_remove_ghost > 0)
static void DEBUG_dump_touch_info_fine_sort(int sort_cnt, int mode)
{
    float densDiffSum;
    touch_info_fine_t *touch_info_fine = &BS_touch_info_fine[0];
    uint8_t *touch_info_fine_sort = &find_touch_info_fine_sort[0];
    uint8_t diffSum, gridWidth, maxCnt;
    int i;

    for (i = 0; i < sort_cnt; i++) {
        diffSum = touch_info_fine[touch_info_fine_sort[i]].diffSum;
        gridWidth = touch_info_fine[touch_info_fine_sort[i]].gridWidth;
        maxCnt = touch_info_fine[touch_info_fine_sort[i]].maxCnt;
        if (diffSum == UINT8_MAX || gridWidth == 0) {
            densDiffSum = (float)UINT8_MAX;
        }
        else {
            densDiffSum = (float)diffSum / gridWidth;
        }
        TRACE_FRG("densDiffSum=(%d) %0.2f %d/%d %d (%d) %d", mode, densDiffSum, diffSum, gridWidth, maxCnt, i, touch_info_fine_sort[i]);
    }
}
#endif

static int fine_get_touch_info_fine_sort(int touch_info_fine_cnt)
{
    int i, j;
    float densDiffSum_i, densDiffSum_j;
    int maxCnt_i, maxCnt_j;
    touch_info_fine_t *touch_info_fine = &BS_touch_info_fine[0];
    uint8_t *touch_info_fine_sort = &find_touch_info_fine_sort[0];
    int sort_cnt = 0, swap;
    uint8_t tmp8;

    //init sort
    for (i = 0; i < touch_info_fine_cnt; i++)
    {
        if (touch_info_fine[i].fineStat == 1) continue;
        touch_info_fine_sort[sort_cnt++] = i;
    }
#if 0 //(DEBUG_fine_remove_ghost > 0)
    IS_DEBUG_FLAG{
        DEBUG_dump_touch_info_fine_sort(sort_cnt, 0);
    };
#endif

    //decending sort
    for (i = 0; i < (sort_cnt - 1); i++)
    {
        for (j = (i + 1); j < sort_cnt; j++)
        {
            densDiffSum_i = fine_cal_densDiffSum(touch_info_fine_sort[i]);
            maxCnt_i = (int)touch_info_fine[touch_info_fine_sort[i]].maxCnt;
            densDiffSum_j = fine_cal_densDiffSum(touch_info_fine_sort[j]);
            maxCnt_j = (int)touch_info_fine[touch_info_fine_sort[j]].maxCnt;
            swap = 0;
            if (GET_ABS(densDiffSum_i - densDiffSum_j) < EPSILON) {
                if (maxCnt_i > maxCnt_j) {
                    swap++;
                }
            }
            else if (densDiffSum_i < densDiffSum_j) {
                swap++;
            }
            //IS_DEBUG_FLAG {TRACE_FRG("i/j=%d/%d %0.2f/%0.2f %d/%d (%d)", i, j, densDiffSum_i, densDiffSum_j, maxCnt_i, maxCnt_j, swap);};
            if (swap) {
                tmp8 = touch_info_fine_sort[i];
                touch_info_fine_sort[i] = touch_info_fine_sort[j];
                touch_info_fine_sort[j] = tmp8;
            }
        }
    }
#if 0 //(DEBUG_fine_remove_ghost > 0)
    IS_DEBUG_FLAG{
        DEBUG_dump_touch_info_fine_sort(sort_cnt, 1);
    };
#endif
    return sort_cnt;
}

static int fine_remove_ghost_over_max(int touch_info_fine_cnt)
{
    int i, idx;
    touch_info_fine_t *touch_info_fine = &BS_touch_info_fine[0];
    uint8_t *touch_info_fine_sort = &find_touch_info_fine_sort[0];
    int sort_cnt, tCnt;
    int notEq = (BS_initial_line_a_x_cnt != BS_initial_line_a_y_cnt);

    sort_cnt = fine_get_touch_info_fine_sort(touch_info_fine_cnt);
    if (sort_cnt < 1) {
        TRACE_ERROR("ERROR! fine_remove_ghost_over_max..invlid sort_cnt %d", sort_cnt);
        return 0;
    }

    tCnt = sort_cnt;
#if (DEBUG_fine_remove_ghost > 0)
    IS_DEBUG_FLAG{TRACE_FRG("fine_remove_ghost_over_max.. %d %d", touch_info_fine_cnt, tCnt);};
#endif

    for (i = 0; i < sort_cnt && tCnt > MAX_TOUCH_LIMIT_FINE; i++)
    {
        idx = touch_info_fine_sort[i];
        if (touch_info_fine[idx].tiXcnt == 1 ||
                (notEq && touch_info_fine[idx].tiYcnt == 1)) //nsmoon@230523
        {
            continue;
        }
#if (DEBUG_fine_remove_ghost > 0)
        IS_DEBUG_FLAG{
            int lineIdxX0 = touch_info_fine[idx].lineIdxX0;
            int lineIdxXY = touch_info_fine[idx].lineIdxXY;
            TRACE_FRG("idx=1= %d-%d (%d)", lineIdxX0, lineIdxXY, idx);
        };
#endif
        touch_info_fine[idx].fineStat = 1; //ghost
        fine_decrease_ti_cnt(touch_info_fine_cnt, idx); //nsmoon@230517
        tCnt--;
    }

    if (tCnt > MAX_TOUCH_LIMIT_FINE) {
        for (i = 0; i < sort_cnt && tCnt > MAX_TOUCH_LIMIT_FINE; i++)
        {
            idx = touch_info_fine_sort[i];
            if (touch_info_fine[idx].tiXcnt == 1) {
#if (DEBUG_fine_remove_ghost > 0)
                IS_DEBUG_FLAG{
                    int lineIdxX0 = touch_info_fine[idx].lineIdxX0;
                    int lineIdxXY = touch_info_fine[idx].lineIdxXY;
                    TRACE_FRG("idx=2= %d-%d (%d)", lineIdxX0, lineIdxXY, idx);
                };
#endif
                touch_info_fine[idx].fineStat = 1; //ghost
                fine_decrease_ti_cnt(touch_info_fine_cnt, idx); //nsmoon@230517
                tCnt--;
            }
        }
    }

    return 0;
}
#endif
#else
static int fine_remove_ghost(int touch_info_fine_cnt, int mode)
{
    int i, j;
    int edgeX_i, edgeY_i;
    int edgeX_j, edgeY_j;
    vec_t cent_i, cent_j;
    int diffSum_i, diffSum_j;
    int maxCnt_i, maxCnt_j;
    int gridWidth_i, gridWidth_j;
    float distX, distY;
    touch_info_fine_t *touch_info_fine = &BS_touch_info_fine[0];
    //float distSq;
    float near_dist_x = FINE_GHOST_NEAR_DIST; //near_dist_sq
    float near_dist_y = FINE_GHOST_NEAR_DIST;
    float densDiffSum_i, densDiffSum_j, densDiffSum;
    uint8_t lineIdxX0_i, lineIdxX0_j;
    int tCnt = chk_touch_info_cnt(touch_info_fine_cnt);
    TRACE_BCF5("fine_remove_ghost.. %d %d %d", touch_info_fine_cnt, mode, tCnt);

    for (i = 0; i < touch_info_fine_cnt; i++)
    {
        if (touch_info_fine[i].fineStat == 1) continue;
        cent_i = touch_info_fine[i].centerPos;
        diffSum_i = touch_info_fine[i].diffSum;
        maxCnt_i = touch_info_fine[i].maxCnt;
        gridWidth_i = touch_info_fine[i].gridWidth;
        lineIdxX0_i = touch_info_fine[i].lineIdxX0;

        edgeX_i = BS_is_edge(ENUM_HOR_X, cent_i.x);
        edgeY_i = BS_is_edge(ENUM_VER_Y, cent_i.y);
        IS_DEBUG_FLAG { TRACE_BCF5("  edgeX_i,edgeY_i=(%d) %d-%d (%d %d) (%0.2f %0.2f)", i, lineIdxX0_i, touch_info_fine[i].lineIdxXY, edgeX_i, edgeY_i, cent_i.x, cent_i.y); };
        if (edgeX_i || edgeY_i) {
            near_dist_y = BS_aarea_end_y; //FINE_GHOST_NEAR_DIST_EDGE_Y; //nsmoon@200922
            near_dist_x = FINE_GHOST_NEAR_DIST_EDGE_X;
        }

        for (j = 0; j < touch_info_fine_cnt; j++)
        {
            if (i == j) continue;
            if (touch_info_fine[j].fineStat == 1) continue;
            cent_j = touch_info_fine[j].centerPos;
            diffSum_j = touch_info_fine[j].diffSum;
            maxCnt_j = touch_info_fine[j].maxCnt;
            gridWidth_j = touch_info_fine[j].gridWidth;
            lineIdxX0_j = touch_info_fine[j].lineIdxX0;

            edgeX_j = BS_is_edge(ENUM_HOR_X, cent_j.x);
            edgeY_j = BS_is_edge(ENUM_VER_Y, cent_j.y);
            IS_DEBUG_FLAG{ TRACE_BCF5("    edgeX_j,edgeY_j=(%d) %d-%d (%d %d) (%0.2f %0.2f)", j, lineIdxX0_j, touch_info_fine[j].lineIdxXY, edgeX_j, edgeY_j, cent_j.x, cent_j.y); };
            if (edgeX_j || edgeY_j) {
                near_dist_y = BS_aarea_end_y; //FINE_GHOST_NEAR_DIST_EDGE_Y;
                near_dist_x = FINE_GHOST_NEAR_DIST_EDGE_X;
            }
#if 1 //nsmoon@211021 //4pen_1020B.dlt #117
#if defined(N75_TEST) || defined(N65_TEST) //nsmoon@230418
            if (mode == 1 && lineIdxX0_i == lineIdxX0_j) {
#else
            else if (mode == 1 && lineIdxX0_i == lineIdxX0_j) {
#endif
                near_dist_y = FINE_GHOST_NEAR_DIST_SAME_AXIS;
            }
#endif

            distX = GET_ABS(cent_i.x - cent_j.x);
            distY = GET_ABS(cent_i.y - cent_j.y);
            IS_DEBUG_FLAG{
                if (mode == 1 && lineIdxX0_i == lineIdxX0_j) {
                    TRACE_NOP;
                }
                if ((edgeX_i || edgeX_j) && distY < FINE_GHOST_SAME_AXIS) {
                    TRACE_NOP;
                }
            };
            IS_DEBUG_FLAG { TRACE_BCF5("      distX,distY=(%d)(%d-%d)(%d-%d/%d %d-%d/%d)%0.2f %0.2f(%d/%d)(%d/%d)(%d/%d)",
                                       mode, i, j, lineIdxX0_i, touch_info_fine[i].lineIdxXY, touch_info_fine[i].lineIdxX, lineIdxX0_j, touch_info_fine[j].lineIdxXY, touch_info_fine[j].lineIdxX,
                                       distX, distY, diffSum_i, diffSum_j, maxCnt_i, maxCnt_j, gridWidth_i, gridWidth_j); };
            if ((mode == 0 && distX < near_dist_x && distY < FINE_GHOST_SAME_AXIS) ||
                ((mode == 1 && lineIdxX0_i == lineIdxX0_j) && distY < near_dist_y) ||
                ((mode == 1 && lineIdxX0_i != lineIdxX0_j && distX < FINE_GHOST_SAME_AXIS) && distY < FINE_GHOST_NEAR_DIST_Y))
            {
                densDiffSum_i = (gridWidth_i) ? ((float)diffSum_i / gridWidth_i) : (diffSum_i * 10);
                densDiffSum_j = (gridWidth_j) ? ((float)diffSum_j / gridWidth_j) : (diffSum_j * 10);
                densDiffSum = GET_ABS(densDiffSum_i - densDiffSum_j);
                if (densDiffSum < 0.6f) { //nsmoon@200921 0.5f=>0.6f
                    densDiffSum_i = densDiffSum_j;
                }
                IS_DEBUG_FLAG { TRACE_BCF5("      distX,distY:(%d-%d) %0.2f %0.2f (%0.2f,%0.2f)(%d/%d)(%d/%d)(%d/%d)", i, j, distX, distY,
                                           densDiffSum_i, densDiffSum_j, diffSum_i, diffSum_j, maxCnt_i, maxCnt_j, gridWidth_i, gridWidth_j); };
                if (((diffSum_i != UINT8_MAX) && (diffSum_j == UINT8_MAX)) || 
                    ((densDiffSum_i < densDiffSum_j) && (diffSum_i != UINT8_MAX)) ||
                    ((GET_ABS(densDiffSum_i - densDiffSum_j) <= EPSILON) && (maxCnt_i >= maxCnt_j))) {
                    touch_info_fine[j].fineStat = 1; //ghost
                    IS_DEBUG_FLAG { 
                        TRACE_BCF5("      =>ghost~:(%d/%d)%d/%d-%d(%0.2f/%0.2f)(%0.2f/%0.2f)(%d/%d)(%d/%d)(%d/%d)", mode, tCnt, j, i, j, distX, distY, densDiffSum_i, densDiffSum_j, diffSum_i, diffSum_j, maxCnt_i, maxCnt_j, gridWidth_i, gridWidth_j); 
                    };
                    if ((--tCnt) < 3) {
                        return 1;
                    }
                }
            }
        } //for (j = 0; j < touch_info_fine_cnt; j++)
    } //for (i = 0; i < touch_info_fine_cnt; i++)
    return 0;
}

#if 1 //nsmoon@230412
static int fine_remove_ghost_over_max(int touch_info_fine_cnt)
{
    int i;
    int diffSum, gridWidth; //maxCnt
    float densDiffSum;
    touch_info_fine_t *touch_info_fine = &BS_touch_info_fine[0];
    int tCnt = chk_touch_info_cnt(touch_info_fine_cnt);
    float densDiffSumMax[MAX_TOUCH_LIMIT_FINE];
    int iMax[MAX_TOUCH_LIMIT_FINE];

    TRACE_BCF5("fine_remove_ghostall.. %d %d %d", touch_info_fine_cnt, tCnt);

    BS_get_float_min_max_init(&densDiffSumMax[0], &iMax[0], MAX_TOUCH_LIMIT_FINE, 1); //1:max

    for (i = 0; i < touch_info_fine_cnt; i++)
    {
        if (touch_info_fine[i].fineStat == 1) continue;
        diffSum = touch_info_fine[i].diffSum;
        //maxCnt = touch_info_fine[i].maxCnt;
        gridWidth = touch_info_fine[i].gridWidth;
        if (diffSum == UINT8_MAX || gridWidth == 0) {
            densDiffSum = UINT8_MAX;
        }
        else {
            densDiffSum = (float)diffSum / gridWidth;
        }
        TRACE("densDiffSum=%f %d %d %d", densDiffSum, i, diffSum, gridWidth);
        BS_get_float_min_max(&densDiffSumMax[0], &iMax[0], MAX_TOUCH_LIMIT_FINE, 0, densDiffSum, i, 1); //1:max
    }
    for (i = 0 ; i < (tCnt-MAX_TOUCH_LIMIT_FINE); i++) {
        TRACE("iMax=%d %f", iMax[i], densDiffSumMax[i]);
        if (i >= MAX_TOUCH_LIMIT_FINE) break;
        if (iMax[i] < 0) break;
        touch_info_fine[iMax[i]].fineStat = 1; //ghost
    }
    return 0;
}
#endif
#endif

#if 1 //save //nsmoon@230613
static void fine_save_inst_xy(void)
{
    BS_inst_xy_grp_cnt_pre = BS_inst_xy_grp_cnt;
    BS_inst_xy_cnt_pre = BS_inst_xy_cnt;
}

static void fine_restore_inst_xy(void)
{
    BS_inst_xy_grp_cnt = BS_inst_xy_grp_cnt_pre;
    BS_inst_xy_cnt = BS_inst_xy_cnt_pre;
}
#endif

static int fine_cal_coordinates5(int fineLoop)
{
    initial_line_a_t *initial_line_x = &BS_initial_line_a_x[0];
    initial_line_a_t *initial_line_x2 = &BS_initial_line_a_x2[0];
    initial_line_a_t *initial_line_y = &BS_initial_line_a_y[0];
    int line_idx_x, line_idx_x2, line_idx_x_y; //nsmoon@230412 line_idx_y=>line_idx_x_y
    initial_line_group_t *grpX, *grpY;
    in_line_t *lineSenX, *lineSenY;
    vec_t centerPos, touchWidth;
    float touchArea;
    pos_min_max_t minMaxPos;
    touch_info_fine_t *touch_info_fine = &BS_touch_info_fine[0];
    int touch_info_fine_cnt = 0;
    int touchCntOrg = BG_touch_count;
    int touchCntFineOrg = BG_touch_count_fine;
    int touchCntFine = touchCntFineOrg;
    int touchCnt = touchCntOrg;
    int ret;
    int i;
    int cxpIdx, yLineCnt;
    int cxpCntMax;
    tp_line_cnt_t thCnt;
    initial_line_a_t *lineX, *lineXY;
    int lineIdxX, lineIdxXY, grpIdxX, grpIdxY;
    IS_DEBUG_FLAG {
        TRACE_NOP;
    };
#if 0 //for test
    TRACE("sizeof(initial_line_a_t)=%d", sizeof(initial_line_a_t));
    TRACE("sizeof(touch_info_fine_t)=%d", sizeof(touch_info_fine_t));
    TRACE("sizeof(initial_line_group_t)=%d", sizeof(initial_line_group_t));
    TRACE("sizeof(pos_min_max_t)=%d", sizeof(pos_min_max_t));
    TRACE("sizeof(vec_t)=%d", sizeof(vec_t));
    TRACE("sizeof(in_line_t)=%d", sizeof(in_line_t));
#endif

    thCnt.th10CntX = thCnt.th50CntX = 0;
    thCnt.th10WidthX = 0;
    thCnt.th10CntY = thCnt.th50CntY = 0;
    thCnt.th10WidthY = 0;

#if 1 //nsmoon@230613
    if (fine_get_initial_cxp() == 0) {
        return 0; //no-touch-point
    }
#endif

    line_idx_x2 = 0; //-1; //nsmoon@230613
    touch_info_fine_cnt = 0;
    for (line_idx_x = 0; line_idx_x < BS_initial_line_a_x_cnt; line_idx_x++)
    {
        IS_DEBUG_FLAG { TRACE_BCF5("********* line_idx_x=%d/%d", line_idx_x, BS_initial_line_a_x_cnt); };
        IS_DEBUG_FLAG{
            TRACE_NOP;
        };
        //get intersect for y-group
        ret = fine_get_inst5(ENUM_HOR_X, initial_line_x, line_idx_x, FINE_USE_INITIAL_LINE,
                             ENUM_VER_Y, &BS_remained_y[0], BS_remained_y_cnt, ENUM_VER_Y, FINE_CXP_CAL_INIT);
        if (ret == FINE_MEM_ERROR) {
            TRACE_ERROR_MEM("ERROR_MEM! ...fine_get_inst5 X");
            return ret; //mem error
        }
        else if (ret != FINE_INST_OK) {
            IS_DEBUG_FLAG { TRACE_BCF52("no inst-1 ...fine_get_inst5"); };
            continue; //not inst
        }

        yLineCnt = 0;
        cxpCntMax = initial_line_x[line_idx_x].cxpCntMax;
        for (cxpIdx = 0; cxpIdx < cxpCntMax; cxpIdx++)
        {
            int sortLenX = (int)initial_line_x[line_idx_x].sortLen_a[cxpIdx];
            int isEdgeAreaX = initial_line_x[line_idx_x].isEdgeArea[cxpIdx];
#if (DEBUG_BG_clipping_fine5 > 0)
            IS_DEBUG_FLAG {
                line_idx_x_y = initial_line_x[line_idx_x].cxOppLine[cxpIdx];
                TRACE_BCF52(" ##cxpIdx=%d-%d,%d %d (%d %04x)", line_idx_x, line_idx_x_y, cxpIdx, cxpCntMax, sortLenX, isEdgeAreaX);
            };
#endif
            if ((IS_NOT_NEAR_Y(isEdgeAreaX) && sortLenX < FINE_INITIAL_GRP_MIN_NUM) ||
                (IS_NEAR_Y(isEdgeAreaX) && sortLenX < FINE_INITIAL_GRP_MIN_NUM_EDGE)) {
                IS_DEBUG_FLAG { TRACE_BCF52("  (*)isEdgeAreaX= %04x %d", isEdgeAreaX, sortLenX); };
                continue;
            }

            fine_save_inst_xy(); //nsmoon@230613

            ////////////////////////////////////
            //make y-group by initial x-line
            ////////////////////////////////////
            ret = fine_make_y_grp_x_line_cxp(initial_line_x, line_idx_x, cxpIdx, initial_line_y);
            if (ret == FINE_MEM_ERROR) {
                TRACE_ERROR_MEM(" ERROR_MEM! ...fine_make_y_grp_x_line_cxp");
                return FINE_MEM_ERROR; //mem error
            }
            else if (ret == FINE_NO_GROUP) {
                IS_DEBUG_FLAG { TRACE_BCF52("  no group-1 ...fine_make_y_grp_x_line_cxp"); };
                fine_restore_inst_xy(); //nsmoon@230613
                continue; //no-group
            }
            IS_DEBUG_FLAG{
                TRACE_NOP;
            };

            ////////////////////////////////
            //make x-group by y-cent-line
            ////////////////////////////////
            line_idx_x_y = initial_line_x[line_idx_x].cxOppLine[cxpIdx];
            ret = fine_get_inst5(ENUM_VER_Y, initial_line_y, line_idx_x_y, FINE_USE_CENTER_LINE,
                                 ENUM_HOR_X, &BS_remained_x[0], BS_remained_x_cnt, ENUM_HOR_X, FINE_CXP_CAL_NORM);
            if (ret == FINE_MEM_ERROR) {
                TRACE_ERROR_MEM(" ERROR_MEM! ...fine_get_inst5 Y");
                return ret; //mem error
            }
            else if (ret != FINE_INST_OK) {
                IS_DEBUG_FLAG { TRACE_BCF52(" no inst-2 ...fine_get_inst4"); };
                fine_restore_inst_xy(); //nsmoon@230613
                continue; //not inst
            }

            int sortLenY = (int)initial_line_y[line_idx_x_y].sortLen_a[FINE_CXP_ORI];
            int isEdgeAreaY = initial_line_y[line_idx_x_y].isEdgeArea[FINE_CXP_ORI];
            IS_DEBUG_FLAG { TRACE_BCF52(" sortLenY,isEdgeAreaY= %d %04x", sortLenY,isEdgeAreaY); };
            if ((IS_NOT_NEAR_X(isEdgeAreaY) && sortLenY < FINE_INITIAL_GRP_MIN_NUM) ||
                (IS_NEAR_X(isEdgeAreaY) && sortLenY < FINE_INITIAL_GRP_MIN_NUM_EDGE)) {
                IS_DEBUG_FLAG { TRACE_BCF52(" (*)isEdgeAreaY= %04x", isEdgeAreaY); };
                fine_restore_inst_xy(); //nsmoon@230613
                continue;
            }

            if (line_idx_x2 < 0 || line_idx_x2 >= MAX_INITIAL_LINE_FINE_X2) { //nsmoon@230517
                TRACE_ERROR_MEM(" ERROR_MEM! fine_cal_coordinates5..invalid line_idx_x2 %d %d", line_idx_x2, line_idx_x_y);
                return FINE_MEM_ERROR; //mem error
            }
            //line_idx_x2++; //increase x-axis2 first //nsmoon@230517 move to below
            ret = fine_make_x_grp_y_cent_line(initial_line_y, line_idx_x_y, initial_line_x2, line_idx_x2);
            if (ret == FINE_MEM_ERROR) {
                TRACE_ERROR_MEM(" ERROR_MEM! ...fine_make_x_grp_y_cent_line");
                return FINE_MEM_ERROR; //mem error
            }
            else if (ret == FINE_NO_GROUP) {
                IS_DEBUG_FLAG { TRACE_BCF52("  no group-2 ...fine_make_x_grp_y_cent_line"); };
                fine_restore_inst_xy(); //nsmoon@230613
                continue; //no-group
            }

            ///////////////////////////////////
            //make y-group by x-cent-line
            ///////////////////////////////////
            ret = fine_get_inst5(ENUM_HOR_X, initial_line_x2, line_idx_x2, FINE_USE_CENTER_LINE,
                                 ENUM_VER_Y, &BS_remained_y[0], BS_remained_y_cnt, ENUM_VER_Y, FINE_CXP_CAL_NORM);
            if (ret == FINE_MEM_ERROR) {
                TRACE_ERROR_MEM("  ERROR_MEM! ...fine_get_inst5 X");
                return ret; //mem error
            }
            else if (ret != FINE_INST_OK) {
                IS_DEBUG_FLAG { TRACE_BCF52(" no inst-3 ...fine_get_inst5"); };
                fine_restore_inst_xy(); //nsmoon@230613
                continue; //not inst
            }

            //IS_DEBUG_FLAG{ TRACE_BCF52("  &initial_line_x2[].sortLen_a[]=%x %d %d %d", &initial_line_x2[line_idx_x2].sortLen_a[FINE_CXP_ORI], line_idx_x2, FINE_CXP_ORI, initial_line_x2[line_idx_x2].sortLen_a[FINE_CXP_ORI]);};
            int sortLenX2 = (int)initial_line_x2[line_idx_x2].sortLen_a[FINE_CXP_ORI];
            int isEdgeAreaX2 = initial_line_x2[line_idx_x2].isEdgeArea[FINE_CXP_ORI];
            IS_DEBUG_FLAG { TRACE_BCF52(" sortLenX2,isEdgeAreaX2= %d %d", sortLenX2,isEdgeAreaX2); };
            if ((IS_NOT_NEAR_Y(isEdgeAreaX2) && sortLenX2 < FINE_INITIAL_GRP_MIN_NUM) ||
                (IS_NEAR_Y(isEdgeAreaX2) && sortLenX2 < FINE_INITIAL_GRP_MIN_NUM_EDGE)) {
                IS_DEBUG_FLAG { TRACE_BCF52(" (*)isEdgeAreaX2= %04x", isEdgeAreaX2); };
                fine_restore_inst_xy(); //nsmoon@230613
                continue;
            }

            ret = fine_make_y_grp_x_line(initial_line_x2, line_idx_x2);
            if (ret == FINE_MEM_ERROR) {
                TRACE_ERROR_MEM(" ERROR_MEM! ...fine_make_y_grp_x_line");
                return FINE_MEM_ERROR; //mem error
            }
            else if (ret == FINE_NO_GROUP) {
                IS_DEBUG_FLAG { TRACE_BCF52("  no group-3 ...fine_make_y_grp_x_line"); };
                fine_restore_inst_xy(); //nsmoon@230613
                continue; //no-group
            }

            yLineCnt++; //increase y-grp cnt
            grpX = initial_line_x2[line_idx_x2].grp_a[FINE_CXP_ORI]; //y-grp
            grpY = initial_line_y[line_idx_x_y].grp_a[FINE_CXP_ORI]; //x-grp
            //IS_DEBUG_FLAG{ TRACE_BCF52("  grpX->len= %d %d", grpX->len, grpY->len);  };

            minMaxPos.minY = grpX->instPos.min;
            minMaxPos.maxY = grpX->instPos.max;
            minMaxPos.minX = grpY->instPos.min;
            minMaxPos.maxX = grpY->instPos.max;
            //DEBUG_SHOW_MIN_MAX(&minMaxPos, MY_COLOR-2, 1);
#if 0 //def FINE_REMAINED_INIT_LINE_MAX //nsmoon@211015
            if (fine_is_closed_tp_min_max2(&minMaxPos, 0/*touchCntOrg*/, touchCnt, 0/*touchCntOrg*/))
#else
            if (fine_is_closed_tp_min_max(&minMaxPos, 0/*touchCntOrg*/, touchCnt))
#endif
            {
                IS_DEBUG_FLAG{ TRACE_BCF52("  closed--%d-%d", line_idx_x, cxpIdx); };
                fine_restore_inst_xy(); //nsmoon@230613
                continue;
            }

            centerPos.y = (grpX->instPos.min + grpX->instPos.max) * 0.5f; //grpX is for y-grp
            centerPos.x = (grpY->instPos.min + grpY->instPos.max) * 0.5f; //grpY is for x-grp
#if (DEBUG_BG_clipping_fine5 > 0)
            IS_DEBUG_FLAG {
                float sizeX, sizeY;
                sizeX = GET_ABS(minMaxPos.maxX - minMaxPos.minX);
                sizeY = GET_ABS(minMaxPos.maxY - minMaxPos.minY);
                TRACE_BCF5("  sizeX,sizeY= %0.1f,%0.1f", sizeX, sizeY);
            //DEBUG_SHOW_POS(&centerPos, 0.5f, 0.5f, MY_COLOR);
            };
#endif

            //if (touch_info_fine_cnt < MAX_TOUCH_INFO_FINE_SIZE)
            if (touch_info_fine_cnt < MAX_TOUCH_INFO_FINE_SIZE &&
                    line_idx_x2 < MAX_INITIAL_LINE_FINE_X2) //nsmoon@230613
            {
                initial_line_x[line_idx_x].cxTcIdx[cxpIdx] = (uint8_t)touch_info_fine_cnt;
                touch_info_fine[touch_info_fine_cnt].lineIdxX0 = (uint8_t)line_idx_x;
#ifdef FINE_REMOVE_GHOST_NEW
                touch_info_fine[touch_info_fine_cnt].lineIdxY0 = (uint8_t)cxpIdx; //nsmoon@230419
#endif
                touch_info_fine[touch_info_fine_cnt].lineX = initial_line_x2;
                touch_info_fine[touch_info_fine_cnt].lineIdxX = (uint8_t)line_idx_x2;
                touch_info_fine[touch_info_fine_cnt].grpIdxX = FINE_CXP_ORI;
                touch_info_fine[touch_info_fine_cnt].lineXY = initial_line_y; //nsmoon@230412 lineIdxY=>lineIdxXY
                touch_info_fine[touch_info_fine_cnt].lineIdxXY = (uint8_t)line_idx_x_y; //nsmoon@230412 lineIdxY=>lineIdxXY
                touch_info_fine[touch_info_fine_cnt].grpIdxY = FINE_CXP_ORI;
                touch_info_fine[touch_info_fine_cnt].mM = minMaxPos;
                touch_info_fine[touch_info_fine_cnt].centerPos = centerPos;
                touch_info_fine[touch_info_fine_cnt].diffSum = GET_MIN(grpX->diffSum + grpY->diffSum, UINT8_MAX -1);
                touch_info_fine[touch_info_fine_cnt].maxCnt = GET_MIN(grpX->maxCnt + grpY->maxCnt, UINT8_MAX);
                touch_info_fine[touch_info_fine_cnt].gridWidth = grpX->gridWidth + grpY->gridWidth;
                touch_info_fine[touch_info_fine_cnt].fineStat = 0; //init
                IS_DEBUG_FLAG { TRACE_BCF5("  ==>line_idx_x,cxpIdx=(%d) %d-%d %d-%d (%0.1f,%0.1f)", touch_info_fine_cnt, line_idx_x, line_idx_x_y, line_idx_x2, cxpIdx, centerPos.x, centerPos.y);};
                touch_info_fine_cnt++;
                line_idx_x2++; //increase x-axis2 //nsmoon@230613
            }
            else {
                if (line_idx_x2 >= MAX_INITIAL_LINE_FINE_X2) {
                    TRACE_ERROR_MEM(" ERROR_MEM! invalid line_idx_x2 %d", line_idx_x2);
                }
                if (touch_info_fine_cnt >= MAX_TOUCH_INFO_FINE_SIZE) {
                    TRACE_ERROR_MEM(" ERROR_MEM!! invalid touch_info_fine_cnt %d", touch_info_fine_cnt);
                }
                break; //
            }
            IS_DEBUG_FLAG{
                TRACE_NOP;
            };
        } //for (cxpIdx = 0; cxpIdx < grpCntMax; cxpIdx++)
        IS_DEBUG_FLAG { TRACE_BCF52("line_idx_x,yLineCnt= (%d) %d", line_idx_x, yLineCnt);};
        if (touch_info_fine_cnt >= MAX_TOUCH_INFO_FINE_SIZE ||
                line_idx_x2 >= MAX_INITIAL_LINE_FINE_X2) { //nsmoon@230613
            break;
                                }

        IS_DEBUG_FLAG{
            TRACE_NOP;
        };
    } //for (line_idx_x = 0; line_idx_x < BS_initial_line_a_x_cnt; line_idx_x++)
    //BS_initial_line_a_x2_cnt = line_idx_x2 + 1;

    ////////////////////////
    //remove ghost
    ////////////////////////
    IS_DEBUG_FLAG {
        //TRACE_BCF5("REMOVE GHOST..touch_info_fine_cnt= %d/%d", touch_info_fine_cnt, cxpCntMax);
    };
#ifdef FINE_REMOVE_GHOST_NEW //nsmoon@230613
        fine_get_one_touch_info(touch_info_fine_cnt); //nsmoon@230425
        if (BS_initial_line_a_x_cnt == BS_initial_line_a_y_cnt) {
            fine_remove_ghost_pre(touch_info_fine_cnt, 0); //nsmoon@230515
#if (DEBUG_BG_clipping_fine5 > 0)
            IS_DEBUG_FLAG {
                fine_get_one_touch_info(touch_info_fine_cnt); //nsmoon@230425
            };
#endif
        }

        for (i = 0; i <= FINE_GHOST_MODE_END; i++) { //nsmoon@230419
            if (fine_remove_ghost(touch_info_fine_cnt, i)) {
                //break;
            }
#if (DEBUG_BG_clipping_fine5 > 0)
        IS_DEBUG_FLAG {
            fine_get_one_touch_info(touch_info_fine_cnt); //nsmoon@230425
        };
#endif
        }
#else
#if 1 //nsmoon@200921
    if (chk_touch_info_cnt(touch_info_fine_cnt) > 2) {
        for (i = 0; i < 2; i++)
        {
            if (fine_remove_ghost(touch_info_fine_cnt, i)) {
                //break;
            }
        }
    }
#endif
#endif
#if 1 //nsmoon@230412
    if (chk_touch_info_cnt(touch_info_fine_cnt) > MAX_TOUCH_LIMIT_FINE) {
        fine_remove_ghost_over_max(touch_info_fine_cnt);
#if (DEBUG_BG_clipping_fine5 > 0)
                IS_DEBUG_FLAG{
#ifdef FINE_REMOVE_GHOST_NEW
                fine_get_one_touch_info(touch_info_fine_cnt); //nsmoon@230425
#endif
                };
    #endif
                }
#endif
    IS_DEBUG_FLAG { TRACE_BCF5("-touch_info_fine_cnt= %d", touch_info_fine_cnt);};

#if 1 //for test
    ////////////////////////
    //add touch count
    ////////////////////////
    for (i = 0; i < touch_info_fine_cnt; i++)
    {
        if (touch_info_fine[i].fineStat == 1) continue;

        lineX = touch_info_fine[i].lineX;
        lineXY = touch_info_fine[i].lineXY; //nsmoon@230412 lineY=>lineXY
        lineIdxX = touch_info_fine[i].lineIdxX;
        lineIdxXY = touch_info_fine[i].lineIdxXY; //nsmoon@230412 lineIdxY=>lineIdxXY
        grpX = lineX[lineIdxX].grp_a[FINE_CXP_ORI]; //must use initial_line_x2
        grpY = lineXY[lineIdxXY].grp_a[FINE_CXP_ORI]; //nsmoon@230412 lineIdxY=>lineIdxXY
        grpIdxX = touch_info_fine[i].grpIdxX;
        grpIdxY = touch_info_fine[i].grpIdxY;
        lineSenX = lineX[lineIdxX].lineSen_a[grpIdxX];
        lineSenY = lineXY[lineIdxXY].lineSen_a[grpIdxY]; //nsmoon@230412 lineIdxY=>lineIdxXY

        if (touchCnt < ALLOWABLE_TOUCH_BE && touchCntFine < MAX_TOUCH_LIMIT_FINE) {
            centerPos = touch_info_fine[i].centerPos;
            if (IS_NOT_ACTIVE_AREA(centerPos.x, centerPos.y)) {
                IS_DEBUG_FLAG { TRACE_BCF5("-fine_cal_coordinates5..out of black area: %0.1f %0.1f", centerPos.x, centerPos.y); };
                fine_add_used_line(ENUM_VER_Y, &grpX[grpIdxX], lineSenX);
                fine_add_used_line(ENUM_HOR_X, &grpY[grpIdxX], lineSenY);
                touch_info_fine[i].fineStat = 1;
#ifdef HOR_EDGE_TOUCH_ENABLE
                if (BG_touch_data_edge.x == 0) {
                    BG_touch_data_edge.x = centerPos.x;
                    BG_touch_data_edge.y = centerPos.y;
                }
#endif
                continue;
            }
#if 1 //nsmoon@200924
#if 0 //def FINE_REMAINED_INIT_LINE_MAX //nsmoon@211015
            if (fine_is_closed_tp_min_max2(&touch_info_fine[i].mM, 0/*touchCntOrg*/, touchCnt, 0))
#else
            if (fine_is_closed_tp_min_max(&touch_info_fine[i].mM, 0/*touchCntOrg*/, touchCnt))
#endif
            {
#if (DEBUG_BG_clipping_fine5 > 0)
                IS_DEBUG_FLAG {
                    int lineIdxXY = touch_info_fine[i].lineIdxXY;
                    int lineIdxX0 = touch_info_fine[i].lineIdxX0;
                    TRACE("-closed~4~ %d-%d (%d %d)", lineIdxX0, lineIdxXY, i, touchCnt);
                    touch_info_fine[i].fineStat = 1;
#ifdef FINE_REMOVE_GHOST_NEW
                    fine_get_one_touch_info(touch_info_fine_cnt); //nsmoon@230425
#endif
                };
#endif
            continue;
        }
#endif

            touchWidth.x = GET_ABS(touch_info_fine[i].mM.maxX - touch_info_fine[i].mM.minX);
            touchWidth.y = GET_ABS(touch_info_fine[i].mM.maxY - touch_info_fine[i].mM.minY);
#if (DEBUG_BG_clipping_fine5 > 0) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
            DEBUG_SHOW_POS(&centerPos, (touchWidth.x*0.5f), (touchWidth.y*0.5f), MY_COLOR);
#endif

#if 1 //nsmoon@200402  //FIXME!!
#define FINE_WIDTH_MIN          2.0f
#define FINE_WIDTH_RATIO_MIN    0.5f
            if (touchWidth.x < FINE_WIDTH_MIN || touchWidth.y < FINE_WIDTH_MIN) {
                //marker or pen
                float widthRatio = (touchWidth.x > touchWidth.y) ? (touchWidth.y / touchWidth.x) : (touchWidth.x / touchWidth.y);
                if (widthRatio < FINE_WIDTH_RATIO_MIN) {
                    if (touchWidth.x > touchWidth.y) {
                        touchWidth.y = touchWidth.x * FINE_WIDTH_RATIO_MIN;
                    }
                    else {
                        touchWidth.x = touchWidth.y * FINE_WIDTH_RATIO_MIN;
                    }
                }
            }
#endif
            touchArea = touchWidth.x * touchWidth.y;
            BG_touch_data[touchCnt] = centerPos;
            BG_touch_area[touchCnt] = touchArea;
            BG_touch_size[touchCnt].xSize = touchWidth.x;
            BG_touch_size[touchCnt].ySize = touchWidth.y;
            BG_multi_fine[touchCnt] = 0; //0:fine, for debugging
            IS_DEBUG_FLAG { TRACE_BCF5("-#find touchCnt=(%d) %d (%0.1f,%0.1f)(%0.1f,%0.1f)", i, touchCnt, centerPos.x, centerPos.y, touchWidth.x, touchWidth.y); };
            //IS_DEBUG_FLAG{ TRACE_BCF5("-grpX->len= %d %d", grpX->len, grpY->len);  };
#ifdef FRONTEND_LINE_THRESHOLD //nsmoon@191226
            fine_getThresholdCnt(ENUM_VER_Y, &touch_info_fine[i].mM, &grpX[grpIdxX], lineSenX, &thCnt); //must be use grp[] with opp axis
            fine_getThresholdCnt(ENUM_HOR_X, &touch_info_fine[i].mM, &grpY[grpIdxY], lineSenY, &thCnt); //must be use grp[] with opp axis
            BG_touch_lineCnt[touchCnt] = thCnt;
#endif
            touchCntFine++; touchCnt++;
        }
        else {
            TRACE_ERROR_MEM("ERROR_MEM! invalid touchCntFine,touchCnt..%d(%d)", touchCntFine, touchCnt);
            break;
        }
    } //for (i = 0; i < touch_info_fine_cnt; i++)
#endif //1
#if 0 //nsmoon@200924 not-use
    BS_adj_used_lines(touchCntOrg, touchCnt, ADJUST_USED_LINE_NORMAL); //adjust used line, nsmoon@200331
#endif

    IS_DEBUG_FLAG{
        TRACE_NOP;
    };

    int touchLocal = touchCntFine - touchCntFineOrg;
    BG_touch_count_fine = touchCntFine;
    BG_touch_count = touchCnt;
    IS_DEBUG_FLAG { TRACE_BCF5("-touchLocal: %d (%d/%d)", touchLocal, touchCntFine, touchCnt); };
    return touchLocal;
}

#if defined(FINE_OLD_METHOD_2PEN) && !defined(FINE_SHARED_LINE_COORD3)
#define FINE_INITIAL_CXP_DIST_MIN       150.0f //100.0f //nsmoon@211019 100.0f=>150.0f
#define FINE_INITIAL_CXP_DIST_MIN_EDGE  200.0f //150.0f //nsmoon@211019 100.0f=>200.0f
static int fine_is_closed_initial_cxp()
{
    int a_i, a_j, b_i, b_j;
    int cxpCnt_a, cxpCnt_b;
    vec_t cent_a, cent_b;
    float distX, dist_min;
    initial_line_a_t *initial_line_x = &BS_initial_line_a_x[0];

    for (a_i = 0; a_i < (BS_initial_line_a_x_cnt - 1); a_i++) {
        cxpCnt_a = initial_line_x[a_i].cxpCntMax;
        for (a_j = 0; a_j < cxpCnt_a; a_j++) {
            cent_a = initial_line_x[a_i].cxpCent[a_j];
            for (b_i = (a_i + 1); b_i < BS_initial_line_a_x_cnt; b_i++) {
                cxpCnt_b = initial_line_x[b_i].cxpCntMax;
                for (b_j = 0; b_j < cxpCnt_b; b_j++) {
                    cent_b = initial_line_x[b_i].cxpCent[b_j];
                    distX = GET_ABS(cent_a.x - cent_b.x);
                    //TRACE("dist = %0.1f (%d/%d)(%d/%d)", distX, a_i, a_j, b_i, b_j);
#if 1 //nsmoon@201109
                    if (BS_is_edge(ENUM_HOR_X, cent_a.x)) {
                        dist_min = FINE_INITIAL_CXP_DIST_MIN_EDGE;
                    }
                    else if (BS_is_edge(ENUM_HOR_X, cent_b.x)) {
                        dist_min = FINE_INITIAL_CXP_DIST_MIN_EDGE;
                    }
#if 1 //nsmoon@211019
                    else if (BS_is_edge(ENUM_VER_Y, cent_a.y)) {
                        dist_min = FINE_INITIAL_CXP_DIST_MIN_EDGE;
                    }
                    else if (BS_is_edge(ENUM_VER_Y, cent_b.y)) {
                        dist_min = FINE_INITIAL_CXP_DIST_MIN_EDGE;
                    }
#endif
                    else {
                        dist_min = FINE_INITIAL_CXP_DIST_MIN;
                    }
                    if (distX < dist_min) {
                        //TRACE("distX = %0.1f/%0.1f (%d/%d)(%d/%d)", distX, dist_min, a_i, a_j, b_i, b_j);
                        return 1; //found
                    }
#else
                    if (distX < FINE_INITIAL_CXP_DIST_MIN) {
                        return 1; //found
                    }
#endif
                } //for (b_j = 0; b_j < cxpCnt_b; b_j++)
            } //for (b_i = 0; b_i < BS_initial_line_a_x_cnt; b_i++)
        } //for (a_j = 0; a_j < cxpCnt_a; a_j++)
    } //for (a_i = 0; a_i < BS_initial_line_a_x_cnt; a_i++)
    return 0; //not-found
}
#endif

int BG_clipping_fine5(int fineLoop)
{
    int ret = 0;
    int touchCntOrg = BG_touch_count;

#ifdef FINE_OLD_METHOD_2PEN //old method for 2-pen
    ret = fine_get_initial_tp(fineLoop);
    if (ret) {
        if (ret == FINE_MEM_ERROR) {
            return ret; //error
        }
        return 0; //no-touch-point
    }
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };
#ifdef FINE_SHARED_LINE_COORD3 //nsmoon@230329 //FIXME!!
    if (BS_initial_line_a_x_cnt == BS_initial_line_a_y_cnt && BS_initial_line_a_x_cnt > 1) {
        ret = fine_cal_coordinates5(fineLoop);
    }
    else {
        ret = fine_cal_coordinates3(fineLoop);
    }
#else
    if (BS_initial_line_a_x_cnt <= FINE_COORD3_LINE_CNT && BS_initial_line_a_y_cnt <= FINE_COORD3_LINE_CNT) {
        if (fine_is_closed_initial_cxp() == 0) {
            ret = fine_cal_coordinates3(fineLoop);
        }
        else
        {
            ret = fine_cal_coordinates5(fineLoop);
        }
    }
    else {
        ret = fine_cal_coordinates5(fineLoop);
    }
#endif
#else
    fine_get_initial_tp(fineLoop);
    ret = fine_cal_coordinates5(fineLoop);
#endif

#if 1 //nsmoon@211201
    if (ret > 0) {
		int touchCnt = BG_touch_count;
    //BS_adj_used_lines_tp(touchCntOrg, touchCnt, ADJUST_USED_LINE_NORMAL); //adjust used line, nsmoon@200331 //BG_clipping_fine5
    BS_adj_used_lines_tp(touchCntOrg, touchCnt, ADJUST_USED_LINE_SHADOW); //adjust used line, nsmoon@220118 //BG_clipping_fine5
    }
#endif
    return ret;
}

void fine_get_tp(int fineLoop)
{
    fine_get_initial_tp(fineLoop);
}
/* end of file */
