/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Lispect

  @File Name
    backend_postp.c

  @Summary
    backend post processingo

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */
#include "backend.h"
#include "backend_postp.h"
#include "bezier.h"

#if !defined(_WIN32) && !defined(WIN32)
#include "app.h"
#endif

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a sect
 * on can be given directly below the section
    banner.
 */

/* ************************************************************************** */
/** Descriptive Data Item Name

  @Summary
    Brief one-line summary of the data item.
    
  @Description
    Full description, explaining the purpose and usage of data item.
    <p>
    Additional description in consecutive paragraphs separated by HTML 
    paragraph breaks, as necessary.
    <p>
    Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.
    
  @Remarks
    Any additional remarks
 */
//******************************************************************************
#if	1	//for brush
ATTR_BACKEND_RAM3 static uint8_t s_Touch_ID_Status_Tbl[ALLOWABLE_TOUCH_DATA_IO];
ATTR_BACKEND_RAM3 static uint8_t s_Touch_ID_Idx_Tbl[ALLOWABLE_TOUCH_DATA_IO];
ATTR_BACKEND_RAM3 static uint8_t s_Touch_Big_Size_ID_Tbl[ALLOWABLE_TOUCH_DATA_IO];
ATTR_BACKEND_RAM3 static vec_t s_BrushSizeFilterOut[ALLOWABLE_TOUCH_DATA_IO];
ATTR_BACKEND_RAM3 static vec_t s_prevBrushSizeFilterOut[ALLOWABLE_TOUCH_DATA_IO];
static uint8_t s_Brush_Release_Cnt;
static uint16_t preXCord, preYCord, Brush_Eraser_xSize, Brush_Eraser_ySize;
static uint32_t s_Brush_Frame_Cnt, s_Brush_Big_Size_Frame_Cnt, s_Brush_Normal_Size_Frame_Cnt;
static float s_OutFilter_Brush_Size;
move_pos_status_t Move_Status, preMove_Status;
brush_size_mode_status_t Brush_Size_Status, preBrush_Size_Status;
brush_process_t Brush_Process;
#endif

//ATTR_BACKEND_RAM3 static touchDataSt_t s_touchPosDataIn[ALLOWABLE_TOUCH_DATA_IO];
ATTR_BACKEND_RAM3 touchDataSt_t s_touchPosDataIn[ALLOWABLE_TOUCH_DATA_IO]; //for test
ATTR_BACKEND_RAM3 static touchDataSt_t s_prevTouchPosDataIn[ALLOWABLE_TOUCH_DATA_IO];
ATTR_BACKEND_RAM3 static uint8_t s_allocIDTbl[MAX_TOUCH_ID_ALLOCTBL];
ATTR_BACKEND_RAM3 DEF_TOUCH_OUT s_touch_data_out[ALLOWABLE_TOUCH_DATA_IO];
ATTR_BACKEND_RAM3 static vec_t s_touchPosFilterOut[ALLOWABLE_TOUCH_DATA_IO];
ATTR_BACKEND_RAM3 static vec_t s_prevTouchPosFilterOut[ALLOWABLE_TOUCH_DATA_IO];
#ifdef SMOOTH_SIZE_ENABLE //nsmoon@190729
ATTR_BACKEND_RAM3 static vec_t s_touchSizeFilterOut[ALLOWABLE_TOUCH_DATA_IO];
ATTR_BACKEND_RAM3 static vec_t s_prevTouchSizeFilterOut[ALLOWABLE_TOUCH_DATA_IO];
#endif
ATTR_BACKEND_RAM3 int s_prevTouchCnt, s_touchReportCnt;
ATTR_BACKEND_RAM3 int s_prevTouchCntPen;
ATTR_BACKEND_RAM3 uint8_t s_allocIDTbl_idx;
ATTR_BACKEND_RAM3 static uint32_t s_frame_period, s_prev_frame_period;

#ifdef DEBUG_EXPECTATION_TRACKING
ATTR_BACKEND_RAM3 float xCord0_expect, xCord0_expect_min;
ATTR_BACKEND_RAM3 float yCord0_expect, yCord0_expect_min;
#endif
ATTR_BACKEND_RAM2 float s_ratioXforP2L, s_ratioYforP2L; // ratio for physical coordinates to logical coordinates
ATTR_BACKEND_RAM2 int s_touch_count;
ATTR_BACKEND_RAM2 vec_t *s_touch_data;
ATTR_BACKEND_RAM2 tp_size_t *s_touch_size;
ATTR_BACKEND_RAM2 tp_line_cnt_t *s_touch_lineCnt;
ATTR_BACKEND_RAM2 int8_t *s_multi_fine; //for debugging
ATTR_BACKEND_RAM2 int s_eraserFlag;
ATTR_BACKEND_RAM2 uint8_t s_movSqFlag[ALLOWABLE_TOUCH_DATA_IO];
#ifdef HOR_EDGE_TOUCH_ENABLE
ATTR_BACKEND_RAM2 float s_touch_data_hor_edge_save, s_touch_data_hor_edge_save_bot;
ATTR_BACKEND_RAM2 int s_wheel_delta;
#endif

#ifdef TOUCH_SCROLL_INFO_ENABLE  //nsmoon@200611
ATTR_BACKEND_RAM3 touch_scroll_info_t s_touch_scroll_info[ALLOWABLE_TOUCH_DATA_IO];
ATTR_BACKEND_RAM3 int s_scroll_Flag, s_scroll_State, s_non_Scroll_Flag, s_non_Scroll_cnt, s_scroll_stats_on_delay;
ATTR_BACKEND_RAM3 float wheel_cordX, wheel_cordY;
#endif

ATTR_BACKEND_RAM3 mBz tBz[ALLOWABLE_TOUCH_DATA_IO];
ATTR_BACKEND_RAM3 mBzStatus[ALLOWABLE_TOUCH_DATA_IO];

#if defined(_WIN32) || defined(WIN32) //nsmoon@200224
float s_sensor_zero_x, s_sensor_end_x;
float s_sensor_zero_y, s_sensor_end_y;
float s_aarea_zero_x, s_aarea_end_x;
float s_aarea_zero_y, s_aarea_end_y;
float s_logical_max_x, s_logical_max_y;
float s_pd_pos_x[MAX_SENSOR_POS_SIZE];
float s_pd_pos_y[MAX_SENSOR_POS_SIZE];
float s_led_pos_x[MAX_SENSOR_POS_SIZE];
float s_led_pos_y[MAX_SENSOR_POS_SIZE];
DEF_PD_INFO *s_inBuf;
DEF_TOUCH_OUT s_outBuf[ALLOWABLE_TOUCH_IO];
typedef enum
{
    APP_DRAW_PEN_MARKER=0,
    APP_DRAW_BRUSH,
    APP_DRAW_ERROR
} touch_mode_t;
touch_mode_t touchMode = APP_DRAW_PEN_MARKER;
#endif //defined(_WIN32) || defined(WIN32)

#ifdef SHOW_DEBUG_SIZE
ATTR_BACKEND_RAM int s_debug_minX, s_debug_maxX;
ATTR_BACKEND_RAM int s_debug_minY, s_debug_maxY;
ATTR_BACKEND_RAM unsigned int s_debug_minXY, s_debug_maxXY;
ATTR_BACKEND_RAM int s_debug_minX2, s_debug_maxX2;
ATTR_BACKEND_RAM int s_debug_minY2, s_debug_maxY2;
ATTR_BACKEND_RAM int s_debug_cnt_total;
ATTR_BACKEND_RAM int s_debug_cnt_true;
ATTR_BACKEND_RAM int s_debug_cnt_false;
#endif
#ifdef SHOW_DEBUG_TRACKING
ATTR_BACKEND_RAM uint32_t s_debug_frameperiod;
ATTR_BACKEND_RAM int32_t s_debug_diff_x, s_debug_diff_y;
#endif
#ifdef SHOW_DEBUG_SIZE_VAR
ATTR_BACKEND_RAM int s_debug_down_skip_cnt;
ATTR_BACKEND_RAM int s_debug_up_skip_cnt;
ATTR_BACKEND_RAM float s_debug_sizeSq, s_debug_sizeX, s_debug_sizeY;
ATTR_BACKEND_RAM float s_debug_sizeSqCur, s_debug_sizeXcur, s_debug_sizeYcur;
ATTR_BACKEND_RAM float s_debug_posXcur, s_debug_posYcur;
ATTR_BACKEND_RAM int s_debug_size_type, s_debug_curCnt, s_debug_curStat;
#endif
#if 0 //def SHOW_DEBUG_SIZE_DOWN //for test
ATTR_BACKEND_RAM float s_debug_sizeSq, s_debug_sizeX, s_debug_sizeY;
ATTR_BACKEND_RAM float s_debug_sizeSqCur, s_debug_sizeXcur, s_debug_sizeYcur;
ATTR_BACKEND_RAM float s_debug_posXcur, s_debug_posYcur;
ATTR_BACKEND_RAM int s_debug_size_type, s_debug_curCnt, s_debug_curStat;
ATTR_BACKEND_RAM float s_debug_sizeSqMax, s_debug_sizeSqMin;
ATTR_BACKEND_RAM float s_debug_sizeXMax, s_debug_sizeXMin;
ATTR_BACKEND_RAM float s_debug_sizeYMax, s_debug_sizeYMin;
ATTR_BACKEND_RAM float s_th50RatioX, s_th50RatioY;
ATTR_BACKEND_RAM float s_sizeMin;
ATTR_BACKEND_RAM int s_th10CntMin;
ATTR_BACKEND_RAM float s_sizeMulMin;
ATTR_BACKEND_RAM float s_sizeMulMin_min;
ATTR_BACKEND_RAM float s_sizeMulMin_max;
ATTR_BACKEND_RAM float s_debug_sizeSq_initial, s_debug_sizeSq_initial_max;
ATTR_BACKEND_RAM int s_debug_th50X, s_debug_th50Y, s_debug_th10X, s_debug_th10Y;
ATTR_BACKEND_RAM int s_debug_th50X_used, s_debug_th50Y_used, s_debug_th10X_used, s_debug_th10Y_used;
ATTR_BACKEND_RAM int s_debug_insideArea;
ATTR_BACKEND_RAM float s_debug_th10sizeXcur, s_debug_th10sizeYcur;
ATTR_BACKEND_RAM float s_debug_th10sizeXMin, s_debug_th10sizeXMax;
ATTR_BACKEND_RAM float s_debug_th10sizeYMin, s_debug_th10sizeYMax;
ATTR_BACKEND_RAM float s_debug_sizeWeight;
#endif
#ifdef SHOW_DEBUG_SIZE_DOWN_0205
ATTR_BACKEND_RAM float s_debug_posXcur, s_debug_posYcur;
ATTR_BACKEND_RAM int s_debug_th50X, s_debug_th10X, s_debug_th50Y, s_debug_th10Y;
ATTR_BACKEND_RAM int s_debug_th50X_used, s_debug_th10X_used, s_debug_th50Y_used, s_debug_th10Y_used;
ATTR_BACKEND_RAM float s_debug_sizeXcur, s_debug_th10sizeXcur, s_debug_sizeYcur, s_debug_th10sizeYcur;
ATTR_BACKEND_RAM float s_debug_sizeXMin, s_debug_sizeXMax, s_debug_sizeYMin, s_debug_sizeYMax;
ATTR_BACKEND_RAM float s_debug_th10sizeXMin, s_debug_th10sizeXMax, s_debug_th10sizeYMin, s_debug_th10sizeYMax;
ATTR_BACKEND_RAM float s_debug_X10Y50, s_debug_X50Y10;
ATTR_BACKEND_RAM int s_debug_size_type, s_debug_edge;
#endif

//#define MARKER_POS_REL_ENABLE
#ifdef MARKER_POS_REL_ENABLE //nsmoon@201214
#if defined(_WIN32) || defined(WIN32)
#define DEBUG_MARKER_POS_REL
#endif
#ifdef DEBUG_MARKER_POS_REL
#define TRACE_MPR(...)   //TRACE_RELEASE(__VA_ARGS__)
ATTR_BACKEND_RAM3 int debug_Marker_Pos_Rel_Cnt;
#endif
#define MAX_DISTSQ_MARKER_POS   2500 //900=30x30 //2500=50x50
#define MAX_EXP_MARKER_POS      40 //30 //frame, 50*6ms=300ms
#define MAX_MARKER_POS_REL      10 //touch
#define MAX_TIME_MARKER_POS     200 //ms

typedef struct {
    float x, y;
    int exp; //expired
    unsigned int time;
} marker_pos_rel_t;
ATTR_BACKEND_RAM3 marker_pos_rel_t s_Marker_Pos_Rel[MAX_MARKER_POS_REL];
#endif

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_touchPointSize        1
#endif
#if (DEBUG_touchPointSize > 0)
float s_sizeSqSav;
#endif

#if defined(_WIN32) || defined(WIN32)
static uint32_t preTime = 0;
uint32_t s_get_frame_span(void)
{
    uint32_t curTime = GetTickCount();
    uint32_t difTime = 0;
    if (preTime != 0) {
        difTime = (curTime - preTime);
    }
    preTime = curTime;
    return difTime;
}

static uint32_t preTime2 = 0;
uint32_t s_get_time_diff(void)
{
    uint32_t curTime = GetTickCount();
    uint32_t difTime = 0;
    if (preTime2 != 0) {
        difTime = (curTime - preTime2);
        //TRACE("difTick: %d", difTime);
    }
    preTime2 = curTime;
    return difTime;
}
#else
extern uint32_t s_get_frame_span(void);
extern uint32_t s_get_time_diff(void);
#endif

#if defined(_WIN32) || defined(WIN32)
int tp_delay(uint32_t cnt) { return 0; }
#else
int tp_delay(uint32_t cnt)
{
    volatile uint32_t i = 0;
    uint32_t cntTotal = cnt * 1000;
    for (i = 0; i < cntTotal; ++i)
    {
        __asm(" nop"); /* delay */
    }
    return 0;
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////
#define FRAME_PERIOD_DEFAULT    60 //6ms
void s_init_post_processing(void)
{
	int j;
	touchDataSt_t *pPrevDataIn = &s_prevTouchPosDataIn[0];
    if (s_logical_max_x > (float)0xFFFF) {
        TRACE_ERROR("ERROR! s_init_post_processing..invalid s_logical_max_x %0.2f", s_logical_max_x);
    }
    if (s_logical_max_y > (float)0xFFFF) {
        TRACE_ERROR("ERROR! s_init_post_processing..invalid s_logical_max_y %0.2f", s_logical_max_y);
    }

	//cal coordinate scale
	s_ratioXforP2L = s_logical_max_x / (s_aarea_end_x - s_aarea_zero_x);
	s_ratioYforP2L = s_logical_max_y / (s_aarea_end_y - s_aarea_zero_y);
    if (s_ratioXforP2L < (1.0f - EPSILON)) {
        TRACE_ERROR("ERROR! s_init_post_processing.. invalid s_ratioXforP2L %0.3f", s_ratioXforP2L);
        s_ratioXforP2L = 1.0f;
    }
    if (s_ratioYforP2L < (1.0f - EPSILON)) {
        TRACE_ERROR("ERROR! s_init_post_processing.. invalid s_ratioYforP2L %0.3f", s_ratioYforP2L);
        s_ratioYforP2L = 1.0f;
    }

	//init alloc table for touch id
    memset(&s_allocIDTbl[0], TOUCH_ID_UNALLOC, MAX_TOUCH_ID_ALLOCTBL);

	for (j = 0; j < ALLOWABLE_TOUCH_DATA_IO; j++)
	{
		pPrevDataIn[j].contactID = TOUCH_ID_UNALLOC;
		pPrevDataIn[j].status = TOUCH_NO_STATE;
		pPrevDataIn[j].used = 0; //un-used
	}

	//init tracking variables
	s_prevTouchCnt = 0;
	s_allocIDTbl_idx = MAX_TOUCH_ID_ALLOCTBL;
	s_prev_frame_period = 10; //1ms
	s_frame_period = FRAME_PERIOD_DEFAULT; //6ms
	//frame_34566 =0;

#ifdef SHOW_DEBUG_SIZE_VAR
	s_debug_down_skip_cnt = 0;
	s_debug_size_type = 0;
#endif
#ifdef SHOW_DEBUG_SIZE_DOWN_0205
    //BG_sizeMulMin_min = MIN_INITIAL_VAL;
    //BG_sizeMulMin_max = 0;
    s_debug_sizeXMin = MIN_INITIAL_VAL;
    s_debug_sizeYMin = MIN_INITIAL_VAL;
    s_debug_th10sizeXMin = MIN_INITIAL_VAL;
    s_debug_th10sizeYMin = MIN_INITIAL_VAL;
    s_debug_sizeXMax = 0;
    s_debug_sizeYMax = 0;
    s_debug_th10sizeXMax = 0;
    s_debug_th10sizeYMax = 0;
#endif
#ifdef DEBUG_INITIAL_SIZESQ
    s_debug_sizeSq_initial_max = 0;
#endif
}

void tp_init_post_processing(void)
{
	memset(&s_Touch_ID_Status_Tbl[0], 0x00, sizeof(s_Touch_ID_Status_Tbl));
	memset(&s_Touch_ID_Idx_Tbl[0], 0xFF, sizeof(s_Touch_ID_Idx_Tbl));
	memset(&s_Touch_Big_Size_ID_Tbl[0], 0x00, sizeof(s_Touch_Big_Size_ID_Tbl));

	Move_Status = MOVE_NONE;
	preMove_Status = MOVE_NONE;
	Brush_Size_Status = NORMAL_SIZE;
	preBrush_Size_Status = NORMAL_SIZE;
	Brush_Process = NORMAL_SIZE_PROCESS;
	s_Brush_Big_Size_Frame_Cnt = 0;
	s_Brush_Normal_Size_Frame_Cnt = 0;
	s_Brush_Frame_Cnt = 0;
	s_Brush_Release_Cnt = 0;
#ifdef MARKER_POS_REL_ENABLE //nsmoon@201214
    int i;
    for (i = 0; i < MAX_MARKER_POS_REL; i++) {
        s_Marker_Pos_Rel[i].exp = 0;
    }
#ifdef DEBUG_MARKER_POS_REL
    debug_Marker_Pos_Rel_Cnt = 0;
#endif
#endif
}
////////////////////////////////////////////////////////////////
// Object tracking
////////////////////////////////////////////////////////////////
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_touchPointTracking    0
#endif
#if (DEBUG_touchPointTracking > 1)
#define TRACE_TPT(...)          TRACE(__VA_ARGS__)
#define TRACE_GDS4(...)         //TRACE(__VA_ARGS__)
#define DEBUG_DUMP_TRACKING		DEBUG_dump_tracking
#else
#define TRACE_TPT(...)
#define TRACE_GDS4(...)
#define DEBUG_DUMP_TRACKING
#endif

float tracking_getDistanceSquare(touchDataSt_t *pCur, touchDataSt_t *pPrev, vec_t *pPrevFilterOut) {
    float fDiffX, fDiffY;
    float xPredict, yPredict;
    float distSquare;

    ///////////////////////////////////////////////
    //cal predict position
    fDiffX = pPrev->xVel * s_frame_period;
    fDiffY = pPrev->yVel * s_frame_period;
    IS_DEBUG_FLAG{ TRACE_GDS4("..fDiffX,fDiffY: %f,%f (%f,%f)", fDiffX, fDiffY, pPrev->xVel, pPrev->yVel);};

    xPredict = pPrev->xCord + fDiffX;
    yPredict = pPrev->yCord + fDiffY;
#ifdef DEBUG_EXPECTATION_TRACKING
    xCord0_expect = xPredict;
    yCord0_expect = yPredict;
#endif
    IS_DEBUG_FLAG{ TRACE_GDS4("..xPredict,yPredict: %f,%f", xPredict, yPredict);};

    ///////////////////////////////////////////////
    //cal distance between real and prediction
    fDiffX = pCur->xCord - xPredict;
    fDiffY = pCur->yCord - yPredict;
    distSquare = (fDiffX * fDiffX) + (fDiffY * fDiffY);
    IS_DEBUG_FLAG{ TRACE_GDS4("..distSquare: %f, (%f,%f)", distSquare, fDiffX, fDiffY);};
#ifdef SHOW_DEBUG_TRACKING
    s_debug_frameperiod = s_frame_period;
    s_debug_diff_x = (int32_t) (fDiffX * 10);
    s_debug_diff_y = (int32_t) (fDiffY * 10);
#endif

    return distSquare;
}

#if (DEBUG_touchPointTracking > 0)
static void DEBUG_dump_tracking(int sNo, int curInLength, int prevInLength)
{
    IS_NOT_DEBUG_FLAG return;

	int j;
	vec_t *touch_data = &s_touch_data[0];
	touchDataSt_t *pCurDataIn = &s_touchPosDataIn[0];
	//int curInLength = s_touch_count;
	touchDataSt_t *pPrevDataIn = &s_prevTouchPosDataIn[0];
	vec_t *pPrevFilterOut = &s_prevTouchPosFilterOut[0];
	//int prevInLength = s_prevTouchCnt;

    if (curInLength == 0 && prevInLength == 0) return;

    TRACE("sNo:(%d)%d,%d[%d]", sNo, curInLength, prevInLength, s_frame_no);

	//previous
	for (j = 0; j < prevInLength; j++) {
        TRACE("(p)%d(%02x)%02x(%d,%d)(%d,%d)(%0.1f,%0.1f)", j, pPrevDataIn[j].contactID, pPrevDataIn[j].status, pPrevDataIn[j].pressedCnt, pPrevDataIn[j].releaseCnt, pPrevDataIn[j].used, pPrevDataIn[j].cxRef, pPrevDataIn[j].xCord, pPrevDataIn[j].yCord);
	}

	//current
	for (j = 0; j < curInLength; j++) {
        if (sNo == 0) {
            TRACE("(c)%d(--)--(--,--)(%d,%d)(%0.1f,%0.1f)", j, pCurDataIn[j].used, pCurDataIn[j].cxRef, touch_data[j].x, touch_data[j].y);
        }
        else {
            TRACE("(c)%d(%02x)%02x(%d,%d)(%d,%d)(%0.1f,%0.1f)", j, pCurDataIn[j].contactID, pCurDataIn[j].status, pCurDataIn[j].pressedCnt, pCurDataIn[j].releaseCnt, pCurDataIn[j].used, pCurDataIn[j].cxRef, pCurDataIn[j].xCord, pCurDataIn[j].yCord);
        }
	}

	//allocation table
#if 1
    uint8_t *a = &s_allocIDTbl[0];
    TRACENR("(a)%02x: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x", s_allocIDTbl_idx, \
              a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18], a[19]);
#else
    for (j = 0; j < MAX_TOUCH_ID_ALLOCTBL; j++) {
        TRACENR("%02x ", s_allocIDTbl[j]);
    }
#endif
	TRACE(".");
}
#endif

#if 0 //for test
static int is_already_alloc_id(int len, int id)
{
	touchDataSt_t *pCurDataIn = &s_touchPosDataIn[0];
	int i;

	/////////////////////////////////////
	//checking for same contactID
	for (i = 0; i < len; i++) {
		if (pCurDataIn[i].used == 1 || pCurDataIn[i].contactID == TOUCH_ID_UNALLOC) {
			continue;
		}
		if (pCurDataIn[i].contactID == id) {
			return i; //found
		}
	}
	return -1; //not-found
}
#endif

//#define USE_CAL_ACCELERATION //nsmoon@190820
static int tracking_findContinuosTouch()
{
    touchDataSt_t *pCurDataIn = &s_touchPosDataIn[0];
    int curInLength = s_touch_count;
    touchDataSt_t *pPrevDataIn = &s_prevTouchPosDataIn[0];
    vec_t *pPrevFilterOut = &s_prevTouchPosFilterOut[0];
    int prevInLength = s_prevTouchCnt;

    float xMinVel, yMinVel;
    float cur_d2, d2Min;
    float d2MinLimit = D2MIN_LIMIT_PEN; //FIXME
    int i, j, minIdx, sameIdCnt = 0;
#if (DEBUG_touchPointTracking > 0)
    float maxAccel = 0;
#endif
    //float accelLimit = ACCEL_LIMIT_PEN;
#ifdef USE_CAL_ACCELERATION  //calculate acceleration
    float xAccel, yAccel;
#endif
#ifdef DEBUG_EXPECTATION_TRACKING
    xCord0_expect_min = MIN_INITIAL_VAL;
    yCord0_expect_min = MIN_INITIAL_VAL;
#endif

    for (i = 0; i < curInLength; i++)
    {
		if (pCurDataIn[i].contactID != TOUCH_ID_UNALLOC) {
			//already found
			continue;
		}
		else if (pCurDataIn[i].used == 1) {
			//should be new
			continue;
		}

		d2Min = MIN_INITIAL_VAL_SQ; 
        minIdx = TOUCH_ID_UNALLOC;

        //find previous touch data
        for (j = 0; j < prevInLength; j++)
        {
			if (pPrevDataIn[j].used == 1) {
                continue;
            }
#if 1 //FIXME
			else if (pPrevDataIn[j].status == TOUCH_NO_STATE || pPrevDataIn[j].status == TOUCH_UP_STATE) {
				TRACE_ERROR("ERROR! invalid pPrevDataIn[].status: (%d) %d", pPrevDataIn[j].status);
				continue;
			}
#endif
            IS_DEBUG_FLAG{ TRACE_GDS4(">2<%d,%d(%d)", i, j, pPrevDataIn[j].contactID);}
            //TRACE(">2<%d,%d(%d)", i, j, pPrevDataIn[j].contactID);

            cur_d2 = tracking_getDistanceSquare(&pCurDataIn[i], &pPrevDataIn[j], &pPrevFilterOut[j]);
            IS_DEBUG_FLAG{ TRACE_GDS4("..%d:%d(%0.1f,%0.1f:%0.1f,%0.1f):%0.1f", i, j, pCurDataIn[i].xCord, pCurDataIn[i].yCord, pPrevDataIn[j].xCord, pPrevDataIn[j].yCord, cur_d2);}
            if (cur_d2 < d2Min) {
                d2Min = cur_d2;
				minIdx = j;
            }
#ifdef DEBUG_EXPECTATION_TRACKING //nsmoon@190328
               if (i == 0) {
                   xCord0_expect_min = xCord0_expect;
                   yCord0_expect_min = yCord0_expect;
               }
#ifdef ONE_POINT_FAST_WRITING_TEST
            if (xCord0_expect_min == MIN_INITIAL_VAL) {
            	TRACE_ERROR("@%d", curInLength);
            }
#endif
#endif
        } //for (j = 0; j < prevInLength; j++)

        //found touch data
        if (minIdx != TOUCH_ID_UNALLOC) {
			//cal current velocity
			float fDiffX = pCurDataIn[i].xCord - pPrevDataIn[minIdx].xCord;
			float fDiffY = pCurDataIn[i].yCord - pPrevDataIn[minIdx].yCord;
            IS_DEBUG_FLAG{ TRACE_GDS4("--minIdx,fDiffX,fDiffY: %d, %f, %f (%d)", minIdx, fDiffX, fDiffY, s_frame_period);};

			xMinVel = fDiffX / s_frame_period;
			yMinVel = fDiffY / s_frame_period;
            IS_DEBUG_FLAG{ TRACE_GDS4("--xMinVel,yMinVel:(%f,%f)", xMinVel, yMinVel);};

            //accelLimit = ACCEL_LIMIT_PEN;
            if (pPrevDataIn[minIdx].status == TOUCH_DOWN_STATE) {
                d2MinLimit = D2MIN_LIMIT_PEN;
#ifdef USE_CAL_ACCELERATION  //calculate acceleration
                IS_DEBUG_FLAG{ TRACE_GDS4("--pPrevDataIn[].xVel,yVel: %f, %f", pPrevDataIn[minIdx].xVel, pPrevDataIn[minIdx].yVel);};
                xAccel = xMinVel - pPrevDataIn[minIdx].xVel;
                yAccel = yMinVel - pPrevDataIn[minIdx].yVel;
                maxAccel = (xAccel * xAccel * 100) + (yAccel * yAccel * 100);
                IS_DEBUG_FLAG{ TRACE_GDS4("--maxAccel: %0.3f, %0.3f, %0.3f", maxAccel, xAccel, yAccel);};
                if (d2Min > (D2MIN_ACCEL_MFINGER * 2)) {
                	accelLimit = accelLimit * 4;
                }
                else if (d2Min > D2MIN_ACCEL_MFINGER) {
                	accelLimit = accelLimit * 2;
                }
#endif
            }
            else
            {
                //maxAccel = 0;
                d2MinLimit = D2MIN_LIMIT_INITIAL;
            }
            //IS_DEBUG_FLAG{ TRACE_GDS4("--minIdx,d2Min: %d %f", minIdx, d2Min);};
            //IS_DEBUG_FLAG{ TRACE_GDS4("--minIdx,d2Min,maxAccel: %d %f %f", minIdx, d2Min, maxAccel);};
            //TRACE("---: %f %f", d2Min, maxAccel);

            //copy previous touch data
            //if ((d2Min < d2MinLimit) && (maxAccel < accelLimit))
            if (d2Min < d2MinLimit) //nsmoon@191224
            {
                IS_DEBUG_FLAG{ TRACE_GDS4("=>found: %d(%d) %f, %f)", i, minIdx, d2Min, maxAccel);};

                //copy previous data
                pCurDataIn[i].contactID = pPrevDataIn[minIdx].contactID;
                pCurDataIn[i].status = pPrevDataIn[minIdx].status;
                pCurDataIn[i].pressedCnt = pPrevDataIn[minIdx].pressedCnt;
                pCurDataIn[i].releaseCnt = pPrevDataIn[minIdx].releaseCnt;
#ifdef	PENMARKER_FRAME_OP	//KYJ20200228
                pCurDataIn[i].penCnt = pPrevDataIn[minIdx].penCnt;
                pCurDataIn[i].markerCnt = pPrevDataIn[minIdx].markerCnt ;
#endif

                //update new data
                pCurDataIn[i].sizeType = pPrevDataIn[minIdx].sizeType;
                pCurDataIn[i].distSquare = d2Min;
                pCurDataIn[i].xVel = xMinVel;
                pCurDataIn[i].yVel = yMinVel;
                pCurDataIn[i].cxRef = (int8_t)minIdx; //assigned
                pPrevDataIn[minIdx].cxRef = (int8_t)i;
            }
            else {
                //new touch
                pCurDataIn[i].used = 1; //used
                IS_DEBUG_FLAG{ TRACE_GDS4(">3<%d(%0.1f,%0.1f)", i, d2Min, maxAccel);};
                //TRACE_ERROR("%0.3f ", maxAccel);
            } //if ( (d2Min < d2MinLimit) && (maxAccel < accelLimit) )
        } //if (minIdx != TOUCH_ID_UNALLOC)
        else {
            //new touch
            pCurDataIn[i].used = 1; //used
        }
    } //for (i = 0; i < curInLength; i++)
    //DEBUG_DUMP_TRACKING(2, curInLength, prevInLength);

    /////////////////////////////////////
    //checking for same contactID
    for (i = 0; i < (curInLength - 1); i++) {
        if (pCurDataIn[i].used == 1 || pCurDataIn[i].contactID == TOUCH_ID_UNALLOC) {
            continue;
        }
        for (j = (i + 1); j < curInLength; j++) {
            if (pCurDataIn[j].used == 1 || pCurDataIn[j].contactID == TOUCH_ID_UNALLOC) {
                continue;
            }
            if (pCurDataIn[i].contactID == pCurDataIn[j].contactID) {
                IS_DEBUG_FLAG{ TRACE_TPT("duplicated:%d(%d)%d(%d)", i, pCurDataIn[i].contactID, j, pCurDataIn[j].contactID);}
                if ((pCurDataIn[j].distSquare - pCurDataIn[i].distSquare) > EPSILON) {
                    //pCurDataIn[j].distSquare > pCurDataIn[i].distSquare
                    pCurDataIn[j].contactID = TOUCH_ID_UNALLOC;
                    pCurDataIn[j].cxRef = -1; //un-assign
                }
                else {
                    if ((pCurDataIn[i].distSquare - pCurDataIn[j].distSquare) < EPSILON) {
                        TRACE_ERROR("ERROR! same pCurDataIn[].distSquare: %f, %f", pCurDataIn[i].distSquare, pCurDataIn[j].distSquare);
                    }
                    pCurDataIn[i].contactID = TOUCH_ID_UNALLOC;
                    pCurDataIn[i].cxRef = -1; //un-assign
                }
                sameIdCnt++;
            }
        } //for (j = i + 1; j < curInLength; j++)
    } //for (i = 0; i < (curInLength - 1); i++)

    //mark continuously pressed touch
    for (i = 0; i < curInLength; i++) {
        if (pCurDataIn[i].contactID != TOUCH_ID_UNALLOC) {
            pCurDataIn[i].used = 1; //used
            if (pCurDataIn[i].cxRef < 0) {
                TRACE_ERROR("ERROR! invalid pCurDataIn[].cxRef: %d", i);
            }
            else {
                pPrevDataIn[pCurDataIn[i].cxRef].used = 1; //used
            }
        }
    } //for (i = 0; i < curInLength; i++)

    return sameIdCnt;
}

#if 0 //reserved
static void chk_marker_near_eraser(touchDataSt_t *pCurDataIn, int curInLength)
{
    int i, j;
    float diffX, diffY;
    for (i = 0; i < (curInLength - 1); i++) {
        if (pCurDataIn[i].status == TOUCH_DOWN_STATE && pCurDataIn[i].sizeType == ENUM_SIZE_MARKER) {
            for (j = (i + 1); j < curInLength; j++) {
                if (pCurDataIn[j].status == TOUCH_DOWN_STATE && pCurDataIn[j].sizeType == ENUM_SIZE_ERASER) {
                    diffX = GET_ABS(pCurDataIn[i].xCord - pCurDataIn[j].xCord);
                    diffY = GET_ABS(pCurDataIn[i].yCord - pCurDataIn[j].yCord);
                    if ((diffX - TOUCH_SIZE_BIG_RANGE) < -EPSILON && (diffY - TOUCH_SIZE_BIG_RANGE) < -EPSILON) {
                        pCurDataIn[i].sizeType = ENUM_SIZE_ONE;
                        break;
                    }
                }
            } //for (j = (i + 1); j < curInLength; j++)
        }
    } //for (i = 0; i < (curInLength - 1); i++)
}
#endif
#if 0 //for test
static void chk_eraser(touchDataSt_t *pCurDataIn, int curInLength)
{
    int i, eraser = 0;
    for (i = 0; i < curInLength; i++) {
        if (pCurDataIn[i].status == TOUCH_DOWN_STATE && pCurDataIn[i].sizeType == ENUM_SIZE_ERASER) {
            //TRACE("ERASER!!!");
            eraser++;
            break;
        }
    } //for (i = 0; i < (curInLength - 1); i++)

    if (eraser) {
        for (i = 0; i < curInLength; i++) {
            if (pCurDataIn[i].status == TOUCH_DOWN_STATE && pCurDataIn[i].sizeType != ENUM_SIZE_ERASER) {
                //TRACE("ONE!!!");
                pCurDataIn[i].sizeType = ENUM_SIZE_ONE;
            }
        } //for (i = 0; i < (curInLength - 1); i++)
    }
}
#endif

#if (DEBUG_touchPointSize > 0)
#define TRACE_SIZETYPE(...) TRACE(__VA_ARGS__)
#else
#define TRACE_SIZETYPE(...)
#endif
#define TRACKING_RETRY_MAX      5

static int add_new_touch_id(touchDataSt_t *pCurDataIn)
{
	int j;
    //allocate touch data for newly pressed touch points
    for (j = 0; j < MAX_TOUCH_ID_ALLOCTBL; j++) {
    	s_allocIDTbl_idx++; //increase
        if(s_allocIDTbl_idx >= MAX_TOUCH_ID_ALLOCTBL) {
        	s_allocIDTbl_idx = 0;
        }
        if (s_allocIDTbl[s_allocIDTbl_idx] == TOUCH_ID_UNALLOC || j == (MAX_TOUCH_ID_ALLOCTBL -1)) {
            if (j == (MAX_TOUCH_ID_ALLOCTBL - 1) && s_allocIDTbl[s_allocIDTbl_idx] != TOUCH_ID_UNALLOC) {
                TRACE_ERROR("ERROR! overflow s_allocIDTbl[]"); //overwrite to last
            }
#ifdef SHOW_DEBUG_SIZE_VAR
            if (j == 0) {
            	s_debug_down_skip_cnt = 0;
            	s_debug_up_skip_cnt = 0;
            	s_debug_size_type = 0;
            }
#endif
            IS_DEBUG_FLAG{ TRACE_TPT("new:%d(%02x)", j, s_allocIDTbl_idx);}
            s_allocIDTbl[s_allocIDTbl_idx] = s_allocIDTbl_idx; //cross-ref
            pCurDataIn->contactID = s_allocIDTbl_idx;
            pCurDataIn->status = TOUCH_START_STATE;
            pCurDataIn->pressedCnt = 0;
            pCurDataIn->releaseCnt = 0;
            pCurDataIn->xVel = 0;
            pCurDataIn->yVel = 0;
            pCurDataIn->distSquare = 0;
            pCurDataIn->cxRef = -1; //un-assigned
#ifdef	PENMARKER_FRAME_OP		//KYJ20200228
            pCurDataIn->penCnt = 0;
            pCurDataIn->markerCnt = 0;
#endif
            return 0; //no-error
        }
    } //for (j = 0; j < MAX_TOUCH_ID_ALLOCTBL; j++)
    TRACE_ERROR("ERROR! add_new_touch_id...not possible");
    return 1; //error
}

#ifdef SHOW_DEBUG_SIZE_DOWN_0205
void DEBUG_size_info_0205(touchDataSt_t *pCurDataIn, int mode)
{
    //mode: 1=init, 0=normal
    //int prevIdx = pCurDataIn->cxRef;
    //float sizeSq = pCurDataIn->xSize * pCurDataIn->ySize;
    //uint8_t th50CntX = pCurDataIn->th50CntX;
    //uint8_t th10CntX = pCurDataIn->th10CntX;
    //uint8_t th50CntY = pCurDataIn->th50CntY;
    //uint8_t th10CntY = pCurDataIn->th10CntY;
    float th10WidthX = pCurDataIn->th10WidthX;
    float th10WidthY = pCurDataIn->th10WidthY;
    float sizeX = pCurDataIn->xSize;
    float sizeY = pCurDataIn->ySize;
    //float posX = pCurDataIn->xCord;
    //float posY = pCurDataIn->yCord;

    s_debug_sizeXcur = pCurDataIn->xSize;
    s_debug_sizeYcur = pCurDataIn->ySize;
    //s_debug_sizeSq = sizeSq;
    s_debug_posXcur = pCurDataIn->xCord;
    s_debug_posYcur = pCurDataIn->yCord;
    s_debug_size_type = pCurDataIn->sizeType;
    //s_debug_curStat = pCurDataIn->status;

    s_debug_th50X = s_inBuf->hor_len;
    s_debug_th50Y = s_inBuf->ver_len;
    s_debug_th10X = s_inBuf->threshold_x_cnt;
    s_debug_th10Y = s_inBuf->threshold_y_cnt;
    s_debug_th50X_used = pCurDataIn->th50CntX;
    s_debug_th50Y_used = pCurDataIn->th50CntY;
    s_debug_th10X_used = pCurDataIn->th10CntX;
    s_debug_th10Y_used = pCurDataIn->th10CntY;

    s_debug_th10sizeXcur = th10WidthX;
    s_debug_th10sizeYcur = th10WidthY;
    s_debug_X10Y50 = th10WidthX * sizeY;
    s_debug_X50Y10 = sizeX * th10WidthY;

    if (mode) {
        //s_debug_curCnt = 1;
        //s_debug_sizeSqMax = s_debug_sizeSqMin = sizeSq;
    	s_debug_sizeXMax = s_debug_sizeXMin = pCurDataIn->xSize;
    	s_debug_sizeYMax = s_debug_sizeYMin = pCurDataIn->ySize;
        //s_sizeMulMin_min = MIN_INITIAL_VAL;
        //s_sizeMulMin_max = 0;
    	s_debug_sizeXMin = MIN_INITIAL_VAL;
    	s_debug_sizeXMax = 0;
    	s_debug_sizeYMin = MIN_INITIAL_VAL;
    	s_debug_sizeYMax = 0;
    	s_debug_th10sizeXMin = MIN_INITIAL_VAL;
    	s_debug_th10sizeXMax = 0;
    	s_debug_th10sizeYMin = MIN_INITIAL_VAL;
    	s_debug_th10sizeYMax = 0;
    }
    else {
        //s_debug_curCnt++;
        //s_debug_sizeSqMin = GET_MIN(s_debug_sizeSqMin, sizeSq);
        //s_debug_sizeSqMax = GET_MAX(s_debug_sizeSqMax, sizeSq);
    	s_debug_sizeXMin = GET_MIN(s_debug_sizeXMin, pCurDataIn->xSize);
    	s_debug_sizeXMax = GET_MAX(s_debug_sizeXMax, pCurDataIn->xSize);
    	s_debug_sizeYMin = GET_MIN(s_debug_sizeYMin, pCurDataIn->ySize);
    	s_debug_sizeYMax = GET_MAX(s_debug_sizeYMax, pCurDataIn->ySize);
    	s_debug_th10sizeXMin = GET_MIN(s_debug_th10sizeXMin, s_debug_th10sizeXcur);
    	s_debug_th10sizeXMax = GET_MAX(s_debug_th10sizeXMax, s_debug_th10sizeXcur);
    	s_debug_th10sizeYMin = GET_MIN(s_debug_th10sizeYMin, s_debug_th10sizeYcur);
    	s_debug_th10sizeYMax = GET_MAX(s_debug_th10sizeYMax, s_debug_th10sizeYcur);
    }

    return;
}
#endif

#if (MODEL_SPT == MODEL_CTKS_750_V130) || (MODEL_SPT == MODEL_CTKS_750_V140) || (MODEL_SPT == MODEL_CTSK_850_V100)|| (MODEL_SPT == MODEL_CTSK_650_V100) //nsmoon@191212, nsmoon@201103
#if 0 //reserved
#define CENTER_AREA	    0
#define EDGE_AREA_TOP	1
#define EDGE_AREA_BOT	2
#define EDGE_AREA_RHT	3
#define EDGE_AREA_LFT	4
static int decide_touch_size_750_130(touchDataSt_t *pCurDataIn, int idx, touchDataSt_t *pPrevDataIn, int multiflag)
{
    int prevIdx = pCurDataIn[idx].cxRef;
    float posX = pCurDataIn[idx].xCord;
    float posY = pCurDataIn[idx].yCord;
    //uint8_t th50CntX = pCurDataIn[idx].th50CntX;
    //uint8_t th10CntX = pCurDataIn[idx].th10CntX;
    //uint8_t th50CntY = pCurDataIn[idx].th50CntY;
    //uint8_t th10CntY = pCurDataIn[idx].th10CntY;
    float th10WidthX = pCurDataIn[idx].th10WidthX;
    float th10WidthY = pCurDataIn[idx].th10WidthY;
    float sizeX = pCurDataIn[idx].xSize;
    float sizeY = pCurDataIn[idx].ySize;
    float sizeSq = sizeX * sizeY;
    float pen_marker_size;
    float thX10thY50 = th10WidthX * sizeY;
    float thX50thY10 = sizeX * th10WidthY;

    //TRACE("sizeX,sizeY,sizeSq:%0.1f,%0.1f.%0.1f",sizeX,sizeY,sizeSq);
    //TRACE("th10WidthX,th10WidthY: %0.1f %0.1f",th10WidthX,th10WidthY);
    //TRACE("thX10thY50,thX50thY10: %0.1f %0.1f",thX10thY50,thX50thY10);

    if (posY > TOUCH_AREA_EDGE_TOP || posY < TOUCH_AREA_EDGE_BOT ||
        posX > TOUCH_AREA_EDGE_LFT || posX < TOUCH_AREA_EDGE_RHT) {
        pen_marker_size = MIN_SIZE2_MARKER_EDGE;
        //TRACE_START_SIZE("e");
    }
    else {
        pen_marker_size = MIN_SIZE2_MARKER_CENTER;
        //TRACE_START_SIZE("c");
    }

    if (sizeSq > MIN_SIZE_ERASER) {
    	pCurDataIn[idx].status = TOUCH_DOWN_STATE;
        pCurDataIn[idx].sizeType = ENUM_SIZE_ERASER; //eraser
        //TRACE_START_SIZE("E");
    }
    else if (sizeX < MAX_SIZE_PEN_MARKER || sizeY < MAX_SIZE_PEN_MARKER) {
        if (thX10thY50 > pen_marker_size || thX50thY10 > pen_marker_size) {
            pCurDataIn[idx].sizeType = ENUM_SIZE_MARKER; //marker
            if (prevIdx >= 0) {
            	if (pPrevDataIn[prevIdx].sizeType == pCurDataIn[idx].sizeType) {
            		pCurDataIn[idx].status = TOUCH_DOWN_STATE;
                    //TRACE_START_SIZE("M");
            	}
            }
        }
        else  {
        	pCurDataIn[idx].sizeType = ENUM_SIZE_PEN; //pen
            if (prevIdx >= 0) {
            	if (pPrevDataIn[prevIdx].sizeType == pCurDataIn[idx].sizeType) {
            		pCurDataIn[idx].status = TOUCH_DOWN_STATE;
                    //TRACE_START_SIZE("P");
            	}
            }
        }
    }
    else {
    	pCurDataIn[idx].sizeType = ENUM_SIZE_UNKNOWN; //unknown
        //TRACE_START_SIZE("U");
    }

    return 0;
}
#endif //1

#if 1 //yj
uint8_t IsSide(float fPosX, float fPosY, float *pfXTHD, int iXTHDSize, float *pfYTHD, int iYTHDSize)
{
	if (pfXTHD[1] > fPosX &&
		pfYTHD[1] < fPosY &&
		fPosY < pfYTHD[iYTHDSize - 2])
        return 1;

	if (pfXTHD[iXTHDSize-2] < fPosX &&
		pfYTHD[1] < fPosY &&
		fPosY < pfYTHD[iYTHDSize - 2])
        return 1;

	if (pfYTHD[1] > fPosY &&
		pfXTHD[1] < fPosX &&
		fPosX < pfXTHD[iXTHDSize - 2])
        return 1;

	if (pfYTHD[iYTHDSize-2] < fPosY &&
		pfXTHD[1] < fPosX &&
		fPosX < pfXTHD[iXTHDSize - 2])
        return 1;

    return 0;
}

uint8_t IsEdge(float fPosX, float fPosY, float *pfXTHD, int iXTHDSize, float *pfYTHD, int iYTHDSize)
{
	if (pfXTHD[1] > fPosX &&
		pfYTHD[1] > fPosY)
        return 1;

	if (pfXTHD[iXTHDSize-2] < fPosX &&
		pfYTHD[1] > fPosY)
        return 1;

	if (pfXTHD[1] > fPosX &&
		pfYTHD[iYTHDSize-2] < fPosY)
        return 1;

	if (pfXTHD[iXTHDSize-2] < fPosX &&
		pfYTHD[iYTHDSize-2] < fPosY )
        return 1;

    return 0;
}


float GetSlope(float fPosX1, float fPosY1, float fPosX2, float fPosY2)
{
    float fResult = (fPosY2 - fPosY1)/(fPosX2 - fPosX1);

    return fResult;
}

float GetYIntercept(float fPosX1, float fPosY1, float fPosX2, float fPosY2)
{
    float fResult = (GetSlope(fPosX1, fPosY1, fPosX2, fPosY2) * fPosX1 * -1 )+ fPosY1;

    return fResult;
}


float GetTHD(float *pfXAxisInfo, float *pfYAxisInfo, int iTHDNum, float fXAxisValue)
{

	uint8_t i;

    if (fXAxisValue == 0.0f)
        return pfYAxisInfo[0];
    else if (fXAxisValue >= pfXAxisInfo[iTHDNum-1])
        return pfYAxisInfo[iTHDNum-1];

    for (i = 0; i < iTHDNum - 1; i++)
    {
        if (fXAxisValue >= pfXAxisInfo[i] && fXAxisValue < pfXAxisInfo[i + 1])
            break;
    }

    float fSlope = GetSlope(pfXAxisInfo[i], pfYAxisInfo[i], pfXAxisInfo[i+1], pfYAxisInfo[i+1]);
    float fYIntercept = GetYIntercept(pfXAxisInfo[i], pfYAxisInfo[i], pfXAxisInfo[i+1], pfYAxisInfo[i+1]);

    float fTHD = fSlope * fXAxisValue + fYIntercept;

    return fTHD;
}
#endif

#define PM_EDGE		1
#define PM_SIDE 	2
#define PM_CENTER	0
#if 0
static int decide_touch_size_750_131(touchDataSt_t *pCurDataIn, int idx, touchDataSt_t *pPrevDataIn, int multiflag)
{
    //int prevIdx = pCurDataIn[idx].cxRef;
    float posX = pCurDataIn[idx].xCord;
    float posY = pCurDataIn[idx].yCord;
    float th10WidthX = pCurDataIn[idx].th10WidthX;
    float th10WidthY = pCurDataIn[idx].th10WidthY;
    float sizeX = pCurDataIn[idx].xSize;
    float sizeY = pCurDataIn[idx].ySize;
    float sizeSq = sizeX * sizeY;
    float pen_size_1, pen_size_2;
    float marker_size_1, marker_size_2;
    float eraser_size_1;
    float w1 = th10WidthX * sizeY; //x10y50
    float w2 = sizeX * th10WidthY; //x50y10
    float w = w1 + w2;
    float w_min = GET_MIN(w1, w2);
    float w_max = GET_MAX(w1, w2);
    int size_type = ENUM_SIZE_UNKNOWN;

    uint8_t type =0;

    float vfXAxisInfo[4]={ 0.0f, 300.0f,1400.0f,1706.0f};
    float vfYAxisInfo[4]={ 0.0f, 130.0f,850.0f,983.0f};

#if 0
    uint8_t th50CntX = pCurDataIn[idx].th50CntX;
    uint8_t th10CntX = pCurDataIn[idx].th10CntX;
    uint8_t th50CntY = pCurDataIn[idx].th50CntY;
    uint8_t th10CntY = pCurDataIn[idx].th10CntY;

    uint16_t thXratio = 0;
    uint16_t thYratio = 0;

    if((th50CntX > 0)&&(th10CntX > 0))
	{
    	thXratio = (uint16_t)(((float)th50CntX / (float)th10CntX)*100.0f);
	}
    else
    {
    	thXratio = 0;

    }

    if((th50CntY > 0)&&(th10CntY > 0))
	{
    	thYratio = (uint16_t) (((float)th50CntY / (float)th10CntY)*100.0f);
	}
    else
    {

    	thYratio = 0;
    }
#endif

    if(IsSide(posX, posY, vfXAxisInfo, 4, vfYAxisInfo, 4))
    {
    	type = PM_SIDE;
    }
    else if(IsEdge(posX, posY, vfXAxisInfo, 4, vfYAxisInfo, 4))
    {
    	type = PM_EDGE;
    }
    else
    {
    	type = PM_CENTER;
    }

#ifdef SHOW_DEBUG_SIZE_DOWN_0205
    int debug_size_type = 9; //unknown
    int debug_edge = 0;
#endif

    pCurDataIn[idx].sizeType = size_type; //init

    //TRACE("sizeX,sizeY,sizeSq:%0.1f,%0.1f.%0.1f",sizeX,sizeY,sizeSq);
    //TRACE("th10WidthX,th10WidthY: %0.1f %0.1f",th10WidthX,th10WidthY);
    //TRACE("thX10thY50,thX50thY10: %0.1f %0.1f",thX10thY50,thX50thY10);
#if 0
    if (posY > TOUCH_AREA_EDGE_TOP || posY < TOUCH_AREA_EDGE_BOT ||
        posX > TOUCH_AREA_EDGE_LFT || posX < TOUCH_AREA_EDGE_RHT) {
        pen_size_1 = MAX_SIZE_PEN_1_EDGE;
        pen_size_2 = MAX_SIZE_PEN_2_EDGE;
        marker_size_1 = MAX_SIZE_MARKER_1_EDGE;
        marker_size_2 = MAX_SIZE_MARKER_2_EDGE;
        eraser_size_1 = MAX_SIZE_ERASER_1_EDGE;
        edgeflag = 1;
#ifdef SHOW_DEBUG_SIZE_DOWN_0205
        debug_edge = 1;
        //TRACE_START_SIZE("e");
#endif
    }
    else {
        pen_size_1 = MAX_SIZE_PEN_1;
        pen_size_2 = MAX_SIZE_PEN_2;
        marker_size_1 = MAX_SIZE_MARKER_1;
        marker_size_2 = MAX_SIZE_MARKER_2;
        eraser_size_1 = MAX_SIZE_ERASER_1;
        edgeflag = 0;
#ifdef SHOW_DEBUG_SIZE_DOWN_0205
        debug_edge = 0;
        //TRACE_START_SIZE("c");
#endif
    }
#endif

	if (type == PM_EDGE) {
		pen_size_1 = MAX_SIZE_PEN_1_EDGE;
		pen_size_2 = MAX_SIZE_PEN_2_EDGE;
		marker_size_1 = MAX_SIZE_MARKER_1_EDGE;
		marker_size_2 = MAX_SIZE_MARKER_2_EDGE;
		eraser_size_1 = MAX_SIZE_ERASER_1_EDGE;
	}
	else if(type == PM_SIDE){
		pen_size_1 = MAX_SIZE_PEN_1_SIDE;
		pen_size_2 = MAX_SIZE_PEN_2_SIDE;
		marker_size_1 = MAX_SIZE_MARKER_1_SIDE;
		marker_size_2 = MAX_SIZE_MARKER_2_SIDE;
		eraser_size_1 = MAX_SIZE_ERASER_1_SIDE;
	}
	else
	{
		pen_size_1 = MAX_SIZE_PEN_1;
		pen_size_2 = MAX_SIZE_PEN_2;
		marker_size_1 = MAX_SIZE_MARKER_1;
		marker_size_2 = MAX_SIZE_MARKER_2;
		eraser_size_1 = MAX_SIZE_ERASER_1;
	}

    if (sizeSq > MIN_SIZE_ERASER) {
        pCurDataIn[idx].status = TOUCH_DOWN_STATE;
        size_type = ENUM_SIZE_ERASER; //eraser
#ifdef SHOW_DEBUG_SIZE_DOWN_0205
        debug_size_type = 0; //step-0
        //TRACE_START_SIZE("E");
#endif
    }
    else if (w_min < pen_size_1) {
        pCurDataIn[idx].status = TOUCH_DOWN_STATE;
        size_type = ENUM_SIZE_PEN; //pen
		#ifdef	PENMARKER_FRAME_OP //KYJ20200228
        	pCurDataIn[idx].penCnt++;
		#endif

#ifdef SHOW_DEBUG_SIZE_DOWN_0205
        debug_size_type = 1; //step-1
        //TRACE_START_SIZE("P1");
#endif
    }
    else if (w_max < pen_size_2) {
        pCurDataIn[idx].status = TOUCH_DOWN_STATE;
        size_type = ENUM_SIZE_PEN; //pen
		#ifdef	PENMARKER_FRAME_OP //KYJ20200228
        	pCurDataIn[idx].penCnt++;
		#endif
#ifdef SHOW_DEBUG_SIZE_DOWN_0205
        debug_size_type = 2; //step-2
        //TRACE_START_SIZE("P2");
#endif
    }
    else if (w_max > marker_size_2) {
        pCurDataIn[idx].status = TOUCH_DOWN_STATE;
        size_type = ENUM_SIZE_MARKER; //pen
		#ifdef	PENMARKER_FRAME_OP //KYJ20200228
        	pCurDataIn[idx].markerCnt++;
		#endif
#ifdef SHOW_DEBUG_SIZE_DOWN_0205
        debug_size_type = 3; //step-3
        //TRACE_START_SIZE("P2");
#endif
    }
    else if (w_min > marker_size_1) {
        pCurDataIn[idx].status = TOUCH_DOWN_STATE;
        size_type = ENUM_SIZE_MARKER; //
		#ifdef	PENMARKER_FRAME_OP //KYJ20200228
        	pCurDataIn[idx].markerCnt++;
		#endif
#ifdef SHOW_DEBUG_SIZE_DOWN_0205
        debug_size_type = 4; //step-4
        //TRACE_START_SIZE("P2");
#endif
    }
    else {
        size_type = ENUM_SIZE_UNKNOWN; //unknown
#ifdef SHOW_DEBUG_SIZE_DOWN_0205
        //TRACE_START_SIZE("U");
#endif
    }

    if (size_type == ENUM_SIZE_MARKER) {
        if (w > eraser_size_1) {
            pCurDataIn[idx].sizeType = ENUM_SIZE_ERASER;
#ifdef SHOW_DEBUG_SIZE_DOWN_0205
            debug_size_type = 5; //step-5
            //TRACE_START_SIZE("E1");
#endif
        }
        else {
            pCurDataIn[idx].sizeType = size_type;
        }
    }
    else {
        pCurDataIn[idx].sizeType = size_type;
    }

#ifdef SHOW_DEBUG_SIZE_DOWN_0205
    s_debug_size_type = debug_size_type;
    s_debug_edge = debug_edge;
#endif

    return 0;
}
#endif
#endif

#define MOVING_AVG_FILTER_NUM	200
typedef struct {
	float xCord;
	float yCord;
	float xSize;
	float ySize;
    uint8_t th50CntX;
    uint8_t th10CntX;
    uint8_t th50CntY;
    uint8_t th10CntY;
	float th10WidthX;
	float th10WidthY;
} tTouchSizeInfo_t;

//ATTR_BACKEND_RAM3 static tTouchSizeInfo_t s_vtInputDataBuf[ALLOWABLE_TOUCH_DATA_IO][MOVING_AVG_FILTER_NUM];
//static int s_viInIndex[MOVING_AVG_FILTER_NUM] = {0,};
//static int s_viOutIndex[MOVING_AVG_FILTER_NUM] = {0,};
static tTouchSizeInfo_t s_vtFilteredData[ALLOWABLE_TOUCH_DATA_IO];
#if defined(_WIN32) || defined(WIN32) //nsmoon@200224
#if defined(SHOW_DEBUG_SIZE_DOWN_0205) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
static tTouchSizeInfo_t s_vtFilteredData2[ALLOWABLE_TOUCH_DATA_IO];
int s_testBufCnt[ALLOWABLE_TOUCH_DATA_IO];
#endif
#endif //defined(_WIN32) || defined(WIN32)

void init_pos_info(touchDataSt_t *pCurDataIn)
{
    //if(touchMode != APP_DRAW_PEN_MARKER) return; //FIXME
	uint8_t id = pCurDataIn->contactID;
	s_vtFilteredData[id].xSize = -1;
	s_vtFilteredData[id].ySize = -1;
}

#define TH10_WIDTH_MIN		1.0f
#define TH10_WIDTH_VALUE	1.0f

void add_pos_info(touchDataSt_t *pCurDataIn, int idx)
{
	uint8_t id = pCurDataIn[idx].contactID;
#ifdef FRONTEND_LINE_THRESHOLD
if (pCurDataIn[idx].th10WidthX  < TH10_WIDTH_MIN)
{
	//TRACE("& : %0.2f", pCurDataIn[idx].th10WidthX);
	pCurDataIn[idx].th10WidthX  = TH10_WIDTH_VALUE;
}
if(pCurDataIn[idx].th10WidthY < TH10_WIDTH_MIN)
{
	//TRACE("^ : %0.2f", pCurDataIn[idx].th10WidthY);
	pCurDataIn[idx].th10WidthY  = TH10_WIDTH_VALUE;
}
//if(pCurDataIn[idx].th10CntY == 0 || pCurDataIn[idx].th10CntX == 0 )
//{
//	return ;
//
//}
#endif

if(s_vtFilteredData[id].xSize < 0)
{

	s_vtFilteredData[id].xSize = pCurDataIn[idx].xSize;
	s_vtFilteredData[id].ySize = pCurDataIn[idx].ySize;
#ifdef FRONTEND_LINE_THRESHOLD
	s_vtFilteredData[id].th50CntX  = pCurDataIn[idx].th50CntX;
	s_vtFilteredData[id].th10CntX  = pCurDataIn[idx].th10CntX;
	s_vtFilteredData[id].th50CntY  = pCurDataIn[idx].th50CntY;
	s_vtFilteredData[id].th10CntY  =  pCurDataIn[idx].th10CntY;
	s_vtFilteredData[id].th10WidthX = pCurDataIn[idx].th10WidthX;
	s_vtFilteredData[id].th10WidthY = pCurDataIn[idx].th10WidthY;
#endif
}
else
{
	s_vtFilteredData[id].xSize = (s_vtFilteredData[id].xSize + pCurDataIn[idx].xSize)/2;
	s_vtFilteredData[id].ySize = (s_vtFilteredData[id].ySize +  pCurDataIn[idx].ySize)/2;
#ifdef FRONTEND_LINE_THRESHOLD
	s_vtFilteredData[id].th50CntX  = (s_vtFilteredData[id].th50CntX +  pCurDataIn[idx].th50CntX)/2;
	s_vtFilteredData[id].th10CntX  = (s_vtFilteredData[id].th10CntX +  pCurDataIn[idx].th10CntX)/2;
	s_vtFilteredData[id].th50CntY  = (s_vtFilteredData[id].th50CntY + pCurDataIn[idx].th50CntY)/2;
	s_vtFilteredData[id].th10CntY  = (s_vtFilteredData[id].th10CntY +  pCurDataIn[idx].th10CntY)/2;
	s_vtFilteredData[id].th10WidthX = (s_vtFilteredData[id].th10WidthX +  pCurDataIn[idx].th10WidthX)/2;
	s_vtFilteredData[id].th10WidthY  = (s_vtFilteredData[id].th10WidthY + pCurDataIn[idx].th10WidthY)/2;
}
#endif
}

#if defined(_WIN32) || defined(WIN32) //nsmoon@200224
#if defined(SHOW_DEBUG_SIZE_DOWN_0205) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
void add_pos_info4test(touchDataSt_t *pCurDataIn, int idx)
{
	uint8_t id = pCurDataIn[idx].contactID;
	tTouchSizeInfo_t temp;

#ifdef FRONTEND_LINE_THRESHOLD
if (pCurDataIn[idx].th10WidthX  < TH10_WIDTH_MIN)
{
	//TRACE("& : %0.2f", pCurDataIn[idx].th10WidthX);
	pCurDataIn[idx].th10WidthX  = TH10_WIDTH_VALUE;
}
if(pCurDataIn[idx].th10WidthY < TH10_WIDTH_MIN)
{
	//TRACE("^ : %0.2f", pCurDataIn[idx].th10WidthY);
	pCurDataIn[idx].th10WidthY  = TH10_WIDTH_VALUE;
}
#endif

    //TRACE("%d[%d], %0.2f, %0.2f, %0.2f", id, s_testBufCnt[id], pCurDataIn[idx].xSize, s_vtFilteredData[id].xSize, s_vtFilteredData2[id].xSize );

	switch(s_testBufCnt[id])
	{
	case 0 :
		s_vtFilteredData[id].xSize = pCurDataIn[idx].xSize;
		s_vtFilteredData[id].ySize = pCurDataIn[idx].ySize;
		s_vtFilteredData[id].th50CntX  = pCurDataIn[idx].th50CntX;
		s_vtFilteredData[id].th10CntX  = pCurDataIn[idx].th10CntX;
		s_vtFilteredData[id].th50CntY  = pCurDataIn[idx].th50CntY;
		s_vtFilteredData[id].th10CntY  =  pCurDataIn[idx].th10CntY;
		s_vtFilteredData[id].th10WidthX = pCurDataIn[idx].th10WidthX;
		s_vtFilteredData[id].th10WidthY = pCurDataIn[idx].th10WidthY;
		s_testBufCnt[id]++;
		break;

	case 1 :

		s_vtFilteredData2[id].xSize 		= s_vtFilteredData[id].xSize;
		s_vtFilteredData2[id].ySize 		= s_vtFilteredData[id].ySize;
		s_vtFilteredData2[id].th50CntX  	= s_vtFilteredData[id].th50CntX;
		s_vtFilteredData2[id].th10CntX  	= s_vtFilteredData[id].th10CntX;
		s_vtFilteredData2[id].th50CntY  	= s_vtFilteredData[id].th50CntY;
		s_vtFilteredData2[id].th10CntY  	= s_vtFilteredData[id].th10CntY;
		s_vtFilteredData2[id].th10WidthX 	= s_vtFilteredData[id].th10WidthX;
		s_vtFilteredData2[id].th10WidthY  	= s_vtFilteredData[id].th10WidthY;
		s_vtFilteredData[id].xSize 			= pCurDataIn[idx].xSize;
		s_vtFilteredData[id].ySize 			= pCurDataIn[idx].ySize;
		s_vtFilteredData[id].th50CntX  		= pCurDataIn[idx].th50CntX;
		s_vtFilteredData[id].th10CntX  		= pCurDataIn[idx].th10CntX;
		s_vtFilteredData[id].th50CntY  		= pCurDataIn[idx].th50CntY;
		s_vtFilteredData[id].th10CntY  		= pCurDataIn[idx].th10CntY;
		s_vtFilteredData[id].th10WidthX 	= pCurDataIn[idx].th10WidthX;
		s_vtFilteredData[id].th10WidthY 	= pCurDataIn[idx].th10WidthY;

		pCurDataIn[idx].xSize = (s_vtFilteredData[id].xSize + s_vtFilteredData[id].xSize)/2.0f;
		pCurDataIn[idx].ySize = (s_vtFilteredData[id].ySize +  s_vtFilteredData[id].ySize)/2.0f;
		pCurDataIn[idx].th50CntX  = (s_vtFilteredData[id].th50CntX +  s_vtFilteredData[id].th50CntX)/2.0f;
		pCurDataIn[idx].th10CntX  = (s_vtFilteredData[id].th10CntX +  s_vtFilteredData[id].th10CntX)/2.0f;
		pCurDataIn[idx].th50CntY  = (s_vtFilteredData[id].th50CntY + s_vtFilteredData[id].th50CntY)/2.0f;
		pCurDataIn[idx].th10CntY  = (s_vtFilteredData[id].th10CntY +  s_vtFilteredData[id].th10CntY)/2.0f;
		pCurDataIn[idx].th10WidthX = (s_vtFilteredData[id].th10WidthX +  s_vtFilteredData[id].th10WidthX)/2.0f;
		pCurDataIn[idx].th10WidthY  = (s_vtFilteredData[id].th10WidthY + s_vtFilteredData[id].th10WidthY)/2.0f;
		s_testBufCnt[id]++;
		break;

	case 2 :

		temp.xSize 		= pCurDataIn[idx].xSize;
		temp.ySize 		= pCurDataIn[idx].ySize;
		temp.th50CntX	= pCurDataIn[idx].th50CntX;
		temp.th10CntX	= pCurDataIn[idx].th10CntX;
		temp.th50CntY	= pCurDataIn[idx].th50CntY;
		temp.th10CntY	= pCurDataIn[idx].th10CntY;
		temp.th10WidthX = pCurDataIn[idx].th10WidthX;
		temp.th10WidthY = pCurDataIn[idx].th10WidthY;
		pCurDataIn[idx].xSize = (s_vtFilteredData[id].xSize + s_vtFilteredData[id].xSize +pCurDataIn[idx].xSize)/3.0f;
		pCurDataIn[idx].ySize = (s_vtFilteredData[id].ySize +  s_vtFilteredData[id].ySize +pCurDataIn[idx].ySize)/3.0f;
		pCurDataIn[idx].th50CntX  = (s_vtFilteredData[id].th50CntX +  s_vtFilteredData[id].th50CntX +pCurDataIn[idx].th50CntX)/3.0f;
		pCurDataIn[idx].th10CntX  = (s_vtFilteredData[id].th10CntX +  s_vtFilteredData[id].th10CntX +pCurDataIn[idx].th10CntX)/3.0f;
		pCurDataIn[idx].th50CntY  = (s_vtFilteredData[id].th50CntY + s_vtFilteredData[id].th50CntY +pCurDataIn[idx].th50CntY)/3.0f;
		pCurDataIn[idx].th10CntY  = (s_vtFilteredData[id].th10CntY +  s_vtFilteredData[id].th10CntY +pCurDataIn[idx].th10CntY)/3.0f;
		pCurDataIn[idx].th10WidthX = (s_vtFilteredData[id].th10WidthX +  s_vtFilteredData[id].th10WidthX +pCurDataIn[idx].th10WidthX)/3.0f;
		pCurDataIn[idx].th10WidthY  = (s_vtFilteredData[id].th10WidthY + s_vtFilteredData[id].th10WidthY +pCurDataIn[idx].th10WidthY)/3.0f;

		s_vtFilteredData2[id].xSize 		= s_vtFilteredData[id].xSize;
		s_vtFilteredData2[id].ySize 		= s_vtFilteredData[id].ySize;
		s_vtFilteredData2[id].th50CntX  	= s_vtFilteredData[id].th50CntX;
		s_vtFilteredData2[id].th10CntX  	= s_vtFilteredData[id].th10CntX;
		s_vtFilteredData2[id].th50CntY  	= s_vtFilteredData[id].th50CntY;
		s_vtFilteredData2[id].th10CntY  	= s_vtFilteredData[id].th10CntY;
		s_vtFilteredData2[id].th10WidthX 	= s_vtFilteredData[id].th10WidthX;
		s_vtFilteredData2[id].th10WidthY  	= s_vtFilteredData[id].th10WidthY;
		s_vtFilteredData[id].xSize 			= temp.xSize;
		s_vtFilteredData[id].ySize 			= temp.ySize;
		s_vtFilteredData[id].th50CntX  		= temp.th50CntX;
		s_vtFilteredData[id].th10CntX  		= temp.th10CntX;
		s_vtFilteredData[id].th50CntY  		= temp.th50CntY;
		s_vtFilteredData[id].th10CntY  		= temp.th10CntY;
		s_vtFilteredData[id].th10WidthX 	= temp.th10WidthX;
		s_vtFilteredData[id].th10WidthY 	= temp.th10WidthY;
		break;
	}


}
#endif
#endif //defined(_WIN32) || defined(WIN32)

#if 0
void print_pos_info(int idx, int id)
{

	while(s_viInIndex[idx] != s_viOutIndex[id])
	{
		// print data : out index
		TRACE("%0.2f,%0.2f,%0.2f,%0.2f,%d,%d,%d,%d,%0.2f,%0.2f, %d", s_vtInputDataBuf[idx][s_viOutIndex[id]].xCord,
				s_vtInputDataBuf[idx][s_viOutIndex[id]].yCord, s_vtInputDataBuf[idx][s_viOutIndex[id]].xSize,s_vtInputDataBuf[idx][s_viOutIndex[id]].ySize,
				s_vtInputDataBuf[idx][s_viOutIndex[id]].th50CntX, s_vtInputDataBuf[idx][s_viOutIndex[id]].th10CntX, s_vtInputDataBuf[idx][s_viOutIndex[id]].th50CntY,
				s_vtInputDataBuf[idx][s_viOutIndex[id]].th10CntY,s_vtInputDataBuf[idx][s_viOutIndex[id]].th10WidthX, s_vtInputDataBuf[idx][s_viOutIndex[id]].th10WidthY, id);


		s_viOutIndex[id]++;
		if(s_viOutIndex[id] >= MOVING_AVG_FILTER_NUM) s_viOutIndex[id] = 0;
	}

}

tTouchSizeInfo_t *get_filtered_data(touchDataSt_t *pCurDataIn,int idx)
{

	int iFrameCount = 0;

	while(s_viInIndex[idx] != s_viOutIndex[idx])
	{
		if (iFrameCount == 0)
		{
			iFrameCount++;
			s_viOutIndex[idx]++;
			if(s_viOutIndex[idx] >= MOVING_AVG_FILTER_NUM) s_viOutIndex[idx] = 0;
			continue;
		}
		else
		{
			s_tFilteredData.xSize += s_vtInputDataBuf[idx][s_viOutIndex[idx]].xSize;
			s_tFilteredData.ySize += s_vtInputDataBuf[idx][s_viOutIndex[idx]].ySize;
			s_tFilteredData.th50CntX += s_vtInputDataBuf[idx][s_viOutIndex[idx]].th50CntX;
			s_tFilteredData.th10CntX += s_vtInputDataBuf[idx][s_viOutIndex[idx]].th10CntX;
			s_tFilteredData.th50CntY += s_vtInputDataBuf[idx][s_viOutIndex[idx]].th50CntY;
			s_tFilteredData.th10CntY += s_vtInputDataBuf[idx][s_viOutIndex[idx]].th10CntY;
			s_tFilteredData.th10WidthX += s_vtInputDataBuf[idx][s_viOutIndex[idx]].th10WidthX;
			s_tFilteredData.th10WidthY += s_vtInputDataBuf[idx][s_viOutIndex[idx]].th10WidthY;
			s_tFilteredData.xSize /= 2;
			s_tFilteredData.ySize /= 2;
			s_tFilteredData.th50CntX /= 2;
			s_tFilteredData.th10CntX /= 2;
			s_tFilteredData.th50CntY /= 2;
			s_tFilteredData.th10CntY /= 2;
			s_tFilteredData.th10WidthX /= 2;
			s_tFilteredData.th10WidthY /= 2;
		}
		s_viOutIndex[idx]++;
		if(s_viOutIndex[idx] >= MOVING_AVG_FILTER_NUM) s_viOutIndex[idx] = 0;
	}


	//TRACE("%0.2f,%0.2f,%0.2f,%0.2f,%d,%d,%d,%d,%0.2f,%0.2f, %d", s_tFilteredData.xCord,  s_tFilteredData.yCord, s_tFilteredData.xSize, s_tFilteredData.ySize,
	//													s_tFilteredData.th50CntX, s_tFilteredData.th10CntX, s_tFilteredData.th50CntY, s_tFilteredData.th10CntY,
	//													s_tFilteredData.th10WidthX,s_tFilteredData.th10WidthY,idx);
	s_viInIndex[idx] = s_viOutIndex[idx] = 0;

	return &s_tFilteredData;
}
#endif



#ifdef TOUCH_SCROLL_INFO_ENABLE  //nsmoon@200611
#define SRCOLL_START_AREA_WIDTH		8.0f //8mm
#define IS_SRCOLL_START_AREA_TOP	(pCurDataIn[i].yCord > (s_aarea_end_y - SRCOLL_START_AREA_WIDTH))
#define IS_SRCOLL_START_AREA_BOT	(pCurDataIn[i].yCord < (s_aarea_zero_y + SRCOLL_START_AREA_WIDTH))
#define SRCOLL_DOWN_AREA_WIDTH		30.0f //30mm
#define IS_SRCOLL_DOWN_AREA_TOP		(pCurDataIn[i].yCord > (s_aarea_end_y - SRCOLL_DOWN_AREA_WIDTH))
#define IS_SRCOLL_DOWN_AREA_BOT		(pCurDataIn[i].yCord < (s_aarea_zero_y + SRCOLL_DOWN_AREA_WIDTH))
#define MAX_SRCOLL_ELAPSED_TIME		20000 //2sec
#define TRACE_UTSI(...)		//TRACE(__VA_ARGS__)
#define SCROLL_AREA_TOP	(s_aarea_end_y - SRCOLL_DOWN_AREA_WIDTH)
#define SCROLL_AREA_BOT	(s_aarea_zero_y + SRCOLL_DOWN_AREA_WIDTH)
#define EDGE_Y_UP		(s_aarea_end_y - SRCOLL_START_AREA_WIDTH)	//950.73f	//		958.73 - 8mm
#define EDGE_Y_DOWN		(s_aarea_zero_y + SRCOLL_START_AREA_WIDTH)	//36.48f	//		28.48 + 8mm
static void update_touch_scroll_info(int curInLength)
{
    int i;
    int tId, prevIdx;
	touchDataSt_t *pCurDataIn = &s_touchPosDataIn[0];
	//int curInLength = GET_MIN(s_touchReportCnt, ALLOWABLE_TOUCH_DATA_IO);
	touch_scroll_info_t *pTouchScrollInfo = &s_touch_scroll_info[0];

	for (i = 0; i < curInLength; i++) {
		tId = pCurDataIn[i].contactID;
		prevIdx = pCurDataIn[i].cxRef;
		if (prevIdx < 0) {
			//new touch
			if (IS_SRCOLL_START_AREA_TOP) {
			    pTouchScrollInfo[tId].start_from_scroll_area = START_SCROLL_TOP;
			}
			else if (IS_SRCOLL_START_AREA_BOT) {
			    pTouchScrollInfo[tId].start_from_scroll_area = START_SCROLL_BOT;
			}
			else {
			    pTouchScrollInfo[tId].start_from_scroll_area = START_SCROLL_NONE;
			}
		    TRACE_UTSI("start_from_scroll_area= %d", pTouchScrollInfo[tId].start_from_scroll_area);
			pTouchScrollInfo[tId].out_of_scroll_area = 0; //init
			pTouchScrollInfo[tId].touch_elapsed_time = 0; //init
		}
		else {
			if (pTouchScrollInfo[tId].touch_elapsed_time < MAX_SRCOLL_ELAPSED_TIME) {
				pTouchScrollInfo[tId].touch_elapsed_time += s_frame_period;
				//TRACE_UTSI("touch_elapsed_time= %d (%d)", pTouchScrollInfo[tId].touch_elapsed_time, s_frame_period);
			}
			if(!IS_SRCOLL_START_AREA_TOP && !IS_SRCOLL_START_AREA_BOT)
			{
				pTouchScrollInfo[tId].start_from_scroll_area = START_SCROLL_NONE;
			}
		}
		if (!IS_SRCOLL_DOWN_AREA_TOP && !IS_SRCOLL_DOWN_AREA_BOT) {
		    pTouchScrollInfo[tId].out_of_scroll_area = 1;
		    TRACE_UTSI("out_of_scroll_area= %0.1f, [%d]", pCurDataIn[i].yCord, s_scroll_State);
		}
#if 1 //for test
		if ((pCurDataIn[i].status == TOUCH_UP_STATE /*|| pCurDataIn[i].status == TOUCH_START_STATE */)
				&&	pTouchScrollInfo[tId].start_from_scroll_area != START_SCROLL_NONE	) {
		    TRACE_UTSI("start_from_scroll_area is UP!! %d, , [%d]" , pCurDataIn[i].status, s_scroll_State);
		}
#endif
	}
}

static int is_out_of_scroll_area(void)
{
    int tId;
	touchDataSt_t *pCurDataIn = &s_touchPosDataIn[0];
	int curInLength = GET_MIN(s_touchReportCnt, ALLOWABLE_TOUCH_DATA_IO);
	touch_scroll_info_t *pTouchScrollInfo = &s_touch_scroll_info[0];

	//TRACE_UTSI("curInLength= %d", curInLength);
	if (curInLength == 0) {
		return 0;
	}
	else if (curInLength > 1) {
		return 1;
	}

	tId = pCurDataIn[0].contactID;
	if (pTouchScrollInfo[tId].out_of_scroll_area) {
    	TRACE_UTSI("out_of_scroll_area..");
		return 1; //out-of-scrol-area
	}
	return 0;
}

#define SCROLL_ON   1
#define SCROLL_OFF  2
static int is_start_scroll_area(void)
{
    int tId;
	touchDataSt_t *pCurDataIn = &s_touchPosDataIn[0];
	int curInLength = GET_MIN(s_touchReportCnt, ALLOWABLE_TOUCH_DATA_IO);
	touch_scroll_info_t *pTouchScrollInfo = &s_touch_scroll_info[0];

    if (curInLength == 0) {
		TRACE_UTSI("...No Touch => SCROLL_ON");
	    return SCROLL_ON;//SCROLL_ON; //found
    }
    else/* if (curInLength > 0) */{
		//if (pCurDataIn[0].status == TOUCH_UP_STATE)
		{
			tId = pCurDataIn[0].contactID;
			if (pTouchScrollInfo[tId].start_from_scroll_area != START_SCROLL_NONE &&
				pTouchScrollInfo[tId].touch_elapsed_time < MAX_SRCOLL_ELAPSED_TIME) {
				TRACE_UTSI("...start_from_scroll_area => SCROLL_ON");
				return SCROLL_ON;//SCROLL_ON; //found => scroll
			}
			else {
				TRACE_UTSI("...start_from_touch_area => SCROLL_WAIT");
				return SCROLL_OFF;//SCROLL_OFF; // => non-scroll
			}
		}
	    //else {
	    //	TRACE_UTSI("...No Up status %d %0.2f %0.2f(2) => SCROLL_WAIT", pCurDataIn[0].status, pCurDataIn[0].xCord, pCurDataIn[0].yCord);
	    //	return 2;//SCROLL_OFF;
	    //}
    }

	TRACE_UTSI("..is_start_scroll_area(0)");
	//return 0; //not-found
}
#endif



#ifdef HOR_EDGE_TOUCH_ENABLE
/////////////////////////////////////////////////////////////////////
// convert coordinates to horizontal scroll
/////////////////////////////////////////////////////////////////////
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_s_cal_hor_scroll    0
#endif
#if (DEBUG_s_cal_hor_scroll > 0)
#define TRACE_SCHS(...) TRACE(__VA_ARGS__)
#else
#define TRACE_SCHS(...)
#endif

#define WHEEL_MARGIN	0.9f		//0.50f
#define SCROLL_X_OFFSET	100.0f
#define SCROLL_OUT_CNT	20			//10
#define DISTANCE_TH		5.5f		//2.5f

//#define SCROLL_Y_CHECK

#ifdef SCROLL_Y_CHECK
int wheelCalc(float eData, float *edgeSave, float edgeSavedata)
{
	int Delta;
	float diffPos;

	 if (eData <= 0) {
		 *edgeSave = eData; //reset
		 return 0; //not moved
	 }
	 if (edgeSave <= 0) {
		 *edgeSave = eData; //save
		 return 0; //not moved
	 }
	 //Smooth Filter
	 eData = (WHEEL_MARGIN * eData) + ((1 - WHEEL_MARGIN) * edgeSavedata);
	 diffPos = edgeSavedata - eData;
	 if (GET_ABS(diffPos) < EPSILON) {
		 return 0; //not moved
	 }
	 if (GET_ABS(diffPos) > 2.5f)		//2.5f
	 {
		 *edgeSave = eData; //save
		if(diffPos > 0) Delta = 1;
		else Delta = -1;
	 }
	 else
	 {
		 Delta = 0;
	 }

	 return Delta; //moved
}
#endif

int s_cal_hor_scroll(vec_t edgeData_in)
{

    float edgeData = edgeData_in.x;
    int wheelDelta;
    //touchDataSt_t *pCurDataIn = &s_touchPosDataIn[0];
    int curInLength = GET_MIN(s_touchReportCnt, ALLOWABLE_TOUCH_DATA_IO);
    int Check_Touch;
    int step;

#ifdef SCROLL_Y_CHECK

    uint8_t TopBot_flag;
#else
    float diffPos, abs_diffPos;
#endif

#if 0
    int i;
   // float difftable[11] = {-20.0f, -10.0f, -5.0f, -3.0f, -1.0f, 0.0f, 1.0f, 3.0f, 5.0f, 10.0f, 20.0f};

    touchDataSt_t *pCurDataIn = &s_touchPosDataIn[0];
    int curOutLength = GET_MIN(s_touchReportCnt, ALLOWABLE_TOUCH_DATA_IO);

    for (i = 0; i < curOutLength; i++) {
    	TRACE("[ID: %d]status%02x, ", i, pCurDataIn[i].status);
   		if (pCurDataIn[i].status == TOUCH_DOWN_STATE || pCurDataIn[i].status == TOUCH_START_STATE ||pCurDataIn[i].status == TOUCH_UP_STATE) {
   			return 0; //not moved
           }
    }
#endif

   if(edgeData_in.x == 0.0f ||edgeData_in.y == 0.0f )
   {
		//TRACE_UTSI("%0.2f,  %0.2f",edgeData_in.x, edgeData_in.y);
   }

    wheelDelta = 0;
    switch(s_scroll_State)
    {
    case SCROLL_STATE_OFF :
 //   		if(is_start_scroll_area() || ((curInLength <= 0)&&(edgeData_in.x > 0)) )
    	//Check_Touch = is_start_scroll_area();
    	//if( /*curInLength == 0 && edgeData_in.y ||*/ edgeData_in.x && (edgeData_in.y > EDGE_Y_UP|| edgeData_in.y < EDGE_Y_DOWN))
    	if( edgeData_in.x  )
    	{
    		Check_Touch = is_start_scroll_area();
    		//TRACE_UTSI("Scroll..%0.2f, %0.2f", edgeData_in.x, edgeData_in.y );
            if(Check_Touch == SCROLL_OFF)	//Out 8mm
			{
				TRACE_UTSI("SCROLL_STATE_WAIT : Start");
				//s_non_Scroll_Flag = 1;
				s_scroll_State = SCROLL_STATE_WAIT;
				s_non_Scroll_cnt =0;
				s_touch_data_hor_edge_save = 0;
			}
            else /*if(Check_Touch == SCROLL_ON) //In 8mm*/
			{
				//if(s_non_Scroll_Flag == 0)		//Check Out 8mm
				//{
            	if((edgeData_in.y > EDGE_Y_UP)|| (edgeData_in.y < EDGE_Y_DOWN))
            	{
					//s_scroll_State = SCROLL_STATE_ON;
            		s_scroll_State = SCROLL_STATE_ON_DELAY;
            		s_scroll_stats_on_delay = 0;
				TRACE_UTSI("start Scroll..%d", Check_Touch );
            	}
				//}
			}
			/*else								//Direct Touch
			{
				s_scroll_State = 1;
				TRACE_UTSI("start Scroll..%d", Check_Touch );
			}*/
    	}
    	break;
    case SCROLL_STATE_WAIT :

    	if(curInLength == 0 && edgeData_in.x == 0 && edgeData_in.y == 0)
		{
			if(s_non_Scroll_cnt++ >SCROLL_OUT_CNT)
			{
			s_non_Scroll_cnt = 0;
			//s_non_Scroll_Flag = 0;
			s_scroll_State = 0;
			TRACE_UTSI("s_non_Scroll_Flag : End");
			}

		}
		else s_non_Scroll_cnt =0;
    	break;

    case SCROLL_STATE_ON_DELAY :
    	if( edgeData_in.x  )
    	{
    		if(s_scroll_stats_on_delay++ > 1)
    		{
    			s_scroll_stats_on_delay = 0;
    			s_scroll_State = SCROLL_STATE_ON;
    		}

    	}
    	else
    	{
    		s_scroll_stats_on_delay = 0;
    		s_scroll_State = SCROLL_STATE_OFF;
    	}
    	break;

    case SCROLL_STATE_ON :

    		if(is_out_of_scroll_area() == 1)	//Check 30mm
			{
    			s_non_Scroll_cnt = 0;
				s_scroll_State = SCROLL_STATE_OFF;
				TRACE_UTSI("Scroll..end_1");
				return 0; //not moved
			}
    		if(curInLength == 0 && edgeData_in.x == 0 && edgeData_in.y == 0)
    		{
    			if(s_non_Scroll_cnt++ >SCROLL_OUT_CNT)
				{
					s_non_Scroll_cnt = 0;
					//s_non_Scroll_Flag = 0;
					s_scroll_State = SCROLL_STATE_OFF;

	    			TRACE_UTSI("Scroll..end_2");
				 return 0; //not moved
				}
    		}
    		else s_non_Scroll_cnt =0;

    		//if((edgeData_in.x >= s_sensor_end_x - SCROLL_X_OFFSET)||(edgeData_in.x <= s_sensor_zero_x + SCROLL_X_OFFSET)){
			//   return 0; //not moved
			//}

#ifdef SCROLL_Y_CHECK
    		if ((curInLength > 0)&&(edgeData_in.x == 0))
			{
				if(SCROLL_AREA_TOP > wheel_cordY) TopBot_flag = 1;
				else TopBot_flag = 0;

				edgeData = wheel_cordX;		// Tracking x cord
				TRACE_UTSI("x : %0.2f, %0.2f, y : %0.2f, %0.2f", wheel_cordX, edgeData_in.x, wheel_cordY, edgeData_in.y);
			}
			else
			{
				if(SCROLL_AREA_TOP > edgeData_in.y) TopBot_flag = 1;
				 else TopBot_flag = 0;
				edgeData = edgeData_in.x;
			}
#else

    		if ((curInLength > 0)&&(edgeData_in.x == 0))
    		{
    		    edgeData = wheel_cordX;		// Tracking x cord
    		    TRACE_UTSI("x : %0.2f, %0.2f, y : %0.2f, %0.2f", wheel_cordX, edgeData_in.x, wheel_cordY, edgeData_in.y);
    		}
    		else
    		{
    			edgeData = edgeData_in.x;
    		}
#endif
    	 // if((edgeData_in.x == 0.0f)&&(edgeData_in.y == 0.0f)){
    		// if((edgeData_in.x >= s_sensor_end_x - SCROLL_X_OFFSET)||(edgeData_in.x <= s_sensor_zero_x + SCROLL_X_OFFSET)){
    		// return 0; //not moved
    	   // }
#ifdef SCROLL_Y_CHECK
    		if(TopBot_flag == 1) wheelDelta = wheelCalc(edgeData, &s_touch_data_hor_edge_save, s_touch_data_hor_edge_save);		//TOP
    		else wheelDelta =wheelCalc(edgeData, &s_touch_data_hor_edge_save_bot, s_touch_data_hor_edge_save_bot);				//BOT
#else
		if (edgeData <= 0) {
			s_touch_data_hor_edge_save = edgeData; //reset

			return 0; //not moved
		}
		if (s_touch_data_hor_edge_save <= 0) {
			s_touch_data_hor_edge_save = edgeData; //save
			//s_wheelPos = s_lastWheelPos = 0; //init
			return 0; //not moved
		}
		//Smooth Filter
		edgeData = (WHEEL_MARGIN * edgeData) + ((1 - WHEEL_MARGIN) *s_touch_data_hor_edge_save);
		diffPos = s_touch_data_hor_edge_save - edgeData;
#if (DEBUG_s_cal_hor_scroll > 0)
    if (diffPos > 0) {
    	TRACE("diffPos= %0.1f (%0.1f %0.1f)", diffPos, s_touch_data_hor_edge_save, edgeData);
    }
    else {
    	TRACE("diffPos= -%0.1f (%0.1f %0.1f)", diffPos, s_touch_data_hor_edge_save, edgeData);
    }
#endif
		//s_touch_data_hor_edge_save = edgeData; //save
    	abs_diffPos = GET_ABS(diffPos);
		if (abs_diffPos < EPSILON) {
			return 0; //not moved
		}
		if (abs_diffPos > DISTANCE_TH)		//2.5f	//???? ????		55= 4.5f, 65=5.5f, 75=6.5f, 85=7.5f
		{
			s_touch_data_hor_edge_save = edgeData; //save
#if 1
			if(abs_diffPos / DISTANCE_TH > 3.0f) step = 3;
			else if( abs_diffPos / DISTANCE_TH > 2.0f) step = 2;
			else step = 1;
			if(diffPos > 0) wheelDelta = step;
			else wheelDelta = -step;
#else
			step = 1;
			if(diffPos > 0) wheelDelta = step;
			else wheelDelta = -step;
#endif
		}
		else
		{
			//s_wheel_Cnt = 0;
			wheelDelta = 0;
		}
#endif
    	    //TRACE("%d", wheelDelta);
    	break;
    }
    //wheelDelta = (diffPos > 0) ? (int)(diffPos + 0.5f) : (int)(diffPos - 0.5f);
    return wheelDelta; //moved
}
#endif



#define SIZE50_MIN_TH			2.5f
#define TH5010_RATIO_TH			2.5f

#if 1
#define SIZE_XCORD_L			(s_aarea_end_x *0.17f) +s_aarea_zero_x
#define SIZE_XCORD_R			s_aarea_end_x *0.83f
#define SIZE_YCORD_L			(s_aarea_end_y *0.13f) +s_aarea_zero_y
#define SIZE_YCORD_R			s_aarea_end_y *0.87f
#else
#define SIZE_XCORD_L			s_aarea_end_x *0.17f
#define SIZE_XCORD_R			s_aarea_end_x *0.83f
#define SIZE_YCORD_L			s_aarea_end_y *0.13f
#define SIZE_YCORD_R			s_aarea_end_y *0.87f
#endif

static int decide_touch_size_750_132(touchDataSt_t *pCurDataIn, int idx, touchDataSt_t *pPrevDataIn, int multiflag)
{
	//int prevIdx = pCurDataIn[idx].cxRef;
	    float posX = pCurDataIn[idx].xCord;
	    float posY = pCurDataIn[idx].yCord;
	    float th10WidthX = pCurDataIn[idx].th10WidthX;
	    float th10WidthY = pCurDataIn[idx].th10WidthY;
	    float sizeX = pCurDataIn[idx].xSize;
	    float sizeY = pCurDataIn[idx].ySize;
	    float sizeSq = GET_MIN(sizeX,sizeY)*GET_MIN(sizeX,sizeY);
	    float pen_size_1, pen_size_2;
	    float marker_size_1, marker_size_2;
	    float eraser_size_1;
	    float w1 = th10WidthX * sizeY; //x10y50
	    float w2 = sizeX * th10WidthY; //x50y10
	    float w = w1 + w2;
	    float w_min = GET_MIN(w1, w2);
	    float w_max = GET_MAX(w1, w2);
	    float tmpSizeMax = GET_MAX(pCurDataIn[idx].xSize, pCurDataIn[idx].ySize);
	    float tmpSizeMin = GET_MIN(pCurDataIn[idx].xSize, pCurDataIn[idx].ySize);
        TRACE("w_min,max= %f %f \r\n", w_min, w_max);

	   // float size50Min = GET_MIN(th10WidthX,th10WidthY);
	    //float thX_ratio = pCurDataIn[idx].th50CntX /pCurDataIn[idx].th10CntX;
	    //float thY_ratio = pCurDataIn[idx].th50CntY /pCurDataIn[idx].th10CntY;
	    //float th_min_ratio = GET_MIN(thX_ratio, thY_ratio);
	    int size_type = ENUM_SIZE_UNKNOWN;


	    uint8_t type =0;

#if 0
	    float vfXAxisInfo[4]={ 0.0f, 300.0f,1400.0f,1706.0f};
	    float vfYAxisInfo[4]={ 0.0f, 130.0f,850.0f,983.0f};
#endif
	    float vfXAxisInfo[4]={ s_aarea_zero_x, SIZE_XCORD_L, SIZE_XCORD_R, s_aarea_end_x};
	   	float vfYAxisInfo[4]={ s_aarea_zero_y, SIZE_YCORD_L, SIZE_YCORD_R, s_aarea_end_y};

	    if(IsSide(posX, posY, vfXAxisInfo, 4, vfYAxisInfo, 4))
	    {
	    	type = PM_SIDE;
	    }
	    else if(IsEdge(posX, posY, vfXAxisInfo, 4, vfYAxisInfo, 4))
	    {
	    	type = PM_EDGE;
	    }
	    else
	    {
	    	type = PM_CENTER;
	    }
	    pCurDataIn[idx].sizeType = (size_type_t)size_type; //init


		if (type == PM_EDGE) {
			pen_size_1 = MAX_SIZE_PEN_1_EDGE;
			pen_size_2 = MAX_SIZE_PEN_2_EDGE;
			marker_size_1 = MAX_SIZE_MARKER_1_EDGE;
			marker_size_2 = MAX_SIZE_MARKER_2_EDGE;
			eraser_size_1 = MAX_SIZE_ERASER_1_EDGE;
		}
		else if(type == PM_SIDE){
			pen_size_1 = MAX_SIZE_PEN_1_SIDE;
			pen_size_2 = MAX_SIZE_PEN_2_SIDE;
			marker_size_1 = MAX_SIZE_MARKER_1_SIDE;
			marker_size_2 = MAX_SIZE_MARKER_2_SIDE;
			eraser_size_1 = MAX_SIZE_ERASER_1_SIDE;
		}
		else
		{
			pen_size_1 = MAX_SIZE_PEN_1;
			pen_size_2 = MAX_SIZE_PEN_2;
			marker_size_1 = MAX_SIZE_MARKER_1;
			marker_size_2 = MAX_SIZE_MARKER_2;
			eraser_size_1 = MAX_SIZE_ERASER_1;
		}
		
		if(s_eraserFlag)
		{
			pen_size_2 = MAX_SIZE_PEN_E2;
			marker_size_1 = MAX_SIZE_MARKER_E1;
		}

	    if (sizeSq > MIN_SIZE_ERASER) {
	    	if(sizeSq < MIN_SIZE_B_ERASER )		//Samsung Standard	0x32= 50
			{
	    		size_type = ENUM_SIZE_S_ERASER; //eraser
			}
	    	else
	    	{
	    		 size_type = ENUM_SIZE_ERASER; //eraser
	    	}

	        pCurDataIn[idx].status = TOUCH_DOWN_STATE;


	    }
	    else if (w_min < pen_size_1) {
	        pCurDataIn[idx].status = TOUCH_DOWN_STATE;
	        size_type = ENUM_SIZE_PEN; //pen


	    }
	    else if (w_max < pen_size_2) {
	        pCurDataIn[idx].status = TOUCH_DOWN_STATE;

	        size_type = ENUM_SIZE_PEN; //pen
#if 0

	        if(size50Min < SIZE50_MIN_TH)
	        {
			if(th_min_ratio <TH5010_RATIO_TH)
			{
			 size_type = ENUM_SIZE_MARKER; //pen
			}
			else size_type = ENUM_SIZE_PEN; //pen

			}
			else size_type = ENUM_SIZE_MARKER; //pen
#endif
	    }
	    else if (w_max > marker_size_2) {
	        pCurDataIn[idx].status = TOUCH_DOWN_STATE;
	        size_type = ENUM_SIZE_MARKER; //pen
	    }
	    else if (w_min > marker_size_1) {
	        pCurDataIn[idx].status = TOUCH_DOWN_STATE;
	        size_type = ENUM_SIZE_MARKER; //pen
	    }
	    else {
	        size_type = ENUM_SIZE_UNKNOWN; //unknown
	    }
	    if (size_type == ENUM_SIZE_MARKER) {
	        if (w > eraser_size_1) {
	        	size_type = pCurDataIn[idx].sizeType = ENUM_SIZE_S_ERASER;
                    // pCurDataIn[idx].eraseSize = eraseSize;
	        }
	        else {
	            pCurDataIn[idx].sizeType = (size_type_t)size_type;
	        }
	    }
	    else {
	        pCurDataIn[idx].sizeType = (size_type_t)size_type;
	    }

	    if ((size_type == ENUM_SIZE_S_ERASER)|| (size_type == ENUM_SIZE_ERASER))
	    {

			 if(tmpSizeMax > ERASER_BIG_SIZE)
			 {
				pCurDataIn[idx].eraseSize = (uint16_t)(tmpSizeMin*BIG_ERASER_ADJ);
            }
			 else if(tmpSizeMax > ERASER_MID_SIZE )
			 {
				 pCurDataIn[idx].eraseSize = 50;
            }
			 else
			 {
				 pCurDataIn[idx].eraseSize = 8;
            }
        }


#ifdef TOUCH_SCROLL_INFO_ENABLE
	   if(pCurDataIn[idx].status == TOUCH_DOWN_STATE)
	  	{
			if(s_scroll_State == 1 )
			{
				pCurDataIn[idx].status = TOUCH_START_STATE;
			}
	  	}
#endif
	    return 0;
}

#ifdef MARKER_POS_REL_ENABLE //nsmoon@201214
static int add_marker_pos_rel(touchDataSt_t *pCur)
{
    int i;
    for (i = 0; i < MAX_MARKER_POS_REL; i++) {
        if (s_Marker_Pos_Rel[i].exp < 1) {
            if (s_Marker_Pos_Rel[i].exp < 0) {
                TRACE_ERROR("ERROR! invalid exp");
            }
#ifdef DEBUG_MARKER_POS_REL
            TRACE_MPR("...add=%d(%d)", i, debug_Marker_Pos_Rel_Cnt);
            debug_Marker_Pos_Rel_Cnt++;
#endif
            s_Marker_Pos_Rel[i].x = pCur->xCord;
            s_Marker_Pos_Rel[i].y = pCur->yCord;
            s_Marker_Pos_Rel[i].exp = MAX_EXP_MARKER_POS;
            s_Marker_Pos_Rel[i].time = 0; //reset
            return 1; //add
        }
    }
    TRACE_ERROR("ERROR! add_marker_pos_rel..");
    return 0; //error
}

static int chk_marker_pos_rel(touchDataSt_t *pCur)
{
    float distX, distY, distSq;
    int i;
    for (i = 0; i < MAX_MARKER_POS_REL; i++) {
        if (s_Marker_Pos_Rel[i].exp > 0) {
            distX = pCur->xCord - s_Marker_Pos_Rel[i].x;
            distY = pCur->yCord - s_Marker_Pos_Rel[i].y;
            distSq = (distX * distX) + (distY * distY);
#ifdef DEBUG_MARKER_POS_REL
            TRACE_MPR("...distSq= (%d)%0.1f", i, distSq);
#endif
            if (distSq < MAX_DISTSQ_MARKER_POS) {
#ifdef DEBUG_MARKER_POS_REL
                if (debug_Marker_Pos_Rel_Cnt > 0) debug_Marker_Pos_Rel_Cnt--;
                TRACE_MPR("....find=%d(%d)", i, debug_Marker_Pos_Rel_Cnt);
#endif
                s_Marker_Pos_Rel[i].exp = 0; //expired
                return 1; //found
            }
        }
    }

    return 0; //not-found
}

static void exp_marker_pos_rel(void)
{
    int i;
    for (i = 0; i < MAX_MARKER_POS_REL; i++) {
        if (s_Marker_Pos_Rel[i].exp > 0) {
            s_Marker_Pos_Rel[i].time += (s_frame_period / 10); //ms
            if (s_Marker_Pos_Rel[i].time < MAX_TIME_MARKER_POS) {
                s_Marker_Pos_Rel[i].exp--;
            }
            else {
                s_Marker_Pos_Rel[i].exp = 0;
            }
#ifdef DEBUG_MARKER_POS_REL
            if (s_Marker_Pos_Rel[i].exp == 0) {
                if (debug_Marker_Pos_Rel_Cnt > 0) debug_Marker_Pos_Rel_Cnt--;
                TRACE_MPR("....expired=%d(%d)(%d)", i, s_Marker_Pos_Rel[i].time, debug_Marker_Pos_Rel_Cnt);
            }
#endif
        }
    }
#ifdef DEBUG_MARKER_POS_REL
    if (debug_Marker_Pos_Rel_Cnt < 0) {
        TRACE_ERROR("ERROR! invalid debug_Marker_Pos_Rel_Cnt %d", debug_Marker_Pos_Rel_Cnt);
    }
#endif
}
#endif

#if 1 //nsmoon@200310
static void smooth_filter_init(touchDataSt_t *pCurDataIn)
{
    int tId = pCurDataIn->contactID;
    s_prevTouchPosFilterOut[tId].x = -1; //init
    s_prevTouchPosFilterOut[tId].y = -1; //init
}
#endif

#define DEBUG_BRUSH_MODE_TEST    0 //nsmoon@200226
#if (MODEL_SPT == MODEL_CTSK_850_V100)
	#define TOUCH_DOWN_MIN_MOVEMENT     0.8f	//1.2f	//YJ20200529
#else
    #define TOUCH_DOWN_MIN_MOVEMENT     1.0f//1.2f	//2.0f
#endif
//#define TOUCH_DOWN_MIN_MOVEMENT     1.0f //0.7f
#define TOUCH_DOWN_ADJ_MOVEMENT     0.5f //0.7f

#define TOUCH_START_MIN_MOVEMENT    2.0f
#define TOUCH_START_MAX_MOVEMENT    15.0f
#define TOUCH_DOWN_PRESS_CNT		2
#define TOUCH_DOWN_MIN_MOVEMENT_SQ  (TOUCH_DOWN_MIN_MOVEMENT*TOUCH_DOWN_MIN_MOVEMENT)

#define TOUCH_DOWN_MIN_MOVEMENT2_SQ  (TOUCH_DOWN_MIN_MOVEMENT_SQ - 0.2f)
//#define SIZE_2MM_SELECT_ENABLE
static int update_touch_status(touchDataSt_t *pCurDataIn, int curInLength ,touchDataSt_t *pPrevDataIn)
{
    int i, prevIdx, id;
    float sizeSq, thr10_50X, thr10_50Y, thr10_50;
    uint16_t releaseLimit = RELEASE_LIMIT_PEN; //FIXME
    uint16_t startLimit = START_LIMIT_PEN; //FIXME
#ifdef SIZE_UP_FILTER_ENABLE
    float upsizeCheck;
#endif

    update_touch_scroll_info(curInLength);

    //check multi touch
#if 1 //nsmoon@200305
    s_eraserFlag = 0;
    s_prevTouchCntPen = 0;
    for (i = 0; i < curInLength; i++) {
        sizeSq = pCurDataIn[i].xSize * pCurDataIn[i].ySize;
        if (sizeSq > TOUCH_LARGE_ERASE_SIZE3) {
            s_eraserFlag++;
        }
    }

    //if (multiFlag && s_eraserFlag)
    if (s_eraserFlag)
    {
        startLimit = START_FRAME_ERASE;	//2;	//1; YJ200428		// 0;
    }
    else if (curInLength > 2)
    {
        startLimit = START_FRAME_MULTI3;//START_LIMIT_PEN_SHORT;
    }
    else startLimit = START_FRAME_LIMIT;
#else
    s_eraserFlag = 0;
    startLimit = 2;	//START_FRAME_LIMIT;
#endif

#if 0	//YJ20200513
    int multiFlag = 0;
    for (i = 0; i < curInLength; i++) {
        if (pCurDataIn[i].multi_fine) {
            multiFlag++;
        }
    }
#endif
#if defined(SHOW_DEBUG_SIZE_DOWN_0205) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
    int xSizeMinId = -1;
    float xSizeMin = MIN_INITIAL_VAL;
    for (i = 0; i < curInLength; i++) {
        if (xSizeMin > pCurDataIn[i].xSize) {
            xSizeMin = pCurDataIn[i].xSize;
            xSizeMinId = i;
        }
    }
#endif

    /////////////////////////////////////
    //update touch state
    /////////////////////////////////////
    for (i = 0; i < curInLength; i++) {
        prevIdx = pCurDataIn[i].cxRef;
        id = pCurDataIn[i].contactID;
#ifdef FRONTEND_LINE_THRESHOLD //nsmoon@200206
        if (pCurDataIn[i].th50CntX > 0) {
            thr10_50X = (float)pCurDataIn[i].th10CntX / (float)pCurDataIn[i].th50CntX;
        }
        else {
            thr10_50X = 0;
        }
        if (pCurDataIn[i].th50CntY > 0) {
            thr10_50Y = (float)pCurDataIn[i].th10CntY / (float)pCurDataIn[i].th50CntY;
        }
        else {
            thr10_50Y = 0;
        }
        thr10_50 = GET_MAX(thr10_50X, thr10_50Y);
        //TRACE("START:thr10_50(thr10_50X,thr10_50Y)=%0.2f (%0.2f,%0.2f)", thr10_50, thr10_50X, thr10_50Y);
#endif

#ifdef	TOUCH_SCROLL_INFO_ENABLE
        wheel_cordX = pCurDataIn[i].xCord;
        wheel_cordY = pCurDataIn[i].yCord;
#endif

        switch (pCurDataIn[i].status) {
        case TOUCH_START_STATE:
        {
#if (DEBUG_BRUSH_MODE_TEST == 1)
#if defined(SHOW_DEBUG_SIZE_DOWN_0205) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
            //if (i == 0) {
            if (xSizeMinId >= 0) {
                DEBUG_size_info_0205(&pCurDataIn[i], 1); //1:init
            }
#endif
            pCurDataIn[i].status = TOUCH_DOWN_STATE;
            break;
#endif
            //check down threshold
            //  if (thr10_50 > DOWN_THR10_50_RATIO)

#define THR10_50_RATIO_DOWN	0.17f  //0.2f	//
            if (thr10_50 > THR10_50_RATIO_DOWN)
            {

#if 0
                if (pCurDataIn[i].pressedCnt < TOUCH_MAX_CNT) {
                    pCurDataIn[i].pressedCnt++;
                }

                if (pCurDataIn[i].pressedCnt >= startLimit)
                {
#if (MODEL_SPT == MODEL_IRT_550)
                    decide_touch_size_550(pCurDataIn, i, pPrevDataIn);
#endif
#if (MODEL_SPT == MODEL_SPT_750) || (MODEL_SPT == MODEL_CTKS_750_V120) //nsmoon@191218
                    decide_touch_size_750(pCurDataIn, i, pPrevDataIn, multiFlag);
#endif
#if (MODEL_SPT == MODEL_CTKS_750_V130) || (MODEL_SPT == MODEL_CTKS_750_V140)  //nsmoon@191218, nsmoon@201103
                    decide_touch_size_750_131(pCurDataIn, i, pPrevDataIn, multiFlag);
                    //decide_touch_size_750_test(pCurDataIn, i, pPrevDataIn);
#endif
                }
#else

                if (pCurDataIn[i].pressedCnt < TOUCH_MAX_CNT) {
                    pCurDataIn[i].pressedCnt++;
                }
#if 0
                if(pCurDataIn[i].pressedCnt > 1) add_pos_info(pCurDataIn, i);
                else if(s_eraserFlag) add_pos_info(pCurDataIn, i);
#else

                if(pCurDataIn[i].pressedCnt > 1)
                {
                    add_pos_info(pCurDataIn, i);
                    if(prevIdx >= 0)
                    {
                        pCurDataIn[i].xCord = pPrevDataIn[prevIdx].xCord;
                        pCurDataIn[i].yCord = pPrevDataIn[prevIdx].yCord;
                    }
                }
                else if(s_eraserFlag)
                {
                    add_pos_info(pCurDataIn, i);
                    if(prevIdx >= 0)
                    {
                        pCurDataIn[i].xCord = pPrevDataIn[prevIdx].xCord;
                        pCurDataIn[i].yCord = pPrevDataIn[prevIdx].yCord;
                    }
                }
#endif

                sizeSq = pCurDataIn[i].xSize * pCurDataIn[i].ySize;
                if(sizeSq > MIN_SIZE_ERASER)
                {
                    startLimit = START_FRAME_LIMIT_ERASER;
                }

                //if (pCurDataIn[i].pressedCnt >= startLimit)
                //{
#ifdef SIZE_2MM_SELECT_ENABLE
                if(pCurDataIn[i].pressedCnt >= STAY_FRAME_LIMIT)
                {
                    TRACE("$");
                    decide_touch_size_750_132(pCurDataIn, i, pPrevDataIn, multiFlag);
                    pCurDataIn[i].status = TOUCH_DOWN_STATE;
                    //init_pos_info(i);
                }
                else
#endif
                {

                    //if(i == 0)
                    {

                        if(prevIdx >= 0) {
#ifdef SIZE_2MM_SELECT_ENABLE
                            float diffX = GET_ABS(pCurDataIn[i].xCord - pPrevDataIn[prevIdx].xCord);
                            float diffY = GET_ABS(pCurDataIn[i].yCord - pPrevDataIn[prevIdx].yCord);
                            if (diffX > TOUCH_START_MIN_MOVEMENT || diffY > TOUCH_START_MIN_MOVEMENT)
                            {
#endif
                                //if(diffX > TOUCH_START_MAX_MOVEMENT || diffY > TOUCH_START_MAX_MOVEMENT){
                                if (pCurDataIn[i].pressedCnt >= startLimit) {

                                    //decide_touch_size_750_132(pCurDataIn, i, pPrevDataIn, multiFlag);
                                    //print_pos_info(i, pCurDataIn[i].contactID);

                                    //tTouchSizeInfo_t *ptFilteredData = get_filtered_data(pCurDataIn, i);
                                    if(s_vtFilteredData[id].xSize >= 0)
                                    {
                                        pCurDataIn[i].th10CntX = s_vtFilteredData[id].th10CntX;
                                        pCurDataIn[i].th50CntX = s_vtFilteredData[id].th50CntX;
                                        pCurDataIn[i].th50CntY = s_vtFilteredData[id].th50CntY;
                                        pCurDataIn[i].th10CntY = s_vtFilteredData[id].th10CntY;
                                        pCurDataIn[i].th10WidthX = s_vtFilteredData[id].th10WidthX;
                                        pCurDataIn[i].th10WidthY = s_vtFilteredData[id].th10WidthY;
                                        pCurDataIn[i].xSize = s_vtFilteredData[id].xSize;
                                        pCurDataIn[i].ySize = s_vtFilteredData[id].ySize;

                                        decide_touch_size_750_132(pCurDataIn, i, pPrevDataIn, 0);
#if 0
                                        pCurDataIn[i].xCord = pPrevDataIn[prevIdx].xCord;
                                        pCurDataIn[i].yCord = pPrevDataIn[prevIdx].yCord;
#endif
                                        //pCurDataIn[i].status = TOUCH_DOWN_STATE;


                                        //TRACE("START::%0.2f,%0.2f,%0.2f,%0.2f,%d,%d,%d,%d,%0.2f,%0.2f, %d", pCurDataIn[i].xCord,  pCurDataIn[i].yCord, pCurDataIn[i].xSize, pCurDataIn[i].ySize,
                                        //                     pCurDataIn[i].th50CntX, pCurDataIn[i].th10CntX, pCurDataIn[i].th50CntY, pCurDataIn[i].th10CntY,
                                        //                     pCurDataIn[i].th10WidthX, pCurDataIn[i].th10WidthY, pCurDataIn[i].contactID);
                                    }
                                    else
                                    {
                                        TRACE("@");
                                    }
                                    //									TRACE_TPT("%0.2f,%0.2f,%0.2f,%0.2f,%d,%d,%d,%d,%0.2f,%0.2f, %d", pCurDataIn[i].xCord,  pCurDataIn[i].yCord, pCurDataIn[i].xSize, pCurDataIn[i].ySize,
                                    //													pCurDataIn[i].th50CntX, pCurDataIn[i].th10CntX, pCurDataIn[i].th50CntY, pCurDataIn[i].th10CntY,
//													pCurDataIn[i].th10WidthX, pCurDataIn[i].th10WidthY, pCurDataIn[i].contactID);
                                    //init_pos_info(i);
                                }
#ifdef SIZE_2MM_SELECT_ENABLE
                            }
#endif
#if 0
                            else
                            {
                                pCurDataIn[i].xCord = pPrevDataIn[prevIdx].xCord;
                                pCurDataIn[i].yCord = pPrevDataIn[prevIdx].yCord;
                            }
#endif
                        }
                    }
                }
#endif
            } //if (DOWN_THR10_50_RATIO)
#if defined(SHOW_DEBUG_SIZE_DOWN_0205) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
            //if (i == 0) {
            if (xSizeMinId >= 0) {
                DEBUG_size_info_0205(&pCurDataIn[xSizeMinId], 1); //1:init
            }
#endif
#ifdef MARKER_POS_REL_ENABLE //for test
            if (pCurDataIn[i].status == TOUCH_DOWN_STATE) {
#ifdef DEBUG_MARKER_POS_REL
                TRACE_MPR("(*)TOUCH_DOWN..%d/%d(%f, %f)[%d]", pCurDataIn[i].contactID, pCurDataIn[i].sizeType, pCurDataIn[i].xCord, pCurDataIn[i].yCord, BG_frame_no);
#endif
                if (chk_marker_pos_rel((touchDataSt_t *)&pCurDataIn[i])) {
                    if (pCurDataIn[i].sizeType == ENUM_SIZE_PEN) {
                        pCurDataIn[i].sizeType = ENUM_SIZE_MARKER;
                    }
                }
            }
#endif
        } //case TOUCH_START_STATE
        break;

        case TOUCH_DOWN_STATE:
        {
#ifdef WMC_MODE_CHANGE
            if(touchMode == APP_DRAW_BRUSH) {
                pCurDataIn[i].status = TOUCH_UP_STATE;
                break;
            }
#endif
#if 0 //defined(_WIN32) || defined(WIN32) //1: update size@down-state
#if defined(SHOW_DEBUG_SIZE_DOWN_0205) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
#if (MODEL_SPT == MODEL_CTKS_750_V130) || (MODEL_SPT == MODEL_CTKS_750_V140)  || (MODEL_SPT == MODEL_CTSK_850_V100) //nsmoon@191218, nsmoon@201103
            add_pos_info4test(pCurDataIn, i);
            decide_touch_size_750_132(pCurDataIn, i, pPrevDataIn, 0);
            //decide_touch_size_750_test(pCurDataIn, i, pPrevDataIn);
#endif
            //if (i == 0) {
            if (xSizeMinId >= 0) {
                DEBUG_size_info_0205(&pCurDataIn[i], 0); //1:init
            }
#endif
#endif //defined(_WIN32) || defined(WIN32)

            if (pCurDataIn[i].pressedCnt < TOUCH_MAX_CNT) {
                pCurDataIn[i].pressedCnt++;
            }
//#define MOVEMENT_AVG_FILTER_ENABLE
#ifdef MOVEMENT_AVG_FILTER_ENABLE
            if(curInLength < 3)
            {
                if (prevIdx >= 0) {
                    s_movFilterData[i].xCord = pCurDataIn[i].xCord;
                    s_movFilterData[i].yCord = pCurDataIn[i].yCord;
                    pCurDataIn[i].xCord = (s_movFilterData[prevIdx].xCord + pCurDataIn[i].xCord)/2;
                    pCurDataIn[i].yCord = (s_movFilterData[prevIdx].yCord + pCurDataIn[i].yCord)/2;
                }
            }

#endif
#ifdef SIZE_UP_FILTER_ENABLE
#define THR10_50_RATIO_UP		0.1f//0.25f	//0.1f
#define THR10_50_RATIO_UP_2ND	0.50f
#define UP_CON_SIZE				1.75f	//2.4f//2.20f //REF
#define UP_CON_SIZE_EDGE		1.5f
#define UP_CON_SIZE_SIDE		1.5f
#define UP_CON_SIZE_ERASE		1.7f

            if(s_eraserFlag)
            {
                upsizeCheck =  UP_CON_SIZE_ERASE;
            }
            else
            {
                float vfXAxisInfo[4]= { s_aarea_zero_x, SIZE_XCORD_L, SIZE_XCORD_R, s_aarea_end_x};
                float vfYAxisInfo[4]= { s_aarea_zero_y, SIZE_YCORD_L, SIZE_YCORD_R, s_aarea_end_y};
                if(IsSide(pCurDataIn[i].xCord, pCurDataIn[i].yCord, vfXAxisInfo, 4, vfYAxisInfo, 4))
                {
                    upsizeCheck =  UP_CON_SIZE_SIDE;
                }
                else if(IsEdge(pCurDataIn[i].xCord, pCurDataIn[i].yCord, vfXAxisInfo, 4, vfYAxisInfo, 4))
                {
                    upsizeCheck =  UP_CON_SIZE_EDGE;
                }
                else
                {
                    upsizeCheck =  UP_CON_SIZE;
                }
            }

            if((thr10_50X < THR10_50_RATIO_UP && thr10_50Y < THR10_50_RATIO_UP)
                    ||((thr10_50X < THR10_50_RATIO_UP_2ND || thr10_50Y < THR10_50_RATIO_UP_2ND) && (pCurDataIn[i].xSize < upsizeCheck || pCurDataIn[i].ySize < upsizeCheck)))
            {
                if (prevIdx >= 0) {
                    pCurDataIn[i].xCord = pPrevDataIn[prevIdx].xCord;
                    pCurDataIn[i].yCord = pPrevDataIn[prevIdx].yCord;
                }
            }
#endif


#if 0 //nsmoon@200909, to fix 750 1st sample
            //if (s_eraserFlag)
            {
                if (pCurDataIn[i].th10CntX == 0 && pCurDataIn[i].th10CntY == 0) {
                    if (prevIdx >= 0) {
                        pCurDataIn[i].xCord = pPrevDataIn[prevIdx].xCord;
                        pCurDataIn[i].yCord = pPrevDataIn[prevIdx].yCord;
                    }
                    //pCurDataIn[i].status = TOUCH_UP_STATE;
                }
            }
#endif
            pCurDataIn[i].releaseCnt = 0; //nsmoon@201127
            //TRACE_RELEASE("%d = %d\n\r",i, pCurDataIn[i].sizeType );
            if (pCurDataIn[i].sizeType == ENUM_SIZE_PEN || pCurDataIn[i].sizeType == ENUM_SIZE_MARKER) {
                s_prevTouchCntPen++;
            }
        }
        break;

        case TOUCH_UP_STATE:
        {
#if defined(_WIN32) || defined(WIN32)
#if defined(SHOW_DEBUG_SIZE_DOWN_0205) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
            s_testBufCnt[pCurDataIn[i].contactID]= 0;
#endif
#endif //defined(_WIN32) || defined(WIN32)

#if 1 //reserved, do not remove
            //release cnt
            if (pCurDataIn[i].releaseCnt < TOUCH_MAX_CNT) {
                pCurDataIn[i].releaseCnt++;
            }
            if (pCurDataIn[i].releaseCnt < releaseLimit) {
                pCurDataIn[i].status = TOUCH_DOWN_STATE;
                TRACE_TPT("u");
                if (prevIdx >= 0 && prevIdx < ALLOWABLE_TOUCH_DATA_IO) {
                    //transfer previous position
                    IS_DEBUG_FLAG { TRACE_TPT("change:%d(%d)", i, pCurDataIn[i].contactID);}
                    pCurDataIn[i].xCord = pPrevDataIn[prevIdx].xCord;
                    pCurDataIn[i].yCord = pPrevDataIn[prevIdx].yCord;
                }
                else {
                    TRACE_ERROR("ERROR! invalid prevIdx: %d", prevIdx);
                }
            } //if (pCurDataIn[i].releaseCnt < releaseLimit)

#endif
#ifdef MARKER_POS_REL_ENABLE //for test
            if (pCurDataIn[i].status == TOUCH_UP_STATE && pCurDataIn[i].sizeType == ENUM_SIZE_MARKER) {
#ifdef DEBUG_MARKER_POS_REL
                TRACE_MPR("(*)TOUCH_UP..%d/%d(%f, %f)[%d]", pCurDataIn[i].contactID, pCurDataIn[i].sizeType, pCurDataIn[i].xCord, pCurDataIn[i].yCord, BG_frame_no);
#endif
                add_marker_pos_rel((touchDataSt_t *)&pCurDataIn[i]);
            }
#endif
#if 0 //def SHOW_DEBUG_SIZE_VAR
            if (pCurDataIn[0].status == TOUCH_UP_STATE) {
                s_debug_size_type = 0;
                s_debug_sizeSq = 0;
                s_debug_sizeX = 0;
                s_debug_sizeY = 0;
            }
#endif
        }
        break;

        default:
            TRACE_ERROR("ERROR! unknown touchPosDataIn[].status: %d, %d", i, pCurDataIn[i].status);
            break;
        }
    } //for (i = 0; i < curInLength; i++)
#ifdef MARKER_POS_REL_ENABLE //nsmoon@201214
    exp_marker_pos_rel();
#endif
#if (DEBUG_touchPointTracking > 0)
    DEBUG_DUMP_TRACKING(6, curInLength, prevInLength);
#endif
#if 0 //for demo, FIXME, nsmoon@190805
    //chk_marker_near_eraser(pCurDataIn, curInLength);
    chk_eraser(pCurDataIn, curInLength);
#endif

    return 0;
}

static int update_touch_status_brush(touchDataSt_t *pCurDataIn, int curInLength, touchDataSt_t *pPrevDataIn)
{
    int i;
    /////////////////////////////////////
    //update touch state for Brush
    /////////////////////////////////////
    for (i = 0; i < curInLength; i++) {
        switch (pCurDataIn[i].status) {
        case TOUCH_START_STATE:
        {
            pCurDataIn[i].status = TOUCH_DOWN_STATE;
            pCurDataIn[i].sizeType = ENUM_SIZE_PEN;
        }
            break;

        case TOUCH_DOWN_STATE:
            break;

        case TOUCH_UP_STATE:
            break;

        default:
            TRACE_ERROR("ERROR! unknown touchPosDataIn[].status: %d, %d", i, pCurDataIn[i].status);
            break;
        }
    } //for (i = 0; i < curInLength; i++)

    return 0;
}

#if (UPDATE_TOUCH_STATUS_TEST_ENABLE == 1)
static int update_touch_status_test(touchDataSt_t *pCurDataIn, int curInLength ,touchDataSt_t *pPrevDataIn)
{
    int i, prevIdx; //id;

#if defined(SHOW_DEBUG_SIZE_DOWN_0205) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
    int xSizeMinId = -1;
    float xSizeMin = MIN_INITIAL_VAL;
    for (i = 0; i < curInLength; i++) {
        if (xSizeMin > pCurDataIn[i].xSize) {
            xSizeMin = pCurDataIn[i].xSize;
            xSizeMinId = i;
        }
    }
#endif

    /////////////////////////////////////
    //update touch state
    /////////////////////////////////////
    for (i = 0; i < curInLength; i++) {
        prevIdx = pCurDataIn[i].cxRef;
        //id = pCurDataIn[i].contactID;
#if defined(SHOW_DEBUG_SIZE_DOWN_0205) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
        //if (i == 0) {
        if (xSizeMinId >= 0) {
            DEBUG_size_info_0205(&pCurDataIn[xSizeMinId], 1); //1:init
        }
#endif
        switch (pCurDataIn[i].status) {
        case TOUCH_START_STATE:
            pCurDataIn[i].status = TOUCH_DOWN_STATE;
            pCurDataIn[i].sizeType = ENUM_SIZE_PEN;
            break;

        case TOUCH_DOWN_STATE:
            break;

        case TOUCH_UP_STATE:
            break;

        default:
            TRACE_ERROR("ERROR! unknown touchPosDataIn[].status: %d, %d", i, pCurDataIn[i].status);
            break;
        }
    } //for (i = 0; i < curInLength; i++)

    return 0;
}
#endif
#if 0
static int update_touch_status_test(touchDataSt_t *pCurDataIn, int curInLength ,touchDataSt_t *pPrevDataIn)
{
	int i, prevIdx; //id;
	float thr10_50X, thr10_50Y, thr10_50; //sizeSq
    uint16_t releaseLimit = RELEASE_LIMIT_PEN;
	uint16_t startLimit = START_LIMIT_PEN;

    /////////////////////////////////////
    //update touch state
    /////////////////////////////////////
    for (i = 0; i < curInLength; i++) {
        prevIdx = pCurDataIn[i].cxRef;
    	//id = pCurDataIn[i].contactID;

        if (pCurDataIn[i].th50CntX > EPSILON) {
            thr10_50X = (float)pCurDataIn[i].th10CntX / (float)pCurDataIn[i].th50CntX;
        }
        else {
            thr10_50X = 0;
        }
        if (pCurDataIn[i].th50CntY > EPSILON) {
            thr10_50Y = (float)pCurDataIn[i].th10CntY / (float)pCurDataIn[i].th50CntY;
        }
        else {
            thr10_50Y = 0;
        }
        thr10_50 = GET_MAX(thr10_50X, thr10_50Y);

		switch (pCurDataIn[i].status) {
		case TOUCH_START_STATE:
		{
#if (MODEL_SPT == MODEL_CTSK_850_V100)
#define TEST_THR10_50_RATIO_DOWN	0.1f
#else
#define TEST_THR10_50_RATIO_DOWN	0.1f
#endif
            if (thr10_50 > TEST_THR10_50_RATIO_DOWN)
            {
				if (pCurDataIn[i].pressedCnt <= startLimit) {
					pCurDataIn[i].pressedCnt++;
				}
				else {
					pCurDataIn[i].status = TOUCH_DOWN_STATE;
					pCurDataIn[i].sizeType = ENUM_SIZE_PEN;
				}
        }
    }
			break;

		case TOUCH_DOWN_STATE:
		{
			pCurDataIn[i].releaseCnt = 0;
#if 1 //for testing
#if (MODEL_SPT == MODEL_CTSK_850_V100)
#define TEST_THR10_50_RATIO_UP		0.3f //0.2f
#else
#define TEST_THR10_50_RATIO_UP		0.3f //0.2f
#endif
            if (thr10_50X < TEST_THR10_50_RATIO_UP &&
                    thr10_50Y < TEST_THR10_50_RATIO_UP)
            {
#if 1
				if (prevIdx >= 0) {
					pCurDataIn[i].xCord = pPrevDataIn[prevIdx].xCord;
					pCurDataIn[i].yCord = pPrevDataIn[prevIdx].yCord;
                }
#else
            	pCurDataIn[i].status = TOUCH_UP_STATE;
#endif
            }
#else
#define TOUCH_UP_TH10_CNT	20
            if (pCurDataIn[i].th10CntX <= TOUCH_UP_TH10_CNT &&
            		pCurDataIn[i].th10CntY <= TOUCH_UP_TH10_CNT) {
				if (prevIdx >= 0) {
					pCurDataIn[i].xCord = pPrevDataIn[prevIdx].xCord;
					pCurDataIn[i].yCord = pPrevDataIn[prevIdx].yCord;
                }
            }
            else if (pCurDataIn[i].th10CntX == 0 &&
            		pCurDataIn[i].th10CntY == 0) {
            	pCurDataIn[i].status = TOUCH_UP_STATE;
            }
#endif
		}
            break;

		case TOUCH_UP_STATE:
        {
            if (pCurDataIn[i].releaseCnt <= releaseLimit) {
			    pCurDataIn[i].releaseCnt++;
				pCurDataIn[i].status = TOUCH_DOWN_STATE;
				TRACE_TPT("u");
				if (prevIdx >= 0 && prevIdx < ALLOWABLE_TOUCH_DATA_IO) {
				    //transfer previous position
                    IS_DEBUG_FLAG{ TRACE_TPT("change:%d(%d)", i, pCurDataIn[i].contactID);}
					pCurDataIn[i].xCord = pPrevDataIn[prevIdx].xCord;
					pCurDataIn[i].yCord = pPrevDataIn[prevIdx].yCord;
                }
                else {
					TRACE_ERROR("ERROR! invalid prevIdx: %d", prevIdx);
                }
            }
        }
            break;

        default:
			TRACE_ERROR("ERROR! unknown touchPosDataIn[].status: %d, %d", i, pCurDataIn[i].status);
            break;
    }
	} //for (i = 0; i < curInLength; i++)

    return 0;
}
#endif

void s_touch_point_tracking(void)
{
    int i, j, sameIdCnt;
	vec_t *touch_data = &s_touch_data[0];
    tp_size_t *touch_size = &s_touch_size[0];
	touchDataSt_t *pCurDataIn = &s_touchPosDataIn[0];
    int curInLength = GET_MIN(s_touch_count, ALLOWABLE_TOUCH_DATA_IO);
	touchDataSt_t *pPrevDataIn = &s_prevTouchPosDataIn[0];
    //vec_t *pPrevFilterOut = &s_prevTouchPosFilterOut[0];
#ifdef FRONTEND_LINE_THRESHOLD
    tp_line_cnt_t *touch_line_cnt = &s_touch_lineCnt[0];
#endif
    int8_t *multiFine = &s_multi_fine[0];
	int prevInLength = s_prevTouchCnt;

	/////////////////////////////////////
	//get frame period
    /////////////////////////////////////
    s_prev_frame_period = s_frame_period; //save
    s_frame_period = s_get_frame_span();
    //TRACE("s_frame_period: %d", s_frame_period);

#if defined(_WIN32) || defined(WIN32)
    if (s_frame_period < 10) {
        //TRACE_ERROR("Error! sendPosition...invalid s_frame_period !! %d", s_frame_period);
        s_frame_period = 10; //restore
    } else if (s_frame_period > 100) {
        //TRACE_ERROR("Error! sendPosition...invalid s_frame_period !! %d", s_frame_period);
        s_frame_period = 100; //restore
    }
#endif

    if (s_frame_period < 10) {
        TRACE_ERROR("Error! sendPosition...invalid s_frame_period !! %d", s_frame_period);
        s_frame_period = s_prev_frame_period; //restore
    }
	/////////////////////////////////////
    if (curInLength > 0) {
		TRACE_TPT("In-len: %d %d", prevInLength, curInLength);
    }

    /////////////////////////////////////
    //init pCurDataIn[]
    for (i = 0; i < curInLength; i++)
    {
        pCurDataIn[i].contactID = TOUCH_ID_UNALLOC;
        pCurDataIn[i].xCord = touch_data[i].x;
        pCurDataIn[i].yCord = touch_data[i].y;
        pCurDataIn[i].xSize = touch_size[i].xSize;
        pCurDataIn[i].ySize = touch_size[i].ySize;
#ifdef FRONTEND_LINE_THRESHOLD
        pCurDataIn[i].th50CntX = touch_line_cnt[i].th50CntX;
        pCurDataIn[i].th10CntX = touch_line_cnt[i].th10CntX;
        pCurDataIn[i].th50CntY = touch_line_cnt[i].th50CntY;
        pCurDataIn[i].th10CntY = touch_line_cnt[i].th10CntY;
		pCurDataIn[i].th10WidthX = touch_line_cnt[i].th10WidthX;
		pCurDataIn[i].th10WidthY = touch_line_cnt[i].th10WidthY;
#endif
        pCurDataIn[i].cxRef = -1; //un-assigned
        pCurDataIn[i].used = 0; //un-assigned
        pCurDataIn[i].multi_fine = multiFine[i]; //for debugging
        pCurDataIn[i].sizeType = ENUM_SIZE_UNKNOWN;
        }

#if (DEBUG_touchPointTracking > 0)
	DEBUG_DUMP_TRACKING(0, curInLength, prevInLength);
#endif
    IS_DEBUG_FLAG{
		TRACE_NOP;
    };

	/////////////////////////////////////
	//find continuously pressed points	
    for (i = 0; i < TRACKING_RETRY_MAX; i++) {
        //IS_DEBUG_FLAG{ TRACE_GDS4(">1<%d (%d) [%d]", s_frame_period, s_prev_frame_period, s_frame_no);}
        sameIdCnt = tracking_findContinuosTouch();
        if (sameIdCnt == 0) break;
#if (DEBUG_touchPointTracking > 0)
    DEBUG_DUMP_TRACKING(3, curInLength, prevInLength);
#endif
    }
    if (sameIdCnt != 0) {
        TRACE_ERROR("ERROR! invalid sameIdCnt: %d", sameIdCnt);
    }

    /////////////////////////////////////
    //add new touch
    for (i = 0; i < curInLength; i++) {
        if (pCurDataIn[i].contactID == TOUCH_ID_UNALLOC) {
            //allocate touch data for newly pressed touch points
        	add_new_touch_id((touchDataSt_t *)&pCurDataIn[i]);
            smooth_filter_init((touchDataSt_t *)&pCurDataIn[i]);
			init_pos_info((touchDataSt_t *)&pCurDataIn[i]);
        } //if (pCurDataIn[i].contactID == TOUCH_ID_UNALLOC)
    } //for (i = 0; i < curInLength; i++)
#if (DEBUG_touchPointTracking > 0)
    //DEBUG_DUMP_TRACKING(4, curInLength, prevInLength);
#endif

	/////////////////////////////////////
    //add up touch
    for (j = 0; j < prevInLength; j++)
    {
        if (pPrevDataIn[j].used == 0) {
            pPrevDataIn[j].used = 1; //used previous
            if (pPrevDataIn[j].status == TOUCH_DOWN_STATE) {
                if (curInLength < ALLOWABLE_TOUCH_DATA_IO) {
                pCurDataIn[curInLength].status = TOUCH_UP_STATE;
                pCurDataIn[curInLength].contactID = pPrevDataIn[j].contactID;
                pCurDataIn[curInLength].xCord = pPrevDataIn[j].xCord;
                pCurDataIn[curInLength].yCord = pPrevDataIn[j].yCord;
                pCurDataIn[curInLength].xSize = pPrevDataIn[j].xSize;
                pCurDataIn[curInLength].ySize = pPrevDataIn[j].ySize;
                pCurDataIn[curInLength].xVel = pPrevDataIn[j].xVel;
                pCurDataIn[curInLength].yVel = pPrevDataIn[j].yVel;
                pCurDataIn[curInLength].pressedCnt = pPrevDataIn[j].pressedCnt;
                pCurDataIn[curInLength].releaseCnt = pPrevDataIn[j].releaseCnt;
                pCurDataIn[curInLength].sizeType = pPrevDataIn[j].sizeType;
                pCurDataIn[curInLength].cxRef = (int8_t)j; //assigned
                pPrevDataIn[j].cxRef = (int8_t)curInLength;
                curInLength++;
                    IS_DEBUG_FLAG{ TRACE_TPT("add:%d(%d)(%0.1f,%0.1f)", curInLength-1, pPrevDataIn[j].contactID, pPrevDataIn[j].xCord, pPrevDataIn[j].yCord);};
                }
                else {
                    TRACE_ERROR("ERROR! invalid curInLength %d", curInLength);
                }
            }
        } //(pPrevDataIn[j].used == 0)
    } //for (j = 0; j < prevInLength; j++)
#if (DEBUG_touchPointTracking > 0)
    DEBUG_DUMP_TRACKING(5, curInLength, prevInLength);
#endif
      if(touchMode == APP_DRAW_BRUSH) {
          update_touch_status_brush(pCurDataIn, curInLength, pPrevDataIn);
      }
      else {
#if (UPDATE_TOUCH_STATUS_TEST_ENABLE == 1)
        update_touch_status_test( pCurDataIn, curInLength, pPrevDataIn );
#else
      update_touch_status( pCurDataIn, curInLength, pPrevDataIn );
#endif
      }


    ///////////////////////////////////////////////
    //clear allocation table & save current data
    ///////////////////////////////////////////////
    memset(&s_allocIDTbl[0], TOUCH_ID_UNALLOC, MAX_TOUCH_ID_ALLOCTBL);
    prevInLength = 0; //reset
    for (i = 0; i < curInLength; i++) {
        //update allocation table
    	s_allocIDTbl[pCurDataIn[i].contactID] = pCurDataIn[i].contactID;
        //copy current data to previous data
        if (pCurDataIn[i].status == TOUCH_DOWN_STATE || pCurDataIn[i].status == TOUCH_START_STATE) {
            pPrevDataIn[prevInLength] = pCurDataIn[i];
            pPrevDataIn[prevInLength].used = 0; //reset previous
            pPrevDataIn[prevInLength].cxRef = -1;
            prevInLength++;
        }
    }

	//save count
    s_prevTouchCnt = prevInLength;
    s_touchReportCnt = curInLength;
#if (DEBUG_touchPointTracking > 0)
	DEBUG_DUMP_TRACKING(9, s_touchReportCnt, s_prevTouchCnt);
#endif
    //DEBUG_dump_tracking(9, s_touchReportCnt, s_prevTouchCnt);

#ifdef DEBUG_EXPECTATION_TRACKING
    /////////////////////////////////////
    //add tracking debug
    if (curInLength > 0) {
        if (pCurDataIn[0].status == TOUCH_DOWN_STATE || pCurDataIn[0].status == TOUCH_UP_STATE) {
            if (curInLength < ALLOWABLE_TOUCH_DATA_IO) {
                pCurDataIn[curInLength].contactID = DEBUG_TRACKING_TOUCH_ID;
                if (pCurDataIn[0].status == TOUCH_UP_STATE) {
                    pCurDataIn[curInLength].xCord = pCurDataIn[0].xCord;
                    pCurDataIn[curInLength].yCord = pCurDataIn[0].yCord;
                }
                else {
                    pCurDataIn[curInLength].xCord = xCord0_expect_min;
                    pCurDataIn[curInLength].yCord = yCord0_expect_min;
                }
                pCurDataIn[curInLength].xSize = pCurDataIn[0].xSize;
                pCurDataIn[curInLength].ySize = pCurDataIn[0].ySize;
                pCurDataIn[curInLength].sizeType = pCurDataIn[0].sizeType;
                pCurDataIn[curInLength].status = pCurDataIn[0].status;
                pCurDataIn[curInLength].cxRef = -1; //un-assigned
                s_touchReportCnt++;
            }
        }
    }
#endif

	IS_DEBUG_FLAG{
		//if (s_touchReportCnt > 0)
		TRACE_NOP;
    };
}

#if 1 //nsmoon@200310
void s_smooth_filter(void)
{
    int i;
    int tId;
	touchDataSt_t *pCurDataIn = &s_touchPosDataIn[0];
    int curInLength = GET_MIN(s_touchReportCnt, ALLOWABLE_TOUCH_DATA_IO);
	vec_t *pCurFilterOut = &s_touchPosFilterOut[0];
    vec_t *pPrevFilterOut = &s_prevTouchPosFilterOut[0];
	IS_DEBUG_FLAG{
		TRACE_NOP;
	}

	for (i = 0; i < curInLength; i++) {
#if 0 //YJ20200513
        if (pCurDataIn[i].sizeType == ENUM_SIZE_PEN ) {
            pCurFilterOut[i].x = pCurDataIn[i].xCord;
            pCurFilterOut[i].y = pCurDataIn[i].yCord;
            continue;
        }
#endif
#if defined(SMOOTHING_FILTER_ENABLE)
        tId = pCurDataIn[i].contactID;

#ifdef DEBUG_EXPECTATION_TRACKING //nsmoon@190328
        if (tId == DEBUG_TRACKING_TOUCH_ID) {
            pCurFilterOut[i].x = pCurDataIn[i].xCord;
            pCurFilterOut[i].y = pCurDataIn[i].yCord;
            continue;
		}
#endif
        //if (pCurDataIn[i].status == TOUCH_DOWN_STATE || pCurDataIn[i].status == TOUCH_START_STATE)
        if (pCurDataIn[i].status == TOUCH_DOWN_STATE) //nsmoon@200310
        {
            if (pPrevFilterOut[tId].x < 0) {
                if (!s_eraserFlag) {
                    #ifdef BZ_FILTER_USE
                        memset(&tBz[tId], 0, sizeof(mBz));
                        mBzStatus[tId] = 1;
                        tBz[tId].curPosX[tBz[tId].cnt] = pCurDataIn[i].xCord;
                        tBz[tId].curPosY[tBz[tId].cnt] = pCurDataIn[i].yCord;

                        pCurFilterOut[i].x = tBz[tId].curPosX[tBz[tId].cnt];
                        pCurFilterOut[i].y = tBz[tId].curPosY[tBz[tId].cnt];

                        s_movSqFlag[tId] = 0;
                        tBz[tId].cnt++;
                    #else
                        mBzStatus[tId] = 0;
                        pCurFilterOut[i].x = pCurDataIn[i].xCord;
                        pCurFilterOut[i].y = pCurDataIn[i].yCord;
                        s_movSqFlag[tId] = 0;
                    #endif
                } else {
                    mBzStatus[tId] = 0;
                    pCurFilterOut[i].x = pCurDataIn[i].xCord;
                    pCurFilterOut[i].y = pCurDataIn[i].yCord;
                    s_movSqFlag[tId] = 0;
                }
            }
            else {
                if (mBzStatus[tId]) {
                    #ifdef BZ_FILTER_USE
                        float bzFilterOutX = 0;
                        float bzFilterOutY = 0;
                        bzFilterOutX = pCurDataIn[i].xCord;
                        bzFilterOutY = pCurDataIn[i].yCord;

                        if (tBz[tId].cnt >= BZ_MAX_CNT) {

                            memcpy(&tBz[tId].prePosX, &tBz[tId].curPosX, sizeof(tBz[tId].curPosX));
                            memcpy(&tBz[tId].prePosY, &tBz[tId].curPosY, sizeof(tBz[tId].curPosY));

                            tBz[tId].prePosX[tBz[tId].cnt] = pCurDataIn[i].xCord;
                            tBz[tId].prePosY[tBz[tId].cnt] = pCurDataIn[i].yCord;

                            tBz[tId].step++;
                            tBz[tId].cnt = 0;

                            bzFilterOutX = beizerFilterAnalysis(tBz[tId].reportCnt, tBz[tId].prePosX);
                            bzFilterOutY = beizerFilterAnalysis(tBz[tId].reportCnt, tBz[tId].prePosY);

                            tBz[tId].curPosX[tBz[tId].cnt] = pCurDataIn[i].xCord;
                            tBz[tId].curPosY[tBz[tId].cnt] = pCurDataIn[i].yCord;
                        } else {
                            tBz[tId].curPosX[tBz[tId].cnt] = pCurDataIn[i].xCord;
                            tBz[tId].curPosY[tBz[tId].cnt] = pCurDataIn[i].yCord;

                            if (tBz[tId].cnt == (BZ_MAX_CNT-1)) {
                                tBz[tId].reportCnt = 0;
                                bzFilterOutX = beizerFilterAnalysis(tBz[tId].reportCnt, tBz[tId].curPosX);
                                bzFilterOutY = beizerFilterAnalysis(tBz[tId].reportCnt, tBz[tId].curPosY);
                                tBz[tId].reportCnt++;
                            } else {
                                if (tBz[tId].step > 0) {
                                    bzFilterOutX = beizerFilterAnalysis(tBz[tId].reportCnt, tBz[tId].prePosX);
                                    bzFilterOutY = beizerFilterAnalysis(tBz[tId].reportCnt, tBz[tId].prePosY);
                                    tBz[tId].reportCnt++;
                                }
                            }
                        }
                        tBz[tId].cnt++;

                        if (!bzFilterOutX || !bzFilterOutY ) {
                            return;
                        }

                        pCurFilterOut[i].x = bzFilterOutX;
                        pCurFilterOut[i].y = bzFilterOutY;

                    #else
                        pCurFilterOut[i].x = (A0_OUT_FINE * pCurDataIn[i].xCord) + ((1 - A0_OUT_FINE) * pPrevFilterOut[tId].x);
                        pCurFilterOut[i].y = (A0_OUT_FINE * pCurDataIn[i].yCord) + ((1 - A0_OUT_FINE) * pPrevFilterOut[tId].y);
                        //TRACE("X:%0.1f=%0.1f+%0.1f", pCurFilterOut[i].x, pCurDataIn[i].xCord, pPrevFilterOut[tid].x);
                        //TRACE("Y:%0.1f=%0.1f+%0.1f", pCurFilterOut[i].y, pCurDataIn[i].yCord, pPrevFilterOut[tid].y);
                        if (s_eraserFlag == 0)
                        {
                            float diffX = pCurFilterOut[i].x - pPrevFilterOut[tId].x;
                            float diffY = pCurFilterOut[i].y - pPrevFilterOut[tId].y;
                            float distSquare = (diffX * diffX) + (diffY * diffY);
                            if (distSquare < TOUCH_DOWN_MIN_MOVEMENT_SQ) {
                                if(++s_movSqFlag[tId] > 10 )
                                {
                                    s_movSqFlag[tId] = 0;
                                }
                                else
                                {
                                    pCurFilterOut[i].x = pPrevFilterOut[tId].x;
                                    pCurFilterOut[i].y = pPrevFilterOut[tId].y;
                                }
                            }
                            else
                            {
                                s_movSqFlag[tId] = 0;
                            }
                        }
                    #endif
                } else {
                    pCurFilterOut[i].x = (A0_ERASE_OUT_FINE * pCurDataIn[i].xCord) + ((1 - A0_ERASE_OUT_FINE) * pPrevFilterOut[tId].x);
                    pCurFilterOut[i].y = (A0_ERASE_OUT_FINE * pCurDataIn[i].yCord) + ((1 - A0_ERASE_OUT_FINE) * pPrevFilterOut[tId].y);
                    //TRACE("X:%0.1f=%0.1f+%0.1f", pCurFilterOut[i].x, pCurDataIn[i].xCord, pPrevFilterOut[tid].x);
                    //TRACE("Y:%0.1f=%0.1f+%0.1f", pCurFilterOut[i].y, pCurDataIn[i].yCord, pPrevFilterOut[tid].y);
                    if (s_eraserFlag == 0)
                    {
                        float diffX = pCurFilterOut[i].x - pPrevFilterOut[tId].x;
                        float diffY = pCurFilterOut[i].y - pPrevFilterOut[tId].y;
                        float distSquare = (diffX * diffX) + (diffY * diffY);
                        if (distSquare < TOUCH_DOWN_MIN_MOVEMENT_SQ) {
                            if(++s_movSqFlag[tId] > 10 )
                            {
                                s_movSqFlag[tId] = 0;
                            }
                            else
                            {
                                pCurFilterOut[i].x = pPrevFilterOut[tId].x;
                                pCurFilterOut[i].y = pPrevFilterOut[tId].y;
                            }
                        }
                        else
                        {
                            s_movSqFlag[tId] = 0;
                        }
                    }
                }
            }
            pPrevFilterOut[tId] = pCurFilterOut[i];
        }
        else if (pCurDataIn[i].status == TOUCH_UP_STATE)
        {
#ifdef TOUCH_DOWN_MIN_MOVEMENT //nsmoon@200227
            if (pPrevFilterOut[tId].x >= 0) {
                if (s_eraserFlag) {
                    pCurFilterOut[i].x = pCurDataIn[i].xCord;
                    pCurFilterOut[i].y = pCurDataIn[i].yCord;
                }
                else {
                    pCurFilterOut[i].x = pPrevFilterOut[tId].x;
                    pCurFilterOut[i].y = pPrevFilterOut[tId].y;
                }
            }
            else
#endif
            {
                pCurFilterOut[i].x = pCurDataIn[i].xCord;
                pCurFilterOut[i].y = pCurDataIn[i].yCord;
            }
        }
#else
        pCurFilterOut[i].x = pCurDataIn[i].xCord;
        pCurFilterOut[i].y = pCurDataIn[i].yCord;
#endif
    } //for (i = 0; i < curInLength; i++)
}
#else
void s_smooth_filter(void)
{
    int i, prevIdx;
	touchDataSt_t *pCurDataIn = &s_touchPosDataIn[0];
    int curInLength = GET_MIN(s_touchReportCnt, ALLOWABLE_TOUCH_DATA_IO);
	vec_t *pCurFilterOut = &s_touchPosFilterOut[0];
	vec_t *pPrevFilterOut = &s_prevTouchPosFilterOut[0];
#ifdef SMOOTH_SIZE_ENABLE //nsmoon@190729
	vec_t *pCurFilterOutSize = &s_touchSizeFilterOut[0];
	vec_t *pPrevFilterOutSize = &s_prevTouchSizeFilterOut[0];
#endif
	IS_DEBUG_FLAG{
		TRACE_NOP;
	}

	for (i = 0; i < curInLength; i++) {
#ifdef DEBUG_EXPECTATION_TRACKING //nsmoon@190328
		if (pCurDataIn[i].contactID == DEBUG_TRACKING_TOUCH_ID) {
            pCurFilterOut[i].x = pCurDataIn[i].xCord;
            pCurFilterOut[i].y = pCurDataIn[i].yCord;
#ifdef SMOOTH_SIZE_ENABLE //nsmoon@190729
            pCurFilterOutSize[i].x = pCurDataIn[i].xSize;
            pCurFilterOutSize[i].y = pCurDataIn[i].ySize;
#endif
            continue;
		}
#endif
#if defined(SMOOTHING_FILTER_ENABLE)
        prevIdx = pCurDataIn[i].cxRef;
        if (pCurDataIn[i].status == TOUCH_DOWN_STATE || pCurDataIn[i].status == TOUCH_START_STATE)
        {
            if (prevIdx >= 0) {
                pCurFilterOut[i].x = (A0_OUT_FINE * pCurDataIn[i].xCord) + ((1 - A0_OUT_FINE) * pPrevFilterOut[prevIdx].x);
                pCurFilterOut[i].y = (A0_OUT_FINE * pCurDataIn[i].yCord) + ((1 - A0_OUT_FINE) * pPrevFilterOut[prevIdx].y);
                //TRACE("X:%0.1f=%0.1f+%0.1f", pCurFilterOut[i].x, pCurDataIn[i].xCord, pPrevFilterOut[prevIdx].x);
                //TRACE("Y:%0.1f=%0.1f+%0.1f", pCurFilterOut[i].y, pCurDataIn[i].yCord, pPrevFilterOut[prevIdx].y);
#ifdef SMOOTH_SIZE_ENABLE //nsmoon@190729
                if (1 /*pPrevFilterOutSize[i].x > 2.0f || pPrevFilterOutSize[i].y > 2.0f*/)
                {
                    pCurFilterOutSize[i].x = (A0_OUT_FINE_SIZE * pCurDataIn[i].xSize + (1 - A0_OUT_FINE_SIZE) * pPrevFilterOutSize[prevIdx].x);
                    pCurFilterOutSize[i].y = (A0_OUT_FINE_SIZE * pCurDataIn[i].ySize + (1 - A0_OUT_FINE_SIZE) * pPrevFilterOutSize[prevIdx].y);
                }
                else {
                    pCurFilterOutSize[i].x = pCurDataIn[i].xSize;
                    pCurFilterOutSize[i].y = pCurDataIn[i].ySize;
                }
#endif
            }
            else {
                pCurFilterOut[i].x = pCurDataIn[i].xCord;
                pCurFilterOut[i].y = pCurDataIn[i].yCord;
#ifdef SMOOTH_SIZE_ENABLE //nsmoon@190729
                pCurFilterOutSize[i].x = pCurDataIn[i].xSize;
                pCurFilterOutSize[i].y = pCurDataIn[i].ySize;
#endif
            }
        }
		else
#endif
        {
			//TOUCH_START_STATE, TOUCH_UP_STATE
#ifdef TOUCH_DOWN_MIN_MOVEMENT //nsmoon@200227
            if (prevIdx >= 0) {
                pCurFilterOut[i].x = pPrevFilterOut[prevIdx].x;
                pCurFilterOut[i].y = pPrevFilterOut[prevIdx].y;
            }
            else
#endif
            {
			pCurFilterOut[i].x = pCurDataIn[i].xCord;
			pCurFilterOut[i].y = pCurDataIn[i].yCord;
            }

#ifdef SMOOTH_SIZE_ENABLE //nsmoon@190729
			pCurFilterOutSize[i].x = pCurDataIn[i].xSize;
			pCurFilterOutSize[i].y = pCurDataIn[i].ySize;
#endif
        }
	}

#if defined(SMOOTHING_FILTER_ENABLE)
	int prevInLength = 0;
	//save current, must be same as pPrevDataIn[]
	for (i = 0; i < curInLength; i++) {
		if (pCurDataIn[i].status == TOUCH_DOWN_STATE || pCurDataIn[i].status == TOUCH_START_STATE) {
		    pPrevFilterOut[prevInLength++] = pCurFilterOut[i];
		}
#ifdef SMOOTH_SIZE_ENABLE //nsmoon@190729
		pPrevFilterOutSize[i] = pCurFilterOutSize[i];
#endif
    }
#endif
#ifdef SMOOTH_SIZE_ENABLE //nsmoon@190729
    if (curInLength > 0) {
        TRACE("!,%0.1f,%0.1f, %0.1f,%0.1f, %0.1f,%0.1f", s_touchPosFilterOut[0].x, s_touchPosFilterOut[0].y, s_touchSizeFilterOut[0].x, s_touchSizeFilterOut[0].y, pCurDataIn[0].xSize, pCurDataIn[0].ySize);
    }
#endif
}

#endif //1

/////////////////////////////////////////////////////////////////////
// convert coordinates physical to logical 
/////////////////////////////////////////////////////////////////////
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_s_coordinates_conversion    0
#endif
void s_coordinates_conversion(DEF_DATA_INFO *pOutBuf)
{
    int i, cnt; //j,
	touchDataSt_t *pCurDataIn = &s_touchPosDataIn[0];
    int curOutLength = GET_MIN(s_touchReportCnt, ALLOWABLE_TOUCH_DATA_IO);
	vec_t *pCurFilterOut = &s_touchPosFilterOut[0];
	float tmpX, tmpY, aAreaX, aAreaY;
	uint16_t xCord, yCord, xSize, ySize;
	uint8_t r_ID_Cnt, n_ID_Cnt;

#if 1	//shj@	simultaneous release & tip switch
	TP_ID_INFO_T tp_id_info;
	tp_id_info.tp_ID_Status_buf = &s_Touch_ID_Status_Tbl[0];
	tp_id_info.tp_ID_idx_buf = &s_Touch_ID_Idx_Tbl[0];
#endif

	IS_DEBUG_FLAG{
		TRACE_NOP;
	}

	cnt = 0;

#if 1	//shj@	simultaneous release & tip switch
	r_ID_Cnt = 0;	//Release ID Count
	n_ID_Cnt = 0;	//New ID Count
#endif


    for (i = 0; i < curOutLength; i++) {
		if (pCurDataIn[i].status != TOUCH_DOWN_STATE && pCurDataIn[i].status != TOUCH_UP_STATE) {
            continue;
        }
#if 0
		float tmpSizeMax = GET_MAX(pCurDataIn[i].xSize, pCurDataIn[i].ySize);
		float tmpSizeMin = GET_MIN(pCurDataIn[i].xSize, pCurDataIn[i].ySize);
#endif
		//float tmpSizeSq = tmpSizeMin*tmpSizeMin;
        IS_DEBUG_FLAG {
            TRACE_TPT("pCurDataIn[],pCurFilterOut[]: %d,%d,%d (%0.2f,%0.2f) (%0.2f,%0.2f)", i, pCurDataIn[i].contactID, pCurDataIn[i].status, pCurDataIn[i].xCord, pCurDataIn[i].yCord, pCurFilterOut[i].x, pCurFilterOut[i].y);
        }
#if 0 //direct
		//tmpX = (pCurFilterOut[i].x - s_aarea_zero_x) * s_ratioXforP2L; //nsmoon@170914a
		//tmpY = (pCurFilterOut[i].y - s_aarea_zero_y) * s_ratioYforP2L;
#endif
#if 1 //reverse X and Y
        aAreaX = s_sensor_end_x - s_aarea_end_x;
        aAreaY = s_sensor_end_y - s_aarea_end_y;
		tmpX = ((s_sensor_end_x - pCurFilterOut[i].x) - aAreaX) * s_ratioXforP2L; //nsmoon@170914a
		tmpY = ((s_sensor_end_y - pCurFilterOut[i].y) - aAreaY) * s_ratioYforP2L;  // s_ratioYforP2L = s_logical_max_x / (s_aarea_end_x - s_aarea_zero_x);
#endif
		xCord = (uint16_t)(tmpX + 0.5f);
		yCord = (uint16_t)(tmpY + 0.5f);
        if (pCurDataIn[i].sizeType == ENUM_SIZE_PEN) {
            xSize = 2;
            ySize = 2;
        }
        else if (pCurDataIn[i].sizeType == ENUM_SIZE_MARKER) {
            xSize = 4;	//6;
            ySize = 4;	//6;
        }
//#if 1 //def DEBUG_FUNCTION_ENABLE_ALL //for test
#if defined(SHOW_DEBUG_SIZE_DOWN_0205) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
        //else if (pCurDataIn[i].sizeType == ENUM_SIZE_ONE)
        else if (pCurDataIn[i].sizeType == ENUM_SIZE_UNKNOWN)
        {
            xSize = 1;
            ySize = 1;
        }
#endif

        else if (pCurDataIn[i].sizeType == ENUM_SIZE_UNKNOWN)
		{
        	TRACE_TPT("UNKNOWN");
        	continue;
		}
        else
		{
#if 0
#define BIG_ERASER_ADJ	0.75f
        	 //TRACE("%0.2f	%0.2f	%0.2f", tmpSizeSq, tmpSizeMin, tmpSizeMax);

        	 if(tmpSizeMax > ERASER_BIG_SIZE)
        	 {

				xSize = (uint16_t)(tmpSizeMin*BIG_ERASER_ADJ);
				ySize = (uint16_t)(tmpSizeMin*BIG_ERASER_ADJ);
        	 }
        	 else if(tmpSizeMax > ERASER_MID_SIZE )
			 {
				 xSize = 50;
				 ySize = 50;
			 }
        	 else
		{
        		 xSize = 8;
        		 ySize = 8;
        }

        }

#else
        xSize = ySize =pCurDataIn[i].eraseSize;

        }
#endif
#if (DEBUG_s_coordinates_conversion == 1)
        TRACE("*123(,%d,%d,%02x,:,%d,%d,:,%0.1f,%0.1f,:,%d,%d)[%d]", i, pCurDataIn[i].contactID, pCurDataIn[i].status, xCord, yCord, pCurFilterOut[i].x, pCurFilterOut[i].y, xSize, ySize, s_frame_no);
#endif
		//check logical position
		if (xCord > s_logical_max_x)
		{
#if (DEBUG_s_coordinates_conversion == 1)
			TRACE_ERROR("+123X(,%d:%d,%0.1f,)", i, xCord, tmpX);
#endif
			xCord = (uint16_t)s_logical_max_x;
        }
		if (yCord > s_logical_max_y)
		{
#if (DEBUG_s_coordinates_conversion == 1)
			TRACE_ERROR("+123Y(,%d:%d,%0.1f,)", i, yCord, tmpY);
#endif
			yCord = (uint16_t)s_logical_max_y;
        }

        pOutBuf->buf[cnt].contactID = pCurDataIn[i].contactID;
        //TRACE("(%d) ", pOutBuf->buf[cnt].contactID);
        if (pCurDataIn[i].status == TOUCH_DOWN_STATE)
        {
            pOutBuf->buf[cnt].status = TOUCH_SIG_DOWN;

#if 1	//shj@	simultaneous release & tip switch
            if(tp_id_info.tp_ID_Status_buf[pCurDataIn[i].contactID] == NOT_IN_USE_ID)
            {
            	if(pOutBuf->outcnt != 0)
            	{
					n_ID_Cnt++;	//new ID
					pOutBuf->buf[cnt].ID_staus = NEW_ID;
					tp_id_info.tp_ID_Status_buf[pCurDataIn[i].contactID] = IN_USE_ID;
            	}
            }
            else
            {
            	pOutBuf->buf[cnt].ID_staus = IN_USE_ID;
            }
#endif
        }
        else
        {
            pOutBuf->buf[cnt].status = TOUCH_SIG_UP;

#if 1	//shj@	simultaneous release & tip switch
            pOutBuf->buf[cnt].ID_staus = NOT_IN_USE_ID;
            tp_id_info.tp_ID_Status_buf[pCurDataIn[i].contactID] = NOT_IN_USE_ID;
            r_ID_Cnt++;		// Release ID
#endif
        }
        //TRACE("(%d)%02X", cnt, pOutBuf->buf[cnt].status);
		pOutBuf->buf[cnt].xCord = xCord;
		pOutBuf->buf[cnt].yCord = yCord;
		pOutBuf->buf[cnt].xSize = xSize;
		pOutBuf->buf[cnt].ySize = ySize;

		if (++cnt == ALLOWABLE_TOUCH_DATA_IO)
        {
            TRACE_ERROR("ERROR! invalid cnt");
            break; //nsmoon@190322
        }
	} //for (i = 0; i < curOutLength; i++)
    if (i != curOutLength) {
        TRACE_ERROR("ERROR! invalid curOutLength %d(%d)", curOutLength, i);
    }
	pOutBuf->len = (uint8_t)cnt;

#if 1	//shj@	simultaneous release & tip switch
	pOutBuf->outcnt++;
	if(pOutBuf->outcnt >= 0xFFFFFFF0) pOutBuf->outcnt = 1;	//Fixed Me

	if(cnt == 0)	//reset tp id info
	{
		pOutBuf->outcnt = 0;
		pOutBuf->reSend = 0;
		memset(&s_Touch_ID_Status_Tbl[0], 0x00, sizeof(s_Touch_ID_Status_Tbl));
	}
	else
	{
		if(n_ID_Cnt && r_ID_Cnt) // Exist Simultaneous Release and TipSwitch
		{
			pOutBuf->reSend = 1;
		}
	}
#endif

#if 0 //reserved, do not remove
    /////////////////////////////////
    //sort id to avoid windows bug
    /////////////////////////////////
	DEF_TOUCH_OUT bufTmp;
    for (i = 0; i < (pOutBuf->len - 1); i++) {
        for (j = (i + 1); j < pOutBuf->len; j++) {
        	if ((pOutBuf->buf[i].status == pOutBuf->buf[j].status && pOutBuf->buf[i].contactID > pOutBuf->buf[j].contactID) ||
        		(pOutBuf->buf[i].status != pOutBuf->buf[j].status && pOutBuf->buf[i].status == TOUCH_SIG_UP)) {
    		    bufTmp = pOutBuf->buf[i];
    		    pOutBuf->buf[i] = pOutBuf->buf[j];
    		    pOutBuf->buf[j] = bufTmp;
        	}
        } //for (j = (i + 1); j < pOutBuf->len; j++)
    } //for (i = 0; i < (pOutBuf->len - 1); i++)
#endif

	IS_DEBUG_FLAG{
		TRACE_NOP;
    }
}

#if (UPDATE_TOUCH_STATUS_TEST_ENABLE == 1)
void s_smooth_filter_test(void)
{
    int i;
    touchDataSt_t *pCurDataIn = &s_touchPosDataIn[0];
    int curInLength = GET_MIN(s_touchReportCnt, ALLOWABLE_TOUCH_DATA_IO);
    vec_t *pCurFilterOut = &s_touchPosFilterOut[0];
    IS_DEBUG_FLAG{
        TRACE_NOP;
    }

    for (i = 0; i < curInLength; i++) {
  #ifdef DEBUG_EXPECTATION_TRACKING //nsmoon@190328
        if (pCurDataIn[i].contactID == DEBUG_TRACKING_TOUCH_ID) {
            pCurFilterOut[i].x = pCurDataIn[i].xCord;
            pCurFilterOut[i].y = pCurDataIn[i].yCord;
            continue;
        }
  #endif
        pCurFilterOut[i].x = pCurDataIn[i].xCord;
        pCurFilterOut[i].y = pCurDataIn[i].yCord;
    }
}
#endif


void s_smooth_filter_brush(void)
{
	int i, tId;
	touchDataSt_t *pCurDataIn = &s_touchPosDataIn[0];
    int curInLength = GET_MIN(s_touchReportCnt, ALLOWABLE_TOUCH_DATA_IO);
	vec_t *pCurFilterOut = &s_touchPosFilterOut[0];
	vec_t *pPrevFilterOut = &s_prevTouchPosFilterOut[0];

	//for Size
	vec_t *pCurBrushFilterOutSize = &s_BrushSizeFilterOut[0];
	vec_t *pPrevBrushFilterOutSize = &s_prevBrushSizeFilterOut[0];

	IS_DEBUG_FLAG{
		TRACE_NOP;
	}

	for (i = 0; i < curInLength; i++) {
#if defined(SMOOTHING_FILTER_ENABLE)
        tId = pCurDataIn[i].contactID;

#ifdef DEBUG_EXPECTATION_TRACKING //nsmoon@190328
        if (tId == DEBUG_TRACKING_TOUCH_ID) {
            pCurFilterOut[i].x = pCurDataIn[i].xCord;
            pCurFilterOut[i].y = pCurDataIn[i].yCord;
            continue;
		}
#endif
        //if (pCurDataIn[i].status == TOUCH_DOWN_STATE || pCurDataIn[i].status == TOUCH_START_STATE)
        if (pCurDataIn[i].status == TOUCH_DOWN_STATE) //nsmoon@200310
        {
            if (pPrevFilterOut[tId].x < 0)
            {
                pCurFilterOut[i].x = pCurDataIn[i].xCord;
                pCurFilterOut[i].y = pCurDataIn[i].yCord;
                pCurBrushFilterOutSize[i].x = pCurDataIn[i].xSize;
                pCurBrushFilterOutSize[i].y = pCurDataIn[i].ySize;

            }
            else
            {
                pCurFilterOut[i].x = (A0_OUT_FINE * pCurDataIn[i].xCord) + ((1 - A0_OUT_FINE) * pPrevFilterOut[tId].x);
                pCurFilterOut[i].y = (A0_OUT_FINE * pCurDataIn[i].yCord) + ((1 - A0_OUT_FINE) * pPrevFilterOut[tId].y);
                pCurBrushFilterOutSize[i].x = (A0_OUT_BRUSH_SIZE * pCurDataIn[i].xSize + (1 - A0_OUT_BRUSH_SIZE) * pPrevBrushFilterOutSize[tId].x);
                pCurBrushFilterOutSize[i].y = (A0_OUT_BRUSH_SIZE * pCurDataIn[i].ySize + (1 - A0_OUT_BRUSH_SIZE) * pPrevBrushFilterOutSize[tId].y);

            }
            pPrevFilterOut[tId] = pCurFilterOut[i];
            pPrevBrushFilterOutSize[tId] = pCurBrushFilterOutSize[i];

        }
        else if (pCurDataIn[i].status == TOUCH_UP_STATE)
        {
#ifdef TOUCH_DOWN_MIN_MOVEMENT //nsmoon@200227
            if (pPrevFilterOut[tId].x >= 0)
            {
            	if (s_eraserFlag)
            	{
    			    pCurFilterOut[i].x = pCurDataIn[i].xCord;
    			    pCurFilterOut[i].y = pCurDataIn[i].yCord;
            	}
            	else
            	{
                    pCurFilterOut[i].x = pPrevFilterOut[tId].x;
                    pCurFilterOut[i].y = pPrevFilterOut[tId].y;
            	}
            }
            else
#endif
            {
				pCurFilterOut[i].x = pCurDataIn[i].xCord;
				pCurFilterOut[i].y = pCurDataIn[i].yCord;
            }

            pCurBrushFilterOutSize[i].x = pCurDataIn[i].xSize;
            pCurBrushFilterOutSize[i].y = pCurDataIn[i].ySize;
    }
#else
        pCurFilterOut[i].x = pCurDataIn[i].xCord;
        pCurFilterOut[i].y = pCurDataIn[i].yCord;
#endif
    } //for (i = 0; i < curInLength; i++)
}

void s_smooth_filter_brush_2nd(uint32_t t_frame, uint8_t t_status, float brush_size)
{
    if (t_status == TOUCH_DOWN_STATE)
    {
		if(t_frame == 0)
		{
			s_OutFilter_Brush_Size = brush_size;
		}
		else
		{
			s_OutFilter_Brush_Size = (A1_OUT_BRUSH_SIZE * brush_size + (1 - A1_OUT_BRUSH_SIZE) * s_OutFilter_Brush_Size);
		}
    }
    else if(t_status == TOUCH_UP_STATE)
    {
    	s_OutFilter_Brush_Size = brush_size;
    }
}

#define PRESSURE_STEP 45
void s_coordinates_conversion_brush(DEF_DATA_INFO *pOutBuf)
{
    int i, cnt;
	touchDataSt_t *pCurDataIn = &s_touchPosDataIn[0];
    int curOutLength = GET_MIN(s_touchReportCnt, ALLOWABLE_TOUCH_DATA_IO);
	vec_t *pCurFilterOut = &s_touchPosFilterOut[0];
	vec_t *pCurBrushFilterOutSize = &s_BrushSizeFilterOut[0];

	float tmpX, tmpY, aAreaX, aAreaY;
	uint16_t xCord, yCord, pressure, xSize, ySize;
	float xSize_f, ySize_f, cSize_f, rSize_f, out_size_f, reSize_Ratio_f;
	float decide_Size;

	float Real_Pressure[PRESSURE_STEP] = {
			32.35f, 50.00f, 46.67f, 65.00f, 68.00f, 63.33f, 60.00f, 60.00f, 57.78f, 54.00f,
			52.73f, 50.00f, 47.69f, 47.14f, 46.67f, 45.00f, 44.71f, 43.33f, 44.21f, 44.00f,
			43.81f, 43.64f, 45.22f, 45.00f, 44.80f, 44.62f, 44.44f, 45.00f, 44.83f, 44.67f,
			44.52f, 44.38f, 44.85f, 44.71f, 45.14f, 45.00f, 45.41f, 45.26f, 45.13f, 45.00f,
			44.88f, 44.76f, 45.58f, 45.45f, 45.49f
		};
	uint8_t xx;
	uint16_t xCord_diff, yCord_diff;

	TP_ID_INFO_T tp_id_info;
	uint8_t use_in_id_cnt, exist_Mode_Change;

	uint8_t tBrush_ID_idx;

	tp_id_info.tp_ID_Status_buf = &s_Touch_ID_Status_Tbl[0];
	tp_id_info.tp_ID_idx_buf = &s_Touch_ID_Idx_Tbl[0];
	tp_id_info.tp_Big_Size_ID_buf = &s_Touch_Big_Size_ID_Tbl[0];

	IS_DEBUG_FLAG{
		TRACE_NOP;
	}

	cnt = 0;
	use_in_id_cnt = 0;
	exist_Mode_Change = 0;
	tBrush_ID_idx = 255;
	pOutBuf->len = 0;
	pOutBuf->reSend = 0;

	if(curOutLength == 0)
	{
		s_Brush_Frame_Cnt = 0;
		s_Brush_Release_Cnt = BRUSH_NO_OBJECT_FRAME_CNT;
		if(cnt == 0)	//reset tp id info
		{
			memset(&s_Touch_ID_Status_Tbl[0], 0x00, sizeof(s_Touch_ID_Status_Tbl));
		}
		return;
	}

	if(s_Brush_Frame_Cnt == 0)	//first frame
	{
		for(i=0; i < curOutLength; i++)
	{
			if(pCurDataIn[i].status == TOUCH_UP_STATE)
		{
				continue;
			}
			else
		{
				tBrush_ID_idx = (uint8_t)i;
				break;
			}
		}
	}
	else
	{
		//find use in id number
	for(i=0; i<ALLOWABLE_TOUCH_DATA_IO; i++)
	{
			if(tp_id_info.tp_ID_Status_buf[i] == IN_USE_ID)
		{
				tp_id_info.tp_ID_idx_buf[use_in_id_cnt] = (uint8_t)i;
			use_in_id_cnt++;
		}
	}
		if(use_in_id_cnt > 0)
		{
    for (i = 0; i < curOutLength; i++)
    {
				if(pCurDataIn[i].contactID == tp_id_info.tp_ID_idx_buf[0])
		{
					tBrush_ID_idx = (uint8_t)i;
					break;
				}
			}
        }

	}
	if(tBrush_ID_idx == 255) //unknown objects
		{
		s_Brush_Frame_Cnt = 0;
		s_Brush_Release_Cnt = BRUSH_NO_OBJECT_FRAME_CNT;
		memset(&s_Touch_ID_Status_Tbl[0], 0x00, sizeof(s_Touch_ID_Status_Tbl));
		return;
        }

	if(s_Brush_Release_Cnt > 0)	//
        {
		s_Brush_Release_Cnt--;
		return;
        }

        aAreaX = s_sensor_end_x - s_aarea_end_x;
        aAreaY = s_sensor_end_y - s_aarea_end_y;
	tmpX = ((s_sensor_end_x - pCurFilterOut[tBrush_ID_idx].x) - aAreaX) * s_ratioXforP2L; //nsmoon@170914a
	tmpY = ((s_sensor_end_y - pCurFilterOut[tBrush_ID_idx].y) - aAreaY) * s_ratioYforP2L;

		xCord = (uint16_t)(tmpX + 0.5f);
		yCord = (uint16_t)(tmpY + 0.5f);

		//check logical position
		if (xCord > s_logical_max_x)
		{
			xCord = (uint16_t)s_logical_max_x;
        }
		if (yCord > s_logical_max_y)
		{
			yCord = (uint16_t)s_logical_max_y;
        }
#if 0
	xSize_f = pCurBrushFilterOutSize[tBrush_ID_idx].x;
	ySize_f = pCurBrushFilterOutSize[tBrush_ID_idx].y;
#else
	xSize_f = pCurDataIn[tBrush_ID_idx].xSize;
	ySize_f = pCurDataIn[tBrush_ID_idx].ySize;
#endif

	if(xSize_f < BRUSH_MIN_OBJECT_SIZE_F) xSize_f = BRUSH_MIN_OBJECT_SIZE_F;
	if(ySize_f < BRUSH_MIN_OBJECT_SIZE_F) ySize_f = BRUSH_MIN_OBJECT_SIZE_F;
#if 0
	reSize_Ratio_f = BRUSH_RESIZE_OFFSET_DEFAULT;	//default size 1x

	if(pCurDataIn[tBrush_ID_idx].th50CntX != 0 && pCurDataIn[tBrush_ID_idx].th50CntY != 0)
	{
		if(pCurDataIn[tBrush_ID_idx].th10CntX == 0 && pCurDataIn[tBrush_ID_idx].th10CntY == 0)
		{
			reSize_Ratio_f = BRUSH_RESIZE_OFFSET1;
		}
		else
		{
			if(pCurDataIn[tBrush_ID_idx].th10CntX == 0 || pCurDataIn[tBrush_ID_idx].th10CntY == 0)
			{
				if(pCurDataIn[tBrush_ID_idx].xSize > 4.0f && pCurDataIn[tBrush_ID_idx].ySize > 4.0f)
		{
					reSize_Ratio_f = BRUSH_RESIZE_OFFSET2;
        }
        }
		}
	}
	else
		{
		reSize_Ratio_f = 0.3f;
        }
#endif

	if(s_Brush_Frame_Cnt < 1)	//first frame
		{
		if(xSize_f > ySize_f)
		{
			out_size_f = ySize_f;
		}
		else
		{
			out_size_f = xSize_f;
		}
		Move_Status = MOVE_NONE;
		preMove_Status = MOVE_NONE;
		xCord_diff = 0;
		yCord_diff = 0;
		s_Brush_Frame_Cnt++;
		}
	else
	{
		if(preXCord > xCord) xCord_diff = preXCord - xCord;
		else if(preXCord < xCord) xCord_diff = xCord - preXCord;
		else xCord_diff = 0;

		if(preYCord > yCord) yCord_diff = preYCord - yCord;
		else if(preYCord < yCord) yCord_diff = yCord - preYCord;
		else yCord_diff = 0;

		if(xCord_diff < 2 && yCord_diff < 2)
		{
			Move_Status = MOVE_NONE;
		}
		else
		{
			if(xCord_diff > yCord_diff) Move_Status = MOVE_X_POS;
			else if(xCord_diff < yCord_diff) Move_Status = MOVE_Y_POS;
			else Move_Status = MOVE_XY_POS;
		}

		if(xSize_f < 5.5f && ySize_f < 5.5f)
		{
			out_size_f = (xSize_f + ySize_f)/2;
		}
		else
		{
			if(xSize_f > ySize_f)
			{
				out_size_f = ySize_f;
			}
			else
			{
				out_size_f = xSize_f;
			}

			if(Move_Status == MOVE_X_POS)
			{
				if(preMove_Status == MOVE_Y_POS)
				{
					out_size_f = (xSize_f + ySize_f)/2;
				}
				else
				{
					out_size_f = ySize_f;
				}
			}
			else if(Move_Status == MOVE_Y_POS)
			{
				if(preMove_Status == MOVE_X_POS)
        {
					out_size_f = (xSize_f + ySize_f)/2;
        }
        else
        {
					out_size_f = xSize_f;
				}
			}
			else if(Move_Status == MOVE_XY_POS)
			{
				if(preMove_Status == MOVE_Y_POS)
				{
					out_size_f = xSize_f;
				}
				else if(preMove_Status == MOVE_X_POS)
				{
					out_size_f = ySize_f;
				}
				else if(preMove_Status == MOVE_XY_POS)
				{
					out_size_f = (xSize_f + ySize_f) / 2;
				}
			}
		}
	}
#if 0
	s_smooth_filter_brush_2nd(s_Brush_Frame_Cnt, pCurDataIn[tBrush_ID_idx].status, out_size_f);

	cSize_f = s_OutFilter_Brush_Size * reSize_Ratio_f;
	decide_Size = (xSize_f + ySize_f) / 2;

	if(cSize_f < BRUSH_MIN_OBJECT_SIZE_F) cSize_f = BRUSH_MIN_OBJECT_SIZE_F;

	if(cSize_f > BRUSH_NORMAL_SIZE_OFFSET)
	{
		cSize_f = cSize_f - ((cSize_f-BRUSH_NORMAL_SIZE_OFFSET)/10.0f);
        }

	preMove_Status = Move_Status;

	if(cSize_f > BRUSH_MAX_OBJECT_SIZE_F) cSize_f = BRUSH_MAX_OBJECT_SIZE_F;
	if(cSize_f < BRUSH_MIN_OBJECT_SIZE_F) cSize_f = BRUSH_MIN_OBJECT_SIZE_F;

	rSize_f = (cSize_f * 10.0f) - 10.0f;
	cSize_f = cSize_f * (float)BRUSH_PRESSURE_MIN;

	pressure = (uint16_t)(cSize_f - (cSize_f * (float)BRUSH_PRESSURE_RESCALE_VAL * rSize_f));
#else

	cSize_f = out_size_f; //(xSize_f + ySize_f) / 2;
	decide_Size = (xSize_f + ySize_f) / 2;
	pressure = BRUSH_PRESSURE_MAX;
	xx= 0;

	if(cSize_f > PRESSURE_STEP) pressure = BRUSH_PRESSURE_MAX;
	else
	{
		for(xx= 0; xx < PRESSURE_STEP; xx++ )
		{
			if(cSize_f <= (float)xx+1)
			{
				pressure = (uint16_t)(cSize_f*Real_Pressure[xx]);
				break;
			}
		}
	}
#endif

	if(pressure > BRUSH_PRESSURE_MAX) pressure = BRUSH_PRESSURE_MAX;
	if(pressure < BRUSH_PRESSURE_MIN) pressure = BRUSH_PRESSURE_MIN;

	if(Brush_Process == NORMAL_SIZE_PROCESS)
	{
		Brush_Size_Status = NORMAL_SIZE;
		xSize = 0x0A;
		ySize = 0x0A;

		if(decide_Size > (BRUSH_MAX_OBJECT_SIZE_F + 1.0f))
		{
			s_Brush_Big_Size_Frame_Cnt++;
		}
		else
		{
			s_Brush_Big_Size_Frame_Cnt = 0;
		}
		if(s_Brush_Big_Size_Frame_Cnt >= BRUSH_BIG_SIZE_FRAME_CNT)
		{
			if (pCurDataIn[tBrush_ID_idx].status == TOUCH_DOWN_STATE)
			{
				exist_Mode_Change = 1;
				Brush_Process = ERASER_SIZE_PROCESS;
				s_Brush_Big_Size_Frame_Cnt = 0;
				s_Brush_Normal_Size_Frame_Cnt = 0;
			}
		}
	}
	else
	{
		Brush_Size_Status = ERASER_SIZE;
		xSize = (uint16_t)(((xSize_f + ySize_f) / 2) - BRUSH_ERASER_SIZE_OFFSET);
		if(xSize > BURSH_ERASER_SIZE_MAX) xSize = BURSH_ERASER_SIZE_MAX;
		if(xSize < BURSH_ERASER_SIZE_MIN) xSize = BURSH_ERASER_SIZE_MIN;
		ySize = xSize;

		tp_delay(1000);

		if(s_Brush_Big_Size_Frame_Cnt < 7)
		{
			if(s_Brush_Big_Size_Frame_Cnt == 0)
			{
				Brush_Eraser_xSize = xSize;
				Brush_Eraser_ySize = ySize;
				pressure = BRUSH_ERASER_PRESSURE_START_VALUE;
				if(s_Brush_Frame_Cnt > 1)
				{
					xCord = preXCord;
					yCord = preYCord;
				}
			}
			else
			{
				ySize = Brush_Eraser_xSize;
				xSize = Brush_Eraser_ySize;
				pressure = BRUSH_ERASER_PRESSURE_START_VALUE;
				pressure = pressure + ((uint16_t)s_Brush_Big_Size_Frame_Cnt * BRUSH_ERASER_PRESSURE_STEP_CNT);
				if(pressure > BRUSH_PRESSURE_MAX) pressure = BRUSH_PRESSURE_MAX;
			}
			s_Brush_Big_Size_Frame_Cnt++;
		}
		else
        {
			pressure = BRUSH_PRESSURE_MAX;
        }

		if(decide_Size < (BRUSH_MAX_OBJECT_SIZE_F + 1.0f))
		{
			s_Brush_Normal_Size_Frame_Cnt++;
		}
		else
		{
			s_Brush_Normal_Size_Frame_Cnt = 0;
		}

		if(s_Brush_Normal_Size_Frame_Cnt >= BRUSH_NORMAL_SIZE_FRAME_CNT)
		{
			if (pCurDataIn[tBrush_ID_idx].status == TOUCH_DOWN_STATE)
			{
				exist_Mode_Change = 1;
				Brush_Process = NORMAL_SIZE_PROCESS;
				s_Brush_Normal_Size_Frame_Cnt = 0;
				s_Brush_Big_Size_Frame_Cnt = 0;
			}
		}
    }

	preXCord = xCord;
	preYCord = yCord;

	preBrush_Size_Status = Brush_Size_Status;

	pOutBuf->len = BRUSH_MAX_ID_NUM;
	pOutBuf->buf[cnt].contactID = pCurDataIn[tBrush_ID_idx].contactID;
	if(pCurDataIn[tBrush_ID_idx].status == TOUCH_DOWN_STATE)
	{
		pOutBuf->buf[cnt].status = TOUCH_SIG_DOWN;
		tp_id_info.tp_ID_Status_buf[pCurDataIn[tBrush_ID_idx].contactID] = IN_USE_ID;
		pOutBuf->reSend = 0;

		if(exist_Mode_Change)	//small size object -> big size object
		{
			pOutBuf->buf[cnt].status = TOUCH_SIG_UP;
			xSize = 0;
			ySize = 0;
			pressure = 0;
		}
		}
	else
	{
		pOutBuf->buf[cnt].status = TOUCH_SIG_UP;
		tp_id_info.tp_ID_Status_buf[pCurDataIn[tBrush_ID_idx].contactID] = NOT_IN_USE_ID;
		memset(&tp_id_info.tp_ID_idx_buf[0], 0xFF, sizeof(ALLOWABLE_TOUCH_DATA_IO));
		s_Brush_Frame_Cnt = 0;
		s_Brush_Big_Size_Frame_Cnt = 0;
		s_Brush_Normal_Size_Frame_Cnt = 0;
		s_Brush_Release_Cnt = BRUSH_RELEASE_FRAME_CNT;
		pOutBuf->reSend = 1;
		Brush_Size_Status = NORMAL_SIZE;
		preBrush_Size_Status = NORMAL_SIZE;
		Brush_Process = NORMAL_SIZE_PROCESS;
	}

	pOutBuf->buf[cnt].xCord = xCord;
	pOutBuf->buf[cnt].yCord = yCord;

	pOutBuf->buf[cnt].xSize = xSize;
	pOutBuf->buf[cnt].ySize = ySize;

	pOutBuf->buf[cnt].pressure = pressure;
	pOutBuf->buf[cnt].nReportSendCnt = pOutBuf->reSend;
}
//////////////////////////////////////////////////////////////////////
/*End of File*/
