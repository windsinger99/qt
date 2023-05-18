/*
 * front_if.h
 *
 *  Created on: Feb 14, 2020
 *      Author: user
 */

#ifndef FRONT_IF_H_
#define FRONT_IF_H_
#include "backend_common.h"
#include "backend_utils.h"

//////////////////////////////////////
// Parameters from outside
// might require interface later!!!
//////////////////////////////////////
#define TOUCH_ERASER_SIZESQ_XXLARGE 15000.0f //9000.0f //95*95
#if 1 //(MODEL_SPT == MODEL_CTSO_430_V100) //nsmoon@210304
#define TOUCH_ERASER_SIZESQ_XLARGE 2500.0f //50*50 //5000.0f 71*71
#else
#define TOUCH_ERASER_SIZESQ_XLARGE 9000.0f //95*95
#endif
#define TOUCH_ERASER_SIZESQ_LARGE  900.0f //30*30
#define TOUCH_ERASER_SIZESQ_NORM   100.0f //10*10
#define TOUCH_ERASER_SIZESQ_MERGE  2000.0f //45*45
#define TOUCH_ERASER_SIZESQ_MIN    81.0f //9*9, nsmoon@200401 36=>81
#define TOUCH_PEN_SIZESQ_INITIAL   500.0f
#define TOUCH_MARKER_SIZE_MAX      8.0f
#define TOUCH_FINGER_SIZESQ        36.0f //6*6
#define ALLOWABLE_TOUCH_BE		 (MAX_TOUCH_LIMIT_MULTI) //(10)
#define TOUCH_PALM_CLIPAREA_RATIO		0.70f
#define TOUCH_PEN_CLIPAREA_RATIO		0.20f
//#define TOUCH_PALM_SIZESQ 				6000.0f //95*95
//////////////////////////////////////

//-----------------------------------------------------------------
// DO NOT CHANGE WITHOUT CHANGING BFRONTEND DEFINITION
// MUST BE SAME STRUCTURE WITH BACKEND !!!
//-----------------------------------------------------------------
typedef struct {
	uint8_t len;
	int8_t *tbl;
} ofs_tbl_t;

typedef struct {
	uint8_t len;
	uint8_t *tbl;
} sen_tbl_t;

typedef struct {
	uint8_t pd;
	uint8_t led; //slope-idx, 0:-max
#if (USE_ADC_DATA == 1)
#error USE_ADC_DATA is '1'
	uint8_t adc;
#endif
} DEF_PD;

typedef struct {
	uint16_t hor_len;
	uint16_t ver_len;
	DEF_PD	*ver_touch_pd;
	DEF_PD	*hor_touch_pd;
#ifdef FRONTEND_LINE_THRESHOLD
	uint16_t threshold_x_cnt;
	uint16_t threshold_y_cnt;
    uint8_t *threshold_x;
    uint8_t *threshold_y;
#endif
} DEF_PD_INFO;

#if (DEAD_CELL_TBL == 1) //nsmoon@190829
typedef struct {
    uint8_t hor_pd_len;
    uint8_t hor_led_len;
    uint8_t ver_pd_len;
    uint8_t ver_led_len;
    uint8_t	*hor_dead_pd;
    uint8_t	*hor_dead_led;
    uint8_t	*ver_dead_pd;
    uint8_t	*ver_dead_led;
} DEF_DEADCELL_INFO;
#endif

typedef struct {
#if 1 //(SECOND_OFST_TBL == 1) //nsmoon@190625
	uint16_t  hor_pd_num;		//number of sensor on X axis
	uint16_t  ver_pd_num;		//number of sensor on Y axis
	uint16_t  logical_x_size;	//logical horizontal size
	uint16_t  logical_y_size;	//logical vertical size
	uint8_t maxOfstValX;		//max ofst value of X axis
	uint8_t maxOfstValY;		//max ofst value of Y axis
	uint8_t maxOfstValX2;		//max 2nd ofst value of X axis
	uint8_t maxOfstValY2;		//max 2nd ofst value of Y axis
	sen_tbl_t senTblX2;			//X sensor table for 2nd offset
	sen_tbl_t senTblY2;			//Y sensor table for 2nd offset
#endif
#if (USE_VALID_OFST_TBL == 1)  //nsmoon@191129
#error USE_VALID_OFST_TBL is '1'
	uint8_t *validOfstX;
	uint8_t *validOfstY;
#endif
    float sensor_zero_x;
    float sensor_end_x;  //(sensor_pos_x[] + sensor_pos_x[0])
    float sensor_zero_y;
    float sensor_end_y;  //(sensor_pos_y[] + sensor_pos_y[0])

    float aarea_zero_x;  // Active Area
    float aarea_end_x;   // sensor_end_x - aarea_zero_x
    float aarea_zero_y;
    float aarea_end_y;

	float *pd_pos_x;		//X pd position table
	float *pd_pos_y;		//X led position table
	float *led_pos_x;		//Y pd position table
	float *led_pos_y;		//Y led position table
} DEF_TP_LAYOUT_INFO;

typedef struct {
    float minX;
    float minY;
    float maxX;
    float maxY;
} DEF_MIN_MAX;

typedef enum
{
	INIT_SCAN = 0,
	NORMAL_SCAN = 1,
} SCAN_STATES;

typedef struct {
    SCAN_STATES currScan; //4
    SCAN_STATES nextScan; //4
    uint16_t numTouch; //2
    uint16_t x1, y1; //4
    uint16_t x2, y2; //4
#if (BRUSH_MODE_ENABLE == 1) //brush mode, FIXME
    uint8_t brushMode; //1
#endif
} next_scan_t;

typedef enum
{
	NO_BACKEND_ERROR = 0,
	NOT_INITIALIZED = 10,   // 10 error, not initialized
	INVALID_SCAN_MODE = 20, // 20 error, invalid scan mode
	SKIP_HID_REPORT = 30,   // 30 no error, skip hid report
	SKIP_FRONTEND = 40,     // 40 no error, skip frontend
} BACKEND_STATUS;

#if 0 //refer to backend_utils.h
typedef struct {
    float x, y;
} vec_t, *vec;
#endif

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

typedef struct {
	int touch_count;
	vec_t *touch_data;
    tp_size_t *touch_size;
    tp_line_cnt_t *touch_line_cnt;
    int8_t *multi_fine;
#ifdef HOR_EDGE_TOUCH_ENABLE
    vec_t touch_data_edge;
#endif
} DEF_DATA_INFO2;
//---------------------------------------------

//---------------------------------------------
#if defined(_WIN32) || defined(WIN32) //nsmoon@200401, change to DEF_DATA_INFO2
typedef struct {
    uint8_t status;
    uint8_t contactID;
    uint16_t xCord;
    uint16_t yCord;
    uint16_t xSize;
    uint16_t ySize;
#if 1	//shj	for brush, simultaneous release & tip switch
    uint8_t ID_staus;
    uint16_t pressure;
    uint8_t nReportSendCnt;
#endif
} DEF_TOUCH_OUT;

typedef struct {
    uint8_t len;
#if 1	//shj	for brush, simultaneous release & tip switch
    uint8_t reSend;
    uint32_t outcnt;
#endif
    DEF_TOUCH_OUT *buf;
} DEF_DATA_INFO;
#endif
//---------------------------------------------

#endif /* FRONT_IF_H_ */
