/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Lispect

  @File Name
    backend_main.c

  @Summary
    Backend Main Routine

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */
#include "backend_main.h"
#include "backend_multi.h"
#include "backend_fine.h"
#include "backend_debug.h"
#include "IRTP_layout.h"
//#include "pd_led_tbl.h"
#if (MODEL_MCU == RT1052) || (MODEL_MCU == RT1064)
//#include <cr_section_macros.h>
#endif

#if defined(_WIN32) || defined(WIN32)
#include <stdarg.h>
#include <stdio.h>
#ifdef DEBUG_TOOL_QT
#include <string.h>
#endif
#else
#include <string.h>
#endif //defined(_WIN32) || defined(WIN32)

#ifdef USE_CUST_MALLOC //nsmoon@201012
ATTR_BACKEND_RAM2 int8_t *BS_offsetTblX, *BS_offsetTblY;
#else
ATTR_BACKEND_RAM2 int8_t BS_offsetTblX[MAX_X_SLOPE_IDX];
ATTR_BACKEND_RAM2 int8_t BS_offsetTblY[MAX_Y_SLOPE_IDX];
#endif
ATTR_BACKEND_RAM2 int BS_offsetTblLenX, BS_offsetTblLenY;
ATTR_BACKEND_RAM2 int BS_slopeValMaxX, BS_slopeValMaxY;
//int global_data;

ATTR_BACKEND_RAM2 int BS_slopeValMaxX2, BS_slopeValMaxY2;
#if (SECOND_OFST_TBL == 1) //nsmoon@190625
ATTR_BACKEND_RAM2 uint8_t *BS_senTblX2, *BS_senTblY2;
ATTR_BACKEND_RAM2 int BS_senTblLenX2, BS_senTblLenY2;
ATTR_BACKEND_RAM2 uint8_t BS_senTblX2found[MAX_2ND_SLOPE_IDX], BS_senTblY2found[MAX_2ND_SLOPE_IDX];
//ATTR_BACKEND_RAM2 int BS_senTblLenX2cnt, BS_senTblLenY2cnt;
#endif
#if (DEAD_CELL_TBL == 1) //nsmoon@190829
ATTR_BACKEND_RAM2 uint8_t *BS_inValidTblXpd;
ATTR_BACKEND_RAM2 uint8_t *BS_inValidTblXled;
ATTR_BACKEND_RAM2 uint8_t *BS_inValidTblYpd;
ATTR_BACKEND_RAM2 uint8_t *BS_inValidTblYled;
ATTR_BACKEND_RAM2 uint8_t BS_inValidTblLenXpd;
ATTR_BACKEND_RAM2 uint8_t BS_inValidTblLenXled;
ATTR_BACKEND_RAM2 uint8_t BS_inValidTblLenYpd;
ATTR_BACKEND_RAM2 uint8_t BS_inValidTblLenYled;
#endif
#if (USE_VALID_OFST_TBL == 1)  //nsmoon@191202
//valid offset table
ATTR_BACKEND_RAM2 uint8_t *BS_valOfstTblX;
ATTR_BACKEND_RAM2 uint8_t *BS_valOfstTblY;
#endif

ATTR_BACKEND_RAM DEF_PD_INFO *BS_inBuf;
#if (USE_ADC_DATA == 1)
ATTR_BACKEND_RAM uint8_t *BS_ver_adc, *BS_hor_adc;
#endif

#ifdef GET_SLOPE_0
ATTR_BACKEND_RAM2 static uint8_t slope_cnt0_x, slope_cnt0_y;
#endif
ATTR_BACKEND_RAM2 static int backend_initialized;  // 0: not initialized, 1: initialized
ATTR_BACKEND_RAM2 uint16_t BS_logical_max_x, BS_logical_max_y; //logical touch size

////////////////////////////////////////
#ifdef USE_CUST_MALLOC //nsmoon@201012
ATTR_BACKEND_RAM2 int BS_max_ep_count, BS_max_mum_pd;
ATTR_BACKEND_RAM2 ep_no_t *BS_edge_pattern_ep;
ATTR_BACKEND_RAM2 uint8_t *BS_edge_pattern_deadzone; //nsmoon@190326
ATTR_BACKEND_RAM2 int BS_edge_pattern_ep_cnt;
#else
ATTR_BACKEND_RAM3 ep_no_t BS_edge_pattern_ep[MAX_EP_COUNT];
ATTR_BACKEND_RAM3 vec_e_t BS_edge_pattern_poly[MAX_EP_COUNT][INITIAL_POLYGON_VERTEX_NO];
ATTR_BACKEND_RAM3 uint8_t BS_edge_pattern_deadzone[MAX_EP_COUNT]; //nsmoon@190326
ATTR_BACKEND_RAM3 int BS_edge_pattern_ep_cnt;
#endif

ATTR_BACKEND_RAM2 uint16_t BS_remained_x[MAX_REMAINED_LINE];
ATTR_BACKEND_RAM2 uint16_t BS_remained_y[MAX_REMAINED_LINE];
ATTR_BACKEND_RAM2 int BS_remained_x_cnt, BS_remained_y_cnt;
ATTR_BACKEND_RAM2 uint16_t BS_remained_sort[MAX_REMAINED_LINE_SORT]; //nsmoon@211028

#if 0 //nsmoon@191209
ATTR_BACKEND_RAM2 uint16_t BS_remained2_x[MAX_REMAINED2_LINE];
ATTR_BACKEND_RAM2 uint16_t BS_remained2_y[MAX_REMAINED2_LINE];
ATTR_BACKEND_RAM2 int BS_remained2_x_cnt, BS_remained2_y_cnt;
#endif
#if 1 //nsmoon@191209
ATTR_BACKEND_RAM2 pos_min_max_t BS_large_touch_idx[MAX_LARGE_TOUCH_IDX];
ATTR_BACKEND_RAM2 int BS_large_touch_idx_cnt;
ATTR_BACKEND_RAM2 pos_min_max_t BS_large_touch_idx_prev[2][MAX_LARGE_TOUCH_IDX];
ATTR_BACKEND_RAM2 int BS_large_touch_idx_cnt_prev[2];
ATTR_BACKEND_RAM2 uint8_t BS_remained_used_x[MAX_REMAINED_USED_LINE];
ATTR_BACKEND_RAM2 uint8_t BS_remained_used_y[MAX_REMAINED_USED_LINE];
#endif

ATTR_BACKEND_RAM2 int BG_touch_count;
#if defined(_WIN32) || defined(WIN32)
int BG_touch_count_prev;
#endif
ATTR_BACKEND_RAM2 int BG_touch_count_multi, BG_touch_count_fine;
ATTR_BACKEND_RAM2 vec_t BG_touch_data[ALLOWABLE_TOUCH_BE];
ATTR_BACKEND_RAM2 float BG_touch_area[ALLOWABLE_TOUCH_BE];
ATTR_BACKEND_RAM2 tp_size_t BG_touch_size[ALLOWABLE_TOUCH_BE];
ATTR_BACKEND_RAM2 int8_t BG_multi_fine[ALLOWABLE_TOUCH_BE]; //for debugging
ATTR_BACKEND_RAM2 int8_t BG_touch_misc[ALLOWABLE_TOUCH_BE];
ATTR_BACKEND_RAM2 int8_t BS_touch_sort[ALLOWABLE_TOUCH_BE]; //nsmoon@211027
#ifdef HOR_EDGE_TOUCH_ENABLE
ATTR_BACKEND_RAM2 vec_t BG_touch_data_edge; //nsmoon@200415
#endif
#ifdef FRONTEND_LINE_THRESHOLD
ATTR_BACKEND_RAM2 tp_line_cnt_t BG_touch_lineCnt[ALLOWABLE_TOUCH_BE];
#endif
//#ifdef FRONTEND_LINE_THRESHOLD
//ATTR_BACKEND_RAM2 uint8_t BG_touch_threshold[ALLOWABLE_TOUCH_BE];
//#endif

#if 1 //nsmoon@190812
ATTR_BACKEND_RAM2 vec_t BS_minmax_inst_xy[MAX_MINMAX_INST_TBL];
ATTR_BACKEND_RAM2 uint16_t BS_minmax_inst_stat[MAX_MINMAX_INST_TBL];
ATTR_BACKEND_RAM2 uint16_t BS_int_sect_group2_epf_used[INT_SECT_GRP_EPF_USED_SIZE];
ATTR_BACKEND_RAM2 int BS_int_sect_group2_epf_used_cnt;
#endif

#ifdef USE_SENSOR_LIST //for test, nsmoon@200327
#ifdef USE_CUST_MALLOC //nsmoon@201012
ATTR_BACKEND_RAM2 uint8_t *BS_pd_x, *BS_led_x, *BS_pd_y, *BS_led_y;
#else
ATTR_BACKEND_RAM3 uint8_t BS_pd_x[X_NUM_PD];
ATTR_BACKEND_RAM3 uint8_t BS_led_x[X_NUM_PD];
ATTR_BACKEND_RAM3 uint8_t BS_pd_y[Y_NUM_PD];
ATTR_BACKEND_RAM3 uint8_t BS_led_y[Y_NUM_PD];
#endif
ATTR_BACKEND_RAM3 int BS_pd_x_cnt, BS_led_x_cnt, BS_pd_y_cnt, BS_led_y_cnt;
#endif
////////////////////////////////////////

///////////////////////////////////////////
//for debugging
//unsigned long debug_cnt;
ATTR_BACKEND_RAM2 int BG_debug_flag, BG_debug_flag2, BS_debug_nop;
ATTR_BACKEND_RAM2 int BG_show_ep;
ATTR_BACKEND_RAM2 int BG_show_line_x;
ATTR_BACKEND_RAM2 int BG_show_line_y;
ATTR_BACKEND_RAM2 int BG_show_line_init;
ATTR_BACKEND_RAM2 int BG_show_line_all;
ATTR_BACKEND_RAM2 int BG_show_line_th10;
ATTR_BACKEND_RAM2 int BG_show_line_rem;

ATTR_BACKEND_RAM2 int BG_show_debug;
ATTR_BACKEND_RAM2 int BG_show_debug_max;
ATTR_BACKEND_RAM2 unsigned int BG_frame_no;
ATTR_BACKEND_RAM2 int BG_debug_pause;

ATTR_BACKEND_RAM int BG_debug_misc[MAX_BG_DEBUG_MISC];
#if defined(DEBUG_FUNCTION_ENABLE_ALL) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
ATTR_BACKEND_RAM cal_slot_t BG_cal_slot_cur[MAX_GRID_CAL_SLOT];
ATTR_BACKEND_RAM cal_slot_t BG_cal_slot_org[MAX_GRID_CAL_SLOT];
ATTR_BACKEND_RAM uint8_t BG_cal_slot_buf[MAX_LINE_CAL_SLOT];
ATTR_BACKEND_RAM int BG_cal_slot_buf_idx;
ATTR_BACKEND_RAM int BG_cal_slot_cur_cnt, BG_cal_slot_org_cnt;
#endif

ATTR_BACKEND_RAM2 pos_min_max_t BS_initial_minMax[MAX_RESULT_POLYGON_NO]; //nsmoon@200402
#ifdef MULTI_SKIP_USED_VB //nsmoon@210218
ATTR_BACKEND_RAM2 initial_line_no_t BS_initial_lineNo[MAX_RESULT_POLYGON_NO];
#endif

#if (BRUSH_MODE_ENABLE == 1) //brush mode, nsmoon@200227
ATTR_BACKEND_RAM int BS_brushMode;
#endif

//#define HOVERING_TEST
#ifdef HOVERING_TEST //for testing
//ATTR_BACKEND_RAM static int zero_touch_cnt;
ATTR_BACKEND_RAM static int diffXmean;
ATTR_BACKEND_RAM static int diffYmean;
#endif //HOVERING_TEST

#ifdef FINE_RULE_1A_TEST
ATTR_BACKEND_RAM2 vec_t BG_debug_touch_data[ALLOWABLE_TOUCH_BE];
ATTR_BACKEND_RAM2 float BG_debug_touch_area[ALLOWABLE_TOUCH_BE];
ATTR_BACKEND_RAM2 tp_size_t BG_debug_touch_size[ALLOWABLE_TOUCH_BE];
ATTR_BACKEND_RAM2 int8_t BG_debug_multi_fine[ALLOWABLE_TOUCH_BE];
ATTR_BACKEND_RAM2 pos_minMax2_t BG_debug_touch_area_minmax[ALLOWABLE_TOUCH_BE];
#endif

//------------------------------------------------------------------
int BS_set_unset2_used_led_pd(axis_t axis, int led, int pd, int mode)
{
	//mode:1 used line
	uint8_t *lineUsed, lineUsedMask;
	int lineUsedIdx, maxInLineBufLed;

	if (axis == ENUM_HOR_X) {
        lineUsed = LINE_USED2_BUF_ADDR_X;
#ifdef USE_CUST_MALLOC //nsmoon@201012
        maxInLineBufLed = BS_max_in_line_buf_led_x; //nsmoon@201014
#else
        maxInLineBufLed = MAX_IN_LINE_BUF_LED_X;
#endif
	}
	else {
        lineUsed = LINE_USED2_BUF_ADDR_Y;
#ifdef USE_CUST_MALLOC //nsmoon@201012
        maxInLineBufLed = BS_max_in_line_buf_led_y; //nsmoon@201014
#else
        maxInLineBufLed = MAX_IN_LINE_BUF_LED_Y;
#endif
	}
    lineUsedIdx = GET_LINE_USED_IDX(pd, led);
    lineUsedMask = GET_LED_BIT_MASK(led);

	if (mode) {
		//used line
        lineUsed[lineUsedIdx] &= ~lineUsedMask;
	}
	else {
		//un-used line
        lineUsed[lineUsedIdx] |= lineUsedMask;
	}
	TRACE_NOP;
	return 0; //no error
}

ATTR_BACKEND_RAMFUNC
int BS_get_remained_line(axis_t axis, int multiLoopCnt)
{
	uint16_t *remLine;
	int cnt, inBufLen, inBufIdx;
	uint8_t *lineUsed2;
	uint8_t lineUsedMask;
	int lineUsedIdx, maxInLineBufLed;
	int pd, led;

	if (axis == ENUM_HOR_X) {
		//x-axis
		inBufLen = BS_inBuf->hor_len;
		remLine = &BS_remained_x[0];
		BS_remained_x_cnt = 0;
	}
	else {
		//y-axis
		inBufLen = BS_inBuf->ver_len;
		remLine = &BS_remained_y[0];
		BS_remained_y_cnt = 0;
	}

	if (axis == ENUM_HOR_X) {
		lineUsed2 = LINE_USED2_BUF_ADDR_X;
#ifdef USE_CUST_MALLOC //nsmoon@201012
        maxInLineBufLed = BS_max_in_line_buf_led_x; //nsmoon@201014
#else
        maxInLineBufLed = MAX_IN_LINE_BUF_LED_X;
#endif
	}
	else {
		lineUsed2 = LINE_USED2_BUF_ADDR_Y;
#ifdef USE_CUST_MALLOC //nsmoon@201012
        maxInLineBufLed = BS_max_in_line_buf_led_y; //nsmoon@201014
#else
        maxInLineBufLed = MAX_IN_LINE_BUF_LED_Y;
#endif
	}

#if (defined(DEBUG_FUNCTION_ENABLE_ALL) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)) && defined(DRAW_POLYGON_TEST)
    int color = MY_COLOR - multiLoopCnt;
    if (color <= MY_COLOR - 3) color--;
#endif
#if 0 //for test only
    cnt = 0;
    for (inBufIdx = 0; inBufIdx < inBufLen; inBufIdx++) {
        if (BS_getSenInBuf(axis, inBufIdx, &pd, &led)) {
            continue;
        }
        lineUsedIdx = GET_LINE_USED_IDX(pd, led);
        lineUsedMask = GET_LED_BIT_MASK(led);
        if ((lineUsed2[lineUsedIdx] & lineUsedMask) == 0x00) {
            continue;
        }
        cnt++;
    }
    if (cnt > MAX_REMAINED_LINE) {
        TRACE_ERROR("ERROR! remained_line_cnt= %d", cnt);
    }
#endif

	cnt = 0;
	for (inBufIdx = 0; inBufIdx < inBufLen; inBufIdx++) {
		if (BS_getSenInBuf(axis, inBufIdx, &pd, &led)) {
			continue;
		}
        //TRACE("BS_get_remained_line..axis,pd,led=%d,%d,%d", axis, pd, led);
		lineUsedIdx = GET_LINE_USED_IDX(pd, led);
		lineUsedMask = GET_LED_BIT_MASK(led);

#if (defined(DEBUG_FUNCTION_ENABLE_ALL) || defined(DEBUG_FUNCTION_ENABLE_ALL)) && defined(DRAW_POLYGON_TEST) //for test
        if (multiLoopCnt >= REM_LOOP_CNT_FINE5) {
            if (!BG_show_line_all && BG_show_line_rem) {
                if ((lineUsed2[lineUsedIdx] & lineUsedMask) != 0x00) {
                    DEBUG_SHOW_LINE_PD_LED(axis, pd, led, color);
                }
            }
        }
#elif defined(DEBUG_FUNCTION_ENABLE_RELEASE) && defined(DRAW_POLYGON_TEST)
        //if (multiLoopCnt >= REM_LOOP_CNT_FINE5) //nsmoon@210216
        {
            if (!BG_show_line_all && BG_show_line_rem) {
                if ((lineUsed2[lineUsedIdx] & lineUsedMask) == 0x00) {
                    //DEBUG_SHOW_LINE_PD_LED(axis, pd, led, color);
                }
                else {
                    DEBUG_SHOW_LINE_PD_LED(axis, pd, led, color);
                }
            }
        }
#endif
		if ((lineUsed2[lineUsedIdx] & lineUsedMask) == 0x00) {
			continue;
		}

		if (cnt < MAX_REMAINED_LINE) {
			remLine[cnt++] = (uint16_t)inBufIdx;
		}
		else {
			//TRACE_ERROR("ERROR! invalid MAX_REMAINED_LINE: (%d) %d (%d/%d)", axis, MAX_REMAINED_LINE, inBufLen, inBufIdx);
            TRACE_ERROR("*3:%d %d", axis, cnt);
#if 1 //(MODEL_SPT == MODEL_CTSK_850_V100) //nsmoon@200910
                        //break; //nsmoon@200313
			return -1; //mem-error
#else
            cnt = MAX_REMAINED_LINE;
            break; //nsmoon@200313
#endif
		}
		TRACE_NOP;
	} //for (inBufIdx = 0; inBufIdx < inBufLen; inBufIdx++)

	if (axis == ENUM_HOR_X) {
		BS_remained_x_cnt = cnt;
	}
	else {
		BS_remained_y_cnt = cnt;
	}
	IS_DEBUG_FLAG{
		if (cnt > 0) {
			TRACE_NOP;
		}
	};
    return cnt; //nsmoon@200401
}

#if (LARGE_PITCH_TBL == 1) //nsmoon@200108
#ifdef USE_CUST_MALLOC //nsmoon@201012
ATTR_BACKEND_RAM2 int BS_max_large_pitch_tbl_x, BS_max_large_pitch_tbl_y;
ATTR_BACKEND_RAM2 uint8_t *BS_large_pitch_tbl_x_pd; //[MAX_LARGE_PITCH_TBL_X]
ATTR_BACKEND_RAM2 uint8_t *BS_large_pitch_tbl_x_led; //[MAX_LARGE_PITCH_TBL_X]
ATTR_BACKEND_RAM2 uint8_t *BS_large_pitch_tbl_y_pd; //[MAX_LARGE_PITCH_TBL_Y]
ATTR_BACKEND_RAM2 uint8_t *BS_large_pitch_tbl_y_led; //[MAX_LARGE_PITCH_TBL_Y]
ATTR_BACKEND_RAM2 int BS_large_pitch_tbl_x_pd_cnt, BS_large_pitch_tbl_x_led_cnt;
ATTR_BACKEND_RAM2 int BS_large_pitch_tbl_y_pd_cnt, BS_large_pitch_tbl_y_led_cnt;
#else
#define MAX_LARGE_PITCH_TBL_X		((X_NUM_PD+7)/8)
#define MAX_LARGE_PITCH_TBL_Y		((Y_NUM_PD+7)/8)
ATTR_BACKEND_RAM3 uint8_t BS_large_pitch_tbl_x_pd[MAX_LARGE_PITCH_TBL_X];
ATTR_BACKEND_RAM3 uint8_t BS_large_pitch_tbl_x_led[MAX_LARGE_PITCH_TBL_X];
ATTR_BACKEND_RAM3 uint8_t BS_large_pitch_tbl_y_pd[MAX_LARGE_PITCH_TBL_Y];
ATTR_BACKEND_RAM3 uint8_t BS_large_pitch_tbl_y_led[MAX_LARGE_PITCH_TBL_Y];
ATTR_BACKEND_RAM3 int BS_large_pitch_tbl_x_pd_cnt, BS_large_pitch_tbl_x_led_cnt;
ATTR_BACKEND_RAM3 int BS_large_pitch_tbl_y_pd_cnt, BS_large_pitch_tbl_y_led_cnt;
#endif

#define MIN_LARGE_PITCH_X				10.5f //11.0f //12.0f
#define MIN_LARGE_PITCH_Y				9.5f //9.0f //10.0f //11.0f //14.0f
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_BS_large_pitch	1
#endif
#if (DEBUG_BS_large_pitch > 0)
#define TRACE_BILP(...)		TRACE(__VA_ARGS__)
#define TRACENR_BILP(...)	TRACENR(__VA_ARGS__)
#else
#define TRACE_BILP(...)
#define TRACENR_BILP(...)
#endif
int BS_is_large_pitch(axis_t axis, sen_type_t sen, int senNo)
{
	uint8_t *large_pitch_tbl, large_pitch_val;
	int byteIdx, bitIdx;
	if (axis == ENUM_HOR_X) {
		if (sen == ENUM_PD) {
			large_pitch_tbl = &BS_large_pitch_tbl_x_pd[0];
		}
		else {
			large_pitch_tbl = &BS_large_pitch_tbl_x_led[0];
		}
	}
	else {
		if (sen == ENUM_PD) {
			large_pitch_tbl = &BS_large_pitch_tbl_y_pd[0];
		}
		else {
			large_pitch_tbl = &BS_large_pitch_tbl_y_led[0];
		}
	}
	byteIdx = senNo / 8;
	bitIdx = senNo % 8;
	//TRACE_BILP("large_pitch_tbl:(%d/%d-%d/%d)%x ", axis, sen, byteIdx, bitIdx, large_pitch_tbl[byteIdx]);
	large_pitch_val = large_pitch_tbl[byteIdx] & (0x1 << bitIdx);
	return (int)large_pitch_val;
}

static int BS_make_large_pitch_tbl(axis_t axis, sen_type_t sen, float *posTbl)
{
	uint8_t *large_pitch_tbl;
	int byteIdx, bitIdx;
	int senNo, maxPdNum;
	float diffPos, pitchMin, pitchMax;
        float minLargePitch;
	int largePitchCnt, maxLargePitchCnt;

	if (axis == ENUM_HOR_X) {
		maxPdNum = BS_maxHorPdNum - 1;
#ifdef USE_CUST_MALLOC //nsmoon@201012
        maxLargePitchCnt = BS_max_large_pitch_tbl_x; //nsmoon@201014
#else
        maxLargePitchCnt = MAX_LARGE_PITCH_TBL_X;
#endif
		minLargePitch = MIN_LARGE_PITCH_X;
		if (sen == ENUM_PD) {
			large_pitch_tbl = &BS_large_pitch_tbl_x_pd[0];
		}
		else {
			large_pitch_tbl = &BS_large_pitch_tbl_x_led[0];
		}
	}
	else {
		maxPdNum = BS_maxVerPdNum - 1;
#ifdef USE_CUST_MALLOC //nsmoon@201012
        maxLargePitchCnt = BS_max_large_pitch_tbl_y; //nsmoon@201014
#else
        maxLargePitchCnt = MAX_LARGE_PITCH_TBL_Y;
#endif
		minLargePitch = MIN_LARGE_PITCH_Y;
		if (sen == ENUM_PD) {
			large_pitch_tbl = &BS_large_pitch_tbl_y_pd[0];
		}
		else {
			large_pitch_tbl = &BS_large_pitch_tbl_y_led[0];
		}
	}

	for (byteIdx = 0; byteIdx < maxLargePitchCnt; byteIdx++) {
		large_pitch_tbl[byteIdx] = 0; //init
	}

	largePitchCnt = 0;
	pitchMin = MIN_INITIAL_VAL;
	pitchMax = 0;
	for (senNo = 0; senNo < maxPdNum; senNo++) {
		//exclude last sensor
		diffPos = posTbl[senNo + 1] - posTbl[senNo];
		pitchMin = GET_MIN(pitchMin, diffPos);
		pitchMax = GET_MAX(pitchMax, diffPos);
		if ((diffPos - EPSILON) > minLargePitch) {
			byteIdx = senNo / 8;
			bitIdx = senNo % 8;
			if (byteIdx < maxLargePitchCnt) {
				large_pitch_tbl[byteIdx] |= (0x1 << bitIdx);
				largePitchCnt++;
#if (DEBUG_BS_large_pitch > 1)
				TRACENR_BILP("%0.2f ", diffPos);
#endif
			}
			else {
				TRACE_ERROR("ERROR! invalid byteIdx: %d, %d", byteIdx, maxLargePitchCnt);
			}
		}
	}
	TRACE_BILP("largePitchCnt=(%d-%d)%d %0.2f/%0.2f %x", axis, sen, largePitchCnt, pitchMin, pitchMax, large_pitch_tbl);
	if (axis == ENUM_HOR_X) {
		if (sen == ENUM_PD) {
			BS_large_pitch_tbl_x_pd_cnt = largePitchCnt;
		}
		else {
			BS_large_pitch_tbl_x_led_cnt = largePitchCnt;
		}
	}
	else {
		if (sen == ENUM_PD) {
			BS_large_pitch_tbl_y_pd_cnt = largePitchCnt;
		}
		else {
			BS_large_pitch_tbl_y_led_cnt = largePitchCnt;
		}
	}

#if (DEBUG_BS_large_pitch > 1)
	if (largePitchCnt > 0) {
		for (byteIdx = 0; byteIdx < maxLargePitchCnt; byteIdx++) {
			TRACENR_BILP("%x ", large_pitch_tbl[byteIdx]);
		}
		TRACE_BILP(".");
	}
#endif
	return largePitchCnt;
}
#endif

#ifdef USE_CUST_MALLOC //nsmoon@201012
ATTR_BACKEND_RAMFUNC
static int BS_main_malloc(void)
{
    BS_max_ep_count = (BS_max_mum_pd * ALLOWABLE_TOUCH_BE * 2);
    ////////////////////////////////////////////////
    /// ep_no_t *BS_edge_pattern_ep
    ////////////////////////////////////////////////
    BS_edge_pattern_ep = (ep_no_t *)BS_malloc(sizeof(ep_no_t) * BS_max_ep_count);
    if (BS_edge_pattern_ep == 0) {
        TRACE_ERROR("ERROR! BS_main_malloc..BS_edge_pattern_ep");
        return 1; //error
    }
    TRACE_MALLOC("BS_edge_pattern_ep=,%x,%x,%x,", &BS_edge_pattern_ep[0], sizeof(ep_no_t) * BS_max_ep_count, BS_end_of_heap);

    ////////////////////////////////////////////////
    /// uint8_t *BS_edge_pattern_deadzone
    ////////////////////////////////////////////////
    BS_edge_pattern_deadzone = (uint8_t *)BS_malloc(sizeof(uint8_t) * BS_max_ep_count);
    if (BS_edge_pattern_deadzone == 0) {
        TRACE_ERROR("ERROR! BS_main_malloc..BS_edge_pattern_deadzone");
        return 1; //error
    }
    TRACE_MALLOC("BS_edge_pattern_deadzone=,%x,%x,%x,", &BS_edge_pattern_deadzone[0], sizeof(uint8_t) * BS_max_ep_count, BS_end_of_heap);

#ifdef USE_SENSOR_LIST //for test, nsmoon@201015
    ////////////////////////////////////////////////
    /// uint8_t *BS_pd_x
    ////////////////////////////////////////////////
    BS_pd_x = (uint8_t *)BS_malloc(sizeof(uint8_t) * BS_maxHorPdNum);
    if (BS_pd_x == 0) {
        TRACE_ERROR("ERROR! BS_main_malloc..BS_pd_x");
        return 1; //error
    }
    TRACE_MALLOC("BS_pd_x=,%x,%x,%x,", &BS_pd_x[0], sizeof(uint8_t) * BS_maxHorPdNum, BS_end_of_heap);

    ////////////////////////////////////////////////
    /// uint8_t *BS_led_x
    ////////////////////////////////////////////////
    BS_led_x = (uint8_t *)BS_malloc(sizeof(uint8_t) * BS_maxHorPdNum);
    if (BS_led_x == 0) {
        TRACE_ERROR("ERROR! BS_main_malloc..BS_led_x");
        return 1; //error
    }
    TRACE_MALLOC("BS_led_x=,%x,%x,%x,", &BS_led_x[0], sizeof(uint8_t) * BS_maxHorPdNum, BS_end_of_heap);

    ////////////////////////////////////////////////
    /// uint8_t *BS_pd_y
    ////////////////////////////////////////////////
    BS_pd_y = (uint8_t *)BS_malloc(sizeof(uint8_t) * BS_maxVerPdNum);
    if (BS_pd_y == 0) {
        TRACE_ERROR("ERROR! BS_main_malloc..BS_pd_y");
        return 1; //error
    }
    TRACE_MALLOC("BS_pd_y=,%x,%x,%x,", &BS_pd_y[0], sizeof(uint8_t) * BS_maxVerPdNum, BS_end_of_heap);

    ////////////////////////////////////////////////
    /// uint8_t *BS_led_y
    ////////////////////////////////////////////////
    BS_led_y = (uint8_t *)BS_malloc(sizeof(uint8_t) * BS_maxVerPdNum);
    if (BS_led_y == 0) {
        TRACE_ERROR("ERROR! BS_main_malloc..BS_led_y");
        return 1; //error
    }
    TRACE_MALLOC("BS_led_y=,%x,%x,%x,", &BS_led_y[0], sizeof(uint8_t) * BS_maxVerPdNum, BS_end_of_heap);
#endif

#if (LARGE_PITCH_TBL == 1) //nsmoon@200108
    BS_max_large_pitch_tbl_x = (BS_maxHorPdNum + 7) / 8;
    BS_max_large_pitch_tbl_y = (BS_maxVerPdNum + 7) / 8;

    ///////////////////////////////////////////////////////////////////
    /// uint8_t *BS_large_pitch_tbl_x_pd; //[MAX_LARGE_PITCH_TBL_X]
    ///////////////////////////////////////////////////////////////////
    BS_large_pitch_tbl_x_pd = (uint8_t *)BS_malloc(sizeof(uint8_t) * BS_max_large_pitch_tbl_x);
    if (BS_large_pitch_tbl_x_pd == 0) {
        TRACE_ERROR("ERROR! BS_main_malloc..BS_large_pitch_tbl_x_pd");
        return 1; //error
    }
    TRACE_MALLOC("BS_large_pitch_tbl_x_pd=,%x,%x,%x,", &BS_large_pitch_tbl_x_pd[0], sizeof(uint8_t) * BS_max_large_pitch_tbl_x, BS_end_of_heap);

    ///////////////////////////////////////////////////////////////////
    /// uint8_t *BS_large_pitch_tbl_x_led; //[MAX_LARGE_PITCH_TBL_X]
    ///////////////////////////////////////////////////////////////////
    BS_large_pitch_tbl_x_led = (uint8_t *)BS_malloc(sizeof(uint8_t) * BS_max_large_pitch_tbl_x);
    if (BS_large_pitch_tbl_x_led == 0) {
        TRACE_ERROR("ERROR! BS_main_malloc..BS_large_pitch_tbl_x_led");
        return 1; //error
    }
    TRACE_MALLOC("BS_large_pitch_tbl_x_led=,%x,%x,%x,", &BS_large_pitch_tbl_x_led[0], sizeof(uint8_t) * BS_max_large_pitch_tbl_x, BS_end_of_heap);

    ///////////////////////////////////////////////////////////////////
    /// uint8_t *BS_large_pitch_tbl_y_pd; //[MAX_LARGE_PITCH_TBL_Y]
    ///////////////////////////////////////////////////////////////////
    BS_large_pitch_tbl_y_pd = (uint8_t *)BS_malloc(sizeof(uint8_t) * BS_max_large_pitch_tbl_y);
    if (BS_large_pitch_tbl_y_pd == 0) {
        TRACE_ERROR("ERROR! BS_main_malloc..BS_large_pitch_tbl_y_pd");
        return 1; //error
    }
    TRACE_MALLOC("BS_large_pitch_tbl_y_pd=,%x,%x,%x,", &BS_large_pitch_tbl_y_pd[0], sizeof(uint8_t) * BS_max_large_pitch_tbl_y, BS_end_of_heap);

    ///////////////////////////////////////////////////////////////////
    /// uint8_t *BS_large_pitch_tbl_y_led; //[MAX_LARGE_PITCH_TBL_Y]
    ///////////////////////////////////////////////////////////////////
    BS_large_pitch_tbl_y_led = (uint8_t *)BS_malloc(sizeof(uint8_t) * BS_max_large_pitch_tbl_y);
    if (BS_large_pitch_tbl_y_led == 0) {
        TRACE_ERROR("ERROR! BS_main_malloc..BS_large_pitch_tbl_y_led");
        return 1; //error
    }
    TRACE_MALLOC("BS_large_pitch_tbl_y_led=,%x,%x,%x,", &BS_large_pitch_tbl_y_led[0], sizeof(uint8_t) * BS_max_large_pitch_tbl_y, BS_end_of_heap);
#endif

    return 0; //no-error
}
#endif

///////////////////////////////////////////
#if 1 //nsmoon@190812
ATTR_BACKEND_RAMFUNC
void BS_add_epf_used(uint16_t epfNo)
{
    int cnt = BS_int_sect_group2_epf_used_cnt;
    uint16_t *used = &BS_int_sect_group2_epf_used[0];
    if (cnt < INT_SECT_GRP_EPF_USED_SIZE) {
        used[cnt] = epfNo;
        cnt++;
    }
    else {
        TRACE_ERROR_MEM("ERROR_MEM! invalid int_set_group2_epf_used_cnt %d", BS_int_sect_group2_epf_used_cnt);
    }
    BS_int_sect_group2_epf_used_cnt = cnt;
}

ATTR_BACKEND_RAMFUNC
int BS_is_epf_used(uint16_t epfNo)
{
    int i;
    int cnt = BS_int_sect_group2_epf_used_cnt;
    uint16_t *used = &BS_int_sect_group2_epf_used[0];

    for (i = 0; i < cnt; i++) {
        if (epfNo == used[i]) {
            return 1; //used
        }
    }
    return 0; //not-used
}
#endif

#ifdef FRONTEND_LINE_THRESHOLD
int BS_is_set_threshold(axis_t axis, int pd, int ofstIdx)
{
	uint8_t *threshold, ret;
	int idx, byteIdx, bitIdx, ofst_byte;
	//uint16_t thresholdCnt;

	if (axis == ENUM_HOR_X) {
		threshold = &BS_inBuf->threshold_x[0];
		//thresholdCnt = BS_inBuf->threshold_x_cnt;
#ifdef USE_CUST_MALLOC //nsmoon@201012
        ofst_byte = ((BS_offsetTblLenX + 7) / 8);
#else
		ofst_byte = MAX_SLOPE_BYTE_X;
#endif
	}
	else {
		threshold = &BS_inBuf->threshold_y[0];
		//thresholdCnt = BS_inBuf->threshold_y_cnt;
#ifdef USE_CUST_MALLOC //nsmoon@201012
        ofst_byte = ((BS_offsetTblLenY + 7) / 8);
#else
		ofst_byte = MAX_SLOPE_BYTE_Y;
#endif
	}
	byteIdx = ofstIdx / 8;
	bitIdx = ofstIdx % 8;
	idx = ((pd * ofst_byte) + byteIdx);
	ret = threshold[idx] & (uint8_t)(0x1 << bitIdx);

#if 0 //for test
	int i, j, tmpIdx, found;
	int pdLen = (axis == ENUM_HOR_X) ? BS_maxHorPdNum : BS_maxVerPdNum;
	for (i = 0; i < pdLen; i++) {
		found = 0;
        for (j = 0; j < ofst_byte; j++) {
        	tmpIdx = (i * ofst_byte) + j;
        	if (threshold[tmpIdx] != 0) {
        		TRACE("%02x ", threshold[tmpIdx]);
        		found++;
        	}
        	if (found) {
        		if (i == pd) {
        			TRACE("found: %d, %d, %d", pd, tmpIdx, idx);
        			break;
        		}
        	}
        }
	}
#endif
	return (int)ret;
}
#endif

#if (SECOND_OFST_TBL == 1) //nsmoon@190911
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_init_ofst_table	1
#endif
int BS_is_near_2nd_ofst_led(axis_t axis, int led)
{
	int i, senTblLen;
	uint8_t *senTbl;

	if (axis == ENUM_HOR_X) {
		senTbl = BS_senTblX2;
		senTblLen = BS_senTblLenX2;
	}
	else {
		senTbl = BS_senTblY2;
		senTblLen = BS_senTblLenY2;
	}

	for (i = 0; i < senTblLen; i++) {
		if ((led - 1) == (int)senTbl[i]) {
			return (int)senTbl[i]; //right
		}
		if ((led + 1) == (int)senTbl[i]) {
			return (int)senTbl[i]; //left
		}
	}

	return -1; //not-found
}
#endif

#if (DEAD_CELL_TBL == 1) //nsmoon@191202
int BS_is_near_dead(axis_t axis, sen_type_t sen, int senNo, int mode)
{
    int i, deadTblLen, deadSen;
    uint8_t *deadTbl;

    if (axis == ENUM_HOR_X) {
		if (sen == ENUM_LED) {
        deadTbl = BS_inValidTblXled;
        deadTblLen = BS_inValidTblLenXled;
    }
    else {
			deadTbl = BS_inValidTblXpd;
			deadTblLen = BS_inValidTblLenXpd;
		}
    }
    else {
		if (sen == ENUM_LED) {
        deadTbl = BS_inValidTblYled;
        deadTblLen = BS_inValidTblLenYled;
    }
		else {
			deadTbl = BS_inValidTblYpd;
			deadTblLen = BS_inValidTblLenYpd;
		}
    }

    for (i = 0; i < deadTblLen; i++) {
		deadSen = (int)deadTbl[i];
        if ((senNo - 1) == deadSen) {
            return deadSen; //right
        }
        if ((senNo + 1) == deadSen) {
            return deadSen; //left
        }
		//include itself
		if (mode && (senNo == deadSen)) {
			return deadSen;
        }
    }

    return -1; //not-found
}

int BS_is_dead_cell(axis_t axis, sen_type_t sen, int senNo)
{
	int i, deadTblLen, deadSen;
	uint8_t *deadTbl;

	if (axis == ENUM_HOR_X) {
		if (sen == ENUM_LED) {
			deadTbl = BS_inValidTblXled;
			deadTblLen = BS_inValidTblLenXled;
		}
		else {
			deadTbl = BS_inValidTblXpd;
			deadTblLen = BS_inValidTblLenXpd;
		}
	}
	else {
		if (sen == ENUM_LED) {
			deadTbl = BS_inValidTblYled;
			deadTblLen = BS_inValidTblLenYled;
		}
		else {
			deadTbl = BS_inValidTblYpd;
			deadTblLen = BS_inValidTblLenYpd;
		}
	}

	for (i = 0; i < deadTblLen; i++) {
		deadSen = (int)deadTbl[i];
		if (senNo == deadSen) {
			return 1; //found
		}
	}

	return 0; //not-found
}

#define TOL_DEAD_CELL_POS	4.0f
int BS_is_dead_cell_pos(axis_t axis, sen_type_t sen, float *pos)
{
	int i, deadTblLen, deadSen;
	uint8_t *deadTbl;
	float senPos, *sen_pos;

	if (axis == ENUM_HOR_X) {
		if (sen == ENUM_LED) {
			deadTbl = BS_inValidTblXled;
			deadTblLen = BS_inValidTblLenXled;
			sen_pos = &BS_led_pos_x[0];
		}
		else {
			deadTbl = BS_inValidTblXpd;
			deadTblLen = BS_inValidTblLenXpd;
			sen_pos = &BS_pd_pos_x[0];
		}
	}
	else {
		if (sen == ENUM_LED) {
			deadTbl = BS_inValidTblYled;
			deadTblLen = BS_inValidTblLenYled;
			sen_pos = &BS_led_pos_y[0];
		}
		else {
			deadTbl = BS_inValidTblYpd;
			deadTblLen = BS_inValidTblLenYpd;
			sen_pos = &BS_pd_pos_y[0];
		}
	}

	for (i = 0; i < deadTblLen; i++) {
		deadSen = (int)deadTbl[i];
		senPos = sen_pos[deadSen];
		if (*pos <= senPos + TOL_DEAD_CELL_POS &&
			*pos >= senPos - TOL_DEAD_CELL_POS) {
			return 1; //found
		}
	}

	return 0; //not-found
}
#endif

#if 1 //nsmoon@190911
#define MAX_SLOPE_VAL     ((INT8_MAX - 1) / 2)
static int init_ofst_table()
{
	int i;
    if (BS_slopeValMaxX > MAX_SLOPE_VAL) {
        TRACE_ERROR("ERROR! init_ofst_table..invalid BS_slopeValMaxX %x/%X", BS_slopeValMaxX, MAX_SLOPE_VAL);
        BS_slopeValMaxX = MAX_SLOPE_VAL;
    }
    if (BS_slopeValMaxY > MAX_SLOPE_VAL) {
        TRACE_ERROR("ERROR! init_ofst_table..invalid BS_slopeValMaxY %x/%X", BS_slopeValMaxY, MAX_SLOPE_VAL);
        BS_slopeValMaxY = MAX_SLOPE_VAL;
    }

	BS_offsetTblLenX = (BS_slopeValMaxX * 2) + 1;
	BS_offsetTblLenY = (BS_slopeValMaxY * 2) + 1;

#ifdef USE_CUST_MALLOC //nsmoon@201012
    ////////////////////////////////////////////////
    /// int8_t *BS_offsetTblX
    ////////////////////////////////////////////////
    BS_offsetTblX = (int8_t *)BS_malloc(sizeof(int8_t) * BS_offsetTblLenX);
    if (BS_offsetTblX == 0) {
        TRACE_ERROR("ERROR! init_ofst_table..BS_offsetTblX");
        return 1; //error
    }
    TRACE_MALLOC("BS_offsetTblX=,%x,%x,%x,", BS_offsetTblX, sizeof(int8_t) * BS_offsetTblLenX, BS_end_of_heap);

    ////////////////////////////////////////////////
    /// int8_t *BS_offsetTblY
    ////////////////////////////////////////////////
    BS_offsetTblY = (int8_t *)BS_malloc(sizeof(int8_t) * BS_offsetTblLenY);
    if (BS_offsetTblY == 0) {
        TRACE_ERROR("ERROR! init_ofst_table..BS_offsetTblY");
        return 1; //error
    }
    TRACE_MALLOC("BS_offsetTblY=,%x,%x,%x,", BS_offsetTblY, sizeof(int8_t) * BS_offsetTblLenY, BS_end_of_heap);
#endif

	//1st offset table
	for (i = 0; i < BS_offsetTblLenX; i++)
	{
		BS_offsetTblX[i] = (int8_t)(i - BS_slopeValMaxX);
#if (DEBUG_init_ofst_table > 0)
		TRACENR("%d ", BS_offsetTblX[i]);
#endif
	}
#if (DEBUG_init_ofst_table > 0)
	TRACE("=>BS_offsetTblLenX: %d", BS_offsetTblLenX);
#endif
	for (i = 0; i < BS_offsetTblLenY; i++)
{
		BS_offsetTblY[i] = (int8_t)(i - BS_slopeValMaxY);
#if (DEBUG_init_ofst_table > 0)
		TRACENR("%d ", BS_offsetTblY[i]);
#endif
	}
#if (DEBUG_init_ofst_table > 0)
	TRACE("=>BS_offsetTblLenY: %d", BS_offsetTblLenY);
#endif
	return 0;
	}

#ifdef USE_CUST_MALLOC //nsmoon@201012
//#define ARRY_TEST_2D //nsmoon@201015
#ifdef ARRY_TEST_2D //for test
static int arry_2d_test()
{
    int r=3, c=4, len=0;
    uint8_t *ptr, **arr, *ptr2;
    int cnt, i, j;

    len = (sizeof(int *) * r) + (sizeof(int) * c * r);
    arr = (uint8_t **)BS_malloc(len);

    // ptr is now pointing to the first element in of 2D array
    ptr = (uint8_t *)(arr + r);

    // for loop to point rows pointer to appropriate location in 2D array
    for(i = 0; i < r; i++) {
        arr[i] = (ptr + c * i);
    }

    cnt = 0;
    for (i = 0; i < r; i++) {
        cnt = i * 10;
        for (j = 0; j < c; j++) {
            arr[i][j] = cnt + j;
        }
    }

    for (i = 0; i < r; i++) {
        TRACE("i= %d %x", i, &arr[i][0]);
        for (j = 0; j < c; j++) {
            TRACE(" %d ", arr[i][j]);
        }
    }
    TRACE("!!!arr= %x %x %x %x %x %x", arr, &arr[0], &arr[0][0], ptr, *arr[0], *arr[1]);

    ptr2 = (uint8_t *)&arr[0][0];
    for (i = 0; i < r; i++) {
        for (j = 0; j < c; j++) {
            int ptr2_idx = (i * c) + j;
            if (ptr2[ptr2_idx] != arr[i][j]) {
                TRACE("ptr2[%d]=%d,arr[%d][%d]=%d", ptr2_idx, ptr2[ptr2_idx], i, j, arr[i][j]);
            }
        }
    }

#if 0
    memset(&arr[0][0], 5, sizeof(uint8_t)*(r*c));
    for (i = 0; i < r; i++) {
        TRACE("i= %d %x", i, &arr[i][0]);
        for (j = 0; j < c; j++) {
            TRACE(" %d ", arr[i][j]);
        }
    }
#endif
    return 0;
}
#endif
#endif

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_INIT_BACKEND		1
#endif
#if (DEBUG_INIT_BACKEND > 0)
#define TRACE_BIB	TRACE
#else
#define TRACE_BIB(...)
#endif
#if (DEAD_CELL_TBL == 1) //nsmoon@190829
ATTR_BACKEND_RAMFUNC
int BG_init_backend(
    DEF_TP_LAYOUT_INFO *tpLayout, 	//sensor laypout
    DEF_DEADCELL_INFO  *deadCellTbl, //dead cell table
    uint16_t  init_flag          // reserved: 0
) {
#else
ATTR_BACKEND_RAMFUNC
int BG_init_backend(
	DEF_TP_LAYOUT_INFO *tpLayout, 	//sensor laypout
	DEF_PD_INFO  *ValidTbl,			//not used yet
	uint16_t  init_flag          // reserved: 0
) {
#endif
#ifdef USE_CUST_MALLOC //nsmoon@201012
    BS_malloc_init(); //nsmoon@201021
#ifdef ARRY_TEST_2D //for test
    arry_2d_test();
#endif
#endif

	//num of pd(led)
	BS_maxHorPdNum = tpLayout->hor_pd_num;
	BS_maxVerPdNum = tpLayout->ver_pd_num;
    if (BS_maxHorPdNum < 10 || BS_maxHorPdNum > UINT8_MAX || BS_maxVerPdNum < 10 || BS_maxVerPdNum > UINT8_MAX) {
        TRACE_ERROR("ERROR! invlaid BS_maxHorPdNum,BS_maxVerPdNum %d %d", BS_maxHorPdNum, BS_maxVerPdNum);
        return NOT_INITIALIZED;
    }
#ifdef USE_CUST_MALLOC //nsmoon@201012
    BS_max_mum_pd = BS_maxHorPdNum + BS_maxVerPdNum;
#else
#if (DEBUG_INIT_BACKEND > 0) // for testing
    if (BS_maxHorPdNum != MODEL_SPT_X_CELL || BS_maxVerPdNum != MODEL_SPT_Y_CELL) {
        TRACE_ERROR("ERROR! invalid pd,led number: %d %d", BS_maxHorPdNum, BS_maxVerPdNum);
    }
#endif
#endif

    //logical size
	BS_logical_max_x = tpLayout->logical_x_size;
	BS_logical_max_y = tpLayout->logical_y_size;

	//max offset value
	BS_slopeValMaxX = tpLayout->maxOfstValX;
	BS_slopeValMaxY = tpLayout->maxOfstValY;
#if (SECOND_OFST_TBL == 1) //nsmoon@190625
	BS_slopeValMaxX2 = tpLayout->maxOfstValX2;
	BS_slopeValMaxY2 = tpLayout->maxOfstValY2;
#endif
#ifndef USE_CUST_MALLOC //nsmoon@201012
	if (BS_maxHorPdNum != X_NUM_PD || BS_maxVerPdNum != Y_NUM_PD) {
		TRACE_ERROR("ERROR! BS_maxHorPdNum,BS_maxVerPdNum= %d %d", BS_maxHorPdNum, BS_maxVerPdNum);
		return NOT_INITIALIZED;
	}
#endif
#ifndef USE_CUST_MALLOC //nsmoon@201012
	if (BS_slopeValMaxX != MAX_X_SLOPE_VAL || BS_slopeValMaxY != MAX_Y_SLOPE_VAL) {
        TRACE_ERROR("ERROR! BS_slopeValMaxX,BS_slopeValMaxY= %d(%d) %d(%d)", BS_slopeValMaxX, MAX_X_SLOPE_VAL, BS_slopeValMaxY, MAX_Y_SLOPE_VAL);
		return NOT_INITIALIZED;
	}
#endif

	//ofst table
	if (init_ofst_table()) {
		TRACE_ERROR("ERROR! init_ofst_table()...");
		return NOT_INITIALIZED;
	}

#if (SECOND_OFST_TBL == 1) //nsmoon@190625
	//sensor table for 2nd offset
	BS_senTblX2 = tpLayout->senTblX2.tbl;
	BS_senTblY2 = tpLayout->senTblY2.tbl;
	BS_senTblLenX2 = tpLayout->senTblX2.len;
	BS_senTblLenY2 = tpLayout->senTblY2.len;
#endif

#if (USE_VALID_OFST_TBL == 1)  //nsmoon@191129
	//valid offset table
	BS_valOfstTblX = tpLayout->validOfstX;
	BS_valOfstTblY = tpLayout->validOfstY;
#endif

#if (DEAD_CELL_TBL == 1) //nsmoon@190829
    if (deadCellTbl > (DEF_DEADCELL_INFO  *)0) {
        BS_inValidTblXpd = deadCellTbl->hor_dead_pd;
        BS_inValidTblXled = deadCellTbl->hor_dead_led;
        BS_inValidTblYpd = deadCellTbl->ver_dead_pd;
        BS_inValidTblYled = deadCellTbl->ver_dead_led;
        BS_inValidTblLenXpd = deadCellTbl->hor_pd_len;
        BS_inValidTblLenXled = deadCellTbl->hor_led_len;
        BS_inValidTblLenYpd = deadCellTbl->ver_pd_len;
        BS_inValidTblLenYled = deadCellTbl->ver_led_len;
	}
	else {
	    BS_inValidTblLenXpd = 0;
	    BS_inValidTblLenXled = 0;
	    BS_inValidTblLenYpd = 0;
	    BS_inValidTblLenYled = 0;
	}
#endif

#if (DEBUG_INIT_BACKEND > 0) // for testing
    int i;
	TRACE_BIB("BS_maxHorPdNum,BS_maxVePdNum:%d,%d", BS_maxHorPdNum, BS_maxVerPdNum);
	TRACE_BIB("BS_slopeValMaxX,BS_slopeValMaxY: %d,%d (%d,%d)", BS_slopeValMaxX, BS_slopeValMaxY, BS_slopeValMaxX2, BS_slopeValMaxY2);
	TRACE_BIB("BS_offsetTblLenX,BS_offsetTblLenY: %d %d", BS_offsetTblLenX, BS_offsetTblLenY);
	//TRACE("BS_offsetTblX(len,tbl):(%d,%x)", BS_offsetTblLenX, BS_offsetTblX);
	//TRACE("BS_offsetTblY(len,tbl):(%d,%x)", BS_offsetTblLenY, BS_offsetTblY);
#if (SECOND_OFST_TBL == 1) //nsmoon@190625
	for (i = 0; i < BS_senTblLenX2; i++) {
		TRACENR("%d ", BS_senTblX2[i]);
    }
	TRACE_BIB("=>BS_senTblLenX2: %d", BS_senTblLenX2);
	for (i = 0; i < BS_senTblLenY2; i++) {
		TRACENR("%d ", BS_senTblY2[i]);
    }
	TRACE_BIB("=>BS_senTblLenY2: %d", BS_senTblLenY2);
#endif
#if (DEAD_CELL_TBL == 1) //nsmoon@190829
    TRACE("BS_inValidTblLenXpd,BS_inValidTblLenXled: %d %d", BS_inValidTblLenXpd, BS_inValidTblLenXled);
    for (i = 0; i < BS_inValidTblLenXpd; i++) {
        TRACENR("%d ", BS_inValidTblXpd[i]);
    }
    TRACE("=>BS_inValidTblLenXpd: %d ", BS_inValidTblLenXpd);
    for (i = 0; i < BS_inValidTblLenXled; i++) {
        TRACENR("%d ", BS_inValidTblXled[i]);
    }
    TRACE("=>BS_inValidTblLenXled: %d ", BS_inValidTblLenXled);
    TRACE("BS_inValidTblLenYpd,BS_inValidTblLenYled: %d %d", BS_inValidTblLenYpd, BS_inValidTblLenYled);
    for (i = 0; i < BS_inValidTblLenYpd; i++) {
        TRACENR("%d ", BS_inValidTblYpd[i]);
    }
    TRACE("=>BS_inValidTblLenYpd: %d ", BS_inValidTblLenYpd);
    for (i = 0; i < BS_inValidTblLenYled; i++) {
        TRACENR("%d ", BS_inValidTblYled[i]);
    }
    TRACE("=>BS_inValidTblLenYled: %d ", BS_inValidTblLenYled);
#endif
#endif
#ifndef USE_CUST_MALLOC //nsmoon@201012
	if (BS_offsetTblLenX > MAX_NUM_X_SLOPES || BS_offsetTblLenY > MAX_NUM_Y_SLOPES) {
		TRACE_ERROR("ERROR! invalid BS_offsetTblLenX or BS_offsetTblLenY: %d %d", BS_offsetTblLenX, BS_offsetTblLenY);
		return NOT_INITIALIZED;
    }
#endif

	//sensor position
	BS_sensor_zero_x = tpLayout->sensor_zero_x;
	BS_sensor_end_x = tpLayout->sensor_end_x;
	BS_sensor_zero_y = tpLayout->sensor_zero_y;
	BS_sensor_end_y = tpLayout->sensor_end_y;
	BS_aarea_zero_x = tpLayout->aarea_zero_x;
	BS_aarea_end_x = tpLayout->aarea_end_x;
	BS_aarea_zero_y = tpLayout->aarea_zero_y;
	BS_aarea_end_y = tpLayout->aarea_end_y;
	BS_pd_pos_x = tpLayout->pd_pos_x;
	BS_pd_pos_y = tpLayout->pd_pos_y;
	BS_led_pos_x = tpLayout->led_pos_x;
	BS_led_pos_y = tpLayout->led_pos_y;
	BS_half_end_x = BS_sensor_end_x * 0.5f;
	BS_half_end_y = BS_sensor_end_y * 0.5f;
#if (LARGE_PITCH_TBL == 1) //nsmoon@200108
	float tmp_width_x = (BS_aarea_end_x - BS_aarea_zero_x) * 0.25f;
	float tmp_width_y = (BS_aarea_end_y - BS_aarea_zero_y) * 0.25f;
	BS_large_pitch_zero_x = BS_aarea_zero_x + tmp_width_x;
	BS_large_pitch_end_x = BS_aarea_end_x - tmp_width_x;
	BS_large_pitch_zero_y = BS_aarea_zero_y + tmp_width_y;
	BS_large_pitch_end_y = BS_aarea_end_y - tmp_width_y;
#endif
#if 0 //for test
	for (i = 0; i < BS_maxHorPdNum; i++) {
		TRACE("%d,%f ", i, BS_pd_pos_x[i]);
		//if ((i + 1) % 10 == 0) TRACENR("\r\n");
	}
	TRACE("=>BS_pd_pos_x=%d", BS_maxHorPdNum);
	for (i = 0; i < BS_maxHorPdNum; i++) {
		TRACE("%d,%f ", i, BS_led_pos_x[i]);
		//if ((i + 1) % 10 == 0) TRACENR("\r\n");
	}
	TRACE("=>BS_led_pos_x=%d", BS_maxHorPdNum);

	for (i = 0; i < BS_maxVerPdNum; i++) {
		TRACE("%d,%f ", i, BS_pd_pos_y[i]);
		//if ((i + 1) % 10 == 0) TRACENR("\r\n");
	}
	TRACE("=>BS_pd_pos_y=%d", BS_maxVerPdNum);
	for (i = 0; i < BS_maxVerPdNum; i++) {
		TRACE("%d,%f ", i, BS_led_pos_y[i]);
		//if ((i + 1) % 10 == 0) TRACENR("\r\n");
	}
	TRACE("=>BS_led_pos_y=%d", BS_maxVerPdNum);
#endif
#if (LARGE_PITCH_TBL == 1) //nsmoon@200108
#if (DEBUG_INIT_BACKEND > 0) // for testing
	TRACE_BIB("BS_large_pitch_zero_x,BS_large_pitch_end_x,BS_large_pitch_zero_y,BS_large_pitch_end_y: (%0.1f, %0.1f) (%0.1f, %0.1f)", BS_large_pitch_zero_x, BS_large_pitch_end_x, BS_large_pitch_zero_y, BS_large_pitch_end_y);
	TRACE_BIB("BS_sensor_zero_x,BS_sensor_end_x,BS_sensor_zero_y,BS_sensor_end_y: (%0.1f, %0.1f) (%0.1f, %0.1f)", BS_sensor_zero_x, BS_sensor_end_x, BS_sensor_zero_y, BS_sensor_end_y);
	TRACE_BIB("BS_aarea_zero_x,BS_aarea_end_x,BS_aarea_zero_y,BS_aarea_end_y: (%0.1f, %0.1f) (%0.1f, %0.1f)", BS_aarea_zero_x, BS_aarea_end_x, BS_aarea_zero_y, BS_aarea_end_y);
#endif
#endif

#ifdef USE_CUST_MALLOC //nsmoon@201012
    if (BS_main_malloc()) {
        TRACE_ERROR("ERROR! BG_init_backend..BS_main_malloc");
        return NOT_INITIALIZED;
    }
    if (BS_utils_malloc()) {
        TRACE_ERROR("ERROR! BG_init_backend..BS_utils_malloc");
        return NOT_INITIALIZED;
    }
    if (BS_multi_malloc()) {
        TRACE_ERROR("ERROR! BG_init_backend..BS_multi_malloc");
        return NOT_INITIALIZED;
    }
    if (BS_fine_malloc()) {
        TRACE_ERROR("ERROR! BG_init_backend..BG_fine_malloc");
        return NOT_INITIALIZED;
    }
    TRACE_ERROR("BS_end_of_heap= 0x%0x/0x%0x(%d)", BS_end_of_heap, MAX_BS_HEAP_SIZE, MAX_BS_HEAP_SIZE-BS_end_of_heap);
#endif

#if 1  //nsmoon@191224
    BS_large_touch_idx_cnt = 0;
    BS_large_touch_idx_cnt_prev[0] = 0;
    BS_large_touch_idx_cnt_prev[1] = 0;
#endif

#if (LARGE_PITCH_TBL == 1) //nsmoon@200108
	BS_make_large_pitch_tbl(ENUM_HOR_X, ENUM_PD, BS_pd_pos_x);
	BS_make_large_pitch_tbl(ENUM_HOR_X, ENUM_LED, BS_led_pos_x);
	BS_make_large_pitch_tbl(ENUM_VER_Y, ENUM_PD, BS_pd_pos_y);
	BS_make_large_pitch_tbl(ENUM_VER_Y, ENUM_LED, BS_led_pos_y);
#endif

#ifdef CHECK_SLOPE_TBL  //nsmoon@170803a, for testing
	int i, err = 0;
	if (fineScanTbl.len != MAX_fineScanTbl2__) {
		TRACE("ERROR! invalid slope table length %d %d", roughScanTbl.len, MAX_fineScanTbl2__);
		return 14;
}
	for (i = 0; i < fineScanTbl.len; i++) {
		TRACENR("%d:%d(%d) ", i, fineScanTbl.tbl[i], fineScanTbl2__[i]);
		if (fineScanTbl.tbl[i] != fineScanTbl2__[i]) {
			err++;
    }
	}
	if (err > 0) {
		TRACE("=>ERROR! invalid slope table %d", err);
		return 15;
	}
	else {
		TRACE("=>slope table is good !!!");
 }
#endif
#if 0 //def CHK_POS_TBL
    int i;
    TRACE("const float sensor_XPD_Pos__[HORIZONTAL_PCB_CELL_NUMBER] =");
    TRACE("{");
    for (i = 0; i < BS_maxHorPdNum; i++) {
        TRACE("%0.2ff,", BS_pd_pos_x[i]);
    }
    TRACE("};\r\n");

    TRACE("const float sensor_YPD_Pos__[VERTICAL_PCB_CELL_NUMBER] =");
    TRACE("{");
    for (i = 0; i < BS_maxVerPdNum; i++) {
        TRACE("%0.2ff,", BS_pd_pos_y[i]);
	}
    TRACE("};\r\n");

    TRACE("const float sensor_XLED_Pos__[HORIZONTAL_PCB_CELL_NUMBER] =");
    TRACE("{");
    for (i = 0; i < BS_maxHorPdNum; i++) {
        TRACE("%0.2ff,", BS_led_pos_x[i]);
}
    TRACE("};\r\n");

    TRACE("const float sensor_YLED_Pos__[VERTICAL_PCB_CELL_NUMBER] =");
    TRACE("{");
    for (i = 0; i < BS_maxVerPdNum; i++) {
        TRACE("%0.2ff,", BS_led_pos_y[i]);
}
    TRACE("};\r\n");
#endif

#if (BRUSH_MODE_ENABLE == 1)
    BS_brushMode = 0; //init, normal mode
#endif

    BG_frame_no = 0; //for debugging
	backend_initialized = TRUE; // initialized
#if defined(_WIN32) || defined(WIN32)
    BG_touch_count_prev = 0; //nsmoon@230207
#endif
#if 1 //show debug control
    BG_show_ep = 0;
    BG_show_line_x = 0;
    BG_show_line_y = 0;
    BG_show_line_init = 0;
#if defined(DEBUG_FUNCTION_ENABLE_ALL)
    BG_show_line_all = 0;
    BG_show_line_th10 = 0;
    BG_show_line_rem = 1;
#elif defined(DEBUG_FUNCTION_ENABLE_RELEASE)
    BG_show_line_all = 0;
    BG_show_line_th10 = 0;
#if defined(INITIAL_LINE_TEST_MULTI)
    BG_show_line_rem = 1;
#else
    BG_show_line_rem = 1; //0;
#endif
#else
    BG_show_line_all = 0;
    BG_show_line_th10 = 0;
	BG_show_line_rem = 0;
#endif
    //BG_show_debug = 0;
    //BG_show_debug_max = 0;
    BG_debug_pause = 0;
#endif
#ifdef SHOW_DEBUG_SIZE
    BG_debug_minX = MIN_INITIAL_VAL;
    BG_debug_maxX = 0;
    BG_debug_minY = MIN_INITIAL_VAL;
    BG_debug_maxY = 0;
    BG_debug_minXY = MIN_INITIAL_VAL * MIN_INITIAL_VAL;
    BG_debug_maxXY = 0;
    BG_debug_minX2 = MIN_INITIAL_VAL;
    BG_debug_maxX2 = 0;
    BG_debug_minY2 = MIN_INITIAL_VAL;
    BG_debug_maxY2 = 0;
    BG_debug_cnt_total = 0;
    BG_debug_cnt_true = 0;
    BG_debug_cnt_false = 0;
#endif

#ifdef HOVERING_TEST //for testing
//zero_touch_cnt = 0;
    diffXmean = 0;
    diffYmean = 0;
#endif //HOVERING_TEST
#if (DEBUG_TOOL_QT == 1) //nsmoon@181001
    polgon_saved_idx = -1;
    bStartAddPol = FALSE;
#endif
#ifdef FINE_RULE_1A_TEST
    int m;
    for (m = 0; m < ALLOWABLE_TOUCH_BE; m++) {
        BG_debug_touch_area_minmax[m].min = MIN_INITIAL_VAL;
        BG_debug_touch_area_minmax[m].max = 0;
    }
#endif
#if 1 //nsmoon@190325
    maxVertex = 0; //for test
    maxProcessCnt = 0;
    maxSubjCnt = 0;
	maxClipIdx = 0;
#endif

    DEBUG_GET_TIME_DIFF_INIT();

	//backend_utils_init(); //for test
	TRACE_BIB("done...init_backend");
    return (int)NO_BACKEND_ERROR; //no error
}
#else
ATTR_BACKEND_RAMFUNC
int BG_init_backend(
	uint16_t  hor_pd_num,	//number of sensor on X axis
	uint16_t  ver_pd_num,	//number of sensor on Y axis
	ofs_tbl_t *ofsTblX,		//offset table of X axis
	ofs_tbl_t *ofsTblY,		//offset table of Y axis
	DEF_PD_INFO  *ValidTbl,			//not used yet
	DEF_TP_LAYOUT_INFO *tpLayout, 	//sensor laypout
    uint16_t  init_flag,          // reserved: 0
    uint16_t  logical_x_size,	//logical horizontal size
    uint16_t  logical_y_size	//logical vertical size
) {
	//num of pd(led)
	BS_maxHorPdNum = hor_pd_num;
	BS_maxVerPdNum = ver_pd_num;

    //logical size
    BS_logical_max_x = logical_x_size;
    BS_logical_max_y = logical_y_size;

	//offset table
	BS_offsetTblX = ofsTblX->tbl;
	BS_offsetTblLenX = ofsTblX->len;
	BS_offsetTblY = ofsTblY->tbl;
	BS_offsetTblLenY = ofsTblY->len;
	BS_slopeValMaxX = GET_ABS(BS_offsetTblX[0]);
	BS_slopeValMaxY = GET_ABS(BS_offsetTblY[0]);
#ifdef DRAW_POLYGON_TEST
	if (BS_offsetTblLenX != MAX_NUM_X_SLOPES) {
		TRACE_ERROR("ERROR! invalid BS_offsetTblLenX,MAX_NUM_X_SLOPES: %d %d [%d]", BS_offsetTblLenX, MAX_NUM_X_SLOPES, BG_frame_no);
	}
	if (BS_offsetTblLenY != MAX_NUM_Y_SLOPES) {
		TRACE_ERROR("ERROR! invalid BS_offsetTblLenY,MAX_NUM_Y_SLOPES: %d %d [%d]", BS_offsetTblLenY, MAX_NUM_Y_SLOPES, BG_frame_no);
	}
#endif
#if (DEBUG_INIT_BACKEND > 0) // for testing
	TRACE("BS_offsetTblLenX,BS_offsetTblLenY: %d %d", BS_offsetTblLenX, BS_offsetTblLenY);
#endif
	if (BS_offsetTblLenX > MAX_NUM_X_SLOPES || BS_offsetTblLenY > MAX_NUM_Y_SLOPES) {
		TRACE_ERROR("ERROR! invalid BS_offsetTblLenX or BS_offsetTblLenY: %d %d", BS_offsetTblLenX, BS_offsetTblLenY);
		return NOT_INITIALIZED;
	}

#if (DEBUG_INIT_BACKEND > 0) // for testing
	TRACE("BS_maxHorPdNum,BS_maxVePdNum:%d,%d", BS_maxHorPdNum, BS_maxVerPdNum);
	TRACE("BS_offsetTblX(len,tbl):(%d,%x)", BS_offsetTblLenX, BS_offsetTblX);
	TRACE("BS_offsetTblY(len,tbl):(%d,%x)", BS_offsetTblLenY, BS_offsetTblY);
#endif
    
	//sensor position
	BS_sensor_zero_x = tpLayout->sensor_zero_x;
	BS_sensor_end_x = tpLayout->sensor_end_x;
	BS_sensor_zero_y = tpLayout->sensor_zero_y;
	BS_sensor_end_y = tpLayout->sensor_end_y;
	BS_aarea_zero_x = tpLayout->aarea_zero_x;
	BS_aarea_end_x = tpLayout->aarea_end_x;
	BS_aarea_zero_y = tpLayout->aarea_zero_y;
	BS_aarea_end_y = tpLayout->aarea_end_y;
	BS_pd_pos_x = tpLayout->pd_pos_x;
	BS_pd_pos_y = tpLayout->pd_pos_y;
	BS_led_pos_x = tpLayout->led_pos_x;
	BS_led_pos_y = tpLayout->led_pos_y;
	BS_half_end_x = BS_sensor_end_x * 0.5f;
	BS_half_end_y = BS_sensor_end_y * 0.5f;

#if (DEBUG_INIT_BACKEND > 0) // for testing
    TRACE("BS_sensor_zero_x,BS_sensor_end_x,BS_sensor_zero_y,BS_sensor_end_y: %0.1f %0.1f %0.1f %0.1f", BS_sensor_zero_x,BS_sensor_end_x,BS_sensor_zero_y,BS_sensor_end_y);
    TRACE("BS_aarea_zero_x,BS_aarea_end_x,BS_aarea_zero_y,BS_aarea_end_y: %0.1f %0.1f %0.1f %0.1f", BS_aarea_zero_x,BS_aarea_end_x,BS_aarea_zero_y,BS_aarea_end_y);
#endif

#ifdef CHECK_SLOPE_TBL  //nsmoon@170803a, for testing
	int i, err = 0;
	if (fineScanTbl.len != MAX_fineScanTbl2__) {
		TRACE("ERROR! invalid slope table length %d %d", roughScanTbl.len, MAX_fineScanTbl2__);
		return 14;
    }
	for (i = 0; i < fineScanTbl.len; i++) {
		TRACENR("%d:%d(%d) ", i, fineScanTbl.tbl[i], fineScanTbl2__[i]);
		if (fineScanTbl.tbl[i] != fineScanTbl2__[i]) {
			err++;
		}
	}
	if (err > 0) {
		TRACE("=>ERROR! invalid slope table %d", err);
		return 15;
	}
	else {
		TRACE("=>slope table is good !!!");
	}
#endif
#if 0 //def CHK_POS_TBL
    int i;
    TRACE("const float sensor_XPD_Pos__[HORIZONTAL_PCB_CELL_NUMBER] =");
    TRACE("{");
    for (i = 0; i < BS_maxHorPdNum; i++) {
        TRACE("%0.2ff,", BS_pd_pos_x[i]);
    }
    TRACE("};\r\n");

    TRACE("const float sensor_YPD_Pos__[VERTICAL_PCB_CELL_NUMBER] =");
    TRACE("{");
    for (i = 0; i < BS_maxVerPdNum; i++) {
        TRACE("%0.2ff,", BS_pd_pos_y[i]);
    }
    TRACE("};\r\n");

    TRACE("const float sensor_XLED_Pos__[HORIZONTAL_PCB_CELL_NUMBER] =");
    TRACE("{");
    for (i = 0; i < BS_maxHorPdNum; i++) {
        TRACE("%0.2ff,", BS_led_pos_x[i]);
    }
    TRACE("};\r\n");

    TRACE("const float sensor_YLED_Pos__[VERTICAL_PCB_CELL_NUMBER] =");
    TRACE("{");
    for (i = 0; i < BS_maxVerPdNum; i++) {
        TRACE("%0.2ff,", BS_led_pos_y[i]);
    }
    TRACE("};\r\n");
#endif

	BG_init_post_processing(); //FIXME

    BG_frame_no = 0; //for debugging
	backend_initialized = TRUE; // initialized

#ifdef ENABLE_CHECK_INVALID_PD //nsmoon@170121
#if 1 //def ENABLE_NEW_PD_FORMAT
	int i;
	if (ValidTbl->hor_len < BS_maxHorPdNum) {
		invalidPdTable.hor_len = ValidTbl->hor_len;
	}
	else {
		TRACE_ERROR("Error! init_backend().. invalid ValidTbl->hor_len %d \n", ValidTbl->hor_len);
		invalidPdTable.hor_len = BS_maxHorPdNum - 1;
	}

	if (ValidTbl->ver_len < maxVerPdNum) {
		invalidPdTable.ver_len = ValidTbl->ver_len;
	}
	else {
		TRACE_ERROR("Error! init_backend().. invalid ValidTbl->ver_len %d \n", ValidTbl->ver_len);
		invalidPdTable.ver_len = maxVerPdNum - 1;
	}

	for (i = 0; i < ValidTbl->hor_len; i++) {
		invalidPdTable.hor_touch_pd[i] = ValidTbl->hor_touch_pd[i];
	}

	for (i = 0; i < ValidTbl->ver_len; i++) {
		invalidPdTable.ver_touch_pd[i] = ValidTbl->ver_touch_pd[i];
	}
#else
	int i;
	uint32_t  *inValidTbl = ValidTbl;
	for (i = 0; i < MAX_INVALID_PD_TBL - 1; i++) {
		if (*inValidTbl == 0) break;
		invalidPdTable[i] = *inValidTbl++;
	}
	invalidPdTable[i] = 0;  //end of table
#endif
#if 0 //def ENABLE_NEW_PD_FORMAT
    int i;
	for (i = 0; i < BS_maxHorPdNum; i++) {
		invalidPdHorTable[i] = 0;
	}
	for (i = 0; i < maxVerPdNum; i++) {
		invalidPdVerTable[i] = 0;
	}

	for (i = 0; i < ValidTbl->hor_len; i++) {
		if (i >= BS_maxHorPdNum) {
			TRACE_ERROR("Error! invalid invalidPdTable.hor_len %d \n", ValidTbl->hor_len);
			break;
		}

		int bmIdx = GET_SLOPE_INDEX__(ValidTbl->hor_touch_pd[i].pd, ValidTbl->hor_touch_pd[i].led);
		if (bmIdx < 0 || bmIdx >= BIT_OFFSET_MASK_SIZE) {
			TRACE_ERROR("Error! invalid pd %d led %d\n", ValidTbl->hor_touch_pd[i].pd, ValidTbl->hor_touch_pd[i].led);
			break;
		}

		//invalidPdTable.hor_touch_pd[i] = ValidTbl->hor_touch_pd[i];
		invalidPdHorTable[ValidTbl->hor_touch_pd[i].pd] |= bit_offset_mask[bmIdx];
	}

	for(i = 0; i < ValidTbl->ver_len; i++) {
        if(i >= maxVerPdNum) {
            TRACE_ERROR("Error! invalid hor invalidPdTable.ver_len %d \n", ValidTbl->ver_len);
            break;
        }

		int bmIdx = GET_SLOPE_INDEX__(ValidTbl->ver_touch_pd[i].pd, ValidTbl->ver_touch_pd[i].led);
		if (bmIdx < 0 || bmIdx >= BIT_OFFSET_MASK_SIZE) {
			TRACE_ERROR("Error! invalid ver pd %d led %d\n", ValidTbl->ver_touch_pd[i].pd, ValidTbl->ver_touch_pd[i].led);
			break;
		}
		//invalidPdTable.ver_touch_pd[i] = ValidTbl->ver_touch_pd[i];
		invalidPdVerTable[ValidTbl->ver_touch_pd[i].pd] |= bit_offset_mask[bmIdx];
    }
#endif
#endif //ENABLE_CHECK_INVALID_PD
#if 1 //show debug control
    BG_show_ep = 0;
    BG_show_line_x = 0;
    BG_show_line_y = 0;
    BG_show_line_init = 0;
    BG_show_line_all = 0;
    BG_show_line_rem = 0;
#ifdef DEBUG_FUNCTION_ENABLE_ALL
    BG_show_line_rem = 1;
#endif
    //BG_show_debug = 0;
    //BG_show_debug_max = 0;
    BG_debug_pause = 0;
#endif
#ifdef SHOW_DEBUG_SIZE
    BG_debug_minX = MIN_INITIAL_VAL;
    BG_debug_maxX = 0;
    BG_debug_minY = MIN_INITIAL_VAL;
    BG_debug_maxY = 0;
    BG_debug_minXY = MIN_INITIAL_VAL * MIN_INITIAL_VAL;
    BG_debug_maxXY = 0;
    BG_debug_minX2 = MIN_INITIAL_VAL;
    BG_debug_maxX2 = 0;
    BG_debug_minY2 = MIN_INITIAL_VAL;
    BG_debug_maxY2 = 0;
    BG_debug_cnt_total = 0;
    BG_debug_cnt_true = 0;
    BG_debug_cnt_false = 0;
#endif
#ifdef HOVERING_TEST //for testing
//zero_touch_cnt = 0;
    diffXmean = 0;
    diffYmean = 0;
#endif //HOVERING_TEST
#if (DEBUG_TOOL_QT == 1) //nsmoon@181001
    polgon_saved_idx = -1;
    bStartAddPol = FALSE;
#endif
#if 1 //nsmoon@190325
    maxVertex = 0; //for test
    maxProcessCnt = 0;
    maxSubjCnt = 0;
#endif

	TRACE("done...init_backend");
    return 0; //no error
}
#endif

/////////////////////////////////////////////////////
#ifdef MULTI_PACK_EP3_ENABLE //for testing
//move to backend_main.h
//#define EPA_IDX_UNKNOWN  -1
//#define EPA_IDX_0        0
//#define EPA_IDX_1        1
//#define EPA_IDX_2        2
//#define EPA_IDX_3        3
//#define EPA_IDX_LEN      4

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_BG_pack_edge_pattern  0 //1:enable
#endif
ATTR_BACKEND_RAMFUNC
static int epaAddInitial(axis_t axis, int lineNo, int epaIdx, int slopePdVal, int end, int start, int left, int right)
{
	ep_buf_t *epa;
	//ep_no_t epTmp;
	int i; //slopePdValTmp; //j, 
	//int16_t tmpNo;
	int s1, s2, e1, e2;
	//int diffS1 = 0, diffE1 = 0;
	int replace = 0;

	if (axis == ENUM_HOR_X) {
		epa = &BS_edge_pattern_x[epaIdx];
	}
	else {
		epa = &BS_edge_pattern_y[epaIdx];
	}

	if (epa->len == 0)
	{
		if (epa->len < MAX_INIT_EP_COUNT) {
		epa->ep[epa->len].s1 = start;
		epa->ep[epa->len].e1 = end;
		epa->ep[epa->len].s2 = start;
		epa->ep[epa->len].e2 = end;
		epa->len++;
#if (DEBUG_BG_pack_edge_pattern == 1)
			IS_DEBUG_FLAG{
		i = 0;
			TRACE("@N0,%d,%d,%d,:,%d,%d,%d,%d,(%x)", epaIdx, axis, i, \
				epa->ep[i].s1, epa->ep[i].e1, epa->ep[i].s2, epa->ep[i].e2, &epa->ep[epa->len - 1]);
			};
#endif
		}
		else {
            TRACE_ERROR_MEM("ERROR_MEM! epaAddInitial.1.invalid epa->len: %d (%d) [%d]", epaIdx, axis, BG_frame_no);
            return -1; //mem-error
		}
#if (DEBUG_BG_pack_edge_pattern == 2)
		IS_DEBUG_FLAG{
		if (epaIdx == 1) {
			TRACE("@N0,%d,%d,%d,:,%d,%d,%d,%d,(%x)", epaIdx, axis, 0, \
				epa->ep[0].s1, epa->ep[0].e1, epa->ep[0].s2, epa->ep[0].e2, &epa->ep[epa->len - 1]);
			TRACE_NOP;
		}
		};
#endif
		return 1; //new ep
	} //if (epa->len == 0)

	for (i = 0; i < epa->len; i++)
	{
		s1 = epa->ep[i].s1;
		s2 = epa->ep[i].s2;
		e1 = epa->ep[i].e1;
		e2 = epa->ep[i].e2;

#if 1 //nsmoon@190703
		if (epaIdx == EPA_IDX_0) {
			if (slopePdVal == 0) {
				if ((s1 - 1) == start && (e1 - 1) == end) {
					//replace s1, e1
					epa->ep[i].s1 = start;
					epa->ep[i].e1 = end;
					replace++;
				}
				if ((s2 + 1) == start && (e2 + 1) == end) {
					//replace s2, e2
					epa->ep[i].s2 = start;
					epa->ep[i].e2 = end;
					replace++;
				}
				else if (s1 == start && (e1 - 1) == end) {
					//replace e1
					epa->ep[i].e1 = end;
					replace++;
				}
				else if ((s1 - 1) == start && e1 == end) {
				//replace s1
				epa->ep[i].s1 = start;
				replace++;
			}
				else if (s2 == start && (e2 + 1) == end) {
					//replace e2
					epa->ep[i].e2 = end;
					replace++;
				}
				else if ((s2 + 1) == start && e2 == end) {
					//replace s2
					epa->ep[i].s2 = start;
					replace++;
				}
			}
			else if (slopePdVal == 1) {
				if (s1 == start && ((e1 - 1) == end || (e1 - 2) == end)) {
				//replace e1
				epa->ep[i].e1 = end;
				replace++;
			}
				else if ((s1 - 1) == start && (e1 - 1) == end) {
					//replace s1, e1
					epa->ep[i].s1 = start;
					epa->ep[i].e1 = end;
					replace++;
				}
				if (e2 == end && ((s2 + 1) == start || (s2 + 2) == start)) {
				//replace s2
				epa->ep[i].s2 = start;
				replace++;
			}
				else if ((e2 + 1) == end && (s2 + 1) == start) {
					//replace s2, e2
					epa->ep[i].s2 = start;
					epa->ep[i].e2 = end;
					replace++;
				}
				if ((s1 - 1) == start && (e1 + 1) == end) {
					//replace s1, e2
					epa->ep[i].s1 = start;
					replace++;
				}
				if ((s2 + 1) == start && (e2 - 1) == end) {
					//replace s2, e1
					epa->ep[i].s2 = start;
					if ((e1 - 1) == end) {
						epa->ep[i].e1 = end;
					}
					replace++;
				}
			}
			else if (slopePdVal == -1) {
				if (e1 == end && ((s1 - 1) == start || (s1 - 2) == start)) {
					//replace s1
					epa->ep[i].s1 = start;
					replace++;
				}
				else if ((e1 - 1) == end && (s1 - 1) == start) {
					//replace s1, e1
					epa->ep[i].s1 = start;
					epa->ep[i].e1 = end;
					replace++;
				}
				if (s2 == start && ((e2 + 1) == end || (e2 + 2) == end)) {
					//replace e2
					epa->ep[i].e2 = end;
					replace++;
				}
				else if ((s2 + 1) == start && (e2 + 1) == end) {
					//replace s2, e2
					epa->ep[i].s2 = start;
					epa->ep[i].e2 = end;
					replace++;
				}
				if ((s1 - 1) == start && (e1 + 1) == end) {
					//replace s1, e2
					epa->ep[i].s1 = start;
			if ((e2 + 1) == end) {
				epa->ep[i].e2 = end;
					}
				replace++;
			}
			}

			if (s1 <= start && s2 >= start) {
				replace++; //no-change
			}
			if (e1 <= end && e2 >= end) {
				replace++; //no-change
			}

			if (replace) {
  #if (DEBUG_BG_pack_edge_pattern == 1)
				IS_DEBUG_FLAG{
				TRACE("@C11+,%d,%d,%d,:,%d,%d,%d,%d,:%d,%d:,%d,%d,%d,%d,", epaIdx, axis, i, \
					s1, e1, s2, e2, start, end, \
					epa->ep[i].s1, epa->ep[i].e1, epa->ep[i].s2, epa->ep[i].e2);
				};
  #endif
			    return 0; //replace
		    }
		}
#else
		if (slopePdVal == 0) {
			if ((e1 - 1) == end && (s1 - 1) <= start) {
				//replace s1
				epa->ep[i].s1 = start;
				epa->ep[i].e1 = end;
#if (DEBUG_BG_pack_edge_pattern == 1)
				IS_DEBUG_FLAG{
                TRACE("@C11+,%d,%d,%d,:,%d,%d,%d,%d,:%d,%d:,%d,%d,%d,%d,", epaIdx, axis, i, \
                    s1, e1, s2, e2, start, end, \
                    epa->ep[i].s1, epa->ep[i].e1, epa->ep[i].s2, epa->ep[i].e2);
				};
#endif
				return 0;
			}

			if ((e2 + 1) == end && (s2 + 1) >= start) { //nsmoon@181030
				//replace s2
				epa->ep[i].s2 = start;
				epa->ep[i].e2 = end;
#if (DEBUG_BG_pack_edge_pattern == 1)
				IS_DEBUG_FLAG{
                TRACE("@C12+,%d,%d,%d,:,%d,%d,%d,%d,:%d,%d:,%d,%d,%d,%d,", epaIdx, axis, i, \
                    s1, e1, s2, e2, start, end, \
                    epa->ep[i].s1, epa->ep[i].e1, epa->ep[i].s2, epa->ep[i].e2);
				};
#endif
				return 0;
			}

			if ((e1 <= end && e2 >= end) || (s1 <= start && s2 >= start))
			{
				//no-change, ignore
#if (DEBUG_BG_pack_edge_pattern == 1)
				IS_DEBUG_FLAG{
                TRACE("@nochange,%d,%d,%d,:,%d,%d,%d,%d,:%d,%d:,%d,%d,%d,%d,", epaIdx, axis, i, \
                    s1, e1, s2, e2, start, end, \
                    epa->ep[i].s1, epa->ep[i].e1, epa->ep[i].s2, epa->ep[i].e2);
				};
#endif
				return 0;
			}
		}
		else if (slopePdVal == 1 || slopePdVal == -1) {
			if ((e1 - 2) == end && s1 == start) {
				epa->ep[i].e1 = end;
#if (DEBUG_BG_pack_edge_pattern == 1)
				IS_DEBUG_FLAG{
                TRACE("@C21+,%d,%d,%d,:,%d,%d,%d,%d,:%d,%d:,%d,%d,%d,%d,", epaIdx, axis, i, \
                    s1, e1, s2, e2, start, end, \
                    epa->ep[i].s1, epa->ep[i].e1, epa->ep[i].s2, epa->ep[i].e2);
				};
#endif
				return 0;
			}

			if ((e1 - 1) == end && (s1 - 1) <= start) { //nsmoon@181030
				epa->ep[i].e1 = end;
				epa->ep[i].s1 = GET_MIN(s1, start);
#if (DEBUG_BG_pack_edge_pattern == 1)
				IS_DEBUG_FLAG{
                TRACE("@C22+,%d,%d,%d,:,%d,%d,%d,%d,:%d,%d:,%d,%d,%d,%d,", epaIdx, axis, i, \
                    s1, e1, s2, e2, start, end, \
                    epa->ep[i].s1, epa->ep[i].e1, epa->ep[i].s2, epa->ep[i].e2);
				};
#endif
				return 0;
			}

			if ((e1 <= end && e2 >= end) || (s1 <= start && s2 >= start)) {
				if (e1 == end) {
					epa->ep[i].s1 = GET_MIN(s1, start);
#if (DEBUG_BG_pack_edge_pattern == 1)
					IS_DEBUG_FLAG{
                    TRACE("@C23+,%d,%d,%d,:,%d,%d,%d,%d,:%d,%d:,%d,%d,%d,%d,", epaIdx, axis, i, \
                        s1, e1, s2, e2, start, end, \
                        epa->ep[i].s1, epa->ep[i].e1, epa->ep[i].s2, epa->ep[i].e2);
					};
#endif
				}
				if (e2 == end) {
					epa->ep[i].s2 = GET_MAX(s2, start);
#if (DEBUG_BG_pack_edge_pattern == 1)
					IS_DEBUG_FLAG{
                    TRACE("@C24+,%d,%d,%d,:,%d,%d,%d,%d,:%d,%d:,%d,%d,%d,%d,", epaIdx, axis, i, \
                        s1, e1, s2, e2, start, end, \
                        epa->ep[i].s1, epa->ep[i].e1, epa->ep[i].s2, epa->ep[i].e2);
					};
#endif
				}
				return 0;
			}

			if ((e2 + 1) == end && (s2 + 1) >= start) {
				epa->ep[i].e2 = end;
				epa->ep[i].s2 = GET_MAX(s2, start);
#if (DEBUG_BG_pack_edge_pattern == 1)
				IS_DEBUG_FLAG{
                TRACE("@C25+,%d,%d,%d,:,%d,%d,%d,%d,:%d,%d:,%d,%d,%d,%d,", epaIdx, axis, i, \
                    s1, e1, s2, e2, start, end, \
                    epa->ep[i].s1, epa->ep[i].e1, epa->ep[i].s2, epa->ep[i].e2);
				};
#endif
				return 0;
			}

			if ((e2 + 2) == end && s2 == start) {
				epa->ep[i].e2 = end;
#if (DEBUG_BG_pack_edge_pattern == 1)
				IS_DEBUG_FLAG{
				TRACE("@C26+,%d,%d,%d,:,%d,%d,%d,%d,:,%d,%d", epaIdx, axis, i, \
					epa->ep[i].s1, epa->ep[i].e1, epa->ep[i].s2, epa->ep[i].e2, start, end);
				}
#endif
				return 0;
			}
		}
#endif //1
#if 0 //for test, nsmoon@200115, nsmoon@200326 disable
		else if (epaIdx == EPA_IDX_1 || epaIdx == EPA_IDX_2 || epaIdx == EPA_IDX_3) {
#if 0 //nsmoon@190701, nsmoon@200107 for test
			if (axis == ENUM_VER_Y && !left) {
				diffS1 = 0;
			}
			else {
			diffS1 = GET_ABS(s1 - start);
			}

			if (axis == ENUM_VER_Y && !right) {
				diffS1 = 0;
			}
			else {
			    diffE1 = GET_ABS(e1 - end);
			}
#else
			diffS1 = GET_ABS(s1 - start);
			diffE1 = GET_ABS(e1 - end);
#endif
			int chgEpaIdx = 0;
			if (diffS1 == 1) {
				if (s1 > start && end > s1 && start > e1) {
#if (DEBUG_BG_pack_edge_pattern == 2)
					IS_DEBUG_FLAG{
					if (epaIdx == 1) {
						TRACE("@fou--,%d,%d,%d,:,%d,%d,%d,%d,:,%d,%d", epaIdx, axis, i, s1, e1, s2, e2, start, end);
						TRACE_NOP;
					}
					};
#endif
					//replace
					epa->ep[i].s1 = start;
					epa->ep[i].s2 = s1;
					epa->ep[i].e1 = start;
					epa->ep[i].e2 = s1;
					chgEpaIdx++;
				}
				else if (s1 < start && end < s1 && start < e1) {
#if (DEBUG_BG_pack_edge_pattern == 2)
					IS_DEBUG_FLAG{
					if (epaIdx == 1) {
						TRACE("@fou--,%d,%d,%d,:,%d,%d,%d,%d,:,%d,%d", epaIdx, axis, i, s1, e1, s2, e2, start, end);
						TRACE_NOP;
					}
					};
#endif
					//replace
					epa->ep[i].s1 = s1;
					epa->ep[i].s2 = start;
					epa->ep[i].e1 = s1;
					epa->ep[i].e2 = start;
					chgEpaIdx++;
				}
			}
			if (chgEpaIdx == 0 && diffE1 == 1) {
				if (e1 > end && start > e1 && end > s1) {
#if (DEBUG_BG_pack_edge_pattern == 2)
					IS_DEBUG_FLAG{
					if (epaIdx == 1) {
						TRACE("@fou--,%d,%d,%d,:,%d,%d,%d,%d,:,%d,%d", epaIdx, axis, i, s1, e1, s2, e2, start, end);
						TRACE_NOP;
					}
					}:
#endif
					//replace
					epa->ep[i].e1 = end;
					epa->ep[i].e2 = e1;
					epa->ep[i].s1 = end;
					epa->ep[i].s2 = e1;
					chgEpaIdx++;
				}
				else if (e1 < end && start < e1 && end < s1) {
#if (DEBUG_BG_pack_edge_pattern == 2)
					IS_DEBUG_FLAG{
					if (epaIdx == 1) {
						TRACE("@fou--,%d,%d,%d,:,%d,%d,%d,%d,:,%d,%d", epaIdx, axis, i, s1, e1, s2, e2, start, end);
						TRACE_NOP;
					}
					};
#endif
					//replace
					epa->ep[i].e1 = e1;
					epa->ep[i].e2 = end;
					epa->ep[i].s1 = e1;
					epa->ep[i].s2 = end;
					chgEpaIdx++;
				}
			}
			if (chgEpaIdx) {
#if (DEBUG_BG_pack_edge_pattern == 1)
				IS_DEBUG_FLAG{
				TRACE("@found,%d,%d,%d,:,%d,%d,%d,%d,:,%d,%d", epaIdx, axis, i, \
					epa->ep[i].s1, epa->ep[i].e1, epa->ep[i].s2, epa->ep[i].e2, start, end);
				};
#endif	
#if (DEBUG_BG_pack_edge_pattern == 2)
				IS_DEBUG_FLAG{
				if (epaIdx == 1) {
					TRACE("@found,%d,%d,%d,:,%d,%d,%d,%d,:,%d,%d", epaIdx, axis, i, \
						epa->ep[i].s1, epa->ep[i].e1, epa->ep[i].s2, epa->ep[i].e2, start, end);
					TRACE_NOP;
				}
				};
#endif
				return 0;
			}
		}
		else {
			TRACE_ERROR("ERROR! invalid slopePdVal %d [%d]", slopePdVal, BG_frame_no);
			return -1; //error
		}
#endif //1
	} //for (i = 0; i < epa[slopeIdx].len; i++)

	if (epa->len < MAX_INIT_EP_COUNT) {
	  //empty, new0
	epa->ep[epa->len].s1 = start;
	epa->ep[epa->len].e1 = end;
	epa->ep[epa->len].s2 = start;
	epa->ep[epa->len].e2 = end;
	epa->len++;
#if (DEBUG_BG_pack_edge_pattern == 1)
		IS_DEBUG_FLAG{
		TRACE("@N1,%d,%d,%d,:,%d,%d,%d,%d,(%x)", epaIdx, axis, i, \
			epa->ep[i].s1, epa->ep[i].e1, epa->ep[i].s2, epa->ep[i].e2, &epa->ep[epa->len - 1]);
		};
#endif
#if 0 // ascending sorting
	for (i = 0; i < (epa->len - 1); i++) {
		for (j = (i + 1); j < epa->len; j++) {
			if (epa->ep[i].s1 > epa->ep[j].s1) {
				epTmp = epa->ep[i];
				epa->ep[i] = epa->ep[j];
				epa->ep[j] = epTmp;
			}
		}
	}
#endif //0
	}
	else {
        TRACE_ERROR_MEM("ERROR_MEM! epaAddInitial.2.invalid epa->len: %d (%d) [%d]", epaIdx, axis, BG_frame_no);
        return -1; //mem-error
	}
#if (DEBUG_BG_pack_edge_pattern == 2)
	IS_DEBUG_FLAG{
	if (epaIdx == 1) {
		TRACE("@N1,%d,%d,%d,:,%d,%d,%d,%d,(%x)", epaIdx, axis, i, \
			epa->ep[i].s1, epa->ep[i].e1, epa->ep[i].s2, epa->ep[i].e2, &epa->ep[epa->len - 1]);
		TRACE_NOP;
	}
    };
#endif
	return 1; //new
}

void BS_epaInit_fine(void)
{
    int i;
    ep_buf_t *epaX = &BS_edge_pattern_x[0];
    ep_buf_t *epaY = &BS_edge_pattern_y[0];

    //TRACE("BG_epaInit_fine...");
    if (BS_edge_pattern_ep_cnt == 0) {
        BG_epaInit();
        return;
    }

    //x-axis
    for (i = 0; i < MAX_INIT_EPA_SIZE; i++) {
        epaX[i].len = 0;
    }
    //y-axis
    for (i = 0; i < MAX_INIT_EPA_SIZE; i++) {
        epaY[i].len = 0;
    }
}

#define MULTI_INITIAL_MERGE  //nsmoon@191205, FIXME
#ifdef MULTI_INITIAL_MERGE //nsmoon@190321
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_chkInEpa0		0
#endif

#ifdef MULTI_OTHER_EPA_IDX_ENABLE //nsmoon@200129, FIXME
ATTR_BACKEND_RAMFUNC
static int chkInEpa0(axis_t axis, ep_no_t *ep)
{
    ep_buf_t *epa0;
    int i, s1, s2, e1, e2;
    int diffLed, diffPd, merged;

    if (axis == ENUM_HOR_X) {
        epa0 = &BS_edge_pattern_x[EPA_IDX_0];
    }
    else {
        epa0 = &BS_edge_pattern_y[EPA_IDX_0];
    }

#if 0 //(DEBUG_chkInEpa0 == 1)
    TRACE("epa0->ep: %x %d (%d)", &epa0->ep[0], epa0->len, axis);
#endif

    if (ep->s1 == ep->s2) {
        //single line
        return 1; //not check
    }

    diffLed = GET_ABS(ep->s2 - ep->s1);
    diffPd = GET_ABS(ep->e2 - ep->e1);
    if (diffLed == 1) {
        ep->e1 = ep->s1;
        ep->e2 = ep->s2;
    }
    else if (diffPd == 1) {
        ep->s1 = ep->e1;
        ep->s2 = ep->e2;
    }
    else {
        return 1; //not check
    }

    //epa0->len is uint8_t, 256
    for (i = 0; i < epa0->len; i++)
    {
		if (epa0->ep[i].s1 == EPA0_INITIAL_INVALID) continue;
        s1 = epa0->ep[i].s1;
        s2 = epa0->ep[i].s2;
        e1 = epa0->ep[i].e1;
        e2 = epa0->ep[i].e2;
#if (DEBUG_chkInEpa0 == 1)
        //TRACE("epa0->ep[]:(%d)%x:", i, &epa0->ep[i]);
        TRACE("s1,e1,s2,e2: %d %d %d %d, %d %d %d %d", s1, e1, s2, e2, ep->s1, ep->e1, ep->s2, ep->e2);
#endif
        //check merge
        merged = 0;
        if (ep->s2 == (s1 - 1) || ep->s2 == s1) {
            epa0->ep[i].s1 = ep->s1;
            merged++;
        }
        if (ep->e2 == (e1 - 1) || ep->e2 == e1) {
            epa0->ep[i].e1 = ep->e1;
            merged++;
        }
        if (ep->s1 == (s2 + 1) || ep->s1 == s2) {
            epa0->ep[i].s2 = ep->s2;
            merged++;
        }
        if (ep->e1 == (e2 + 1) || ep->e1 == e2) {
            epa0->ep[i].e2 = ep->e2;
            merged++;
        }
        if (merged) {
#if (DEBUG_chkInEpa0 == 1)
            TRACE(" merged:s1,e1,s2,e2: %d %d %d %d", epa0->ep[i].s1, epa0->ep[i].e1, epa0->ep[i].s2, epa0->ep[i].e2);
#endif
            return 1; //merged
        }

        //check included
        if ((ep->s1 >= s1 && ep->s1 <= s2) || (ep->s2 >= s1 && ep->s2 <= s2)) {
#if (DEBUG_chkInEpa0 == 1)
            TRACE(" included:s1,e1,s2,e2: %d %d %d %d", epa0->ep[i].s1, epa0->ep[i].e1, epa0->ep[i].s2, epa0->ep[i].e2);
#endif
            return 1; //included
        }
    }
    return 0;
}
#endif
#else //old method, FIXME
ATTR_BACKEND_RAMFUNC
static int isInEpa0(axis_t axis, int st1, int st2)
{
	ep_buf_t *epa0;
	int i, s1, s2, e1, e2;

	if (axis == ENUM_HOR_X) {
		epa0 = &BS_edge_pattern_x[EPA_IDX_0];
	}
	else {
		epa0 = &BS_edge_pattern_y[EPA_IDX_0];
	}

#if (DEBUG_BG_pack_edge_pattern == 1)
	IS_DEBUG_FLAG{
		TRACE("epa0->ep: (%d) %x %d (%d)", axis, &epa0->ep[0], epa0->len, axis);
	};
#endif

    //epa0->len is uint8_t, 256
	for (i = 0; i < epa0->len; i++)
	{
		s1 = epa0->ep[i].s1;
		s2 = epa0->ep[i].s2;
		e1 = epa0->ep[i].e1;
		e2 = epa0->ep[i].e2;
		s1 = GET_MIN(s1, e1);
		s2 = GET_MAX(s2, e2);
		e1 = s1; 
		e2 = s2;
#if  0 //(DEBUG_BG_pack_edge_pattern == 1)
		IS_DEBUG_FLAG{
		TRACE("epa0->ep[]: (%d) %x", i, &epa0->ep[i]);
		TRACE("s1,s2,e1,e2,st1,st2: %d %d %d %d, %d %d", s1, s2, e1, e2, st1, st2);
	    };
#endif
		if (st1 >= s1 && st1 <= s2) {
			return 1; //found
		}
		if (st2 >= s1 && st2 <= s2) {
			return 1; //found
		}
#if 0
		if (st1 >= e1 && st1 <= e2) {
			return 1; //found
		}
		if (st2 >= e1 && st2 <= e2) {
			return 1; //found
		}
#endif
	}
	return 0;
}
#endif

#if defined(DEBUG_FUNCTION_ENABLE_ALL) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
#define DEBUG_packEdgePatternInitial   0
#endif
ATTR_BACKEND_RAMFUNC
static void epaPosInitial(axis_t axis)
{
    ep_buf_t *epa0;
	float *ledPos, *pdPos;
	float padLedRht, padLedLft, padPdRht, padPdLft;
	int i, j, k, s1, s2, e1, e2, needPad1, needPad2;
    int maxPdNum;

	if (axis == ENUM_HOR_X) {
		epa0 = &BS_edge_pattern_x[EPA_IDX_0];
		//epa1 = &BS_edge_pattern_x[EPA_IDX_1];
		ledPos = &BS_led_pos_x[0];
		pdPos = &BS_pd_pos_x[0];
        maxPdNum = BS_maxHorPdNum - 1;
	}
	else {
		epa0 = &BS_edge_pattern_y[EPA_IDX_0];
		//epa1 = &BS_edge_pattern_y[EPA_IDX_1];
		ledPos = &BS_led_pos_y[0];
		pdPos = &BS_pd_pos_y[0];
        maxPdNum = BS_maxVerPdNum - 1;
	}

#ifdef MULTI_INITIAL_MERGE //nsmoon@190321
    for (i = 0; i < epa0->len; i++) {
		s1 = epa0->ep[i].s1;
		s2 = epa0->ep[i].s2;
		e1 = epa0->ep[i].e1;
		e2 = epa0->ep[i].e2;
        epa0->ep[i].s1 = GET_MIN(s1, e1);
        epa0->ep[i].e1 = epa0->ep[i].s1;
        epa0->ep[i].s2 = GET_MAX(s2, e2);
        epa0->ep[i].e2 = epa0->ep[i].s2;
		}
#endif
#ifdef MULTI_OTHER_EPA_IDX_ENABLE //nsmoon@200129, FIXME
    ep_buf_t *epa1;
	for (k = EPA_IDX_1; k < EPA_IDX_LEN; k++)
	{
		if (axis == ENUM_HOR_X) {
			epa1 = &BS_edge_pattern_x[k];
		}
		else {
			epa1 = &BS_edge_pattern_y[k];
		}

		//TRACE("epa1->len=(%d-%d) %d", axis, k, epa1->len);
		for (i = 0; i < epa1->len; i++)
		{
#if  0 //(DEBUG_packEdgePatternInitial > 1)
			TRACE("epa1->ep[]: (%d) %x", k, &epa1->ep[k]);
#endif
#ifdef MULTI_INITIAL_MERGE //nsmoon@190321
            if (chkInEpa0(axis, &epa1->ep[i])) {
                continue;
            }

			s1 = epa1->ep[i].s1;
			s2 = epa1->ep[i].s2;
			if (s1 == s2) {
				continue; //single line
			}

			e1 = epa1->ep[i].e1;
			e2 = epa1->ep[i].e2;
#else
			s1 = epa1->ep[i].s1;
			s2 = epa1->ep[i].s2;
			if (s1 == s2) {
				continue; //single line
			}

			e1 = epa1->ep[i].e1;
			e2 = epa1->ep[i].e2;

			int diffLed = GET_ABS(s2 - s1);
			int diffPd = GET_ABS(e2 - e1);
			if (diffLed == 1) {
				e1 = s1;
				e2 = s2;
			}
			else if (diffPd == 1) {
				s1 = e1;
				s2 = e2;
			}

			if (isInEpa0(axis, s1, s2)) {
				continue;
			}
#endif
			/////////////////////////////////////////////////////////////////////
			//FIXME, Is necessary merge EPA_IDX_0, EPA_IDX_1, and EPA_IDX_2... ?
			/////////////////////////////////////////////////////////////////////
			j = epa0->len;
			epa0->ep[j].s1 = s1;
			epa0->ep[j].e1 = e1;
			epa0->ep[j].s2 = s2;
			epa0->ep[j].e2 = e2;

			if (axis == ENUM_HOR_X) {
				epa0->poly[j][0].vert.x = ledPos[s1]; //bot-rht
				epa0->poly[j][1].vert.x = pdPos[e1];  //top-rht
				epa0->poly[j][2].vert.x = pdPos[e2];  //top-lft
				epa0->poly[j][3].vert.x = ledPos[s2]; //bot-lft
				epa0->poly[j][0].vert.y = BS_sensor_zero_y; //bot-rht
				epa0->poly[j][1].vert.y = BS_sensor_end_y;  //top-rht
				epa0->poly[j][2].vert.y = BS_sensor_end_y;  //top-lft
				epa0->poly[j][3].vert.y = BS_sensor_zero_y; //bot-lft
			}
			else {
				epa0->poly[j][0].vert.x = BS_sensor_end_x; //bot-rht
				epa0->poly[j][1].vert.x = BS_sensor_zero_x;  //top-rht
				epa0->poly[j][2].vert.x = BS_sensor_zero_x;  //top-lft
				epa0->poly[j][3].vert.x = BS_sensor_end_x; //bot-lft
				epa0->poly[j][0].vert.y = ledPos[s1]; //bot-rht
				epa0->poly[j][1].vert.y = pdPos[e1];  //top-rht
				epa0->poly[j][2].vert.y = pdPos[e2];  //top-lft
				epa0->poly[j][3].vert.y = ledPos[s2]; //bot-lft
			}
			epa0->len++;
#if (DEBUG_packEdgePatternInitial > 1)
			IS_DEBUG_FLAG{
				if (BG_show_line_init)
				//if (k == 1)
				{
					DEBUG_SHOW_POLY(&epa0->poly[j][0], 4, MY_COLOR - 1);
			        TRACE_NOP;
				}
                //TRACE("found,s1,e1,s2,e2: (%d) %d %d %d %d", j, s1, e1, s2, e2);
			};
#endif
		} //for (i = 0; i < epa1->len; i++)
	} //for (k = EPA_IDX_1; k < EPA_IDX_LEN; k++)
#endif //0

#ifdef MULTI_INITIAL_MERGE //nsmoon@190321
    //merge epa0 again
    for (i = 0; i < epa0->len; i++) {
        if (epa0->ep[i].s1 == EPA0_INITIAL_INVALID) continue;
#if 0 //(DEBUG_packEdgePatternInitial > 1)
        IS_DEBUG_FLAG{ TRACE("I:s1,e1,s2,e2: (%d) %d %d %d %d", i, epa0->ep[i].s1, epa0->ep[i].e1, epa0->ep[i].s2, epa0->ep[i].e2); }
#endif
        for (j = 0; j < epa0->len; j++) {
            if (epa0->ep[j].s1 == EPA0_INITIAL_INVALID) continue;
            if (i == j) continue;
            int merged = 0;
            s1 = epa0->ep[j].s1;
            s2 = epa0->ep[j].s2;
            e1 = epa0->ep[j].e1;
            e2 = epa0->ep[j].e2;
#if 0 //(DEBUG_packEdgePatternInitial > 1)
            IS_DEBUG_FLAG{ TRACE("J:s1,e1,s2,e2: (%d) %d %d %d %d", j, s1, e1, s2, e2); }
#endif
            if (epa0->ep[i].s2 >= (s1 - 1) && epa0->ep[i].s2 <= s2) {
                epa0->ep[i].s2 = s2;
                merged++;
	}
            if (epa0->ep[i].e2 >= (e1 - 1) && epa0->ep[i].e2 <= e2) {
                epa0->ep[i].e2 = e2;
                merged++;
}
            if (epa0->ep[i].s1 <= (s2 + 1) && epa0->ep[i].s1 >= s1) {
                epa0->ep[i].s1 = s1;
                merged++;
		}
            if (epa0->ep[i].e1 <= (e2 + 1) && epa0->ep[i].e1 >= e1) {
                epa0->ep[i].e1 = e1;
                merged++;
		}
            if (merged) {
#if 0 //(DEBUG_packEdgePatternInitial > 1)
                IS_DEBUG_FLAG{
                    TRACE("=>delete:J:s1,e1,s2,e2: (%d) %d %d %d %d", j, s1, e1, s2, e2);
                    TRACE("=>merged:I:s1,e1,s2,e2: (%d) %d %d %d %d", i, epa0->ep[i].s1, epa0->ep[i].e1, epa0->ep[i].s2, epa0->ep[i].e2);
                }
#endif
#if 0 //for testing, FIXME ERROR!!!
				epa0->ep[j].s1 = EPA0_INITIAL_INVALID;
#else
                for (k = (j + 1); k < epa0->len; k++) {
                    epa0->ep[k - 1] = epa0->ep[k];
		}
                epa0->len--;
#endif
                i--;
		}
		}
		}
#endif //MULTI_INITIAL_MERGE

    needPad1 = 1; needPad2 = 1; //nsmoon@190320, SPT750
    for (i = 0; i < epa0->len; i++)
    {
		if (epa0->ep[i].s1 == EPA0_INITIAL_INVALID) continue;
        s1 = epa0->ep[i].s1;
        s2 = epa0->ep[i].s2;
        e1 = epa0->ep[i].e1;
        e2 = epa0->ep[i].e2;
        //needPad1 = (s1 == e1);
        //needPad2 = (s2 == e2);
#if 0 //(DEBUG_packEdgePatternInitial > 1)
        IS_DEBUG_FLAG{ TRACE("(axis)s1,e1,s2,e2: (%d)%d,%d,%d,%d", axis, s1, e1, s2, e2); }
#endif
#if 0 //move to above
        s1 = GET_MIN(s1, e1);
        e1 = s1;
        s2 = GET_MAX(s2, e2);
        e2 = s2;
#endif
        padLedRht = 0;
        padLedLft = 0;
        padPdRht = 0;
        padPdLft = 0;

        if (needPad1) {
            if ((s1 - 1) >= 0) {
                //padLedRht = (ledPos[s1] - ledPos[s1 - 1]) * MULTI_INITIAL_POLY_PAD_RATIO;
                padLedRht = (ledPos[s1] - ledPos[s1 - 1]) - MULTI_INITIAL_POLY_PAD_DIST;
        }
            if ((e1 - 1) >= 0) {
                //padPdRht = (pdPos[e1] - pdPos[e1 - 1]) * MULTI_INITIAL_POLY_PAD_RATIO;
                padPdRht = (pdPos[e1] - pdPos[e1 - 1]) - MULTI_INITIAL_POLY_PAD_DIST;
    }
		}
        if (needPad2) {
            if ((s2 + 1) < maxPdNum) {
                //padLedLft = (ledPos[s2 + 1] - ledPos[s2]) * MULTI_INITIAL_POLY_PAD_RATIO;
                padLedLft = (ledPos[s2 + 1] - ledPos[s2]) - MULTI_INITIAL_POLY_PAD_DIST;
        }
            if ((e2 + 1) < maxPdNum) {
                //padPdLft = (pdPos[e2 + 1] - pdPos[e2]) * MULTI_INITIAL_POLY_PAD_RATIO;
                padPdLft = (pdPos[e2 + 1] - pdPos[e2]) - MULTI_INITIAL_POLY_PAD_DIST;
        }
		}

        if (axis == ENUM_HOR_X) {
            epa0->poly[i][0].vert.x = ledPos[s1] - padLedRht; //bot-rht
            epa0->poly[i][0].edgeId = i; //epa idx, FIXME
            epa0->poly[i][1].vert.x = pdPos[e1] - padPdRht;  //top-rht
            epa0->poly[i][1].edgeId = NO_CLIPPING_EDGEID;
            epa0->poly[i][2].vert.x = pdPos[e2] + padPdLft;  //top-lft
            epa0->poly[i][2].edgeId = i; //epa idx, FIXME
            epa0->poly[i][3].vert.x = ledPos[s2] + padLedLft; //bot-lft
            epa0->poly[i][3].edgeId = NO_CLIPPING_EDGEID;
            epa0->poly[i][0].vert.y = BS_sensor_zero_y; //bot-rht
            epa0->poly[i][1].vert.y = BS_sensor_end_y;  //top-rht
            epa0->poly[i][2].vert.y = BS_sensor_end_y;  //top-lft
            epa0->poly[i][3].vert.y = BS_sensor_zero_y; //bot-lft
		}
        else {
            epa0->poly[i][0].vert.x = BS_sensor_end_x; //bot-rht
            epa0->poly[i][1].vert.x = BS_sensor_zero_x;  //top-rht
            epa0->poly[i][2].vert.x = BS_sensor_zero_x;  //top-lft
            epa0->poly[i][3].vert.x = BS_sensor_end_x; //bot-lft
            epa0->poly[i][0].vert.y = ledPos[s1] - padLedRht; //bot-rht
            epa0->poly[i][0].edgeId = i; //epa idx, FIXME
            epa0->poly[i][1].vert.y = pdPos[e1] - padPdRht;  //top-rht
            epa0->poly[i][1].edgeId = NO_CLIPPING_EDGEID;
            epa0->poly[i][2].vert.y = pdPos[e2] + padPdLft;  //top-lft
            epa0->poly[i][2].edgeId = i; //epa idx, FIXME
            epa0->poly[i][3].vert.y = ledPos[s2] + padLedLft; //bot-lft
            epa0->poly[i][3].edgeId = NO_CLIPPING_EDGEID;
		}
#if (DEBUG_packEdgePatternInitial > 1)
        IS_DEBUG_FLAG{
            if (BG_show_line_init) 
		    {
			    DEBUG_SHOW_POLY(&epa0->poly[i][0], 4, MY_COLOR - 2);
			TRACE_NOP;
		}
		};
#endif
	}

    IS_DEBUG_FLAG{
	TRACE_NOP;
	};
}

#define EPA_CENT_SEN_NO_X		(BS_maxHorPdNum / 2)
static int packEdgePatternInitial3(DEF_PD_INFO *InBuf)
{
    int i, led, pd, slopePdIdx, slopePdVal, epaIdx;
#if (DEBUG_packEdgePatternInitial > 0)
    int showLine, color = MY_COLOR;
#endif
    int inBufLen, offsetTblLen, maxPdNum;
    DEF_PD *inBufTouch;
    int8_t *offsetTbl;
    axis_t axis;
#if  1 //nsmoon@190701
	int epaLeftX = 0, epaRightX = 0;
#endif
#if (DEAD_CELL_TBL == 1) //nsmoon@190829
	int deadSen;
#endif
	int ret;

    //TRACE("DEBUG_packEdgePatternInitial...");

    BG_epaInit();

    for (axis = (axis_t)0; axis < ENUM_AXIS_END; axis++) {
        if (axis == ENUM_HOR_X) {
            //x-axis
            inBufLen = InBuf->hor_len;
            inBufTouch = InBuf->hor_touch_pd;
            offsetTblLen = BS_offsetTblLenX;
            offsetTbl = BS_offsetTblX;
            maxPdNum = BS_maxHorPdNum;
#if (DEBUG_packEdgePatternInitial > 0)
			showLine = 1; // BG_show_line_x;
#endif
        }
        else {
            //y-axis
            inBufLen = InBuf->ver_len;
            inBufTouch = InBuf->ver_touch_pd;
            offsetTblLen = BS_offsetTblLenY;
            offsetTbl = BS_offsetTblY;
            maxPdNum = BS_maxVerPdNum;
#if (DEBUG_packEdgePatternInitial > 0)
			showLine = 1; // BG_show_line_y;
#endif
        }

        for (i = 0; i < inBufLen; i++) {
            pd = inBufTouch[i].pd;
            slopePdIdx = inBufTouch[i].led;
            if (slopePdIdx < 0 || slopePdIdx >= offsetTblLen) {
                TRACE_ERROR("ERROR! invalid slopePdIdx,pd: %d %d (%d) %d [%d]", slopePdIdx, pd, i, axis, BG_frame_no);
                continue;
            }
            slopePdVal = offsetTbl[slopePdIdx]; //led-pd
            led = pd + slopePdVal;
            if (led < 0 || pd < 0 || led >= maxPdNum || pd >= maxPdNum) {
                TRACE_ERROR("ERROR! invalid start or end %d %d (%d) %d [%d]", led, pd, i, axis, BG_frame_no);
                continue;
            }
            epaIdx = EPA_IDX_UNKNOWN;
            if (slopePdVal == 0 || slopePdVal == 1 || slopePdVal == -1) {
                epaIdx = EPA_IDX_0;
#if (DEBUG_packEdgePatternInitial > 0)
				color = 0; // MY_COLOR - 2;
#endif
            }
#ifdef MULTI_OTHER_EPA_IDX_ENABLE //nsmoon@200129, FIXME
            else {
#if  1 //nsmoon@190701
				epaIdx = chkOtherEpaIdx(axis, pd, led, epaLeftX, epaRightX); //nsmoon@190701
#else
                epaIdx = chkOtherEpaIdx(axis, pd, led);
#endif
#if (DEBUG_packEdgePatternInitial > 0)
                if (epaIdx == EPA_IDX_1) {
                    color = 1;
                }
                else if (epaIdx == EPA_IDX_2) {
                    color = 2;
                }
                else if (epaIdx == EPA_IDX_3) {
                    color = 3;
                }
#endif
            }
#endif

            if (epaIdx > EPA_IDX_UNKNOWN) {
				if (axis == ENUM_HOR_X) {
					TRACE_NOP;
				}
#if (DEBUG_packEdgePatternInitial > 0)
				if (showLine) 
				{
                    if (epaIdx == EPA_IDX_0) {
                        DEBUG_SHOW_LINE_PD_LED(axis, pd, led, color);
                    }
					TRACE_NOP;
				}
#endif
#if  1 //nsmoon@190701
				if (axis == ENUM_HOR_X) {
					if (pd <= EPA_CENT_SEN_NO_X) {
						epaRightX++;
					}
					else if (pd > EPA_CENT_SEN_NO_X) {
						epaLeftX++;
					}
				}
				ret = epaAddInitial(axis, i, epaIdx, slopePdVal, pd, led, epaLeftX, epaRightX);
				if (ret < 0) {
					continue; //mem-error
				}
  #if (DEAD_CELL_TBL == 1) //nsmoon@190829
				deadSen = BS_is_near_dead(axis, ENUM_LED, led, 1); //include itself
				if (deadSen >= 0) {
					epaIdx = EPA_IDX_0;
    #if (DEBUG_packEdgePatternInitial > 0)
					color = MY_COLOR; // MY_COLOR - 2;
					if (showLine) {
						DEBUG_SHOW_LINE_PD_LED(axis, deadSen, deadSen, color);
						TRACE_NOP;
					}
    #endif
					ret = epaAddInitial(axis, i, epaIdx, slopePdVal, deadSen, deadSen, epaLeftX, epaRightX);
					if (ret < 0) {
						continue; //mem-error
					}
				}
				else {
					deadSen = BS_is_near_dead(axis, ENUM_PD, pd, 1); //include itself
					if (deadSen >= 0) {
						epaIdx = EPA_IDX_0;
    #if (DEBUG_packEdgePatternInitial > 0)
						color = MY_COLOR; // MY_COLOR - 2;
						if (showLine) {
							DEBUG_SHOW_LINE_PD_LED(axis, deadSen, deadSen, color);
							TRACE_NOP;
						}
    #endif
						ret = epaAddInitial(axis, i, epaIdx, slopePdVal, deadSen, deadSen, epaLeftX, epaRightX);
						if (ret < 0) {
							continue; //mem-error
						}
					}
				}
  #endif //(DEAD_CELL_TBL == 1)
#else
                ret = epaAddInitial(axis, i, epaIdx, slopePdVal, pd, led);
				if (ret < 0) {
					continue; //mem-error
				}
#endif
				IS_DEBUG_FLAG{
                TRACE_NOP;
				};
            }
#if 0 //for test //nsmoon@191202
			else {
				//not-found initial line
  #if (DEAD_CELL_TBL == 1) //nsmoon@190829
				if (GET_ABS(slopePdVal) >= 25) 
				{
					deadSen = BS_is_near_dead(axis, ENUM_LED, led, 0); //not include itself
					if (deadSen >= 0) {
						epaIdx = EPA_IDX_0;
    #if (DEBUG_packEdgePatternInitial > 0)
						color = 0; // MY_COLOR - 2;
						if (showLine) {
							DEBUG_SHOW_LINE_PD_LED(axis, deadSen, deadSen, color);
							TRACE_NOP;
						}
    #endif
						ret = epaAddInitial(axis, i, epaIdx, slopePdVal, deadSen, deadSen, epaLeftX, epaRightX);
						if (ret < 0) {
							continue; //mem-error
						}
					}
					else {
						deadSen = BS_is_near_dead(axis, ENUM_PD, pd, 0); //not include itself
						if (deadSen >= 0) {
							epaIdx = EPA_IDX_0;
    #if (DEBUG_packEdgePatternInitial > 0)
							color = 0; // MY_COLOR - 2;
							if (showLine) {
								DEBUG_SHOW_LINE_PD_LED(axis, deadSen, deadSen, color);
								TRACE_NOP;
							}
    #endif
							ret = epaAddInitial(axis, i, epaIdx, slopePdVal, deadSen, deadSen, epaLeftX, epaRightX);
							if (ret < 0) {
								continue; //mem-error
							}
						}
					}
				}
  #endif //(DEAD_CELL_TBL == 1)
			}
#endif //0
        } //for (i = 0; i < inBufLen; i++)
        epaPosInitial(axis);
        IS_DEBUG_FLAG{
        TRACE_NOP;
		};
    } //for (axis = 0; axis < ENUM_AXIS_END; axis++)

    return 0; //no-error
}
#endif
/////////////////////////////////////////////////////

/****************************************************/
#ifdef MULTI_PACK_EP4_ENABLE //for testing
void BS_epaInit_fine(void)
{
	int i;
	ep_buf_t *epaX = &BS_edge_pattern_x[0];
	ep_buf_t *epaY = &BS_edge_pattern_y[0];

	//TRACE("BG_epaInit_fine...");
	if (BS_edge_pattern_ep_cnt == 0) {
		BG_epaInit();
		return;
	}

	//x-axis
#ifdef USE_CUST_MALLOC //nsmoon@201012
    for (i = 0; i < BS_max_ini_epa_size; i++) { //nsmoon@201013
#else
    for (i = 0; i < MAX_INIT_EPA_SIZE; i++) {
#endif
		epaX[i].len = 0;
	}
	//y-axis
#ifdef USE_CUST_MALLOC //nsmoon@201012
    for (i = 0; i < BS_max_ini_epa_size; i++) { //nsmoon@201013
#else
    for (i = 0; i < MAX_INIT_EPA_SIZE; i++) {
#endif
		epaY[i].len = 0;
	}
}

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_BG_pack_edge_pattern  0 //1:enable
#endif
ATTR_BACKEND_RAMFUNC
static int epaAddInitial4(axis_t axis, int epaIdx, int slopePdVal, int end, int start)
{
    ep_buf_t *epa, *epa0, *epa1;
	int s1, s2, e1, e2;
	int replace;
    int i; //, j, k;

#ifdef USE_CUST_MALLOC //nsmoon@201012
    if (epaIdx >= BS_max_ini_epa_size) { //nsmoon@201013
        TRACE_ERROR("ERROR! epaAddInitial4..invalid epaIdx %d/%d", epaIdx, BS_max_ini_epa_size);
        return -1; //mem-error
    }
#else
    if (epaIdx >= MAX_INIT_EPA_SIZE) { //nsmoon@201013
        TRACE_ERROR("ERROR! epaAddInitial4..invalid epaIdx %d/%d", epaIdx, MAX_INIT_EPA_SIZE);
        return -1; //mem-error
    }
#endif

	if (axis == ENUM_HOR_X) {
		epa = &BS_edge_pattern_x[epaIdx];
        epa0 = &BS_edge_pattern_x[EPA_IDX_0];
        epa1 = &BS_edge_pattern_x[EPA_IDX_1];
	}
	else {
		epa = &BS_edge_pattern_y[epaIdx];
        epa0 = &BS_edge_pattern_y[EPA_IDX_0];
        epa1 = &BS_edge_pattern_y[EPA_IDX_1];
	}

	if (epa->len == 0 || epaIdx == EPA_IDX_1)
	{
		if (epaIdx == EPA_IDX_1) {
			//check EPA_IDX_0
			for (i = 0; i < epa0->len; i++)
			{
				s1 = epa0->ep[i].s1;
				s2 = epa0->ep[i].s2;
				e1 = epa0->ep[i].e1;
				e2 = epa0->ep[i].e2;
				if ((start >= s1 && start <= s2) || (end >= e1 && end <= e2)) {
#if (DEBUG_BG_pack_edge_pattern > 0)
		IS_DEBUG_FLAG{
                        TRACE("@D10+,%d,%d,%d,%d,:,%d,%d,%d,%d,:,%d", axis, epaIdx, start, end, \
							epa0->ep[i].s1, epa0->ep[i].e1, epa0->ep[i].s2, epa0->ep[i].e2, i);
				TRACE_NOP;
		};
#endif
					return 0; //duplicated
				}
			} //for (i = 0; i < epa0->len; i++)

			//check EPA_IDX_1
            for (i = 0; i < epa1->len; i++)
	{
                if (epa1->ep[i].s1 == EPA0_INITIAL_INVALID) continue;
				//s1 and s2 must be same
                s1 = epa1->ep[i].s1;
                s2 = epa1->ep[i].s2;
                e1 = epa1->ep[i].e1;
                e2 = epa1->ep[i].e2;
				if ((s1 - start) == -(e1 - end)) {
#if (DEBUG_BG_pack_edge_pattern > 0)
					IS_DEBUG_FLAG{
                        TRACE("@D11+,%d,%d,%d,%d,:,%d,%d,%d,%d,:,%d", axis, epaIdx, start, end, \
                              epa1->ep[i].s1, epa1->ep[i].e1, epa1->ep[i].s2, epa1->ep[i].e2, i);
					TRACE_NOP;
					};
#endif
					return 0; //duplicated
				}
            } //for (i = 0; i < epa1->len; i++)
				}
        else { //EPA_IDX_0
            //check EPA_IDX_1
            for (i = 0; i < epa1->len; i++)
			{
                if (epa1->ep[i].s1 == EPA0_INITIAL_INVALID) continue;
                s1 = epa1->ep[i].s1;
                e1 = epa1->ep[i].e1;
				if (start == s1 || start == e1 || end == s1 || end == e1) {
					//merging
                    epa1->ep[i].s1 = EPA0_INITIAL_INVALID;
					//delete_initial_epa(epa0, i);
#if (DEBUG_BG_pack_edge_pattern > 0)
					IS_DEBUG_FLAG{
                        TRACE("@M10+,%d,%d,%d,%d,:,%d,%d,%d,%d,:,%d", axis, epaIdx, start, end, \
                              epa1->ep[i].s1, epa1->ep[i].e1, epa1->ep[i].s2, epa1->ep[i].e2, i);
					TRACE_NOP;
					};
#endif
				}
            } //for (i = 0; i < epa1->len; i++)
				}

		if (epa->len < MAX_INIT_EP_COUNT) {
			epa->ep[epa->len].s1 = (uint8_t)start;
			epa->ep[epa->len].e1 = (uint8_t)end;
			epa->ep[epa->len].s2 = (uint8_t)start;
			epa->ep[epa->len].e2 = (uint8_t)end;
#if (DEBUG_BG_pack_edge_pattern> 0)
			IS_DEBUG_FLAG{
                TRACE("@N0,%d,%d,%d,%d,:,%d,%d,%d,%d,:,%x", axis, epaIdx, start, end, \
				epa->ep[epa->len].s1, epa->ep[epa->len].e1, epa->ep[epa->len].s2, epa->ep[epa->len].e2, &epa->ep[epa->len]);
			};
#endif
			epa->len++;
				}
		else {
			TRACE_ERROR_MEM("ERROR_MEM! epaAddInitial.1.invalid epa->len: %d (%d) [%d]", epaIdx, axis, BG_frame_no);
			return -1; //mem-error
				}
		return 1; //new ep
	} //if (epa->len == 0 || epaIdx == EPA_IDX_1)

	//check for replace
	if (epaIdx == EPA_IDX_0) {
        //check EPA_IDX_1
        for (i = 0; i < epa1->len; i++)
        {
            if (epa1->ep[i].s1 == EPA0_INITIAL_INVALID) continue;
            s1 = epa1->ep[i].s1;
            e1 = epa1->ep[i].e1;
            s2 = epa1->ep[i].s2;
            e2 = epa1->ep[i].e2;
            if (start == s1 || start == e1 || end == s2 || end == e2) {
                //merging
                epa1->ep[i].s1 = EPA0_INITIAL_INVALID;
                //delete_initial_epa(epa0, i);
#if (DEBUG_BG_pack_edge_pattern > 0)
                IS_DEBUG_FLAG{
                    TRACE("@M13+,%d,%d,%d,%d,:,%d,%d,%d,%d,:,%d", axis, epaIdx, start, end, \
                          epa1->ep[i].s1, epa1->ep[i].e1, epa1->ep[i].s2, epa1->ep[i].e2, i);
                    TRACE_NOP;
                };
#endif
            }
        } //for (i = 0; i < epa1->len; i++)

		//check for replacement
		replace = 0;
		for (i = 0; i < epa->len; i++)
		{
			s1 = epa->ep[i].s1;
			s2 = epa->ep[i].s2;
			if ((s1 - 1) == start) {
					//replace s1, e1
					epa->ep[i].s1 = (uint8_t)start;
					epa->ep[i].e1 = (uint8_t)end;
					replace++;
				}
			else if ((s2 + 1) == start) {
					//replace s2, e2
					epa->ep[i].s2 = (uint8_t)start;
					epa->ep[i].e2 = (uint8_t)end;
					replace++;
				}
			else if (s1 <= start && s2 >= start) {
				replace++; //no-change
			}
			if (replace) {
#if (DEBUG_BG_pack_edge_pattern > 0)
				IS_DEBUG_FLAG{
                    TRACE("@C11+,%d,%d,%d,%d,:,%d,%d,%d,%d,:,%d,%d,:,%d", axis, epaIdx, start, end, \
					       epa->ep[i].s1, epa->ep[i].e1, epa->ep[i].s2, epa->ep[i].e2, s1, s2, i);
				TRACE_NOP;
				};
#endif
				return 0; //replace
            } //if (replace)
		} //for (i = 0; i < epa[slopeIdx].len; i++)

	if (epa->len < MAX_INIT_EP_COUNT) {
		//empty, new0
		epa->ep[epa->len].s1 = (uint8_t)start;
		epa->ep[epa->len].e1 = (uint8_t)end;
		epa->ep[epa->len].s2 = (uint8_t)start;
		epa->ep[epa->len].e2 = (uint8_t)end;
#if (DEBUG_BG_pack_edge_pattern > 0)
		IS_DEBUG_FLAG{
                TRACE("@N1,%d,%d,%d,%d,:,%d,%d,%d,%d,:,%x,:,%d", epaIdx, axis, start, end, \
				epa->ep[epa->len].s1, epa->ep[epa->len].e1, epa->ep[epa->len].s2, epa->ep[epa->len].e2, &epa->ep[epa->len], epa->len);
		};
#endif
			epa->len++;
#if 0 // ascending sorting
		for (i = 0; i < (epa->len - 1); i++) {
			for (j = (i + 1); j < epa->len; j++) {
				if (epa->ep[i].s1 > epa->ep[j].s1) {
					epTmp = epa->ep[i];
					epa->ep[i] = epa->ep[j];
					epa->ep[j] = epTmp;
				}
			}
		}
#endif //0
	}
	else {
		TRACE_ERROR_MEM("ERROR_MEM! epaAddInitial.2.invalid epa->len: %d (%d) [%d]", epaIdx, axis, BG_frame_no);
		return -1; //mem-error
	}
	return 1; //new
	} //if (epaIdx == EPA_IDX_0)

    TRACE_ERROR("ERROR! unknown epa %d-%d %d %d %d", axis, epaIdx, slopePdVal, end, start);
	return 0; //unknown
}

static int epaAddInitial4_epa1(axis_t axis)
{
    ep_buf_t *epa0, *epa1;
    int i;

    if (axis == ENUM_HOR_X) {
        epa0 = &BS_edge_pattern_x[EPA_IDX_0];
        epa1 = &BS_edge_pattern_x[EPA_IDX_1];
    }
    else {

        epa0 = &BS_edge_pattern_y[EPA_IDX_0];
        epa1 = &BS_edge_pattern_y[EPA_IDX_1];
    }

    for (i = 0; i < epa1->len; i++)
    {
        if (epa1->ep[i].s1 == EPA0_INITIAL_INVALID) continue;
        IS_DEBUG_FLAG{
            TRACE_NOP;
        };
        if (epa0->len < MAX_INIT_EP_COUNT) {
            epa0->ep[epa0->len].s1 = epa1->ep[i].s1;
            epa0->ep[epa0->len].e1 = epa1->ep[i].e1;
            epa0->ep[epa0->len].s2 = epa1->ep[i].s2;
            epa0->ep[epa0->len].e2 = epa1->ep[i].e2;
#if (DEBUG_BG_pack_edge_pattern> 0)
            IS_DEBUG_FLAG{
                TRACE("@N2,%d,%d,%d,%d,:,%d,%d,%d,%d,:,%x", axis, i, epa1->ep[i].s1, epa1->ep[i].e1, \
                epa0->ep[epa0->len].s1, epa0->ep[epa0->len].e1, epa0->ep[epa0->len].s2, epa0->ep[epa0->len].e2, &epa0->ep[epa0->len]);
            };
#endif
            epa0->len++;
        }
    } //for (i = 0; i < epa1->len; i++)
    //TRACE("epa0->len= (%d) %d %d", axis, epa0->len, epa1->len);
    return 0; //no-error
}

#if defined(DEBUG_FUNCTION_ENABLE_ALL) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
#define DEBUG_packEdgePatternInitial   1
#endif
#ifndef USE_CUST_MALLOC //nsmoon@201012
ATTR_BACKEND_RAMFUNC
static void epaPosInitial4(axis_t axis)
{
	ep_buf_t *epa;
	float *ledPos, *pdPos;
	float posBotRht, posBotLft, posTopRht, posTopLft;
    int i, s1, s2, e1, e2;
	int maxPdNum;

	if (axis == ENUM_HOR_X) {
		maxPdNum = BS_maxHorPdNum - 1;
	}
	else {
		maxPdNum = BS_maxVerPdNum - 1;
	}

		if (axis == ENUM_HOR_X) {
        epa = &BS_edge_pattern_x[EPA_IDX_0];
			ledPos = &BS_led_pos_x[0];
			pdPos = &BS_pd_pos_x[0];
		}
		else {
        epa = &BS_edge_pattern_y[EPA_IDX_0];
			ledPos = &BS_led_pos_y[0];
			pdPos = &BS_pd_pos_y[0];
		}
		for (i = 0; i < epa->len; i++)
		{
            //if (epa->ep[i].s1 == EPA0_INITIAL_INVALID) continue;
			s1 = epa->ep[i].s1;
			s2 = epa->ep[i].s2;
			e1 = epa->ep[i].e1;
			e2 = epa->ep[i].e2;
        IS_DEBUG_FLAG{
            TRACE_NOP;
        };
            //if (k == EPA_IDX_0)
            if (s1 == e1 && s2 == e2) //EPA_IDX_0
            {
				if (s1 == 0) {
					posBotRht = ledPos[s1] - MULTI_INITIAL_POLY_PAD_0;
			}
				else {
					posBotRht = ledPos[s1 - 1] + MULTI_INITIAL_POLY_PAD_DIST;
			}
				if (e1 == 0) {
					posTopRht = pdPos[e1] - MULTI_INITIAL_POLY_PAD_0;
			}
				else {
					posTopRht = pdPos[e1 - 1] + MULTI_INITIAL_POLY_PAD_DIST;
			}
				if (s2 == maxPdNum) {
					posBotLft = ledPos[s2] + MULTI_INITIAL_POLY_PAD_0;
			}
			else {
					posBotLft = ledPos[s2 + 1] - MULTI_INITIAL_POLY_PAD_DIST;
			}
				if (e2 == maxPdNum) {
					posTopLft = pdPos[e2] + MULTI_INITIAL_POLY_PAD_0;
				}
				else {
					posTopLft = pdPos[e2 + 1] - MULTI_INITIAL_POLY_PAD_DIST;
				}
			}
			else {
                if (e1 < s1) {
					posTopRht = pdPos[e1] + MULTI_INITIAL_POLY_PAD_DIST;
                    posBotRht = ledPos[e1] + MULTI_INITIAL_POLY_PAD_DIST;;
                    posBotLft = ledPos[s1] - MULTI_INITIAL_POLY_PAD_DIST;
                    posTopLft = pdPos[s1] - MULTI_INITIAL_POLY_PAD_DIST;
			}
				else {
					//(s1 < e1)
					posBotRht = ledPos[s1] + MULTI_INITIAL_POLY_PAD_DIST;
                    posTopRht = pdPos[s1] + MULTI_INITIAL_POLY_PAD_DIST;
					posTopLft = pdPos[e1] - MULTI_INITIAL_POLY_PAD_DIST;
                    posBotLft = ledPos[e1] - MULTI_INITIAL_POLY_PAD_DIST;
			}
		}

		if (axis == ENUM_HOR_X) {
				epa->poly[i][0].vert.x = posBotRht; //bot-rht
				epa->poly[i][0].vert.y = BS_sensor_zero_y;
				epa->poly[i][0].edgeId = (uint16_t)i; //epa idx, FIXME
				epa->poly[i][1].vert.x = posTopRht;  //top-rht
				epa->poly[i][1].vert.y = BS_sensor_end_y;
				epa->poly[i][1].edgeId = NO_CLIPPING_EDGEID;
				epa->poly[i][2].vert.x = posTopLft;  //top-lft
				epa->poly[i][2].vert.y = BS_sensor_end_y;
				epa->poly[i][2].edgeId = (uint16_t)i; //epa idx, FIXME
				epa->poly[i][3].vert.x = posBotLft; //bot-lft
				epa->poly[i][3].vert.y = BS_sensor_zero_y;
				epa->poly[i][3].edgeId = NO_CLIPPING_EDGEID;
		}
		else {
				epa->poly[i][0].vert.y = posBotRht; //bot-rht
				epa->poly[i][0].vert.x = BS_sensor_end_x;
				epa->poly[i][0].edgeId = (uint16_t)i; //epa idx, FIXME
				epa->poly[i][1].vert.y = posTopRht;  //top-rht
				epa->poly[i][1].vert.x = BS_sensor_zero_x;
				epa->poly[i][1].edgeId = NO_CLIPPING_EDGEID;
				epa->poly[i][2].vert.y = posTopLft;  //top-lft
				epa->poly[i][2].vert.x = BS_sensor_zero_x;
				epa->poly[i][2].edgeId = (uint16_t)i; //epa idx, FIXME
				epa->poly[i][3].vert.y = posBotLft; //bot-lft
				epa->poly[i][3].vert.x = BS_sensor_end_x;
				epa->poly[i][3].edgeId = NO_CLIPPING_EDGEID;
		}
#if (DEBUG_packEdgePatternInitial > 0)
		IS_DEBUG_FLAG{
				//if (BG_show_line_init)
			{
                    DEBUG_SHOW_POLY(&epa->poly[i][0], 4, MY_COLOR-4);
				TRACE_NOP;
			}
		};
#endif
		} //for (i = 0; i < epa->len; i++)
	IS_DEBUG_FLAG{
		TRACE_NOP;
	};
}
#endif

#define EPA_CENT_SEN_NO_X		(BS_maxHorPdNum / 2)
static int packEdgePatternInitial4(void)
{
    int led, pd, slopePdVal, epaIdx;
	axis_t axis;
    int maxPdNum;
    uint8_t *inBuf;
    int inBufIdx, ledMask;
    int maxInLineBufLed;
#if (DEAD_CELL_TBL == 1) //nsmoon@190829
	int deadSen;
#endif
	int ret;
#if (DEBUG_packEdgePatternInitial > 0) && defined(DRAW_POLYGON_TEST)
    int showLine = 1, color = MY_COLOR;
    if (BG_show_line_init != 1) {
        showLine = 0;
    }
#endif
	//TRACE("DEBUG_packEdgePatternInitial...");
	IS_DEBUG_FLAG{
		TRACE_NOP;
	};

	BG_epaInit();

    for (axis = (axis_t)0; axis < ENUM_AXIS_END; axis++) {
		if (axis == ENUM_HOR_X) {
			//x-axis
            maxPdNum = BS_maxHorPdNum - 1; //nsmoon@201015-bugfix
            inBuf = IN_LINE_BUF_ADDR_X_PD;
#ifdef USE_CUST_MALLOC //nsmoon@201012
            maxInLineBufLed = BS_max_in_line_buf_led_x; //nsmoon@201014
#else
            maxInLineBufLed = MAX_IN_LINE_BUF_LED_X;
#endif
		}
		else {
			//y-axis
            maxPdNum = BS_maxVerPdNum - 1; //nsmoon@201015-bugfix
            inBuf = IN_LINE_BUF_ADDR_Y_PD;
#ifdef USE_CUST_MALLOC //nsmoon@201012
            maxInLineBufLed = BS_max_in_line_buf_led_y; //nsmoon@201014
#else
            maxInLineBufLed = MAX_IN_LINE_BUF_LED_Y;
#endif
		}

#ifdef USE_SENSOR_LIST //for test, nsmoon@200327
        uint8_t *senList;
        int senCnt, senIdx;
        if (axis == ENUM_HOR_X) {
            senList = &BS_pd_x[0];
            senCnt = BS_pd_x_cnt;
		}
		else {
            senList = &BS_pd_y[0];
            senCnt = BS_pd_y_cnt;
		}
#endif
#ifdef USE_SENSOR_LIST //for test, nsmoon@200327
        for (senIdx = 0; senIdx < senCnt; senIdx++) {
            pd = senList[senIdx];
#else
        for (pd = 0; pd <= maxPdNum; pd++) {
#endif
            int ledSt = pd - 1;
            int ledEd = pd + 1;
            ledSt = GET_MAX(ledSt, 0);
            ledEd = GET_MIN(ledEd, maxPdNum);
            int byteIdxSt = GET_LED_BYTE_IDX(ledSt);
            int byteIdxEd = GET_LED_BYTE_IDX(ledEd);
            int byteIdx;
            for (byteIdx = byteIdxSt; byteIdx <= byteIdxEd; byteIdx++) {
                inBufIdx = GET_OFFSET_IN_LINE(pd) + byteIdx; //(pd * ledLen) + led;
                uint8_t tmp8 = inBuf[inBufIdx];
                if (tmp8 == 0) continue;
                int bitIdx;
                for (bitIdx = 0; bitIdx < 8; bitIdx++) {
                    ledMask = (uint8_t)0x1 << bitIdx;
                    if ((tmp8 & (uint8_t)ledMask) == 0) {
				continue;
			}
                    led = byteIdx * 8 + bitIdx;
                    slopePdVal = led - pd;
                    if (slopePdVal == 0) {
                        epaIdx = EPA_IDX_0;
			}
                    else if (slopePdVal == 1 || slopePdVal == -1) {
                        epaIdx = EPA_IDX_1;
			}
			else {
                        epaIdx = EPA_IDX_UNKNOWN;
			}
#if (DEBUG_packEdgePatternInitial > 0) && defined(DRAW_POLYGON_TEST)
                    if (epaIdx == EPA_IDX_0) {
                        color = 0;
			}
                    else if (epaIdx == EPA_IDX_1) {
                        color = 1;
			}
			else {
                        color = 2;
			}
#endif
#ifdef USE_CUST_MALLOC //nsmoon@201012
                    if (epaIdx > EPA_IDX_UNKNOWN && epaIdx < BS_max_ini_epa_size) {
#else
                    if (epaIdx > EPA_IDX_UNKNOWN && epaIdx < MAX_INIT_EPA_SIZE) {
#endif
#if (DEBUG_packEdgePatternInitial > 0) && defined(DRAW_POLYGON_TEST)
				if (showLine)
				{
					//if (epaIdx == EPA_IDX_0) 
					{
						DEBUG_SHOW_LINE_PD_LED(axis, pd, led, color);
					}
					TRACE_NOP;
				}
#endif
                        //TRACE("--pd,led,lineBufIdx,ledMask:, (%d) %d, %d, %d, %x, (%x)", axis, pd, led, inBufIdx, ledMask, tmp8);
                        ret = epaAddInitial4(axis, epaIdx, slopePdVal, pd, led);
				if (ret < 0) {
                            TRACE_ERROR("ERROR! epaAddInitial4..1");
					continue; //mem-error
				}

#if (DEAD_CELL_TBL == 1) //nsmoon@190829
				deadSen = BS_is_near_dead(axis, ENUM_LED, led, 1); //include itself
				if (deadSen >= 0) {
					epaIdx = EPA_IDX_0;
#if (DEBUG_packEdgePatternInitial > 0) && defined(DRAW_POLYGON_TEST)
                            IS_DEBUG_FLAG{
					color = MY_COLOR; // MY_COLOR - 2;
					if (showLine) {
						DEBUG_SHOW_LINE_PD_LED(axis, deadSen, deadSen, color);
						TRACE_NOP;
					}
                            };
#endif
                            ret = epaAddInitial4(axis, epaIdx, slopePdVal, deadSen, deadSen);
					if (ret < 0) {
                                TRACE_ERROR("ERROR! epaAddInitial4..2");
						continue; //mem-error
					}
				}
				else {
					deadSen = BS_is_near_dead(axis, ENUM_PD, pd, 1); //include itself
					if (deadSen >= 0) {
						epaIdx = EPA_IDX_0;
#if (DEBUG_packEdgePatternInitial > 0) && defined(DRAW_POLYGON_TEST)
                                IS_DEBUG_FLAG{
						color = MY_COLOR; // MY_COLOR - 2;
						if (showLine) {
							DEBUG_SHOW_LINE_PD_LED(axis, deadSen, deadSen, color);
							TRACE_NOP;
						}
                                };
#endif
                                ret = epaAddInitial4(axis, epaIdx, slopePdVal, deadSen, deadSen);
						if (ret < 0) {
                                    TRACE_ERROR("ERROR! epaAddInitial4..3");
							continue; //mem-error
						}
					}
				}
#endif //(DEAD_CELL_TBL == 1)
				IS_DEBUG_FLAG{
					TRACE_NOP;
				};
			} //if (epaIdx > EPA_IDX_UNKNOWN)
                } //for (bitIdx = 0; bitIdx < 8; bitIdx++)
            } //for (byteIdx = byteIdxSt; byteIdx <= byteIdxEd; byteIdx++)
            IS_DEBUG_FLAG{
                TRACE_NOP;
            };
        } //for (senIdx = 0; senIdx < senCnt; senIdx++)

        epaAddInitial4_epa1(axis);
#ifndef USE_CUST_MALLOC //nsmoon@201012
        epaPosInitial4(axis); //nsmoon@201008
#endif
		IS_DEBUG_FLAG{
			TRACE_NOP;
		};
	} //for (axis = 0; axis < ENUM_AXIS_END; axis++)

	return 0; //no-error
}
#endif
/****************************************************/

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_addEpaValueMulti	0
#endif
//#define FIXED_PAD_WIDTH_MULTI	0 //4.0f //not used, nsmoon@180823
//#define USE_TOP_EP_PAD			0 //0:disable
//#define USE_HALF_EP_PAD			0 //0:disable
#ifndef USE_CUST_MALLOC //nsmoon@201012
ATTR_BACKEND_RAMFUNC
static void add_pos_edge_pattern(bd_type_t bdType, vec_e_t *poly, ep_no_t *epNo, ep_dz_t deadZone, uint16_t edgeId)
{
	float *top_pd = 0, *bot_led = 0;
	//pad_width_t padWidth;
    int maxPdNum; //maxPosSlopeVal;
	//float diffPos;
	//vec_t p0, p1, p2, p3, res;
    //int ovDz = 0;
	//int slopeVal1, slopeVal2, right_dz = 0, left_dz = 0;
	axis_t axis;
	//sen_type_t sensor;
	int s1 = epNo->s1, s2 = epNo->s2, e1 = epNo->e1, e2 = epNo->e2;

	if (bdType == ENUM_TOP_BD || bdType == ENUM_BOT_BD) {
		//x-axis
		axis = ENUM_HOR_X;
		maxPdNum = BS_maxHorPdNum - 1;
        //maxPosSlopeVal = BS_slopeValMaxX;
		top_pd = &BS_pd_pos_x[0];
		bot_led = &BS_led_pos_x[0];
#if (USE_HALF_EP_PAD ==1)
		//y center line
		p0.x = BS_sensor_zero_y;
		p0.y = (BS_sensor_end_y - BS_sensor_zero_y) * 0.5f;
		p1.x = BS_sensor_end_y;
		p1.y = p0.y;
#endif
	}
	else if (bdType == ENUM_RHT_BD || bdType == ENUM_LFT_BD) {
		//y-axis
		axis = ENUM_VER_Y;
		maxPdNum = BS_maxVerPdNum - 1;
        //maxPosSlopeVal = BS_slopeValMaxY;
		top_pd = &BS_pd_pos_y[0];
		bot_led = &BS_led_pos_y[0];
#if (USE_HALF_EP_PAD ==1)
		//x center line
		p0.x = (BS_sensor_end_x - BS_sensor_zero_x) * 0.5f;
		p0.y = BS_sensor_zero_y;
		p1.x = p0.x;
		p1.y = BS_sensor_end_y;
#endif
	}
	else {
		TRACE_ERROR("ERROR! add_pos_edge_pattern..invalid side %d [%d]", bdType, BG_frame_no);
		return;
	}

	//-----------------------------------
	//bottom right
	//-----------------------------------
	if (deadZone == ENUM_EP_DZ_RIGHT) {
		if (axis == ENUM_HOR_X) {
			poly[0].vert.x = bot_led[0];
			poly[0].vert.y = BS_sensor_zero_y;
		}
		else {
			poly[0].vert.x = BS_sensor_end_x;
			poly[0].vert.y = bot_led[0];
		}
		poly[0].edgeId = NO_CLIPPING_EDGEID;
	}
	else {
		if (axis == ENUM_HOR_X) {
			poly[0].vert.x = bot_led[s1];
			poly[0].vert.y = BS_sensor_zero_y;
		}
		else {
			poly[0].vert.x = BS_sensor_end_x;
			poly[0].vert.y = bot_led[s1];
		}
		poly[0].edgeId = edgeId;
	}

	//-----------------------------------
	//top right
	//-----------------------------------
	if (deadZone == ENUM_EP_DZ_RIGHT) {
		if (axis == ENUM_HOR_X) {
			poly[1].vert.x = top_pd[0];
			poly[1].vert.y = BS_sensor_end_y;
		}
		else {
			poly[1].vert.x = BS_sensor_zero_x;
			poly[1].vert.y = top_pd[0];
		}
	}
	else {
		if (axis == ENUM_HOR_X) {
			poly[1].vert.x = top_pd[e1];
			poly[1].vert.y = BS_sensor_end_y;
		}
		else {
			poly[1].vert.x = BS_sensor_zero_x;
			poly[1].vert.y = top_pd[e1];
		}
	}
	poly[1].edgeId = NO_CLIPPING_EDGEID;

#if (USE_HALF_EP_PAD == 1) 
	{
		p2.x = poly[0].vert.x;
		p2.y = poly[0].vert.y;
		p3.x = poly[1].vert.x;
		p3.y = poly[1].vert.y;

		if (line_sect(&p0, &p1, &p2, &p3, &res) != 0) {
			if (sensor == ENUM_PD)
			{
				poly[0].vert.x = res.x;
				poly[0].vert.y = res.y;
			}
			else {
				poly[1].vert.x = res.x;
				poly[1].vert.y = res.y;
			}
		}
		else {
			TRACE_ERROR("ERROR! add_pos_edge_pattern.1.no intersect");
		}
	}
#endif

	//-----------------------------------
	//top left
	//-----------------------------------
	if (deadZone == ENUM_EP_DZ_LEFT) {
		if (axis == ENUM_HOR_X) {
			poly[2].vert.x = top_pd[maxPdNum]; //top left x
			poly[2].vert.y = BS_sensor_end_y;
		}
		else {
			poly[2].vert.x = BS_sensor_zero_x;
			poly[2].vert.y = top_pd[maxPdNum]; //top left y
		}
		poly[2].edgeId = NO_CLIPPING_EDGEID;
	}
	else {
		if (axis == ENUM_HOR_X) {
			poly[2].vert.x = top_pd[e2]; //top left x
			poly[2].vert.y = BS_sensor_end_y;
		}
		else {
			poly[2].vert.x = BS_sensor_zero_x;
			poly[2].vert.y = top_pd[e2]; //top left y
		}
		poly[2].edgeId = edgeId;
	}

	//-----------------------------------
	//bottom left
	//-----------------------------------
	if (deadZone == ENUM_EP_DZ_LEFT) {
		if (axis == ENUM_HOR_X) {
			poly[3].vert.x = bot_led[maxPdNum]; //bottom left x
			poly[3].vert.y = BS_sensor_zero_y;
		}
		else {
			poly[3].vert.x = BS_sensor_end_x;
			poly[3].vert.y = bot_led[maxPdNum]; //bottom left y
		}
	}
	else {
		if (axis == ENUM_HOR_X) {
			poly[3].vert.x = bot_led[s2]; //bottom left x
			poly[3].vert.y = BS_sensor_zero_y;
		}
		else {
			poly[3].vert.x = BS_sensor_end_x;
			poly[3].vert.y = bot_led[s2]; //bottom left y
		}
	}
	poly[3].edgeId = NO_CLIPPING_EDGEID;

#if (USE_HALF_EP_PAD == 1) 
	{
		p2.x = poly[2].vert.x;
		p2.y = poly[2].vert.y;
		p3.x = poly[3].vert.x;
		p3.y = poly[3].vert.y;
		if (line_sect(&p0, &p1, &p2, &p3, &res) != 0) {
			if (sensor == ENUM_PD)
			{ 
				poly[3].vert.x = res.x;
				poly[3].vert.y = res.y;
			}
			else {
				poly[2].vert.x = res.x;
				poly[2].vert.y = res.y;
			}
		}
		else {
			TRACE_ERROR("ERROR! add_pos_edge_pattern5.2.no intersect");
		}
	}
#endif

#if (DEBUG_addEpaValueMulti > 1)
	IS_DEBUG_FLAG{
	int i;
	TRACENR("%d::(%d-%d)(%d-%d)", bdType, epNo->s1, epNo->s2, epNo->e1, epNo->e2);
	for (i = 0; i < 4; i++) {
		TRACENR("(%0.2f,%0.2f)", poly[i].vert.x, poly[i].vert.y);
	}
	TRACENR("\r\n");
	};
#endif
}
#endif

#if defined(LARGE_SEN_GAP_16) //nsmoon@190313
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_checkLargeGap	0
#endif
#if (DEBUG_checkLargeGap == 1)
#define TRACE_CLG(...)	TRACE(__VA_ARGS__)
#else
#define TRACE_CLG(...)
#endif
ATTR_BACKEND_RAMFUNC
static int checkLargeGap(bd_type_t bdType, ep_no_t *ep)
{
    axis_t axis;
    sen_type_t sensor;
    //uint8_t (*pd_led_tbl)[2], (*led_pd_tbl)[2];
    float *led_pos, *pd_pos, preDiff, nextDiff;
    int color1 = 0, color2 = 0;
    int maxPd, prev, next;

    if (bdType == ENUM_TOP_BD || bdType == ENUM_BOT_BD) {
        axis = ENUM_HOR_X;
        //pd_led_tbl = pd_led_tbl_x3;
        //led_pd_tbl = led_pd_tbl_x3;
        led_pos = BS_led_pos_x;
        pd_pos = BS_pd_pos_x;
        maxPd = BS_maxHorPdNum - 1;
    }
    else {
        //(bdType == ENUM_RHT_BD || bdType == ENUM_LFT_BD)
        axis = ENUM_VER_Y;
        //pd_led_tbl = pd_led_tbl_y3;
        //led_pd_tbl = led_pd_tbl_y3;
        led_pos = BS_led_pos_y;
        pd_pos = BS_pd_pos_y;
        maxPd = BS_maxHorPdNum - 1;
    }
    if (bdType == ENUM_TOP_BD || bdType == ENUM_RHT_BD) {
        sensor = ENUM_PD;
    }
    else {
        sensor = ENUM_LED;
    }

    if (sensor == ENUM_LED) {
        prev = (ep->e1 > 0) ? (ep->e1 - 1) : ep->e1;
        next = (ep->e2 < maxPd) ? (ep->e2 + 1) : ep->e2;
        preDiff = pd_pos[ep->e1] - pd_pos[prev];
        nextDiff = pd_pos[next] - pd_pos[ep->e2];
        if (preDiff > MULTI_ADJ_EPA_GAP) {
            TRACE_CLG("e1: %d %d %0.2f (%0.2f,%0.2f)", axis, ep->e1, preDiff, pd_pos[ep->e1], pd_pos[prev]);
            color1 = 1;
            ep->e1 = prev;
        }
        if (nextDiff > MULTI_ADJ_EPA_GAP) {
            TRACE_CLG("e2: %d %d %0.2f (%0.2f,%0.2f)", axis, ep->e2, nextDiff, pd_pos[ep->e2], pd_pos[next]);
            color2 = 2;
            ep->e2 = next;
        }
    }
    else {
        //(sensor == ENUM_PD)
        prev = (ep->s1 > 0) ? (ep->s1 - 1) : ep->s1;
        next = (ep->s2 < maxPd) ? (ep->s2 + 1) : ep->s2;
        preDiff = led_pos[ep->s1] - led_pos[prev];
        nextDiff = led_pos[next] - led_pos[ep->s2];
        if (preDiff > MULTI_ADJ_EPA_GAP) {
            TRACE_CLG("s1: %d %d %0.2f (%0.2f,%0.2f)", axis, ep->s1, preDiff, led_pos[ep->s1], led_pos[prev]);
            color1 = 1;
            ep->s1 = prev;
        }
        if (nextDiff > MULTI_ADJ_EPA_GAP) {
            TRACE_CLG("s2: %d %d %0.2f (%0.2f,%0.2f)", axis, ep->s2, nextDiff, led_pos[ep->s2], led_pos[next]);
            color2 = 2;
            ep->s2 = next;
        }
    }
    if (color1 || color2) {
#if (DEBUG_checkLargeGap == 1)
        DEBUG_SHOW_LINE_PD_LED(axis, ep->e1, ep->s1, color1);
        DEBUG_SHOW_LINE_PD_LED(axis, ep->e2, ep->s2, color2);
#endif
        TRACE_NOP;
        return 1; //found & modified
    }
    return 0;
}
#endif

ATTR_BACKEND_RAMFUNC
void BS_addEpaPosMulti(bd_type_t bdType)
{
	ep_buf_t *epa;
	int i, j, firstEp, lastEp, cnt;
	int maxPdNum;
	int numEp; //slopeIdx, slopeLedVal,
	int maxSlopeVal, slopeVal1, slopeVal2;
#ifndef USE_CUST_MALLOC //nsmoon@201012
	uint16_t axis_mask, edgeId;
#endif
	//int dz_s1, dz_s2, dz_e1, dz_e2;
	//uint8_t s1, s2, e1, e2;

    //vec_t *top_pd, *bot_led;
#ifndef USE_CUST_MALLOC //nsmoon@201012
	ep_dz_t epDeadZone;
#endif
	//vec_t *top_pd_dz, *bot_led_dz;
	sen_type_t sensor;

	if (bdType != ENUM_TOP_BD && bdType != ENUM_BOT_BD && bdType != ENUM_RHT_BD && bdType != ENUM_LFT_BD) {
		TRACE_ERROR("ERROR! addEpaPosMulti..invalid side %d [%d]", bdType, BG_frame_no);
		return;
	}

	if (bdType == ENUM_TOP_BD || bdType == ENUM_BOT_BD) {
		maxPdNum = BS_maxHorPdNum - 1;
#ifndef USE_CUST_MALLOC //nsmoon@201012
		axis_mask = AXIS_MASK_X;  //0x8000;
#endif
        maxSlopeVal = BS_slopeValMaxX; //BS_offsetTblX[BS_offsetTblLenX - 1];
        //top_pd = (vec_t *)&BS_pd_pos_x[0];
        //bot_led = (vec_t *)&BS_led_pos_x[0];
		epa = EPA_ADDR_X;
	}
	else {
		//(bdType == ENUM_RHT_BD || bdType == ENUM_LFT_BD)
		maxPdNum = BS_maxVerPdNum - 1;
#ifndef USE_CUST_MALLOC //nsmoon@201012
		axis_mask = AXIS_MASK_Y;
#endif
        maxSlopeVal = BS_slopeValMaxY; //BS_offsetTblY[BS_offsetTblLenY - 1];
        //top_pd = (vec_t *)&BS_pd_pos_y[0];
        //bot_led = (vec_t *)&BS_led_pos_y[0];
		epa = EPA_ADDR_Y;
	}
	if (bdType == ENUM_TOP_BD || bdType == ENUM_RHT_BD) {
		sensor = ENUM_PD;
	}
	else {
		sensor = ENUM_LED;
	}
#ifdef USE_SENSOR_LIST //for test, nsmoon@200327
    uint8_t *senList;
    int senCnt, senIdx;
    axis_t axis;
    if (bdType == ENUM_TOP_BD || bdType == ENUM_BOT_BD) {
        axis = ENUM_HOR_X;
    }
    else {
        axis = ENUM_VER_Y;
    }
    if (axis == ENUM_HOR_X) {
        if (sensor == ENUM_PD) {
            senList = &BS_pd_x[0];
            senCnt = BS_pd_x_cnt;
        }
        else {
            senList = &BS_led_x[0];
            senCnt = BS_led_x_cnt;
        }
    }
    else {
        if (sensor == ENUM_PD) {
            senList = &BS_pd_y[0];
            senCnt = BS_pd_y_cnt;
        }
        else {
            senList = &BS_led_y[0];
            senCnt = BS_led_y_cnt;
        }
    }
#endif
	cnt = 0;

#ifdef USE_SENSOR_LIST //for test, nsmoon@200327
    for (senIdx = 0; senIdx < senCnt; senIdx++) {
        i = senList[senIdx];
#else
    for (i = 0; i <= maxPdNum; i++) {
#endif
		numEp = epa[i].len;
		if (numEp < 1) continue; //for testing

		firstEp = 0;
		lastEp = numEp - 1;

		//SEND_POLY(0, 0x21, 0);
		for (j = 0; j < numEp; j++) {
#if (_DEBUG == 1) //for testing
			if (epa[i].ep[j].s1 < 0 || epa[i].ep[j].e1 < 0) {
				TRACE_ERROR("ERROR! invalid s1,e1: (%d,%d) %d %d [%d]", i, j, epa[i].ep[j].s1, epa[i].ep[j].e1, BG_frame_no);
				continue;
			}
			if (epa[i].ep[j].s2 > maxPdNum || epa[i].ep[j].e2 > maxPdNum) {
				TRACE_ERROR("ERROR! invalid s2,e2: (%d,%d) %d %d [%d]", i, j, epa[i].ep[j].s2, epa[i].ep[j].e2, BG_frame_no);
				continue;
			}
			if (sensor == ENUM_PD && epa[i].ep[j].e1 != epa[i].ep[j].e2) {
				TRACE_ERROR("ERROR! invalid e1,e2: (%d,%d) %d %d  [%d]", i, j, epa[i].ep[j].e1, epa[i].ep[j].e2, BG_frame_no);
				continue;
			}
			if (sensor == ENUM_LED && epa[i].ep[j].s1 != epa[i].ep[j].s2) {
				TRACE_ERROR("ERROR! invalid s1,s2: (%d,%d) %d %d  [%d]", i, j, epa[i].ep[j].s1, epa[i].ep[j].s2, BG_frame_no);
				continue;
			}
#endif
#if defined(LARGE_SEN_GAP_16) //nsmoon@1903113
            checkLargeGap(bdType, &epa[i].ep[j]);
#endif
			cnt++;
#ifndef USE_CUST_MALLOC //nsmoon@201012
			epDeadZone = ENUM_EP_DZ_NORMAL;
#endif
            epa[i].dz[j] = 0; //init

			if (j == firstEp) {
				//check right-side
				if (sensor == ENUM_PD && epa[i].ep[j].s1 == 0) {
#ifndef USE_CUST_MALLOC //nsmoon@201012
					epDeadZone = ENUM_EP_DZ_RIGHT;
#endif
					epa[i].dz[j] = epa[i].ep[j].e1; //save org ep 
					epa[i].ep[j].e1 = 0;
				}
				else if (sensor == ENUM_LED && epa[i].ep[j].e1 == 0) {
#ifndef USE_CUST_MALLOC //nsmoon@201012
					epDeadZone = ENUM_EP_DZ_RIGHT;
#endif
					epa[i].dz[j] = epa[i].ep[j].s1; //save org ep no.
					epa[i].ep[j].s1 = 0;
				}
	        }
			if (j == lastEp) {
				//left-side
				if (sensor == ENUM_PD && epa[i].ep[j].s2 == maxPdNum) {
#ifndef USE_CUST_MALLOC //nsmoon@201012
					epDeadZone = ENUM_EP_DZ_LEFT;
#endif
					epa[i].dz[j] = epa[i].ep[j].e2; //save org ep 
					epa[i].ep[j].e2 = (uint8_t)maxPdNum;
				}
				if (sensor == ENUM_LED && epa[i].ep[j].e2 >= maxPdNum) {
#ifndef USE_CUST_MALLOC //nsmoon@201012
					epDeadZone = ENUM_EP_DZ_LEFT;
#endif
					epa[i].dz[j] = epa[i].ep[j].s2; //save org ep 
					epa[i].ep[j].s2 = (uint8_t)maxPdNum;
				}
			}

			if (sensor == ENUM_PD) {
                //check max slope (led-pd)
				slopeVal1 = epa[i].ep[j].s1 - epa[i].ep[j].e1;
				slopeVal2 = epa[i].ep[j].s2 - epa[i].ep[j].e2;
			}
			else {
				//(sensor == ENUM_LED)
				//check max slope (pd-led)
				slopeVal1 = epa[i].ep[j].e1 - epa[i].ep[j].s1;
				slopeVal2 = epa[i].ep[j].e2 - epa[i].ep[j].s2;
			}

			if (slopeVal1 == -maxSlopeVal) {
#if (DEBUG_addEpaValueMulti > 1)
				IS_DEBUG_FLAG{
				TRACE("right_dz = %d (%d %d)", slopeVal2, epa[i].ep[j].s1, epa[i].ep[j].e1);
				};
#endif
#ifndef USE_CUST_MALLOC //nsmoon@201012
				epDeadZone = ENUM_EP_DZ_RIGHT;
#endif
				if (sensor == ENUM_PD) {
					epa[i].dz[j] = epa[i].ep[j].e1; //save org ep no.
				}
				else {
					epa[i].dz[j] = epa[i].ep[j].s1; //save org ep no.
				}
				epa[i].ep[j].s1 = 0;
				epa[i].ep[j].e1 = 0;
			}
			if (slopeVal2 == maxSlopeVal) {
#if (DEBUG_addEpaValueMulti > 1)
				IS_DEBUG_FLAG{
				TRACE("left_dz = %d (%d %d)", slopeVal2, epa[i].ep[j].s2, epa[i].ep[j].e2);
				};
#endif
#ifndef USE_CUST_MALLOC //nsmoon@201012
				epDeadZone = ENUM_EP_DZ_LEFT;
#endif
				if (sensor == ENUM_PD) {
					epa[i].dz[j] = epa[i].ep[j].e2; //save org ep no.
				}
				else {
					epa[i].dz[j] = epa[i].ep[j].s2; //save org ep no.
				}
				epa[i].ep[j].s2 = (uint8_t)maxPdNum;
				epa[i].ep[j].e2 = (uint8_t)maxPdNum;
			}
#ifndef USE_CUST_MALLOC //nsmoon@201012
			edgeId = (uint16_t)(((int16_t)i << 8) + (int16_t)j) | axis_mask;
#endif
#if (DEBUG_addEpaValueMulti > 1)
			IS_DEBUG_FLAG{
			TRACE("bdType,i,j,epa->ep: %d %d %d %x", bdType, i, j, &epa[i].ep[j]);
			};
#endif
#ifndef USE_CUST_MALLOC //nsmoon@201012
            add_pos_edge_pattern(bdType, &epa[i].poly[j][0], &epa[i].ep[j], epDeadZone, edgeId);
    #if (DEBUG_addEpaValueMulti > 0)
			IS_DEBUG_FLAG
			{
				//TRACE("(bdType,i,j)(epDeadZone)(s1,e1,s2,e2)(dz)=(,%d,%d,%d,)(,%d,)(,%d,%d,%d,%d,(,%d,)", bdType, i, j, epDeadZone, epa[i].ep[j].s1, epa[i].ep[j].e1, epa[i].ep[j].s2, epa[i].ep[j].e2, epa[i].dz[j]);
				//SEND_POLY(0, 0x21, 0);
				//if (bdType == ENUM_BOT_BD || bdType == ENUM_RHT_BD)
				{
                    if (BG_show_ep)
                    //if (axis_mask != AXIS_MASK_X)
					{
						DEBUG_SHOW_POLY(&epa[i].poly[j][0], INITIAL_POLYGON_VERTEX_NO, 3);
                        TRACE_NOP;
				}
				}
			//TRACENR("\r\n");
			TRACE_NOP;
			}
    #endif
#endif    
#ifdef DEBUG_EP_TEST //for test //nsmoon@210216
            if (BG_show_ep) {
                vec_e_t polyTmp[INITIAL_POLYGON_VERTEX_NO];
                axis_t axis = (bdType == ENUM_TOP_BD || bdType == ENUM_BOT_BD) ? ENUM_HOR_X : ENUM_VER_Y;
                //DEBUG_SHOW_LINE_PD_LED(axis, epa[i].ep[j].e1, epa[i].ep[j].s1, 0);
                //DEBUG_SHOW_LINE_PD_LED(axis, epa[i].ep[j].e2, epa[i].ep[j].s2, 0);
                //add padding of edge pattern
                addEdgePatternHalfPad(axis, epa, i, j, &polyTmp[0], 0); //0:normal
                DEBUG_SHOW_POLY(&polyTmp[0], INITIAL_POLYGON_VERTEX_NO, 11);
            }
#endif
        } //for (j = 0; j < numEp; j++)
		TRACE_NOP;
	} //for (i = 0; i < MAX_NUM_PD; i++)
	//TRACE("BG_edge_pattern_cnt[]: %d (%d)", BG_edge_pattern_cnt[bdType], bdType);
	if (bdType == ENUM_TOP_BD || bdType == ENUM_BOT_BD) {
		BG_edge_pattern_cnt_x = cnt;
	}
	else {
		BG_edge_pattern_cnt_y = cnt;
	}
	TRACE_NOP;
}

#if 0
ATTR_BACKEND_RAMFUNC
static void sort_inbuf_by_axis(bd_type_t bd, DEF_PD_INFO *InBuf)
{
	int i, j, start_i, start_j, len, slopeVal_i, slopeVal_j, s_flag;
	DEF_PD lineTmp, *touch_pd;
	int8_t *offsetTbl;
	int offsetTblLen;

	if (bd == ENUM_BOT_BD || bd == ENUM_TOP_BD) {
		len = InBuf->hor_len;
		touch_pd = &InBuf->hor_touch_pd[0];
		offsetTbl = &BS_offsetTblX[0];
		offsetTblLen = BS_offsetTblLenX;
	}
	else {
		//(bd == ENUM_LFT_BD || bd == ENUM_RHT_BD)
		len = InBuf->ver_len;
		touch_pd = &InBuf->ver_touch_pd[0];
		offsetTbl = &BS_offsetTblY[0];
		offsetTblLen = BS_offsetTblLenY;
	}

	for (i = 0; i < (len - 1); i++) {
		for (j = (i + 1); j < len; j++) {
			slopeVal_i = offsetTbl[touch_pd[i].led];
			slopeVal_j = offsetTbl[touch_pd[j].led];
			if (bd == ENUM_BOT_BD || bd == ENUM_LFT_BD) {
				start_i = touch_pd[i].pd + slopeVal_i;
				start_j = touch_pd[j].pd + slopeVal_j;
			}
			else {
				//(bd == ENUM_TOP_BD ||bd == ENUM_RHT_BD)
				start_i = touch_pd[i].pd;
				start_j = touch_pd[j].pd;
			}
			//TRACE("start_i,start_j,slopeVal_i,slopeVal_j: %d %d %d %d", start_i, start_j, slopeVal_i, slopeVal_j);
			if (start_j < 0) {
				continue;
			}

			if (start_i < 0) {
				s_flag = 1;
			}
			else {
				if (bd == ENUM_BOT_BD || bd == ENUM_LFT_BD) {
					slopeVal_i = -slopeVal_i;
					slopeVal_j = -slopeVal_j;
				}
				if ((start_i > start_j) || ((start_i == start_j) && (slopeVal_i > slopeVal_j))) {
					s_flag = 1;
				}
				else {
					s_flag = 0;
				}
			}

			if (s_flag) {
				lineTmp = touch_pd[i];
				touch_pd[i] = touch_pd[j];
				touch_pd[j] = lineTmp;
			}
		} //for (j = (i + 1); j < len; j++)
	} //for (i = 0; i < (len - 1); i++)
#if 0 //for testing
	TRACENR("sort_inbuf_by_axis: ");
	for (i = 0; i < len; i++) {
		if (bd == ENUM_BOT_BD || bd == ENUM_LFT_BD) {
			slopeVal_i = offsetTbl[touch_pd[i].led];
			start_i = touch_pd[i].pd + offsetTbl[touch_pd[i].led];
		}
		else {
			//(bd == ENUM_TOP_BD || bd == ENUM_RHT_BD)
			slopeVal_i = offsetTbl[touch_pd[i].led];
			start_i = touch_pd[i].pd;
		}
		if (((i + 1) % 20) == 0) TRACE(".");
		TRACENR("(%d,%d)", start_i, slopeVal_i);
	}
	TRACE(".");
#endif
}
#endif

#if 0 //for test
ATTR_BACKEND_RAMFUNC
static int drawIntersect(bd_type_t bd)
{
	ep_buf_t *epaPd;
	int i, j, k, m;
	vec_t s0, s1, s2, s3, sR;
	vec_t e0, e1, e2, e3, eR;

	if (bd == ENUM_BOT_BD || bd == ENUM_TOP_BD) {
		epaPd = EPA_ADDR_X;
	}
	else {
		epaPd = EPA_ADDR_Y;
	}

	for (i = 0; i < BS_maxHorPdNum; i++) {
		if (epaPd[i].len < 1) continue;
		for (j = 0; j < epaPd[i].len; j++) {
			//if (epaPd[i].ep[j].e1 == 0) continue;
			//if (epaPd[i].ep[j].e2 == BS_maxHorPdNum - 1) continue;
			s0 = epaPd[i].poly[j][0].vert;
			s1 = epaPd[i].poly[j][1].vert;
			e0 = epaPd[i].poly[j][2].vert;
			e1 = epaPd[i].poly[j][3].vert;
			for (k = 0; k < BS_maxHorPdNum; k++) {
				if (epaPd[k].len < 1) continue;
				for (m = 0; m < epaPd[k].len; m++) {
					if (k == i && m == j) continue;
					//if (epaPd[k].ep[m].e1 == 0) continue;
					//if (epaPd[k].ep[m].e2 == BS_maxHorPdNum - 1) continue;
					s2 = epaPd[k].poly[m][0].vert;
					s3 = epaPd[k].poly[m][1].vert;
					e2 = epaPd[k].poly[m][2].vert;
					e3 = epaPd[k].poly[m][3].vert;
                    if (BS_line_intersection(&s0, &s1, &s2, &s3, &sR)) {
						DEBUG_SHOW_POS(&sR, 0.2f, 0.2f, 1);
					}
					else {
						//TRACE("S:no-intersect: (%d %d) (%d %d)", i, j, k, m);
					}
                    if (BS_line_intersection(&e0, &e1, &e2, &e3, &eR)) {
						DEBUG_SHOW_POS(&eR, 0.2f, 0.2f, 2);
					}
					else {
						//TRACE("E:no-intersect: (%d %d) (%d %d)", i, j, k, m);
					}
				}
			}
		}
	}

	return 0; //no error
}

ATTR_BACKEND_RAMFUNC
static int drawEpaMulti(bd_type_t bd)
{
	ep_buf_t *epa;
	int i, j, s1, s2, e1, e2;
	sen_type_t sensor;
	axis_t axis;

	if (bd == ENUM_TOP_BD || bd == ENUM_BOT_BD) {
		axis = ENUM_HOR_X;
		epa = EPA_ADDR_X;
	}
	else {
		axis = ENUM_VER_Y;
		epa = EPA_ADDR_Y;
	}

	if (bd == ENUM_TOP_BD || bd == ENUM_RHT_BD) {
		sensor = ENUM_PD;
	}
	else {
		sensor = ENUM_LED;
	}
	TRACE("drawEpaMulti...bd,axis,sensor: %d %d %d", bd, axis, sensor);
	for (i = 0; i < BS_maxHorPdNum; i++) {
		if (epa[i].len < 1) continue;
		for (j = 0; j < epa[i].len; j++) {
			e1 = epa[i].ep[j].e1;
			s1 = epa[i].ep[j].s1;
			e2 = epa[i].ep[j].e2;
			s2 = epa[i].ep[j].s2;
			//if (e1 == 0 || e2 == BS_maxHorPdNum - 1) continue;
			TRACE("s1,e1,s2,e2: %d %d %d %d", s1, e1, s2, e2);
			//SEND_POLY(0, 0x20, 0);
			if (sensor == ENUM_PD) {
				if (s1 == s2) {
					DEBUG_SHOW_LINE_PD_LED(axis, e1, s1, 3);
				}
				else {
					DEBUG_SHOW_LINE_PD_LED(axis, e1, s1, 1);
					DEBUG_SHOW_LINE_PD_LED(axis, e2, s2, 2);
				}
			}
			else {
				if (e1 == e2) {
					DEBUG_SHOW_LINE_PD_LED(axis, e1, s1, 3);
				}
				else {
					DEBUG_SHOW_LINE_PD_LED(axis, e1, s1, 1);
					DEBUG_SHOW_LINE_PD_LED(axis, e2, s2, 2);
				}
			}
			TRACE_NOP;
		}
	}

	return 0; //no error
}
#endif

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_get_input_buffer	1
#endif
#if (DEBUG_get_input_buffer > 0)
#define TRACE_GIB0(...)	TRACE(__VA_ARGS__)
#else
#define TRACE_GIB0(...)
#endif
#if (DEBUG_get_input_buffer > 1)
#define TRACE_GIB(...)	TRACE(__VA_ARGS__)
#else
#define TRACE_GIB(...)
#endif

#if 1 //nsmoon@190926
int BS_set_line_buffer(axis_t axis, int led, int pd)
{
	uint8_t *inlineUsed;
	uint8_t *inlinePd, *inlineLed;
	int inBufPdIdx, maxInLineBufLed;
	int ledMask;

	if (axis == (int)ENUM_HOR_X) {
		inlinePd = IN_LINE_BUF_ADDR_X_PD;
		inlineLed = IN_LINE_BUF_ADDR_X_LED;
        inlineUsed = LINE_USED2_BUF_ADDR_X;
#ifdef USE_CUST_MALLOC //nsmoon@201012
        maxInLineBufLed = BS_max_in_line_buf_led_x; //nsmoon@201014
#else
        maxInLineBufLed = MAX_IN_LINE_BUF_LED_X;
#endif
						}
						else {
		inlinePd = IN_LINE_BUF_ADDR_Y_PD;
		inlineLed = IN_LINE_BUF_ADDR_Y_LED;
        inlineUsed = LINE_USED2_BUF_ADDR_Y;
#ifdef USE_CUST_MALLOC //nsmoon@201012
        maxInLineBufLed = BS_max_in_line_buf_led_y; //nsmoon@201014
#else
        maxInLineBufLed = MAX_IN_LINE_BUF_LED_Y;
#endif
	}

	inBufPdIdx = GET_IN_LINE_IDX(pd, led); //(pd * ledLen) + led;
	ledMask = GET_LED_BIT_MASK(led);
	inlinePd[inBufPdIdx] |= (uint8_t)ledMask;
    inlineUsed[inBufPdIdx] &= ~(uint8_t)ledMask; //make used line

	inBufPdIdx = GET_IN_LINE_IDX(led, pd);
	ledMask = GET_LED_BIT_MASK(pd);
	inlineLed[inBufPdIdx] |= (uint8_t)ledMask;

	return 0;
}

int BS_is_set_line_buffer(axis_t axis, int pd, int led)
{
	uint8_t *inlinePd;
	int inBufPdIdx, maxInLineBufLed;
	int ledMask, ret;

	if (axis == (int)ENUM_HOR_X) {
		inlinePd = IN_LINE_BUF_ADDR_X_PD;
#ifdef USE_CUST_MALLOC //nsmoon@201012
        maxInLineBufLed = BS_max_in_line_buf_led_x; //nsmoon@201014
#else
        maxInLineBufLed = MAX_IN_LINE_BUF_LED_X;
#endif
	}
	else {
		inlinePd = IN_LINE_BUF_ADDR_Y_PD;
#ifdef USE_CUST_MALLOC //nsmoon@201012
        maxInLineBufLed = BS_max_in_line_buf_led_y; //nsmoon@201014
#else
        maxInLineBufLed = MAX_IN_LINE_BUF_LED_Y;
#endif
	}

	inBufPdIdx = GET_IN_LINE_IDX(pd, led); //(pd * ledLen) + led;
	ledMask = GET_LED_BIT_MASK(led);
	ret = inlinePd[inBufPdIdx] & (uint8_t)ledMask;

	return ret; //0:not-found
						}

int BS_is_used_line_buffer(axis_t axis, int pd, int led)
{
    uint8_t *lineUsed;
    int inBufPdIdx, maxInLineBufLed;
    int ledMask, ret;

    if (axis == (int)ENUM_HOR_X) {
        lineUsed = LINE_USED2_BUF_ADDR_X;
#ifdef USE_CUST_MALLOC //nsmoon@201012
        maxInLineBufLed = BS_max_in_line_buf_led_x; //nsmoon@201014
#else
        maxInLineBufLed = MAX_IN_LINE_BUF_LED_X;
#endif
    }
    else {
        lineUsed = LINE_USED2_BUF_ADDR_Y;
#ifdef USE_CUST_MALLOC //nsmoon@201012
        maxInLineBufLed = BS_max_in_line_buf_led_y; //nsmoon@201014
#else
        maxInLineBufLed = MAX_IN_LINE_BUF_LED_Y;
#endif
    }

    inBufPdIdx = GET_IN_LINE_IDX(pd, led); //(pd * ledLen) + led;
    ledMask = GET_LED_BIT_MASK(led);
    ret = lineUsed[inBufPdIdx] & (uint8_t)ledMask;

    return ret; //0:used
}
#endif

#if (DEAD_CELL_TBL == 1) //nsmoon@190829
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_add_dead_sensor		1
#endif
static int add_dead_sensor_used(void)
{
    int i, m;
    int pd, led, maxPd, stPd, edPd;
    int slopeValMax;
    uint8_t *invalidTblPd, *invalidTblLed;
    int invalidTblPdLen, invalidTblLedLen;
    int axis, sen;
    uint8_t *inlineUsed;
    int inBufPdIdx,  maxInLineBufLed;
    int ledMask;
    IS_DEBUG_FLAG {
      TRACE_NOP;
    };

    for (axis = 0; axis < (int)ENUM_AXIS_END; axis++)
    {
        for (sen = 0; sen < (int)ENUM_SEN_END; sen++)
        {
            if (axis == (int)ENUM_HOR_X) {
                slopeValMax = BS_slopeValMaxX;
                maxPd = BS_maxHorPdNum;
                invalidTblPd = BS_inValidTblXpd;
                invalidTblPdLen = BS_inValidTblLenXpd;
                invalidTblLed = BS_inValidTblXled;
                invalidTblLedLen = BS_inValidTblLenXled;
#ifdef USED_LINE_METHOD_OLD //nsmoon@200119
                inlineStat = LINE_STAT_BUF_ADDR_X;
#endif
                inlineUsed = LINE_USED2_BUF_ADDR_X; //DEAD_CELL_TBL
#ifdef USE_CUST_MALLOC //nsmoon@201012
                maxInLineBufLed = BS_max_in_line_buf_led_x; //nsmoon@201014
#else
                maxInLineBufLed = MAX_IN_LINE_BUF_LED_X;
#endif					
					}
            else {
                slopeValMax = BS_slopeValMaxY;
                maxPd = BS_maxVerPdNum;
                invalidTblPd = BS_inValidTblYpd;
                invalidTblPdLen = BS_inValidTblLenYpd;
                invalidTblLed = BS_inValidTblYled;
                invalidTblLedLen = BS_inValidTblLenYled;
#ifdef USED_LINE_METHOD_OLD //nsmoon@200119
                inlineStat = LINE_STAT_BUF_ADDR_Y;
#endif
                inlineUsed = LINE_USED2_BUF_ADDR_Y; //DEAD_CELL_TBL
#ifdef USE_CUST_MALLOC //nsmoon@201012
                maxInLineBufLed = BS_max_in_line_buf_led_y; //nsmoon@201014
#else
                maxInLineBufLed = MAX_IN_LINE_BUF_LED_Y;
#endif
				}

            //invalid led table
            for (i = 0; i < invalidTblLedLen; i++) {
                led = invalidTblLed[i];
                stPd = GET_MAX(led - slopeValMax, 0);
                edPd = GET_MIN(led + slopeValMax, maxPd - 1);
                for (m = stPd; m <= edPd; m++) {
                    inBufPdIdx = GET_IN_LINE_IDX(m, led); //(pd * ledLen) + led;
                    ledMask = GET_LED_BIT_MASK(led);
                    inlineUsed[inBufPdIdx] &= ~(uint8_t)ledMask; //make used line
                } //for (m = stPd; m <= edPd; m++)
            } //for (i = 0; i < invalidTblLedLen; i++)

            //invalid pd table
            for (i = 0; i < invalidTblPdLen; i++) {
                pd = invalidTblPd[i];
                inBufPdIdx = GET_IN_LINE_IDX(pd, 0); //(pd * ledLen) + led;
                for (m = 0; m < maxInLineBufLed; m++) {
                    inlineUsed[inBufPdIdx + m] = 0x00; //make used line
			}
            } //for (i = 0; i < invalidTblLedLen; i++)

        } //for (sen = 0; sen < (int)ENUM_SEN_END; sen++)
    } //for (axis = 0; axis < (int)ENUM_AXIS_END; axis++)

	return 0; //not-found
}
#endif //DEAD_CELL_TBL
#if 0 //for test
void BS_debug_inlines(void)
{
    DEF_PD_INFO *InBuf = BS_inBuf;
    int8_t *offsetTbl;
    int pd, led;
    int slopeVal;
    int i;
    int color = (MY_COLOR - 3);

#if 0 //x-axis
    offsetTbl = &BS_offsetTblX[0];
    for (i = 0; i < InBuf->hor_len; i++) {
        pd = InBuf->hor_touch_pd[i].pd;
        if (InBuf->hor_touch_pd[i].led >= BS_offsetTblLenX) {
            TRACE_ERROR("ERROR! X invalid led offset %d [%d]", InBuf->hor_touch_pd[i].led, BG_frame_no);
            continue;
        }
        slopeVal = offsetTbl[InBuf->hor_touch_pd[i].led];
        led = InBuf->hor_touch_pd[i].pd + slopeVal;
        if (led < 0 || led >= BS_maxHorPdNum) {
            TRACE_ERROR("ERROR! X invalid led %d/%d/%d [%d]", pd, led, BS_maxHorPdNum, BG_frame_no);
            continue;
        }
        if (GET_ABS(slopeVal) > 30)
        //if (GET_ABS(slopeVal) < 5)
        DEBUG_SHOW_LINE_PD_LED(ENUM_HOR_X, pd, led, color);
    } //for (i = 0; i < InBuf->hor_len; i++)
#endif
#if 1 //y-axis
    offsetTbl = &BS_offsetTblY[0];
    for (i = 0; i < InBuf->ver_len; i++) {
        pd = InBuf->ver_touch_pd[i].pd;
        if (InBuf->ver_touch_pd[i].led >= BS_offsetTblLenY) {
            TRACE_ERROR("ERROR! Y invalid led offset %d [%d]", InBuf->ver_touch_pd[i].led, BG_frame_no);
            continue;
        }
        slopeVal = offsetTbl[InBuf->ver_touch_pd[i].led];
        led = InBuf->ver_touch_pd[i].pd + slopeVal;
        if (led < 0 || led >= BS_maxVerPdNum) {
            TRACE_ERROR("ERROR! Y invalid led %d/%d/%d [%d]", pd, led, BS_maxVerPdNum, BG_frame_no);
            continue;
        }
        if (GET_ABS(slopeVal) > 38)
        DEBUG_SHOW_LINE_PD_LED(ENUM_VER_Y, pd, led, color);
    }
#endif
}
#endif //1

#ifdef USE_SENSOR_LIST //for test, nsmoon@200327
static void sensor_list_init(void)
{
    BS_pd_x_cnt = 0; //init
    BS_led_x_cnt = 0; //init
    BS_pd_y_cnt = 0; //init
    BS_led_y_cnt = 0; //init
}

static int sensor_list_add(axis_t axis, sen_type_t sen, int senNo)
{
    uint8_t *senList;
    int i, senCnt, senCntMax; //senMin, j
    IS_DEBUG_FLAG {
        TRACE_NOP;
    };

    if (axis == ENUM_HOR_X) {
        if (sen == ENUM_PD) {
            senList = &BS_pd_x[0];
            senCnt = BS_pd_x_cnt;
            //senCntMax = X_NUM_PD;
        }
        else {
            senList = &BS_led_x[0];
            senCnt = BS_led_x_cnt;
            //senCntMax = X_NUM_PD;
        }
#ifdef USE_CUST_MALLOC //nsmoon@201012
        senCntMax = BS_maxHorPdNum; //nsmoon@201014
#else
        senCntMax = X_NUM_PD; //nsmoon@201014
#endif
    }
    else {
        if (sen == ENUM_PD) {
            senList = &BS_pd_y[0];
            senCnt = BS_pd_y_cnt;
            //senCntMax = Y_NUM_PD;
        }
        else {
            senList = &BS_led_y[0];
            senCnt = BS_led_y_cnt;
            //senCntMax = Y_NUM_PD;
        }
#ifdef USE_CUST_MALLOC //nsmoon@201012
        senCntMax = BS_maxVerPdNum; //nsmoon@201014
#else
        senCntMax = Y_NUM_PD; //nsmoon@201014
#endif
    }
    
    if (senCnt == 0) {
        senList[senCnt++] = (uint8_t)senNo;
    }
    else {
#if 1 //for test
        for (i = 0; i < senCnt; i++) {
            if (senList[i] == senNo) {
                return i; //found
            }
        }
        if (senCnt < senCntMax) {
            senList[senCnt++] = (uint8_t)senNo;
        }
        else {
            TRACE_ERROR("ERROR! invalid senCnt (%d-%d-%d) %d", axis, sen, senNo, senCnt);
        }
#else
        if (senList[senCnt - 1] < senNo) {
            if (senCnt < senCntMax) {
                senList[senCnt++] = senNo;
            }
            else {
                TRACE_ERROR("ERROR! invalid senCnt (%d-%d-%d) %d", axis, sen, senNo, senCnt);
            }
        }
        else {
            senMin = -1;
            for (i = 0; i < senCnt; i++) {
                if (senList[i] == senNo) {
                    return 1; //found
                }
                if (senList[i] < senNo) {
                    senMin = i;
                }
            }
            if (senCnt < (senCntMax - 1)) {
                for (j = (senCnt - 1); j > senMin; j--) {
                    senList[j + 1] = senList[j];
                }
                senList[senMin + 1] = senNo;
                senCnt++;
            }
            else {
                TRACE_ERROR("ERROR! invalid senCnt (%d-%d-%d) %d", axis, sen, senNo, senCnt);
            }
        }
#endif
    }
    if (axis == ENUM_HOR_X) {
        if (sen == ENUM_PD) {
            BS_pd_x_cnt = senCnt;
        }
        else {
            BS_led_x_cnt = senCnt;
        }
    }
    else {
        if (sen == ENUM_PD) {
            BS_pd_y_cnt = senCnt;
        }
        else {
            BS_led_y_cnt = senCnt;
        }
    }
    return (senCnt-1); //new
}
#endif

#if 1 //for test
ATTR_BACKEND_RAMFUNC
static void get_input_buffer(DEF_PD_INFO *InBuf)
{
    int i, lineBufIdx, pd, led, maxInLineBufLed; //j, ledMask, 
    uint8_t *inlineBuf_pd, *inlineBuf_led, ledMask;
#ifdef USED_LINE_METHOD_OLD //nsmoon@200119
    uint8_t *inlineStat_pd;
#endif
    uint8_t *inlineUsed2_pd;
    int8_t *offsetTbl;
#ifdef GET_SLOPE_0
    uint8_t slope0 = 0, slope1p = 0, slope1m = 0;
#endif
#if (DEBUG_get_input_buffer > 1)
    int show_line = 1; // BG_show_line_all;
    int line_color = (MY_COLOR - 4);
#endif
    int axis;
    int maxSizeInLineBuf, inBufLen;
    int maxPdNum, offsetTblLen;
    DEF_PD	*inBuf;
#ifdef GET_SLOPE_0
    int maxSlopeVal;
#endif

#ifdef SHOW_DEBUG_SIZE
    BG_debug_misc[0] = 0; //+x
    BG_debug_misc[1] = 0; //-x
    BG_debug_misc[2] = 0; //x
    BG_debug_misc[3] = 0; //+y
    BG_debug_misc[4] = 0; //-y
    BG_debug_misc[5] = 0; //y
#endif

#if 0
    for (i = 0; i < MAX_IN_LINE_BUF_PD_X; i++) {
        for (j = 0; j < MAX_IN_LINE_BUF_LED_X; j++) {
            BS_inlineBuf_x_pd[i][j] = 0;
            BS_inlineBuf_x_led[i][j] = 0;
        }
    }
    for (i = 0; i < MAX_IN_LINE_BUF_PD_Y; i++) {
        for (j = 0; j < MAX_IN_LINE_BUF_LED_Y; j++) {
            BS_inlineBuf_y_pd[i][j] = 0;
            BS_inlineBuf_y_led[i][j] = 0;
        }
    }
#endif
#ifdef USE_SENSOR_LIST //for test, nsmoon@200327
    sensor_list_init();
#endif

    for (axis = 0; axis < (int)ENUM_AXIS_END; axis++) {
        if (axis == (int)ENUM_HOR_X) {
            inlineBuf_pd = IN_LINE_BUF_ADDR_X_PD;
            inlineBuf_led = IN_LINE_BUF_ADDR_X_LED;
#ifdef USED_LINE_METHOD_OLD //nsmoon@200119
            inlineStat_pd = LINE_STAT_BUF_ADDR_X;
#endif
            inlineUsed2_pd = LINE_USED2_BUF_ADDR_X;
            offsetTbl = &BS_offsetTblX[0];
#ifdef USE_CUST_MALLOC //nsmoon@201012
            maxInLineBufLed = BS_max_in_line_buf_led_x; //nsmoon@201014
            maxSizeInLineBuf = BS_max_in_line_buf_pd_x * BS_max_in_line_buf_led_x; //nsmoon@201014
#else
            maxInLineBufLed = MAX_IN_LINE_BUF_LED_X;
            maxSizeInLineBuf = MAX_IN_LINE_BUF_PD_X * MAX_IN_LINE_BUF_LED_X;
#endif
            inBufLen = InBuf->hor_len;
            inBuf = &InBuf->hor_touch_pd[0];
            offsetTblLen = BS_offsetTblLenX;
            maxPdNum = BS_maxHorPdNum;
#ifdef GET_SLOPE_0
            maxSlopeVal = BS_slopeValMaxX;
#endif
        }
        else {
            inlineBuf_pd = IN_LINE_BUF_ADDR_Y_PD;
            inlineBuf_led = IN_LINE_BUF_ADDR_Y_LED;
#ifdef USED_LINE_METHOD_OLD //nsmoon@200119
            inlineStat_pd = LINE_STAT_BUF_ADDR_Y;
#endif
            inlineUsed2_pd = LINE_USED2_BUF_ADDR_Y;
            offsetTbl = &BS_offsetTblY[0];
#ifdef USE_CUST_MALLOC //nsmoon@201012
            maxInLineBufLed = BS_max_in_line_buf_led_y; //nsmoon@201014
            maxSizeInLineBuf = BS_max_in_line_buf_pd_y * BS_max_in_line_buf_led_y; //nsmoon@201014
#else
            maxInLineBufLed = MAX_IN_LINE_BUF_LED_Y;
            maxSizeInLineBuf = MAX_IN_LINE_BUF_PD_Y * MAX_IN_LINE_BUF_LED_Y;
#endif
            inBufLen = InBuf->ver_len;
            inBuf = &InBuf->ver_touch_pd[0];
            offsetTblLen = BS_offsetTblLenY;
            maxPdNum = BS_maxVerPdNum;
#ifdef GET_SLOPE_0
            maxSlopeVal = BS_slopeValMaxY;
#endif
        }
        //BS_inlineBuf_x_pd is not same &BS_inlineBuf_x_pd[0][0], FIXME!!!
        //TRACE("inlineBuf_pd= %x %x %x %x", inlineBuf_pd, BS_inlineBuf_x_pd, *BS_inlineBuf_x_pd[0], &BS_inlineBuf_x_pd[0][0]);
        IS_DEBUG_FLAG{ TRACE_GIB("inlineBuf_led in get_input_buffer: (%d) %x %d ", axis, inlineBuf_led, InBuf->hor_len); };
        //IS_DEBUG_FLAG{ TRACE("inlineBuf_led in get_input_buffer: (%d) %x %d ", axis, inlineBuf_led, InBuf->hor_len); }
        memset(inlineBuf_pd, 0, maxSizeInLineBuf);
        memset(inlineBuf_led, 0, maxSizeInLineBuf);
#ifdef USED_LINE_METHOD_OLD //nsmoon@200119
        memset(inlineStat_pd, 0, maxSizeInLineBuf);
#else
        memset(inlineUsed2_pd, 0, maxSizeInLineBuf);
#endif

        //TRACE("uint8_t test_data_%d[MAX_TEST_DATA][2]= {", axis);
        for (i = 0; i < inBufLen; i++) {
#if 0 //(USE_ADC_DATA == 1), for debugging
            int8_t adcTmp = inBuf[i].adc;
            TRACE("adcTmp= (%d) %d", axis, adcTmp);
#endif
            //TRACE("{%d,%d},", inBuf[i].pd, inBuf[i].led);
            pd = inBuf[i].pd;
            if (inBuf[i].led >= offsetTblLen) {
                TRACE_ERROR("ERROR! invalid led offset (%d) %d [%d]", axis, inBuf[i].led, BG_frame_no);
                continue;
            }
            led = inBuf[i].pd + offsetTbl[inBuf[i].led];
            if (led < 0 || led >= maxPdNum) {
                TRACE_ERROR("ERROR! invalid led (%d) %d/%d/%d [%d]", axis, pd, led, maxPdNum, BG_frame_no);
                continue;
            }
#if 0 //for test
            if (offsetTbl[inBuf[i].led] >= -1 && offsetTbl[inBuf[i].led] <= 1) {
                TRACE("{%d,%d,%d},", axis, pd, led);
            }
#endif
#ifdef USE_SENSOR_LIST //for test, nsmoon@200327
            sensor_list_add((axis_t)axis, ENUM_PD, pd);
            sensor_list_add((axis_t)axis, ENUM_LED, led);
#endif

#ifdef GET_SLOPE_0
            if (inBuf[i].led == maxSlopeVal) {
                slope0++;
            }
            else if (inBuf[i].led == (maxSlopeVal - 1)) {
                slope1m++;
            }
            else if (inBuf[i].led == (maxSlopeVal + 1)) {
                slope1p++;
            }
#endif
#ifdef SHOW_DEBUG_SIZE
            int slopeVal = led - pd;
            if (axis == (int)ENUM_HOR_X) {
                if (slopeVal > 0) {
                    BG_debug_misc[0]++; //+x
                }
                else if (slopeVal < 0){
                    BG_debug_misc[1]++; //-x
                }
                else {
                    BG_debug_misc[2]++; //x
                }
            }
            else {
                if (slopeVal > 0) {
                    BG_debug_misc[3]++; //+x
                }
                else if (slopeVal < 0){
                    BG_debug_misc[4]++; //-x
                }
                else {
                    BG_debug_misc[5]++; //x
                }
            }
#endif
#if (DEBUG_get_input_buffer > 1)
            if (show_line) DEBUG_SHOW_LINE_PD_LED((axis_t)axis, pd, led, line_color);
#endif
#if (defined(DEBUG_FUNCTION_ENABLE_RELEASE) || defined(DEBUG_FUNCTION_ENABLE_ALL)) && (DEBUG_packEdgePatternInitial > 0)
#ifdef FRONTEND_LINE_THRESHOLD
            //int slopeValMax = (axis == ENUM_HOR_X) ? BS_slopeValMaxX : BS_slopeValMaxY;
            //int slopePdIdx = slopePdVal + slopeValMax;
            int slopePdIdx = inBuf[i].led;
            if (BG_show_line_all && !BG_show_line_rem) {
                if (BS_is_set_threshold(axis, pd, slopePdIdx)) {
                    //TRACE_RELEASE("pd,led(th10)= %d %d", pd, led);
                    DEBUG_SHOW_LINE_PD_LED(axis, pd, led, MY_COLOR - 2);
                }
                else {
                    //TRACE_RELEASE("pd,led= %d %d", pd, led);
                    DEBUG_SHOW_LINE_PD_LED(axis, pd, led, MY_COLOR - 1);
                }
            }
#else
            if (BG_show_line_all && !BG_show_line_rem) {
                DEBUG_SHOW_LINE_PD_LED(axis, pd, led, MY_COLOR - 4);
            }
#endif
#endif
            //pd table
            lineBufIdx = GET_IN_LINE_IDX(pd, led);
            ledMask = GET_LED_BIT_MASK(led);
            inlineBuf_pd[lineBufIdx] |= (uint8_t)ledMask;
#ifdef USED_LINE_METHOD_OLD //nsmoon@200119
            inlineStat_pd[lineBufIdx] |= (uint8_t)ledMask;
#else
            inlineUsed2_pd[lineBufIdx] |= (uint8_t)ledMask;
#endif
#if 0 //for test
            if (offsetTbl[inBuf[i].led] >= -1 && offsetTbl[inBuf[i].led] <= 1) {
                TRACE("--pd,led,lineBufIdx,ledMask:, (%d) %d, %d, %d, %x, (%x)", axis, pd, led, lineBufIdx, ledMask, inlineBuf_pd[lineBufIdx]);
            }
#endif
            IS_DEBUG_FLAG{ TRACE_GIB("--pd,led,lineBufIdx,ledMask:, (%d) %d, %d, %d, %x, (%x)", axis, pd, led, lineBufIdx, ledMask, inlineBuf_pd[lineBufIdx]); };

            //led table
            lineBufIdx = GET_IN_LINE_IDX(led, pd);
            ledMask = GET_LED_BIT_MASK(pd);
            inlineBuf_led[lineBufIdx] |= (uint8_t)ledMask;
            IS_DEBUG_FLAG{ TRACE_GIB("--led,pd,lineBufIdx,ledMask:, (%d) %d, %d, %d, %x, (%x)", axis, led, pd, lineBufIdx, ledMask, inlineBuf_led[lineBufIdx]); };
            TRACE_NOP;
        } //for (i = 0; i < inBuf-Len; i++)
#ifdef GET_SLOPE_0
        slope_cnt0 = GET_MAX(slope1m, slope1p);
        slope_cnt0 = GET_MAX(slope0, slope_cnt0);
        IS_DEBUG_FLAG{ TRACE_GIB0("slope0: (%d) %d %d %d", axis, slope0, slope1p, slope1m); };
        //TRACE_RELEASE("slope0: (%d) %d %d %d", axis, slope0, slope1p, slope1m);
#endif
    } //for (axis = 0; axis < (int)ENUM_AXIS_END; axis++)

#ifdef USE_SENSOR_LIST //for test, nsmoon@200327
    IS_DEBUG_FLAG{ TRACE("BS_pd_x_cnt:,%d,%d,%d,%d", BS_pd_x_cnt, BS_led_x_cnt, BS_pd_y_cnt, BS_led_y_cnt); };
#endif
#if 0 //def USE_SENSOR_LIST //for test, nsmoon@200327
    int listNo, senCnt;
    uint8_t *senList;
    for (listNo = 0; listNo < 4; listNo++) {
        if (listNo == 0) {
            senList = &BS_pd_x[0];
            senCnt = BS_pd_x_cnt;
        }
        else if (listNo == 1) {
            senList = &BS_led_x[0];
            senCnt = BS_led_x_cnt;
        }
        else if (listNo == 2) {
            senList = &BS_pd_y[0];
            senCnt = BS_pd_y_cnt;
        }
        else {
            senList = &BS_led_y[0];
            senCnt = BS_led_y_cnt;
        }
        for (i = 0; i < senCnt; i++) {
            TRACENR("%d ", senList[i]);
        }
        TRACE("=>%d (%d)", senCnt, listNo);
    }
    TRACE_NOP;
#endif
    IS_DEBUG_FLAG{
        //TRACE("slope_cnt0_x,slope_cnt0_y: %d, %d", slope_cnt0_x, slope_cnt0_y);
        TRACE_NOP;
    };
}
#endif //1
#if 0 //for test
ATTR_BACKEND_RAMFUNC
static void get_input_buffer(DEF_PD_INFO *InBuf)
{
	int i, lineBufIdx, pd, led, ledMask, maxInLineBufLed; //j, 
	uint8_t *inlineBuf_pd, *inlineBuf_led;
#ifdef USED_LINE_METHOD_OLD //nsmoon@200119
	uint8_t *inlineStat_pd;
#endif
	uint8_t *inlineUsed2_pd;
	int8_t *offsetTbl;
#ifdef GET_SLOPE_0
	uint8_t slope0 = 0, slope1p = 0, slope1m = 0;
#endif
#if (DEBUG_get_input_buffer > 1)
	int show_line = 1; // BG_show_line_all;
	int line_color = (MY_COLOR - 4);
#endif

#ifdef SHOW_DEBUG_SIZE
    BG_debug_misc[0] = 0; //+x
    BG_debug_misc[1] = 0; //-x
    BG_debug_misc[2] = 0; //x
    BG_debug_misc[3] = 0; //+y
    BG_debug_misc[4] = 0; //-y
    BG_debug_misc[5] = 0; //y
#endif

#if 0
	for (i = 0; i < MAX_IN_LINE_BUF_PD_X; i++) {
		for (j = 0; j < MAX_IN_LINE_BUF_LED_X; j++) {
			BS_inlineBuf_x_pd[i][j] = 0;
			BS_inlineBuf_x_led[i][j] = 0;
		}
	}
	for (i = 0; i < MAX_IN_LINE_BUF_PD_Y; i++) {
		for (j = 0; j < MAX_IN_LINE_BUF_LED_Y; j++) {
			BS_inlineBuf_y_pd[i][j] = 0;
			BS_inlineBuf_y_led[i][j] = 0;
		}
	}
#endif
#ifdef USE_SENSOR_LIST //for test, nsmoon@200327
    sensor_list_init();
#endif

	//x-axis
	inlineBuf_pd = IN_LINE_BUF_ADDR_X_PD;
	inlineBuf_led = IN_LINE_BUF_ADDR_X_LED;
#ifdef USED_LINE_METHOD_OLD //nsmoon@200119
	inlineStat_pd = LINE_STAT_BUF_ADDR_X;
#endif
	inlineUsed2_pd = LINE_USED2_BUF_ADDR_X;
	offsetTbl = &BS_offsetTblX[0];
	IS_DEBUG_FLAG{ TRACE_GIB("X inlineBuf_led in get_input_buffer: %x %d ", inlineBuf_led, InBuf->hor_len); };
	//IS_DEBUG_FLAG{ TRACE("X inlineBuf_led in get_input_buffer: %x %d ", inlineBuf_led, InBuf->hor_len); }

    maxInLineBufLed = MAX_IN_LINE_BUF_LED_X;
	memset(inlineBuf_pd, 0, MAX_IN_LINE_BUF_PD_X * MAX_IN_LINE_BUF_LED_X);
	memset(inlineBuf_led, 0, MAX_IN_LINE_BUF_PD_X * MAX_IN_LINE_BUF_LED_X);
#ifdef USED_LINE_METHOD_OLD //nsmoon@200119
	memset(inlineStat_pd, 0, MAX_LINE_STAT_BUF_PD_X * MAX_LINE_STAT_BUF_LED_X);
#else
    memset(inlineUsed2_pd, 0, MAX_LINE_USED_BUF_PD_X * MAX_LINE_USED_BUF_LED_X);
#endif

	for (i = 0; i < InBuf->hor_len; i++) {
#if 0 //(USE_ADC_DATA == 1), for debugging
		int8_t adcTmp = InBuf->hor_touch_pd[i].adc;
		//if (adcTmp > 40)
			TRACE("X%d\r\n", adcTmp);
#endif
		pd = InBuf->hor_touch_pd[i].pd;
		if (InBuf->hor_touch_pd[i].led >= BS_offsetTblLenX) {
			TRACE_ERROR("ERROR! X invalid led offset %d [%d]", InBuf->hor_touch_pd[i].led, BG_frame_no);
			continue;
		}
		led = InBuf->hor_touch_pd[i].pd + offsetTbl[InBuf->hor_touch_pd[i].led];
		if (led < 0 || led >= BS_maxHorPdNum) {
		    TRACE_ERROR("ERROR! X invalid led %d/%d/%d [%d]", pd, led, BS_maxHorPdNum, BG_frame_no);
			continue;
		}
#ifdef USE_SENSOR_LIST //for test, nsmoon@200327
        sensor_list_add(ENUM_HOR_X, ENUM_PD, pd);
        sensor_list_add(ENUM_HOR_X, ENUM_LED, led);
#endif

#ifdef GET_SLOPE_0
		if (InBuf->hor_touch_pd[i].led == MAX_X_SLOPE_VAL) {
			slope0++;
		}
		else if (InBuf->hor_touch_pd[i].led == (MAX_X_SLOPE_VAL - 1)) {
			slope1m++;
		}
		else if (InBuf->hor_touch_pd[i].led == (MAX_X_SLOPE_VAL + 1)) {
			slope1p++;
		}
#endif
#ifdef SHOW_DEBUG_SIZE
        int slopeValX = led - pd;
        if (slopeValX > 0) {
            BG_debug_misc[0]++; //+x
        }
        else if (slopeValX < 0){
            BG_debug_misc[1]++; //-x
        }
        else {
            BG_debug_misc[2]++; //x
        }
#endif
#if (DEBUG_get_input_buffer > 1)
        if (show_line) DEBUG_SHOW_LINE_PD_LED(ENUM_HOR_X, pd, led, line_color);
#endif
        //x-pd table
		lineBufIdx = GET_IN_LINE_IDX(pd, led);
		ledMask = GET_LED_BIT_MASK(led);
		inlineBuf_pd[lineBufIdx] |= (uint8_t)ledMask;
#ifdef USED_LINE_METHOD_OLD //nsmoon@200119
		inlineStat_pd[lineBufIdx] |= (uint8_t)ledMask;
#else
        inlineUsed2_pd[lineBufIdx] |= (uint8_t)ledMask;
#endif
#if 0 //for testing
		if (pd == 28) //for testing
#endif
			IS_DEBUG_FLAG{ TRACE_GIB("--pd,led,lineBufIdx,ledMask:, %d, %d, %d, %x, (%x)", pd, led, lineBufIdx, ledMask, inlineBuf_pd[lineBufIdx]); };

        //x-led table
		lineBufIdx = GET_IN_LINE_IDX(led, pd);
		ledMask = GET_LED_BIT_MASK(pd);
		inlineBuf_led[lineBufIdx] |= (uint8_t)ledMask;
		IS_DEBUG_FLAG{ TRACE_GIB("--led,pd,lineBufIdx,ledMask:, %d, %d, %d, %x, (%x)", led, pd, lineBufIdx, ledMask, inlineBuf_led[lineBufIdx]); };
		TRACE_NOP;
	} //for (i = 0; i < InBuf->hor_len; i++)
#ifdef GET_SLOPE_0
	slope_cnt0_x = GET_MAX(slope1m, slope1p);
	slope_cnt0_x = GET_MAX(slope0, slope_cnt0_x);
    IS_DEBUG_FLAG{ TRACE_GIB0("X:slope0: %d %d %d", slope0, slope1p, slope1m); };
    //TRACE_RELEASE("X:slope0: %d %d %d", slope0, slope1p, slope1m);
#endif

	//y-axis
	inlineBuf_pd = IN_LINE_BUF_ADDR_Y_PD;
	inlineBuf_led = IN_LINE_BUF_ADDR_Y_LED;
#ifdef USED_LINE_METHOD_OLD //nsmoon@200119
	inlineStat_pd = LINE_STAT_BUF_ADDR_Y;
#endif
	inlineUsed2_pd = LINE_USED2_BUF_ADDR_Y;
    IS_DEBUG_FLAG{ TRACE_GIB("Y inlineBuf_led in get_input_buffer: %x %d ", inlineBuf_led, InBuf->ver_len); };
    //IS_DEBUG_FLAG{ TRACE("Y inlineBuf_led in get_input_buffer: %x %d ", inlineBuf_led, InBuf->ver_len); }

	offsetTbl = &BS_offsetTblY[0];
	maxInLineBufLed = MAX_IN_LINE_BUF_LED_Y;
	memset(inlineBuf_pd, 0, MAX_IN_LINE_BUF_PD_Y * MAX_IN_LINE_BUF_LED_Y);
	memset(inlineBuf_led, 0, MAX_IN_LINE_BUF_PD_Y * MAX_IN_LINE_BUF_LED_Y);
#ifdef USED_LINE_METHOD_OLD //nsmoon@200119
	memset(inlineStat_pd, 0, MAX_LINE_STAT_BUF_PD_Y * MAX_LINE_STAT_BUF_LED_Y);
#endif
	memset(inlineUsed2_pd, 0, MAX_LINE_USED_BUF_PD_Y * MAX_LINE_USED_BUF_LED_Y);

	for (i = 0; i < InBuf->ver_len; i++) {
#if 0 //(USE_ADC_DATA == 1), for debugging
		int8_t adcTmp = InBuf->ver_touch_pd[i].adc;
		//if (adcTmp > 40)
			TRACE("Y%d\r\n", adcTmp);
#endif
		pd = InBuf->ver_touch_pd[i].pd;
		if (InBuf->ver_touch_pd[i].led >= BS_offsetTblLenY) {
			TRACE_ERROR("ERROR! Y invalid led offset %d [%d]", InBuf->ver_touch_pd[i].led, BG_frame_no);
			continue;
		}
		led = InBuf->ver_touch_pd[i].pd + offsetTbl[InBuf->ver_touch_pd[i].led];
		if (led < 0 || led >= BS_maxVerPdNum) {
		    TRACE_ERROR("ERROR! Y invalid led %d/%d/%d [%d]", pd, led, BS_maxVerPdNum, BG_frame_no);
			continue;
		}
#ifdef USE_SENSOR_LIST //for test, nsmoon@200327
        sensor_list_add(ENUM_VER_Y, ENUM_PD, pd);
        sensor_list_add(ENUM_VER_Y, ENUM_LED, led);
#endif
#ifdef GET_SLOPE_0
		if (InBuf->ver_touch_pd[i].led == MAX_Y_SLOPE_VAL) {
			slope0++;
		}
		else if (InBuf->ver_touch_pd[i].led == (MAX_Y_SLOPE_VAL - 1)) {
			slope1m++;
		}
		else if (InBuf->ver_touch_pd[i].led == (MAX_Y_SLOPE_VAL + 1)) {
			slope1p++;
		}
#endif
#ifdef SHOW_DEBUG_SIZE
        int slopeValY = led - pd;
        if (slopeValY > 0) {
            BG_debug_misc[3]++; //+y
        }
        else if (slopeValY < 0){
            BG_debug_misc[4]++; //-y
        }
        else {
            BG_debug_misc[5]++; //y
        }
#endif
#if (DEBUG_get_input_buffer > 1)
        if (show_line) DEBUG_SHOW_LINE_PD_LED(ENUM_VER_Y, pd, led, line_color);
#endif
        //y-pd-table
		lineBufIdx = GET_IN_LINE_IDX(pd, led);
		ledMask = GET_LED_BIT_MASK(led);

		inlineBuf_pd[lineBufIdx] |= (uint8_t)ledMask;
#ifdef USED_LINE_METHOD_OLD //nsmoon@200119
		inlineStat_pd[lineBufIdx] |= (uint8_t)ledMask;
#endif
		inlineUsed2_pd[lineBufIdx] |= (uint8_t)ledMask;
		IS_DEBUG_FLAG{ TRACE_GIB("==pd,led,lineBufIdx,ledMask:, %d, %d, %d, %x", pd, led, lineBufIdx, ledMask); };

        //y-led-table
		lineBufIdx = GET_IN_LINE_IDX(led, pd);
		ledMask = GET_LED_BIT_MASK(pd);
		inlineBuf_led[lineBufIdx] |= (uint8_t)ledMask;
		IS_DEBUG_FLAG{ TRACE_GIB("==led,pd,lineBufIdx,ledMask:, %d, %d, %d, %x", led, pd, lineBufIdx, ledMask); };
		TRACE_NOP;
	} //for (i = 0; i < InBuf->ver_len; i++)
#ifdef GET_SLOPE_0
	slope_cnt0_y = GET_MAX(slope1m, slope1p);
	slope_cnt0_y = GET_MAX(slope0, slope_cnt0_y);
    IS_DEBUG_FLAG{ TRACE_GIB0("Y:slope0: %d %d %d", slope0, slope1p, slope1m); };
    //TRACE_RELEASE("Y:slope0: %d %d %d", slope0, slope1p, slope1m);
#endif

#if 0 //for testing
	led = 29;
	pd = 0;
	inlineBuf_pd = IN_LINE_BUF_ADDR_X_LED;
	maxInLineBufLed = MAX_IN_LINE_BUF_LED_X;
	lineBufIdx = GET_IN_LINE_IDX(led, pd);
	ledMask = GET_LED_BIT_MASK(pd);
	IS_DEBUG_FLAG{ TRACE_GIB("..led,pd,lineBufIdx,ledMask: %d %d %d %x (%x)(%x)", led, pd, lineBufIdx, ledMask, inlineBuf_pd[lineBufIdx], BS_inlineBuf_x_led[led][pd]); };
#endif
#ifdef USE_SENSOR_LIST //for test, nsmoon@200327
    IS_DEBUG_FLAG{ TRACE("BS_pd_x_cnt:,%d,%d,%d,%d", BS_pd_x_cnt, BS_led_x_cnt, BS_pd_y_cnt, BS_led_y_cnt); };
#endif
#if 0 //def USE_SENSOR_LIST //for test, nsmoon@200327
    int listNo, senCnt;
    uint8_t *senList;
    for (listNo = 0; listNo < 4; listNo++) {
        if (listNo == 0) {
            senList = &BS_pd_x[0];
            senCnt = BS_pd_x_cnt;
        }
        else if (listNo == 1) {
            senList = &BS_led_x[0];
            senCnt = BS_led_x_cnt;
        }
        else if (listNo == 2) {
            senList = &BS_pd_y[0];
            senCnt = BS_pd_y_cnt;
        }
        else {
            senList = &BS_led_y[0];
            senCnt = BS_led_y_cnt;
        }
        for (i = 0; i < senCnt; i++) {
            TRACENR("%d ", senList[i]);
        }
        TRACE("=>%d (%d)", senCnt, listNo);
    }
    TRACE_NOP;
#endif
	IS_DEBUG_FLAG{
	    //TRACE("slope_cnt0_x,slope_cnt0_y: %d, %d", slope_cnt0_x, slope_cnt0_y);
        TRACE_NOP;
    };
}
#endif //0

#define REMAINED_MULTI_INITIAL_HALFSIZE		12.0f //10.0f //5.0f
#define REMAINED_MULTI_INITIAL_HALFSIZE2	5.0f
#define MULTI_VIRTUAL_LINE_WIDTH			80.0f

#ifdef MULTI_SKIP_USED_VB //nsmoon@210218
static int save_initial_polygon(initial_polygon_t *initial_polygon, int *initialTouchCnt, vec_t pR, pos_min_max_t minMaxPos, initial_line_no_t initial_lineNo)
{
	side_type_t	side;
	sen_type_t senX, senY;

	if (*initialTouchCnt < MAX_RESULT_POLYGON_NO) {
        initial_polygon[*initialTouchCnt].pos.minX = minMaxPos.minX;
        initial_polygon[*initialTouchCnt].pos.maxX = minMaxPos.maxX;
        initial_polygon[*initialTouchCnt].pos.minY = minMaxPos.minY;
        initial_polygon[*initialTouchCnt].pos.maxY = minMaxPos.maxY;
		side = BS_getSideFromPos(&pR);
		initial_polygon[*initialTouchCnt].side = side;
		if (side == ENUM_BOT_RHT) {
			senX = ENUM_LED;
			senY = ENUM_PD;
			//bdX = ENUM_BOT_BD;
			//bdY = ENUM_RHT_BD;
		}
		else if (side == ENUM_TOP_RHT) {
			senX = ENUM_PD;
			senY = ENUM_PD;
			//bdX = ENUM_TOP_BD;
			//bdY = ENUM_RHT_BD;
		}
		else if (side == ENUM_TOP_LFT) {
			senX = ENUM_PD;
			senY = ENUM_LED;
			//bdX = ENUM_TOP_BD;
			//bdY = ENUM_LFT_BD;
		}
		else {
			//(side == ENUM_BOT_LFT)
			senX = ENUM_LED;
			senY = ENUM_LED;
			//bdX = ENUM_BOT_BD;
			//bdY = ENUM_LFT_BD;
		}
        initial_polygon[*initialTouchCnt].ep.minX = (uint8_t)BS_getSenNumPos(ENUM_HOR_X, senX, minMaxPos.minX, 0); //0:before
        initial_polygon[*initialTouchCnt].ep.maxX = (uint8_t)BS_getSenNumPos(ENUM_HOR_X, senX, minMaxPos.maxX, 1); //1:after
        initial_polygon[*initialTouchCnt].ep.minY = (uint8_t)BS_getSenNumPos(ENUM_VER_Y, senY, minMaxPos.minY, 0); //0:before
        initial_polygon[*initialTouchCnt].ep.maxY = (uint8_t)BS_getSenNumPos(ENUM_VER_Y, senY, minMaxPos.maxY, 1); //1:after
		initial_polygon[*initialTouchCnt].firstX = 1;
		initial_polygon[*initialTouchCnt].firstY = 1;
		initial_polygon[*initialTouchCnt].stat = 0; //init
        initial_polygon[*initialTouchCnt].xNo = initial_lineNo.xNo;
        initial_polygon[*initialTouchCnt].yNo = initial_lineNo.yNo;
		//DEBUG_SHOW_MIN_MAX(&initial_polygon[initialTouchCnt].pos, 3, 1);
		(*initialTouchCnt)++;
	}
	else {
        TRACE_ERROR("ERROR! save_initial_polygon... invalid initialTouchCnt");
		return -1; //mem-error
	}
	return 0;
}
#else
static int save_initial_polygon(initial_polygon_t *initial_polygon, int *initialTouchCnt, vec_t pR, pos_min_max_t minMaxPos)
{
	side_type_t	side;
	sen_type_t senX, senY;

	if (*initialTouchCnt < MAX_RESULT_POLYGON_NO) {
		initial_polygon[*initialTouchCnt].pos.minX = minMaxPos.minX;
		initial_polygon[*initialTouchCnt].pos.maxX = minMaxPos.maxX;
		initial_polygon[*initialTouchCnt].pos.minY = minMaxPos.minY;
		initial_polygon[*initialTouchCnt].pos.maxY = minMaxPos.maxY;
		side = BS_getSideFromPos(&pR);
		initial_polygon[*initialTouchCnt].side = side;
		if (side == ENUM_BOT_RHT) {
			senX = ENUM_LED;
			senY = ENUM_PD;
			//bdX = ENUM_BOT_BD;
			//bdY = ENUM_RHT_BD;
		}
		else if (side == ENUM_TOP_RHT) {
			senX = ENUM_PD;
			senY = ENUM_PD;
			//bdX = ENUM_TOP_BD;
			//bdY = ENUM_RHT_BD;
		}
		else if (side == ENUM_TOP_LFT) {
			senX = ENUM_PD;
			senY = ENUM_LED;
			//bdX = ENUM_TOP_BD;
			//bdY = ENUM_LFT_BD;
		}
		else {
			//(side == ENUM_BOT_LFT)
			senX = ENUM_LED;
			senY = ENUM_LED;
			//bdX = ENUM_BOT_BD;
			//bdY = ENUM_LFT_BD;
		}
		initial_polygon[*initialTouchCnt].ep.minX = (uint8_t)BS_getSenNumPos(ENUM_HOR_X, senX, minMaxPos.minX, 0); //0:before
        initial_polygon[*initialTouchCnt].ep.maxX = (uint8_t)BS_getSenNumPos(ENUM_HOR_X, senX, minMaxPos.maxX, 1); //1:after
		initial_polygon[*initialTouchCnt].ep.minY = (uint8_t)BS_getSenNumPos(ENUM_VER_Y, senY, minMaxPos.minY, 0); //0:before
        initial_polygon[*initialTouchCnt].ep.maxY = (uint8_t)BS_getSenNumPos(ENUM_VER_Y, senY, minMaxPos.maxY, 1); //1:after
		initial_polygon[*initialTouchCnt].firstX = 1;
		initial_polygon[*initialTouchCnt].firstY = 1;
		initial_polygon[*initialTouchCnt].stat = 0; //init
		//DEBUG_SHOW_MIN_MAX(&initial_polygon[initialTouchCnt].pos, 3, 1);
		(*initialTouchCnt)++;
	}
	else {
        TRACE_ERROR("ERROR! save_initial_polygon... invalid initialTouchCnt");
		return -1; //mem-error
	}
	return 0;
}
#endif


#if 0 //reserved, do not remove
#define INITIAL_OVERLAP_TOL		EPSILON
#define MULTI_OVERLAP_DIST_3	8.0f
static int is_overlap_min_max_initial_polygon(int stIdx, int edIdx, pos_min_max_t mM, int mode)
{
	//mode: 0:include all, 1:exclude removed touch
    int ol_x1, ol_x2, ol_x3, ol_x4, ol_y1, ol_y2, ol_y3, ol_y4;
	initial_polygon_t *initial = &BS_initial_polygon[0];
	int i;
	float distX, distY, distMax = MULTI_OVERLAP_DIST_3;
	vec_t cent, cent2;
	cent.x = (mM.minX + mM.maxX) * 0.5f;
	cent.y = (mM.minY + mM.maxY) * 0.5f;

	//TRACE("cent.x,cent.y=%0.1f, %0.1f", cent.x, cent.y);
	for (i = stIdx; i < edIdx; i++) {
		//DEBUG_SHOW_MIN_MAX(&info[i].mM, MY_COLOR, 1);
		if (mode) {
			if (initial[i].stat == 1) continue; //removed
		}
#if 1 //for test
		cent2.x = (initial[i].pos.minX + initial[i].pos.maxX) * 0.5f;
		cent2.y = (initial[i].pos.minY + initial[i].pos.maxY) * 0.5f;
		distX = GET_ABS(cent.x - cent2.x);
		distY = GET_ABS(cent.y - cent2.y);
		//TRACE("distX,distY=%0.1f %0.1f (%0.1f)", distX, distY, distMax);
		if (distX < distMax && distY < distMax) {
			return 1; //overlapped
		}
#else
		if (initial[i].pos.minX > initial[i].pos.maxX || mM.minX > mM.maxX) {
			TRACE_ERROR("ERROR! invalid min,max: %f,%f %f,%f", initial[i].pos.minX, initial[i].pos.maxX, mM.minX, mM.maxX);
		}
		ol_x1 = (mM.minX > (initial[i].pos.minX - INITIAL_OVERLAP_TOL) && mM.minX < (initial[i].pos.maxX + INITIAL_OVERLAP_TOL));
		ol_x2 = (mM.maxX >(initial[i].pos.minX - INITIAL_OVERLAP_TOL) && mM.maxX < (initial[i].pos.maxX + INITIAL_OVERLAP_TOL));
		ol_y1 = (mM.minY >(initial[i].pos.minY - INITIAL_OVERLAP_TOL) && mM.minY < (initial[i].pos.maxY + INITIAL_OVERLAP_TOL));
		ol_y2 = (mM.maxY >(initial[i].pos.minY - INITIAL_OVERLAP_TOL) && mM.maxY < (initial[i].pos.maxY + INITIAL_OVERLAP_TOL));
        ol_x3 = (initial[i].pos.minX > (mM.minX - INITIAL_OVERLAP_TOL) && initial[i].pos.minX < (mM.maxX + INITIAL_OVERLAP_TOL));
        ol_x4 = (initial[i].pos.maxX >(mM.minX - INITIAL_OVERLAP_TOL) && initial[i].pos.maxX < (mM.maxX + INITIAL_OVERLAP_TOL));
        ol_y3 = (initial[i].pos.minY >(mM.minY - INITIAL_OVERLAP_TOL) && initial[i].pos.minY < (mM.maxY + INITIAL_OVERLAP_TOL));
        ol_y4 = (initial[i].pos.maxY >(mM.minY - INITIAL_OVERLAP_TOL) && initial[i].pos.maxY < (mM.maxY + INITIAL_OVERLAP_TOL));
        if ((ol_x1 || ol_x2 || ol_x3 || ol_x4) && (ol_y1 || ol_y2 || ol_y3 || ol_y4)) {
			//TRACE("overlapped~~");
			return 1;
		}
#endif
	}

	return 0; //not-overlapped
}
#endif //1

#ifdef MULTI_VIRTUAL_INITIALL_MERGE_ENABLE
#define MULTI_OVERLAP_DIST_INITIAL    20.0f

#ifdef MULTI_SKIP_USED_VB //nsmoon@210218
static int remaied_multi_initial_merge(pos_min_max_t *initial_mM, int *initial_mM_cnt, pos_min_max_t mM, int xNo, int yNo)
{
    initial_line_no_t *initial_lineNo = &BS_initial_lineNo[0];
    float distX, distY, distMax = MULTI_OVERLAP_DIST_INITIAL;
    vec_t cent0, cent;
    int i;
    int cnt = *initial_mM_cnt;

    cent0.x = (mM.maxX + mM.minX) * 0.5f;
    cent0.y = (mM.maxY + mM.minY) * 0.5f;
    //TRACE("cent0.x,cent0.y=%0.1f, %0.1f", cent0.x, cent0.y);

    for (i = 0; i < cnt; i++) {
        //cent to cent
        cent.x = (initial_mM[i].maxX + initial_mM[i].minX) * 0.5f;
        cent.y = (initial_mM[i].maxY + initial_mM[i].minY) * 0.5f;
        distX = cent.x - cent0.x;
        distY = cent.y - cent0.y;
        //TRACE("distX,distY=%0.1f %0.1f (%0.1f)", distX, distY, distMax);
        if (GET_ABS(distX) < distMax && GET_ABS(distY) < distMax) {
            //adjust min/max
            initial_mM[i].minX = GET_MIN(initial_mM[i].minX, mM.minX);
            initial_mM[i].maxX = GET_MAX(initial_mM[i].maxX, mM.maxX);
            initial_mM[i].minY = GET_MIN(initial_mM[i].minY, mM.minY);
            initial_mM[i].maxY = GET_MAX(initial_mM[i].maxY, mM.maxY);
            //can not check initial line no
            uint8_t xNo_i = initial_lineNo[i].xNo;
            uint8_t yNo_i = initial_lineNo[i].yNo;
            if (xNo_i != xNo) {
                initial_lineNo[i].xNo = xNo_i | INITIAL_VB_MERGED;
            }
            if (yNo_i != yNo) {
                initial_lineNo[i].yNo = yNo_i | INITIAL_VB_MERGED;
            }
            TRACE_MSUV("..save line no=merged(%d) %d %d (%d %d)", i, xNo_i, yNo_i, xNo, yNo);
            return 0; //overlapped, merged
        }
    } //for (i = 0; i < BS_touch_info_multi_cnt; i++)

    if (i < MAX_RESULT_POLYGON_NO) {
        initial_mM[i].minX = mM.minX;
        initial_mM[i].maxX = mM.maxX;
        initial_mM[i].minY = mM.minY;
        initial_mM[i].maxY = mM.maxY;
        initial_lineNo[i].xNo = xNo;
        initial_lineNo[i].yNo = yNo;
        TRACE_MSUV("..save line no=(%d) %d %d", i, xNo, yNo);
        (*initial_mM_cnt) = ++i;
        return 1; //not-overlapped, new
    }
    TRACE_ERROR("ERROR! remaied_multi_initial_merge..invalid cnt %d", cnt);
    return 0; //error
}
#else
static int remaied_multi_initial_merge(pos_min_max_t *initial_mM, int initial_mM_cnt, pos_min_max_t mM)
{
    //includeRemoved: 0:include all, 1:exclude removed touch
    //compareMode: 0:cent-cent, 1:min/max-min/max,
    //compareMode: 2:cent-min/max with size, 3:min/max-cent

    //pos_min_max_t *initial_mM = &BS_initial_minMax[0];
    float distX, distY, distMax = MULTI_OVERLAP_DIST_INITIAL;
    vec_t cent0, cent;
    int i;

    cent0.x = (mM.maxX + mM.minX) * 0.5f;
    cent0.y = (mM.maxY + mM.minY) * 0.5f;
    //TRACE("cent0.x,cent0.y=%0.1f, %0.1f", cent0.x, cent0.y);

    for (i = 0; i < initial_mM_cnt; i++) {
        //cent to cent
        cent.x = (initial_mM[i].maxX + initial_mM[i].minX) * 0.5f;
        cent.y = (initial_mM[i].maxY + initial_mM[i].minY) * 0.5f;
        distX = cent.x - cent0.x;
        distY = cent.y - cent0.y;
        //TRACE("distX,distY=%0.1f %0.1f (%0.1f)", distX, distY, distMax);
        if (GET_ABS(distX) < distMax && GET_ABS(distY) < distMax) {
            initial_mM[i].minX = GET_MIN(initial_mM[i].minX, mM.minX);
            initial_mM[i].maxX = GET_MAX(initial_mM[i].maxX, mM.maxX);
            initial_mM[i].minY = GET_MIN(initial_mM[i].minY, mM.minY);
            initial_mM[i].maxY = GET_MAX(initial_mM[i].maxY, mM.maxY);
            return initial_mM_cnt; //overlapped, merged
        }
    } //for (i = 0; i < BS_touch_info_multi_cnt; i++)

    if (i < MAX_RESULT_POLYGON_NO) {
        initial_mM[i].minX = mM.minX;
        initial_mM[i].maxX = mM.maxX;
        initial_mM[i].minY = mM.minY;
        initial_mM[i].maxY = mM.maxY;
        return (++i); //not-overlapped, new
    }
    TRACE_ERROR("ERROR! invalid initial_mM_cnt %d", initial_mM_cnt);
    return initial_mM_cnt; //error
}
#endif
#endif

#define DEBUG_remaied_multi_initial     0
static int remaied_multi_initial(void)
{
	uint16_t *remLineX = &BS_remained_x[0];
	int remLineCntX = BS_remained_x_cnt;
	uint16_t *remLineY = &BS_remained_y[0];
	int remLineCntY = BS_remained_y_cnt;
	int line_idx_x, line_idx_y;
	initial_polygon_t *initial_polygon = &BS_initial_polygon[0];
    pos_min_max_t *initial_mM = &BS_initial_minMax[0];
    int initialTouchCnt, initial_mM_cnt;
    initial_line_a_t *initial_line_x = &BS_initial_line_a_x[0];
    initial_line_a_t *initial_line_y = &BS_initial_line_a_y[0];
	int pdX, ledX, pdY, ledY;
	float *pdXpos = &BS_pd_pos_x[0];
	float *ledXpos = &BS_led_pos_x[0];
	float *pdYpos = &BS_pd_pos_y[0];
	float *ledYpos = &BS_led_pos_y[0];
	vec_t p0, p1, p2, p3, pR;
    vec_t cent;
	pos_min_max_t minMaxPos;
	int retTmp; 
    int i; 
    float initial_half_size = REMAINED_MULTI_INITIAL_HALFSIZE;
#ifdef MULTI_SKIP_USED_VB //nsmoon@210218
    initial_line_no_t *initial_lineNo = &BS_initial_lineNo[0];
    TRACE_MSUV("****BG_frame_no=%d ", BG_frame_no);
#endif
	IS_DEBUG_FLAG{
		TRACE_NOP;
	};

	retTmp = BS_fine_get_initial_ep5(ENUM_HOR_X, remLineX, &remLineCntX);
	if (retTmp) {
		TRACE_ERROR("ERROR! remaied_multi_initial..0 mem over flow");
		return -1; //mem error
	}

	retTmp = BS_fine_get_initial_ep5(ENUM_VER_Y, remLineY, &remLineCntY);
	if (retTmp) {
		TRACE_ERROR("ERROR! remaied_multi_initial..0 mem over flow");
		return -1; //mem error
	}

	//////////////////////////////////
	//make real initial polygon
	//////////////////////////////////
    initial_mM_cnt = 0;
	for (line_idx_x = 0; line_idx_x < BS_initial_line_a_x_cnt; line_idx_x++) {
		pdX = initial_line_x[line_idx_x].line.pd;
		ledX = initial_line_x[line_idx_x].line.led;
#ifdef DEBUG_EP_TEST //nsmoon@210216
        //TRACE("line_idx_x=%d %d %d", line_idx_x, pdX, ledX);
        DEBUG_SHOW_LINE_PD_LED(ENUM_HOR_X, pdX, ledX, line_idx_x/*0*/);
#endif
#if (DEBUG_remaied_multi_initial > 0)
        IS_DEBUG_FLAG{ DEBUG_SHOW_LINE_PD_LED(ENUM_HOR_X, pdX, ledX, 0); }; //initial line
#endif
		p0.x = ledXpos[ledX];
		p0.y = BS_sensor_zero_y;
		p1.x = pdXpos[pdX];
		p1.y = BS_sensor_end_y;

		for (line_idx_y = 0; line_idx_y < BS_initial_line_a_y_cnt; line_idx_y++) {
			pdY = initial_line_y[line_idx_y].line.pd;
			ledY = initial_line_y[line_idx_y].line.led;
#ifdef DEBUG_EP_TEST //nsmoon@210216
            //TRACE("line_idx_y=%d %d %d", line_idx_y, pdY, ledY);
            DEBUG_SHOW_LINE_PD_LED(ENUM_VER_Y, pdY, ledY, line_idx_y/*0*/);
#endif
#if (DEBUG_remaied_multi_initial > 0)
            IS_DEBUG_FLAG{ DEBUG_SHOW_LINE_PD_LED(ENUM_VER_Y, pdY, ledY, 0); }; //initial line
#endif
			p2.y = ledYpos[ledY];
			p2.x = BS_sensor_end_x;
			p3.y = pdYpos[pdY];
			p3.x = BS_sensor_zero_x;
			if (BS_line_intersection(&p0, &p1, &p2, &p3, &pR)) {
#if (DEBUG_remaied_multi_initial > 1)
                DEBUG_SHOW_POS(&pR, 1.0f, 1.0f, MY_COLOR);
#endif
                minMaxPos.minX = pR.x - initial_half_size;
                minMaxPos.maxX = pR.x + initial_half_size;
                minMaxPos.minY = pR.y - initial_half_size;
                minMaxPos.maxY = pR.y + initial_half_size;
#if (DEBUG_remaied_multi_initial > 1)
                DEBUG_SHOW_MIN_MAX(&minMaxPos, MY_COLOR, 1);
#endif
                if (BS_multi_is_overlap_min_max_tp(minMaxPos, 1, 2)) { //1:exclude removed
                    continue;
                }
#ifdef MULTI_VIRTUAL_INITIALL_MERGE_ENABLE
  #ifdef MULTI_SKIP_USED_VB //nsmoon@210218
                remaied_multi_initial_merge(initial_mM, &initial_mM_cnt, minMaxPos, line_idx_x, line_idx_y);
  #else
                initial_mM_cnt = remaied_multi_initial_merge(initial_mM, initial_mM_cnt, minMaxPos);
  #endif
#else
                if (initial_mM_cnt < MAX_RESULT_POLYGON_NO) {
                initial_mM[initial_mM_cnt].minX = minMaxPos.minX;
                initial_mM[initial_mM_cnt].maxX = minMaxPos.maxX;
                initial_mM[initial_mM_cnt].minY = minMaxPos.minY;
                initial_mM[initial_mM_cnt].maxY = minMaxPos.maxY;
                initial_mM_cnt++;
                }
                else {
                    TRACE_ERROR("ERROR! invalid initial_mM_cnt %d", initial_mM_cnt);
                }
#endif
			}
			else {
				//TRACE_ERROR("ERROR! remaied_multi_initial..no inst");
				TRACE_NOP;
			}
		} //for (line_idx_y = 0; line_idx_y < BS_initial_line_y_cnt; line_idx_y++)
	} //for (line_idx_x = 0; line_idx_x < BS_initial_line_x_cnt; line_idx_x++)

    initialTouchCnt = 0;
    for (i = 0; i < initial_mM_cnt; i++) {
        cent.x = (initial_mM[i].minX + initial_mM[i].maxX) * 0.5f;
        cent.y = (initial_mM[i].minY + initial_mM[i].maxY) * 0.5f;
#ifdef MULTI_SKIP_USED_VB //nsmoon@210218
        retTmp = save_initial_polygon(initial_polygon, &initialTouchCnt, cent, initial_mM[i], initial_lineNo[i]);
#else
        retTmp = save_initial_polygon(initial_polygon, &initialTouchCnt, cent, initial_mM[i]);
#endif
        if (retTmp < 0) {
            break; //mem-error
        }
    }

#if defined(DEBUG_FUNCTION_ENABLE_ALL) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
	IS_DEBUG_FLAG
	{
        int m;
		//SEND_POLY(0, 0x21, MY_COLOR);
        for (m = 0; m < initialTouchCnt; m++) {
            DEBUG_SHOW_MIN_MAX(&initial_polygon[m].pos, MY_COLOR-2, 1);
			TRACE_NOP;
		}
	};
#endif

	BS_initial_polygon_cnt = initialTouchCnt;
	return initialTouchCnt;
}

#if 0 //reserved
static int remaied_multi_initial2()
{
    uint16_t *remLineX = &BS_remained_x[0];
    int remLineCntX = BS_remained_x_cnt;
    uint16_t *remLineY = &BS_remained_y[0];
    int remLineCntY = BS_remained_y_cnt;
    int line_idx_x, line_idx_y;
    initial_line_a_t *initial_line_x = &BS_initial_line_a_x[0];
    initial_line_a_t *initial_line_y = &BS_initial_line_a_y[0];
    int pdX, ledX, pdY, ledY;
    float *pdXpos = &BS_pd_pos_x[0];
    float *ledXpos = &BS_led_pos_x[0];
    float *pdYpos = &BS_pd_pos_y[0];
    float *ledYpos = &BS_led_pos_y[0];
    //pos_min_max_t minMaxPos;

    float initial_half_size = REMAINED_MULTI_INITIAL_HALFSIZE2;
    ep_buf_t *epaX = &BS_edge_pattern_x[0];
    ep_buf_t *epaY = &BS_edge_pattern_y[0];
    int maxPdNumX = BS_maxHorPdNum - 1;
    int maxPdNumY = BS_maxVerPdNum - 1;
    float padLedRht, padPdRht, padLedLft, padPdLft;
    int len;
    int retTmp;

    retTmp = BS_fine_get_initial_ep5(ENUM_HOR_X, remLineX, &remLineCntX);
    if (retTmp) {
        TRACE_ERROR("ERROR! remaied_multi_initial2..0 mem over flow");
        return -1; //mem error
    }
    retTmp = BS_fine_get_initial_ep5(ENUM_VER_Y, remLineY, &remLineCntY);
    if (retTmp) {
        TRACE_ERROR("ERROR! remaied_multi_initial2..1 mem over flow");
        return -1; //mem error
    }

    //x-axis
    len = 0;
    for (line_idx_x = 0; line_idx_x < BS_initial_line_a_x_cnt; line_idx_x++) {
        pdX = initial_line_x[line_idx_x].line.pd;
        ledX = initial_line_x[line_idx_x].line.led;
        if (len < MAX_INIT_EP_COUNT) {
            epaX->ep[len].s1 = ledX;
            epaX->ep[len].e1 = pdX;
            epaX->ep[len].s2 = ledX;
            epaX->ep[len].e2 = pdX;

            padLedRht = 0; padLedLft = 0;
            padPdRht = 0; padPdLft = 0;
#if 1 //for test
            if ((ledX - 1) >= 0) {
                padLedRht = MULTI_INITIAL_POLY_PAD_FIXED;
            }
            if ((pdX - 1) >= 0) {
                padPdRht = MULTI_INITIAL_POLY_PAD_FIXED;
            }
            if ((ledX + 1) < maxPdNumX) {
                padLedLft = MULTI_INITIAL_POLY_PAD_FIXED;
            }
            if ((pdX + 1) < maxPdNumX) {
                padPdLft = MULTI_INITIAL_POLY_PAD_FIXED;
            }
#else
            if ((ledX - 1) >= 0) {
                padLedRht = (ledXpos[ledX] - ledXpos[ledX - 1]) - MULTI_INITIAL_POLY_PAD_DIST;
            }
            if ((pdX - 1) >= 0) {
                padPdRht = (pdXpos[pdX] - pdXpos[pdX - 1]) - MULTI_INITIAL_POLY_PAD_DIST;
            }
            if ((ledX + 1) < maxPdNumX) {
                padLedLft = (ledXpos[ledX + 1] - ledXpos[ledX]) - MULTI_INITIAL_POLY_PAD_DIST;
            }
            if ((pdX + 1) < maxPdNumX) {
                padPdLft = (pdXpos[pdX + 1] - pdXpos[pdX]) - MULTI_INITIAL_POLY_PAD_DIST;
            }
#endif

            epaX->poly[len][0].vert.x = ledXpos[ledX] - padLedRht; //bot-rht
            epaX->poly[len][1].vert.x = pdXpos[pdX] - padPdRht;  //top-rht
            epaX->poly[len][2].vert.x = pdXpos[pdX] + padPdLft;  //top-lft
            epaX->poly[len][3].vert.x = ledXpos[ledX] + padLedLft; //bot-lft
            epaX->poly[len][0].vert.y = BS_sensor_zero_y; //bot-rht
            epaX->poly[len][1].vert.y = BS_sensor_end_y;  //top-rht
            epaX->poly[len][2].vert.y = BS_sensor_end_y;  //top-lft
            epaX->poly[len][3].vert.y = BS_sensor_zero_y; //bot-lft
            epaX->poly[len][0].edgeId = len;
            epaX->poly[len][1].edgeId = NO_CLIPPING_EDGEID;
            epaX->poly[len][2].edgeId = len;
            epaX->poly[len][3].edgeId = NO_CLIPPING_EDGEID;
#if 0 //(DEBUG_remaied_multi_initial2 > 1)
            IS_DEBUG_FLAG{
                //if (BG_show_line_init)
                {
                    DEBUG_SHOW_POLY(&epaX->poly[len][0], 4, MY_COLOR);
                    TRACE_NOP;
                }
            };
#endif
            len++;
        }
        else {
            TRACE_ERROR("ERROR! remaied_multi_initial2..3 invalid len");
            return -1; //mem error
        }
    } //for (line_idx_x = 0; line_idx_x < BS_initial_line_x_cnt; line_idx_x++)
    epaX->len = len;

    //y-axis
    len = 0;
    for (line_idx_y = 0; line_idx_y < BS_initial_line_a_y_cnt; line_idx_y++) {
        pdY = initial_line_y[line_idx_y].line.pd;
        ledY = initial_line_y[line_idx_y].line.led;
        if (len < MAX_INIT_EP_COUNT) {
            epaY->ep[len].s1 = ledY;
            epaY->ep[len].e1 = pdY;
            epaY->ep[len].s2 = ledY;
            epaY->ep[len].e2 = pdY;

            padLedRht = 0; padLedLft = 0;
            padPdRht = 0; padPdLft = 0;
#if 1 //for test
            if ((ledY - 1) >= 0) {
                padLedRht = MULTI_INITIAL_POLY_PAD_FIXED;
            }
            if ((pdY - 1) >= 0) {
                padPdRht = MULTI_INITIAL_POLY_PAD_FIXED;
            }
            if ((ledY + 1) < maxPdNumY) {
                padLedLft = MULTI_INITIAL_POLY_PAD_FIXED;
            }
            if ((pdY + 1) < maxPdNumY) {
                padPdLft = MULTI_INITIAL_POLY_PAD_FIXED;
            }
#else
            if ((ledY - 1) >= 0) {
                padLedRht = (ledYpos[ledY] - ledYpos[ledY - 1]) - MULTI_INITIAL_POLY_PAD_DIST;
            }
            if ((pdY - 1) >= 0) {
                padPdRht = (pdYpos[pdY] - pdYpos[pdY - 1]) - MULTI_INITIAL_POLY_PAD_DIST;
            }
            if ((ledY + 1) < maxPdNumY) {
                padLedLft = (ledYpos[ledY + 1] - ledYpos[ledY]) - MULTI_INITIAL_POLY_PAD_DIST;
            }
            if ((pdY + 1) < maxPdNumY) {
                padPdLft = (pdYpos[pdY + 1] - pdYpos[pdY]) - MULTI_INITIAL_POLY_PAD_DIST;
            }
#endif
            epaY->poly[len][0].vert.y = ledYpos[ledY] - padLedRht; //bot-rht
            epaY->poly[len][1].vert.y = pdYpos[pdY] - padPdRht;  //top-rht
            epaY->poly[len][2].vert.y = pdYpos[pdY] + padPdLft;  //top-lft
            epaY->poly[len][3].vert.y = ledYpos[ledY] + padLedLft; //bot-lft
            epaY->poly[len][0].vert.x = BS_sensor_end_x; //bot-rht
            epaY->poly[len][1].vert.x = BS_sensor_zero_x;  //top-rht
            epaY->poly[len][2].vert.x = BS_sensor_zero_x;  //top-lft
            epaY->poly[len][3].vert.x = BS_sensor_end_x; //bot-lft
            epaY->poly[len][0].edgeId = len;
            epaY->poly[len][1].edgeId = NO_CLIPPING_EDGEID;
            epaY->poly[len][2].edgeId = len;
            epaY->poly[len][3].edgeId = NO_CLIPPING_EDGEID;
#if 0 //(DEBUG_remaied_multi_initial2 > 1)
            IS_DEBUG_FLAG{
                //if (BG_show_line_init)
                {
                    DEBUG_SHOW_POLY(&epaY->poly[len][0], 4, MY_COLOR);
                    TRACE_NOP;
                }
            };
#endif
            len++;
        }
        else {
            TRACE_ERROR("ERROR! remaied_multi_initial2..4 invalid len");
            return -1; //mem error
        }
    } //for (line_idx_y = 0; line_idx_y < BS_initial_line_y_cnt; line_idx_y++)
    epaY->len = len;

    if (epaX->len > 0 || epaY->len > 0) {
        return 1;
    }
    return 0;
}
#endif //1

#if 0 //for test
static int inst_test()
{
	vec_t p0, p1, p2, p3, pR;

	p0.x = BS_led_pos_x[10];
	p0.y = BS_sensor_zero_y;
	p1.x = BS_led_pos_x[10];
	p1.y = BS_sensor_end_y;

	p2.x = BS_led_pos_x[10] + EPSILON;
	p2.y = BS_sensor_end_y / 2;
	p3.x = BS_led_pos_x[10] + 10.0f;
	p3.y = BS_sensor_end_y / 2;

	//if (BS_line_intersection(&p0, &p1, &p2, &p3, &pR)) 
	if (BS_line_intersection(&p2, &p3, &p0, &p1, &pR))
	{
		TRACE("p0: %0.3f %0.3f", p0.x, p0.y);
		TRACE("p2: %0.3f %0.3f", p2.x, p2.y);
		TRACE("pR: %0.3f %0.3f", pR.x, pR.y);
		TRACE_NOP;
	}
}
#endif

//#define TEST_DATA_ENABLE
#ifdef TEST_DATA_ENABLE //for test
#include "test_data.h"
ATTR_BACKEND_RAM2  static DEF_PD_INFO test_input_buffer;
ATTR_BACKEND_RAM2  static DEF_PD test_hor_touch_pd2[MAX_TEST_DATA];
ATTR_BACKEND_RAM2  static DEF_PD test_ver_touch_pd2[MAX_TEST_DATA];
static int test_data(void)
{
    int i;
    for (i = 0; i < MAX_TEST_DATA; i++) {
        if (test_data_x[i][0] < 0 || test_data_x[i][1] < 0) {
            break;
        }
        test_hor_touch_pd2[i].pd = (uint8_t)test_data_x[i][0]; //pd
        test_hor_touch_pd2[i].led = (uint8_t)test_data_x[i][1]; //led
    }
    test_input_buffer.hor_len = i;

    for (i = 0; i < MAX_TEST_DATA; i++) {
        if (test_data_y[i][0] < 0 || test_data_y[i][1] < 0) {
            break;
        }
        test_ver_touch_pd2[i].pd = (uint8_t)test_data_y[i][0]; //pd
        test_ver_touch_pd2[i].led = (uint8_t)test_data_y[i][1]; //led
    }
    test_input_buffer.ver_len = i;
    test_input_buffer.hor_touch_pd = &test_hor_touch_pd2[0];
    test_input_buffer.ver_touch_pd = &test_ver_touch_pd2[0];
    return 0;
}
#endif

//-----------------------------------------------------------------
// call_backend
//-----------------------------------------------------------------
//#define DEBUG_EP_INFO
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_SHOW_FRAME_NO     1  //1:show
#endif
//#define MIN_NUM_IN_LINES         3
//#define MIN_NUM_IN_LINES_MULTI   50 //40
//#define MIN_NUM_REMAINED_FINE    5 //8

ATTR_BACKEND_RAMFUNC
BACKEND_STATUS BG_call_backend2(
	DEF_PD_INFO *InBuf,       // (hor_pd_num + ver_pd_num)
    DEF_DATA_INFO2 *OutBuf2,      // data out
	next_scan_t *nextScan     // next scan infomation
) {
    //BACKEND_STATUS ret = NO_BACKEND_ERROR;
	int retTmp; //i,
    int multiLoopCnt, fineLoop;
    int inLineMax = 0;
    int skip_multi_2 = 0; //nsmoon@210218-bugfix
    int rmlineCnt = 0; //nsmoon@210430
    //int remainedLineMax = 0;
	//side_type_t side;
    //int retAdj;
    //int touch_count_prev;
    //TRACE("BG_call_backend2..");
    DEBUG_GET_TIME_DIFF(DEBUG_TIME_DIFF_START);
    nextScan->x1 = nextScan->y1 = 0; //remained lines //nsmoon@210430

	if (backend_initialized != TRUE) {
		TRACE_ERROR("ERROR! call_backend...not init backend!! [%d]", BG_frame_no);
		return NOT_INITIALIZED; // 10 error, not initialized
	}

#if (BRUSH_MODE_ENABLE == 1)
    if (nextScan->brushMode)
    {
        BS_brushMode = 1; //brush mode
    }
    else {
        BS_brushMode = 0; //brush mode
    }
#endif

    //////////////////////////////////////
    // init OutBuf2
    //////////////////////////////////////
    OutBuf2->touch_data = &BG_touch_data[0];
    OutBuf2->touch_size = &BG_touch_size[0];
#ifdef FRONTEND_LINE_THRESHOLD
    OutBuf2->touch_line_cnt = &BG_touch_lineCnt[0];
#endif
    OutBuf2->multi_fine = &BG_multi_fine[0];
    OutBuf2->touch_count = 0;

#ifdef HOR_EDGE_TOUCH_ENABLE
    //init touch data at edge, nsmoon@200415
    BG_touch_data_edge.x = 0;
    BG_touch_data_edge.y = 0;
#endif

	//////////////////////////////////////
	// init debug variables
	//////////////////////////////////////
    BG_frame_no++;
#if defined(DEBUG_SKIP_FRAME_NO)
	if (BG_frame_no <= DEBUG_SKIP_FRAME_NO) {
		goto L_BG_call_backend_tracking;
	}
#endif
    //START_STOP_POLY_TRACE(0xFD); //nsmoon@211125 move to out of lib
#if defined(DEBUG_FRAME_NO)
	if (BG_frame_no >= DEBUG_FRAME_NO) {
	BG_debug_flag = 1;
		BG_debug_flag2 = 0;
  #ifndef DEBUG_FRAME_PAUSE
		BG_debug_pause = 1;
  #endif
    }
    else {
	    BG_debug_flag = 0;
        BG_debug_flag2 = 0;
  #ifndef DEBUG_FRAME_PAUSE
		BG_debug_pause = 0;
  #endif
    }
#else
	BG_debug_flag = 0;
  #ifndef DEBUG_FRAME_PAUSE
	BG_debug_pause = 0;
  #endif
#endif
#ifdef DEBUG_FRAME_PAUSE
  #if (DEBUG_FRAME_PAUSE == 1)
	BG_debug_pause = 1;
  #else
	BG_debug_pause = 0;
  #endif
#endif

	//////////////////////////////////////
    // Init variable for frontend
	//////////////////////////////////////
#ifdef TEST_DATA_ENABLE //for test
    test_data();
    InBuf = &test_input_buffer;
#endif
	BS_inBuf = InBuf;
	BG_touch_count = 0;
    BG_touch_count_multi = BG_touch_count_fine = 0;
    BS_remained_x_cnt = BS_remained_y_cnt = 0; //nsmoon@221115

#if 1 //for test, nsmoon@190703
	maxVertex = 0; //for test
	maxProcessCnt = 0;
	maxSubjCnt = 0;
	maxClipIdx = 0;
#endif
	multiLoopCnt = 0;

#if 0 //save BS_large_touch_idx_cnt, nsmoon@191224
	int i;
    for (i = 0; i < BS_large_touch_idx_cnt_prev[0]; i++) {
        BS_large_touch_idx_prev[1][i] = BS_large_touch_idx_prev[0][i];
    }
    BS_large_touch_idx_cnt_prev[1] = BS_large_touch_idx_cnt_prev[0];

	for (i = 0; i < BS_large_touch_idx_cnt; i++) {
        BS_large_touch_idx_prev[0][i] = BS_large_touch_idx[i];
	}
    BS_large_touch_idx_cnt_prev[0] = BS_large_touch_idx_cnt;
	BS_large_touch_idx_cnt = 0;
#endif

    inLineMax = GET_MAX(InBuf->hor_len, InBuf->ver_len);
    if (inLineMax < MIN_NUM_IN_LINES) {
        InBuf->hor_len = InBuf->ver_len = 0; //nsmoon@221115
		goto L_BG_call_backend_tracking;
	}
#if (DEBUG_SHOW_FRAME_NO > 0) //&& defined(DEBUG_FRAME_NO)
#ifdef FRONTEND_LINE_THRESHOLD
	TRACE("---[%d](%d,%d)(%d,%d)", BG_frame_no, InBuf->hor_len, InBuf->ver_len, InBuf->threshold_x_cnt, InBuf->threshold_y_cnt); //BG_frame_no:
#else
	TRACE("---[%d](%d,%d)", BG_frame_no, (int)InBuf->hor_len, (int)InBuf->ver_len); //BG_frame_no:
#endif
#endif
	IS_DEBUG_FLAG{
		TRACE_NOP;
	};

#if defined(DEBUG_FUNCTION_ENABLE_ALL) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
    BG_cal_slot_cur_cnt = 0;
    BG_cal_slot_org_cnt = 0;
    BG_cal_slot_buf_idx = 0;
#endif

	//////////////////////////
	//update input buffer
	//////////////////////////
	get_input_buffer(InBuf);
#if 0 //for test
    BS_packEdgePattern(ENUM_HOR_X, 0);
    BS_packEdgePattern(ENUM_VER_Y, 0);
#endif
#if 0 //reserved, do not remove
    inLineMax = GET_MAX(InBuf->hor_len, InBuf->ver_len);
    if (inLineMax < MIN_NUM_IN_LINES_MULTI) {
        goto L_BG_call_backend_fine;
    }
#endif

    //////////////////////////
    // multi-touch
    //////////////////////////
#if !defined(DEBUG_SKIP_MULTI) //multi-touch first //nsmoon@230412
    //initial clipping
#ifdef MULTI_PACK_EP3_ENABLE
    retTmp = packEdgePatternInitial3(InBuf);
    if (retTmp < 0) {
        goto L_BG_call_backend_mem_error; //mem-error
    }
    retTmp = BG_clipping_multi_initial();
#endif
#ifdef MULTI_PACK_EP4_ENABLE
    retTmp = packEdgePatternInitial4();
    if (retTmp < 0) {
		goto L_BG_call_backend_mem_error; //mem-error
    }
    retTmp = BG_clipping_multi_initial4();
#endif

#if 0 //for evaluation
        //drawIntersect(ENUM_BOT_BD);
        //drawEpaMulti(ENUM_BOT_BD);
        //DEBUG_drawPolyScore();
#endif
    BS_touch_info_multi_cnt = 0; //init
    while (multiLoopCnt++ < MAX_MULTI_LOOP_CNT) //2 times loop
    {
        //TRACE("multiLoopCnt=%d (%d)", multiLoopCnt, retTmp);
        DEBUG_GET_TIME_DIFF(DEBUG_TIME_DIFF_MULTI_PRE);
        retTmp = BG_clipping_multi(multiLoopCnt);
        DEBUG_GET_TIME_DIFF(DEBUG_TIME_DIFF_MULTI);
        if (retTmp < 0) {
            TRACE_ERROR("ERROR! ...BG_clipping_multi");
            goto L_BG_call_backend_mem_error; //mem-error
        }
#if 0 //(MAX_TOUCH_LIMIT_FINE == 8) //ndef DEBUG_EP_TEST //nsmoon@210226 //nsmoon@210510 1=>0
        if (multiLoopCnt == 1 && retTmp < 1) {
        	//skip multi-2
            skip_multi_2 = 1;
        	break;
        }
#endif
        IS_DEBUG_FLAG{
            TRACE_NOP;
        };

        //////////////////////////
        // MULTI-2
        if (multiLoopCnt < MAX_MULTI_LOOP_CNT) //nsmoon@200814
        {
            //save BS_touch_info_multi_cnt of loop1
#if (DEAD_CELL_TBL == 1) //nsmoon@190829
            //add dead cell
            add_dead_sensor_used();
#endif
#if 0 //(SECOND_OFST_TBL == 1) //nsmoon@190829
            //add 2nd ofst cell
            add_2ndOfst_sensor_used();
#endif

            //get remained line
            rmlineCnt = BS_get_remained_line(ENUM_HOR_X, multiLoopCnt);
            if (rmlineCnt < 0) {
                goto L_BG_call_backend_mem_error; //mem-error
            }
            rmlineCnt = BS_get_remained_line(ENUM_VER_Y, multiLoopCnt);
            if (rmlineCnt < 0) {
                goto L_BG_call_backend_mem_error; //mem-error
            }
            if (BS_remained_x_cnt == 0 || BS_remained_y_cnt == 0) {
                //BS_initial_polygon_cnt = 0;
                //continue;
                break;
            }
            //goto L_BG_call_backend_02; //for testing
            IS_DEBUG_FLAG{
                TRACE_NOP;
            };

            //re-cal initial polygon
            retTmp = remaied_multi_initial();
            if (retTmp < 0) {
                goto L_BG_call_backend_mem_error; //mem-error
            }
#if (DEBUG_SHOW_FRAME_NO > 1)
            TRACE("BS_initial_polygon_cnt: %d (%d,%d)", BS_initial_polygon_cnt, BS_remained_x_cnt, BS_remained_y_cnt);
            IS_DEBUG_FLAG{
                TRACE_NOP;
            };
#endif
            if (retTmp == 0) {
                break;
            }
        } //if (multiLoopCnt == 1)
    } //while (multiLoopCnt++ < 2)

    if (skip_multi_2 == 0) //nsmoon@210218-bugfix
	{
    retTmp = BG_clipping_multi_post(multiLoopCnt);
        DEBUG_GET_TIME_DIFF(DEBUG_TIME_DIFF_MULTI_POST);
    if (retTmp < 0) {
        goto L_BG_call_backend_mem_error; //mem-error
    }
    if (retTmp > 0) {
#if (DEBUG_SHOW_FRAME_NO > 0)
        TRACE("-multi-,%d,%d, %d,%d,%d,%d %0.2f,%0.2f,", BG_touch_count, BG_frame_no, maxVertex, maxProcessCnt, maxSubjCnt, maxClipIdx, BG_touch_area[0], BG_touch_area[1]);
		IS_DEBUG_FLAG{
			TRACE_NOP;
		};
#endif
	}
    if (BG_touch_count >= ALLOWABLE_TOUCH_BE) {
        if (BG_touch_count > ALLOWABLE_TOUCH_BE) {
            TRACE_ERROR("ERROR! invalid BG_touch_count...%d", BG_touch_count);
            BG_touch_count = ALLOWABLE_TOUCH_BE;
        }
        goto L_BG_call_backend_tracking;
    }
    }
#endif //1
#if defined(DEBUG_SKIP_AFTER_MULTI)
    goto L_BG_call_backend_tracking;
#endif

#if 0 //for test
    BS_packEdgePattern(ENUM_HOR_X, 1);
    BS_packEdgePattern(ENUM_VER_Y, 1);
#endif

    //////////////////////////
    // fine-touch
    //////////////////////////
#if 1 //for fine
        //get remained line
    rmlineCnt = BS_get_remained_line(ENUM_HOR_X, REM_LOOP_CNT_FINE5);
    if (rmlineCnt < 0) {
            goto L_BG_call_backend_mem_error; //mem-error
        }
    rmlineCnt = BS_get_remained_line(ENUM_VER_Y, REM_LOOP_CNT_FINE5);
    if (rmlineCnt < 0) {
            goto L_BG_call_backend_mem_error; //mem-error
        }
    //if (BS_remained_x_cnt >= MIN_NUM_IN_LINES || BS_remained_y_cnt >= MIN_NUM_IN_LINES)
    if (BS_remained_x_cnt > MIN_NUM_IN_LINES || BS_remained_y_cnt > MIN_NUM_IN_LINES) //nsmoon@211022
    {
#if (DEBUG_SHOW_FRAME_NO > 0)
            TRACE("(fine5)BS_remained_x_cnt,BS_remained_y_cnt: %d,%d", BS_remained_x_cnt, BS_remained_y_cnt);
            IS_DEBUG_FLAG{
                TRACE_NOP;
            };
#endif
        }
        else {
#if (BRUSH_MODE_ENABLE == 1)
            if (BS_brushMode) {
                goto L_BG_call_backend_brush;
            }
#endif
#ifdef HOR_EDGE_TOUCH_ENABLE //nsmoon@220118
        goto L_BG_call_backend_edge;
#else
            goto L_BG_call_backend_tracking;
#endif
        }
    //remainedLineMax = GET_MAX(BS_remained_x_cnt, BS_remained_y_cnt);
    //if (remainedLineMax > MIN_NUM_REMAINED_FINE) 
    //if (BS_remained_x_cnt >= MIN_NUM_REMAINED_FINE && BS_remained_y_cnt >= MIN_NUM_REMAINED_FINE)
    retTmp = -1; //reset, nsmoon@201117
    if (BS_remained_x_cnt > MIN_NUM_REMAINED_FINE && BS_remained_y_cnt > MIN_NUM_REMAINED_FINE)
    {
#if (DEBUG_SHOW_FRAME_NO > 0)
        TRACE("fine5..%d,%d ", BS_remained_x_cnt, BS_remained_y_cnt);
#endif
        DEBUG_GET_TIME_DIFF(DEBUG_TIME_DIFF_FINE_PRE);
        retTmp = BG_clipping_fine5(FINE_LOOP_FINE5);
        DEBUG_GET_TIME_DIFF(DEBUG_TIME_DIFF_FINE);
            if (retTmp < 0) {
                goto L_BG_call_backend_mem_error;
            }
#if (DEBUG_SHOW_FRAME_NO > 0)
        if (retTmp > 0) {
            TRACE("-fine-,%d,%d %0.2f,%0.2f,", BG_touch_count, BG_frame_no, BG_touch_area[0], BG_touch_area[1]);
            IS_DEBUG_FLAG{
                TRACE_NOP;
            };
        }
#endif
#if 1 //nsmoon@211021
        if (BG_touch_count_fine  == MAX_TOUCH_LIMIT_FINE) {
            goto L_BG_call_backend_tracking;
        }
#endif
    }
#if (BRUSH_MODE_ENABLE == 1)
    else {
        if (BS_brushMode) {
            goto L_BG_call_backend_brush;
        }
    }
#else
    else {
        retTmp = GET_MAX(BS_remained_x_cnt, BS_remained_y_cnt);
        }
#endif
#endif //for fine
    IS_DEBUG_FLAG {
    //goto L_BG_call_backend_test; //for test
        //goto L_BG_call_backend_mem_error; //for test
    };
    //SEND_POLY(0, 0x21, 0); //for test
#if defined(DEBUG_UNBLOCKED_LINE) || defined(DEBUG_SKIP_AFTER_FINE) //nsmoon@230412
    goto L_BG_call_backend_tracking; //for test
#endif

#if 1 //incompleted multi-pen, nsmoon@200107 FIXME
	//////////////////////////
	//remained fine-touch
	//////////////////////////
    for (fineLoop = FINE_LOOP_XY_ST; fineLoop <= FINE_LOOP_XY_ED; fineLoop++) {
        //TRACE("retTmp(1)=%d,%d,%d ", retTmp, BS_remained_x_cnt, BS_remained_y_cnt);
        if (retTmp > 0) {
            rmlineCnt = BS_get_remained_line(ENUM_HOR_X, REM_LOOP_CNT_XY);
            if (rmlineCnt < 0) {
                goto L_BG_call_backend_mem_error; //mem-error
            }
            rmlineCnt = BS_get_remained_line(ENUM_VER_Y, REM_LOOP_CNT_XY);
            if (rmlineCnt < 0) {
                goto L_BG_call_backend_mem_error; //mem-error
            }
            //goto L_BG_call_backend_tracking; //for testing
            //TRACE("retTmp(2)=%d,%d,%d ", retTmp, BS_remained_x_cnt, BS_remained_y_cnt);

            //if (BS_remained_x_cnt > MIN_NUM_IN_LINES || BS_remained_y_cnt > MIN_NUM_IN_LINES)
            //if (BS_remained_x_cnt > MIN_NUM_IN_LINES && BS_remained_y_cnt > MIN_NUM_IN_LINES)
            if (BS_remained_x_cnt >= MIN_NUM_IN_LINES && BS_remained_y_cnt >= MIN_NUM_IN_LINES)  //nsmoon@211022
            {
#if (DEBUG_SHOW_FRAME_NO > 0)
                TRACE("(xy)BS_remained_x_cnt,BS_remained_y_cnt: %d,%d", BS_remained_x_cnt, BS_remained_y_cnt);
                IS_DEBUG_FLAG{
                    TRACE_NOP;
                };
#endif
            }
            else {
                break;
            }
#if 0 //for test
            else {
#if (BRUSH_MODE_ENABLE == 1)
                if (BS_brushMode) {
                    goto L_BG_call_backend_brush;
                }
#endif
                goto L_BG_call_backend_tracking;
            }
#endif
        } //if (retTmp > 0)

        retTmp = -1; //reset, nsmoon@201117
        if ((BS_remained_x_cnt > 0 && BS_remained_y_cnt > 0) &&
            (BS_remained_x_cnt >= FINE_MIN_REMAINED_LINE_X2 || BS_remained_y_cnt >= FINE_MIN_REMAINED_LINE_Y2)) {
#if (DEBUG_SHOW_FRAME_NO > 0)
            TRACE("xy..(%d)%d,%d ", fineLoop, BS_remained_x_cnt, BS_remained_y_cnt);
#endif
            //touch_count_prev = BG_touch_count;
            DEBUG_GET_TIME_DIFF(DEBUG_TIME_DIFF_XY_PRE);
            retTmp = BS_fine_add_remained_touch_xy(fineLoop);
            DEBUG_GET_TIME_DIFF(DEBUG_TIME_DIFF_XY);
            if (retTmp < 0) {
                goto L_BG_call_backend_mem_error;
            }
            if (retTmp == 0) {
                break;
            }
            //retAdj = 0; //reset
#if (DEBUG_SHOW_FRAME_NO > 0)
        if (retTmp > 0) {
            TRACE("-xy-,%d,%d,%d %0.2f,%0.2f,", BG_touch_count, BG_frame_no, fineLoop, BG_touch_area[0], BG_touch_area[1]);
            IS_DEBUG_FLAG{
                TRACE_NOP;
            };
        }
#endif
#if 1 //nsmoon@211021
            if (BG_touch_count_fine  == MAX_TOUCH_LIMIT_FINE) {
                goto L_BG_call_backend_tracking;
            }
#endif
        }
        else {
            //retTmp = GET_MAX(BS_remained_x_cnt, BS_remained_y_cnt);
            break;
        }
    } //for
#endif //1

#if 1 //large or edge multi-pen, nsmoon@200107 FIXME
    if (BG_touch_count > 0) //nsmoon@211130
    {
	//////////////////////////
	//remained fine-touch
	//////////////////////////
#if 0 //nsmoon@220116 //for test
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };
    BS_adj_used_lines_tp(0, BG_touch_count, ADJUST_USED_LINE_SHADOW);
    retTmp = 1;
#endif
    for (fineLoop = FINE_LOOP_SHADOW_ST; fineLoop <= FINE_LOOP_SHADOW_ED; fineLoop++) {
        if (retTmp > 0)
        {
            rmlineCnt = BS_get_remained_line(ENUM_HOR_X, REM_LOOP_CNT_SHADOW);
            if (rmlineCnt < 0) {
                goto L_BG_call_backend_mem_error; //mem-error
            }
            rmlineCnt = BS_get_remained_line(ENUM_VER_Y, REM_LOOP_CNT_SHADOW);
            if (rmlineCnt < 0) {
                goto L_BG_call_backend_mem_error; //mem-error
            }
            //goto L_BG_call_backend_tracking; //for testing

            if (BS_remained_x_cnt > MIN_NUM_REMAINED_FINE || BS_remained_y_cnt > MIN_NUM_REMAINED_FINE) { //nsmoon@211021 MIN_NUM_IN_LINES=>MIN_NUM_REMAINED_FINE
#if (DEBUG_SHOW_FRAME_NO > 0)
                TRACE("(shadow)BS_remained_x_cnt,BS_remained_y_cnt: %d,%d", BS_remained_x_cnt, BS_remained_y_cnt);
                IS_DEBUG_FLAG{
                    TRACE_NOP;
                };
#endif
            }
            else {
#if (BRUSH_MODE_ENABLE == 1)
                if (BS_brushMode) {
                    goto L_BG_call_backend_brush;
                }
#endif
#ifdef HOR_EDGE_TOUCH_ENABLE //nsmoon@220118
                goto L_BG_call_backend_edge;
#else
                goto L_BG_call_backend_tracking;
#endif
            }
        } //if (retTmp > 0)

        retTmp = -1; //reset, nsmoon@201117
        if (BS_remained_x_cnt >= FINE_MIN_REMAINED_LINE_X2 || BS_remained_y_cnt >= FINE_MIN_REMAINED_LINE_Y2) {
#if (DEBUG_SHOW_FRAME_NO > 0)
            TRACE("shadow..(%d)%d,%d ", fineLoop, BS_remained_x_cnt, BS_remained_y_cnt);
#endif
            //touch_count_prev = BG_touch_count;
            DEBUG_GET_TIME_DIFF(DEBUG_TIME_DIFF_SHADOW_PRE);
            retTmp = BS_fine_add_remained_touch_shadow(fineLoop);
            DEBUG_GET_TIME_DIFF(DEBUG_TIME_DIFF_SHADOW);
            if (retTmp < 0) {
                goto L_BG_call_backend_mem_error;
            }
            if (retTmp == 0) {
                break;
            }
#if (DEBUG_SHOW_FRAME_NO > 0)
        if (retTmp > 0) {
            TRACE("-shdow-,%d,%d,%d %0.2f,%0.2f,", BG_touch_count, BG_frame_no, fineLoop,BG_touch_area[0], BG_touch_area[1]);
            IS_DEBUG_FLAG{
                TRACE_NOP;
            };
        }
#endif
#if 1 //nsmoon@211021
            if (BG_touch_count_fine  == MAX_TOUCH_LIMIT_FINE) {
                goto L_BG_call_backend_tracking;
            }
#endif
        }
        else {
            break;
        }
    } //for
#endif //1
    //goto L_BG_call_backend_test; //for testing
    }

#ifdef HOR_EDGE_TOUCH_ENABLE //nsmoon@201118
L_BG_call_backend_edge:
    IS_DEBUG_FLAG {
        TRACE_NOP;
    }
    if (!BS_brushMode && BG_touch_count == 0) {
        TRACE("(edge.x)=%0.1f,%0.1f %d,%d, ", BG_touch_data_edge.x, BG_touch_data_edge.y, BS_remained_x_cnt, BS_remained_y_cnt);
        if (BG_touch_data_edge.x == 0.0f)
        {
            //if (BS_remained_x_cnt > MIN_NUM_IN_LINES && BS_remained_y_cnt == 0) //nsmoon@211022
            //if (BS_remained_x_cnt >= MIN_NUM_IN_LINES && BS_remained_y_cnt <= MIN_NUM_IN_LINES) //nsmoon@220118
            if (BS_remained_x_cnt >= MIN_NUM_REMAINED_FINE && BS_remained_y_cnt <= MIN_NUM_REMAINED_FINE) //nsmoon@220127
            {
                DEBUG_GET_TIME_DIFF(DEBUG_TIME_DIFF_EDGE_PRE);
                BS_fine_add_remained_touch_edge_x();
                DEBUG_GET_TIME_DIFF(DEBUG_TIME_DIFF_EDGE);
            }
            else if (BS_remained_x_cnt <= MIN_NUM_REMAINED_FINE && BS_remained_y_cnt >= MIN_NUM_REMAINED_FINE) //nsmoon@220127
            {
                DEBUG_GET_TIME_DIFF(DEBUG_TIME_DIFF_EDGE_PRE);
                BS_fine_add_remained_touch_edge_y();
                DEBUG_GET_TIME_DIFF(DEBUG_TIME_DIFF_EDGE);
            }
        }
    }
#endif

#if (BRUSH_MODE_ENABLE == 1)
L_BG_call_backend_brush:
    if (BS_brushMode) {
        if (retTmp > 0) {
        /////////////////////////////////////////
        //remained fine-touch for brush mode
        /////////////////////////////////////////
            rmlineCnt = BS_get_remained_line(ENUM_HOR_X, REM_LOOP_CNT_BRUSH);
            if (rmlineCnt < 0) {
            goto L_BG_call_backend_mem_error; //mem-error
        }
            rmlineCnt = BS_get_remained_line(ENUM_VER_Y, REM_LOOP_CNT_BRUSH);
            if (rmlineCnt < 0) {
            goto L_BG_call_backend_mem_error; //mem-error
        }
        //goto L_BG_call_backend_tracking; //for testing
#if (DEBUG_SHOW_FRAME_NO > 0)
        if (BS_remained_x_cnt > 0 || BS_remained_y_cnt > 0) {
            TRACE("(brush)BS_remained_x_cnt,BS_remained_y_cnt: %d,%d", BS_remained_x_cnt, BS_remained_y_cnt);
            IS_DEBUG_FLAG{
                TRACE_NOP;
            };
        }
#endif
        }

        retTmp = -1; //reset, nsmoon@210430
        if (BS_remained_x_cnt > FINE_INITIAL_GRP_MIN_NUM_BRUSH && BS_remained_y_cnt > FINE_INITIAL_GRP_MIN_NUM_BRUSH) {
#if (DEBUG_SHOW_FRAME_NO > 0)
            TRACE("brush..%d,%d ", BS_remained_x_cnt, BS_remained_y_cnt);
#endif
            DEBUG_GET_TIME_DIFF(DEBUG_TIME_DIFF_BRUSH_PRE);
            retTmp = BS_fine_add_remained_touch_brush(FINE_LOOP_BRUSH);
            DEBUG_GET_TIME_DIFF(DEBUG_TIME_DIFF_BRUSH);
#if (DEBUG_SHOW_FRAME_NO > 0)
        if (retTmp > 0) {
            TRACE("-brush-,%d,%d, %0.2f,%0.2f,", BG_touch_count, BG_frame_no, BG_touch_area[0], BG_touch_area[1]);
            TRACE_NOP;
        }
#endif
        }
        else if (BS_remained_x_cnt > 1 || BS_remained_y_cnt > 1) {
#if (DEBUG_SHOW_FRAME_NO > 0)
            TRACE("brush2..%d,%d ", BS_remained_x_cnt, BS_remained_y_cnt);
#endif
            DEBUG_GET_TIME_DIFF(DEBUG_TIME_DIFF_BRUSH_PRE);
            retTmp = BS_fine_add_remained_touch_brush2(FINE_LOOP_BRUSH);
            DEBUG_GET_TIME_DIFF(DEBUG_TIME_DIFF_BRUSH);
#if (DEBUG_SHOW_FRAME_NO > 0)
        if (retTmp > 0) {
            TRACE("-brush2-,%d,%d, %0.2f,%0.2f,", BG_touch_count, BG_frame_no, BG_touch_area[0], BG_touch_area[1]);
            TRACE_NOP;
        }
#endif
        }
    } //if (BS_brushMode)
#endif

	//TRACE("(70)BS_slopeValMaxY=%d", BS_slopeValMaxY); //FIXME
#ifdef ONE_POINT_FAST_WRITING_TEST //for testing
	if (BG_touch_count > 1) {
	    TRACE_ERROR("@%d", BG_touch_count);
	}
#endif

L_BG_call_backend_tracking: //nsmoon@211021
    if (BG_touch_data_edge.x != 0) {
#if 1 //nsmoon@220127
        retTmp = BS_is_edge(ENUM_HOR_X, BG_touch_data_edge.x);
        if (retTmp == EDGE_INNER_PD_X || retTmp == EDGE_INNER_LED_X) {
            BS_adj_used_lines_tp(0, 0, ADJUST_USED_LINE_EDGE_Y);
        }
        retTmp = BS_is_edge(ENUM_VER_Y, BG_touch_data_edge.y);
        if (retTmp == EDGE_INNER_PD_Y || retTmp == EDGE_INNER_LED_Y) {
        BS_adj_used_lines_tp(0, 0, ADJUST_USED_LINE_EDGE_X);
    }
        BS_adj_used_lines_tp(0, 0, ADJUST_USED_LINE_EDGE_X); //nsmoon@220115
#endif
    }
    BS_adj_used_lines_tp(0, BG_touch_count, ADJUST_USED_LINE_FINAL);
#if 1  //remained lines //nsmoon@210430
    //if (retTmp > 0)
    {
        /////////////////////////////////////////
        //remained line
        /////////////////////////////////////////
        rmlineCnt = BS_get_remained_line(ENUM_HOR_X, REM_LOOP_CNT_FINAL);
        if (rmlineCnt < 0) {
            goto L_BG_call_backend_mem_error; //mem-error
        }
        rmlineCnt = BS_get_remained_line(ENUM_VER_Y, REM_LOOP_CNT_FINAL);
        if (rmlineCnt < 0) {
            goto L_BG_call_backend_mem_error; //mem-error
        }
    #if (DEBUG_SHOW_FRAME_NO > 0)
        if (BS_remained_x_cnt > 0 || BS_remained_y_cnt > 0) {
            TRACE("(final)BS_remained_x_cnt,BS_remained_y_cnt: %d,%d ", BS_remained_x_cnt, BS_remained_y_cnt);
            IS_DEBUG_FLAG{
                TRACE_NOP;
            };
        }
    #endif
    }
    nextScan->x1 = BS_remained_x_cnt;
    nextScan->y1 = BS_remained_y_cnt;
#endif

//L_BG_call_backend_test:
#ifdef HOR_EDGE_TOUCH_ENABLE
    //TRACE("(B)touch_data_edge= %0.1f %0.1f", BG_touch_data_edge.x, BG_touch_data_edge.y);
    OutBuf2->touch_data_edge = BG_touch_data_edge;
#endif
    OutBuf2->touch_count = BG_touch_count;
#if defined(_WIN32) || defined(WIN32)
    IS_DEBUG_FLAG{
        if (BG_touch_count_prev != BG_touch_count) {
            TRACE_RELEASE("===[%d]%d/%d(%d,%d)(%d,%d)", BG_frame_no, BG_touch_count, BG_touch_count_prev, InBuf->hor_len, InBuf->ver_len, InBuf->threshold_x_cnt, InBuf->threshold_y_cnt);
            TRACE_NOP;
        }
        BG_touch_count_prev = BG_touch_count;
    };
#endif
    if (BG_touch_count > 0)
    {
#if (DEBUG_SHOW_FRAME_NO > 0)
        //DEBUG_SHOW_TOUCH_POINT;
        TRACE("=!=,%d,%d,", BG_touch_count, BG_frame_no);
#if 1 //debug pos and size
        int i;
        for (i = 0; i < BG_touch_count; i++) {
            TRACE("..(%d) %0.1f,%0.1f (%0.2f,%0.2f)(%0.2f,%0.2f)", i, BG_touch_data[i].x, BG_touch_data[i].y, BG_touch_size[i].xSize,  BG_touch_size[i].ySize, BG_touch_lineCnt[i].th10WidthX, BG_touch_lineCnt[i].th10WidthY);
        }
#endif //1
#endif
#if defined(DEBUG_FUNCTION_ENABLE_RELEASE) //for size test
#if (DEBUG_SHOW_FRAME_NO > 0)
#if (BRUSH_MODE_ENABLE == 1)
        if (BS_brushMode) {
            //TRACE_RELEASE("touch_count(brush)= %d [%d]", BG_touch_count, BG_frame_no);
            TRACE_RELEASE("touch_count(brush)=,[%d],%d,:,%f,%f,", BG_frame_no, BG_touch_count, BG_touch_data[0].x, BG_touch_data[0].y);
        }
        else
#endif
        {
            //TRACE_RELEASE("touch_count=,[%d],%d,:,%d,%d,", BG_frame_no, BG_touch_count, (int)InBuf->hor_len, (int)InBuf->ver_len);
            TRACE_RELEASE("touch_count=,[%d],%d,:,%0.2f,%0.2f,", BG_frame_no, BG_touch_count, BG_touch_data[0].x, BG_touch_data[0].y);
        }
#endif
#endif
		IS_DEBUG_FLAG{
            //if (BG_touch_count != 2) 
			{
		    TRACE_NOP;
            }
		};
    }
#ifdef NORMAL_EDGE_ONLY_ONE_USED //nsmoon@210512a
    else
#endif
    {
#if defined(DEBUG_FUNCTION_ENABLE_RELEASE) && (BRUSH_MODE_ENABLE == 1)
        vec_t tmpPos;
        tmpPos.x = 0; tmpPos.y = 0;
        DEBUG_SHOW_POS(&tmpPos, 0.01f, 0.01f, 0);
#endif
#ifdef HOR_EDGE_TOUCH_ENABLE
        DEBUG_SHOW_POS(&BG_touch_data_edge, 2.0f, 2.0f, MY_COLOR);
#if 0 //defined(DEBUG_FUNCTION_ENABLE_RELEASE)
        if (BG_touch_data_edge.x > 0) {
            TRACE_RELEASE("touch_data_edge=,[%d],%0.1f,%0.1f", BG_frame_no, BG_touch_data_edge.x, BG_touch_data_edge.y);
        }
        else {
            TRACE_RELEASE("touch_count=,[%d],%d,:,%d,%d,", BG_frame_no, BG_touch_count, (int)InBuf->hor_len, (int)InBuf->ver_len);
        }
#endif
#endif
        //TRACE_RELEASE("touch_count=,[%d],%d,:,%d,%d,", BG_frame_no, BG_touch_count, (int)InBuf->hor_len, (int)InBuf->ver_len);
        IS_DEBUG_FLAG{
            //if (BG_touch_count != 2) 
            {
                TRACE_NOP;
            }
        };
    }
    //START_STOP_POLY_TRACE(0xFE); //nsmoon@211105 move to out of lib
    DEBUG_GET_TIME_DIFF_SHOW();
    return NO_BACKEND_ERROR; //no error

L_BG_call_backend_mem_error:
    //START_STOP_POLY_TRACE(0xFE); //nsmoon@211125 move to out of lib
    //TRACE("polgon_saved_idx= %d", polgon_saved_idx);
    DEBUG_GET_TIME_DIFF_SHOW();
    return SKIP_HID_REPORT; //skip hid report
}
//////////////////////////////////////////////////////////////

//*****************************************************************************
// End of File
