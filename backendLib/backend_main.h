/* ************************************************************************** */
/** Descriptive File Name

  @Company
    lispect.com

  @File Name
    backend_main.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ***************************************************************************/
#ifndef _BACKEND_MAIN_H    /* Guard against multiple inclusion */
#define _BACKEND_MAIN_H

#include "backend_common.h"
#include "backend_utils.h"
#include "backend_debug.h"
#include "front_if.h"

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

#define MIN_NUM_IN_LINES         3
#define MIN_NUM_IN_LINES_MULTI   50 //40
#define MIN_NUM_REMAINED_FINE    5 //8

#if 1 //def MULTI_PACK_EP3_ENABLE
#define EPA_IDX_UNKNOWN  -1
#define EPA_IDX_0        0
#define EPA_IDX_1        1
#define EPA_IDX_2        2
#define EPA_IDX_3        3
#define EPA_IDX_LEN      4
#endif

#if 1 //nsmoon@201117
#define MAX_MULTI_LOOP_CNT  2  //3 //nsmoon@200814
#define REM_LOOP_CNT_FINE5  (MAX_MULTI_LOOP_CNT)   //2
#define REM_LOOP_CNT_XY     (MAX_MULTI_LOOP_CNT+1) //3
#define REM_LOOP_CNT_SHADOW (MAX_MULTI_LOOP_CNT+2) //4
#define REM_LOOP_CNT_BRUSH  (MAX_MULTI_LOOP_CNT+3) //5
#define REM_LOOP_CNT_FINAL  (MAX_MULTI_LOOP_CNT+4) //6 //nsmoon@210430
#endif

//////////////////////////////////////////////////////
// Exported Global function prototypes
//
#if 1 //(SECOND_OFST_TBL == 1) //nsmoon@190625
#if (DEAD_CELL_TBL == 1) //nsmoon@190829
int BG_init_backend(
	DEF_TP_LAYOUT_INFO *tpLayout, 	//sensor laypout
    DEF_DEADCELL_INFO  *deadCellTbl, //dead cell table
	uint16_t  init_flag          // reserved: 0
);
#else
int BG_init_backend(
	DEF_TP_LAYOUT_INFO *tpLayout, 	//sensor laypout
	DEF_PD_INFO  *ValidTbl,			//not used yet
	uint16_t  init_flag          // reserved: 0
);
#endif
#else
extern int BG_init_backend(
    uint16_t  hor_pd_num,	//number of sensor on X axis
    uint16_t  ver_pd_num,	//number of sensor on Y axis
    ofs_tbl_t *ofsTblX,		//offset table of X axis
    ofs_tbl_t *ofsTblY,		//offset table of Y axis
    DEF_PD_INFO  *ValidTbl,			//not used yet
    DEF_TP_LAYOUT_INFO *tpLayout, 	//sensor laypout
    uint16_t  init_flag,          // reserved: 0
    uint16_t  logical_x_size,	//logical horizontal size
    uint16_t  logical_y_size	//logical vertical size
);
#endif

extern BACKEND_STATUS BG_call_backend2(
	DEF_PD_INFO *InBuf,       // (hor_pd_num + ver_pd_num)
    DEF_DATA_INFO2 *OutBuf,
	next_scan_t *nextScan     // next scan infomation
);

#ifdef USE_CUST_MALLOC //nsmoon@201012
extern int BS_maxHorPdNum, BS_maxVerPdNum; //nsmoon@201013
#else
extern uint16_t BS_maxHorPdNum;
extern uint16_t BS_maxVerPdNum;
#endif

#ifdef USE_CUST_MALLOC //nsmoon@201012
extern int BS_max_ep_count, BS_max_mum_pd;
extern ep_no_t *BS_edge_pattern_ep;
extern uint8_t *BS_edge_pattern_deadzone; //nsmoon@190326
#else
extern ep_no_t BS_edge_pattern_ep[MAX_EP_COUNT];
extern vec_e_t BS_edge_pattern_poly[MAX_EP_COUNT][INITIAL_POLYGON_VERTEX_NO];
extern uint8_t BS_edge_pattern_deadzone[MAX_EP_COUNT]; //nsmoon@190326
#endif
extern int BS_edge_pattern_ep_cnt;

#ifdef USE_CUST_MALLOC //nsmoon@201012
extern int8_t *BS_offsetTblX, *BS_offsetTblY;
#else
#if 1 //(SECOND_OFST_TBL == 1) //nsmoon@190625
extern int8_t BS_offsetTblX[MAX_X_SLOPE_IDX];
extern int8_t BS_offsetTblY[MAX_Y_SLOPE_IDX];
#endif
#endif
extern int BS_offsetTblLenX, BS_offsetTblLenY;
extern int BS_slopeValMaxX, BS_slopeValMaxY;
extern float BS_ratioXforP2L, BS_ratioYforP2L;
extern int BG_touch_count;
extern int BG_touch_count_multi, BG_touch_count_fine;
extern DEF_PD_INFO *BS_inBuf;
extern uint16_t BS_logical_max_x, BS_logical_max_y;

extern vec_t BG_touch_data[ALLOWABLE_TOUCH_BE];
extern tp_size_t BG_touch_size[ALLOWABLE_TOUCH_BE];
extern float BG_touch_area[ALLOWABLE_TOUCH_BE];
extern int8_t BG_multi_fine[ALLOWABLE_TOUCH_BE]; //for debugging
extern int8_t BG_touch_misc[ALLOWABLE_TOUCH_BE];
extern int8_t BS_touch_sort[ALLOWABLE_TOUCH_BE]; //nsmoon@211027
#ifdef HOR_EDGE_TOUCH_ENABLE
extern vec_t BG_touch_data_edge; //nsmoon@200415
#endif
#ifdef FRONTEND_LINE_THRESHOLD
extern tp_line_cnt_t BG_touch_lineCnt[ALLOWABLE_TOUCH_BE];
#endif
//#ifdef FRONTEND_LINE_THRESHOLD
//extern uint8_t BG_touch_threshold[ALLOWABLE_TOUCH_BE];
//#endif
//extern int8_t BS_pdOffsetTbl[MAX_NUM_SLOPES];
//extern int    BS_pdOffsetTblLen;
#if 1 //(MODEL_SPT == MODEL_CTSK_850_V100) //nsmoon@200910
#define MAX_REMAINED_LINE			500 //300
#define MAX_REMAINED_LINE_SORT		100 //nsmoon@211028
#else
#define MAX_REMAINED_LINE			1024 //500 //nsmoon@200910, for test
#endif
extern uint16_t BS_remained_x[MAX_REMAINED_LINE];
extern uint16_t BS_remained_y[MAX_REMAINED_LINE];
extern int BS_remained_x_cnt, BS_remained_y_cnt;
extern uint16_t BS_remained_sort[MAX_REMAINED_LINE_SORT]; //nsmoon@211028

#if 1 //nsmoon@191209
#define MAX_REMAINED2_LINE			100
extern uint16_t BS_remained2_x[MAX_REMAINED2_LINE];
extern uint16_t BS_remained2_y[MAX_REMAINED2_LINE];
extern int BS_remained2_x_cnt, BS_remained2_y_cnt;
#define MAX_LARGE_TOUCH_IDX			(ALLOWABLE_TOUCH_BE/2)
extern pos_min_max_t BS_large_touch_idx[MAX_LARGE_TOUCH_IDX];
extern int BS_large_touch_idx_cnt;
extern pos_min_max_t BS_large_touch_idx_prev[2][MAX_LARGE_TOUCH_IDX];
extern int BS_large_touch_idx_cnt_prev[2];
#define MAX_REMAINED_USED_LINE		((MAX_REMAINED_LINE + 7) / 8)
extern uint8_t BS_remained_used_x[MAX_REMAINED_USED_LINE];
extern uint8_t BS_remained_used_y[MAX_REMAINED_USED_LINE];
#endif

#if 1 //nsmoon@190812
#define MAX_INST_SORT_TBL_SIZE      256 //do not increase //nsmoon@211125
#define MAX_MINMAX_INST_TBL         MAX_INST_SORT_TBL_SIZE
extern vec_t BS_minmax_inst_xy[MAX_MINMAX_INST_TBL];
extern uint16_t BS_minmax_inst_stat[MAX_MINMAX_INST_TBL];
#define INT_SECT_GRP_EPF_USED_SIZE	(15) //FIXME
extern uint16_t BS_int_sect_group2_epf_used[INT_SECT_GRP_EPF_USED_SIZE];
extern int BS_int_sect_group2_epf_used_cnt;
extern int BS_is_epf_used(uint16_t epfNo);
extern void BS_add_epf_used(uint16_t epfNo);
#endif

#if (SECOND_OFST_TBL == 1) //nsmoon@190625
#error FIXME
extern int BS_slopeValMaxX2, BS_slopeValMaxY2;
extern uint8_t *BS_senTblX2, *BS_senTblY2;
extern int BS_senTblLenX2, BS_senTblLenY2;
extern uint8_t BS_senTblX2found[MAX_2ND_SLOPE_IDX], BS_senTblY2found[MAX_2ND_SLOPE_IDX];
//extern int BS_senTblLenX2cnt, BS_senTblLenY2cnt;
extern int BS_is_near_2nd_ofst_led(axis_t axis, int led);
#endif
#if (DEAD_CELL_TBL == 1) //nsmoon@190829
extern uint8_t *BS_inValidTblXpd;
extern uint8_t *BS_inValidTblXled;
extern uint8_t *BS_inValidTblYpd;
extern uint8_t *BS_inValidTblYled;
extern uint8_t BS_inValidTblLenXpd;
extern uint8_t BS_inValidTblLenXled;
extern uint8_t BS_inValidTblLenYpd;
extern uint8_t BS_inValidTblLenYled;
extern int BS_is_near_dead(axis_t axis, sen_type_t sen, int senNo, int mode); //nsmoon@191202
#endif

//////////////////////////////////////////////////
//for debugging
//////////////////////////////////////////////////
extern int BG_show_ep;
//extern int BG_show_line;
extern int BG_show_line_x;
extern int BG_show_line_y;
extern int BG_show_line_init;
extern int BG_show_line_all;
extern int BG_show_line_th10;
extern int BG_show_line_rem;

extern int BG_show_debug;
extern int BG_show_debug_max;
extern unsigned int BG_frame_no;
extern int BG_debug_pause;

#define MAX_BG_DEBUG_MISC   (ALLOWABLE_TOUCH_BE * 2) //20
extern int BG_debug_misc[MAX_BG_DEBUG_MISC];

#if defined(DEBUG_FUNCTION_ENABLE_ALL) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
#define MAX_GRID_CAL_SLOT   (50)
#define MAX_LINE_CAL_SLOT	(500)
typedef struct {
    axis_t axis;
    uint8_t line;
    uint8_t cxp;
    uint8_t slotCnt;
    uint8_t calMode;
    uint8_t *slot;
    uint8_t maxCnt;
    uint8_t diffSumMin;
    uint8_t gridWidth;
} cal_slot_t;
extern cal_slot_t BG_cal_slot_cur[MAX_GRID_CAL_SLOT];
extern cal_slot_t BG_cal_slot_org[MAX_GRID_CAL_SLOT];
extern uint8_t BG_cal_slot_buf[MAX_LINE_CAL_SLOT];
extern int BG_cal_slot_buf_idx;
extern int BG_cal_slot_cur_cnt, BG_cal_slot_org_cnt;
#endif
//////////////////////////////////////////////////

#if (BRUSH_MODE_ENABLE == 1) //brush mode, nsmoon@200227
extern int BS_brushMode;
#endif

#ifdef DRAW_POLYGON_TEST
//#define SHOW_DEBUG_SIZE
#endif
#ifdef SHOW_DEBUG_SIZE
extern int BG_debug_minX, BG_debug_maxX;
extern int BG_debug_minY, BG_debug_maxY;
extern int BG_debug_minX2, BG_debug_maxX2;
extern int BG_debug_minY2, BG_debug_maxY2;
extern unsigned int BG_debug_minXY, BG_debug_maxXY;
extern int BG_debug_cnt_total;
extern int BG_debug_cnt_true;
extern int BG_debug_cnt_false;
#endif
#ifdef SHOW_DEBUG_TRACKING
extern uint32_t BG_debug_frameperiod;
extern int32_t BG_debug_diff_x, BG_debug_diff_y;
#endif
#ifdef SHOW_DEBUG_SIZE_VAR
extern int BG_debug_down_skip_cnt;
extern int BG_debug_up_skip_cnt;
extern float BG_debug_sizeSq, BG_debug_sizeX, BG_debug_sizeY;
extern float BG_debug_sizeSqCur, BG_debug_sizeXcur, BG_debug_sizeYcur;
extern float BG_debug_posXcur, BG_debug_posYcur;
extern int BG_debug_size_type, BG_debug_curCnt, BG_debug_curStat;
#endif
#if 0 //def SHOW_DEBUG_SIZE_DOWN //for test
extern float BG_debug_sizeSq, BG_debug_sizeX, BG_debug_sizeY;
extern float BG_debug_sizeSqCur, BG_debug_sizeXcur, BG_debug_sizeYcur;
extern float BG_debug_posXcur, BG_debug_posYcur;
extern int BG_debug_size_type, BG_debug_curCnt, BG_debug_curStat;
extern float BG_debug_sizeSqMax, BG_debug_sizeSqMin;
extern float BG_debug_sizeXMax, BG_debug_sizeXMin;
extern float BG_debug_sizeYMax, BG_debug_sizeYMin;
extern float BG_th50RatioX, BG_th50RatioY;
extern float BG_sizeMin;
extern int BG_th10CntMin;
extern float BG_sizeMulMin;
extern float BG_sizeMulMin_min;
extern float BG_sizeMulMin_max;
extern float BG_debug_sizeSq_initial, BG_debug_sizeSq_initial_max;
extern int BG_debug_th50X, BG_debug_th50Y, BG_debug_th10X, BG_debug_th10Y;
extern int BG_debug_th50X_used, BG_debug_th50Y_used, BG_debug_th10X_used, BG_debug_th10Y_used;
extern int BG_debug_insideArea;
extern float BG_debug_th10sizeXcur, BG_debug_th10sizeYcur;
extern float BG_debug_th10sizeXMin, BG_debug_th10sizeXMax;
extern float BG_debug_th10sizeYMin, BG_debug_th10sizeYMax;
extern float BG_debug_sizeWeight;
#endif
#ifdef SHOW_DEBUG_SIZE_DOWN_0205
extern float BG_debug_posXcur, BG_debug_posYcur;
extern int BG_debug_th50X, BG_debug_th10X, BG_debug_th50Y, BG_debug_th10Y;
extern int BG_debug_th50X_used, BG_debug_th10X_used, BG_debug_th50Y_used, BG_debug_th10Y_used;
extern float BG_debug_sizeXcur, BG_debug_th10sizeXcur, BG_debug_sizeYcur, BG_debug_th10sizeYcur;
extern float BG_debug_sizeXMin, BG_debug_sizeXMax, BG_debug_sizeYMin, BG_debug_sizeYMax;
extern float BG_debug_th10sizeXMin, BG_debug_th10sizeXMax, BG_debug_th10sizeYMin, BG_debug_th10sizeYMax;
extern float BG_debug_X10Y50, BG_debug_X50Y10;
extern int BG_debug_size_type, BG_debug_edge;
#endif

#ifdef FINE_RULE_1A_TEST
extern vec_t BG_debug_touch_data[ALLOWABLE_TOUCH_BE];
extern float BG_debug_touch_area[ALLOWABLE_TOUCH_BE];
extern tp_size_t BG_debug_touch_size[ALLOWABLE_TOUCH_BE];
extern int8_t BG_debug_multi_fine[ALLOWABLE_TOUCH_BE];
extern pos_minMax2_t BG_debug_touch_area_minmax[ALLOWABLE_TOUCH_BE];
#endif

#define LARGE_PITCH_TBL     1
#if (LARGE_PITCH_TBL == 1) //nsmoon@200108
extern int BS_large_pitch_tbl_x_pd_cnt, BS_large_pitch_tbl_x_led_cnt;
extern int BS_large_pitch_tbl_y_pd_cnt, BS_large_pitch_tbl_y_led_cnt;
extern int BS_is_large_pitch(axis_t axis, sen_type_t sen, int senNo); //nsmoon@200108
#endif

#ifdef USE_SENSOR_LIST //for test, nsmoon@200327
#ifdef USE_CUST_MALLOC //nsmoon@201012
extern uint8_t *BS_pd_x, *BS_led_x, *BS_pd_y, *BS_led_y;
#else
extern uint8_t BS_pd_x[X_NUM_PD];
extern uint8_t BS_led_x[X_NUM_PD];
extern uint8_t BS_pd_y[Y_NUM_PD];
extern uint8_t BS_led_y[Y_NUM_PD];
#endif
extern int BS_pd_x_cnt, BS_led_x_cnt, BS_pd_y_cnt, BS_led_y_cnt;
#endif

//--------------------------------------------
//FIXME
extern void BS_epaInit_fine(void);
extern void BS_addEpaPosMulti(bd_type_t bdType);
extern int BS_set_line_buffer(axis_t axis, int led, int pd); //nsmoon@190926
extern int BS_is_dead_cell(axis_t axis, sen_type_t sen, int senNo);
extern int BS_is_dead_cell_pos(axis_t axis, sen_type_t sen, float *pos);
extern int BS_is_set_threshold(axis_t axis, int pd, int ofstIdx);
extern int BS_is_set_line_buffer(axis_t axis, int pd, int led);
extern int BS_is_used_line_buffer(axis_t axis, int pd, int led);
extern int BS_get_remained_line(axis_t axis, int multiLoopCnt);
extern int BS_set_unset2_used_led_pd(axis_t axis, int led, int pd, int mode);
extern void BS_debug_inlines(void);

#ifdef LINPACK_TEST
extern void Linpack_test();
#endif

//extern void getPdOffsetTbl(uint8_t *src, int8_t *dst);

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _BACKEND_MAIN_H */

/* *****************************************************************************
 End of File
 */
