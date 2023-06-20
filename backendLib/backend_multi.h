/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Lispect

  @File Name
    backend_main.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */
#ifndef _BACKEND_MULTI_H    /* Guard against multiple inclusion */
#define _BACKEND_MULTI_H
#include "backend_common.h"
#include "backend_utils.h"

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

//#define FINE_USE_CLIPPING
//#define FINE_ADJUST_MIN_MAX

#if 1 //nsmoon@200107
#define MULTI_RANGE_RECAL_MIN_MAX
#endif

////////////////////////////////////////
//cal min and max for center
//#define CAL_MIN_MAX_AVG3
//#define CAL_MIN_MAX_SLOPE_INST

///////////////////////////////////////////////
// Tunning Parameters
///////////////////////////////////////////////
//#define MULTI_MIN_INITIAL_LINES             3

#if defined(LARGE_SEN_GAP_16) //nsmoon@190313
#define MULTI_ADJ_EPA_GAP				18.0f
#endif

#define MULTI_SKIP_LEN_X    35 //nsmoon@230614 //0:no-skiop

#if 1 //(MODEL_TOUCH_SIZE == MODEL_TOUCH_SIZE_75)||(MODEL_TOUCH_SIZE == MODEL_TOUCH_SIZE_85) //nsmoon@191218
#define MULTI_ADJ_SEN_PITCH				10.0f
#define MULTI_LIM_EPA_PAD				5.0f
#define MULTI_LIM_EPA_PAD2				1.0f //2.0f
#define MULTI_LIM_EPA_PAD3				3.0f
#define MULTI_LARGE_PITCH_PAD_FACTOR	0.75f
#define MAX_SIZESQ_ENABLE_VIRTUAL_EP	10000
#endif

#if (MODEL_SPT == MODEL_IRT_550) //nsmoon@190314
#define MULTI_IS_EDGE_DIST_X			30.0f
#define MULTI_IS_EDGE_DIST_Y			40.0f
#define MULTI_MIN_TOUCH_AREA            20.0f //36.0f //25.0f //9.0f  //FIXME, for 8-pi at center area, nsmoon@190805
#define MULTI_MIN_TOUCH_AREA_EDGE       MULTI_MIN_TOUCH_AREA //FIXME, for 8-pi at edge area
#elif (MODEL_SPT == MODEL_SPT_550)
#define MULTI_IS_EDGE_DIST_X			10.0f //6.0f
#define MULTI_IS_EDGE_DIST_Y			30.0f
#define MULTI_MIN_TOUCH_AREA            2.0f  //FIXME, for 8-pi at center area, nsmoon@190805
#define MULTI_MIN_TOUCH_AREA_EDGE       16.0f //FIXME, for 8-pi at edge area
#elif (MODEL_SPT == MODEL_SPT_750)
#define MULTI_IS_EDGE_DIST_X			30.0f
#define MULTI_IS_EDGE_DIST_Y			40.0f
#define MULTI_MIN_TOUCH_AREA            9.0f //16.0f //25.0f //16.0f //9.0f  //FIXME, for 8-pi at center area, nsmoon@190805
#define MULTI_MIN_TOUCH_AREA_EDGE       MULTI_MIN_TOUCH_AREA //FIXME, for 8-pi at edge area
#elif (MODEL_SPT == MODEL_CTKS_750_V120) //nsmoon@191212
#define MULTI_IS_EDGE_DIST_X			30.0f
#define MULTI_IS_EDGE_DIST_Y			40.0f
#define MULTI_MIN_TOUCH_AREA            9.0f //4.0f //9.0f //49.0f //30.0f //25.0f //16.0f //9.0f  //FIXME, for 8-pi at center area, nsmoon@190805
#define MULTI_MIN_TOUCH_AREA_EDGE       MULTI_MIN_TOUCH_AREA //FIXME, for 8-pi at edge area
#elif (MODEL_SPT == MODEL_CTKS_750_V130) || (MODEL_SPT == MODEL_CTKS_750_V140) //nsmoon@200118
#define MULTI_IS_EDGE_DIST_X			EDGE_DIST_X
#define MULTI_IS_EDGE_DIST_Y			EDGE_DIST_Y
#define MULTI_MIN_TOUCH_AREA            16.0f //25.0f //9.0f //4.0f //FIXME
#define MULTI_MIN_TOUCH_AREA2           1.0f
#define MULTI_MIN_TOUCH_AREA_RATIO      0.1f
#define MULTI_MIN_TOUCH_AREA_EDGE       MULTI_MIN_TOUCH_AREA //FIXME, for 8-pi at edge area
#elif (MODEL_SPT == MODEL_CTSI_550_SINGLE_V100) || (MODEL_SPT == MODEL_CTSI_650_V100) || \
    (MODEL_SPT == MODEL_CTSI_N750_V100) || (MODEL_SPT == MODEL_CTSI_N650_V100) //nsmoon@230411
#define MULTI_IS_EDGE_DIST_X			EDGE_DIST_X
#define MULTI_IS_EDGE_DIST_Y			EDGE_DIST_Y
#if defined(GN65_TEST) //nsmoon@230131
#define MULTI_MIN_TOUCH_AREA            8.0f
#elif defined(N75_TEST) || defined(N65_TEST)//nsmoon@230328
#define MULTI_MIN_TOUCH_AREA            36.0f //16.0f //10.0f //nsmoon@230410 16.0f //nsmoon@230418 36.0f
#else
#define MULTI_MIN_TOUCH_AREA            16.0f //10.0f //16.0f //nsmoon@211018 16.0f=>10.0f //nsmoon@211028 10.0f=>16.0f
#endif
#define MULTI_MIN_TOUCH_AREA2           1.0f
#define MULTI_MIN_TOUCH_AREA_RATIO      0.1f
#define MULTI_MIN_TOUCH_AREA_EDGE       MULTI_MIN_TOUCH_AREA //FIXME, for 8-pi at edge area
#elif (MODEL_SPT == MODEL_CTKC_750) //nsmoon@191218
#define MULTI_IS_EDGE_DIST_X			30.0f
#define MULTI_IS_EDGE_DIST_Y			40.0f
#define MULTI_MIN_TOUCH_AREA            30.0f //25.0f //16.0f //9.0f  //FIXME, for 8-pi at center area, nsmoon@190805
#define MULTI_MIN_TOUCH_AREA_EDGE       MULTI_MIN_TOUCH_AREA //FIXME, for 8-pi at edge area
#elif (MODEL_SPT == MODEL_CTSK_850_V100) //nsmoon@200422
#define MULTI_IS_EDGE_DIST_X			EDGE_DIST_X
#define MULTI_IS_EDGE_DIST_Y			EDGE_DIST_Y
#define MULTI_MIN_TOUCH_AREA            10.0f //16.0f //25.0f //9.0f //4.0f //FIXME
#define MULTI_MIN_TOUCH_AREA2           1.0f
#define MULTI_MIN_TOUCH_AREA_RATIO      0.1f
#define MULTI_MIN_TOUCH_AREA_EDGE       MULTI_MIN_TOUCH_AREA //FIXME, for 8-pi at edge area
#elif (MODEL_SPT == MODEL_CTKC_750) //nsmoon@191218
#define MULTI_IS_EDGE_DIST_X			30.0f
#define MULTI_IS_EDGE_DIST_Y			40.0f
#define MULTI_MIN_TOUCH_AREA            30.0f //25.0f //16.0f //9.0f  //FIXME, for 8-pi at center area, nsmoon@190805
#define MULTI_MIN_TOUCH_AREA_EDGE       MULTI_MIN_TOUCH_AREA //FIXME, for 8-pi at edge area
#else
#error "not defined MODEL_SPT"
#endif

#define MULTI_IS_TP_TOL					15.0f //10.0f //8.0f //nsmoon@200407 10.0f=>15.0f
//#define MULTI_MAX_TOUCH_AREA            100.0f
#define MULTI_GHOST_MIN_AREA			MULTI_MIN_TOUCH_AREA
//#define MULTI_GHOST_MIN_AREA2           MULTI_MIN_TOUCH_AREA2
#define MULTI_GHOST_MIN_MAX_TOL			5.0f //3.0f //FIXME,nsmoon@190705, nsmoon@200226

#define MULTI_GHOST_MIN_DIST_SURR		50.0f //60.0f //50.0f, FIXME,nsmoon@190705
#define MULTI_GHOST_MIN_DIST_SURR_EDGE	150.0f //nsmoon190702, FIXME,nsmoon@190708
#define MULTI_GHOST_WIDTH_RATIO		    0.8f //90%=>80%, nsmoon@190128
#define MULTI_GHOST_SIZE_RATIO		    0.7f //0.6f //0.8f //90%=>80%, nsmoon@190128, //60%=>70%, nsmoon@200330
#define MULTI_GHOST_SIZE_RATIO_EDGE     0.8f //nsmoon@200407
#define MULTI_GHOST_WIDTH_PEN_RATIO_EDGE	0.9f
#define MULTI_GHOST_SIZE_PEN_RATIO_EDGE	    0.9f
#define MULTI_GHOST_SIZE_MIN_PEN_EDGE	    49.0f
//#define MULTI_GHOST_WIDTH_RATIO_Y		0.6f //0.8f //0.9f //90%=>80%, nsmoon@190128
//#define MULTI_MAX_ADJACENT_AREA_RATIO	0.9f

#define MULTI_GHOST_MIN_DIST_SIZE_FAR	40.0f  //50.0f
#define MULTI_GHOST_MIN_DIST_SIZE_MID	30.0f
#define MULTI_GHOST_MIN_DIST_SIZE_NEAR	4.0f //nsmoon@190708
#define MULTI_GHOST_MIN_DIST_SIZE_NEAR_EDGE     9.0f //6.0f //nsmoon@200309
#define MULTI_GHOST_MIN_DIST_SIZE_EDGE	150.0f //nsmoon@190708

#define MULTI_GHOST_FAR_SIZE_RATIO		0.15f //0.15f //30%=15%, nsmoon@190129
#define MULTI_GHOST_MID_SIZE_RATIO		0.20f //30%, nsmoon@190129
#define MULTI_GHOST_NEAR_SIZE_RATIO		0.90f
#define MULTI_GHOST_FAR_SIZE_RATIO_EDGE		0.25f //0.50f //nsmoon@200407 0.5=>0.25
#define MULTI_GHOST_MID_SIZE_RATIO_EDGE		0.30f //0.60f //nsmoon@200309 0.6=>0.3
#define MULTI_GHOST_NEAR_SIZE_RATIO_EDGE	1 //0.95f //nsmoon@200323, 1m_0310a.dlt-#219
#define MULTI_NO_CLIPPING_SIZE                  0.1f //nsmoon@210308
#define MULTI_EPA_INST_DIST             2.0f  //nsmoon@190604

//#define MULTI_FINGER_TOUCH_AREA         36.0f
//#define MULTI_FINGER_GRP_MIN_CNT        5

#define MULTI_GROUP_DIST			    100.0f
#define MULTI_GROUP_DIST_SQ		        (MULTI_GROUP_DIST * MULTI_GROUP_DIST)

#define MULTI_CLOSED_TOUCHINFO_CENT_MERGE       100.0f //150.0f
#define MULTI_CLOSED_TOUCHINFO_CENT_MERGE_SQ    (MULTI_CLOSED_TOUCHINFO_CENT_MERGE * MULTI_CLOSED_TOUCHINFO_CENT_MERGE)

#if (MODEL_SPT == MODEL_IRT_550) //nsmoon@190314
//#define MULTI_LIMIT_LIMIT_NUM_X			6 //5->6, nsmoon@190128a
//#define MULTI_LIMIT_LIMIT_NUM_Y			10
//#define MULTI_LIMIT_LIMIT_AREA_RATIO	0.3f
//#define MULTI_LIMIT_LIMIT_AREA			25.0f
//#define MULTI_LIMIT_SAVE_AREA			6.0f  //CLIPPING_CHK_ADJ_VERT
#define MULTI_CAL_MIN_MAX_RANGE			200.0f //FIXME
#define MULTI_CAL_MIN_MAX_TOL           5.0f
#define MULTI_PAD_WIDTH_FACTOR          0.4f //0.49f
#define MULTI_PAD_WIDTH_FACTOR_DZ       0.0f
#define MULTI_PAD_WIDTH                 3.0f
#define MULTI_INITIAL_POLY_PAD_RATIO			0.4f //nsmoon@190321, FIXME 0.4f
#elif (MODEL_SPT == MODEL_SPT_550) //nsmoon@190314
#define MULTI_LIMIT_LIMIT_NUM_X			6 //5->6, nsmoon@190128a
#define MULTI_LIMIT_LIMIT_NUM_Y			10
#define MULTI_LIMIT_LIMIT_AREA_RATIO	0.3f
#define MULTI_LIMIT_LIMIT_AREA			25.0f
#define MULTI_LIMIT_SAVE_AREA			6.0f  //CLIPPING_CHK_ADJ_VERT
#define MULTI_CAL_MIN_MAX_RANGE			200.0f //FIXME
#define MULTI_CAL_MIN_MAX_TOL           5.0f
#define MULTI_PAD_WIDTH_FACTOR          0.4f //0.49f
#define MULTI_PAD_WIDTH_FACTOR_DZ       0.0f
#define MULTI_PAD_WIDTH                 3.0f
#define MULTI_INITIAL_POLY_PAD_RATIO			0.4f //nsmoon@190321, FIXME 0.4f
#else //(MODEL_TOUCH_SIZE == MODEL_TOUCH_SIZE_75)||(MODEL_TOUCH_SIZE == MODEL_TOUCH_SIZE_85) //nsmoon@191218
#define MULTI_LIMIT_LIMIT_NUM_X			8 //6 //5->6 nsmoon@190128a, //6->8 nsmoon@190315, m5p0315a-214
#define MULTI_LIMIT_LIMIT_NUM_Y			10
#define MULTI_LIMIT_LIMIT_AREA_RATIO	0.3f
#define MULTI_LIMIT_LIMIT_WIDTH			5.0f
#define MULTI_LIMIT_LIMIT_AREA			(MULTI_LIMIT_LIMIT_WIDTH * MULTI_LIMIT_LIMIT_WIDTH)
#define MULTI_LIMIT_SAVE_AREA			6.0f
#define MULTI_CAL_MIN_MAX_RANGE			200.0f //FIXME
#define MULTI_CAL_MIN_MAX_TOL           (TOUCH_ERASER_SIZESQ_NORM * 0.5f) //5.0f
#define MULTI_PAD_WIDTH_FACTOR          0.4f //0.49f //0.4f
#define MULTI_PAD_WIDTH_FACTOR_DZ       0.0f
#define MULTI_PAD_WIDTH                 3.0f
#define MULTI_INITIAL_POLY_PAD_RATIO	0.5f //nsmoon@190321, FIXME 0.4f
#define MULTI_INITIAL_POLY_PAD_DIST		1.0f //nsmoon@200129
#define MULTI_INITIAL_POLY_PAD_FIXED	3.0f //nsmoon@200129
#define MULTI_INITIAL_POLY_PAD_0		5.0f
#endif
//---------------------------------------------

//---------------------------------------------
// Definitions
//---------------------------------------------
//#define MAX_NUM_SLOPES				(MAX_NUM_X_SLOPES + MAX_NUM_Y_SLOPES)
#define MAX_RESULT_POLYGON_NO		(MAX_TOUCH_LIMIT_MULTI * (MAX_TOUCH_LIMIT_MULTI)) //FIXME
//#define MAX_RESULT_POLYGON_NO		(MAX_TOUCH_LIMIT_MULTI * 4) //FIXME
#define MAX_MULTI_USED_EPA			MAX_RESULT_POLYGON_NO

#ifdef POLY_VERTEX_LIMIT_ENABLE  //nsmoon@200303
#define MAX_RESULT_VERTEX_NO		POLY_MAX_VERTEX_LEN //FIXME
#define MAX_RESULT_PACK_NO			MAX_RESULT_VERTEX_NO //FIXME
#define MAX_SAVE_POLY_PACK_NO		MAX_RESULT_PACK_NO //FIXME
#else
#define MAX_RESULT_VERTEX_NO		8 //FIXME
#define MAX_RESULT_PACK_NO			10 //FIXME
#define MAX_SAVE_POLY_PACK_NO		MAX_RESULT_PACK_NO //FIXME
#endif

#if defined(_WIN32) || defined(WIN32)
#define MAX_INIT_EP_COUNT			(MAX_TOUCH_LIMIT_MULTI * 5) //X_NUM_PD //10*5
#define MAX_INIT_EP_COUNT_FINE		(MAX_TOUCH_LIMIT_FINE * 5) //X_NUM_PD //10*5
#else
#define MAX_INIT_EP_COUNT			(MAX_TOUCH_LIMIT_MULTI * 5) //X_NUM_PD //10*5, FIXME
#endif
#define MAX_DZ_EP_COUNT				(4)
#ifndef USE_CUST_MALLOC //nsmon@201012
#define MAX_VIRTUAL_EP_COUNT		(X_NUM_PD) //FIXME
#endif

#define MAX_INST_BUFF				(50 * 2)
#define MAX_IN_LINE_BUF				(MAX_EP_COUNT * 4)

#define MAX_IN_LINE_UNIQUE			50
#define MAX_IN_LINE_UNIQUE_MODE		3
#define MAX_RESULT_POINT_NO			20

#ifdef USE_CUST_MALLOC //nsmon@201012
#define EPA_ADDR_X					((ep_buf_t *)&BG_edge_pattern[0])
#define EPA_ADDR_Y					((ep_buf_t *)&BG_edge_pattern[BS_maxHorPdNum]) //nsmoon@201014
#else
#define EPA_ADDR_X					(&BG_edge_pattern[0])
#define EPA_ADDR_Y					(&BG_edge_pattern[X_NUM_PD])
#endif

#define GET_LED_BYTE_IDX(led)		((led) / 8)
#define GET_LED_BYTE(idx)			((idx) * 8)
#define GET_LED_BIT_MASK(led)		((uint8_t)0x1 << ((led) % 8))
//#define GET_LED_BIT_MASK(led)		(bit_mask_byte_arry[(led) % 8])

//input buffer

#ifndef USE_CUST_MALLOC //nsmon@201012
//#define MAX_IN_LINE_BUF_PD			(MAX_NUM_PD) //nsmoon@201013
#define MAX_IN_LINE_BUF_PD_X		(X_NUM_PD) //nsmoon@201013
#define MAX_IN_LINE_BUF_PD_Y		(Y_NUM_PD) //nsmoon@201013
#endif

#define IN_LINE_BUF_ADDR_X_PD		((uint8_t *)&BS_inlineBuf_x_pd[0][0])
#define IN_LINE_BUF_ADDR_X_LED		((uint8_t *)&BS_inlineBuf_x_led[0][0])
#define IN_LINE_BUF_ADDR_Y_PD		((uint8_t *)&BS_inlineBuf_y_pd[0][0])
#define IN_LINE_BUF_ADDR_Y_LED		((uint8_t *)&BS_inlineBuf_y_led[0][0])

#define GET_OFFSET_IN_LINE(pd)      ((pd) * maxInLineBufLed)
#define GET_IN_LINE_IDX(pd,led)		(GET_OFFSET_IN_LINE((pd)) + GET_LED_BYTE_IDX((led)))

//line used
#define MAX_LINE_USED_BUF_PD_X		MAX_IN_LINE_BUF_PD_X
#define MAX_LINE_USED_BUF_LED_X		MAX_IN_LINE_BUF_LED_X
//#define LINE_USED_BUF_ADDR_X		((uint8_t *)&BS_inlineUsed_x_pd[0][0])
#define LINE_USED2_BUF_ADDR_X		((uint8_t *)&BS_inlineUsed2_x_pd[0][0])

#define MAX_LINE_USED_BUF_PD_Y		MAX_IN_LINE_BUF_PD_Y
#define MAX_LINE_USED_BUF_LED_Y		MAX_IN_LINE_BUF_LED_Y
#ifdef USED_LINE_METHOD_OLD
#define LINE_USED_BUF_ADDR_Y		((uint8_t *)&BS_inlineUsed_y_pd[0][0])
#endif
#define LINE_USED2_BUF_ADDR_Y		((uint8_t *)&BS_inlineUsed2_y_pd[0][0])

#ifndef USE_CUST_MALLOC //nsmon@201012
#define MAX_IN_LINE_BUF_LED_X		((MAX_IN_LINE_BUF_PD_X + 7) / 8) //nsmoon@201013
#define MAX_IN_LINE_BUF_LED_Y		((MAX_IN_LINE_BUF_PD_Y + 7) / 8) //nsmoon@201013
#endif

#define GET_LINE_USED_IDX(pd,led)	GET_IN_LINE_IDX((pd),(led))

//#if (MODEL_MCU == PIC_32)
//#define MAX_TOUCH_INFO_MULTI_SIZE	(MAX_TOUCH_LIMIT_MULTI * 2)
//#else
#define MAX_TOUCH_INFO_MULTI_SIZE	(MAX_TOUCH_LIMIT_MULTI * 5)
#define MAX_POLY_INFO_MULTI_SIZE	(MAX_TOUCH_LIMIT_MULTI * 2)
//#endif

#define DEBUG_POS_WIDTH				0.2f
#define MAX_TMP_USED_EP             800 //700 //500 //300 //nsmoon@190328, FIXME
#define MAX_TMP_USED_EPIDX			10 //nsmoon@190703
#define EPA0_INITIAL_INVALID		255 //nsmoon@190322
//---------------------------------------------

#ifndef CURRENT
#define CURRENT 0
#endif
#ifndef PREVIOUS
#define PREVIOUS 1
#endif
#define macT2P(templateStructure, i, j, packsPerPoly) (templateStructure[(i)*(packsPerPoly) + (j)/8][(j)%8])

//---------------------------------------------
// type definitions
//---------------------------------------------
typedef struct {
    uint8_t *dz;
    ep_no_t *ep;
    uint8_t len;
#ifndef USE_CUST_MALLOC //nsmon@201012
    vec_e_t (*poly)[INITIAL_POLYGON_VERTEX_NO];
#endif
} ep_buf_t;

#if 0
typedef struct {
    float BotRightX;
    float BotLeftX;
    float BotRightY;
    float BotLeftY;
    float TopRightX;
    float TopLeftX;
    float TopRightY;
    float TopLeftY;
} pad_width_t;
#endif

#if 0
typedef struct {
    uint8_t sen;  //FIXME uint8_t
    uint8_t ep;
    //uint8_t bd;
} epa_idx_t;
#endif

typedef struct {
    uint8_t slopeIdx;
    uint8_t epaIdx;
} epa_idx_t;

typedef struct {
    float inst;
    uint8_t slopeIdx;
    uint8_t clipIdx;
} epa_inst_t;

typedef struct {
    int8_t stat;
    side_type_t	side;
#if 1 //nsmoon@190715
	uint8_t firstX;
	uint8_t firstY;
#endif
#ifdef MULTI_SKIP_USED_VB //nsmoon@210218
    uint8_t xNo;
    uint8_t yNo;
#endif
	float area;
    ep_min_max_t ep;
    pos_min_max_t pos;
    //vec_t line[RESULT_LINE_VERTEX_NO];
} initial_polygon_t;

typedef struct {
    vec_t pos;
    side_type_t	side;
} initial_point_t;

typedef struct {
    int start, end;
} start_end_t;

typedef struct {
    float start, end;
} start_end_pos_t;

typedef struct {
    uint8_t x, y; //FIXME uint8_t
} sen_no_t;

typedef struct {
    int idx;
    bd_type_t bdX, bdY;
    sen_no_t centSen;
    //start_end_t stEdX, stEdY;
} cur_poly_t;

#if 0 //for testing
typedef struct {
    int8_t stat;
    vec_t line[RESULT_LINE_VERTEX_NO];
} result_line_t;
#endif

#if 0 //nsmoon@190715a
//#define TEST_0321
#define ENUM_POLYGON_RIGHT_MASK	0x0F
#define ENUM_POLYGON_LEFT_MASK	0xF0
typedef enum {
	ENUM_POLYGON_START = 0x00,
	ENUM_POLYGON_RIGHT = 0x01,
	ENUM_POLYGON_RIGHT_AGAIN = 0x02,
	ENUM_POLYGON_LEFT = 0x10,
	ENUM_POLYGON_LEFT_AGAIN = 0x20,
    ENUM_POLYGON_STOP = UINT8_MAX,
	ENUM_POLYGON_GHOST = 0xBD, //ghost, nsmoon@190701
} polygon_state_t;
#endif

typedef struct {
    uint8_t right;
    uint8_t left;
} polygon_range_t;

#define MULTISTAT_REMOVED_MASK	0x0F //removed mask
#define MULTISTAT_NOCLIPPING	0x01
#define MULTISTAT_MERGED		0x02
#define MULTISTAT_GHOST			0x04

#define MULTISTAT_SKIP_MASK		0xF0 //info mask
#define MULTISTAT_BIG			0x10
#define MULTISTAT_SMALL 		0x20
#define MULTISTAT_CLOSED		0x40
#define MULTISTAT_SKIP			0x80

typedef struct {
	uint8_t multiStat;
	uint8_t polLen;
    //uint8_t clipCnt;
    bd_type_t bdX, bdY; //board
    polygon_range_t sRx, sRy; //sensor range
	float multiArea;
    pos_min_max_t mM; //min,max
	pos_min_max_t mM2; //th10
	vec_t centerPos;
#ifdef FRONTEND_LINE_THRESHOLD //nsmoon@200119
	uint8_t th50CntX;
	uint8_t th10CntX;
	uint8_t th50CntY;
	uint8_t th10CntY;
#endif
    uint8_t eraser_pen; //nsmoon@200311
    uint8_t no_clip_y; //nsmoon@210308
} touch_info_multi_t;

typedef struct {
    //uint8_t clipCnt;
    uint8_t polLen;
    bd_type_t bdX, bdY; //board
    polygon_range_t sRx, sRy; //sensor range
    float multiArea;
    pos_min_max_t mM; //min,max
    vec_t centerPos;
} poly_info_multi_t;

typedef struct {
    axis_t axis;
    sen_type_t sen;
#if 1 //nsmoon@190703
	uint8_t slopeIdx;
	uint8_t epIdx;
#else
    ep_no_t *ep;
#endif
} save_ep_t;

typedef struct {
    uint16_t maxPolygonErrCnt;
    uint16_t minTouchAreaErrCnt;
    uint16_t noClippingErrCnt;
    uint8_t totalProcessedSlopeCnt;
    uint8_t storage;
    int16_t maxPolygonCnt;
    //---------------------------
    uint8_t  subject_cnt;
    uint8_t  subject_packs;
    uint8_t  *subject_length;
    vec_e_t(*subject_vertex)[MAX_RESULT_VERTEX_NO];
    float    *subject_area;
#ifdef MULTI_SENSOR_RANGE_OLD //nsmoon@200119
    polygon_range_t  *subject_range;
#endif
    //---------------------------

    //---------------------------
    uint8_t  polygon_cnt;
    uint8_t  polygon_packs;
    uint8_t  *polygon_length;
    vec_e_t(*polygon_vertex)[MAX_RESULT_VERTEX_NO];
    float    *polygon_area;
#ifdef MULTI_SENSOR_RANGE_OLD //nsmoon@200119
    polygon_range_t  *polygon_range;
#endif
    //---------------------------

    //ta_pos_t  ta_pos[MAX_RESULT_POLYGON_NO];
    int8_t  poly_idx_saved[MAX_RESULT_POLYGON_NO];
    int8_t  touch_idx_saved[MAX_TOUCH_LIMIT_MULTI];
    //polygon_range_t  sensor_range_x[MAX_RESULT_POLYGON_NO];
    //polygon_range_t  sensor_range_y[MAX_RESULT_POLYGON_NO];
    uint8_t eraser_pen; //nsmoon@200306 (1:eraser,2:pen)
    uint8_t no_clip_y; //nsmoon@210308
} polygon_clipping_info_t;

//---------------------------------------------

//---------------------------------------------
// extern variable and functions
//---------------------------------------------
extern float BS_pad_width_factor_multi;
extern float BS_pad_width_multi;

extern int BS_line_intersection(vec p0, vec p1, vec p2, vec p3, vec res);
extern int BS_line_intersection_test(float *posX, int idX, float *posY, int idY);
extern side_type_t BS_getSideFromPos(vec_t *pos);
extern int BS_drawPolyScore(void);

extern void BG_epaInit(void);
extern int BG_clipping_multi_initial(void);
extern int BG_clipping_multi_initial4(void);
extern int BG_clipping_multi(int multiLoopCnt);
extern int BG_clipping_multi_small(int multiLoopCnt);
extern int BG_clipping_multi_post(int multiLoopCnt);
extern int BS_multi_is_overlap_min_max_tp(pos_min_max_t mM, int excludeRemoved, int compareMode);
extern int BG_clipping_fine2(void);
extern void BS_epaInitMulti(void);
extern int test_initial_fine(void);
//extern int BS_packEdgePattern(axis_t axis, int mode);

#ifdef USE_CUST_MALLOC //nsmon@201012
extern ep_buf_t *BG_edge_pattern;
extern ep_buf_t *BS_edge_pattern_x, *BS_edge_pattern_y;
#else
extern ep_buf_t BG_edge_pattern[MAX_NUM_PD];
extern ep_buf_t BS_edge_pattern_x[MAX_INIT_EPA_SIZE];
extern ep_buf_t BS_edge_pattern_y[MAX_INIT_EPA_SIZE];
#endif
extern int BG_edge_pattern_cnt_x, BG_edge_pattern_cnt_y;

//input buffer
#ifdef USE_CUST_MALLOC //nsmon@201012
extern int BS_max_in_line_buf_pd_x, BS_max_in_line_buf_led_x;
extern int BS_max_in_line_buf_pd_y, BS_max_in_line_buf_led_y;
extern uint8_t **BS_inlineBuf_x_pd; //[MAX_IN_LINE_BUF_PD_X][MAX_IN_LINE_BUF_LED_X]
extern uint8_t **BS_inlineBuf_x_led; //[MAX_IN_LINE_BUF_PD_X][MAX_IN_LINE_BUF_LED_X]
extern uint8_t **BS_inlineBuf_y_pd; //[MAX_IN_LINE_BUF_PD_Y][MAX_IN_LINE_BUF_LED_Y]
extern uint8_t **BS_inlineBuf_y_led; //[MAX_IN_LINE_BUF_PD_Y][MAX_IN_LINE_BUF_LED_Y]
#ifdef USED_LINE_METHOD_OLD
extern uint8_t **BS_inlineUsed_x_pd; //[MAX_LINE_USED_BUF_PD_X][MAX_LINE_USED_BUF_LED_X]
extern uint8_t **BS_inlineUsed_y_pd; //[MAX_LINE_USED_BUF_PD_Y][MAX_LINE_USED_BUF_LED_Y]
#else
extern uint8_t **BS_inlineUsed2_x_pd; //[MAX_LINE_USED_BUF_PD_X][MAX_LINE_USED_BUF_LED_X]
extern uint8_t **BS_inlineUsed2_y_pd; //[MAX_LINE_USED_BUF_PD_Y][MAX_LINE_USED_BUF_LED_Y]
#endif
#else
extern uint8_t BS_inlineBuf_x_pd[MAX_IN_LINE_BUF_PD_X][MAX_IN_LINE_BUF_LED_X];
extern uint8_t BS_inlineBuf_x_led[MAX_IN_LINE_BUF_PD_X][MAX_IN_LINE_BUF_LED_X];
extern uint8_t BS_inlineBuf_y_pd[MAX_IN_LINE_BUF_PD_Y][MAX_IN_LINE_BUF_LED_Y];
extern uint8_t BS_inlineBuf_y_led[MAX_IN_LINE_BUF_PD_Y][MAX_IN_LINE_BUF_LED_Y];
#ifdef USED_LINE_METHOD_OLD
extern uint8_t BS_inlineUsed_x_pd[MAX_LINE_USED_BUF_PD_X][MAX_LINE_USED_BUF_LED_X];
extern uint8_t BS_inlineUsed_y_pd[MAX_LINE_USED_BUF_PD_Y][MAX_LINE_USED_BUF_LED_Y];
#else
extern uint8_t BS_inlineUsed2_x_pd[MAX_LINE_USED_BUF_PD_X][MAX_LINE_USED_BUF_LED_X];
extern uint8_t BS_inlineUsed2_y_pd[MAX_LINE_USED_BUF_PD_Y][MAX_LINE_USED_BUF_LED_Y];
#endif
#endif

extern int maxVertex, maxProcessCnt, maxSubjCnt, maxClipIdx;

extern initial_polygon_t BS_initial_polygon[MAX_RESULT_POLYGON_NO]; //FIXME MAX_RESULT_POLYGON_NO??
extern int BS_initial_polygon_cnt;
extern initial_point_t BS_initial_point[MAX_RESULT_POINT_NO]; //FIXME MAX_RESULT_POLYGON_NO??
extern int BS_initial_point_cnt;

extern polygon_clipping_info_t BS_pclipping_info;

#ifdef DRAW_POLYGON_TEST
#define MAX_DEBUG_TOUCH_POLYGON     (MAX_RESULT_POLYGON_NO*2)
extern vec_e_t DEBUG_touch_polygon[MAX_DEBUG_TOUCH_POLYGON][MAX_RESULT_VERTEX_NO]; //FIXME
extern int DEBUG_touch_polygon_cnt;
extern int DEBUG_touch_polygon_len[MAX_DEBUG_TOUCH_POLYGON]; //FIXME
#endif

extern touch_info_multi_t BS_touch_info_multi[MAX_TOUCH_INFO_MULTI_SIZE];
extern uint8_t BS_touch_info_multi_sort[MAX_TOUCH_INFO_MULTI_SIZE];
extern uint8_t BS_touch_info_multi_grp[MAX_TOUCH_LIMIT_MULTI];
//extern uint8_t BS_touch_info_multi_outgrp[MAX_TOUCH_LIMIT_MULTI];
extern int BS_touch_info_multi_cnt; //BS_touch_info_multi_cnt_start;

#ifdef USE_CUST_MALLOC //nsmon@201012
extern int BS_max_ini_epa_size;
extern int BS_multi_malloc(void);
#endif
#if 1 //nsmoon@210216
int addEdgePatternHalfPad(axis_t axis, ep_buf_t *epa_all, int slopeIdx, int clipIdx, vec_e_t *buf, int paddingMode);
#endif

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif
#endif //_BACKEND_MULTI_H
/* *****************************************************************************
 End of File
 */
