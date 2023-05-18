#ifndef _BACKEND_UTILS_H    /* Guard against multiple inclusion */
#define _BACKEND_UTILS_H
/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif
#include "backend_common.h"
#include "stdlib.h"

#if 1
#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif
#endif

#define GET_MAX(a, b)			(((a) > (b)) ? (a) : (b))
#define GET_MIN(a, b)			(((a) < (b)) ? (a) : (b))
#define GET_ABS(a)				(((a) < 0) ? -(a) : (a))
#define GET_LINE_CENT(a, b)		(((a) + (b)) * 0.5f)
#define GET_LINE_CENT_INT(a, b)	(((a) + (b)) / 2)
#define GET_LINE_WIDTH_INT(a, b)	(GET_ABS((a) - (b)) / 2)
#define EPSILON					0.0001f //FIXME 0.00001f

//#define MAX_MIN_SLOPE_LINE    10
#define MIN_INITIAL_VAL         9999
#define MIN_INITIAL_VAL_SQ      90000000 //3000*3000
#define IS_ON_X_AXIS_SLOPE(a)	((a) < BS_offsetTblLenX)  //nsmoon@170323
#define IS_ON_Y_AXIS_SLOPE(a)	((a) >= BS_offsetTblLenX)  //nsmoon@170323
#define NO_CLIPPING_EDGEID		0xFFFF
#define AXIS_MASK_X				0x8000
#define AXIS_MASK_Y				0x0000

#define INITIAL_POLYGON_VERTEX_NO	4
#define RESULT_LINE_VERTEX_NO		2

#define POLY_VERTEX_LIMIT_ENABLE  //nsmoon@200303
#ifdef POLY_VERTEX_LIMIT_ENABLE
#define POLY_MAX_VERTEX_LEN         8 //16
#endif

#define UINT16_SIZE     0x10000
#define INT16_SIZE      0x8000
#define UINT8_SIZE      0x100
#define INT8_SIZE       0x80
#ifndef UINT16_MAX
#define UINT16_MAX      0xFFFF
#endif
#ifndef INT16_MAX
#define INT16_MAX       0x7FFF
#endif
#ifndef UINT8_MAX
#define UINT8_MAX       0xFF
#endif
#ifndef INT8_MAX
#define INT8_MAX        0x7F
#endif

#if 1
//#if defined(_WIN32) || defined(WIN32)  //FIXME nsmoon@170111
#ifndef uint8_t
typedef unsigned char      uint8_t;
#endif
#ifndef uint16_t
typedef unsigned short     uint16_t;
#endif
//#if (MODEL_MCU == STM32H7)
#ifndef uint32_t
//typedef unsigned long       uint32_t;
typedef unsigned            uint32_t;
#endif
//#endif
#ifndef uint64_t
typedef unsigned long long uint64_t;
#endif
#ifndef int8_t
//typedef char               int8_t;    //-127 ~ +127
typedef signed char          int8_t;
#endif
#ifndef int16_t
typedef short              int16_t;
#endif
//#if (MODEL_MCU == STM32H7)
#ifndef int32_t
//typedef long               int32_t;
typedef int               int32_t;
#endif
//#endif
#ifndef int64_t
typedef long long          int64_t;
#endif
//#else
//#include <stdio.h>
//#endif //defined(_WIN32) || defined(WIN32)
#endif

//---------------------------------------------
// type definitions
//---------------------------------------------
typedef struct {
    uint64_t upper;
    uint64_t lower;
} slope_mask_t;

typedef enum
{
    ENUM_HOR_X = 0,
    ENUM_VER_Y = 1,
    ENUM_AXIS_END = 2,
} axis_t;

#if 0 //move to front_if.h
typedef struct {
    float xSize, ySize;
} tp_size_t;

#define MAX_THRESHOLD_CNT   0xFF
typedef struct {
    uint8_t th50CntX;
    uint8_t th10CntX;
    uint8_t th50CntY;
    uint8_t th10CntY;
	float th10WidthX;
	float th10WidthY;
} tp_line_cnt_t;
#endif //0

typedef struct {
    float x, y;
} vec_t, *vec;

typedef struct {
    vec_t vert;
    uint16_t edgeId;
} vec_e_t, *vec_e;

typedef struct {
    int len, alloc;
    vec_e ve;
} poly_e_t, *poly_e;

typedef struct {
    uint8_t minX; //FIXME uint8_t
    uint8_t maxX;
    uint8_t minY;
    uint8_t maxY;
} ep_min_max_t;

typedef struct {
    float minX;
    float maxX;
    float minY;
    float maxY;
} pos_min_max_t;

#ifdef MULTI_SKIP_USED_VB //nsmoon@210218
#define INITIAL_VB_MERGED    (0x80) //(0xFF)
#define TRACE_MSUV(...)  //TRACE_ERROR(__VA_ARGS__)
typedef struct {
    uint8_t xNo;
    uint8_t yNo;
} initial_line_no_t;
#endif

typedef struct {
    float min;
    float max;
} pos_minMax2_t;

typedef struct {
    uint8_t min;
    uint8_t max;
} idx_minMax2_t;

typedef enum {
    ENUM_PD = 0,
    ENUM_LED = 1,
    ENUM_SEN_END = 2,
} sen_type_t;

typedef struct {
    uint8_t pd;  //top
    uint8_t led; //bot
} in_line_t;

typedef struct {
	uint8_t byteIdx;
	uint8_t bitIdx;
} in_dlt_buf_idx_t;

typedef struct {
    uint8_t s1; //led, start //FIXME uint8_t
    uint8_t e1; //pd, end
    uint8_t s2;
    uint8_t e2;
} ep_no_t;

typedef struct {
	uint8_t s1; //start
	uint8_t s2; //end
} ep2_no_t;

#define MAX_NUM_SIDE 4
typedef enum {
    ENUM_BOT_BD = 0,
    ENUM_LFT_BD = 1,
    ENUM_TOP_BD = 2,
    ENUM_RHT_BD = 3,
    ENUM_BD_END = 4
} bd_type_t;

typedef enum {
    ENUM_BOT_EDGE = 0,
    ENUM_LFT_EDGE = 1,
    ENUM_TOP_EDGE = 2,
    ENUM_RHT_EDGE = 3,
    ENUM_EDGE_END = 4
} edge_type_t;

typedef enum {
    ENUM_EP_DZ_RIGHT = 0,
    ENUM_EP_DZ_LEFT = 1,
    ENUM_EP_DZ_NORMAL = 2
} ep_dz_t;

typedef enum {
    ENUM_BOT_RHT = 0,
    ENUM_TOP_RHT = 1,
    ENUM_TOP_LFT = 2,
    ENUM_BOT_LFT = 3,
    ENUM_SIDE_MAX = 4
} side_type_t;

typedef enum {
    ENUM_NOTHING = 0,
    ENUM_PEN = 1,
    ENUM_MARKER = 2,
} touch_type_t;

typedef struct {
    uint16_t len;
    in_line_t *line;
} in_buf_t;

typedef struct {
    bd_type_t bd, bd2;
    sen_type_t sensor, sensor2;
    float *senPosX, *senPosY;
    float *senPos2;
    int sideIdx, polyIdx;
    uint16_t minSenlim;
    uint16_t maxSenlim;
    //uint16_t centerSen;
} clipping_range_info_t;

/////////////////////////////////////////////////
//Global Variables
/////////////////////////////////////////////////
#if 0 //for test
#define BIT_MASK_BYTE_ARRY_LEN	8
extern uint8_t bit_mask_byte_arry[BIT_MASK_BYTE_ARRY_LEN];
extern void backend_utils_init(void);
#endif
extern clipping_range_info_t BS_clipping_range_info[ENUM_AXIS_END]; //nsmoon@200115
#ifdef USE_CUST_MALLOC //nsmon@201012
extern unsigned int __end_of_heap; //nsmoon@201013
#endif

/////////////////////////////////////////////////
//Global Functions
/////////////////////////////////////////////////
extern int BS_line_intersection(vec p0, vec p1, vec p2, vec p3, vec res);
extern int BS_getSenInBuf(axis_t axis, int inBufIdx, int *pd, int *led);
extern int BS_getLinePosInBuf(axis_t axis, int idx, vec_t *p0, vec_t *p1);
extern int BS_getLinePosInBufSen(axis_t axis, int led, int pd, vec_t *p0, vec_t *p1);
extern int BS_getPosSen(axis_t axis, sen_type_t sen, int senNum, vec_t *pR);
extern int BS_getSlopeInBuf(axis_t axis, int inBufIdx, int *slopeValPlus);
extern int BS_getSenNumPos(axis_t axis, sen_type_t sen, float pos, int mode);
extern poly_e BS_poly_e_clip(poly_e sube, poly_e clipe);
extern float BS_getTouchArea(poly_e pe);
extern int BS_left_of(vec p0, vec p1, vec dt);
extern float BG_calDistSquare(vec_t p1, vec_t p2);
extern int BS_calMinMaxSen(axis_t axis, int led, int pd, pos_min_max_t *mM_in, pos_min_max_t *mM_out);
extern int BS_get_slope_sign(int led0, int pd0);
extern int BS_init_max4_float(float *max, int maxCnt0);
extern int BS_get_max4_float(float *max, float val, int maxCnt);
extern int BS_init_max4_int(int *max, int *maxIdx, int maxCnt0);
extern int BS_get_max4_int(int *max, int val, int *maxIdx, int idx, int maxCnt);
extern int BS_get_pos_on_line(vec_t *p0, vec_t *p1, axis_t axis, float *pos, float *posRet);
extern int BS_get_clipping_range(axis_t axis, ep_min_max_t *minMaxEp, pos_min_max_t *minMaxEpPos, int *minSenlim, int *maxSenlim);
extern int BS_get_virtual_range(axis_t axis, sen_type_t sen, ep_min_max_t *minMaxEp, pos_min_max_t *minMaxPos, int senNo, int st_ed);
extern int BS_get_sensor_via_p2(float x1Pos, float y1Pos, float x2Pos, float y2Pos, float y3Pos, float *senPosRet);
extern void BS_3point_angle(vec a, vec b, vec c, float *angle);
#if 1 //nsmoon@230418
extern int BS_get_int_min_max_init(int *max, int *idxMax, int maxCnt, int initVal);
extern int BS_get_int_min_max(int *max, int *idxMax, int maxCnt, int idx, int val, int idxVal, int minMax);
#endif
#ifdef USE_CUST_MALLOC //nsmon@201012
extern uint32_t BS_start_of_heap;
extern uint32_t BS_end_of_heap;
extern void BS_malloc_init();
extern void *BS_malloc(int size);
extern void *BS_malloc_uint8_t2(int arryRow, int arryCol); //nsmoon@201014
extern int BS_utils_malloc();
#endif

#define ADJUST_USED_LINE_NORMAL   0
#define ADJUST_USED_LINE_MULTI    1
#define ADJUST_USED_LINE_SHADOW   2
#if 1 //nsmoon@220115
#define ADJUST_USED_LINE_EDGE_X   3
#define ADJUST_USED_LINE_EDGE_Y   4 //nsmoon@220127
#define ADJUST_USED_LINE_FINAL    5
#endif

#if (MODEL_SPT == MODEL_CTSI_550_SINGLE_V100) || (MODEL_SPT == MODEL_CTSI_650_V100) || \
    (MODEL_SPT == MODEL_CTSI_N750_V100) || (MODEL_SPT == MODEL_CTSI_N650_V100) //nsmoon@230411
#define MULTI_IS_EDGE_DIST_X			EDGE_DIST_X
#define LARGE_TOUCH_GAP_SMALL_MULTI    9.0f //nsmoon@211020 2.0=>9.0
#else
#define LARGE_TOUCH_GAP_SMALL_MULTI    2.0f //4p_40mm_edge_top_0303a.dlt-#166 //nsmoon@211015 2.0f=>3.0f
#endif
#define LARGE_TOUCH_GAP_SMALL    1.0f //1l2p_0214a.dlt-#153,#169
#define LARGE_TOUCH_GAP_NORM     10.0f //5.0f, nsmoon@200326 5=>8, nsmoon@211021 8=>10
#define LARGE_TOUCH_GAP_LARGE    10.0f
#if 1 //(MODEL_SPT == MODEL_CTSO_430_V100) //nsmoon@210317
#define LARGE_TOUCH_GAP_XLARGE   5 //nsmoon@210308 43v100_palm+1p_210308g.dlt-#2
#else
#define LARGE_TOUCH_GAP_XLARGE   1 //20.0f //30.0f //do not use 0
#endif
#define LARGE_TOUCH_GAP_BRUSH    30.0f
#define LARGE_TOUCH_GAP_SHADOW   10.0f //nsmoon@220116
extern int BS_adj_used_lines_tp(int tpStart, int tpEnd, int mode);

#define EDGE_NEAR_PD_X		0x0001
#define EDGE_INNER_PD_X		0x0010
#define EDGE_NEAR_LED_X		0x0002
#define EDGE_INNER_LED_X	0x0020
#define EDGE_NEAR_PD_Y		0x0100
#define EDGE_INNER_PD_Y		0x1000
#define EDGE_NEAR_LED_Y		0x0200
#define EDGE_INNER_LED_Y	0x2000
#define IS_NEAR_X(a)        ((a) == EDGE_NEAR_PD_X || (a) == EDGE_NEAR_LED_X)
#define IS_NOT_NEAR_X(a)    ((a) != EDGE_NEAR_PD_X && (a) != EDGE_NEAR_LED_X)
#define IS_NEAR_Y(a)        ((a) == EDGE_NEAR_PD_Y || (a) == EDGE_NEAR_LED_Y)
#define IS_NOT_NEAR_Y(a)    ((a) != EDGE_NEAR_PD_Y && (a) != EDGE_NEAR_LED_Y)
#define IS_INNER_Y(a)       ((a) == EDGE_INNER_PD_Y || (a) == EDGE_INNER_LED_Y) //nsmoon@201203
#define IS_EDGE_PD_Y(a)     ((a) == EDGE_NEAR_PD_Y || (a) == EDGE_INNER_PD_Y)
#define IS_EDGE_LED_Y(a)    ((a) == EDGE_NEAR_LED_Y || (a) == EDGE_INNER_LED_Y)
#define IS_EDGE_PD_X(a)     ((a) == EDGE_NEAR_PD_X || (a) == EDGE_INNER_PD_X)
#define IS_EDGE_LED_X(a)    ((a) == EDGE_NEAR_LED_X || (a) == EDGE_INNER_LED_X)

#if (MODEL_SPT == MODEL_CTSK_850_V100) //nsmoon@200910
#define EDGE_DIST_X			50.0f //nsmoon@200406 30.0f=>50.0f
#define EDGE_DIST_Y			50.0f
#else
#if defined(N75_TEST) //nsmoon@230327
#define EDGE_DIST_X			50.0f
#define EDGE_DIST_Y			50.0f
#elif defined(N65_TEST) //nsmoon@230411 //4p_top_1 #40 //n65_4p_230412b.dlt #32
#define EDGE_DIST_X			100.0f
#define EDGE_DIST_Y			100.0f
#else
#define EDGE_DIST_X			100.0f //50.0f //nsmoon@200406 30.0f=>50.0f
#define EDGE_DIST_Y			100.0f //50.0f //nsmoon@200910 50.0f=>100.0f
#endif
#endif
extern int BS_is_edge(axis_t axis, float cent);

#define IS_NOT_ACTIVE_AREA(x, y) ((x) < (BS_aarea_zero_x - EPSILON) ||\
    (x) > (BS_aarea_end_x + EPSILON) || (y) < (BS_aarea_zero_y - EPSILON) ||\
    (y) > (BS_aarea_end_y + EPSILON))

#define IS_NEAR_BLACK_AREA_X(x, offset) ((x) < (BS_aarea_zero_x + offset) || (x) > (BS_aarea_end_x - offset))
#define IS_NEAR_BLACK_AREA_Y(y, offset) ((y) < (BS_aarea_zero_y + offset) || (y) > (BS_aarea_end_y - offset))
#define IS_NEAR_BLACK_AREA(x, y, offset) (IS_NEAR_BLACK_AREA_X(x, offset) || IS_NEAR_BLACK_AREA_Y(y, offset))

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _BACKEND_UTILS_H */
/*end of file*/
