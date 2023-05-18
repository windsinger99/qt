/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Lispect

  @File Name
    backend_multi.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */
#include "backend_main.h"
#include "backend_multi.h"
#include "backend_utils.h"
#include "IRTP_layout.h"
#include "backend_debug.h"
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

////////////////////////////////
// Global Variable and Functions
////////////////////////////////
#if 0 //reduce mem size
ATTR_BACKEND_RAM ep_buf_t BS_edge_pattern_x[MAX_NUM_X_SLOPES];
ATTR_BACKEND_RAM ep_buf_t BS_edge_pattern_y[MAX_NUM_Y_SLOPES];
ATTR_BACKEND_RAM ep_buf_t BG_edge_pattern[MAX_NUM_SIDE][MAX_NUM_PD];
#endif
#ifdef USE_CUST_MALLOC //nsmoon@201012
ATTR_BACKEND_RAM2 int BS_max_ini_epa_size;
ATTR_BACKEND_RAM2 ep_buf_t *BS_edge_pattern_x, *BS_edge_pattern_y;
ATTR_BACKEND_RAM2 ep_buf_t *BG_edge_pattern;
#else
ATTR_BACKEND_RAM2 ep_buf_t BS_edge_pattern_x[MAX_INIT_EPA_SIZE]; //x ep for multi initial
ATTR_BACKEND_RAM2 ep_buf_t BS_edge_pattern_y[MAX_INIT_EPA_SIZE]; //y ep for multi initial
ATTR_BACKEND_RAM2 ep_buf_t BG_edge_pattern[MAX_NUM_PD];
#endif

ATTR_BACKEND_RAM2 ep_buf_t BG_edge_pattern_dz;
#if 1 //nsmoon@190326
ATTR_BACKEND_RAM2 ep_no_t BS_edge_pattern_dz_ep[MAX_DZ_EP_COUNT];
ATTR_BACKEND_RAM2 vec_e_t BS_edge_pattern_dz_poly[MAX_DZ_EP_COUNT][INITIAL_POLYGON_VERTEX_NO];
#endif
ATTR_BACKEND_RAM2 int BG_edge_pattern_cnt_x, BG_edge_pattern_cnt_y;

#if (LARGE_PITCH_TBL == 1) //nsmoon@200108
#ifdef USE_CUST_MALLOC //nsmoon@201012
ATTR_BACKEND_RAM2 int BS_max_virtual_ep_count;
ATTR_BACKEND_RAM2 ep2_no_t *BS_edge_pattern_virtul_ep; //[MAX_VIRTUAL_EP_COUNT]
ATTR_BACKEND_RAM2 vec_e_t **BS_edge_pattern_virtul_poly; //[MAX_VIRTUAL_EP_COUNT][INITIAL_POLYGON_VERTEX_NO]
#else
ATTR_BACKEND_RAM2 ep2_no_t BS_edge_pattern_virtul_ep[MAX_VIRTUAL_EP_COUNT];
ATTR_BACKEND_RAM2 vec_e_t BS_edge_pattern_virtul_poly[MAX_VIRTUAL_EP_COUNT][INITIAL_POLYGON_VERTEX_NO];
#endif
ATTR_BACKEND_RAM2 int BG_edge_pattern_virtual_cnt;
#endif

#ifdef MULTI_SENSOR_RANGE_OLD //nsmoon@200119
#if 1 //sensor_range
//ATTR_BACKEND_RAM2 polygon_range_t  sensor_range_tmp[MAX_RESULT_POLYGON_NO];
ATTR_BACKEND_RAM2 polygon_range_t  BS_sensor_range_y[MAX_RESULT_POLYGON_NO];
ATTR_BACKEND_RAM2 polygon_range_t  BS_result_sensor_range[MAX_RESULT_POLYGON_NO];
ATTR_BACKEND_RAM2 polygon_range_t  BS_prev_sensor_range[MAX_RESULT_POLYGON_NO];
#endif
#endif //0
#ifdef MULTI_RANGE_RECAL_MIN_MAX //nsmoon@200107
ATTR_BACKEND_RAM static float BS_epa_inst[MAX_INST_BUFF];
#endif
ATTR_BACKEND_RAM2 static vec_e_t  BS_initVert[INITIAL_POLYGON_VERTEX_NO];

//input buffer
#ifdef USE_CUST_MALLOC //nsmoon@201012
ATTR_BACKEND_RAM2 int BS_max_in_line_buf_pd_x, BS_max_in_line_buf_led_x;
ATTR_BACKEND_RAM2 int BS_max_in_line_buf_pd_y, BS_max_in_line_buf_led_y;
ATTR_BACKEND_RAM2 uint8_t **BS_inlineBuf_x_pd; //[MAX_IN_LINE_BUF_PD_X][MAX_IN_LINE_BUF_LED_X]
ATTR_BACKEND_RAM2 uint8_t **BS_inlineBuf_x_led; //[MAX_IN_LINE_BUF_PD_X][MAX_IN_LINE_BUF_LED_X]
ATTR_BACKEND_RAM2 uint8_t **BS_inlineBuf_y_pd; //[MAX_IN_LINE_BUF_PD_Y][MAX_IN_LINE_BUF_LED_Y]
ATTR_BACKEND_RAM2 uint8_t **BS_inlineBuf_y_led; //[MAX_IN_LINE_BUF_PD_Y][MAX_IN_LINE_BUF_LED_Y]
#ifdef USED_LINE_METHOD_OLD
ATTR_BACKEND_RAM2 uint8_t **BS_inlineUsed_x_pd; //[MAX_LINE_USED_BUF_PD_X][MAX_LINE_USED_BUF_LED_X]
ATTR_BACKEND_RAM2 uint8_t **BS_inlineUsed_y_pd; //[MAX_LINE_USED_BUF_PD_Y][MAX_LINE_USED_BUF_LED_Y]
#else
ATTR_BACKEND_RAM2 uint8_t **BS_inlineUsed2_x_pd; //[MAX_LINE_USED_BUF_PD_X][MAX_LINE_USED_BUF_LED_X]
ATTR_BACKEND_RAM2 uint8_t **BS_inlineUsed2_y_pd; //[MAX_LINE_USED_BUF_PD_Y][MAX_LINE_USED_BUF_LED_Y]
#endif
#else
ATTR_BACKEND_RAM2 uint8_t BS_inlineBuf_x_pd[MAX_IN_LINE_BUF_PD_X][MAX_IN_LINE_BUF_LED_X];
ATTR_BACKEND_RAM2 uint8_t BS_inlineBuf_x_led[MAX_IN_LINE_BUF_PD_X][MAX_IN_LINE_BUF_LED_X];
ATTR_BACKEND_RAM2 uint8_t BS_inlineBuf_y_pd[MAX_IN_LINE_BUF_PD_Y][MAX_IN_LINE_BUF_LED_Y];
ATTR_BACKEND_RAM2 uint8_t BS_inlineBuf_y_led[MAX_IN_LINE_BUF_PD_Y][MAX_IN_LINE_BUF_LED_Y];
#ifdef USED_LINE_METHOD_OLD
ATTR_BACKEND_RAM2 uint8_t BS_inlineUsed_x_pd[MAX_LINE_USED_BUF_PD_X][MAX_LINE_USED_BUF_LED_X];
ATTR_BACKEND_RAM2 uint8_t BS_inlineUsed_y_pd[MAX_LINE_USED_BUF_PD_Y][MAX_LINE_USED_BUF_LED_Y];
#else
ATTR_BACKEND_RAM2 uint8_t BS_inlineUsed2_x_pd[MAX_LINE_USED_BUF_PD_X][MAX_LINE_USED_BUF_LED_X];
ATTR_BACKEND_RAM2 uint8_t BS_inlineUsed2_y_pd[MAX_LINE_USED_BUF_PD_Y][MAX_LINE_USED_BUF_LED_Y];
#endif
#endif

#if 0 //for test
//for fine initial subject
ATTR_BACKEND_RAM in_line_t BS_inline_unique_x[MAX_NUM_X_SLOPES][MAX_IN_LINE_UNIQUE];
ATTR_BACKEND_RAM in_line_t BS_inline_unique_y[MAX_NUM_Y_SLOPES][MAX_IN_LINE_UNIQUE];
ATTR_BACKEND_RAM int BS_inline_unique_x_len;
ATTR_BACKEND_RAM int BS_inline_unique_y_len;
ATTR_BACKEND_RAM int BS_inline_unique_x_mode;
ATTR_BACKEND_RAM int BS_inline_unique_y_mode;
#endif

////////////////////////////////////////////
// Backend Static Variable and Functions
////////////////////////////////////////////
ATTR_BACKEND_RAM2 initial_polygon_t BS_initial_polygon[MAX_RESULT_POLYGON_NO]; //FIXME MAX_RESULT_POLYGON_NO??
ATTR_BACKEND_RAM2 int BS_initial_polygon_cnt;
//ATTR_BACKEND_RAM2 initial_point_t BS_initial_point[MAX_RESULT_POINT_NO]; //FIXME MAX_RESULT_POLYGON_NO??
//ATTR_BACKEND_RAM2 int BS_initial_point_cnt;

#ifdef FINE_DATA_CHECK //debug@190420-1
ATTR_BACKEND_RAM2 vec_t BG_touch_data_fine[MAX_TOUCH_LIMIT_FINE];
ATTR_BACKEND_RAM2 float BG_touch_area_fine[MAX_TOUCH_LIMIT_FINE];
ATTR_BACKEND_RAM2 tp_size_t BG_touch_size_fine[MAX_TOUCH_LIMIT_FINE];
ATTR_BACKEND_RAM2 int BG_touch_count_fine;
#endif

ATTR_BACKEND_RAM2 touch_info_multi_t BS_touch_info_multi[MAX_TOUCH_INFO_MULTI_SIZE];
ATTR_BACKEND_RAM2 uint8_t BS_touch_info_multi_sort[MAX_TOUCH_INFO_MULTI_SIZE];
ATTR_BACKEND_RAM2 uint8_t BS_touch_info_multi_grp[MAX_TOUCH_LIMIT_MULTI];
//ATTR_BACKEND_RAM2 uint8_t BS_touch_info_multi_outgrp[MAX_TOUCH_LIMIT_MULTI];
ATTR_BACKEND_RAM2 int BS_touch_info_multi_cnt; //BS_touch_info_multi_cnt_start;
ATTR_BACKEND_RAM2 int BS_touch_info_multi_cnt_sort; //nsmoon@200330
ATTR_BACKEND_RAM2 poly_info_multi_t BS_poly_info_multi_x[MAX_POLY_INFO_MULTI_SIZE];
ATTR_BACKEND_RAM2 poly_info_multi_t BS_poly_info_multi_y[MAX_POLY_INFO_MULTI_SIZE];

#ifndef USE_CUST_MALLOC //nsmoon@201012
ATTR_BACKEND_RAM2 save_ep_t BS_multi_tmp_used_ep[MAX_TMP_USED_EP];
ATTR_BACKEND_RAM2 int BS_multi_tmp_used_ep_cnt;
#endif
#ifdef USED_LINE_METHOD_OLD //nsmoon@200119
ATTR_BACKEND_RAM2 save_ep_t BS_tmp_used_ep[MAX_TMP_USED_EP];
ATTR_BACKEND_RAM2 int BS_tmp_used_ep_cnt;
ATTR_BACKEND_RAM2 int BS_tmp_used_ep_cnt_max;
ATTR_BACKEND_RAM2 static uint8_t save_used_epIdx[MAX_TMP_USED_EPIDX]; //nsmoon@190703
ATTR_BACKEND_RAM2 static uint8_t save_used_epIdx_cnt;
#endif

#ifdef MULTI_SKIP_USED_VB  //nsmoon@210218
ATTR_BACKEND_RAM2 static uint8_t BS_initial_lineNo_x[MAX_TOUCH_LIMIT_MULTI];
ATTR_BACKEND_RAM2 static uint8_t BS_initial_lineNo_y[MAX_TOUCH_LIMIT_MULTI];
ATTR_BACKEND_RAM2 static int BS_initial_lineNo_x_cnt, BS_initial_lineNo_y_cnt;
#endif

#ifdef DRAW_POLYGON_TEST
ATTR_BACKEND_RAM2 vec_e_t DEBUG_touch_polygon[MAX_DEBUG_TOUCH_POLYGON][MAX_RESULT_VERTEX_NO]; //FIXME
ATTR_BACKEND_RAM2 int DEBUG_touch_polygon_cnt;
ATTR_BACKEND_RAM2 int DEBUG_touch_polygon_len[MAX_DEBUG_TOUCH_POLYGON]; //FIXME
#endif

//ATTR_BACKEND_RAM3 float BS_fineGrpEdgeMinPosX;
//ATTR_BACKEND_RAM3 float BS_fineGrpEdgeMaxPosX;
//ATTR_BACKEND_RAM3 float BS_fineGrpEdgeMinPosY;
//ATTR_BACKEND_RAM3 float BS_fineGrpEdgeMaxPosY;

////////////////////////////////////////////
// Backend Static Variable and Functions
////////////////////////////////////////////
ATTR_BACKEND_RAM3 polygon_clipping_info_t BS_pclipping_info;

ATTR_BACKEND_RAM3 static uint8_t  BS_result_polygon_length[MAX_RESULT_POLYGON_NO]; // result polygon's Number of vertices
ATTR_BACKEND_RAM3 static vec_e_t  BS_result_polygon_vertex[MAX_RESULT_POLYGON_NO][MAX_RESULT_VERTEX_NO]; // result polygon's vertices information, 200*8*(4+4+2)=16000 bytes
ATTR_BACKEND_RAM3 static float BS_result_area[MAX_RESULT_POLYGON_NO];
//ATTR_BACKEND_RAM2 static polygon_state_t BS_result_state[MAX_RESULT_POLYGON_NO];
ATTR_BACKEND_RAM3 static uint8_t  BS_prev_polygon_length[MAX_RESULT_POLYGON_NO]; // previous result polygon's Number of vertices
ATTR_BACKEND_RAM3 static vec_e_t  BS_prev_polygon_vertex[MAX_RESULT_POLYGON_NO][MAX_RESULT_VERTEX_NO]; // previous result polygon's vertices information, 200*8*(4+4+2)=16000 bytes
ATTR_BACKEND_RAM3 static float BS_prev_area[MAX_RESULT_POLYGON_NO];
//ATTR_BACKEND_RAM2 static polygon_state_t BS_prev_state[MAX_RESULT_POLYGON_NO];

//ATTR_BACKEND_RAM3 static poly_e_t BS_tmp_polye;
//ATTR_BACKEND_RAM3 static vec_e_t  BS_tmp_ve[MAX_P1_P2_VE];

#ifdef MULTI_SEN_CNT_EVEN_SKIP_ENABLE_2 //nsmoon@200330
#define MAX_MULTI_SEN_CNT_SKIP   20 //10
ATTR_BACKEND_RAM2 static int BS_multi_even_skip_en;
#endif

//#define REMOVE_SMALL_CLIPPING_LOOP //nsmoon@191216
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_remove_small_clipping_loop	0
#endif
#if (DEBUG_remove_small_clipping_loop > 0)
#define TRACE_RSCL(...)	TRACE(__VA_ARGS__)
#else
#define TRACE_RSCL(...)
#endif
#ifdef REMOVE_SMALL_CLIPPING_LOOP
ATTR_BACKEND_RAM2 static unsigned long small_clipping_cnt, small_pd_cnt;
#endif

////////////////////////////////////////////
// static functions
////////////////////////////////////////////
static int addEpaValueMulti(axis_t axis, sen_type_t senType, ep_buf_t *epa, int start, int end);
static void getTouchMinMax(poly_e pe, pos_min_max_t *minMax);
#ifndef POLY_VERTEX_LIMIT_ENABLE //nsmoon@200303
static uint8_t clippingPacksPerPolyNo(void);
#endif
static void clipping_var_init(void);
static void clipping_var_reset(void);
static void clipping_var_swap(void);
static int clipping_var_save_result(poly_e_t *rese, poly_e_t *subj, int skip);
//static int get_slope_order(axis_t axis, sen_no_t *sen, int idx);
static int get_slope_order(axis_t axis, int center, int idx);
static int saveEdgePatternDeadzone(bd_type_t bd, int side, int s1, int s2, int e1, int e2);
static int make_edge_pattern_dz(axis_t axis, ep_buf_t *epa, int side, int senStOp, int senEdOp);
static int make_edge_pattern_empty_dz(axis_t axis, int slopeIdx, int side, int senStOp, int senEdOp);
//static int addEdgePatternHalfPad(axis_t axis, ep_buf_t *epa_all, int slopeIdx, int clipIdx, vec_e_t *buf, int paddingMode);
//static int clipping_multi_remained(bd_type_t bd, ep_min_max_t *minMaxEp, int first);
static int clipping_multi_remained(axis_t axis, initial_polygon_t *initial_polygon, int multiLoopCnt);
static int clipping_add_initial_polygon(int idx);

#ifdef MULTI_RANGE_RECAL_MIN_MAX //nsmoon@200107
static int multi_recal_min_max(axis_t axis, touch_info_multi_t *touchInfo, pos_minMax2_t *minMaxRet);
#endif

#ifdef USED_LINE_METHOD_OLD //nsmoon@200119
//static int set_used_line(axis_t axis, int led, int pd);
//static int set_used_ep(axis_t axis, sen_type_t sen, ep_no_t *ep);
static int set_used_ep2(void);
#if 1 //nsmoon@190703
static int multi_save_used_ep(axis_t axis, sen_type_t sen, uint8_t slopeIdx, uint8_t epIdx);
//static void init_save_used_epIdx(void);
static int is_save_used_epIdx(uint8_t epIdx);
#else
static int multi_save_used_ep(axis_t axis, sen_type_t sen, ep_no_t *ep);
#endif
#endif

static int multi_is_adjacent_tp_surrounded(axis_t axis, int idx_i, int idx_j);
static int multi_is_adjacent_tp_size(int idx_i, int idx_j, int touch_info_cnt);
static int multi_get_clipping_info(axis_t axis, bd_type_t bdX, bd_type_t bdY, int sideIdx, int polyIdx, initial_polygon_t *initial_polygon);

////////////////////////////////////////////
// global functions
////////////////////////////////////////////
int BS_packEdgePatternMulti_ind(bd_type_t bd);
int BG_clipping_multi_initial(void);
void BS_epaInitMulti(void);
int BG_clipping_multi(int multiLoopCnt);
int BG_clipping_multi_post(int multiLoopCnt);
void BG_epaInit(void);
side_type_t BS_getSideFromPos(vec_t *pos);
int BS_multi_is_overlap_min_max_tp(pos_min_max_t mM, int excludeRemoved, int compareMode);

#ifdef USE_CUST_MALLOC //nsmoon@201012
static void *malloc_vec_e_t2(int arryRow, int arryCol)
{
    vec_e_t **arry, *ptr;
    int i, size;

    size = (sizeof(vec_e_t *) * arryRow) + (sizeof(vec_e_t) * arryRow * arryCol); //column
    arry = (vec_e_t **)BS_malloc(size);
    if (arry == 0) {
        TRACE_ERROR("ERROR! malloc_vec_e_t2..");
        return 0; //error
    }
    //ptr is pointing to the first element in of 2D array
    ptr = (vec_e_t *)(arry + arryRow);
    for(i = 0; i < arryRow; i++) { //column
        arry[i] = (ptr + (arryCol * i));
    }

    return (void *)arry;
}

ATTR_BACKEND_RAMFUNC
int BS_multi_malloc(void)
{
    ///////////////////////////////////////////////////////////////////////////////////
    /// ep_buf_t *BG_edge_pattern
    ///////////////////////////////////////////////////////////////////////////////////
    BG_edge_pattern = (ep_buf_t *)BS_malloc(sizeof(ep_buf_t) * BS_max_mum_pd);
    if (BG_edge_pattern == 0) {
        TRACE_ERROR("ERROR! BS_multi_malloc..BS_edge_pattern");
        return 1; //error
    }
    TRACE_MALLOC("sizeof(ep_buf_t)=,%x,%x,", sizeof(ep_buf_t), BS_max_mum_pd);
    TRACE_MALLOC("BG_edge_pattern=,%x,%x,%x,", &BG_edge_pattern[0], sizeof(ep_buf_t) * BS_max_mum_pd, BS_end_of_heap);

    BS_max_ini_epa_size = GET_MAX(BS_slopeValMaxX, BS_slopeValMaxY) + 1;

    ///////////////////////////////////////////////////////////////////////////////////
    /// ep_buf_t *BS_edge_pattern_x
    ///////////////////////////////////////////////////////////////////////////////////
    BS_edge_pattern_x = (ep_buf_t *)BS_malloc(sizeof(ep_buf_t) * BS_max_ini_epa_size);
    if (BS_edge_pattern_x == 0) {
        TRACE_ERROR("ERROR! BS_multi_malloc..BS_edge_pattern_x");
        return 1; //error
    }
    TRACE_MALLOC("BS_edge_pattern_x=,%x,%x,%x,", &BS_edge_pattern_x[0], sizeof(ep_buf_t) * BS_max_ini_epa_size, BS_end_of_heap);

    ///////////////////////////////////////////////////////////////////////////////////
    /// ep_buf_t *BS_edge_pattern_y
    ///////////////////////////////////////////////////////////////////////////////////
    BS_edge_pattern_y = (ep_buf_t *)BS_malloc(sizeof(ep_buf_t) * BS_max_ini_epa_size);
    if (BS_edge_pattern_y == 0) {
        TRACE_ERROR("ERROR! BS_multi_malloc..BS_edge_pattern_y");
        return 1; //error
    }
    TRACE_MALLOC("BS_edge_pattern_y=,%x,%x,%x,", &BS_edge_pattern_y[0], sizeof(ep_buf_t) * BS_max_ini_epa_size, BS_end_of_heap);

    //init variables
    BS_max_in_line_buf_pd_x = BS_maxHorPdNum;
    BS_max_in_line_buf_led_x = (BS_max_in_line_buf_pd_x + 7) / 8;
    BS_max_in_line_buf_pd_y = BS_maxVerPdNum;
    BS_max_in_line_buf_led_y = (BS_max_in_line_buf_pd_y + 7) / 8;

    ///////////////////////////////////////////////////////////////////////////////////
    /// uint8_t **BS_inlineBuf_x_pd; //[MAX_IN_LINE_BUF_PD_X][MAX_IN_LINE_BUF_LED_X]
    ///////////////////////////////////////////////////////////////////////////////////
    BS_inlineBuf_x_pd = (uint8_t **)BS_malloc_uint8_t2(BS_max_in_line_buf_pd_x, BS_max_in_line_buf_led_x);
    if (BS_inlineBuf_x_pd == 0) {
        TRACE_ERROR("ERROR! BS_multi_malloc..BS_inlineBuf_x_pd");
        return 1; //error
    }
    TRACE_MALLOC("BS_inlineBuf_x_pd=,%x,%x,%x,", BS_inlineBuf_x_pd, (sizeof(uint8_t *)*BS_max_in_line_buf_pd_x) + (sizeof(uint8_t)*BS_max_in_line_buf_pd_x*BS_max_in_line_buf_led_x), BS_end_of_heap);

    ///////////////////////////////////////////////////////////////////////////////////
    /// uint8_t **BS_inlineBuf_x_led; //[MAX_IN_LINE_BUF_PD_X][MAX_IN_LINE_BUF_LED_X]
    ///////////////////////////////////////////////////////////////////////////////////
    BS_inlineBuf_x_led = (uint8_t **)BS_malloc_uint8_t2(BS_max_in_line_buf_pd_x, BS_max_in_line_buf_led_x);
    if (BS_inlineBuf_x_led == 0) {
        TRACE_ERROR("ERROR! BS_multi_malloc..BS_inlineBuf_x_led");
        return 1; //error
    }
    TRACE_MALLOC("BS_inlineBuf_x_led=,%x,%x,%x,", BS_inlineBuf_x_led, (sizeof(uint8_t *)*BS_max_in_line_buf_pd_x) + (sizeof(uint8_t)*BS_max_in_line_buf_pd_x*BS_max_in_line_buf_led_x), BS_end_of_heap);

    ///////////////////////////////////////////////////////////////////////////////////
    /// uint8_t **BS_inlineBuf_y_pd; //[MAX_IN_LINE_BUF_PD_Y][MAX_IN_LINE_BUF_LED_Y]
    ///////////////////////////////////////////////////////////////////////////////////
    BS_inlineBuf_y_pd = (uint8_t **)BS_malloc_uint8_t2(BS_max_in_line_buf_pd_y, BS_max_in_line_buf_led_y);
    if (BS_inlineBuf_y_pd == 0) {
        TRACE_ERROR("ERROR! BS_multi_malloc..BS_inlineBuf_y_pd");
        return 1; //error
    }
    TRACE_MALLOC("BS_inlineBuf_y_pd=,%x,%x,%x,", BS_inlineBuf_y_pd, (sizeof(uint8_t *)*BS_max_in_line_buf_pd_y) + (sizeof(uint8_t)*BS_max_in_line_buf_pd_y*BS_max_in_line_buf_led_y), BS_end_of_heap);

    ///////////////////////////////////////////////////////////////////////////////////
    /// uint8_t **BS_inlineBuf_y_led; //[MAX_IN_LINE_BUF_PD_Y][MAX_IN_LINE_BUF_LED_Y]
    ///////////////////////////////////////////////////////////////////////////////////
    BS_inlineBuf_y_led = (uint8_t **)BS_malloc_uint8_t2(BS_max_in_line_buf_pd_y, BS_max_in_line_buf_led_y);
    if (BS_inlineBuf_y_led == 0) {
        TRACE_ERROR("ERROR! BS_multi_malloc..BS_inlineBuf_y_led");
        return 1; //error
    }
    TRACE_MALLOC("BS_inlineBuf_y_led=,%x,%x,%x,", BS_inlineBuf_y_led, (sizeof(uint8_t *)*BS_max_in_line_buf_pd_y) + (sizeof(uint8_t)*BS_max_in_line_buf_pd_y*BS_max_in_line_buf_led_y), BS_end_of_heap);

#ifdef USED_LINE_METHOD_OLD
    ///////////////////////////////////////////////////////////////////////////////////
    /// uint8_t **BS_inlineUsed_x_pd; //[MAX_LINE_USED_BUF_PD_X][MAX_LINE_USED_BUF_LED_X]
    ///////////////////////////////////////////////////////////////////////////////////
    BS_inlineUsed_x_pd = (uint8_t **)BS_malloc_uint8_t2(BS_max_in_line_buf_pd_x, BS_max_in_line_buf_led_x);
    if (BS_inlineUsed_x_pd == 0) {
        TRACE_ERROR("ERROR! BS_multi_malloc..BS_inlineUsed_x_pd");
        return 1; //error
    }
    TRACE_MALLOC("BS_inlineUsed_x_pd=,%x,%x,%x,", BS_inlineUsed_x_pd, (sizeof(uint8_t *)*BS_max_in_line_buf_pd_x) + (sizeof(uint8_t)*BS_max_in_line_buf_pd_x*BS_max_in_line_buf_led_x), BS_end_of_heap);

    ///////////////////////////////////////////////////////////////////////////////////
    /// uint8_t **BS_inlineUsed_y_pd; //[MAX_LINE_USED_BUF_PD_Y][MAX_LINE_USED_BUF_LED_Y]
    ///////////////////////////////////////////////////////////////////////////////////
    BS_inlineUsed_y_pd = (uint8_t **)BS_malloc_uint8_t2(BS_max_in_line_buf_pd_y, BS_max_in_line_buf_led_y);
    if (BS_inlineUsed_y_pd == 0) {
        TRACE_ERROR("ERROR! BS_multi_malloc..BS_inlineUsed_y_pd");
        return 1; //error
    }
    TRACE_MALLOC("BS_inlineUsed_y_pd=,%x,%x,%x,", BS_inlineUsed_y_pd, (sizeof(uint8_t *)*BS_max_in_line_buf_pd_y) + (sizeof(uint8_t)*BS_max_in_line_buf_pd_y*BS_max_in_line_buf_led_y), BS_end_of_heap);
#else
    ///////////////////////////////////////////////////////////////////////////////////
    /// uint8_t **BS_inlineUsed2_x_pd; //[MAX_LINE_USED_BUF_PD_X][MAX_LINE_USED_BUF_LED_X]
    ///////////////////////////////////////////////////////////////////////////////////
    BS_inlineUsed2_x_pd = (uint8_t **)BS_malloc_uint8_t2(BS_max_in_line_buf_pd_x, BS_max_in_line_buf_led_x);
    if (BS_inlineUsed2_x_pd == 0) {
        TRACE_ERROR("ERROR! BS_multi_malloc..BS_inlineUsed2_x_pd");
        return 1; //error
    }
    TRACE_MALLOC("BS_inlineUsed2_x_pd=,%x,%x,%x,", BS_inlineUsed2_x_pd, (sizeof(uint8_t *)*BS_max_in_line_buf_pd_x) + (sizeof(uint8_t)*BS_max_in_line_buf_pd_x*BS_max_in_line_buf_led_x), BS_end_of_heap);

    ///////////////////////////////////////////////////////////////////////////////////
    /// uint8_t **BS_inlineUsed2_y_pd; //[MAX_LINE_USED_BUF_PD_Y][MAX_LINE_USED_BUF_LED_Y]
    ///////////////////////////////////////////////////////////////////////////////////
    BS_inlineUsed2_y_pd = (uint8_t **)BS_malloc_uint8_t2(BS_max_in_line_buf_pd_y, BS_max_in_line_buf_led_y);
    if (BS_inlineUsed2_y_pd == 0) {
        TRACE_ERROR("ERROR! BS_multi_malloc..BS_inlineUsed2_y_pd");
        return 1; //error
    }
    TRACE_MALLOC("BS_inlineUsed2_y_pd=,%x,%x,%x,", BS_inlineUsed2_y_pd, (sizeof(uint8_t *)*BS_max_in_line_buf_pd_y) + (sizeof(uint8_t)*BS_max_in_line_buf_pd_y*BS_max_in_line_buf_led_y), BS_end_of_heap);
#endif

    ///////////////////////////////////////////////////////////////////////////////////
    /// ep2_no_t *BS_edge_pattern_virtul_ep; //[MAX_VIRTUAL_EP_COUNT]
    ///////////////////////////////////////////////////////////////////////////////////
    BS_max_virtual_ep_count = GET_MAX(BS_maxHorPdNum, BS_maxVerPdNum);
    BS_edge_pattern_virtul_ep = (ep2_no_t *)BS_malloc(sizeof(ep2_no_t) * BS_max_virtual_ep_count);
    if (BS_edge_pattern_virtul_ep == 0) {
        TRACE_ERROR("ERROR! BS_multi_malloc..BS_edge_pattern_virtul_ep");
        return 1; //error
    }
    TRACE_MALLOC("BS_edge_pattern_virtul_ep=,%x,%x,%x,", &BS_edge_pattern_virtul_ep[0], sizeof(ep2_no_t) * BS_max_virtual_ep_count, BS_end_of_heap);

    ///////////////////////////////////////////////////////////////////////////////////
    /// vec_e_t **BS_edge_pattern_virtul_poly; //[MAX_VIRTUAL_EP_COUNT][INITIAL_POLYGON_VERTEX_NO]
    ///////////////////////////////////////////////////////////////////////////////////
    BS_edge_pattern_virtul_poly = (vec_e_t **)malloc_vec_e_t2(BS_max_virtual_ep_count, INITIAL_POLYGON_VERTEX_NO);
    if (BS_edge_pattern_virtul_poly == 0) {
        TRACE_ERROR("ERROR! BS_multi_malloc..BS_edge_pattern_virtul_poly");
        return 1; //error
    }
    TRACE_MALLOC("BS_edge_pattern_virtul_poly=,%x,%x,%x,", BS_edge_pattern_virtul_poly, (sizeof(vec_e_t *)*BS_max_virtual_ep_count + sizeof(vec_e_t)*BS_max_virtual_ep_count*INITIAL_POLYGON_VERTEX_NO), BS_end_of_heap);

    return 0; //no-error
}
#endif


////////////////////////////////////////////
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_addEpaValueMulti	0
#endif
ATTR_BACKEND_RAMFUNC
static int addEpaValueMulti(axis_t axis, sen_type_t senType, ep_buf_t *epa, int start, int end)
{
    int i;
#ifdef USE_CUST_MALLOC //nsmoon@201012
    int maxNumPd = (axis == ENUM_HOR_X) ? BS_maxHorPdNum-1 : BS_maxVerPdNum-1; //nsmoon@201014
    if (start < 0 || end < 0 || start > maxNumPd || end > maxNumPd /*|| MAX_NUM_PD != 256*/) {
#else
    if (start < 0 || end < 0 || start >= MAX_NUM_PD || end >= MAX_NUM_PD /*|| MAX_NUM_PD != 256*/) {
#endif
        TRACE_ERROR("ERROR! addEpaValueMulti..invalid start or end %d %d [%d]", start, end, BG_frame_no);
        return -1; //error
    }

#if 0 //nsmoon@200110
    int slopeVal = start - end;
    int maxOfstVal = (axis == ENUM_HOR_X) ? BS_slopeValMaxX : BS_slopeValMaxY;
    int ofstIdx = slopeVal + maxOfstVal;
#endif
    //TRACE("epa->len= %d", epa->len);

    if (epa->len == 0)
    {
#ifdef USE_CUST_MALLOC //nsmoon@201012
        if (BS_edge_pattern_ep_cnt < BS_max_ep_count) { //nsmoon@201014
#else
        if (BS_edge_pattern_ep_cnt < MAX_EP_COUNT) {
            epa->poly = &BS_edge_pattern_poly[BS_edge_pattern_ep_cnt]; //nsmoon@201008
#endif
            epa->ep = (ep_no_t *)&BS_edge_pattern_ep[BS_edge_pattern_ep_cnt];
            epa->dz = (uint8_t *)&BS_edge_pattern_deadzone[BS_edge_pattern_ep_cnt]; //nsmoon@190326
#if (DEBUG_addEpaValueMulti == 1)
			IS_DEBUG_FLAG{
            TRACE("senType,start,end,epa->ep: %d %d %d %x", senType, start, end, &epa->ep[0]);
            TRACE("addEpaValueMulti..epa->ep[]: %d %d %x %d", start, end, &epa->ep[0], BS_edge_pattern_ep_cnt);
			};
#endif
            epa->dz[epa->len] = 0; //init
            epa->ep[epa->len].s1 = (uint8_t)start; //led
            epa->ep[epa->len].e1 = (uint8_t)end; //pd
        epa->ep[epa->len].s2 = (uint8_t)start;
        epa->ep[epa->len].e2 = (uint8_t)end;
        epa->len++;
            BS_edge_pattern_ep_cnt++;
#if (DEBUG_addEpaValueMulti == 1)
			IS_DEBUG_FLAG{
        i = (epa->len - 1);
            TRACE("@N0,%d,%d,%d,%d,:,%d,%d,%d,%d,(,%d,)", senType, start, end, i, \
                epa->ep[i].s1, epa->ep[i].e1, epa->ep[i].s2, epa->ep[i].e2, BS_edge_pattern_ep_cnt - 1);
			};
#endif
            return 0; //new ep
        }
        else {
			TRACE_ERROR_MEM("ERROR_MEM!1 invalid BS_edge_pattern_ep_cnt %d [%d]", BS_edge_pattern_ep_cnt, BG_frame_no);
            return -1; //mem-error
        }
    } //if (epa->len == 0)

    for (i = 0; i < epa->len; i++)
    {
        //TRACE("&epa->ep[i]= (%d %d) %x %x", i, epa->len, epa, &epa->ep[i]);
        if ((senType == ENUM_PD && (epa->ep[i].s1 - 1) == start)
            || (senType == ENUM_LED && (epa->ep[i].e1 - 1) == end))
        {
            //replace s1 or e1
            epa->ep[i].s1 = (uint8_t)start;
            epa->ep[i].e1 = (uint8_t)end;
#if (DEBUG_addEpaValueMulti == 1)
			IS_DEBUG_FLAG{
            TRACE("@C1+,%d,%d,%d,%d,:,%d,%d,%d,%d", senType, start, end, i, \
                epa->ep[i].s1, epa->ep[i].e1, epa->ep[i].s2, epa->ep[i].e2);
			};
#endif
            return i; //replace ep
        }
        if ((senType == ENUM_PD && (epa->ep[i].s2 + 1) == start)
            || (senType == ENUM_LED && (epa->ep[i].e2 + 1) == end))
        {
            //replace s2 or e2
            epa->ep[i].s2 = (uint8_t)start;
            epa->ep[i].e2 = (uint8_t)end;
#if (DEBUG_addEpaValueMulti == 1)
			IS_DEBUG_FLAG{
            TRACE("@C2+,%d,%d,%d,%d,:,%d,%d,%d,%d", senType, start, end, i, \
                epa->ep[i].s1, epa->ep[i].e1, epa->ep[i].s2, epa->ep[i].e2);
			};
#endif
            return i; //replace ep
        }
        else if ((senType == ENUM_PD && epa->ep[i].s1 <= start && epa->ep[i].s2 >= start)
            || (senType == ENUM_LED && epa->ep[i].e1 <= end && epa->ep[i].e2 >= end))
        {
#if 0 //(DEBUG_addEpaValueMulti == 1)
			IS_DEBUG_FLAG{
				TRACE("@--,%d,%d,%d,%d,:,%d,%d,%d,%d", senType, start, end, i, \
				epa->ep[i].s1, epa->ep[i].e1, epa->ep[i].s2, epa->ep[i].e2);
			};
#endif            
            return -1; //ignore
        }
    } //for (i = 0; i < epa[slopeLedIdx].len; i++)

    //empty, new0
#ifdef USE_CUST_MALLOC //nsmoon@201012
    if (BS_edge_pattern_ep_cnt < BS_max_ep_count) { //nsmoon@201014
#else
    if (BS_edge_pattern_ep_cnt < MAX_EP_COUNT) {
#endif
        epa->dz[epa->len] = 0; //init
    epa->ep[epa->len].s1 = (uint8_t)start;
    epa->ep[epa->len].e1 = (uint8_t)end;
    epa->ep[epa->len].s2 = (uint8_t)start;
    epa->ep[epa->len].e2 = (uint8_t)end;
    epa->len++;
        BS_edge_pattern_ep_cnt++;
#if (DEBUG_addEpaValueMulti == 1)
		IS_DEBUG_FLAG{
    i = (epa->len - 1);
        TRACE("@N1,%d,%d,%d,%d,:,%d,%d,%d,%d,(,%d,)", senType, start, end, i, \
            epa->ep[i].s1, epa->ep[i].e1, epa->ep[i].s2, epa->ep[i].e2, BS_edge_pattern_ep_cnt - 1);
		};
#endif
#if 0
    // ascending sorting
    for (i = 0; i < (epa->len - 1); i++) {
        for (j = (i + 1); j < epa->len; j++) {
            if ((epaType == 1 && epa->ep[i].s1 > epa->ep[j].s1) || (epaType == 0 && epa->ep[i].e1 > epa->ep[j].e1)) {
                epTmp = epa->ep[i];
                epa->ep[i] = epa->ep[j];
                epa->ep[j] = epTmp;
            }
        }
    }
#endif //0
    }
    else {
		TRACE_ERROR_MEM("ERROR_MEM!2 invalid BS_edge_pattern_ep_cnt %d [%d]", BS_edge_pattern_ep_cnt, BG_frame_no);
        return -1; //mem-error
    }

    return (epa->len - 1); //new ep
}

#if 0 //nsmoon@190926
static int mergeEpaValueMulti(sen_type_t senType, ep_buf_t *epa)
{
	//FIXME!! not completed yes, still has bug!!!
	int i, j, k, merged;
#if 0 //(DEBUG_addEpaValueMulti == 1)
		IS_DEBUG_FLAG{
			TRACE("mergeEpaValueMulti..");
			for (i = 0; i < epa->len; i++)
			{
				TRACE("..%d-%d %d-%d", epa->ep[i].s1, epa->ep[i].s2, epa->ep[i].e1, epa->ep[i].e2);
			}
		};
#endif
	for (i = 0; i < (epa->len - 1); i++)
	{
		for (j = (i + 1); j < epa->len; j++)
		{
			merged = 0;
			if (senType == ENUM_PD) {
				if ((epa->ep[i].s1 >= (epa->ep[j].s1 - 1) && epa->ep[i].s1 <= (epa->ep[j].s2 + 1)) ||
					(epa->ep[i].s2 >= (epa->ep[j].s1 - 1) && epa->ep[i].s2 <= (epa->ep[j].s2 + 1)) ||
					(epa->ep[j].s1 >= (epa->ep[i].s1 - 1) && epa->ep[j].s1 <= (epa->ep[i].s2 + 1)) ||
					(epa->ep[j].s2 >= (epa->ep[i].s1 - 1) && epa->ep[j].s2 <= (epa->ep[i].s2 + 1))) {
					epa->ep[i].s1 = GET_MIN(epa->ep[i].s1, epa->ep[j].s1);
					epa->ep[i].s2 = GET_MAX(epa->ep[i].s2, epa->ep[j].s2);
					merged++;
				}
			}
			else {
				if ((epa->ep[i].e1 >= (epa->ep[j].e1 - 1) && epa->ep[i].e1 <= (epa->ep[j].e2 + 1)) ||
					(epa->ep[i].e2 >= (epa->ep[j].e1 - 1) && epa->ep[i].e2 <= (epa->ep[j].e2 + 1)) ||
					(epa->ep[j].e1 >= (epa->ep[i].e1 - 1) && epa->ep[j].e1 <= (epa->ep[i].e2 + 1)) ||
					(epa->ep[j].e2 >= (epa->ep[i].e1 - 1) && epa->ep[j].e2 <= (epa->ep[i].e2 + 1))) {
					epa->ep[i].e1 = GET_MIN(epa->ep[i].e1, epa->ep[j].e1);
					epa->ep[i].e2 = GET_MAX(epa->ep[i].e2, epa->ep[j].e2);
					merged++;
				}
			}
			if (merged) {
#if (DEBUG_addEpaValueMulti == 1)
				IS_DEBUG_FLAG{
					TRACE("@M0,%d-%d %d-%d", epa->ep[i].s1, epa->ep[i].s2, epa->ep[i].e1, epa->ep[i].e2);
				};
#endif
				if (j < (epa->len - 1)) {
					for (k = j; k < (epa->len - 1); k++) {
						epa->ep[k] = epa->ep[k + 1];
					}
					epa->len--;
				}
				else {
					epa->len--;
				}
			}
		}
	}
	return 0;
}
#endif

#if 0 //(SECOND_OFST_TBL == 1) //nsmoon@190916
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_add_ofst2_sensor_deadline		1
#endif
#if 0 //for testing
static int add_ofst2_sensor_line(axis_t axis, int led, int pd)
{
	int lineBufIdx, ledMask, maxInLineBufLed;
	uint8_t *inlineStat;

	if (axis == (int)ENUM_HOR_X) {
		inlineStat = LINE_STAT_BUF_ADDR_X;
		maxInLineBufLed = MAX_IN_LINE_BUF_LED_X;
	}
	else {
		inlineStat = LINE_STAT_BUF_ADDR_Y;
		maxInLineBufLed = MAX_IN_LINE_BUF_LED_Y;
	}

	//stat table
	lineBufIdx = GET_IN_LINE_IDX(pd, led);
	ledMask = GET_LED_BIT_MASK(led);
	inlineStat[lineBufIdx] &= ~(uint8_t)ledMask; //make used line
    TRACE_NOP;

	return 0; //not-found
}
#endif

static int addEpaValueMulti_2ndOfst(axis_t axis, sen_type_t sensor, ep_buf_t *epa, int led)
{
	int slopeValMax, slopeValMax2;
	int pd, stPd, edPd, maxPd;
	uint8_t *senTbl2;
	int senTblLen2;
	int i, k, m;
    int cnt = 0;

	if (sensor == ENUM_PD) {
		return 0;
	}

	if (axis == ENUM_HOR_X) {
		slopeValMax = BS_slopeValMaxX;
		slopeValMax2 = BS_slopeValMaxX2;
		maxPd = BS_maxHorPdNum;
		senTbl2 = &BS_senTblX2[0];
		senTblLen2 = BS_senTblLenX2;
	}
	else {
		slopeValMax = BS_slopeValMaxY;
		slopeValMax2 = BS_slopeValMaxY2;
		maxPd = BS_maxVerPdNum;
		senTbl2 = &BS_senTblY2[0];
		senTblLen2 = BS_senTblLenY2;
	}

	for (i = 0; i < senTblLen2; i++) {
		if (led == senTbl2[i]) {
			for (k = 0; k < 2; k++) {
				pd = (k) ? (led - (slopeValMax2 + 1)) : (led + (slopeValMax2 + 1));
				if (pd < 0 || pd >= maxPd) continue;

				stPd = (k) ? GET_MAX((led - slopeValMax), 0) : pd;
				edPd = (k) ? pd : GET_MIN((led + slopeValMax), (maxPd - 1));
				for (m = stPd; m <= edPd; m++) {
					//TRACE("@@@, %d, %d, %d", sensor, led, m); //FIXME!!!
					BS_set_line_buffer(axis, led, m); //nsmoon@190926, SECOND_OFST_TBL
                    addEpaValueMulti(axis, sensor, epa, led, m);
                    cnt++;
				}
			} //for (k = 0; k < 2; k++)

			return 1; //found
		} //if (led == senTbl2[i])
	} //for (i = 0; i < senTblLen2; i++)

    return cnt; //not-found
}
#endif

#if (DEAD_CELL_TBL == 1) //nsmoon@190926
static int addEpaValueMulti_deadCell(axis_t axis, sen_type_t sensor, ep_buf_t *epa, int led)
{
    int slopeValMax;
    int stPd, edPd, maxPd;
    uint8_t *deadTbl;
    int deadTblLen;
    int i, m;
    int cnt = 0;

    if (sensor == ENUM_PD) {
        return 0;
    }

    if (axis == ENUM_HOR_X) {
        slopeValMax = BS_slopeValMaxX;
        maxPd = BS_maxHorPdNum;
        deadTbl = BS_inValidTblXled;
        deadTblLen = BS_inValidTblLenXled;
    }
    else {
        slopeValMax = BS_slopeValMaxY;
        maxPd = BS_maxVerPdNum;
        deadTbl = BS_inValidTblYled;
        deadTblLen = BS_inValidTblLenYled;
    }

    for (i = 0; i < deadTblLen; i++) {
        if (led == deadTbl[i]) {
            stPd = GET_MAX((led - slopeValMax), 0);
            edPd = GET_MIN((led + slopeValMax), (maxPd - 1));
            for (m = stPd; m <= edPd; m++) {
                //TRACE("@@@, %d, %d, %d", sensor, led, m); //FIXME!!!
				BS_set_line_buffer(axis, led, m); //nsmoon@190926, DEAD_CELL_TBL
                addEpaValueMulti(axis, sensor, epa, led, m);
                cnt++;
            }
            return 1; //found
        } //if (led == senTbl2[i])
    } //for (i = 0; i < senTblLen2; i++)

    return cnt; //not-found
}
#endif

#if 0 //reserved, do not remove
int BS_packEdgePattern(axis_t axis, int mode)
{
    //mode: 0:inBuf, 1:remained
    int pd, led, ledSt, ledEd, pdSt, pdEd;
    ep_buf_t *epa = 0;
    int maxPdNum, maxSlopeVal; //slopeVal,
    int inBufLen, inBufIdx0, inBufIdx, inBufIdxSt, inBufIdxEd;
    uint16_t *remLine;
    int remLineCnt, remLineIdx, remLineIdxSt, remLineIdxEd;
    int cnt = 0;

    if (axis == ENUM_HOR_X) {
        maxPdNum = BS_maxHorPdNum - 1;
        maxSlopeVal = BS_slopeValMaxX;
        epa = EPA_ADDR_X;
        inBufLen = BS_inBuf->hor_len;
        remLine = &BS_remained_x[0];
        remLineCnt = BS_remained_x_cnt;
    }
    else {
        maxPdNum = BS_maxVerPdNum - 1;
        maxSlopeVal = BS_slopeValMaxY;
        epa = EPA_ADDR_Y;
        inBufLen = BS_inBuf->ver_len;
        remLine = &BS_remained_y[0];
        remLineCnt = BS_remained_y_cnt;
    }

    cnt = 0;
    remLineIdxSt = inBufIdxSt = 0;
    if (mode) {
        remLineIdxEd = remLineCnt;
        inBufIdxEd = 1;
    }
    else {
        remLineIdxEd = 1;
        inBufIdxEd = inBufLen;
    }

    for (remLineIdx = remLineIdxSt; remLineIdx < remLineIdxEd; remLineIdx++)
    {
        for (inBufIdx = inBufIdxSt; inBufIdx < inBufLen; inBufIdx++) {
            if (mode) {
                inBufIdx0 = remLine[remLineIdx];
            }
            else {
                inBufIdx0 = inBufIdx;
            }
            if (BS_getSenInBuf(axis, inBufIdx0, &pd, &led)) {
                continue;
            }
            addEpaValueMulti(axis, ENUM_PD, &epa[pd], led, pd);
            mergeEpaValueMulti(ENUM_PD, &epa[pd]); //nsmoon@190926
        }
    }

    for (pd = 0; pd < maxPdNum; pd++) {
        cnt += epa[pd].len;
    }
    TRACE("BS_packEdgePattern...%d-%d %d", axis, mode, cnt);

    return cnt;
}
#endif //1

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define TRACE_EPMI(...)	//TRACE(__VA_ARGS__)
#else
#define TRACE_EPMI(...)
#endif
ATTR_BACKEND_RAMFUNC
int BS_packEdgePatternMulti_ind(bd_type_t bd)
{
	int pd, led;
    axis_t axis;
    sen_type_t sensor, sensorOpp;
    ep_buf_t *epa = 0;
    uint8_t *inBuf;
    int maxPdNum, maxSlopeVal; //slopeVal,
    int inBufIdx, ledMask;
	int maxInLineBufLed;
    int cnt = 0;
#if (DEAD_CELL_TBL == 1) || (SECOND_OFST_TBL == 1) //nsmoon@190926
    int ret;
#endif
    IS_DEBUG_FLAG {
	TRACE_EPMI("BS_packEdgePatternMulti_ind... %d", bd);
        TRACE_NOP;
    };

    if (bd == ENUM_BOT_BD) {
        axis = ENUM_HOR_X;
        sensor = ENUM_LED;
        inBuf = IN_LINE_BUF_ADDR_X_LED;
    }
    else if (bd == ENUM_TOP_BD) {
        axis = ENUM_HOR_X;
        sensor = ENUM_PD;
        inBuf = IN_LINE_BUF_ADDR_X_PD;
    }
    else if (bd == ENUM_LFT_BD) {
        axis = ENUM_VER_Y;
        sensor = ENUM_LED;
        inBuf = IN_LINE_BUF_ADDR_Y_LED;
    }
    else {
        //(bd == ENUM_RHT_BD)
        axis = ENUM_VER_Y;
        sensor = ENUM_PD;
        inBuf = IN_LINE_BUF_ADDR_Y_PD;
    }

    if (axis == ENUM_HOR_X) {
        maxPdNum = BS_maxHorPdNum - 1;
        maxSlopeVal = BS_slopeValMaxX;
        epa = EPA_ADDR_X;
#ifdef USE_CUST_MALLOC //nsmoon@201012
        maxInLineBufLed = BS_max_in_line_buf_led_x; //nsmoon@201014
#else
        maxInLineBufLed = MAX_IN_LINE_BUF_LED_X;
#endif
    }
    else {
        maxPdNum = BS_maxVerPdNum - 1;
        maxSlopeVal = BS_slopeValMaxY;
        epa = EPA_ADDR_Y;
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
    sensorOpp = (sensor == ENUM_PD) ? ENUM_LED : ENUM_PD;

#if 0 //(SECOND_OFST_TBL == 1) //nsmoon@190916
	if (axis == ENUM_HOR_X) {
		BS_senTblLenX2cnt = 0;
	}
	else {
		BS_senTblLenY2cnt = 0;
	}
#endif
    TRACE_EPMI("epa in BS_packEdgePatternMulti_ind(axis): %x (%d) ", epa, axis);
	TRACE_EPMI("inBuf in BS_packEdgePatternMulti_ind(axis): %x (%d)", inBuf, axis);

#ifdef MULTI_SEN_CNT_EVEN_SKIP_ENABLE_2 //nsmoon@200330
    BS_multi_even_skip_en = (senCnt > MAX_MULTI_SEN_CNT_SKIP) ? 1 : 0;
#endif

#ifdef USE_SENSOR_LIST //for test, nsmoon@200327
    //reset ep[].len //nsmoon@201015-bugfix
    for (pd = 0; pd <= maxPdNum; pd++) {
        epa[pd].len = 0; //reset
    }

        for (senIdx = 0; senIdx < senCnt; senIdx++) {
            pd = senList[senIdx];
#ifdef MULTI_SEN_CNT_EVEN_SKIP_ENABLE_2 //nsmoon@200330
            if (BS_multi_even_skip_en) {
                if ((pd % 2) == 0) continue;
            }
#endif
#else
    for (pd = 0; pd <= maxPdNum; pd++) {
        epa[pd].len = 0; //reset //nsmoon@201015-bugfix
#endif
#if (DEAD_CELL_TBL == 1) //nsmoon@190926
			ret = addEpaValueMulti_deadCell(axis, sensor, &epa[pd], pd);
			if (ret > 0) {
				cnt += ret;
				continue;
			}
#endif

        int ledSt = pd - maxSlopeVal;
        int ledEd = pd + maxSlopeVal;
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
                if (sensor == ENUM_PD) {
				TRACE_EPMI(".1.pd,led,lineBufIdx,ledMask: (%d/%d),%d,%d,%d,%x,%x", axis, sensor, pd, led, inBufIdx, ledMask, inBuf[inBufIdx]);
                addEpaValueMulti(axis, sensor, &epa[pd], led, pd);
					}
                else {
                    TRACE_EPMI(".2.pd,led,lineBufIdx,ledMask: (%d/%d),%d,%d,%d,%x,%x", axis, sensor, led, pd, inBufIdx, ledMask, inBuf[inBufIdx]);
                    addEpaValueMulti(axis, sensor, &epa[pd], pd, led);
				}
                cnt++;
#if (DEAD_CELL_TBL == 1) //nsmoon@190926
                int adjLed2 = BS_is_near_dead(axis, sensorOpp, led, 0); //nsmoon@191202
                if (adjLed2 >= 0) {
                    int slopeVal = GET_ABS(adjLed2 - pd);
                    if (slopeVal <= maxSlopeVal) {
                        TRACE_EPMI("@@, %d, %d, %d", sensor, adjLed2, pd);
                        if (sensor == ENUM_PD) {
						BS_set_line_buffer(axis, adjLed2, pd); //nsmoon@190926, DEAD_CELL_TBL
                        addEpaValueMulti(axis, sensor, &epa[pd], adjLed2, pd);
            }
            else {
                            BS_set_line_buffer(axis, pd, adjLed2); //nsmoon@190926, DEAD_CELL_TBL
                            addEpaValueMulti(axis, sensor, &epa[pd], pd, adjLed2);
                }
						cnt++;
					}
				}
#endif
            } //for (led = ledSt; led <= ledEd; led++)
        } //for (byteIdx = byteIdxSt; byteIdx <= byteIdxEd; byteIdx++)
        //mergeEpaValueMulti(sensor, &epa[pd]); //nsmoon@200406 do not need when led is sorted
    } //for (pd = 0; pd <= maxPdNum; pd++) 

    if (cnt > 0) {
            BS_addEpaPosMulti(bd);
    }
    return cnt;
}

ATTR_BACKEND_RAMFUNC
static void getTouchMinMax(poly_e pe, pos_min_max_t *minMax)
{
    float minX, maxX, minY, maxY;
    int k;

    minX = pe->ve[0].vert.x;
    maxX = pe->ve[0].vert.x;
    minY = pe->ve[0].vert.y;
    maxY = pe->ve[0].vert.y;
    for (k = 1; k < pe->len; k++) {
        if (minX > pe->ve[k].vert.x) minX = pe->ve[k].vert.x;
        if (maxX < pe->ve[k].vert.x) maxX = pe->ve[k].vert.x;
        if (minY > pe->ve[k].vert.y) minY = pe->ve[k].vert.y;
        if (maxY < pe->ve[k].vert.y) maxY = pe->ve[k].vert.y;
    }

    minMax->minX = minX;
    minMax->maxX = maxX;
    minMax->minY = minY;
    minMax->maxY = maxY;
}

////////////////////////////////////////////
#if 1 //clipping_var
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_clippingPacksPerPolyNo    1
#endif
#define PROCESSED_CNT_INITIAL   40 //48
#define INITIAL_RESULT_VAL		2 //3 FIXME nsmoon@200129

#ifndef POLY_VERTEX_LIMIT_ENABLE //nsmoon@200303
ATTR_BACKEND_RAMFUNC
static uint8_t clippingPacksPerPolyNo(void)
{
    uint8_t result;
#if 1 //nsmoon@190322, FIXME
    if (BS_pclipping_info.totalProcessedSlopeCnt <= PROCESSED_CNT_INITIAL) {
        result = INITIAL_RESULT_VAL; //1
    }
    else {
        result = INITIAL_RESULT_VAL + (BS_pclipping_info.totalProcessedSlopeCnt / PROCESSED_CNT_INITIAL);
        result = GET_MIN(result, MAX_RESULT_PACK_NO);
        }
    int i, len = 0, maxVertexTmp = 0;
    for (i = 0; i < BS_pclipping_info.subject_cnt; i++)
    {
        len = BS_pclipping_info.subject_length[i];
		maxVertexTmp = GET_MAX(maxVertexTmp, len);
    }
	if (maxVertexTmp >(result * MAX_RESULT_VERTEX_NO)) {
		TRACE_ERROR_MEM("ERROR_MEM! max,result: %d %d (%d)", maxVertexTmp, result, BS_pclipping_info.totalProcessedSlopeCnt);
        result++;
        result = GET_MIN(result, MAX_RESULT_PACK_NO);
	}
#if (DEBUG_clippingPacksPerPolyNo == 1)
	maxVertex = GET_MAX(maxVertexTmp, maxVertex); //nsmoon@190328
    maxProcessCnt = GET_MAX(maxProcessCnt, BS_pclipping_info.totalProcessedSlopeCnt);
    maxSubjCnt = GET_MAX(maxSubjCnt, BS_pclipping_info.subject_cnt);
    //TRACE("totalProcessedSlopeCnt: %d/%d (%d/%d) %d/%d", BS_pclipping_info.totalProcessedSlopeCnt, maxProcessCnt, maxVertex, (result * MAX_RESULT_VERTEX_NO), (result * BS_pclipping_info.subject_cnt), maxSubjCnt);
#endif
#endif //1
#if 0 //old-method
    if (BS_pclipping_info.totalProcessedSlopeCnt <= 4) result = 1;
    else if (BS_pclipping_info.totalProcessedSlopeCnt <= 8) result = 2;
    else if (BS_pclipping_info.totalProcessedSlopeCnt <= 16) result = 4;
    else if (BS_pclipping_info.totalProcessedSlopeCnt <= 28) result = 7;
    else result = MAX_RESULT_PACK_NO;
#endif
    return result;
}
#endif

ATTR_BACKEND_RAMFUNC
static void clipping_var_init(void)
{
    BS_pclipping_info.maxPolygonErrCnt = 0;
    BS_pclipping_info.minTouchAreaErrCnt = 0;
    BS_pclipping_info.noClippingErrCnt = 0;
    BS_pclipping_info.totalProcessedSlopeCnt = 0;
    BS_pclipping_info.storage = CURRENT;
    BS_pclipping_info.polygon_length = &BS_result_polygon_length[0];
    BS_pclipping_info.polygon_vertex = BS_result_polygon_vertex;
    BS_pclipping_info.polygon_area = &BS_result_area[0];
    BS_pclipping_info.eraser_pen = 0; //init, nsmoon@200306
    BS_pclipping_info.no_clip_y = 0; //nsmoon@210308

    //current polygon_cnt
    BS_pclipping_info.polygon_cnt = 0;
    BS_pclipping_info.subject_cnt = 0; //nsmoon@190328

    //init variables related to polygon_cnt
    //???

#ifdef DEBUG_PCNT_FINE2
    if (BS_pclipping_info.polygon_vertex != BS_result_polygon_vertex || BS_pclipping_info.polygon_length != BS_result_polygon_length) {
        TRACE_ERROR("Error! BS_pclipping_info.storage %d,BS_pclipping_info.polygon_vertex %x (%x), BS_pclipping_info.polygon_length %x (%x)",
            BS_pclipping_info.storage, BS_pclipping_info.polygon_vertex, BS_result_polygon_vertex, BS_pclipping_info.polygon_length, BS_result_polygon_length);
    }
#endif
}

#if 0
ATTR_BACKEND_RAMFUNC
static void clipping_var_polygon_cnt_increase(void)
{
    if (BS_pclipping_info.polygon_cnt < MAX_RESULT_POLYGON_NO) {
        BS_pclipping_info.polygon_cnt++;
        //init variables related to polygon_cnt
    }
    else {
        TRACE_ERROR("ERROR! invlaid BS_pclipping_info.polygon_cnt %d", BS_pclipping_info.polygon_cnt);
    }
}
#endif

//static void change_clipping_var(int start)
ATTR_BACKEND_RAMFUNC
static void clipping_var_reset(void)
{
    BS_pclipping_info.subject_packs = BS_pclipping_info.polygon_packs;
    BS_pclipping_info.subject_cnt = BS_pclipping_info.polygon_cnt;
    if (BS_pclipping_info.totalProcessedSlopeCnt < UINT8_MAX) {
        BS_pclipping_info.totalProcessedSlopeCnt++;
    }
    else {
        TRACE_ERROR("ERROR! clipping_var_reset..invalid totalProcessedSlopeCnt %d", BS_pclipping_info.totalProcessedSlopeCnt);
    }
#ifdef POLY_VERTEX_LIMIT_ENABLE //nsmoon@200303
    BS_pclipping_info.polygon_packs = 2;
#else
    BS_pclipping_info.polygon_packs = clippingPacksPerPolyNo();
#endif

    //init polygon_cnt, polygon_unique_edge and polygon_clipping_cnt
    BS_pclipping_info.polygon_cnt = 0; //current polygon_cnt
    BS_pclipping_info.maxPolygonCnt = (int16_t)(MAX_RESULT_POLYGON_NO / BS_pclipping_info.polygon_packs);
    BS_pclipping_info.poly_idx_saved[0] = -1; //empty

#ifdef DEBUG_PCNT_FINE2
    if (BS_pclipping_info.polygon_vertex != BS_result_polygon_vertex || BS_pclipping_info.polygon_length != BS_result_polygon_length) {
        TRACE_ERROR("Error! BS_pclipping_info.storage %d, BS_pclipping_info.polygon_vertex %x (%x), BS_pclipping_info.polygon_length %x (%x)",
            BS_pclipping_info.storage, BS_pclipping_info.polygon_vertex, BS_result_polygon_vertex, BS_pclipping_info.polygon_length, BS_result_polygon_length);
    }
#endif
}

ATTR_BACKEND_RAMFUNC
static void clipping_var_swap(void)
{
    BS_pclipping_info.storage = (BS_pclipping_info.storage == PREVIOUS) ? CURRENT : PREVIOUS;
    if (BS_pclipping_info.storage == CURRENT) {
        BS_pclipping_info.subject_length = &BS_prev_polygon_length[0];
        BS_pclipping_info.subject_vertex = BS_prev_polygon_vertex;
        //BS_pclipping_info.subject_unique_edge = &BS_prev_unique_edge[0];
        //BS_pclipping_info.subject_clipping_cnt_x = &BS_prev_clipping_cnt_x[0];
        //BS_pclipping_info.subject_clipping_cnt_y = &BS_prev_clipping_cnt_y[0];
        //BS_pclipping_info.subject_ep_cnt = &BS_prev_ep_cnt[0];
        BS_pclipping_info.subject_area = &BS_prev_area[0];
        //BS_pclipping_info.subject_state = &BS_prev_state[0];
#ifdef MULTI_SENSOR_RANGE_OLD //nsmoon@200119
		BS_pclipping_info.subject_range = &BS_prev_sensor_range[0]; //nsmoon@190715
#endif

        BS_pclipping_info.polygon_length = &BS_result_polygon_length[0];
        BS_pclipping_info.polygon_vertex = BS_result_polygon_vertex;
        //BS_pclipping_info.polygon_unique_edge = &BS_result_unique_edge[0];
        //BS_pclipping_info.polygon_clipping_cnt_x = &BS_result_clipping_cnt_x[0];
        //BS_pclipping_info.polygon_clipping_cnt_y = &BS_result_clipping_cnt_y[0];
        //BS_pclipping_info.polygon_ep_cnt = &BS_result_ep_cnt[0];
        BS_pclipping_info.polygon_area = &BS_result_area[0];
        //BS_pclipping_info.polygon_state = &BS_result_state[0];
#ifdef MULTI_SENSOR_RANGE_OLD //nsmoon@200119
		BS_pclipping_info.polygon_range = &BS_result_sensor_range[0]; //nsmoon@190715
#endif
    }
    else { //PREVIOUS
        BS_pclipping_info.subject_length = &BS_result_polygon_length[0];
        BS_pclipping_info.subject_vertex = BS_result_polygon_vertex;
        //BS_pclipping_info.subject_unique_edge = &BS_result_unique_edge[0];
        //BS_pclipping_info.subject_clipping_cnt_x = &BS_result_clipping_cnt_x[0];
        //BS_pclipping_info.subject_clipping_cnt_y = &BS_result_clipping_cnt_y[0];
        //BS_pclipping_info.subject_ep_cnt = &BS_result_ep_cnt[0];
        BS_pclipping_info.subject_area = &BS_result_area[0];
        //BS_pclipping_info.subject_state = &BS_result_state[0];
#ifdef MULTI_SENSOR_RANGE_OLD //nsmoon@200119
		BS_pclipping_info.subject_range = &BS_result_sensor_range[0]; //nsmoon@190715
#endif

        BS_pclipping_info.polygon_length = &BS_prev_polygon_length[0];
        BS_pclipping_info.polygon_vertex = BS_prev_polygon_vertex;
        //BS_pclipping_info.polygon_unique_edge = &BS_prev_unique_edge[0];
        //BS_pclipping_info.polygon_clipping_cnt_x = &BS_prev_clipping_cnt_x[0];
        //BS_pclipping_info.polygon_clipping_cnt_y = &BS_prev_clipping_cnt_y[0];
        //BS_pclipping_info.polygon_ep_cnt = &BS_prev_ep_cnt[0];
        BS_pclipping_info.polygon_area = &BS_prev_area[0];
        //BS_pclipping_info.polygon_state = &BS_prev_state[0];
#ifdef MULTI_SENSOR_RANGE_OLD //nsmoon@200119
		BS_pclipping_info.polygon_range = &BS_prev_sensor_range[0]; //nsmoon@190715
#endif
    }
}

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_clipping_var_save_result	0 //1:enable
#endif
//static int clipping_var_save_result(poly_e_t *rese, poly_e_t *subj)
ATTR_BACKEND_RAMFUNC
static int clipping_var_save_result(poly_e_t *rese, poly_e_t *subj, int skip)
{
	//skip:1 copy subj to BS_pclipping_info.polygon_vertex[]
    int k, pIdx, vIdx, len;
    int  polygon_packs = BS_pclipping_info.polygon_packs;
    int  polygon_cnt = BS_pclipping_info.polygon_cnt;

#if 0 //def CLIPPING_CHK_ADJ_VERT, for test
    int skip = 0;
    if (rese != subj) {
        for (k = 0; k < rese->len; k++) {
            int tmpIdx = (k == 0) ? (rese->len - 1) : (k - 1);
            float diffX = GET_ABS(rese->ve[k].vert.x - rese->ve[tmpIdx].vert.x);
            float diffY = GET_ABS(rese->ve[k].vert.y - rese->ve[tmpIdx].vert.y);
            if ((diffX - MULTI_MIN_DIST_EDGE_POLYGON) < EPSILON && (diffY - MULTI_MIN_DIST_EDGE_POLYGON) < EPSILON) {
                //TRACE("skip:%0.3f,%0.3f", diffX, diffY);
                skip = 1;
                break;
            }
        }
    }
#endif

    len = (skip) ? subj->len : rese->len;
#if 1 //nsmoon@200206
    int maxLen = polygon_packs * MAX_RESULT_VERTEX_NO;
    if (len > maxLen) {
        TRACE_ERROR_MEM("ERROR_MEM!! exceed polygon_packs: %d (%d)", len, maxLen);
        len = maxLen;
        if (skip) {
            subj->len = len;
        }
        else {
            rese->len = len;
        }
    }
#endif
    if (polygon_cnt < MAX_RESULT_POLYGON_NO) {
        BS_pclipping_info.polygon_length[polygon_cnt] = (uint8_t)len;
    }
    else {
        TRACE_ERROR_MEM("ERROR_MEM! invalid polygon_cnt: %d [%d]", polygon_cnt, BG_frame_no);
        return 1;
    }

    for (k = 0; k < len; k++) {
        pIdx = (polygon_cnt * polygon_packs) + (k / MAX_RESULT_VERTEX_NO);
        vIdx = (k % MAX_RESULT_VERTEX_NO);
  #if (DEBUG_clipping_var_save_result == 1)
        TRACE("..polygon_cnt,packsPerPoly,k: %d %d %d => [%d][%d]", polygon_cnt, polygon_packs, k, pIdx, vIdx);
  #endif
        //if (pIdx < MAX_RESULT_POLYGON_NO && vIdx < MAX_RESULT_VERTEX_NO) 
		if (pIdx < MAX_RESULT_POLYGON_NO)
		{
            if (skip) {
                //TRACE("skip~");
                BS_pclipping_info.polygon_vertex[pIdx][vIdx].vert.x = subj->ve[k].vert.x;
                BS_pclipping_info.polygon_vertex[pIdx][vIdx].vert.y = subj->ve[k].vert.y;
                BS_pclipping_info.polygon_vertex[pIdx][vIdx].edgeId = subj->ve[k].edgeId;
            }
            else {
                BS_pclipping_info.polygon_vertex[pIdx][vIdx].vert.x = rese->ve[k].vert.x;
                BS_pclipping_info.polygon_vertex[pIdx][vIdx].vert.y = rese->ve[k].vert.y;
                BS_pclipping_info.polygon_vertex[pIdx][vIdx].edgeId = rese->ve[k].edgeId;
            }
        }
        else {
            TRACE_ERROR_MEM("ERROR_MEM! 1 invalid pIdx or vIdx: %d %d [%d]", pIdx, vIdx, BG_frame_no);
            return 1;
        }
  #if 0 //old-macro
        macT2P(BS_pclipping_info.polygon_vertex, polygon_cnt, k, polygon_packs).vert.x = rese->ve[k].vert.x;
        macT2P(BS_pclipping_info.polygon_vertex, polygon_cnt, k, polygon_packs).vert.y = rese->ve[k].vert.y;
        macT2P(BS_pclipping_info.polygon_vertex, polygon_cnt, k, polygon_packs).edgeId = rese->ve[k].edgeId;
        //(polygon_vertex[polygon_cnt*polygon_packs + k / 8][k % 8])
  #endif
    }

    //clipping_var_polygon_cnt_increase();
    if (polygon_cnt < MAX_RESULT_POLYGON_NO) {
        BS_pclipping_info.polygon_cnt++;
        //init variables related to polygon_cnt
        //???
    }
    else {
        TRACE_ERROR("ERROR! invlaid BS_pclipping_info.polygon_cnt %d [%d]", BS_pclipping_info.polygon_cnt, BG_frame_no);
        return 1;
    }
    return 0;
}

#if 0 //for testing
ATTR_BACKEND_RAMFUNC
static void clipping_var_save_as(poly_e_t *rese)
{
    int k, pIdx, vIdx;
    BS_pclipping_info.subject_length[BS_pclipping_info.subject_cnt] = rese->len;
    for (k = 0; k < rese->len; k++) {
        pIdx = BS_pclipping_info.subject_cnt * BS_pclipping_info.subject_packs + k / MAX_RESULT_VERTEX_NO;
        vIdx = k % MAX_RESULT_VERTEX_NO;
  #if (DEBUG_clipping_var_save_result == 1)
        TRACE("..subject_cnt,packsPerPoly,k: %d %d %d => [%d][%d]", BS_pclipping_info.subject_cnt, BS_pclipping_info.subject_packs, k, pIdx, vIdx);
  #endif
        if (pIdx < MAX_RESULT_POLYGON_NO && vIdx < MAX_RESULT_VERTEX_NO) {
            BS_pclipping_info.subject_vertex[pIdx][vIdx].vert.x = rese->ve[k].vert.x;
            BS_pclipping_info.subject_vertex[pIdx][vIdx].vert.y = rese->ve[k].vert.y;
            BS_pclipping_info.subject_vertex[pIdx][vIdx].edgeId = rese->ve[k].edgeId;
        }
        else {
            TRACE_ERROR("ERROR! 2 invalid pIdx or vIdx: %d %d [%d]", pIdx, vIdx, BG_frame_no);
        }
    }
    BS_pclipping_info.subject_cnt++;
}
#endif

#ifdef DRAW_POLYGON_TEST  //nsmoon@170202
ATTR_BACKEND_RAMFUNC
static int DEBUG_clipping_var_save_to_touch(int touchCntLocal, poly_e_t *rese)
{
    int k, pIdx, vIdx;
    DEBUG_touch_polygon_len[touchCntLocal] = rese->len;
    for (k = 0; k < rese->len; k++) {
		if (k >= (MAX_SAVE_POLY_PACK_NO * MAX_RESULT_VERTEX_NO)) {
			TRACE_ERROR("ERROR! invalid rese->len-1- %d", rese->len);
			break;
		}
#if 0 //for test, nsmoon@190322, FIXME
        pIdx = touchCntLocal * BS_pclipping_info.polygon_packs + k / MAX_RESULT_VERTEX_NO;
        vIdx = k % MAX_RESULT_VERTEX_NO;
#else
        pIdx = (touchCntLocal * MAX_SAVE_POLY_PACK_NO) + (k / MAX_RESULT_VERTEX_NO);
        vIdx = (k % MAX_RESULT_VERTEX_NO);
#endif
#if 0 //(DEBUG_clipping_var_save_result == 1)
        TRACE("..subject_cnt,packsPerPoly,k: %d %d %d => [%d][%d]", touchCntLocal, MAX_SAVE_POLY_PACK_NO, k, pIdx, vIdx);
        //TRACE("..subject_cnt,packsPerPoly,k: %d %d %d => [%d][%d]", touchCntLocal, BS_pclipping_info.polygon_packs, k, pIdx, vIdx);
  #endif
        //if (pIdx < MAX_RESULT_POLYGON_NO && vIdx < MAX_RESULT_VERTEX_NO) 
        if (pIdx < MAX_DEBUG_TOUCH_POLYGON)
		{
            DEBUG_touch_polygon[pIdx][vIdx].vert.x = rese->ve[k].vert.x;
            DEBUG_touch_polygon[pIdx][vIdx].vert.y = rese->ve[k].vert.y;
            DEBUG_touch_polygon[pIdx][vIdx].edgeId = rese->ve[k].edgeId;
        }
        else {
            TRACE_ERROR_MEM("ERROR_MEM! 3 invalid pIdx or vIdx: %d %d (%d) [%d]", pIdx, vIdx, BS_pclipping_info.totalProcessedSlopeCnt, BG_frame_no);
            return 1; //error
        }
    }
    return 0; //no-error
}
#endif

#if 0 //for test
ATTR_BACKEND_RAMFUNC
void BG_init_clipping_var(void)
{
    clipping_var_init();
}

ATTR_BACKEND_RAMFUNC
int BG_add_clipping_result(poly_e_t *rese, poly_e_t *subj, int skip)
{
    int ret = clipping_var_save_result(rese, subj, skip);
    return ret;
}
#endif

#endif //clipping_var
////////////////////////////////////////////

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_BG_clipping_multi_initial		1 //0:disable, 1:initial-box
#endif
#ifdef MULTI_PACK_EP3_ENABLE
ATTR_BACKEND_RAMFUNC
int BG_clipping_multi_initial(void)
{
    int i, j, k;
    //int slopeLedIdx, polyIdx;
    float touchAreaTmp, minX, maxX, minY, maxY;
    poly_e_t subjecte, clippere;
    //uint8_t subjAdc, clipAdc;
    //poly_e_t debuge;
    poly_e   rese;
    ep_buf_t *epaX0 = &BS_edge_pattern_x[EPA_IDX_0];
    ep_buf_t *epaY0 = &BS_edge_pattern_y[EPA_IDX_0];
    vec_t vecTmp;
    initial_polygon_t *initial_polygon = &BS_initial_polygon[0];
    int initialTouchCnt;

	BS_initial_polygon_cnt = 0; //init

#if 1 //defined(DRAW_POLYGON_TEST)
    IS_DEBUG_FLAG{
        //TRACE("BG_clipping_multi_initial...");
        //SEND_POLY(0, 0x21, 0);
        TRACE_NOP;
    }
#endif

    initialTouchCnt = 0;
    for (i = 0; i < epaX0->len; i++)
    {
        subjecte.len = INITIAL_POLYGON_VERTEX_NO;
        subjecte.ve = (vec_e_t *)&(epaX0->poly[i]);

        for (k = 0; k < epaY0->len; k++)
        {
            clippere.len = INITIAL_POLYGON_VERTEX_NO;
            clippere.ve = (vec_e_t *)&(epaY0->poly[k]);
#if (DEBUG_BG_clipping_multi_initial > 1)
            SEND_POLY(0, 0x21, 0);
            IS_DEBUG_FLAG{ 
				//if (BG_show_ep) 
				{
					TRACE_POLYGON_MULTI(&subjecte, 0x11, 0);
					TRACE_POLYGON_MULTI(&clippere, 0x11, 1);
				} 
			};
#endif
            rese = BS_poly_e_clip(&subjecte, &clippere);
#if (DEBUG_BG_clipping_multi_initial > 1)
            IS_DEBUG_FLAG{
                TRACE_POLYGON_MULTI(rese, 0x11, MY_COLOR);
                TRACE_NOP;
            }
#endif
                if (rese->len > 2) {
                    //touchAreaTmp = findCentroid_e(rese, &centerPoint);
                    touchAreaTmp = BS_getTouchArea(rese);
                    if (touchAreaTmp > MULTI_MIN_TOUCH_AREA) { //remove small area, FIXME min size tunning!!!! nsmoon@170518a
                        //if ((initialTouchCnt + 1) < MAX_RESULT_POLYGON_NO)
						if (initialTouchCnt < MAX_RESULT_POLYGON_NO)
						{
                                                               //save area
                        //initial_polygon[initialTouchCnt].area = touchAreaTmp;
                        //get min/max ep
#ifdef DEBUG_INITIAL_SIZESQ
                        BG_debug_sizeSq_initial = touchAreaTmp; //for test
                        BG_debug_sizeSq_initial_max = GET_MAX(BG_debug_sizeSq_initial_max, touchAreaTmp);
#endif
                        initial_polygon[initialTouchCnt].ep.minX = GET_MIN(epaX0->ep[i].s1, epaX0->ep[i].e1);
                        initial_polygon[initialTouchCnt].ep.maxX = GET_MAX(epaX0->ep[i].s2, epaX0->ep[i].e2);
                        initial_polygon[initialTouchCnt].ep.minY = GET_MIN(epaY0->ep[k].s1, epaY0->ep[k].e1);
                        initial_polygon[initialTouchCnt].ep.maxY = GET_MAX(epaY0->ep[k].s2, epaY0->ep[k].e2);
#if 1 //nsmoon@190715
						if (i < (int)(epaX0->len / 2)) {
                            initial_polygon[initialTouchCnt].firstX = 1;
						}
						else {
                            initial_polygon[initialTouchCnt].firstX = 0;
						}
						if (k < (int)(epaY0->len / 2)) {
                            initial_polygon[initialTouchCnt].firstY = 1;
						}
						else {
                            initial_polygon[initialTouchCnt].firstY = 0;
						}
#endif

                        if (rese->len != INITIAL_POLYGON_VERTEX_NO) {
                                TRACE_ERROR("ERROR! invalid rese->len %d [%d]", rese->len, BG_frame_no);
                            continue;
                        }
                        //get min/max pos
                        j = 0;
                        minX = rese->ve[j].vert.x;
                        maxX = rese->ve[j].vert.x;
                        minY = rese->ve[j].vert.y;
                        maxY = rese->ve[j].vert.y;
                        for (j = 1; j < INITIAL_POLYGON_VERTEX_NO; j++) {
                            minX = GET_MIN(minX, rese->ve[j].vert.x);
                            maxX = GET_MAX(maxX, rese->ve[j].vert.x);
                            minY = GET_MIN(minY, rese->ve[j].vert.y);
                            maxY = GET_MAX(maxY, rese->ve[j].vert.y);
                        }
                        initial_polygon[initialTouchCnt].pos.minX = minX;
                        initial_polygon[initialTouchCnt].pos.maxX = maxX;
                        initial_polygon[initialTouchCnt].pos.minY = minY;
                        initial_polygon[initialTouchCnt].pos.maxY = maxY;

                            //vecTmp.x = initial_polygon[initialTouchCnt].pos.minX;
                            //vecTmp.y = initial_polygon[initialTouchCnt].pos.minY;
						    //nsmoon@190703
                            vecTmp.x = (initial_polygon[initialTouchCnt].pos.minX + initial_polygon[initialTouchCnt].pos.maxX) * 0.5f;
                            vecTmp.y = (initial_polygon[initialTouchCnt].pos.minY + initial_polygon[initialTouchCnt].pos.maxY) * 0.5f;
                            initial_polygon[initialTouchCnt].side = BS_getSideFromPos(&vecTmp);

                            initialTouchCnt++;
                        }
                        else {
                            TRACE_ERROR("ERROR! invalid initialTouchCnt %d [%d]", initialTouchCnt, BG_frame_no);
							return -1; //mem-error
                        }
                    }
                    else {
                    	//for debugging
                    	//TRACE_ERROR("BG_clipping_multi_initial...touchAreaTmp < minTouchArea:(%0.1f %0.1f)", touchAreaTmp, MULTI_MIN_TOUCH_AREA);
                    }
                } //if (rese->len > 2)
                else {
                    //TRACE("BG_clipping_multi_initial...rese->len %d", rese->len);
                }
        } //for (k = 0; k < epaY0->len; k++)
    } //for (i = 0; i < epaX0->len; i++)
#if defined(DEBUG_FUNCTION_ENABLE_ALL)
    IS_DEBUG_FLAG
    {
        //SEND_POLY(0, 0x21, MY_COLOR);
        for (i = 0; i < initialTouchCnt; i++) {
        #if 0 //for test
            float xWidth = GET_ABS(initial_polygon[i].pos.minX - initial_polygon[i].pos.maxX);
            float yWidth = GET_ABS(initial_polygon[i].pos.minY - initial_polygon[i].pos.maxY);
            TRACE("xWidth,yWidth=(%d) %0.1f %0.1f", i, xWidth, yWidth);
        #endif
            DEBUG_SHOW_MIN_MAX(&initial_polygon[i].pos, MY_COLOR-1, 1);
			TRACE_NOP;
        }
    }
#endif
#if defined(DEBUG_FUNCTION_ENABLE_RELEASE)
    for (i = 0; i < initialTouchCnt; i++) {
#if defined(INITIAL_LINE_TEST_MULTI)
        DEBUG_SHOW_MIN_MAX(&initial_polygon[i].pos, MY_COLOR-1, 1);
#else
        DEBUG_SHOW_MIN_MAX(&initial_polygon[i].pos, MY_COLOR-1, 1);
#endif
        TRACE_NOP;
    }
#endif
    IS_DEBUG_FLAG
    {
        if (initialTouchCnt > 0) {
            TRACE_NOP;
        }
    //SEND_POLY(0, 0x21, MY_COLOR);
    }
    BS_initial_polygon_cnt = initialTouchCnt; //save
    return initialTouchCnt;
}
#endif

#ifdef USE_CUST_MALLOC //nsmoon@201012
#define DEBUG_multi_get_poly    0
static int multi_get_initial_poly(axis_t axis, ep_no_t *ep, vec_e_t *poly, int initial)
{
    float *ledPos, *pdPos;
    float posBotRht, posBotLft, posTopRht, posTopLft;
    int maxPdNum;

    if (axis == ENUM_HOR_X) {
        maxPdNum = BS_maxHorPdNum - 1;
        ledPos = &BS_led_pos_x[0];
        pdPos = &BS_pd_pos_x[0];
    }
    else {
        maxPdNum = BS_maxVerPdNum - 1;
        ledPos = &BS_led_pos_y[0];
        pdPos = &BS_pd_pos_y[0];
    }

    int s1 = ep->s1;
    int s2 = ep->s2;
    int e1 = ep->e1;
    int e2 = ep->e2;
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };

    if (initial) {
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
    }
    else {
        posBotRht = ledPos[s1];
        posTopRht = pdPos[e1];
        posTopLft = pdPos[e2];
        posBotLft = ledPos[s2];
    }

    if (axis == ENUM_HOR_X) {
        poly[0].vert.x = posBotRht; //bot-rht
        poly[0].vert.y = BS_sensor_zero_y;
        poly[0].edgeId = 0; //epa idx, FIXME
        poly[1].vert.x = posTopRht;  //top-rht
        poly[1].vert.y = BS_sensor_end_y;
        poly[1].edgeId = NO_CLIPPING_EDGEID;
        poly[2].vert.x = posTopLft;  //top-lft
        poly[2].vert.y = BS_sensor_end_y;
        poly[2].edgeId = 0; //epa idx, FIXME
        poly[3].vert.x = posBotLft; //bot-lft
        poly[3].vert.y = BS_sensor_zero_y;
        poly[3].edgeId = NO_CLIPPING_EDGEID;
    }
    else {
        poly[0].vert.y = posBotRht; //bot-rht
        poly[0].vert.x = BS_sensor_end_x;
        poly[0].edgeId = 0; //epa idx, FIXME
        poly[1].vert.y = posTopRht;  //top-rht
        poly[1].vert.x = BS_sensor_zero_x;
        poly[1].edgeId = NO_CLIPPING_EDGEID;
        poly[2].vert.y = posTopLft;  //top-lft
        poly[2].vert.x = BS_sensor_zero_x;
        poly[2].edgeId = 0; //epa idx, FIXME
        poly[3].vert.y = posBotLft; //bot-lft
        poly[3].vert.x = BS_sensor_end_x;
        poly[3].edgeId = NO_CLIPPING_EDGEID;
    }
#if (DEBUG_multi_get_initial_poly > 0)
    IS_DEBUG_FLAG{
        //if (BG_show_line_init)
        {
            DEBUG_SHOW_POLY(&poly[0], 4, MY_COLOR-4);
            TRACE_NOP;
        }
    };
#endif
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };
    return 0; //no-error
}
#endif

#ifdef MULTI_PACK_EP4_ENABLE
int BG_clipping_multi_initial4(void)
{
    int i, j, k;
    //int slopeLedIdx, polyIdx;
    float touchAreaTmp, minX, maxX, minY, maxY;
    poly_e_t subjecte, clippere;
    //uint8_t subjAdc, clipAdc;
    //poly_e_t debuge;
    poly_e   rese;
    ep_buf_t *epaX0 = &BS_edge_pattern_x[EPA_IDX_0];
    ep_buf_t *epaY0 = &BS_edge_pattern_y[EPA_IDX_0];
    vec_t vecTmp;
    initial_polygon_t *initial_polygon = &BS_initial_polygon[0];
    int initialTouchCnt;
#ifdef USE_CUST_MALLOC //nsmoon@201012
    vec_e_t polySubj[INITIAL_POLYGON_VERTEX_NO]; //nsmoon@201008
    vec_e_t polyClip[INITIAL_POLYGON_VERTEX_NO]; //nsmoon@201008
#endif

    BS_initial_polygon_cnt = 0; //init

#if 1 //defined(DRAW_POLYGON_TEST)
    IS_DEBUG_FLAG{
        //TRACE("BG_clipping_multi_initial...");
        //SEND_POLY(0, 0x21, 0);
        TRACE_NOP;
    }
#endif

    initialTouchCnt = 0;
    for (i = 0; i < epaX0->len; i++)
    {
        //if (epaX0->ep[i].s1 == EPA0_INITIAL_INVALID) continue;
        subjecte.len = INITIAL_POLYGON_VERTEX_NO;
#ifdef USE_CUST_MALLOC //nsmoon@201012
        multi_get_initial_poly(ENUM_HOR_X, &epaX0->ep[i], &polySubj[0], 1); //1:initial
        subjecte.ve = &polySubj[0];
#else
        subjecte.ve = (vec_e_t *)&(epaX0->poly[i]);
#endif

        for (k = 0; k < epaY0->len; k++)
        {
            //if (epaY0->ep[i].s1 == EPA0_INITIAL_INVALID) continue;
            clippere.len = INITIAL_POLYGON_VERTEX_NO;
#ifdef USE_CUST_MALLOC //nsmoon@201012
            multi_get_initial_poly(ENUM_VER_Y, &epaY0->ep[k], &polyClip[0], 1); //initial
            clippere.ve = &polyClip[0];
#else
            clippere.ve = (vec_e_t *)&(epaY0->poly[k]);
#endif
#if (DEBUG_BG_clipping_multi_initial > 1)
            //SEND_POLY(0, 0x21, 0);
            IS_DEBUG_FLAG{
                //if (BG_show_ep)
                {
                    TRACE_POLYGON_MULTI(&subjecte, 0x11, MY_COLOR - 2);
                    TRACE_POLYGON_MULTI(&clippere, 0x11, MY_COLOR - 2);
                }
            };
#endif
#if 1 //(DEBUG_clipping_multi_initial4 > 0)
            IS_DEBUG_FLAG{
                if (BG_show_line_init == 2)
                {
                    TRACE_POLYGON_MULTI(&subjecte, 0x11, MY_COLOR - 2);
                    TRACE_POLYGON_MULTI(&clippere, 0x11, MY_COLOR - 2);
                }
            };
#endif
            rese = BS_poly_e_clip(&subjecte, &clippere);
#if (DEBUG_BG_clipping_multi_initial > 1)
            IS_DEBUG_FLAG{
                TRACE_POLYGON_MULTI(rese, 0x11, MY_COLOR);
                TRACE_NOP;
            }
#endif
                if (rese->len > 2) {
                    //touchAreaTmp = findCentroid_e(rese, &centerPoint);
                    touchAreaTmp = BS_getTouchArea(rese);
                    if (touchAreaTmp > MULTI_MIN_TOUCH_AREA) { //remove small area, FIXME min size tunning!!!! nsmoon@170518a
                        //if ((initialTouchCnt + 1) < MAX_RESULT_POLYGON_NO)
						if (initialTouchCnt < MAX_RESULT_POLYGON_NO)
						{
                                                               //save area
                        //initial_polygon[initialTouchCnt].area = touchAreaTmp;
                        //get min/max ep
#ifdef DEBUG_INITIAL_SIZESQ
                        BG_debug_sizeSq_initial = touchAreaTmp; //for test
                        BG_debug_sizeSq_initial_max = GET_MAX(BG_debug_sizeSq_initial_max, touchAreaTmp);
#endif
                        initial_polygon[initialTouchCnt].ep.minX = GET_MIN(epaX0->ep[i].s1, epaX0->ep[i].e1);
                        initial_polygon[initialTouchCnt].ep.maxX = GET_MAX(epaX0->ep[i].s2, epaX0->ep[i].e2);
                        initial_polygon[initialTouchCnt].ep.minY = GET_MIN(epaY0->ep[k].s1, epaY0->ep[k].e1);
                        initial_polygon[initialTouchCnt].ep.maxY = GET_MAX(epaY0->ep[k].s2, epaY0->ep[k].e2);
#if 1 //nsmoon@190715
						if (i < (int)(epaX0->len / 2)) {
                            initial_polygon[initialTouchCnt].firstX = 1;
						}
						else {
                            initial_polygon[initialTouchCnt].firstX = 0;
						}
						if (k < (int)(epaY0->len / 2)) {
                            initial_polygon[initialTouchCnt].firstY = 1;
						}
						else {
                            initial_polygon[initialTouchCnt].firstY = 0;
						}
#endif

                        if (rese->len != INITIAL_POLYGON_VERTEX_NO) {
                                TRACE_ERROR("ERROR! invalid rese->len-2- %d [%d]", rese->len, BG_frame_no);
                            continue;
                        }
                        //get min/max pos
                        j = 0;
                        minX = rese->ve[j].vert.x;
                        maxX = rese->ve[j].vert.x;
                        minY = rese->ve[j].vert.y;
                        maxY = rese->ve[j].vert.y;
                        for (j = 1; j < INITIAL_POLYGON_VERTEX_NO; j++) {
                            minX = GET_MIN(minX, rese->ve[j].vert.x);
                            maxX = GET_MAX(maxX, rese->ve[j].vert.x);
                            minY = GET_MIN(minY, rese->ve[j].vert.y);
                            maxY = GET_MAX(maxY, rese->ve[j].vert.y);
                        }
                        initial_polygon[initialTouchCnt].pos.minX = minX;
                        initial_polygon[initialTouchCnt].pos.maxX = maxX;
                        initial_polygon[initialTouchCnt].pos.minY = minY;
                        initial_polygon[initialTouchCnt].pos.maxY = maxY;
                        //initial_polygon[initialTouchCnt].area = GET_ABS(maxX - minX) * GET_ABS(maxY - minY); //nsmoon@210308

                            //vecTmp.x = initial_polygon[initialTouchCnt].pos.minX;
                            //vecTmp.y = initial_polygon[initialTouchCnt].pos.minY;
						    //nsmoon@190703
                            vecTmp.x = (initial_polygon[initialTouchCnt].pos.minX + initial_polygon[initialTouchCnt].pos.maxX) * 0.5f;
                            vecTmp.y = (initial_polygon[initialTouchCnt].pos.minY + initial_polygon[initialTouchCnt].pos.maxY) * 0.5f;
                            initial_polygon[initialTouchCnt].side = BS_getSideFromPos(&vecTmp);

                            initialTouchCnt++;
                        }
                        else {
                            TRACE_ERROR("ERROR! invalid initialTouchCnt %d [%d]", initialTouchCnt, BG_frame_no);
							return -1; //mem-error
                        }
                    }
                    else {
                    	//for debugging
                    	//TRACE_ERROR("BG_clipping_multi_initial...touchAreaTmp < minTouchArea:(%0.1f %0.1f)", touchAreaTmp, MULTI_MIN_TOUCH_AREA);
                    }
                } //if (rese->len > 2)
                else {
                    //TRACE("BG_clipping_multi_initial...rese->len %d", rese->len);
                }
        } //for (k = 0; k < epaY0->len; k++)
    } //for (i = 0; i < epaX0->len; i++)
#if defined(DEBUG_FUNCTION_ENABLE_ALL)
    IS_DEBUG_FLAG
    {
        //SEND_POLY(0, 0x21, MY_COLOR);
        for (i = 0; i < initialTouchCnt; i++) {
        #if 0 //for test
            float xWidth = GET_ABS(initial_polygon[i].pos.minX - initial_polygon[i].pos.maxX);
            float yWidth = GET_ABS(initial_polygon[i].pos.minY - initial_polygon[i].pos.maxY);
            TRACE("xWidth,yWidth=(%d) %0.1f %0.1f", i, xWidth, yWidth);
        #endif
            //TRACE("initial_polygon[].pos=(%d) %0.1f %0.1f %0.1f %0.1f", i, initial_polygon[i].pos.maxX, initial_polygon[i].pos.maxY, initial_polygon[i].pos.minX, initial_polygon[i].pos.minY);
            DEBUG_SHOW_MIN_MAX(&initial_polygon[i].pos, MY_COLOR-1, 1);
			TRACE_NOP;
        }
    }
#endif
#if defined(DEBUG_FUNCTION_ENABLE_RELEASE)
    for (i = 0; i < initialTouchCnt; i++) {
#if defined(INITIAL_LINE_TEST_MULTI)
        DEBUG_SHOW_MIN_MAX(&initial_polygon[i].pos, MY_COLOR-1, 1);
#else
        DEBUG_SHOW_MIN_MAX(&initial_polygon[i].pos, MY_COLOR-1, 1);
#endif
        TRACE_NOP;
    }
#endif
    IS_DEBUG_FLAG
    {
        if (initialTouchCnt > 0) {
            TRACE_NOP;
        }
    //SEND_POLY(0, 0x21, MY_COLOR);
    }
    BS_initial_polygon_cnt = initialTouchCnt; //save
    return initialTouchCnt;
}
#endif

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_get_slope_order	0 //1:enable
#endif
ATTR_BACKEND_RAMFUNC
//static int get_slope_order(axis_t axis, sen_no_t *sen, int idx)
static int get_slope_order(axis_t axis, int center, int idx)
{
    int maxPdNum, ofst, ret; //center, 
    if (axis == ENUM_HOR_X) {
        //center = sen->x;
        maxPdNum = BS_maxHorPdNum - 1;
    }
    else {
        //center = sen->y;
        maxPdNum = BS_maxVerPdNum - 1;
    }
    if (idx < 0 || idx > maxPdNum) {
        TRACE_ERROR("ERROR! get_slope_order..invalid idx %d [%d]", idx, BG_frame_no);
        return -1; //error
    }

    ofst = (idx + 1) / 2;

    if (center > (maxPdNum / 2)) {
        if (ofst > (maxPdNum - center)) {
            ret = maxPdNum - idx;
        }
        else {
            if ((idx % 2) == 1) {
                //odd, right first
                ret = center - ofst;
            }
            else {
                //even, left
                ret = center + ofst;
            }
        }
    }
    else {
        //(center <= (maxPdNum / 2))
        if (ofst > center) {
            ret = idx;
        }
        else {
            if ((idx % 2) == 1) {
                //odd, right first
                ret = center - ofst;
            }
            else {
                //even, left
                ret = center + ofst;
            }
        }
    }
#if (DEBUG_get_slope_order == 1)
    TRACE("center,ofst,ret-1: %d %d %d", center, ofst, ret);
#endif

    return ret;
}

#if 0
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_sort_slope_order	0 //1:enable
#endif
ATTR_BACKEND_RAMFUNC
static void sort_slope_order(axis_t axis, sen_no_t *sen)
{
    int i, n, k, cnt = 0;
    int center, maxPdNum;
    uint8_t *buf = &BS_slope_order[0];

    if (axis == ENUM_HOR_X) {
        center = sen->x;
        maxPdNum = BS_maxHorPdNum - 1;
    }
    else {
        center = sen->y;
        maxPdNum = BS_maxVerPdNum - 1;
    }

    // 0, slope first, and then +, - slope
    for (i = center, n = center; i <= maxPdNum || n >= 0; i++, n--) {
        // 0 slope first, +, -
        if (n == center) {
            buf[cnt++] = n;
        }
        else {
            if (n >= 0) buf[cnt++] = n;
            if (i <= maxPdNum) buf[cnt++] = i;
        }
    }
#if 0
    if (i < maxPdNum) {
        for (k = i; k <= maxPdNum; k++) {
            buf[cnt++] = k;
        }
    }
    if (n > -1) {
        for (k = n; k >= 0; k--) {
            buf[cnt++] = k;
        }
    }
#endif
    if (cnt != maxPdNum + 1) {
        TRACE_ERROR("ERROR! sort_slope_order...invalid size %d (%d)", cnt, maxPdNum);
        for (i = 0; i <= maxPdNum; i++) {
            buf[i] = i;
        }
    }
    BS_slope_order_cnt = cnt;
#if (DEBUG_sort_slope_order == 1)
    for (i = 0; i < BS_slope_order_cnt; i++) {
        TRACENR("%d ", buf[i]);
    }
    TRACE(".");
#endif
}
#endif
#if 0 //not used
ATTR_BACKEND_RAMFUNC
static void sort_slope_order(start_end_t *stEd)
{
    int i, n, cnt = 0;
    int center = (stEd->start + stEd->end) / 2;
    uint8_t *buf = &BS_slope_order[0];

    // 0, slope first, and then +, - slope
    for (i = center, n = center; n >= stEd->start; i++, n--) {
        // 0 slope first, +, -
        if (n == center) {
            buf[cnt++] = n;
        }
        else {
            buf[cnt++] = n;
            if (i < stEd->end) {
                buf[cnt++] = i;
            }
        }
    }
    if (i < stEd->end - 1) {
        for (n = i; n < stEd->end; n++) {
            buf[cnt++] = n;
        }
    }

    if (cnt != (stEd->end - stEd->start)) {
        TRACE_ERROR("ERROR! sort_slope_order...invalid size %d (%d %d)", cnt, stEd->start, stEd->end);
        for (i = stEd->start; i <= stEd->end; i++) {
            if ((i - stEd->start) < MAX_NUM_PD) {
                buf[i - stEd->start] = i;
            }
            else {
                TRACE_ERROR("ERROR! invalid start or end: %d %d", stEd->start, stEd->end);
            }
        }
    }
    BS_slope_order_cnt = cnt;
#if (DEBUG_sort_slope_order == 1)
    for (i = 0; i < BS_slope_order_cnt; i++) {
        TRACENR("%d ", buf[i]);
    }
    TRACE(".");
#endif
}

ATTR_BACKEND_RAMFUNC
static int sort_slope_reorder(bd_type_t bd, int mode, int num)
{
    uint8_t *buf = &BS_slope_order[0];
    int end1 = buf[BS_slope_order_cnt - 1];
    int end2 = buf[BS_slope_order_cnt - 2];
    int d1 = GET_ABS(num - end1);
    int d2 = GET_ABS(num - end2);
    int start;
    int i, j, ret, inc, cnt;
    int maxPdNum;

    if (d1 == 0 || d2 == 0) {
        TRACE_ERROR("ERROR! invalid end %d (%d %d)", num, end1, end2);
        return 0; //error
    }
    if (bd == ENUM_BOT_BD || bd == ENUM_TOP_BD) {
        maxPdNum = BS_maxHorPdNum - 1;
    }
    else {
        maxPdNum = BS_maxVerPdNum - 1;
    }

    inc = (mode == 0) ? -1 : 1; //mode: (0)start, (1)end
    if (d1 > d2) {
        //d2(-2) is end side
        start = end2 + inc;
    }
    else {
        //d1(-1) is end side
        start = end1 + inc;
    }

    cnt = BS_slope_order_cnt;
    for (i = start; (i * inc) < (num * inc); i += inc) {
        buf[cnt++] = i;
    }
    BS_slope_order_cnt = cnt;

#if (DEBUG_sort_slope_order == 1)
    for (i = 0; i < BS_slope_order_cnt; i++) {
        TRACENR("%d ", buf[i]);
    }
    TRACE(".");
#endif
    return 1; //no error
}
#endif
#if 0 //for testing
ATTR_BACKEND_RAMFUNC
static int sort_slope_reorder_test(bd_type_t bd, int mode, int num)
{
    uint8_t *buf = &BS_slope_order[0];
    int end1 = buf[BS_slope_order_cnt - 1];
    int end2 = buf[BS_slope_order_cnt - 2];
    int d1 = GET_ABS(num - end1);
    int d2 = GET_ABS(num - end2);
    int start1, start2;
    int i, j, ret, inc, cnt = BS_slope_order_cnt;
    int maxPdNum;

    if (d1 == 0 || d2 == 0) {
        TRACE_ERROR("ERROR! invalid end %d (%d %d)", num, end1, end2);
        return -1; //error
    }
    if (bd == ENUM_BOT_BD || bd == ENUM_TOP_BD) {
        maxPdNum = BS_maxHorPdNum - 1;
    }
    else {
        maxPdNum = BS_maxVerPdNum - 1;
    }

    inc = (mode == 0) ? -1 : 1; //mode: (0)start, (1)end

    if (d1 > d2) {
        //d2(-2) is end side
        start1 = end2 + inc;
        start2 = end1 + inc;
    }
    else {
        //d1(-1) is end side
        start1 = end1 + inc;
        start2 = end2 + inc;
    }

    for (i = start1, j = start2; (i * inc) < (num * inc); i += inc, j -= inc) {
        if (d1 > d2) {
            buf[cnt++] = i; //start1 first
            if (j >= 0 || j <= maxPdNum) {
                buf[cnt++] = j;
                ret = j;
            }
        }
        else {
            if (j >= 0 || j <= maxPdNum) {
                buf[cnt++] = j; //start2 first
                ret = j;
            }
            buf[cnt++] = i;
        }
    }
    BS_slope_order_cnt = cnt;

#if (DEBUG_sort_slope_order == 1)
    for (i = 0; i < BS_slope_order_cnt; i++) {
        TRACENR("%d ", buf[i]);
    }
    TRACE(".");
#endif
    return ret;
}
#endif //0

#if 0
ATTR_BACKEND_RAMFUNC
static int isPolyIdxSaved(int subjIdx)
{
    int i;
    for (i = 0; i < BS_pclipping_info.polygon_cnt; i++) {
        if (BS_pclipping_info.poly_idx_saved[i] == subjIdx) {
            return i; //found
        }
    }
    return -1; //not found
}
#endif

#define DEAD_ZONE_RIGHT		0
#define DEAD_ZONE_LEFT		1
#define DEAD_ZONE_ADJ_PAD	(10.0f) //(5.0f) //nsmoon@190315, m5p0315a-174, FIXME
ATTR_BACKEND_RAMFUNC
static int saveEdgePatternDeadzone(bd_type_t bd, int side, int s1, int s2, int e1, int e2)
{
#ifndef USE_CUST_MALLOC //nsmoon@201012
    float *bdTop, *bdBot;
    axis_t axis;
    float s1Pad, s2Pad, e1Pad, e2Pad;

    if (bd == ENUM_BOT_BD) {
        axis = ENUM_HOR_X; //ENUM_LED
        bdBot = &BS_led_pos_x[0];
        bdTop = &BS_pd_pos_x[0];
    }
    else if (bd == ENUM_TOP_BD) {
        axis = ENUM_HOR_X; //ENUM_PD
        bdBot = &BS_led_pos_x[0];
        bdTop = &BS_pd_pos_x[0];
    }
    else if (bd == ENUM_LFT_BD) {
        axis = ENUM_VER_Y; //ENUM_LED
        bdBot = &BS_led_pos_y[0];
        bdTop = &BS_pd_pos_y[0];
    }
    else {
        //(bd == ENUM_RHT_BD)
        axis = ENUM_VER_Y; //ENUM_PD
        bdBot = &BS_led_pos_y[0];
        bdTop = &BS_pd_pos_y[0];
    }
#endif

    if (side == DEAD_ZONE_RIGHT) {
#ifdef USE_CUST_MALLOC //nsmoon@201012
        if (BS_edge_pattern_ep_cnt < BS_max_ep_count) { //nsmoon@201014
#else
        if (BS_edge_pattern_ep_cnt < MAX_EP_COUNT) {
#endif
#if 1 //nsmoon@190326
            BG_edge_pattern_dz.ep = &BS_edge_pattern_dz_ep[DEAD_ZONE_RIGHT];
#ifndef USE_CUST_MALLOC //nsmoon@201012
            BG_edge_pattern_dz.poly = &BS_edge_pattern_dz_poly[DEAD_ZONE_RIGHT]; //nsmoon@201008
#endif
#else
            BG_edge_pattern_dz.ep = &BS_edge_pattern_ep[BS_edge_pattern_ep_cnt];
            BG_edge_pattern_dz.poly = &BS_edge_pattern_poly[BS_edge_pattern_ep_cnt];
            BS_edge_pattern_ep_cnt++;
#endif
        }
        else {
            TRACE_ERROR("ERROR!3 invalid BS_edge_pattern_ep_cnt %d [%d]", BS_edge_pattern_ep_cnt, BG_frame_no);
            return 1; //error
        }
#ifndef USE_CUST_MALLOC //nsmoon@201012
		//RIGHT
		s1Pad = -DEAD_ZONE_ADJ_PAD;
		s2Pad = 0.0f;
		e1Pad = -DEAD_ZONE_ADJ_PAD;
		e2Pad = 0.0f;
#endif
#if 0 //nsmoon@190315, m5p0315a-xs174
        if (bd == ENUM_BOT_BD || bd == ENUM_LFT_BD) {
            //LED, RIGHT
            s1Pad = -DEAD_ZONE_ADJ_PAD;
            s2Pad = 0.0f;
            e1Pad = -DEAD_ZONE_ADJ_PAD;
            e2Pad = -DEAD_ZONE_ADJ_PAD;
        }
        else {
            //PD, RIGHT
            s1Pad = -DEAD_ZONE_ADJ_PAD;
            s2Pad = -DEAD_ZONE_ADJ_PAD;
            e1Pad = -DEAD_ZONE_ADJ_PAD;
            e2Pad = 0.0f;
        }
#endif
        //right side deadzone
        BG_edge_pattern_dz.ep[DEAD_ZONE_RIGHT].s1 = (uint8_t)s1;
        BG_edge_pattern_dz.ep[DEAD_ZONE_RIGHT].e1 = (uint8_t)e1;
        BG_edge_pattern_dz.ep[DEAD_ZONE_RIGHT].s2 = (uint8_t)s2;
        BG_edge_pattern_dz.ep[DEAD_ZONE_RIGHT].e2 = (uint8_t)e2;
#ifndef USE_CUST_MALLOC //nsmoon@201012
        if (axis == ENUM_HOR_X) {
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][0].vert.x = bdBot[s1] + s1Pad;
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][0].vert.y = BS_sensor_zero_y;
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][0].edgeId = (uint16_t)s1;
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][1].vert.x = bdTop[e1] + e1Pad;
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][1].vert.y = BS_sensor_end_y;
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][1].edgeId = NO_CLIPPING_EDGEID;
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][2].vert.x = bdTop[e2] + e2Pad;
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][2].vert.y = BS_sensor_end_y;
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][2].edgeId = (uint16_t)e2;
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][3].vert.x = bdBot[s2] + s2Pad;
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][3].vert.y = BS_sensor_zero_y;
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][3].edgeId = NO_CLIPPING_EDGEID;
        }
        else {
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][0].vert.y = bdBot[s1] + s1Pad;
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][0].vert.x = BS_sensor_end_x;
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][0].edgeId = (uint16_t)s1;
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][1].vert.y = bdTop[e1] + e1Pad;
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][1].vert.x = BS_sensor_zero_x;
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][1].edgeId = NO_CLIPPING_EDGEID;
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][2].vert.y = bdTop[e2] + e2Pad;
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][2].vert.x = BS_sensor_zero_x;
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][2].edgeId = (uint16_t)e2;
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][3].vert.y = bdBot[s2] + s2Pad;
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][3].vert.x = BS_sensor_end_x;
            BG_edge_pattern_dz.poly[DEAD_ZONE_RIGHT][3].edgeId = NO_CLIPPING_EDGEID;
        }
#endif
    }
    else if (side == DEAD_ZONE_LEFT) {
#ifdef USE_CUST_MALLOC //nsmoon@201012
        if (BS_edge_pattern_ep_cnt < BS_max_ep_count) { //nsmoon@201014
#else
        if (BS_edge_pattern_ep_cnt < MAX_EP_COUNT) {
#endif
#if 1 //nsmoon@190326
            BG_edge_pattern_dz.ep = &BS_edge_pattern_dz_ep[DEAD_ZONE_LEFT];
#ifndef USE_CUST_MALLOC //nsmoon@201012
            BG_edge_pattern_dz.poly = &BS_edge_pattern_dz_poly[DEAD_ZONE_LEFT]; //nsmoon@201008
#endif
#else
            BG_edge_pattern_dz.ep = &BS_edge_pattern_ep[BS_edge_pattern_ep_cnt];
            BG_edge_pattern_dz.poly = &BS_edge_pattern_poly[BS_edge_pattern_ep_cnt];
            BS_edge_pattern_ep_cnt++;
#endif
        }
        else {
            TRACE_ERROR("ERROR!4 invalid BS_edge_pattern_ep_cnt %d [%d]", BS_edge_pattern_ep_cnt, BG_frame_no);
            return 1; //error
        }
#ifndef USE_CUST_MALLOC //nsmoon@201012
		//LEFT
		s1Pad = 0.0f;
		s2Pad = DEAD_ZONE_ADJ_PAD;
		e1Pad = 0.0f;
		e2Pad = DEAD_ZONE_ADJ_PAD;
#endif
#if 0 //nsmoon@190315, m5p0315a-174
        if (bd == ENUM_BOT_BD || bd == ENUM_LFT_BD) {
            //LED, LEFT
            s1Pad = 0.0f;
            s2Pad = DEAD_ZONE_ADJ_PAD;
            e1Pad = DEAD_ZONE_ADJ_PAD;
            e2Pad = DEAD_ZONE_ADJ_PAD;
        }
        else {
            //PD, LEFT
            s1Pad = DEAD_ZONE_ADJ_PAD;
            s2Pad = DEAD_ZONE_ADJ_PAD;
            e1Pad = 0.0f;
            e2Pad = DEAD_ZONE_ADJ_PAD;
        }
#endif
        //left side deadzone
        BG_edge_pattern_dz.ep[DEAD_ZONE_LEFT].s1 = (uint8_t)s1;
        BG_edge_pattern_dz.ep[DEAD_ZONE_LEFT].e1 = (uint8_t)e1;
        BG_edge_pattern_dz.ep[DEAD_ZONE_LEFT].s2 = (uint8_t)s2;
        BG_edge_pattern_dz.ep[DEAD_ZONE_LEFT].e2 = (uint8_t)e2;
#ifndef USE_CUST_MALLOC //nsmoon@201012
        if (axis == ENUM_HOR_X) {
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][0].vert.x = bdBot[s1] + s1Pad;
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][0].vert.y = BS_sensor_zero_y;
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][0].edgeId = (uint16_t)s1;
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][1].vert.x = bdTop[e1] + e1Pad;
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][1].vert.y = BS_sensor_end_y;
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][1].edgeId = NO_CLIPPING_EDGEID;
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][2].vert.x = bdTop[e2] + e2Pad;
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][2].vert.y = BS_sensor_end_y;
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][2].edgeId = (uint16_t)e2;
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][3].vert.x = bdBot[s2] + s2Pad;
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][3].vert.y = BS_sensor_zero_y;
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][3].edgeId = NO_CLIPPING_EDGEID;
        }
        else {
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][0].vert.y = bdBot[s1] + s1Pad;
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][0].vert.x = BS_sensor_end_x;
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][0].edgeId = (uint16_t)s1;
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][1].vert.y = bdTop[e1] + e1Pad;
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][1].vert.x = BS_sensor_zero_x;
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][1].edgeId = NO_CLIPPING_EDGEID;
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][2].vert.y = bdTop[e2] + e2Pad;
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][2].vert.x = BS_sensor_zero_x;
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][2].edgeId = (uint16_t)e2;
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][3].vert.y = bdBot[s2] + s2Pad;
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][3].vert.x = BS_sensor_end_x;
            BG_edge_pattern_dz.poly[DEAD_ZONE_LEFT][3].edgeId = NO_CLIPPING_EDGEID;
        }
#endif
    }
    return 0; //no-error
}

#if (LARGE_PITCH_TBL == 1) //nsmoon@200108
static int is_sensor_adjacent_epa(sen_type_t sensor, ep_buf_t *epa_all, int senNo, int senNoOp)
{
	int i;
	ep_buf_t *epa = &epa_all[senNo];
	for (i = 0; i < epa->len; i++) {
		if (sensor == ENUM_PD) {
			if (senNo != epa->ep[i].e1 && senNo != epa->ep[i].e2) {
				TRACE_ERROR("ERROR! invalid senNo: %d %d %d", senNo, epa->ep[i].e1, epa->ep[i].e2);
				return 0;
			}
			if (senNoOp >= (epa->ep[i].s1 - 1) && senNoOp <= (epa->ep[i].s2)) {
				return 1; //found
			}
		}
		else {
			if (senNo != epa->ep[i].s1 && senNo != epa->ep[i].s2) {
				TRACE_ERROR("ERROR! invalid senNo: %d %d %d", senNo, epa->ep[i].s1, epa->ep[i].s2);
				return 0;
			}
			if (senNoOp >= (epa->ep[i].e1 - 1) && senNoOp <= (epa->ep[i].e2)) {
				return 1; //found
			}
		}
	}
	return 0; //not found
}

#ifdef VIRTUAL_EP_MERGE_ENABLE //nsmoon@200409 for 550-kiosk
static int is_adjacent_virtual_epa(int cnt, int senNo)
{
	ep2_no_t *virtul_ep = &BS_edge_pattern_virtul_ep[0];

	if (cnt == 0) {
#ifdef USE_CUST_MALLOC //nsmoon@201012
        if (cnt < BS_max_virtual_ep_count) { //nsmoon@201014
#else
        if (cnt < MAX_VIRTUAL_EP_COUNT) {
#endif
			//alloc new
			virtul_ep[cnt].s1 = senNo;
			virtul_ep[cnt].s2 = senNo;
		}
	}
	else {
		if ((virtul_ep[cnt - 1].s2 + 1) == senNo) {
			virtul_ep[cnt - 1].s2 = senNo;
			return 1; //found
		}
		else {
#ifdef USE_CUST_MALLOC //nsmoon@201012
            if (cnt < BS_max_virtual_ep_count) { //nsmoon@201014
#else
            if (cnt < MAX_VIRTUAL_EP_COUNT) {
#endif
				//alloc new
				virtul_ep[cnt].s1 = senNo;
				virtul_ep[cnt].s2 = senNo;
			}
		}
	}
	return 0;
}
#endif

//#define MAKE_VIRTUAL_PAD	(pad = width * 0.5f)
#define MAKE_VIRTUAL_PAD	(pad = width - MULTI_LIM_EPA_PAD2)
//#define MAKE_VIRTUAL_PAD	(pad = width * MULTI_PAD_WIDTH_FACTOR)
ATTR_BACKEND_RAMFUNC
static int make_edge_pattern_virtual(axis_t axis, sen_type_t sensor, ep_buf_t *epa_all, int slopeIdx, int senStOp, int senEdOp, int multiLoopCnt)
{
	int ret, cnt;
	int senNo, senNoOp;
	//int senStOp, senEdOp;
#ifdef USE_CUST_MALLOC //nsmoon@201012
    vec_e_t **virtul_poly = BS_edge_pattern_virtul_poly; //nsmoon@201015
#else
    vec_e_t (*virtul_poly)[INITIAL_POLYGON_VERTEX_NO] = BS_edge_pattern_virtul_poly;
    //BS_edge_pattern_virtul_poly is not same &BS_edge_pattern_virtul_poly[0][0], FIXME!!!
#endif
	int maxSlopeVal, maxPdNum;
	sen_type_t sensorOp = (sensor == ENUM_PD) ? ENUM_LED : ENUM_PD;
	float width = 0, pad = 0;
	int senLimSt, senLimEd;
#ifdef VIRTUAL_EP_MERGE_ENABLE //nsmoon@200409 for 550-kiosk
	int prevCnt;
#endif

	if (axis == ENUM_HOR_X) {
		maxSlopeVal = BS_slopeValMaxX;
		maxPdNum = BS_maxHorPdNum - 1;
	}
	else {
		maxSlopeVal = BS_slopeValMaxY;
		maxPdNum = BS_maxVerPdNum - 1;
	}

	senNo = slopeIdx;
	senLimSt = senNo - maxSlopeVal;
	senLimSt = GET_MAX(senLimSt, 0);
	senLimEd = senNo + maxSlopeVal;
	senLimEd = GET_MIN(senLimEd, maxPdNum);
	if (senEdOp < senStOp) {
		TRACE_ERROR("ERROR! invalid senStOp,senEdOp: %d %d", senStOp, senEdOp);
		return 0;
	}
	if (senEdOp < senLimSt || senStOp > senLimEd) {
		return 0;
	}
	senStOp = GET_MAX(senStOp, senLimSt);
	senEdOp = GET_MIN(senEdOp, senLimEd);

	cnt = 0;
	for (senNoOp = senStOp; senNoOp < senEdOp; senNoOp++) {
		//not include senEdOp, FIXME
		ret = is_sensor_adjacent_epa(sensor, epa_all, senNo, senNoOp);
		if (ret) continue;
        if (multiLoopCnt != 3) {
            ret = BS_is_large_pitch(axis, sensorOp, senNoOp);
            if (ret == 0) continue;
        } //if multiLoopCnt==3, always virtual epa
#ifdef VIRTUAL_EP_MERGE_ENABLE //nsmoon@200409 for 550-kiosk
		prevCnt = cnt - 1;
#endif
			if (axis == ENUM_HOR_X) {
				if (sensor == ENUM_LED) {
                        width = (BS_pd_pos_x[senNoOp + 1] - BS_pd_pos_x[senNoOp]) * 0.5f;
					MAKE_VIRTUAL_PAD;
#ifdef VIRTUAL_EP_MERGE_ENABLE //nsmoon@200409 for 550-kiosk
				ret = is_adjacent_virtual_epa(cnt, senNoOp);
				if (ret) {
					virtul_poly[prevCnt][2].vert.x = BS_pd_pos_x[senNoOp] + width + pad;
					virtul_poly[prevCnt][2].vert.y = BS_sensor_end_y;
					virtul_poly[prevCnt][2].edgeId = senNoOp;
				}
                else 
#endif
                {
#ifdef USE_CUST_MALLOC //nsmoon@201012
                    if (cnt < BS_max_virtual_ep_count) { //nsmoon@201014
#else
                    if (cnt < MAX_VIRTUAL_EP_COUNT) {
#endif
                        virtul_poly[cnt][0].vert.x = BS_led_pos_x[senNo];
                        virtul_poly[cnt][0].vert.y = BS_sensor_zero_y;
                        virtul_poly[cnt][0].edgeId = (uint16_t)senNo;
                        virtul_poly[cnt][1].vert.x = BS_pd_pos_x[senNoOp] + width - pad;
                        virtul_poly[cnt][1].vert.y = BS_sensor_end_y;
                        virtul_poly[cnt][1].edgeId = NO_CLIPPING_EDGEID;
						virtul_poly[cnt][2].vert.x = BS_pd_pos_x[senNoOp] + width + pad;
                        virtul_poly[cnt][2].vert.y = BS_sensor_end_y;
                        virtul_poly[cnt][2].edgeId = (uint16_t)senNoOp;
                        virtul_poly[cnt][3].vert.x = BS_led_pos_x[senNo];
                        virtul_poly[cnt][3].vert.y = BS_sensor_zero_y;
                        virtul_poly[cnt][3].edgeId = NO_CLIPPING_EDGEID;
                        cnt++;
                    }
                    else {
#ifdef USE_CUST_MALLOC //nsmoon@201012
                        TRACE_ERROR_MEM("ERROR! invalid BSmax_virtual_ep_count %d", BS_max_virtual_ep_count);
#else
                        TRACE_ERROR_MEM("ERROR! invalid MAX_VIRTUAL_EP_COUNT");
#endif
                        break;
                    }
				}
			}
				else {
					//(sensor == ENUM_PD)
                        width = (BS_led_pos_x[senNoOp + 1] - BS_led_pos_x[senNoOp]) * 0.5f;
					MAKE_VIRTUAL_PAD;
#ifdef VIRTUAL_EP_MERGE_ENABLE //nsmoon@200409 for 550-kiosk
				ret = is_adjacent_virtual_epa(cnt, senNoOp);
				if (ret) {
					virtul_poly[prevCnt][3].vert.x = BS_led_pos_x[senNoOp] + width + pad;
					virtul_poly[prevCnt][3].vert.y = BS_sensor_zero_y;
					virtul_poly[prevCnt][3].edgeId = NO_CLIPPING_EDGEID;
				}
                else 
#endif
                {
#ifdef USE_CUST_MALLOC //nsmoon@201012
                    if (cnt < BS_max_virtual_ep_count) { //nsmoon@201014
#else
                    if (cnt < MAX_VIRTUAL_EP_COUNT) {
#endif
                        virtul_poly[cnt][0].vert.x = BS_led_pos_x[senNoOp] + width - pad;
                        virtul_poly[cnt][0].vert.y = BS_sensor_zero_y;
                        virtul_poly[cnt][0].edgeId = (uint16_t)senNoOp;
                        virtul_poly[cnt][1].vert.x = BS_pd_pos_x[senNo];
                        virtul_poly[cnt][1].vert.y = BS_sensor_end_y;
                        virtul_poly[cnt][1].edgeId = NO_CLIPPING_EDGEID;
                        virtul_poly[cnt][2].vert.x = BS_pd_pos_x[senNo];
                        virtul_poly[cnt][2].vert.y = BS_sensor_end_y;
                        virtul_poly[cnt][2].edgeId = (uint16_t)senNo;
						virtul_poly[cnt][3].vert.x = BS_led_pos_x[senNoOp] + width + pad;
                        virtul_poly[cnt][3].vert.y = BS_sensor_zero_y;
                        virtul_poly[cnt][3].edgeId = NO_CLIPPING_EDGEID;
                        cnt++;
                    }
                    else {
#ifdef USE_CUST_MALLOC //nsmoon@201012
                        TRACE_ERROR_MEM("ERROR! invalid BS_max_virtual_ep_count %d", BS_max_virtual_ep_count);
#else
                        TRACE_ERROR_MEM("ERROR! invalid MAX_VIRTUAL_EP_COUNT");
#endif
                        break;
                    }
				}
			}
		}
			else {
				//(axis == ENUM_VER_Y)
				if (sensor == ENUM_LED) {
                        width = (BS_pd_pos_y[senNoOp + 1] - BS_pd_pos_y[senNoOp]) * 0.5f;
					MAKE_VIRTUAL_PAD;
#ifdef VIRTUAL_EP_MERGE_ENABLE //nsmoon@200409 for 550-kiosk
				ret = is_adjacent_virtual_epa(cnt, senNoOp);
				if (ret) {
					virtul_poly[prevCnt][2].vert.y = BS_pd_pos_y[senNoOp] + width + pad;
					virtul_poly[prevCnt][2].vert.x = BS_sensor_zero_x;
					virtul_poly[prevCnt][2].edgeId = senNoOp;
				}
                else 
#endif
                {
#ifdef USE_CUST_MALLOC //nsmoon@201012
                    if (cnt < BS_max_virtual_ep_count) { //nsmoon@201014
#else
                    if (cnt < MAX_VIRTUAL_EP_COUNT) {
#endif
                        virtul_poly[cnt][0].vert.y = BS_led_pos_y[senNo];
                        virtul_poly[cnt][0].vert.x = BS_sensor_end_x;
                        virtul_poly[cnt][0].edgeId = (uint16_t)senNo;
                        virtul_poly[cnt][1].vert.y = BS_pd_pos_y[senNoOp] + width - pad;
                        virtul_poly[cnt][1].vert.x = BS_sensor_zero_x;
                        virtul_poly[cnt][1].edgeId = NO_CLIPPING_EDGEID;
						virtul_poly[cnt][2].vert.y = BS_pd_pos_y[senNoOp] + width + pad;
                        virtul_poly[cnt][2].vert.x = BS_sensor_zero_x;
                        virtul_poly[cnt][2].edgeId = (uint16_t)senNoOp;
                        virtul_poly[cnt][3].vert.y = BS_led_pos_y[senNo];
                        virtul_poly[cnt][3].vert.x = BS_sensor_end_x;
                        virtul_poly[cnt][3].edgeId = NO_CLIPPING_EDGEID;
                        cnt++;
                    }
                    else {
#ifdef USE_CUST_MALLOC //nsmoon@201012
                        TRACE_ERROR_MEM("ERROR! invalid BS_max_virtual_ep_count %d", BS_max_virtual_ep_count);
#else
                        TRACE_ERROR_MEM("ERROR! invalid MAX_VIRTUAL_EP_COUNT");
#endif
                        break;
                    }
				}
			}
				else {
					//(sensor == ENUM_PD)
                        width = (BS_led_pos_y[senNoOp + 1] - BS_led_pos_y[senNoOp]) * 0.5f;
					MAKE_VIRTUAL_PAD;
#ifdef VIRTUAL_EP_MERGE_ENABLE //nsmoon@200409 for 550-kiosk
				ret = is_adjacent_virtual_epa(cnt, senNoOp);
				if (ret) {
					virtul_poly[prevCnt][3].vert.y = BS_led_pos_y[senNoOp] + width + pad;
					virtul_poly[prevCnt][3].vert.x = BS_sensor_end_x;
					virtul_poly[prevCnt][3].edgeId = NO_CLIPPING_EDGEID;
				}
                else 
#endif
                {
#ifdef USE_CUST_MALLOC //nsmoon@201012
                    if (cnt < BS_max_virtual_ep_count) { //nsmoon@201014
#else
                    if (cnt < MAX_VIRTUAL_EP_COUNT) {
#endif
                        virtul_poly[cnt][0].vert.y = BS_led_pos_y[senNoOp] + width - pad;
                        virtul_poly[cnt][0].vert.x = BS_sensor_end_x;
                        virtul_poly[cnt][0].edgeId = (uint16_t)senNoOp;
                        virtul_poly[cnt][1].vert.y = BS_pd_pos_y[senNo];
                        virtul_poly[cnt][1].vert.x = BS_sensor_zero_x;
                        virtul_poly[cnt][1].edgeId = NO_CLIPPING_EDGEID;
                        virtul_poly[cnt][2].vert.y = BS_pd_pos_y[senNo];
                        virtul_poly[cnt][2].vert.x = BS_sensor_zero_x;
                        virtul_poly[cnt][2].edgeId = (uint16_t)senNo;
						virtul_poly[cnt][3].vert.y = BS_led_pos_y[senNoOp] + width + pad;
                        virtul_poly[cnt][3].vert.x = BS_sensor_end_x;
                        virtul_poly[cnt][3].edgeId = NO_CLIPPING_EDGEID;
                        cnt++;
                    }
                    else {
#ifdef USE_CUST_MALLOC //nsmoon@201012
                        TRACE_ERROR_MEM("ERROR! invalid BS_max_virtual_ep_count %d", BS_max_virtual_ep_count);
#else
                        TRACE_ERROR_MEM("ERROR! invalid MAX_VIRTUAL_EP_COUNT");
#endif
                        break;
                    }
				}
			}
		}
	} //for (senNoOp = senStOp; senNoOp <= senEdOp; senNoOp++)
	return cnt; //find deadzone
}
#endif //(LARGE_PITCH_TBL == 1)

ATTR_BACKEND_RAMFUNC
static int make_edge_pattern_dz(axis_t axis, ep_buf_t *epa, int side, int senStOp, int senEdOp)
{
    int slopeValMax, maxPdNum;
    int s1r, s2r, e1r, e2r;
    int s1l, s2l, e1l, e2l;
    //int sX, eX, sY, eY;
    int s1_min, s2_max, e1_min, e2_max;
    int first = 0, last = epa->len - 1;
    int ret;
    bd_type_t bd = BS_clipping_range_info[(int)axis].bd;
    sen_type_t sensor = BS_clipping_range_info[(int)axis].sensor;

    s1r = -1; s1l = -1; //init
    e1r = -1; e1l = -1; //init
    s2r = -1; s2l = -1; //init
    e2r = -1; e2l = -1; //init

    if (axis == ENUM_HOR_X) {
        //x-axis
        slopeValMax = BS_slopeValMaxX;
        maxPdNum = BS_maxHorPdNum - 1;
    }
    else {
        //y-axis
        slopeValMax = BS_slopeValMaxY;
        maxPdNum = BS_maxVerPdNum - 1;
    }

    //number of deadzone edge pattern
    BG_edge_pattern_dz.len = 2;

    if (sensor == ENUM_LED) {
        if (side == DEAD_ZONE_RIGHT) {
            //right side deadzone
            e1_min = epa->ep[first].s1 - slopeValMax;
            e1_min = GET_MAX(e1_min, 0);
            if (epa->ep[first].e1 > e1_min) {
                s1r = 0;
                e1r = 0;
                s2r = epa->ep[first].s1;
                e2r = e1_min;
            }
			if ((e1r > senEdOp && e2r > senEdOp) ||
				(e1r < senStOp && e2r < senStOp)) {
				return 0; //skip
			}
        }
        else {
            //left side deadzone
            e2_max = epa->ep[last].s2 + slopeValMax;
            e2_max = GET_MIN(e2_max, maxPdNum);
            if (epa->ep[last].e2 < e2_max) {
                s1l = epa->ep[last].s2;
                e1l = e2_max;
                s2l = maxPdNum;
                e2l = maxPdNum;
            }
			if ((e1l > senEdOp && e2l > senEdOp) ||
				(e1l < senStOp && e2l < senStOp)) {
				return 0; //skip
			}
        }
    }
    else {
        //(sensor == ENUM_PD)
        if (side == DEAD_ZONE_RIGHT) {
            //right side deadzone
            s1_min = epa->ep[first].e1 - slopeValMax;
            s1_min = GET_MAX(s1_min, 0);
            if (epa->ep[first].s1 > s1_min) {
                s1r = 0;
                e1r = 0;
                s2r = s1_min;
                e2r = epa->ep[first].e1;
            }
			if ((s1r > senEdOp && s2r > senEdOp) ||
				(s1r < senStOp && s2r < senStOp)) {
				return 0; //skip
			}
        }
        else {
            //left side deadzone
            s2_max = epa->ep[last].e2 + slopeValMax;
            s2_max = GET_MIN(s2_max, maxPdNum);
            if (epa->ep[last].s2 < s2_max) {
                s1l = s2_max;
                e1l = epa->ep[last].e2;
                s2l = maxPdNum;
                e2l = maxPdNum;
            }
			if ((s1l > senEdOp && s2l > senEdOp) ||
				(s1l < senStOp && s2l < senStOp)) {
				return 0; //skip
			}
        }
    }

    //right side deadzone
    if (s1r >= 0 && side == DEAD_ZONE_RIGHT) {
        ret = saveEdgePatternDeadzone(bd, side, s1r, s2r, e1r, e2r);
        if (ret) {
            return -1; //error
        }
        return 1; //found deadzone
    }

    //left side deadzone
    if (s1l >= 0 && side == DEAD_ZONE_LEFT) {
        ret = saveEdgePatternDeadzone(bd, side, s1l, s2l, e1l, e2l);
        if (ret) {
            return -1; //error
        }
        return 1; //found deadzone
    }

    return 0; //not found deadzone
}

ATTR_BACKEND_RAMFUNC
static int make_edge_pattern_empty_dz(axis_t axis, int slopeIdx, int side, int senStOp, int senEdOp)
{
    int slopeValMax, maxPdNum;
    int s1r = 0, s2r = 0, e1r = 0, e2r = 0;
    int s1l = 0, s2l = 0, e1l = 0, e2l = 0;
    //int sX, eX, sY, eY;
    int s1_min, s2_max, e1_min, e2_max;
    int ret;
    bd_type_t bd = BS_clipping_range_info[(int)axis].bd;
    sen_type_t sensor = BS_clipping_range_info[(int)axis].sensor;

    s1r = -1; s1l = -1; //init

    if (axis == ENUM_HOR_X) {
        //x-axis
        slopeValMax = BS_slopeValMaxX;
        maxPdNum = BS_maxHorPdNum - 1;
    }
    else {
        //y-axis
        slopeValMax = BS_slopeValMaxY;
        maxPdNum = BS_maxVerPdNum - 1;
    }

    //number of deadzone edge pattern
    BG_edge_pattern_dz.len = 2;

    if (sensor == ENUM_LED) {
        if (side == DEAD_ZONE_RIGHT) {
            //right side deadzone
            e1_min = slopeIdx - slopeValMax;
            e1_min = GET_MAX(e1_min, 0);
            s1r = 0;
            e1r = 0;
            s2r = slopeIdx;
            e2r = e1_min;
			if ((e1r > senEdOp && e2r > senEdOp) ||
				(e1r < senStOp && e2r < senStOp)) {
				return 0; //skip
			}
        }
        else {
            //left side deadzone
            e2_max = slopeIdx + slopeValMax;
            e2_max = GET_MIN(e2_max, maxPdNum);
            s1l = slopeIdx;
            e1l = e2_max;
            s2l = maxPdNum;
            e2l = maxPdNum;
			if ((e1l > senEdOp && e2l > senEdOp) ||
				(e1l < senStOp && e2l < senStOp)) {
				return 0; //skip
			}
        }
    }
    else {
        //(sensor == ENUM_PD)
        if (side == DEAD_ZONE_RIGHT) {
            //right side deadzone
            s1_min = slopeIdx - slopeValMax;
            s1_min = GET_MAX(s1_min, 0);
            s1r = 0;
            e1r = 0;
            s2r = s1_min;
            e2r = slopeIdx;
			if ((s1r > senEdOp && s2r > senEdOp) ||
				(s1r < senStOp && s2r < senStOp)) {
				return 0; //skip
			}
        }
        else {
            //left side deadzone
            s2_max = slopeIdx + slopeValMax;
            s2_max = GET_MIN(s2_max, maxPdNum);
            s1l = s2_max;
            e1l = slopeIdx;
            s2l = maxPdNum;
            e2l = maxPdNum;
			if ((s1l > senEdOp && s2l > senEdOp) ||
				(s1l < senStOp && s2l < senStOp)) {
				return 0; //skip
			}
        }
    }

    //right side deadzone
    if (side == DEAD_ZONE_RIGHT) {
        ret = saveEdgePatternDeadzone(bd, side, s1r, s2r, e1r, e2r);
        if (ret) {
            return -1; //error
        }
        return 1; //found deadzone
    }

    //left side deadzone
    if (side == DEAD_ZONE_LEFT) {
        ret = saveEdgePatternDeadzone(bd, side, s1l, s2l, e1l, e2l);
        if (ret) {
            return -1; //error
        }
        return 1; //found deadzone
    }

    return 0; //not-found
}

#if 0
ATTR_BACKEND_RAMFUNC
static void addEdgePatternPad(bd_type_t bd, vec_e_t *poly, vec_e_t *buf)
{
    int i;
#if 0 //mode-2
    for (i = 0; i < INITIAL_POLYGON_VERTEX_NO; i++) {
        buf[i].edgeId = poly[i].edgeId;
        buf[i].vert = poly[i].vert;
        if (bd == ENUM_BOT_BD || bd == ENUM_TOP_BD) {
            if (i == 0 || i == 1) {
                buf[i].vert.x = poly[i].vert.x - PAD_WITH_MULTI;
            }
            else if (i == 2 || i == 3) {
                buf[i].vert.x = poly[i].vert.x + PAD_WITH_MULTI;
            }
        }
        else {
            //(bd == ENUM_LFT_BD ||bd == ENUM_RHT_BD)
            if (i == 0 || i == 1) {
                buf[i].vert.y = poly[i].vert.y - PAD_WITH_MULTI;
            }
            else if (i == 2 || i == 3) {
                buf[i].vert.y = poly[i].vert.y + PAD_WITH_MULTI;
            }
        }
    }
#endif //mode-2
#if 1 //mode-1
    for (i = 0; i < INITIAL_POLYGON_VERTEX_NO; i++) {
        buf[i].edgeId = poly[i].edgeId;
        buf[i].vert = poly[i].vert;

        if (bd == ENUM_BOT_BD) {
            if (i == 1) {
                buf[i].vert.x = poly[i].vert.x - MULTI_PAD_WIDTH;
            }
            else if (i == 2) {
                buf[i].vert.x = poly[i].vert.x + MULTI_PAD_WIDTH;
            }
        }
        else if (bd == ENUM_TOP_BD) {
            if (i == 0) {
                buf[i].vert.x = poly[i].vert.x - MULTI_PAD_WIDTH;
            }
            else if (i == 3) {
                buf[i].vert.x = poly[i].vert.x + MULTI_PAD_WIDTH;
            }
        }
        else if (bd == ENUM_LFT_BD) {
            if (i == 1) {
                buf[i].vert.y = poly[i].vert.y - MULTI_PAD_WIDTH;
            }
            else if (i == 2) {
                buf[i].vert.y = poly[i].vert.y + MULTI_PAD_WIDTH;
            }
        }
        else {
            //(bd == ENUM_RHT_BD)
            if (i == 0) {
                buf[i].vert.y = poly[i].vert.y - MULTI_PAD_WIDTH;
            }
            else if (i == 3) {
                buf[i].vert.y = poly[i].vert.y + MULTI_PAD_WIDTH;
            }
        }
    }
#endif //mode-1
}
#endif

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_addEdgePatternHalfPad 0
#endif
//#define FIXED_PAD_WIDTH_MULTI	2.5f //nsmoon@181030
#ifdef FIXED_PAD_WIDTH_MULTI
ATTR_BACKEND_RAMFUNC
static void addEdgePatternHalfPad(bd_type_t bd, ep_buf_t *epa_all, int slopeIdx, int clipIdx, vec_e_t *buf)
{
    int i;
    int s1, s2, e1, e2;
    float padRight, padLeft;
    ep_buf_t *epa;

    if (slopeIdx >= 0) {
        epa = &epa_all[slopeIdx];
    }
    else {
        epa = epa_all;
        //FIXME, if PAD_WITH_FACTOR_DZ_MULTI == 0
        for (i = 0; i < INITIAL_POLYGON_VERTEX_NO; i++) {
            buf[i].edgeId = epa->poly[clipIdx][i].edgeId;
            buf[i].vert = epa->poly[clipIdx][i].vert;
        }
        return;
    }
#if (DEBUG_addEdgePatternHalfPad == 1)
    TRACE("addEdgePatternHalfPad..epa->ep[]: %d %d %d %x", bd, slopeIdx, clipIdx, &epa->ep[clipIdx]);
#endif

    if (bd == ENUM_BOT_BD) {
        //x-axis, ped, e1, e2
        e1 = epa->ep[clipIdx].e1;
        e2 = epa->ep[clipIdx].e2;
        padRight = (e1 > 0) ? FIXED_PAD_WIDTH_MULTI : 0;
        padLeft = (e2 < (BS_maxHorPdNum - 1)) ? FIXED_PAD_WIDTH_MULTI : 0;
    }
    else if (bd == ENUM_TOP_BD) {
        //x-axis, led, s1, s2
        s1 = epa->ep[clipIdx].s1;
        s2 = epa->ep[clipIdx].s2;
        padRight = (s1 > 0) ? FIXED_PAD_WIDTH_MULTI : 0;
        padLeft = (s2 < (BS_maxHorPdNum - 1)) ? FIXED_PAD_WIDTH_MULTI : 0;
    }
    else if (bd == ENUM_LFT_BD) {
        //y-axis, pd, e1, e2
        e1 = epa->ep[clipIdx].e1;
        e2 = epa->ep[clipIdx].e2;
        padRight = (e1 > 0) ? FIXED_PAD_WIDTH_MULTI : 0;
        padLeft = (e2 < (BS_maxVerPdNum - 1)) ? FIXED_PAD_WIDTH_MULTI : 0;
    }
    else {
        //(bd == ENUM_RHT_BD)
        //y-axis, led, s1, s2
        s1 = epa->ep[clipIdx].s1;
        s2 = epa->ep[clipIdx].s2;
        padRight = (s1 > 0) ? FIXED_PAD_WIDTH_MULTI : 0;
        padLeft = (s2 < (BS_maxVerPdNum - 1)) ? FIXED_PAD_WIDTH_MULTI : 0;
    }

#if 1 //mode-3
    for (i = 0; i < INITIAL_POLYGON_VERTEX_NO; i++) {
        buf[i].edgeId = epa->poly[clipIdx][i].edgeId;
        buf[i].vert = epa->poly[clipIdx][i].vert;
        if (bd == ENUM_BOT_BD) {
            //LED
            if (i == 1) {
                buf[i].vert.x = epa->poly[clipIdx][i].vert.x - padRight;
            }
            else if (i == 2) {
                buf[i].vert.x = epa->poly[clipIdx][i].vert.x + padLeft;
            }
        }
        else if (bd == ENUM_TOP_BD) {
            if (i == 0) {
                buf[i].vert.x = epa->poly[clipIdx][i].vert.x - padRight;
            }
            else if (i == 3) {
                buf[i].vert.x = epa->poly[clipIdx][i].vert.x + padLeft;
            }
        }
        else if (bd == ENUM_LFT_BD) {
            if (i == 1) {
                buf[i].vert.y = epa->poly[clipIdx][i].vert.y - padRight;
            }
            else if (i == 2) {
                buf[i].vert.y = epa->poly[clipIdx][i].vert.y + padLeft;
            }
        }
        else {
            //(bd == ENUM_RHT_BD)
            if (i == 0) {
                buf[i].vert.y = epa->poly[clipIdx][i].vert.y - padRight;
            }
            else if (i == 3) {
                buf[i].vert.y = epa->poly[clipIdx][i].vert.y + padLeft;
            }
        }
    }
#endif //mode-1
}
#else
//limit_num, addEdgePatternHalfPad()
#define CHK_NON_DEADZONE	0
#define CHK_RHT_DEADZONE	1
#define CHK_LFT_DEADZONE	2
#define CHK_ALL_DEADZONE	3
ATTR_BACKEND_RAMFUNC
//static int addEdgePatternHalfPad(axis_t axis, ep_buf_t *epa_all, int slopeIdx, int clipIdx, vec_e_t *buf, int paddingMode)
int addEdgePatternHalfPad(axis_t axis, ep_buf_t *epa_all, int slopeIdx, int clipIdx, vec_e_t *buf, int paddingMode)
{
    int i;
    int s1, s2, e1, e2;
    float padRight, padLeft;
    //float padFactor;
    ep_buf_t *epa;
	//int vMode = 0; // virtualEpaMode; //FIXME, nsmoon@200115
    bd_type_t bd = BS_clipping_range_info[(int)axis].bd;
#ifdef USE_CUST_MALLOC //nsmoon@201012
    vec_e_t epaPoly[INITIAL_POLYGON_VERTEX_NO]; //nsmoon@201008
#endif

    if (slopeIdx >= 0) {
        epa = &epa_all[slopeIdx];
        //padFactor = MULTI_PAD_WIDTH_FACTOR;
#ifdef USE_CUST_MALLOC //nsmoon@201012
        multi_get_initial_poly(axis, &epa->ep[clipIdx], &epaPoly[0], 0);
#endif
    }
    else {
        //for dead zone
        epa = epa_all;
        //padFactor = MULTI_PAD_WIDTH_FACTOR_DZ;
        //FIXME, if PAD_WITH_FACTOR_DZ_MULTI == 0
#ifdef USE_CUST_MALLOC //nsmoon@201012
        multi_get_initial_poly(axis, &epa->ep[clipIdx], &epaPoly[0], 0);
#endif
        for (i = 0; i < INITIAL_POLYGON_VERTEX_NO; i++) {
#ifdef USE_CUST_MALLOC //nsmoon@201012
            buf[i].edgeId = epaPoly[i].edgeId;
            buf[i].vert = epaPoly[i].vert;
#else
            buf[i].edgeId = epa->poly[clipIdx][i].edgeId;
            buf[i].vert = epa->poly[clipIdx][i].vert;
#endif
        }
        return 0; //if slopeIdx < 0, do not care about return value
    }
#if (DEBUG_addEdgePatternHalfPad == 1)
    TRACE("addEdgePatternHalfPad..epa->ep[]: %d %d %d %x", bd, slopeIdx, clipIdx, &epa->ep[clipIdx]);
#endif

    s1 = epa->ep[clipIdx].s1;
    s2 = epa->ep[clipIdx].s2;
    e1 = epa->ep[clipIdx].e1;
    e2 = epa->ep[clipIdx].e2;

    if (bd == ENUM_BOT_BD) {
        //x-axis, pd, e1, e2
        padRight = (e1 > 0) ? (BS_pd_pos_x[e1] - BS_pd_pos_x[e1 - 1]) : 0;
        padLeft = (e2 < (BS_maxHorPdNum - 1)) ? (BS_pd_pos_x[e2 + 1] - BS_pd_pos_x[e2]) : 0;
    }
    else if (bd == ENUM_TOP_BD) {
        //x-axis, led, s1, s2
        padRight = (s1 > 0) ? (BS_led_pos_x[s1] - BS_led_pos_x[s1 - 1]) : 0;
        padLeft = (s2 < (BS_maxHorPdNum - 1)) ? (BS_led_pos_x[s2 + 1] - BS_led_pos_x[s2]) : 0;
    }
    else if (bd == ENUM_LFT_BD) {
        //y-axis, pd, e1, e2
        padRight = (e1 > 0) ? (BS_pd_pos_y[e1] - BS_pd_pos_y[e1 - 1]) : 0;
        padLeft = (e2 < (BS_maxVerPdNum - 1)) ? (BS_pd_pos_y[e2 + 1] - BS_pd_pos_y[e2]) : 0;
    }
    else {
        //(bd == ENUM_RHT_BD)
        //y-axis, led, s1, s2
        padRight = (s1 > 0) ? (BS_led_pos_y[s1] - BS_led_pos_y[s1 - 1]) : 0;
        padLeft = (s2 < (BS_maxVerPdNum - 1)) ? (BS_led_pos_y[s2 + 1] - BS_led_pos_y[s2]) : 0;
    }

#ifdef USE_CUST_MALLOC //nsmoon@201012
//#if (MODEL_TOUCH_SIZE == MODEL_TOUCH_SIZE_75) || (MODEL_TOUCH_SIZE == MODEL_TOUCH_SIZE_85) //nsmoon@200109
	for (i = 0; i < INITIAL_POLYGON_VERTEX_NO; i++) {
        buf[i].edgeId = epaPoly[i].edgeId;
        buf[i].vert = epaPoly[i].vert;
        if (0/*paddingMode == 1*/) {
            //small padding
            if ((bd == ENUM_BOT_BD && i == 1) || (bd == ENUM_TOP_BD && i == 0)) {
                buf[i].vert.x = epaPoly[i].vert.x - MULTI_LIM_EPA_PAD3;
            }
            else if ((bd == ENUM_BOT_BD && i == 2) || (bd == ENUM_TOP_BD && i == 3)) {
                buf[i].vert.x = epaPoly[i].vert.x + MULTI_LIM_EPA_PAD3;
            }
            else if ((bd == ENUM_LFT_BD && i == 1) || (bd == ENUM_RHT_BD && i == 0)) {
                buf[i].vert.y = epaPoly[i].vert.y - MULTI_LIM_EPA_PAD3;
            }
            else if ((bd == ENUM_LFT_BD && i == 2) || (bd == ENUM_RHT_BD && i == 3)) {
                buf[i].vert.y = epaPoly[i].vert.y + MULTI_LIM_EPA_PAD3;
            }
        }
        else {
            //normal padding
		if ((bd == ENUM_BOT_BD && i == 1) || (bd == ENUM_TOP_BD && i == 0)) {
                buf[i].vert.x = epaPoly[i].vert.x - (padRight - MULTI_LIM_EPA_PAD2);
		}
		else if ((bd == ENUM_BOT_BD && i == 2) || (bd == ENUM_TOP_BD && i == 3)) {
                buf[i].vert.x = epaPoly[i].vert.x + (padLeft - MULTI_LIM_EPA_PAD2);
		}
		else if ((bd == ENUM_LFT_BD && i == 1) || (bd == ENUM_RHT_BD && i == 0)) {
                buf[i].vert.y = epaPoly[i].vert.y - (padRight - MULTI_LIM_EPA_PAD2);
		}
		else if ((bd == ENUM_LFT_BD && i == 2) || (bd == ENUM_RHT_BD && i == 3)) {
                buf[i].vert.y = epaPoly[i].vert.y + (padLeft - MULTI_LIM_EPA_PAD2);
		}
	}
    }
#else //(MODEL_TOUCH_SIZE == MODEL_TOUCH_SIZE_85) //nsmoon@200422
	for (i = 0; i < INITIAL_POLYGON_VERTEX_NO; i++) {
		buf[i].edgeId = epa->poly[clipIdx][i].edgeId;
		buf[i].vert = epa->poly[clipIdx][i].vert;
        if (0/*paddingMode == 1*/) {
            //small padding
            if ((bd == ENUM_BOT_BD && i == 1) || (bd == ENUM_TOP_BD && i == 0)) {
                buf[i].vert.x = epa->poly[clipIdx][i].vert.x - MULTI_LIM_EPA_PAD3;
            }
            else if ((bd == ENUM_BOT_BD && i == 2) || (bd == ENUM_TOP_BD && i == 3)) {
                buf[i].vert.x = epa->poly[clipIdx][i].vert.x + MULTI_LIM_EPA_PAD3;
            }
            else if ((bd == ENUM_LFT_BD && i == 1) || (bd == ENUM_RHT_BD && i == 0)) {
                buf[i].vert.y = epa->poly[clipIdx][i].vert.y - MULTI_LIM_EPA_PAD3;
            }
            else if ((bd == ENUM_LFT_BD && i == 2) || (bd == ENUM_RHT_BD && i == 3)) {
                buf[i].vert.y = epa->poly[clipIdx][i].vert.y + MULTI_LIM_EPA_PAD3;
            }
        }
        else {
            //normal padding
            if ((bd == ENUM_BOT_BD && i == 1) || (bd == ENUM_TOP_BD && i == 0)) {
			buf[i].vert.x = epa->poly[clipIdx][i].vert.x - (padRight - MULTI_LIM_EPA_PAD2);
            }
            else if ((bd == ENUM_BOT_BD && i == 2) || (bd == ENUM_TOP_BD && i == 3)) {
			buf[i].vert.x = epa->poly[clipIdx][i].vert.x + (padLeft - MULTI_LIM_EPA_PAD2);
            }
            else if ((bd == ENUM_LFT_BD && i == 1) || (bd == ENUM_RHT_BD && i == 0)) {
			buf[i].vert.y = epa->poly[clipIdx][i].vert.y - (padRight - MULTI_LIM_EPA_PAD2);
            }
            else if ((bd == ENUM_LFT_BD && i == 2) || (bd == ENUM_RHT_BD && i == 3)) {
			buf[i].vert.y = epa->poly[clipIdx][i].vert.y + (padLeft - MULTI_LIM_EPA_PAD2);
            }
	}
    }
#endif
#if 0 //for test
    for (i = 0; i < INITIAL_POLYGON_VERTEX_NO; i++) {
        buf[i].edgeId = epa->poly[clipIdx][i].edgeId;
        buf[i].vert = epa->poly[clipIdx][i].vert;
        if (bd == ENUM_BOT_BD) {
            //LED
            if (i == 1) {
                buf[i].vert.x = epa->poly[clipIdx][i].vert.x - (padRight * padFactor);
            }
            else if (i == 2) {
                buf[i].vert.x = epa->poly[clipIdx][i].vert.x + (padLeft * padFactor);
            }
        }
        else if (bd == ENUM_TOP_BD) {
            if (i == 0) {
                buf[i].vert.x = epa->poly[clipIdx][i].vert.x - (padRight * padFactor);
            }
            else if (i == 3) {
                buf[i].vert.x = epa->poly[clipIdx][i].vert.x + (padLeft * padFactor);
            }
        }
        else if (bd == ENUM_LFT_BD) {
            if (i == 1) {
                buf[i].vert.y = epa->poly[clipIdx][i].vert.y - (padRight * padFactor);
            }
            else if (i == 2) {
                buf[i].vert.y = epa->poly[clipIdx][i].vert.y + (padLeft * padFactor);
            }
        }
        else {
            //(bd == ENUM_RHT_BD)
            if (i == 0) {
                buf[i].vert.y = epa->poly[clipIdx][i].vert.y - (padRight * padFactor);
            }
            else if (i == 3) {
                buf[i].vert.y = epa->poly[clipIdx][i].vert.y + (padLeft * padFactor);
            }
        }
    }
#endif //mode-3

#if 1 //nsmoon@190321, new-m5p-0322d-604
    return CHK_ALL_DEADZONE;
#endif
}
#endif //FIXED_PAD_WIDTH_MULTI

#if 0 //reserved, do not remove
//static int is_overlap_pos_min_max(touch_info_multi_t *a, touch_info_multi_t *b)
static int multi_is_overlap_erase_min_max(touch_info_multi_t *a, touch_info_multi_t *b)
{
	int ol_x1, ol_x2, ol_x3, ol_x4, ol_y1, ol_y2, ol_y3, ol_y4;

	if (a->mM.minX > a->mM.maxX || b->mM.minX > b->mM.maxX) {
		TRACE_ERROR("ERROR! invalid min,max: %f,%f %f,%f", a->mM.minX, a->mM.maxX, b->mM.minX, b->mM.maxX);
	}
	float sizeSq_a = (a->mM.maxX - a->mM.minX) * (a->mM.maxY - a->mM.minY);
	float sizeSq_b = (b->mM.maxX - b->mM.minX) * (b->mM.maxY - b->mM.minY);
    if (sizeSq_a > TOUCH_LARGE_ERASE_SIZE || sizeSq_b > TOUCH_LARGE_ERASE_SIZE)
	{
        ol_x1 = (a->mM.minX > (b->mM.minX - MULTI_MAX_LARGE_ERASE_GAP) && a->mM.minX < (b->mM.maxX + MULTI_MAX_LARGE_ERASE_GAP));
        ol_x2 = (a->mM.maxX > (b->mM.minX - MULTI_MAX_LARGE_ERASE_GAP) && a->mM.maxX < (b->mM.maxX + MULTI_MAX_LARGE_ERASE_GAP));
        ol_x3 = (b->mM.minX > (a->mM.minX - MULTI_MAX_LARGE_ERASE_GAP) && b->mM.minX < (a->mM.maxX + MULTI_MAX_LARGE_ERASE_GAP));
        ol_x4 = (b->mM.maxX > (a->mM.minX - MULTI_MAX_LARGE_ERASE_GAP) && b->mM.maxX < (a->mM.maxX + MULTI_MAX_LARGE_ERASE_GAP));
        ol_y1 = (a->mM.minY > (b->mM.minY - MULTI_MAX_LARGE_ERASE_GAP) && a->mM.minY < (b->mM.maxY + MULTI_MAX_LARGE_ERASE_GAP));
        ol_y2 = (a->mM.maxY > (b->mM.minY - MULTI_MAX_LARGE_ERASE_GAP) && a->mM.maxY < (b->mM.maxY + MULTI_MAX_LARGE_ERASE_GAP));
        ol_y3 = (b->mM.minY > (a->mM.minY - MULTI_MAX_LARGE_ERASE_GAP) && b->mM.minY < (a->mM.maxY + MULTI_MAX_LARGE_ERASE_GAP));
        ol_y4 = (b->mM.maxY > (a->mM.minY - MULTI_MAX_LARGE_ERASE_GAP) && b->mM.maxY < (a->mM.maxY + MULTI_MAX_LARGE_ERASE_GAP));
		if ((ol_x1 || ol_x2 || ol_x3 || ol_x4) && (ol_y1 || ol_y2 || ol_y3 || ol_y4)) {
			//TRACE("FOUND large erase~~");
			//TRACE("LG");
			if (sizeSq_a < sizeSq_b) {
				return 1; //a < b
			}
			else {
				return 2; //a > b
			}
		}
	}

    return 0; //not-overlapped
}
#endif //1

#define TRACE_MOCM(...)    //TRACE(__VA_ARGS__)
#define MULTI_MAX_OVERLAP_GAP			EPSILON //0.5f //3.0f //5.0f //0.5f //0.8f
#define MULTI_MAX_OVERLAP_AREA_RATIO	0.9f
static int multi_is_overlap_closed_min_max(int idx_i, int idx_j, int mode)
{
    //mode: 0(size), 1(clipCnt)
    touch_info_multi_t *nM_i = &BS_touch_info_multi[idx_i];
    touch_info_multi_t *nM_j = &BS_touch_info_multi[idx_j];
    int ol_x1, ol_x2, ol_x3, ol_x4, ol_y1, ol_y2, ol_y3, ol_y4;
    float max_overlap_gap = MULTI_MAX_OVERLAP_GAP;

    //check ovelapped min/max
    TRACE_MOCM("overlap_closed_min_max: %0.1f %0.1f %0.1f %0.1f - %0.1f %0.1f %0.1f %0.1f", nM_i->mM.minX, nM_i->mM.maxX, nM_i->mM.minY, nM_i->mM.maxY, nM_j->mM.minX, nM_j->mM.maxX, nM_j->mM.minY, nM_j->mM.maxY);
    ol_x1 = (nM_i->mM.minX > (nM_j->mM.minX - max_overlap_gap) && nM_i->mM.minX < (nM_j->mM.maxX + max_overlap_gap));
    ol_x2 = (nM_i->mM.maxX > (nM_j->mM.minX - max_overlap_gap) && nM_i->mM.maxX < (nM_j->mM.maxX + max_overlap_gap));
    ol_x3 = (nM_j->mM.minX > (nM_i->mM.minX - max_overlap_gap) && nM_j->mM.minX < (nM_i->mM.maxX + max_overlap_gap));
    ol_x4 = (nM_j->mM.maxX > (nM_i->mM.minX - max_overlap_gap) && nM_j->mM.maxX < (nM_i->mM.maxX + max_overlap_gap));
    ol_y1 = (nM_i->mM.minY > (nM_j->mM.minY - max_overlap_gap) && nM_i->mM.minY < (nM_j->mM.maxY + max_overlap_gap));
    ol_y2 = (nM_i->mM.maxY > (nM_j->mM.minY - max_overlap_gap) && nM_i->mM.maxY < (nM_j->mM.maxY + max_overlap_gap));
    ol_y3 = (nM_j->mM.minY > (nM_i->mM.minY - max_overlap_gap) && nM_j->mM.minY < (nM_i->mM.maxY + max_overlap_gap));
    ol_y4 = (nM_j->mM.maxY > (nM_i->mM.minY - max_overlap_gap) && nM_j->mM.maxY < (nM_i->mM.maxY + max_overlap_gap));

#if 0 //nsmoon@200217
    if (mode == 1) {
        if ((ol_x1 || ol_x2 || ol_x3 || ol_x4) && (ol_y1 || ol_y2 || ol_y3 || ol_y4)) {
            //if (nM_j->polLen < nM_i->polLen || (nM_j->polLen == nM_i->polLen && nM_j->clipCnt < nM_i->clipCnt))
            if (nM_j->clipCnt < nM_i->clipCnt || (nM_j->clipCnt == nM_i->clipCnt && nM_j->polLen < nM_i->polLen))
            {
                return idx_j; //remove j
            }
            else {
                return idx_i; //remove i
            }
        }
        return 0; //not-overlapped
    }
#endif
#if 1 //size
    if ((ol_x1 || ol_x2 || ol_x3 || ol_x4) && (ol_y1 || ol_y2 || ol_y3 || ol_y4)) {
        //areaRatio = nM_j->multiArea / nM_i->multiArea;
        TRACE_MOCM("areaRatio: %0.1f %0.1f", nM_i->multiArea, nM_j->multiArea);
        if (nM_j->multiArea < nM_i->multiArea) {
            return idx_j; //remove j
        }
        else {
            return idx_i; //remove i
        }
    }
    #endif
    return -1; //not-overlapped
}

#define MULTI_OVERLAP_TOL	5.0f //EPSILON
#define MULTI_OVERLAP_DIST_1	10.0f //all
#define MULTI_OVERLAP_DIST_2	20.0f //49.0f
#define MULTI_OVERLAP_AREA		64.0f //81.0f

#define MULTI_LOOP2_REMOVE_SIZESQ           64.0f //8mm
#define MULTI_LOOP2_REMOVE_DIST_OV_SIZESQ   3.0f //15.0f //15mm
#define MULTI_LOOP2_REMOVE_DIST             3.0f //10.0f //10mm

int BS_multi_is_overlap_min_max_tp(pos_min_max_t mM, int excludeRemoved, int compareMode)
{
    //includeRemoved: 0:include all, 1:exclude removed touch
    //compareMode: 0:cent-cent, 1:min/max-min/max,
    //compareMode: 2:cent-min/max with size, 3:min/max-cent

    touch_info_multi_t *info = &BS_touch_info_multi[0];
    int ol_x1, ol_x2, ol_x3, ol_x4, ol_y1, ol_y2, ol_y3, ol_y4;
	float distX, distY, distMax = MULTI_OVERLAP_DIST_1;
	vec_t cent;
    float sizeSq, sizeX, sizeY;
    int i;
    cent.x = (mM.minX + mM.maxX) * 0.5f;
	cent.y = (mM.minY + mM.maxY) * 0.5f;

	//TRACE("cent.x,cent.y=%0.1f, %0.1f", cent.x, cent.y);
    for (i = 0; i < BS_touch_info_multi_cnt; i++) {
		//DEBUG_SHOW_MIN_MAX(&info[i].mM, MY_COLOR, 1);
        if (excludeRemoved) {
			if ((info[i].multiStat & MULTISTAT_REMOVED_MASK) != 0) {
				continue; //removed, FIXME
			}
		}
        if (compareMode == 0) {
            //cent to cent
            distX = GET_ABS(cent.x - info[i].centerPos.x);
            distY = GET_ABS(cent.y - info[i].centerPos.y);
            if (info[i].multiArea > MULTI_OVERLAP_AREA) {
                distMax = MULTI_OVERLAP_DIST_2;
            }
            //TRACE("distX,distY=%0.1f %0.1f (%0.1f)", distX, distY, distMax);
            if (distX < distMax && distY < distMax) {
                return 1; //overlapped
            }
        }
        else if (compareMode == 1) {
            //min/max to min/max
            if (info[i].mM.minX > info[i].mM.maxX || mM.minX > mM.maxX) {
                TRACE_ERROR("ERROR! invalid min,max: %f,%f %f,%f", info[i].mM.minX, info[i].mM.maxX, mM.minX, mM.maxX);
            }
            ol_x1 = (mM.minX > (info[i].mM.minX - MULTI_OVERLAP_TOL) && mM.minX < (info[i].mM.maxX + MULTI_OVERLAP_TOL));
            ol_x2 = (mM.maxX > (info[i].mM.minX - MULTI_OVERLAP_TOL) && mM.maxX < (info[i].mM.maxX + MULTI_OVERLAP_TOL));
            ol_y1 = (mM.minY > (info[i].mM.minY - MULTI_OVERLAP_TOL) && mM.minY < (info[i].mM.maxY + MULTI_OVERLAP_TOL));
            ol_y2 = (mM.maxY > (info[i].mM.minY - MULTI_OVERLAP_TOL) && mM.maxY < (info[i].mM.maxY + MULTI_OVERLAP_TOL));
            ol_x3 = (info[i].mM.minX > (mM.minX - MULTI_OVERLAP_TOL) && info[i].mM.minX < (mM.maxX + MULTI_OVERLAP_TOL));
            ol_x4 = (info[i].mM.maxX > (mM.minX - MULTI_OVERLAP_TOL) && info[i].mM.maxX < (mM.maxX + MULTI_OVERLAP_TOL));
            ol_y3 = (info[i].mM.minY > (mM.minY - MULTI_OVERLAP_TOL) && info[i].mM.minY < (mM.maxY + MULTI_OVERLAP_TOL));
            ol_y4 = (info[i].mM.maxY > (mM.minY - MULTI_OVERLAP_TOL) && info[i].mM.maxY < (mM.maxY + MULTI_OVERLAP_TOL));
            if ((ol_x1 || ol_x2 || ol_x3 || ol_x4) && (ol_y1 || ol_y2 || ol_y3 || ol_y4)) {
                //TRACE("overlapped~~");
                return 1;
            }
        }
        else if (compareMode == 2) {
            //cent to min/max with sizeSq
            sizeX = GET_ABS(info[i].mM.maxX - info[i].mM.minX);
            sizeY = GET_ABS(info[i].mM.maxY - info[i].mM.minY);
            sizeSq = sizeX * sizeY;
            if (sizeSq > MULTI_LOOP2_REMOVE_SIZESQ - EPSILON) {
                ol_x1 = (cent.x > (info[i].mM.minX - MULTI_LOOP2_REMOVE_DIST_OV_SIZESQ) && cent.x < (info[i].mM.maxX + MULTI_LOOP2_REMOVE_DIST_OV_SIZESQ));
                ol_y1 = (cent.y > (info[i].mM.minY - MULTI_LOOP2_REMOVE_DIST_OV_SIZESQ) && cent.y < (info[i].mM.maxY + MULTI_LOOP2_REMOVE_DIST_OV_SIZESQ));
            }
            else {
                ol_x1 = (cent.x > (info[i].mM.minX - MULTI_LOOP2_REMOVE_DIST) && cent.x < (info[i].mM.maxX + MULTI_LOOP2_REMOVE_DIST));
                ol_y1 = (cent.y > (info[i].mM.minY - MULTI_LOOP2_REMOVE_DIST) && cent.y < (info[i].mM.maxY + MULTI_LOOP2_REMOVE_DIST));
            }
            if (ol_x1 && ol_y1) {
                //TRACE("overlapped~~");
                return 1;
            }
        }
    } //for (i = 0; i < BS_touch_info_multi_cnt; i++)

    return 0; //not-overlapped
}

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_clipping_multi_remained	0 //1:enable
#endif
#if (DEBUG_clipping_multi_remained > 1)
  #define TRACE_CMR(...)	TRACE(__VA_ARGS__)
  #define TRACENR_CMR(...)	TRACENR(__VA_ARGS__)
  #define IS_CLRSCR			if(1) //1:clear screen
#else
  #define TRACE_CMR(...)
  #define TRACENR_CMR(...)
  #define IS_CLRSCR
#endif
#define MULTI_CLIPPING_RANGE   //skip clipping, nsmoon@190702, FIXME remained line error
#define MULTI_SKIP_NO_CLIPPING //skip no-clipping, nsmoon@190715

#if 0 //do not remove, reserved
static int multi_adj_initial_min_max(axis_t axis, ep_min_max_t *ep, pos_min_max_t *pos)
{
	int minX, maxX, minY, maxY;
	float minXpos, maxXpos, minYpos, maxYpos;
    float *posX = BS_clipping_range_info[(int)axis].senPosX;
    float *posY = BS_clipping_range_info[(int)axis].senPosY;
#if (DEBUG_multi_get_clipping_range > 0)
	IS_DEBUG_FLAG2{
		DEBUG_SHOW_MIN_MAX(pos, 0, 0);
	};
#endif

	//get initial polygon
	minX = ep->minX;
	maxX = ep->maxX;
	minY = ep->minY;
	maxY = ep->maxY;
	minXpos = posX[minX];
	maxXpos = posX[maxX];
	minYpos = posY[minY];
	maxYpos = posY[maxY];

	//adjust min and man sensor number
	while (minXpos > pos->minX) {
		if (minX > 0) {
			minX--;
			minXpos = posX[minX];
		}
		else {
			break;
		}
	}
	while (maxXpos < pos->maxX) {
		if (maxX < (BS_maxHorPdNum - 1)) {
			maxX++;
			maxXpos = posX[maxX];
		}
		else {
			break;
		}
	}
	while (minYpos > pos->minY) {
		if (minY > 0) {
			minY--;
			minYpos = posY[minY];
		}
		else {
			break;
		}
	}
	while (maxYpos < pos->maxY) {
		if (maxY < (BS_maxVerPdNum - 1)) {
			maxY++;
			maxYpos = posY[maxY];
		}
		else {
			break;
		}
	}
	if (minX < 0 || minY < 0 ||
		maxX >= BS_maxHorPdNum || maxY >= BS_maxVerPdNum) {
		TRACE_ERROR("ERROR! invalid minX,maxX,minY,maxY: %d, %d, %d, %d", minX, maxX, minY, maxY);
		return 1;
	}

	//save return value
	ep->minX = minX;
	ep->maxX = maxX;
	ep->minY = minY;
	ep->maxY = maxY;
	pos->minX = minXpos;
	pos->maxX = maxXpos;
	pos->minY = minYpos;
	pos->maxY = maxYpos;
#if (DEBUG_multi_get_clipping_range > 0)
	IS_DEBUG_FLAG2{
	    DEBUG_SHOW_MIN_MAX(pos, 1, 0);
	};
#endif
	return 0; //no-error
}
#endif

#if 0 //for test
#define INITIAL_POL_TOL		4
static void multi_get_clipping_range(axis_t axis, ep_min_max_t *minMaxEp, int *minSenlim, int *maxSenlim)
{
	sen_no_t centSen;
	int dSen;
	centSen.x = GET_LINE_CENT_INT(minMaxEp->minX, minMaxEp->maxX);
	centSen.y = GET_LINE_CENT_INT(minMaxEp->minY, minMaxEp->maxY);
    IS_DEBUG_FLAG {
      TRACE_NOP;
    };

	if (axis == ENUM_VER_Y) {
		if (centSen.x < (BS_maxHorPdNum / 2)) {
			//right-side
			dSen = ((BS_slopeValMaxY * (minMaxEp->maxX + (minMaxEp->maxX / INITIAL_POL_TOL))) / BS_maxHorPdNum);
		}
		else {
			//left-side
			dSen = ((BS_slopeValMaxY * ((BS_maxHorPdNum - minMaxEp->minX) + ((BS_maxHorPdNum - minMaxEp->minX) / INITIAL_POL_TOL))) / BS_maxHorPdNum);
		}
		*minSenlim = GET_MAX((minMaxEp->minY - dSen), 0);
		*maxSenlim = GET_MIN((minMaxEp->maxY + dSen), (BS_maxVerPdNum - 1));
		TRACE_NOP;
	}
	else {
		if (centSen.y < (BS_maxVerPdNum / 2)) {
			//right-side
			dSen = ((BS_slopeValMaxX * (minMaxEp->maxY + (minMaxEp->maxY / INITIAL_POL_TOL))) / BS_maxVerPdNum);
		}
		else {
			//left-side
			dSen = ((BS_slopeValMaxX * (BS_maxVerPdNum - (minMaxEp->minY - (minMaxEp->minY / INITIAL_POL_TOL)))) / BS_maxVerPdNum);
		}
		*minSenlim = GET_MAX((minMaxEp->minX - dSen), 0);
		*maxSenlim = GET_MIN((minMaxEp->maxX + dSen), (BS_maxHorPdNum - 1));
		TRACE_NOP;
	}
}
#endif //0

#ifdef MULTI_CLIPPING_RANGE //nsmoon@200108
static int get_subj_min_max(poly_e_t *subjecte, pos_min_max_t *minMaxPos)
{
	int i;
	float minX = MIN_INITIAL_VAL;
	float maxX = -1;
	float minY = MIN_INITIAL_VAL;
	float maxY = -1;

	for (i = 0; i < subjecte->len; i++) {
		minX = GET_MIN(subjecte->ve[i].vert.x, minX);
		maxX = GET_MAX(subjecte->ve[i].vert.x, maxX);
		minY = GET_MIN(subjecte->ve[i].vert.y, minY);
		maxY = GET_MAX(subjecte->ve[i].vert.y, maxY);
	}

	minMaxPos->minX = minX;
	minMaxPos->maxX = maxX;
	minMaxPos->minY = minY;
	minMaxPos->maxY = maxY;

	if (minX == MIN_INITIAL_VAL || minY == MIN_INITIAL_VAL ||
		maxX == -1 || maxY == -1) {
		TRACE_ERROR("ERROR! invalid minX,maxX,minY,maxY");
		return 1;
	}
	return 0; //no-error
}
#endif

#ifdef MULTI_CLIPPING_RANGE //nsmoon@190702
#define TRACE_RNG(...)   //TRACE(__VA_ARGS__)
#define TRACE_MGCR(...)	//TRACE(__VA_ARGS__)
#else
#define TRACE_RNG(...)
#define TRACE_MGCR(...)
#endif

#define CLIP_SUBJ_AREA_RATIO		0.7f //0.8f
ATTR_BACKEND_RAMFUNC
static int clipping_multi_remained(axis_t axis, initial_polygon_t *initial_polygon, int multiLoopCnt)
{
    int	pdNo, subjIdx, clipIdx, slopeIdx = 0; //j, stEdLen, eflag
    float  touchAreaRese; //, touchAreaMin;
    float touchAreaSubj, clipAreaRatio;
	int clipAreaRatioFlag = 0; //disable virtual clipping
    int	maxPdNum, center;
    ep_buf_t *epa = 0;
    poly_e_t subjecte, clippere, *rese;
    sen_type_t sensor, sensor2;
    axis_t axis2;
    int ret;
    vec_e_t polyTmp[INITIAL_POLYGON_VERTEX_NO];
	int clipCnt, noClipCnt, skipDzLeft, skipDzRight;
#ifdef MULTI_SENSOR_RANGE_OLD //nsmoon@200119
	polygon_range_t *sensorRange = &BS_sensor_range_y[0];
#endif
	//sen_no_t centSen;
	int exitflagMinSen, exitflagMaxSen;
	int i;
#if (LARGE_PITCH_TBL == 1) //nsmoon@200108
	int largePitchCnt;
	int senStOp, senEdOp;
#endif
	ep_min_max_t minMaxEp2; //minMaxEp
	pos_min_max_t minMaxPos2; //minMaxPos
	int first;
    //int maxSlopeVal;

	IS_DEBUG_FLAG2{
		TRACE_NOP;
	};
	//centSen.x = GET_LINE_CENT_INT(minMaxEp->minX, minMaxEp->maxX);
	//centSen.y = GET_LINE_CENT_INT(minMaxEp->minY, minMaxEp->maxY);

	//get clipping info
    sensor = BS_clipping_range_info[(int)axis].sensor;
    sensor2 = BS_clipping_range_info[(int)axis].sensor2;

    if (axis == ENUM_HOR_X) {
		axis2 = ENUM_VER_Y;
        epa = EPA_ADDR_X;
        maxPdNum = BS_maxHorPdNum - 1;
        //maxSlopeVal = BS_slopeValMaxX;
		first = initial_polygon->firstX;
    }
    else {
		//(axis == ENUM_VER_Y)
		axis2 = ENUM_HOR_X;
        epa = EPA_ADDR_Y;
        maxPdNum = BS_maxVerPdNum - 1;
        //maxSlopeVal = BS_slopeValMaxY;
		first = initial_polygon->firstY;
    }
#if (DEBUG_clipping_multi_remained > 1)
    if (BS_pclipping_info.polygon_cnt > 0) {
		IS_DEBUG_FLAG2{ TRACE_CMR("clipping_multi_remained: [%d] %d", bd, BS_pclipping_info.polygon_cnt); };
    }
#endif
    //TRACE("epa in clipping_multi_remained(bd): %x (%d)", epa, bd);
	IS_DEBUG_FLAG{
		//if (axis == ENUM_VER_Y) 
		{
		TRACE_NOP;
		}
	};
#ifdef MULTI_CLIPPING_RANGE //nsmoon@200312
    int minSenlim = BS_clipping_range_info[(int)axis].minSenlim;
    int maxSenlim = BS_clipping_range_info[(int)axis].maxSenlim;
#if 0 //move to multi_get_clipping_info()
	int minSenlim, maxSenlim;
	minMaxEp.minX = initial_polygon->ep.minX;
	minMaxEp.maxX = initial_polygon->ep.maxX;
	minMaxEp.minY = initial_polygon->ep.minY;
	minMaxEp.maxY = initial_polygon->ep.maxY;
	minMaxPos.minX = initial_polygon->pos.minX;
	minMaxPos.maxX = initial_polygon->pos.maxX;
	minMaxPos.minY = initial_polygon->pos.minY;
	minMaxPos.maxY = initial_polygon->pos.maxY;
	//ret = multi_adj_initial_min_max(axis, &minMaxEp, &minMaxPos);
    BS_get_clipping_range(axis, &minMaxEp, &minMaxPos, &minSenlim, &maxSenlim);
	center = GET_LINE_CENT_INT(minSenlim, maxSenlim);
    IS_DEBUG_FLAG2{ TRACE_RNG("center,minSenlim,maxSenlim: %d (%d %d)", center, minSenlim, maxSenlim);};
#endif //0
    center = GET_LINE_CENT_INT(minSenlim, maxSenlim);
#ifdef MULTI_SEN_CNT_EVEN_SKIP_ENABLE //nsmoon@200330
    int senlimHalfWidth = GET_LINE_WIDTH_INT(maxSenlim, minSenlim);
    IS_DEBUG_FLAG2{ 
        TRACE_RNG("center,senlimHalfWidth,(minSenlim,maxSenlim): %d,%d(%d,%d)", center, senlimHalfWidth, minSenlim, maxSenlim);
        TRACE_NOP;
    };
#else
    IS_DEBUG_FLAG2{ TRACE_RNG("center,senlimHalfWidth,(minSenlim,maxSenlim): %d(%d,%d)", center, minSenlim, maxSenlim); };
#endif
#endif //MULTI_CLIPPING_RANGE
#if (LARGE_PITCH_TBL == 1) //nsmoon@200108
#if 0 //for test, FIXME
	if (initial_polygon->area > MAX_SIZESQ_ENABLE_VIRTUAL_EP) {
		TRACE("initial_polygon->area=%0.1f", initial_polygon->area);
		largePitchCnt = 0;
	}
	else 
#endif
	{
#if (MODEL_SPT == MODEL_KIO_550) || (MODEL_SPT == MODEL_KIO_430) //nsmoon@201222
        largePitchCnt = (multiLoopCnt == 1) ? 0 : 1;
#else
        if (multiLoopCnt == 1) {
            largePitchCnt = 0;
        }
        else {
            if (axis == ENUM_HOR_X) {
                if (sensor == ENUM_PD) {
                    if (initial_polygon->pos.maxY < BS_large_pitch_end_y) {
                        largePitchCnt = BS_large_pitch_tbl_x_pd_cnt;
                    }
                    else {
                        largePitchCnt = 0;
                    }
                }
                else {
                    if (initial_polygon->pos.minY > BS_large_pitch_zero_y) {
                        largePitchCnt = BS_large_pitch_tbl_x_led_cnt;
                    }
                    else {
                        largePitchCnt = 0;
                    }
                }
            }
            else {
                if (sensor == ENUM_PD) {
                    if (initial_polygon->pos.minX > BS_large_pitch_zero_x) {
                        largePitchCnt = BS_large_pitch_tbl_y_pd_cnt;
                    }
                    else {
                        largePitchCnt = 0;
                }
            }
                else {
                    if (initial_polygon->pos.maxX < BS_large_pitch_end_x) {
                        largePitchCnt = BS_large_pitch_tbl_y_led_cnt;
                    }
                    else {
                        largePitchCnt = 0;
                    }
                }
            }
        } //if (multiLoopCnt)
#endif //0
	}
#else //(LARGE_PITCH_TBL == 1)
	largePitchCnt = 0;
#endif

#ifdef REMOVE_SMALL_CLIPPING_LOOP //nsmoon@191216
	small_clipping_cnt = 0; small_pd_cnt = 0;
#endif
#if 1 //for debugging
    IS_DEBUG_FLAG2 {
		if (axis == ENUM_VER_Y) 
		{
        TRACE_NOP;
    }
    }
#endif

    //scan all sensors
	exitflagMinSen = exitflagMaxSen = 0;
    for (pdNo = 0; pdNo < maxPdNum; pdNo++)
    {
        //get scan order
        //slopeIdx = get_slope_order(axis, &centSen, pdNo);
        slopeIdx = get_slope_order(axis, center, pdNo); //nsmoon@200109
#if 0 //for test
		IS_DEBUG_FLAG2{
            if (axis == ENUM_HOR_X && slopeIdx == 93)
            //if (axis == ENUM_HOR_X && pdNo == 1)
            //if (axis == ENUM_VER_Y && slopeIdx == 94 /*&& BS_clipping_range_info[axis].polyIdx == 2*/)
			{
                //if (multiLoopCnt > 1)
                TRACE_NOP;
            }
        };
#endif
#if (DEAD_CELL_TBL == 1) //nsmoon@200512
        if (BS_is_dead_cell(axis, sensor, slopeIdx)) {
            continue;
        }
#endif
#ifdef MULTI_SEN_CNT_EVEN_SKIP_ENABLE //nsmoon@200330
#define MULTI_SEN_EVEN_SKIP_AREA    2
#define MULTI_SEN_EVEN_SKIP_WIDTH   5
//#define MULTI_DIST_NEAR_EDGE    50.0f //nsmoon@200928, for test
#ifdef MULTI_DIST_NEAR_EDGE
        int near_edge = 0;
        if ((axis == ENUM_HOR_X && (IS_NEAR_BLACK_AREA_Y(initial_polygon->pos.minY, MULTI_DIST_NEAR_EDGE) ||
            IS_NEAR_BLACK_AREA_Y(initial_polygon->pos.maxY, MULTI_DIST_NEAR_EDGE))) ||
            (axis == ENUM_VER_Y && (IS_NEAR_BLACK_AREA_X(initial_polygon->pos.minX, MULTI_DIST_NEAR_EDGE) ||
                IS_NEAR_BLACK_AREA_X(initial_polygon->pos.maxX, MULTI_DIST_NEAR_EDGE)))) {
            near_edge++;
        }
        if (senlimHalfWidth > MULTI_SEN_EVEN_SKIP_WIDTH && near_edge == 0) { //nsmoon@200928
#else
        if (senlimHalfWidth > MULTI_SEN_EVEN_SKIP_WIDTH) {
#endif
            int senDiff = GET_ABS(center - slopeIdx);
            if (senDiff > MULTI_SEN_EVEN_SKIP_AREA && senDiff < (senlimHalfWidth - MULTI_SEN_EVEN_SKIP_AREA)) {
                if ((slopeIdx % 2) == 0) {
                    continue;
                }
            }
        }
#endif
#ifdef MULTI_CLIPPING_RANGE //nsmoon@190702
		if (slopeIdx < minSenlim) {
			IS_DEBUG_FLAG2{
				TRACE_NOP;
			};
			exitflagMinSen++;
			if (exitflagMinSen > 0 && exitflagMaxSen > 0) {
				break;
			}
			continue;
		}
		else if (slopeIdx > maxSenlim) {
			IS_DEBUG_FLAG2{
				TRACE_NOP;
			};
			exitflagMaxSen++;
			if (exitflagMinSen > 0 && exitflagMaxSen > 0) {
				break;
			}
			continue;
			}
#endif
		if (epa[slopeIdx].len == 0) {
			IS_DEBUG_FLAG2{
		TRACE_NOP;
            };
#ifdef MULTI_SEN_CNT_EVEN_SKIP_ENABLE_2 //nsmoon@200330
            if (BS_multi_even_skip_en) {
                if ((slopeIdx % 2) == 0) continue;
            }
#endif
            //continue; //nsmoon@200108, do not continue!!
        }
		else {
			if (epa[slopeIdx].ep[0].s1 == 0 && epa[slopeIdx].ep[0].e1 == 0 &&
				epa[slopeIdx].ep[0].s2 == maxPdNum && epa[slopeIdx].ep[0].e2 == maxPdNum) {
				continue;
			}
		}
#ifdef REMOVE_SMALL_CLIPPING_LOOP //nsmoon@191216
		small_pd_cnt++;
#endif
#if 0 //for debugging
        IS_DEBUG_FLAG2{
            if (axis == ENUM_VER_Y && pdNo == 7 /*slopeIdx == 18*/)
                TRACE_NOP;
        }
#endif
        if (BS_pclipping_info.polygon_cnt > 0) {
            //swap clipping buffer
            clipping_var_swap();
            clipping_var_reset();
        }
        else {
            TRACE_ERROR("ERROR! BS_pclipping_info.polygon_cnt is zero-1 [%d]", BG_frame_no);
            break;
        }
		IS_DEBUG_FLAG2{ TRACENR_CMR(" %d/%d(%d,%d) ", slopeIdx, center, BS_pclipping_info.subject_cnt, epa[slopeIdx].len); };

		//save_used_epIdx_cnt = 0; //init, nsmoon@190715
        for (subjIdx = 0; subjIdx < BS_pclipping_info.subject_cnt; subjIdx++)
        {
            subjecte.len = BS_pclipping_info.subject_length[subjIdx];
            subjecte.ve = &(macT2P(BS_pclipping_info.subject_vertex, subjIdx, 0, BS_pclipping_info.subject_packs));
			touchAreaSubj = BS_getTouchArea(&subjecte); //get touch area of subject
  #if (DEBUG_clipping_multi_remained > 0)
            IS_DEBUG_FLAG2{ IS_CLRSCR{ TRACE_POLYGON_MULTI(0, 0x21, 0); } };  //individual or all
			IS_DEBUG_FLAG2{ DEBUG_SHOW_BS_INITIAL_POLYGON(); };
			IS_DEBUG_FLAG2{ DEBUG_SHOW_BS_SUBJECT_POLYGON(MY_COLOR - 1); };
			IS_DEBUG_FLAG2{ TRACE_POLYGON_MULTI(&subjecte, 0x11, 0); }
            TRACE_NOP;
  #endif
#ifdef MULTI_CLIPPING_RANGE //nsmoon@200108
			ret = get_subj_min_max(&subjecte, &minMaxPos2);
			if (ret) {
				continue;
			}
			if (axis == ENUM_HOR_X) {
				minMaxEp2.minX = (uint8_t)BS_getSenNumPos(axis, sensor, minMaxPos2.minX, 0); //0:before BUG!!! 4p-40_0114a.dlt-#156 
				minMaxEp2.maxX = (uint8_t)BS_getSenNumPos(axis, sensor, minMaxPos2.maxX, 1); //1:after
				minMaxEp2.minY = (uint8_t)BS_getSenNumPos(axis2, sensor2, minMaxPos2.minY, 0); //0:before
				minMaxEp2.maxY = (uint8_t)BS_getSenNumPos(axis2, sensor2, minMaxPos2.maxY, 1); //1:after
			}
			else {
				minMaxEp2.minX = (uint8_t)BS_getSenNumPos(axis2, sensor2, minMaxPos2.minX, 0); //0:before BUG!!! 4p-40_0114a.dlt-#156 
				minMaxEp2.maxX = (uint8_t)BS_getSenNumPos(axis2, sensor2, minMaxPos2.maxX, 1); //1:after
				minMaxEp2.minY = (uint8_t)BS_getSenNumPos(axis, sensor, minMaxPos2.minY, 0); //0:before
				minMaxEp2.maxY = (uint8_t)BS_getSenNumPos(axis, sensor, minMaxPos2.maxY, 1); //1:after
			}
#if 0 //for test
			IS_DEBUG_FLAG{
				TRACE_POLYGON_MULTI(0, 0x21, 0);
				DEBUG_SHOW_MIN_MAX(&minMaxPos2, 1, 0); //color, mode:1(show it at debug on)
				//if (axis == ENUM_VER_Y) 
				{
					TRACE_NOP;
				}
			};
#endif
            senStOp = BS_get_virtual_range(axis, sensor, &minMaxEp2, &minMaxPos2, slopeIdx, 0); //start
            senEdOp = BS_get_virtual_range(axis, sensor, &minMaxEp2, &minMaxPos2, slopeIdx, 1); //end
            //senStOp and senEdOp must be checked maxSlope before using
			if (senStOp == senEdOp) {
                //increase range by 1
				if (senEdOp < maxPdNum - 1) {
					senEdOp++;
				}
			}
#if (DEBUG_clipping_multi_remained > 0)
			IS_DEBUG_FLAG2{
				TRACE_MGCR("senStOp,senEdOp: %d %d", senStOp,senEdOp);
                #if 0
                //TRACE_POLYGON_MULTI(0, 0x21, 0);
                //DEBUG_SHOW_MIN_MAX(&minMaxPos2, 1, 0); //color, mode:1(show it at debug on)
                if (sensor == ENUM_PD) {
                    DEBUG_SHOW_LINE_PD_LED(axis, slopeIdx, senStOp, MY_COLOR -2);
                    DEBUG_SHOW_LINE_PD_LED(axis, slopeIdx, senEdOp, MY_COLOR - 2);
                }
                else {
                    DEBUG_SHOW_LINE_PD_LED(axis, senStOp, slopeIdx, MY_COLOR - 2);
                    DEBUG_SHOW_LINE_PD_LED(axis, senEdOp, slopeIdx, MY_COLOR - 2);
                }
                #endif //0
			    //if (senStOp > senEdOp) 
                            {
				TRACE_NOP;
			}
			};
#endif
#endif
			IS_DEBUG_FLAG2{
                //if (axis == ENUM_HOR_X && slopeIdx == 74 && BS_clipping_range_info[axis].polyIdx == 1)
				//if (axis == ENUM_VER_Y && slopeIdx == 94)
				{
					TRACE_NOP;
				}
			};

			maxClipIdx = GET_MAX(epa[slopeIdx].len, maxClipIdx); //nsmoon@190703
			skipDzLeft = skipDzRight = noClipCnt = clipCnt = 0;

            //decide clipping order
			int start, end, inc;
			if (first) {
				start = 0;
				end = epa[slopeIdx].len - 1;
				inc = +1;
			}
			else {
				start = epa[slopeIdx].len - 1;
				end = 0;
				inc = -1;
			}

			clipAreaRatioFlag = 1; //enable virtual clipping, do not remove

			for (clipIdx = start; (clipIdx * inc) <= end; clipIdx += inc)
            {
#ifdef MULTI_CLIPPING_RANGE //nsmoon@200108
				if (sensor == ENUM_PD) {
					if (epa[slopeIdx].ep[clipIdx].s1 > senStOp && 
						epa[slopeIdx].ep[clipIdx].s1 > senEdOp) {
						continue;
					}
					if (epa[slopeIdx].ep[clipIdx].s2 < senStOp && 
						epa[slopeIdx].ep[clipIdx].s2 < senEdOp) {
						continue;
					}
				}
				else {
					if (epa[slopeIdx].ep[clipIdx].e1 > senStOp && 
						epa[slopeIdx].ep[clipIdx].e1 > senEdOp) {
						continue;
					}
					if (epa[slopeIdx].ep[clipIdx].e2 < senStOp && 
						epa[slopeIdx].ep[clipIdx].e2 < senEdOp) {
						continue;
					}
				}
#endif
                //add padding of edge pattern
                addEdgePatternHalfPad(axis, epa, slopeIdx, clipIdx, &polyTmp[0], 0); //0:normal

                clippere.ve = &polyTmp[0];
                clippere.len = INITIAL_POLYGON_VERTEX_NO;
  #if (DEBUG_clipping_multi_remained > 0)
                //IS_DEBUG_FLAG2{ TRACE_POLYGON_MULTI(0, 0x21, MY_COLOR); }
                //IS_DEBUG_FLAG2{ DEBUG_show_BS_initial_polygon(); }
				IS_DEBUG_FLAG2{ TRACE_POLYGON_MULTI(&subjecte, 0x11, 0); };
				IS_DEBUG_FLAG2{ TRACE_POLYGON_MULTI(&clippere, 0x11, 1); };
                //IS_DEBUG_FLAG2{ TRACE("axis,sen_pos,slopeIdx,clipIdx: (%d-%d) %d %d %d %d", axis, BS_clipping_range_info[axis].polyIdx, pdNo, slopeIdx, subjIdx, clipIdx); }
  #endif
                rese = BS_poly_e_clip(&subjecte, &clippere);
  #if (DEBUG_clipping_multi_remained > 0)
				IS_DEBUG_FLAG2{
                    TRACE_POLYGON_MULTI(rese, 0x11, MY_COLOR);
                    IS_DEBUG_FLAG2{
                        TRACE_NOP;
					};
				};
  #endif
#if 0 //for debugging
                if (axis == ENUM_VER_Y && slopeIdx >= 94 /*&& BS_clipping_range_info[axis].polyIdx == 2*/)
                {
                IS_DEBUG_FLAG {
                        TRACE_NOP;
                        };
                }
#endif
				clipAreaRatioFlag = 1; //enable virtual clipping, do not remove
                    if (rese->len > 2) {
                        touchAreaRese = BS_getTouchArea(rese);
                        if (touchAreaRese > MULTI_MIN_TOUCH_AREA) {
							clipAreaRatio = touchAreaRese / touchAreaSubj; //nsmoon@200131
#if 1 // nsmoon@200310
                            //if (BS_pclipping_info.totalProcessedSlopeCnt == 2) 
                            {
                            if (multiLoopCnt < 0) {
                                //check eraser
                                //IS_DEBUG_FLAG2{ TRACE_RELEASE("clipAreaRatio=,%d,%d,%d,%d,%d,%0.2f,%0.1f,%0.1f,", axis, BS_clipping_range_info[(int)axis].sensor, BS_clipping_range_info[(int)axis].polyIdx, subjIdx, BS_pclipping_info.totalProcessedSlopeCnt, clipAreaRatio, touchAreaRese, touchAreaSubj); };
                                    if ((clipAreaRatio > TOUCH_PALM_CLIPAREA_RATIO && touchAreaRese > TOUCH_ERASER_SIZESQ_XXLARGE) || (BS_pclipping_info.eraser_pen)) {
                                    //clipping_var_save_result(rese, &subjecte, 1);
                                    BS_pclipping_info.subject_area[0] = touchAreaSubj;
                                    BS_pclipping_info.eraser_pen = 1; //eraser
                                    //IS_DEBUG_FLAG2{ TRACE_RELEASE("=>eraser"); };
                                }
                                #if 0 //for test, FIXME!!!
                                if ((clipAreaRatio < TOUCH_PEN_CLIPAREA_RATIO && touchAreaRese > TOUCH_PEN_SIZESQ_INITIAL) || (BS_pclipping_info.eraser_pen))  {
                                    TRACE_POLYGON_MULTI(rese, 0x11, MY_COLOR);
                                    BS_pclipping_info.eraser_pen = 2; //pen
                                }
                                #endif
                                else {
                                    BS_pclipping_info.eraser_pen = 0; //reset
                                }
                                goto L_clipping_multi_remained_eraser;
                            }
                            else {
                                    if ((clipAreaRatio > TOUCH_PALM_CLIPAREA_RATIO && touchAreaRese > TOUCH_ERASER_SIZESQ_XXLARGE) || (BS_pclipping_info.eraser_pen)) {
                                    clipping_var_save_result(rese, &subjecte, 1);
                                    //IS_DEBUG_FLAG2{ TRACE_RELEASE("=>eraser"); };
                                    goto L_clipping_multi_remained_eraser;
                                }
                            }
                        }
#endif
						if (clipAreaRatio > CLIP_SUBJ_AREA_RATIO) {
							clipAreaRatioFlag = 0; //disable virtual clipping
							}
							else {
								IS_DEBUG_FLAG{
								if (multiLoopCnt > 2) {
										TRACE_NOP;
									}
								};
							}
#ifdef CLIPPING_CHK_ADJ_VERT //nsmoon@190107
                            int skip = 0;
                            if (tmpAreaLimit < MULTI_LIMIT_SAVE_AREA) {
                                skip++; //not save
                            }
#endif
                            if (BS_pclipping_info.polygon_cnt < BS_pclipping_info.maxPolygonCnt) {
                                //save the result of clipping
								IS_DEBUG_FLAG2{ TRACENR_CMR("+%d(%d):%d ", BS_pclipping_info.polygon_cnt, subjIdx, BS_pclipping_info.polygon_state[BS_pclipping_info.polygon_cnt]); };
                                BS_pclipping_info.poly_idx_saved[BS_pclipping_info.polygon_cnt] = (int8_t)subjIdx;
                                BS_pclipping_info.polygon_area[BS_pclipping_info.polygon_cnt] = touchAreaRese;
                                //BS_pclipping_info.polygon_state[BS_pclipping_info.polygon_cnt] = BS_pclipping_info.subject_state[subjIdx];
#ifdef MULTI_SENSOR_RANGE_OLD //nsmoon@200119
                                if (center > slopeIdx) {
									//IS_DEBUG_FLAG2{ TRACE_RNG("sensorRange[].right: %d %d", BS_pclipping_info.polygon_cnt, sensorRange[BS_pclipping_info.polygon_cnt].right);};
                                    sensorRange[BS_pclipping_info.polygon_cnt].right = slopeIdx;
                                }
                                else if (center < slopeIdx) {
									//IS_DEBUG_FLAG2{ TRACE_RNG("sensorRange[].left: %d %d", BS_pclipping_info.polygon_cnt, sensorRange[BS_pclipping_info.polygon_cnt].left);};
                                    sensorRange[BS_pclipping_info.polygon_cnt].left = slopeIdx;
                                }
                                else {
                                    sensorRange[BS_pclipping_info.polygon_cnt].right = slopeIdx;
                                    sensorRange[BS_pclipping_info.polygon_cnt].left = slopeIdx;
                                }
								if (axis == ENUM_VER_Y) {
									//for saving x-range
									BS_pclipping_info.polygon_range[BS_pclipping_info.polygon_cnt] = BS_pclipping_info.subject_range[subjIdx];
									IS_DEBUG_FLAG2{ TRACE_RNG("sensorRange,BS_pclipping_info.polygon_range: %d(%d,%d %d,%d)", BS_pclipping_info.polygon_cnt, sensorRange[BS_pclipping_info.polygon_cnt].right, sensorRange[BS_pclipping_info.polygon_cnt].left, BS_pclipping_info.polygon_range[BS_pclipping_info.polygon_cnt].right, BS_pclipping_info.polygon_range[BS_pclipping_info.polygon_cnt].left); };
                                }
								else {
									IS_DEBUG_FLAG2{ TRACE_RNG("sensorRange: %d(%d,%d)", BS_pclipping_info.polygon_cnt, sensorRange[BS_pclipping_info.polygon_cnt].right, sensorRange[BS_pclipping_info.polygon_cnt].left); };
                                }
#if 1 //nsmoon@190703
								if (!is_save_used_epIdx((uint8_t)clipIdx)) {
								multi_save_used_ep(axis, sensor, (uint8_t)slopeIdx, (uint8_t)clipIdx);
								}
#else
                                multi_save_used_ep(axis, sensor, &epa[slopeIdx].ep[clipIdx]);
#endif
#endif //0
#ifdef CLIPPING_CHK_ADJ_VERT //nsmoon@190107
                                clipping_var_save_result(rese, &subjecte, skip);
#else
                                clipping_var_save_result(rese, &subjecte, 0);
#endif
#ifdef REMOVE_SMALL_CLIPPING_LOOP //nsmoon@191216
								int s1 = epa[slopeIdx].ep[clipIdx].s1;
								int s2 = epa[slopeIdx].ep[clipIdx].s2;
								int e1 = epa[slopeIdx].ep[clipIdx].e1;
								int e2 = epa[slopeIdx].ep[clipIdx].e2;
								if ((s1 == e1 && s1 == 0) || (s2 == e2 && s2 == maxPdNum)) {
									TRACE_NOP;
								}
								else {
								small_clipping_cnt++;
								}
#endif
                                clipCnt++;
                            }
                            else {
                                TRACE_ERROR_MEM("ERROR_MEM! 1 exceed maxPolygonCnt %d %d %d %d [%d]", BS_pclipping_info.polygon_cnt, BS_pclipping_info.maxPolygonCnt, BS_pclipping_info.polygon_packs, BS_pclipping_info.totalProcessedSlopeCnt, BG_frame_no);
                                BS_pclipping_info.maxPolygonErrCnt++;
                                return -1; //mem-error, nsmoon@191212
                            } //if (BS_pclipping_info.polygon_cnt < maxPolygonCnt)
                        }
                        else {
							//small size
                            BS_pclipping_info.minTouchAreaErrCnt++;
                        } //if (touchAreaRes > BS_minTouchArea)
                    }
                    else {
						//no-clipping
                        BS_pclipping_info.noClippingErrCnt++;
#ifdef MULTI_SKIP_NO_CLIPPING //nsmoon@190715
						if (clipCnt > 0) {
							noClipCnt++;
							//break;
                    }
#endif
                } //if (rese->len > 2)
            } //for (clipIdx = start; (clipIdx * inc) <= end; clipIdx += inc)
#if (DEBUG_clipping_multi_remained > 0)
			IS_DEBUG_FLAG2{
                if (axis == ENUM_VER_Y && slopeIdx == 94 /*&& BS_clipping_range_info[axis].polyIdx == 2*/)
				//if (axis == ENUM_VER_Y && slopeIdx == 94)
                {
					TRACE_NOP;
            }
			};
#endif

#if (LARGE_PITCH_TBL == 1) //nsmoon@200108
            if (multiLoopCnt == 3) {
                largePitchCnt = 1;
                clipAreaRatioFlag = 1;
            }
			if (largePitchCnt > 0 && clipAreaRatioFlag == 1) {
                BG_edge_pattern_virtual_cnt = make_edge_pattern_virtual(axis, sensor, epa, slopeIdx, senStOp, senEdOp, multiLoopCnt);
				for (i = 0; i < BG_edge_pattern_virtual_cnt; i++)
				{
					clippere.ve = &BS_edge_pattern_virtul_poly[i][0];
                    clippere.len = INITIAL_POLYGON_VERTEX_NO;
#if (DEBUG_clipping_multi_remained > 0)
					//IS_DEBUG_FLAG{ IS_CLRSCR{ TRACE_POLYGON_MULTI(0, 0x21, MY_COLOR); } }
					IS_DEBUG_FLAG2{ TRACE_POLYGON_MULTI(&subjecte, 0x11, 0); };
					IS_DEBUG_FLAG2{ TRACE_POLYGON_MULTI(&clippere, 0x11, 2); };
#endif
					rese = BS_poly_e_clip(&subjecte, &clippere);
#if (DEBUG_clipping_multi_remained > 0)
					IS_DEBUG_FLAG2{
						TRACE_POLYGON_MULTI(rese, 0x11, MY_COLOR);
					TRACE_NOP;
					};
#endif
					if (rese->len > 2) {
						touchAreaRese = BS_getTouchArea(rese);
						if (touchAreaRese > MULTI_MIN_TOUCH_AREA) {
							if (BS_pclipping_info.polygon_cnt < BS_pclipping_info.maxPolygonCnt) {
								IS_DEBUG_FLAG2{ TRACENR_CMR("*%d:%d ", subjIdx, BS_pclipping_info.subject_state[subjIdx]); };
								BS_pclipping_info.poly_idx_saved[BS_pclipping_info.polygon_cnt] = (int8_t)subjIdx;
								BS_pclipping_info.polygon_area[BS_pclipping_info.polygon_cnt] = touchAreaRese; //nsmoon@170521a;
								clipping_var_save_result(rese, rese, 0);
								clipCnt++;
							}
							else {
								TRACE_ERROR_MEM("ERROR_MEM! 2 exceed maxPolygonCnt %d %d %d %d [%d]", BS_pclipping_info.polygon_cnt, BS_pclipping_info.maxPolygonCnt, BS_pclipping_info.polygon_packs, BS_pclipping_info.totalProcessedSlopeCnt, BG_frame_no);
								BS_pclipping_info.maxPolygonErrCnt++;
							} //if (BS_pclipping_info.polygon_cnt < maxPolygonCnt)
						}
					}
				} //for (i = 0; i < BG_edge_pattern_virtual_cnt; i++)
			} //if (largePitchCnt > 0)
#endif

#ifdef MULTI_SKIP_NO_CLIPPING //nsmoon@190715
			if (noClipCnt > 0) {
				if (first) {
					skipDzLeft++;
				}
				else {
					skipDzRight++;
				}
			}
#endif

#if 1 //check left deadzone (max sensor number side)
			if (skipDzLeft == 0) {
                    if (epa[slopeIdx].len > 0) {
                        ret = make_edge_pattern_dz(axis, &epa[slopeIdx], DEAD_ZONE_LEFT, senStOp, senEdOp);
                    }
                    else {
						IS_DEBUG_FLAG2{
							TRACE_NOP;
						}
                        ret = make_edge_pattern_empty_dz(axis, slopeIdx, DEAD_ZONE_LEFT, senStOp, senEdOp);
						IS_DEBUG_FLAG2{ TRACENR_CMR("#"); };
                    }
                    if (ret < 0) {
                        //not enough memory
                        return -1; //nsmoon@190325
                    }
                    if (ret) {
                        addEdgePatternHalfPad(axis, &BG_edge_pattern_dz, -1, DEAD_ZONE_LEFT, &polyTmp[0], 0); //0:normal
                        clippere.ve = &polyTmp[0];
                        clippere.len = INITIAL_POLYGON_VERTEX_NO;
#if (DEBUG_clipping_multi_remained > 0)
                        //IS_DEBUG_FLAG2{ IS_CLRSCR{ TRACE_POLYGON_MULTI(0, 0x21, MY_COLOR); } }
				IS_DEBUG_FLAG2{ TRACE_POLYGON_MULTI(&subjecte, 0x11, 0); };
				IS_DEBUG_FLAG2{ TRACE_POLYGON_MULTI(&clippere, 0x11, 1); };
#endif
                        rese = BS_poly_e_clip(&subjecte, &clippere);
#if (DEBUG_clipping_multi_remained > 0)
				IS_DEBUG_FLAG2{
                            TRACE_POLYGON_MULTI(rese, 0x11, MY_COLOR);
                                TRACE_NOP;
							};
#endif
                            if (rese->len > 2) {
                                touchAreaRese = BS_getTouchArea(rese);
                                if (touchAreaRese > MULTI_MIN_TOUCH_AREA) {
                                    if (BS_pclipping_info.polygon_cnt < BS_pclipping_info.maxPolygonCnt) {
										IS_DEBUG_FLAG2{ TRACENR_CMR("*%d:%d ", subjIdx, BS_pclipping_info.subject_state[subjIdx]); };
                                        BS_pclipping_info.poly_idx_saved[BS_pclipping_info.polygon_cnt] = (int8_t)subjIdx;
                                        BS_pclipping_info.polygon_area[BS_pclipping_info.polygon_cnt] = touchAreaRese; //nsmoon@170521a;
                                        clipping_var_save_result(rese, rese, 0);
                                        clipCnt++;
                                    }
                                    else {
                                        TRACE_ERROR_MEM("ERROR_MEM! 2 exceed maxPolygonCnt %d %d %d %d [%d]", BS_pclipping_info.polygon_cnt, BS_pclipping_info.maxPolygonCnt, BS_pclipping_info.polygon_packs, BS_pclipping_info.totalProcessedSlopeCnt, BG_frame_no);
                                        BS_pclipping_info.maxPolygonErrCnt++;
                                    } //if (BS_pclipping_info.polygon_cnt < maxPolygonCnt)
                                }
								else {
									TRACE_NOP;
								}
                            }
                    }
			} //if (skipDzLeft) 
#endif

#if 1 //check right deadzone (min sensor number side)
			if (skipDzRight == 0) {
                    if (epa[slopeIdx].len > 0) {
                        ret = make_edge_pattern_dz(axis, &epa[slopeIdx], DEAD_ZONE_RIGHT, senStOp, senEdOp);
                    }
                    else {
                        ret = make_edge_pattern_empty_dz(axis, slopeIdx, DEAD_ZONE_RIGHT, senStOp, senEdOp);
						IS_DEBUG_FLAG2{ TRACENR_CMR("#"); };
                    }
                    if (ret < 0) {
                        //not enough memory
                        return -1; //nsmoon@190325
                    }
                    if (ret) {
                        addEdgePatternHalfPad(axis, &BG_edge_pattern_dz, -1, DEAD_ZONE_RIGHT, &polyTmp[0], 0); //0:normal
                        clippere.ve = &polyTmp[0];
                        clippere.len = INITIAL_POLYGON_VERTEX_NO;
#if (DEBUG_clipping_multi_remained > 0)
                        //IS_DEBUG_FLAG2{ IS_CLRSCR{ TRACE_POLYGON_MULTI(0, 0x21, MY_COLOR); } }
				IS_DEBUG_FLAG2{ TRACE_POLYGON_MULTI(&subjecte, 0x11, 0); };
				IS_DEBUG_FLAG2{ TRACE_POLYGON_MULTI(&clippere, 0x11, 1); };
#endif
                        rese = BS_poly_e_clip(&subjecte, &clippere);
#if (DEBUG_clipping_multi_remained > 0)
				IS_DEBUG_FLAG2{
                            TRACE_POLYGON_MULTI(rese, 0x11, MY_COLOR);
                                TRACE_NOP;
							};
#endif
                            if (rese->len > 2) {
                                touchAreaRese = BS_getTouchArea(rese);
                                if (touchAreaRese > MULTI_MIN_TOUCH_AREA) {
                                    if (BS_pclipping_info.polygon_cnt < BS_pclipping_info.maxPolygonCnt) {
										IS_DEBUG_FLAG2{ TRACENR_CMR("*%d:%d ", subjIdx, BS_pclipping_info.subject_state[subjIdx]); };
                                        BS_pclipping_info.poly_idx_saved[BS_pclipping_info.polygon_cnt] = (int8_t)subjIdx;
                                        BS_pclipping_info.polygon_area[BS_pclipping_info.polygon_cnt] = touchAreaRese; //nsmoon@170521a;
                                        clipping_var_save_result(rese, rese, 0);
                                        clipCnt++;
                                    }
                                    else {
                                        TRACE_ERROR_MEM("ERROR_MEM! 3 exceed maxPolygonCnt %d %d %d %d [%d]", BS_pclipping_info.polygon_cnt, BS_pclipping_info.maxPolygonCnt, BS_pclipping_info.polygon_packs, BS_pclipping_info.totalProcessedSlopeCnt, BG_frame_no);
                                        BS_pclipping_info.maxPolygonErrCnt++;
                                    } //if (BS_pclipping_info.polygon_cnt < maxPolygonCnt)
                                }
								else {
									TRACE_NOP;
								}
                            }
                    }
			} //if (skipDzRight)
#endif
			IS_DEBUG_FLAG2{
				//if (axis == ENUM_VER_Y && slopeIdx == 35)
                //if (multiLoopCnt == 3)
				{
                    TRACE_NOP;
                }
			};
        } //for (subjIdx = 0; subjIdx < BS_pclipping_info.subject_cnt; subjIdx++)
		IS_DEBUG_FLAG2{
			//if (axis == ENUM_VER_Y && slopeIdx == 35)
            //if (multiLoopCnt == 3)
            TRACE_NOP;
        };
        if (BS_pclipping_info.polygon_cnt < 1)
        {
			IS_DEBUG_FLAG2{
				//if (axis == ENUM_VER_Y) 
				{
				    TRACE_NOP;
				}
			};
            //should be ghost, removed
            break;
        }
    } //for (pdNo = 0; pdNo < maxPdNum; pdNo++)
#ifdef MULTI_SENSOR_RANGE_OLD //nsmoon@200119
#if 0 //for test
	for (i = 0; i < BS_pclipping_info.polygon_cnt; i++) {
		//TRACE("right,left:(%d)%d,%d", i, BS_pclipping_info.polygon_range[i].right, BS_pclipping_info.polygon_range[i].left);
		if (sensorRange[i].right > sensorRange[i].left) {
			TRACE_ERROR("ERROR! invalid BS_pclipping_info.polygon_range: (%d)%d,%d", i, BS_pclipping_info.polygon_range[i].right, BS_pclipping_info.polygon_range[i].left);
		}
		//TRACE_NOP;
	}
#endif
	for (i = 0; i < BS_pclipping_info.polygon_cnt; i++) {
#if (DEBUG_clipping_multi_remained > 0)
		IS_DEBUG_FLAG2{ TRACE_CMR("%d-%d %d %d", axis, i, sensorRange[i].right, sensorRange[i].left); };
#endif
		if (axis == ENUM_HOR_X) {
			//save sensorRange for x-axis
			BS_pclipping_info.polygon_range[i].right = sensorRange[i].right;
			BS_pclipping_info.polygon_range[i].left = sensorRange[i].left;
		}
#if (DEBUG_clipping_multi_remained > 1)
		else {
			IS_DEBUG_FLAG{ TRACE_CMR("^X^ %d %d %d", j, BS_pclipping_info.polygon_range[i].right, BS_pclipping_info.polygon_range[i].left); };
		}
#endif
	}
#endif //0
#ifdef REMOVE_SMALL_CLIPPING_LOOP //nsmoon@191216
	if (small_clipping_cnt < 2) {
	TRACE_RSCL("small_pd_cnt: %d,%d (%d/%d)", small_pd_cnt, small_clipping_cnt, axis, BS_pclipping_info.polygon_cnt);
		BS_pclipping_info.polygon_cnt = 0;
	}
#endif

	IS_DEBUG_FLAG2 {
        TRACE_NOP;
	};
    return BS_pclipping_info.polygon_cnt;

L_clipping_multi_remained_eraser:
    return 1;
}

#if 0 //nsmoon@200217
static int BS_multi_is_tmp_used_ep(axis_t axis, sen_type_t sen, uint8_t slopeIdx, uint8_t epIdx, int start, int end)
{
    int i;
    for (i = start; i < end; i++) {
        if (BS_multi_tmp_used_ep[i].axis == axis &&
            BS_multi_tmp_used_ep[i].sen == sen &&
            BS_multi_tmp_used_ep[i].slopeIdx == slopeIdx &&
            BS_multi_tmp_used_ep[i].epIdx == epIdx) {
            return 1; //found
        }
    }
    return 0; //not-found
}

static int BS_multi_save_tmp_used_ep(axis_t axis, sen_type_t sen, uint8_t slopeIdx, uint8_t epIdx)
{
    int cnt = BS_multi_tmp_used_ep_cnt;
    if (BS_multi_tmp_used_ep_cnt < MAX_TMP_USED_EP) {
        BS_multi_tmp_used_ep[cnt].axis = axis;
        BS_multi_tmp_used_ep[cnt].sen = sen;
        BS_multi_tmp_used_ep[cnt].slopeIdx = slopeIdx;
        BS_multi_tmp_used_ep[cnt].epIdx = epIdx;
        cnt++;
    }
    else {
        TRACE_ERROR_MEM("ERROR_MEM! multi_save_used_ep..invalid BS_multi_tmp_used_ep_cnt: %d", BS_multi_tmp_used_ep_cnt);
        return 1; //error
    }

    BS_multi_tmp_used_ep_cnt = cnt;
    return 0;
}

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_clipping_multi_remained_small	0 //1:enable
#endif
ATTR_BACKEND_RAMFUNC
static int clipping_multi_remained_small(axis_t axis, initial_polygon_t *initial_polygon, int unusedEpStart, int unusedEpEnd)
{
    int	pdNo, subjIdx, clipIdx, slopeIdx = 0; //j, stEdLen, eflag
    float  touchAreaRese; //, touchAreaMin;
    float touchAreaSubj, clipAreaRatio;
    //int clipAreaRatioFlag = 0; //disable virtual clipping
    int	maxPdNum, center;
    ep_buf_t *epa = 0;
    poly_e_t subjecte, clippere, *rese;
    sen_type_t sensor, sensor2;
    axis_t axis2;
    int ret;
    vec_e_t polyTmp[INITIAL_POLYGON_VERTEX_NO];
    int clipCnt, noClipCnt, skipDzLeft, skipDzRight;
    int clipCntTot = 0;
    //int maxSlopeVal;
    //int noClip;
#ifdef MULTI_SENSOR_RANGE_OLD //nsmoon@200119
    polygon_range_t *sensorRange = &BS_sensor_range_y[0];
#endif
    //sen_no_t centSen;
    int exitflagMinSen, exitflagMaxSen;
#if (LARGE_PITCH_TBL == 1) //nsmoon@200108
    int senStOp, senEdOp;
#endif
    ep_min_max_t minMaxEp, minMaxEp2;
    pos_min_max_t minMaxPos, minMaxPos2;
    int first;
    //int k;

    IS_DEBUG_FLAG2{
        TRACE_NOP;
    };
    //centSen.x = GET_LINE_CENT_INT(minMaxEp->minX, minMaxEp->maxX);
    //centSen.y = GET_LINE_CENT_INT(minMaxEp->minY, minMaxEp->maxY);

    //get clipping info
    sensor = BS_clipping_range_info[(int)axis].sensor;
    sensor2 = BS_clipping_range_info[(int)axis].sensor2;

    if (axis == ENUM_HOR_X) {
        axis2 = ENUM_VER_Y;
        epa = EPA_ADDR_X;
        maxPdNum = BS_maxHorPdNum - 1;
        //maxSlopeVal = BS_slopeValMaxX;
        first = initial_polygon->firstX;
    }
    else {
        //(axis == ENUM_VER_Y)
        axis2 = ENUM_HOR_X;
        epa = EPA_ADDR_Y;
        maxPdNum = BS_maxVerPdNum - 1;
        //maxSlopeVal = BS_slopeValMaxY;
        first = initial_polygon->firstY;
    }
#if (DEBUG_clipping_multi_remained_small > 1)
    if (BS_pclipping_info.polygon_cnt > 0) {
        IS_DEBUG_FLAG2{ TRACE_CMR("clipping_multi_remained: [%d] %d", bd, BS_pclipping_info.polygon_cnt); };
    }
#endif
    //TRACE("epa in clipping_multi_remained(bd): %x (%d)", epa, bd);
    IS_DEBUG_FLAG{
        //if (axis == ENUM_VER_Y) 
        {
            TRACE_NOP;
        }
    };
#ifdef MULTI_CLIPPING_RANGE //nsmoon@190702
    int minSenlim, maxSenlim;
    minMaxEp.minX = initial_polygon->ep.minX;
    minMaxEp.maxX = initial_polygon->ep.maxX;
    minMaxEp.minY = initial_polygon->ep.minY;
    minMaxEp.maxY = initial_polygon->ep.maxY;
    minMaxPos.minX = initial_polygon->pos.minX;
    minMaxPos.maxX = initial_polygon->pos.maxX;
    minMaxPos.minY = initial_polygon->pos.minY;
    minMaxPos.maxY = initial_polygon->pos.maxY;
    //ret = multi_adj_initial_min_max(axis, &minMaxEp, &minMaxPos);
    BS_get_clipping_range(axis, &minMaxEp, &minMaxPos, &minSenlim, &maxSenlim);
    center = GET_LINE_CENT_INT(minSenlim, maxSenlim);
    IS_DEBUG_FLAG2{ TRACE_RNG("center,minSenlim,maxSenlim: %d (%d %d)", center, minSenlim, maxSenlim); };
#endif

#ifdef REMOVE_SMALL_CLIPPING_LOOP //nsmoon@191216
    small_clipping_cnt = 0; small_pd_cnt = 0;
#endif
#if 1 //for debugging
    IS_DEBUG_FLAG2{
        if (axis == ENUM_VER_Y)
        {
            TRACE_NOP;
        }
    }
#endif

    //scan all sensors
    exitflagMinSen = exitflagMaxSen = 0;
    for (pdNo = 0; pdNo <= maxPdNum; pdNo++)
    {
        //get scan order
        //slopeIdx = get_slope_order(axis, &centSen, pdNo);
        slopeIdx = get_slope_order(axis, center, pdNo); //nsmoon@200109
        //slopeIdx = pdNo;
        IS_DEBUG_FLAG2{
            //if (axis == ENUM_HOR_X && slopeIdx == 130) 
            //if (axis == ENUM_VER_Y && slopeIdx == 94 /*&& BS_clipping_range_info[axis].polyIdx == 2*/)
            {
                TRACE_NOP;
            }
        };
#ifdef MULTI_CLIPPING_RANGE //nsmoon@190702
        if (slopeIdx < minSenlim) {
            IS_DEBUG_FLAG2{
                TRACE_NOP;
            };
            exitflagMinSen++;
            if (exitflagMinSen > 0 && exitflagMaxSen > 0) {
                break;
            }
            continue;
        }
        else if (slopeIdx > maxSenlim) {
            IS_DEBUG_FLAG2{
                TRACE_NOP;
            };
            exitflagMaxSen++;
            if (exitflagMinSen > 0 && exitflagMaxSen > 0) {
                break;
            }
            continue;
        }
#endif
        if (epa[slopeIdx].len == 0) {
            continue; //different from normal clipping
        }
        else {
            if (epa[slopeIdx].ep[0].s1 == 0 && epa[slopeIdx].ep[0].e1 == 0 &&
                epa[slopeIdx].ep[0].s2 == maxPdNum && epa[slopeIdx].ep[0].e2 == maxPdNum) {
                continue;
            }
        }
#ifdef REMOVE_SMALL_CLIPPING_LOOP //nsmoon@191216
        small_pd_cnt++;
#endif
#if 0 //for debugging
        IS_DEBUG_FLAG2{
            if (axis == ENUM_VER_Y && pdNo == 7 /*slopeIdx == 18*/)
            TRACE_NOP;
        }
#endif
            if (BS_pclipping_info.polygon_cnt > 0) {
                //swap clipping buffer
                clipping_var_swap();
                clipping_var_reset();
            }
            else {
                TRACE_ERROR("ERROR! BS_pclipping_info.polygon_cnt is zero-2 [%d]", BG_frame_no);
                break;
            }
            IS_DEBUG_FLAG2{ TRACENR_CMR(" %d/%d(%d,%d) ", slopeIdx, center, BS_pclipping_info.subject_cnt, epa[slopeIdx].len); };

            //save_used_epIdx_cnt = 0; //init, nsmoon@190715
            for (subjIdx = 0; subjIdx < BS_pclipping_info.subject_cnt; subjIdx++)
            {
                subjecte.len = BS_pclipping_info.subject_length[subjIdx];
                subjecte.ve = &(macT2P(BS_pclipping_info.subject_vertex, subjIdx, 0, BS_pclipping_info.subject_packs));
                touchAreaSubj = BS_getTouchArea(&subjecte); //get touch area of subject
#if (DEBUG_clipping_multi_remained_small > 0)
                IS_DEBUG_FLAG2{ IS_CLRSCR{ TRACE_POLYGON_MULTI(0, 0x21, 0); } };  //individual or all
                IS_DEBUG_FLAG2{ DEBUG_SHOW_BS_INITIAL_POLYGON(); };
                IS_DEBUG_FLAG2{ DEBUG_SHOW_BS_SUBJECT_POLYGON(MY_COLOR - 1); };
                IS_DEBUG_FLAG2{ TRACE_POLYGON_MULTI(&subjecte, 0x11, 0); }
                TRACE_NOP;
#endif
#ifdef MULTI_CLIPPING_RANGE //nsmoon@200108
                ret = get_subj_min_max(&subjecte, &minMaxPos2);
                if (ret) {
                    continue;
                }
                if (axis == ENUM_HOR_X) {
                    minMaxEp2.minX = BS_getSenNumPos(axis, sensor, minMaxPos2.minX, 0); //0:before BUG!!! 4p-40_0114a.dlt-#156 
                    minMaxEp2.maxX = BS_getSenNumPos(axis, sensor, minMaxPos2.maxX, 1); //1:after
                    minMaxEp2.minY = BS_getSenNumPos(axis2, sensor2, minMaxPos2.minY, 0); //0:before
                    minMaxEp2.maxY = BS_getSenNumPos(axis2, sensor2, minMaxPos2.maxY, 1); //1:after
                }
                else {
                    minMaxEp2.minX = BS_getSenNumPos(axis2, sensor2, minMaxPos2.minX, 0); //0:before BUG!!! 4p-40_0114a.dlt-#156 
                    minMaxEp2.maxX = BS_getSenNumPos(axis2, sensor2, minMaxPos2.maxX, 1); //1:after
                    minMaxEp2.minY = BS_getSenNumPos(axis, sensor, minMaxPos2.minY, 0); //0:before
                    minMaxEp2.maxY = BS_getSenNumPos(axis, sensor, minMaxPos2.maxY, 1); //1:after
                }
#if 0 //for test
                IS_DEBUG_FLAG{
                    TRACE_POLYGON_MULTI(0, 0x21, 0);
                DEBUG_SHOW_MIN_MAX(&minMaxPos2, 1, 0); //color, mode:1(show it at debug on)
                                                       //if (axis == ENUM_VER_Y) 
                {
                    TRACE_NOP;
                }
                };
#endif
                senStOp = BS_get_virtual_range(axis, sensor, &minMaxEp2, &minMaxPos2, slopeIdx, 0); //start
                senEdOp = BS_get_virtual_range(axis, sensor, &minMaxEp2, &minMaxPos2, slopeIdx, 1); //end
                                                                                                    //senStOp and senEdOp must be checked maxSlope before using
                if (senStOp == senEdOp) {
                    //increase range by 1
                    if (senEdOp < maxPdNum - 1) {
                        senEdOp++;
                    }
                }
#if (DEBUG_clipping_multi_remained > 0)
                IS_DEBUG_FLAG2{
                    TRACE_MGCR("senStOp,senEdOp: %d %d", senStOp,senEdOp);
                    #if 0 
                if (sensor == ENUM_PD) {
                    DEBUG_SHOW_LINE_PD_LED(axis, slopeIdx, senStOp, MY_COLOR - 2);
                    DEBUG_SHOW_LINE_PD_LED(axis, slopeIdx, senEdOp, MY_COLOR - 2);
                }
                else {
                    DEBUG_SHOW_LINE_PD_LED(axis, senStOp, slopeIdx, MY_COLOR - 2);
                    DEBUG_SHOW_LINE_PD_LED(axis, senEdOp, slopeIdx, MY_COLOR - 2);
                }
                #endif //0
                //if (senStOp > senEdOp) 
                {
                    TRACE_NOP;
                }
                };
#endif
#endif
                IS_DEBUG_FLAG2{
                    //if (axis == ENUM_HOR_X && slopeIdx == 74 && BS_clipping_range_info[axis].polyIdx == 1)
                    //if (axis == ENUM_VER_Y && slopeIdx == 94)
                    {
                        TRACE_NOP;
                    }
                };

                maxClipIdx = GET_MAX(epa[slopeIdx].len, maxClipIdx); //nsmoon@190703
                skipDzLeft = skipDzRight = noClipCnt = clipCnt = 0;

                //decide clipping order
                int start, end, inc;
                if (first) {
                    start = 0;
                    end = epa[slopeIdx].len - 1;
                    inc = +1;
                }
                else {
                    start = epa[slopeIdx].len - 1;
                    end = 0;
                    inc = -1;
                }

                //clipAreaRatioFlag = 1; //enable virtual clipping, do not remove
                //noClip = -1; //init
                for (clipIdx = start; (clipIdx * inc) <= end; clipIdx += inc)
                {
                    ret = BS_multi_is_tmp_used_ep(axis, sensor, slopeIdx, clipIdx, unusedEpStart, unusedEpEnd);
                    if (ret) continue;
#ifdef MULTI_CLIPPING_RANGE //nsmoon@200108
                    if (sensor == ENUM_PD) {
                        if (epa[slopeIdx].ep[clipIdx].s1 > senStOp &&
                            epa[slopeIdx].ep[clipIdx].s1 > senEdOp) {
                            continue;
                        }
                        if (epa[slopeIdx].ep[clipIdx].s2 < senStOp &&
                            epa[slopeIdx].ep[clipIdx].s2 < senEdOp) {
                            continue;
                        }
                    }
                    else {
                        if (epa[slopeIdx].ep[clipIdx].e1 > senStOp &&
                            epa[slopeIdx].ep[clipIdx].e1 > senEdOp) {
                            continue;
                        }
                        if (epa[slopeIdx].ep[clipIdx].e2 < senStOp &&
                            epa[slopeIdx].ep[clipIdx].e2 < senEdOp) {
                            continue;
                        }
                    }
#endif
                    //noClip = 0; //reset
                    //add padding of edge pattern
                    addEdgePatternHalfPad(axis, epa, slopeIdx, clipIdx, &polyTmp[0], 1); //1:small padding

                    clippere.ve = &polyTmp[0];
                    clippere.len = INITIAL_POLYGON_VERTEX_NO;
#if (DEBUG_clipping_multi_remained_small > 0)
                    //IS_DEBUG_FLAG2{ TRACE_POLYGON_MULTI(0, 0x21, MY_COLOR); }
                    //IS_DEBUG_FLAG2{ DEBUG_show_BS_initial_polygon(); }
                    IS_DEBUG_FLAG2{ TRACE_POLYGON_MULTI(&subjecte, 0x11, 0); };
                    IS_DEBUG_FLAG2{ TRACE_POLYGON_MULTI(&clippere, 0x11, 1); };
                    //IS_DEBUG_FLAG2{ TRACE("axis,sen_pos,slopeIdx,clipIdx: %d %d %d %d", axis, sen_pos, slopeIdx,clipIdx); }
#endif
                    rese = BS_poly_e_clip(&subjecte, &clippere);
#if (DEBUG_clipping_multi_remained_small > 0)
                    IS_DEBUG_FLAG2{
                        TRACE_POLYGON_MULTI(rese, 0x11, MY_COLOR);
                    TRACE_NOP;
                    };
#endif
#if 1 //for debugging
                    //if (axis == ENUM_VER_Y && slopeIdx >= 94 /*&& BS_clipping_range_info[axis].polyIdx == 2*/)
                    {
                        IS_DEBUG_FLAG{
                            TRACE_NOP;
                        };
                    }
#endif
                    //clipAreaRatioFlag = 1; //enable virtual clipping, do not remove
                    if (rese->len > 2) {
                        touchAreaRese = BS_getTouchArea(rese);
                            clipAreaRatio = touchAreaRese / touchAreaSubj; //nsmoon@200131
                                IS_DEBUG_FLAG{
                            //TRACE("clipAreaRatio=%0.3f", clipAreaRatio);
                                    TRACE_NOP;
                                };
                        if (touchAreaRese > MULTI_MIN_TOUCH_AREA2 && clipAreaRatio > MULTI_MIN_TOUCH_AREA_RATIO) {
#ifdef CLIPPING_CHK_ADJ_VERT //nsmoon@190107
                            int skip = 0;
                            if (tmpAreaLimit < MULTI_LIMIT_SAVE_AREA) {
                                skip++; //not save
                            }
#endif
                            if (BS_pclipping_info.polygon_cnt < BS_pclipping_info.maxPolygonCnt) {
                                //save the result of clipping
                                IS_DEBUG_FLAG2{ TRACENR_CMR("+%d(%d):%d ", BS_pclipping_info.polygon_cnt, subjIdx, BS_pclipping_info.polygon_state[BS_pclipping_info.polygon_cnt]); };
                                BS_pclipping_info.poly_idx_saved[BS_pclipping_info.polygon_cnt] = subjIdx;
                                BS_pclipping_info.polygon_area[BS_pclipping_info.polygon_cnt] = touchAreaRese;
                                //BS_pclipping_info.polygon_state[BS_pclipping_info.polygon_cnt] = BS_pclipping_info.subject_state[subjIdx];
#ifdef MULTI_SENSOR_RANGE_OLD //nsmoon@200119
                                if (center > slopeIdx) {
                                    //IS_DEBUG_FLAG2{ TRACE_RNG("sensorRange[].right: %d %d", BS_pclipping_info.polygon_cnt, sensorRange[BS_pclipping_info.polygon_cnt].right);};
                                    sensorRange[BS_pclipping_info.polygon_cnt].right = slopeIdx;
                                }
                                else if (center < slopeIdx) {
                                    //IS_DEBUG_FLAG2{ TRACE_RNG("sensorRange[].left: %d %d", BS_pclipping_info.polygon_cnt, sensorRange[BS_pclipping_info.polygon_cnt].left);};
                                    sensorRange[BS_pclipping_info.polygon_cnt].left = slopeIdx;
                                }
                                else {
                                    sensorRange[BS_pclipping_info.polygon_cnt].right = slopeIdx;
                                    sensorRange[BS_pclipping_info.polygon_cnt].left = slopeIdx;
                                }
                                if (axis == ENUM_VER_Y) {
                                    //for saving x-range
                                    BS_pclipping_info.polygon_range[BS_pclipping_info.polygon_cnt] = BS_pclipping_info.subject_range[subjIdx];
                                    IS_DEBUG_FLAG2{ TRACE_RNG("sensorRange,BS_pclipping_info.polygon_range: %d(%d,%d %d,%d)", BS_pclipping_info.polygon_cnt, sensorRange[BS_pclipping_info.polygon_cnt].right, sensorRange[BS_pclipping_info.polygon_cnt].left, BS_pclipping_info.polygon_range[BS_pclipping_info.polygon_cnt].right, BS_pclipping_info.polygon_range[BS_pclipping_info.polygon_cnt].left); };
                                }
                                else {
                                    IS_DEBUG_FLAG2{ TRACE_RNG("sensorRange: %d(%d,%d)", BS_pclipping_info.polygon_cnt, sensorRange[BS_pclipping_info.polygon_cnt].right, sensorRange[BS_pclipping_info.polygon_cnt].left); };
                                }
#if 1 //nsmoon@190703
                                if (!is_save_used_epIdx((uint8_t)clipIdx)) {
                                    multi_save_used_ep(axis, sensor, (uint8_t)slopeIdx, (uint8_t)clipIdx);
                                }
#else
                                multi_save_used_ep(axis, sensor, &epa[slopeIdx].ep[clipIdx]);
#endif
#endif //0
#ifdef CLIPPING_CHK_ADJ_VERT //nsmoon@190107
                                clipping_var_save_result(rese, &subjecte, skip);
#else
                                clipping_var_save_result(rese, &subjecte, 0);
                                if (unusedEpStart == 0) {
                                BS_multi_save_tmp_used_ep(axis, sensor, slopeIdx, clipIdx);
                                    IS_DEBUG_FLAG{
                                        TRACE_NOP;
                                    };
                                }
                                //if (clipAreaRatio < (1.0f - EPSILON))
                                {
                                    clipCntTot++;
                                }
#endif
#ifdef REMOVE_SMALL_CLIPPING_LOOP //nsmoon@191216
                                int s1 = epa[slopeIdx].ep[clipIdx].s1;
                                int s2 = epa[slopeIdx].ep[clipIdx].s2;
                                int e1 = epa[slopeIdx].ep[clipIdx].e1;
                                int e2 = epa[slopeIdx].ep[clipIdx].e2;
                                if ((s1 == e1 && s1 == 0) || (s2 == e2 && s2 == maxPdNum)) {
                                    TRACE_NOP;
                                }
                                else {
                                    small_clipping_cnt++;
                                }
#endif
                                clipCnt++;
                            }
                            else {
                                TRACE_ERROR_MEM("ERROR_MEM! 1 exceed maxPolygonCnt %d %d %d %d [%d]", BS_pclipping_info.polygon_cnt, BS_pclipping_info.maxPolygonCnt, BS_pclipping_info.polygon_packs, BS_pclipping_info.totalProcessedSlopeCnt, BG_frame_no);
                                BS_pclipping_info.maxPolygonErrCnt++;
                                //noClip++;
                                return -1; //mem-error, nsmoon@191212
                            } //if (BS_pclipping_info.polygon_cnt < maxPolygonCnt)
                        }
                        else {
                            //small size
                            BS_pclipping_info.minTouchAreaErrCnt++;
                            //noClip++;

                            } //if (touchAreaRes > BS_minTouchArea)
                    }
                    else {
                        //no-clipping
                        BS_pclipping_info.noClippingErrCnt++;
                        //noClip++;
#ifdef MULTI_SKIP_NO_CLIPPING //nsmoon@190715
                        if (clipCnt > 0) {
                            noClipCnt++;
                        }
#endif
                    } //if (rese->len > 2)
                    //if (noClip)
                    {
                        IS_DEBUG_FLAG2{
                            //IS_DEBUG_FLAG2{ TRACE("noClip=%d %d-%d", noClip, slopeIdx, clipIdx); };
                        TRACE_NOP;
                        };
                    }
                    if (clipCnt > 0) {
                        break;
                    }
                } //for (clipIdx = start; (clipIdx * inc) <= end; clipIdx += inc)
#if (DEBUG_clipping_multi_remained_small > 0)
                IS_DEBUG_FLAG2{
                    if (axis == ENUM_VER_Y && slopeIdx == 94 /*&& BS_clipping_range_info[axis].polyIdx == 2*/)
                    //if (axis == ENUM_VER_Y && slopeIdx == 94)
					{
					TRACE_NOP;
				}
                };
#endif

#ifdef MULTI_SKIP_NO_CLIPPING //nsmoon@190715
                if (noClipCnt > 0) {
                    if (first) {
                        skipDzLeft++;
                    }
                    else {
                        skipDzRight++;
				}
                }
#endif

#if 0 //check left deadzone (max sensor number side)
                if (skipDzLeft == 0) {
                    if (epa[slopeIdx].len > 0) {
                        ret = make_edge_pattern_dz(axis, &epa[slopeIdx], DEAD_ZONE_LEFT, senStOp, senEdOp);
                    }
                    else {
                        IS_DEBUG_FLAG2{
                            TRACE_NOP;
                        }
                        ret = make_edge_pattern_empty_dz(axis, slopeIdx, DEAD_ZONE_LEFT, senStOp, senEdOp);
                        IS_DEBUG_FLAG2{ TRACENR_CMR("#"); };
                    }
                    if (ret < 0) {
                        //not enough memory
                        return -1; //nsmoon@190325
                    }
                    if (ret) {
                        addEdgePatternHalfPad(axis, &BG_edge_pattern_dz, -1, DEAD_ZONE_LEFT, &polyTmp[0], 1);
                        clippere.ve = &polyTmp[0];
                        clippere.len = INITIAL_POLYGON_VERTEX_NO;
#if (DEBUG_clipping_multi_remained_small > 0)
                        //IS_DEBUG_FLAG2{ IS_CLRSCR{ TRACE_POLYGON_MULTI(0, 0x21, MY_COLOR); } }
                        IS_DEBUG_FLAG2{ TRACE_POLYGON_MULTI(&subjecte, 0x11, 0); };
                        IS_DEBUG_FLAG2{ TRACE_POLYGON_MULTI(&clippere, 0x11, 1); };
#endif
                        rese = BS_poly_e_clip(&subjecte, &clippere);
#if (DEBUG_clipping_multi_remained_small > 0)
                        IS_DEBUG_FLAG2{
                            TRACE_POLYGON_MULTI(rese, 0x11, MY_COLOR);
                        TRACE_NOP;
                        };
#endif
                        if (rese->len > 2) {
                            touchAreaRese = BS_getTouchArea(rese);
                            if (touchAreaRese > MULTI_MIN_TOUCH_AREA) {
                                if (BS_pclipping_info.polygon_cnt < BS_pclipping_info.maxPolygonCnt) {
                                    IS_DEBUG_FLAG2{ TRACENR_CMR("*%d:%d ", subjIdx, BS_pclipping_info.subject_state[subjIdx]); };
                                    BS_pclipping_info.poly_idx_saved[BS_pclipping_info.polygon_cnt] = subjIdx;
                                    BS_pclipping_info.polygon_area[BS_pclipping_info.polygon_cnt] = touchAreaRese; //nsmoon@170521a;
                                    clipping_var_save_result(rese, rese, 0);
                                    clipCnt++;
                                }
                                else {
                                    TRACE_ERROR_MEM("ERROR_MEM! 2 exceed maxPolygonCnt %d %d %d %d [%d]", BS_pclipping_info.polygon_cnt, BS_pclipping_info.maxPolygonCnt, BS_pclipping_info.polygon_packs, BS_pclipping_info.totalProcessedSlopeCnt, BG_frame_no);
                                    BS_pclipping_info.maxPolygonErrCnt++;
                                } //if (BS_pclipping_info.polygon_cnt < maxPolygonCnt)
                            }
                            else {
                                TRACE_NOP;
                            }
                        }
                    }
                } //if (skipDzLeft) 
#endif

#if 0 //check right deadzone (min sensor number side)
                if (skipDzRight == 0) {
                    if (epa[slopeIdx].len > 0) {
                        ret = make_edge_pattern_dz(axis, &epa[slopeIdx], DEAD_ZONE_RIGHT, senStOp, senEdOp);
                    }
                    else {
                        ret = make_edge_pattern_empty_dz(axis, slopeIdx, DEAD_ZONE_RIGHT, senStOp, senEdOp);
                        IS_DEBUG_FLAG2{ TRACENR_CMR("#"); };
                    }
                    if (ret < 0) {
                        //not enough memory
                        return -1; //nsmoon@190325
                    }
                    if (ret) {
                        addEdgePatternHalfPad(axis, &BG_edge_pattern_dz, -1, DEAD_ZONE_RIGHT, &polyTmp[0], 1);
                        clippere.ve = &polyTmp[0];
                        clippere.len = INITIAL_POLYGON_VERTEX_NO;
#if (DEBUG_clipping_multi_remained_small > 0)
                        //IS_DEBUG_FLAG2{ IS_CLRSCR{ TRACE_POLYGON_MULTI(0, 0x21, MY_COLOR); } }
                        IS_DEBUG_FLAG2{ TRACE_POLYGON_MULTI(&subjecte, 0x11, 0); };
                        IS_DEBUG_FLAG2{ TRACE_POLYGON_MULTI(&clippere, 0x11, 1); };
#endif
                        rese = BS_poly_e_clip(&subjecte, &clippere);
#if (DEBUG_clipping_multi_remained_small > 0)
                        IS_DEBUG_FLAG2{
                            TRACE_POLYGON_MULTI(rese, 0x11, MY_COLOR);
                        TRACE_NOP;
			};
#endif
                        if (rese->len > 2) {
                            touchAreaRese = BS_getTouchArea(rese);
                            if (touchAreaRese > MULTI_MIN_TOUCH_AREA) {
                                if (BS_pclipping_info.polygon_cnt < BS_pclipping_info.maxPolygonCnt) {
                                    IS_DEBUG_FLAG2{ TRACENR_CMR("*%d:%d ", subjIdx, BS_pclipping_info.subject_state[subjIdx]); };
                                    BS_pclipping_info.poly_idx_saved[BS_pclipping_info.polygon_cnt] = subjIdx;
                                    BS_pclipping_info.polygon_area[BS_pclipping_info.polygon_cnt] = touchAreaRese; //nsmoon@170521a;
                                    clipping_var_save_result(rese, rese, 0);
                                    clipCnt++;
                                }
                                else {
                                    TRACE_ERROR_MEM("ERROR_MEM! 3 exceed maxPolygonCnt %d %d %d %d [%d]", BS_pclipping_info.polygon_cnt, BS_pclipping_info.maxPolygonCnt, BS_pclipping_info.polygon_packs, BS_pclipping_info.totalProcessedSlopeCnt, BG_frame_no);
                                    BS_pclipping_info.maxPolygonErrCnt++;
                                } //if (BS_pclipping_info.polygon_cnt < maxPolygonCnt)
                            }
                            else {
                                TRACE_NOP;
                            }
                        }
                    }
                } //if (skipDzRight)
#endif
		IS_DEBUG_FLAG2{
			//if (axis == ENUM_VER_Y && slopeIdx == 35)
			{
				TRACE_NOP;
			}
		};
            } //for (subjIdx = 0; subjIdx < BS_pclipping_info.subject_cnt; subjIdx++)
            IS_DEBUG_FLAG2{
                //if (axis == ENUM_VER_Y && slopeIdx == 35)
                TRACE_NOP;
            };
        if (BS_pclipping_info.polygon_cnt < 1)
        {
                clipping_var_swap();
                BS_pclipping_info.polygon_cnt = BS_pclipping_info.subject_cnt;
			IS_DEBUG_FLAG2{
				//if (axis == ENUM_VER_Y) 
				{
        TRACE_NOP;
				}
		};
            //should be ghost, removed
                //break;
        }
    } //for (pdNo = 0; pdNo < maxPdNum; pdNo++)
    IS_DEBUG_FLAG2{
        TRACE_NOP;
    };
#ifdef MULTI_SENSOR_RANGE_OLD //nsmoon@200119
#if 0 //for test
	for (i = 0; i < BS_pclipping_info.polygon_cnt; i++) {
		//TRACE("right,left:(%d)%d,%d", i, BS_pclipping_info.polygon_range[i].right, BS_pclipping_info.polygon_range[i].left);
		if (sensorRange[i].right > sensorRange[i].left) {
			TRACE_ERROR("ERROR! invalid BS_pclipping_info.polygon_range: (%d)%d,%d", i, BS_pclipping_info.polygon_range[i].right, BS_pclipping_info.polygon_range[i].left);
		}
		//TRACE_NOP;
	}
#endif
	for (i = 0; i < BS_pclipping_info.polygon_cnt; i++) {
#if (DEBUG_clipping_multi_remained_small > 0)
		IS_DEBUG_FLAG2{ TRACE_CMR("%d-%d %d %d", axis, i, sensorRange[i].right, sensorRange[i].left); };
#endif
		if (axis == ENUM_HOR_X) {
			//save sensorRange for x-axis
			BS_pclipping_info.polygon_range[i].right = sensorRange[i].right;
			BS_pclipping_info.polygon_range[i].left = sensorRange[i].left;
		}
#if (DEBUG_clipping_multi_remained_small > 1)
		else {
			IS_DEBUG_FLAG{ TRACE_CMR("^X^ %d %d %d", j, BS_pclipping_info.polygon_range[i].right, BS_pclipping_info.polygon_range[i].left); };
    }
#endif
		}
#endif //0
#ifdef REMOVE_SMALL_CLIPPING_LOOP //nsmoon@191216
	if (small_clipping_cnt < 2) {
        TRACE_RSCL("small_pd_cnt: %d,%d (%d/%d)", small_pd_cnt, small_clipping_cnt, axis, BS_pclipping_info.polygon_cnt);
        BS_pclipping_info.polygon_cnt = 0;
	}
#endif
	IS_DEBUG_FLAG2 {
        TRACE_NOP;
	};
    //return BS_pclipping_info.polygon_cnt;
    return clipCntTot;
}
#endif

ATTR_BACKEND_RAMFUNC
static int clipping_add_initial_polygon(int idx)
{
    poly_e_t subjecte;
    //subjecte.ve = &BS_debugVert[0];
    subjecte.ve = &BS_initVert[0]; //nsmoon@201022

    if (idx < 0 || idx >= BS_initial_polygon_cnt) {
        TRACE_ERROR("ERROR! invalid BS_initial_polygon_cnt %d [%d]", BS_initial_polygon_cnt, BG_frame_no);
        return -1; //error
    }
    subjecte.len = INITIAL_POLYGON_VERTEX_NO;
    subjecte.ve[0].vert.x = BS_initial_polygon[idx].pos.minX;
    subjecte.ve[0].vert.y = BS_initial_polygon[idx].pos.minY;
    subjecte.ve[1].vert.x = BS_initial_polygon[idx].pos.minX;
    subjecte.ve[1].vert.y = BS_initial_polygon[idx].pos.maxY;
    subjecte.ve[2].vert.x = BS_initial_polygon[idx].pos.maxX;
    subjecte.ve[2].vert.y = BS_initial_polygon[idx].pos.maxY;
    subjecte.ve[3].vert.x = BS_initial_polygon[idx].pos.maxX;
    subjecte.ve[3].vert.y = BS_initial_polygon[idx].pos.minY;
    subjecte.ve[0].edgeId = 0;
    subjecte.ve[1].edgeId = 1;
    subjecte.ve[2].edgeId = 2;
    subjecte.ve[3].edgeId = 3;
#if (DEBUG_clipping_multi_remained > 0)
    IS_DEBUG_FLAG{ TRACE_POLYGON_MULTI(&subjecte, 0x11, 0); }
#endif

    clipping_var_save_result(&subjecte, &subjecte, 0);

    return 0; //no-error
}

#if 0 //cal_start_end
ATTR_BACKEND_RAMFUNC
static void cal_start_end(bd_type_t bd, sen_no_t *centSen, start_end_t *stEd)
{
    //bd_type_t bdX, bdY;
    int dXs1, dXe1, dXs2, dXe2, dX1, dX2;
    int dY1, dY2; //dYs1, dYe1, dYs2, dYe2,
    int maxSlopeVal, maxPd, len;

    if (bd == ENUM_BOT_BD) {
        maxPd = BS_maxHorPdNum - 1;
        maxSlopeVal = BS_offsetTblX[BS_offsetTblLenX - 1];
        dY1 = centSen->y;
        dY2 = BS_maxVerPdNum - dY1;
        dX1 = centSen->x;
        dX2 = BS_maxHorPdNum - dX1;
    }
    else if (bd == ENUM_TOP_BD) {
        maxPd = BS_maxHorPdNum - 1;
        maxSlopeVal = BS_offsetTblX[BS_offsetTblLenX - 1];
        dY2 = centSen->y;
        dY1 = BS_maxVerPdNum - dY2;
        dX1 = centSen->x;
        dX2 = BS_maxHorPdNum - dX1;
    }
    else if (bd == ENUM_RHT_BD) {
        maxPd = BS_maxVerPdNum - 1;
        maxSlopeVal = BS_offsetTblY[BS_offsetTblLenY - 1];
        dY1 = centSen->x;
        dY2 = BS_maxHorPdNum - dY1;
        dX1 = centSen->y;
        dX2 = BS_maxVerPdNum - dX1;
    }
    else {
        // (bd == ENUM_LFT_BD)
        maxPd = BS_maxVerPdNum - 1;
        maxSlopeVal = BS_offsetTblY[BS_offsetTblLenY - 1];
        dY2 = centSen->x;
        dY1 = BS_maxHorPdNum - dY2;
        dX1 = centSen->y;
        dX2 = BS_maxVerPdNum - dX1;

    }
    if (dY2 < 1) {
        TRACE_ERROR("ERROR! invalid dY2 %d (%d) [%d]", dY2, bd, BG_frame_no);
        dY2 = 1;
    }
    dXe2 = GET_MIN(maxSlopeVal, dX2);
    dXs2 = GET_MIN(maxSlopeVal, dX1);
    dXs1 = (dXe2 * dY1) / dY2;
    dXe1 = (dXs2 * dY1) / dY2;
    stEd->start = dX1 - dXs1;
    stEd->end = dX1 + dXe1;
    len = stEd->end - stEd->start;
    if ((len % 2) == 0) {
        stEd->end++;
    }
    stEd->start = GET_MAX(stEd->start, 0);
    stEd->end = GET_MIN(stEd->end, maxPd);
}
#endif

#if 0 //cal_start_end_pos
#define ADJ_START_END_RANGE		0 //-1
ATTR_BACKEND_RAMFUNC
static void cal_start_end_pos(bd_type_t bd, vec_t *centPos, start_end_t *stEd)
{
    //bd_type_t bdX, bdY;
    int senNo, senNoSt, senNoEd, diffSt, diffEd;
    float xPos; // , xPosSt, xPosEd, xDiffSt, xDiffEd;
    float yPos, yPosMax, yRatio;
    int maxSlopeVal, maxSen;
    float adjSlopeVal; //*senPos,

    if (bd == ENUM_BOT_BD) {
        //x-axis, led
        maxSen = BS_maxHorPdNum - 1;
        //senPos = &BS_led_pos_x[0];
        maxSlopeVal = BS_offsetTblX[BS_offsetTblLenX - 1];

        yPos = centPos->y;
        yPosMax = BS_sensor_end_y;
        yRatio = yPos / yPosMax;
        xPos = centPos->x;
    }
    else if (bd == ENUM_TOP_BD) {
        //x-axis, pd
        maxSen = BS_maxHorPdNum - 1;
        //senPos = &BS_pd_pos_x[0];
        maxSlopeVal = BS_offsetTblX[BS_offsetTblLenX - 1];

        yPos = BS_sensor_end_y - centPos->y;
        yPosMax = BS_sensor_end_y;
        yRatio = yPos / yPosMax;
        xPos = centPos->x;
    }
    else if (bd == ENUM_RHT_BD) {
        //y-axis, pd
        maxSen = BS_maxVerPdNum - 1;
        //senPos = &BS_pd_pos_y[0];
        maxSlopeVal = BS_offsetTblY[BS_offsetTblLenY - 1];

        //swap x and y
        yPos = centPos->x;
        yPosMax = BS_sensor_end_x;
        yRatio = yPos / yPosMax;
        xPos = centPos->y;
    }
    else {
        // (bd == ENUM_LFT_BD)
        //y-axis, led
        maxSen = BS_maxVerPdNum - 1;
        //senPos = &BS_led_pos_y[0];
        maxSlopeVal = BS_offsetTblY[BS_offsetTblLenY - 1];

        //swap x and y
        yPos = BS_sensor_end_x - centPos->x;
        yPosMax = BS_sensor_end_x;
        yRatio = yPos / yPosMax;
        xPos = centPos->y;
    }

    //get start and end sensor
    senNo = getSenNoFromPos2(bd, xPos);
    adjSlopeVal = (maxSlopeVal) * yRatio + ADJ_START_END_RANGE;
    senNoSt = senNo - (int)adjSlopeVal;
    senNoEd = senNo + (int)adjSlopeVal;
    senNoSt = GET_MAX(senNoSt, 0);
    senNoEd = GET_MIN(senNoEd, maxSen);
    diffSt = senNo - senNoSt;
    diffEd = senNoEd - senNo;

    //re-calculate maxSlopeVal
    maxSlopeVal = GET_MIN(diffSt, diffEd);
    stEd->start = senNo - maxSlopeVal;
    stEd->end = senNo + maxSlopeVal;
    //TRACE(" ..senNo,senNoSt,senNoEd: %d,%d,%d (%d,%d)", senNo, senNoSt, senNoEd, stEd->start, stEd->end);
}

ATTR_BACKEND_RAMFUNC
static int getSenNoFromPos(bd_type_t bd, start_end_t *stEd, float cent)
{
    float *bdPos, diff1, diff2;
    int i, inc, mid, end, ret;

    if (stEd->start >= stEd->end + 1) {
        TRACE_ERROR("ERROR! getSenNoFromPos..invalid start,end: %d %d [%d]", stEd->start, stEd->end, BG_frame_no);
        return stEd->start;
    }
    mid = (stEd->start + stEd->end) / 2;

    if (bd == ENUM_BOT_BD) {
        bdPos = &BS_led_pos_x[0];
    }
    else if (bd == ENUM_TOP_BD) {
        bdPos = &BS_pd_pos_x[0];
    }
    else if (bd == ENUM_LFT_BD) {
        bdPos = &BS_led_pos_y[0];
    }
    else {
        //(bd == ENUM_RHT_BD)
        bdPos = &BS_pd_pos_y[0];
    }

    diff1 = bdPos[mid - 1] - cent;
    diff1 = GET_ABS(diff1);
    diff2 = bdPos[mid + 1] - cent;
    diff2 = GET_ABS(diff2);

    if (diff1 < diff2) {
        inc = -1;
        end = stEd->start;
    }
    else {
        inc = 1;
        end = stEd->end;
    }
    ret = end;
    diff2 = bdPos[mid] - cent;
    diff2 = GET_ABS(diff2);
    for (i = (mid + inc); (i * inc) < end; i += inc) {
        diff1 = bdPos[i] - cent;
        diff1 = GET_ABS(diff1);
        //TRACE("i,cent,bdPos[],diff1,diff2: %d, %0.1f, %0.1f, %0.1f, %0.1f", i, cent, bdPos[i], diff1, diff2);
        if (diff1 > diff2) {
            ret = i - inc;
            break;
        }
        diff2 = diff1;
    }
    return ret;
}
#endif

#if 0 //for testing
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_getSenNoFromPos2	0
#endif
#if (DEBUG_getSenNoFromPos2 == 1)
#define TRACE_GSNFP(...)		//TRACE(__VA_ARGS__)
#define TRACENR_GSNFP(...)	//TRACENR(__VA_ARGS__)
#else
#define TRACE_GSNFP(...)
#define TRACENR_GSNFP(...)
#endif

#define MAX_DIFF_CENT	20.0f
#define MAX_RETRY_CNT	20
ATTR_BACKEND_RAMFUNC
static int getSenNoFromPos2(bd_type_t bd, float cent)
{

    float *bdPos, diff = 0, diffPre = 0;
    int i, min = 0, max, mid, inc, midPre = 0;
    int end; //start

    if (bd == ENUM_BOT_BD) {
        bdPos = &BS_led_pos_x[0];
        max = BS_maxHorPdNum - 1;
    }
    else if (bd == ENUM_TOP_BD) {
        bdPos = &BS_pd_pos_x[0];
        max = BS_maxHorPdNum - 1;
    }
    else if (bd == ENUM_LFT_BD) {
        bdPos = &BS_led_pos_y[0];
        max = BS_maxVerPdNum - 1;
    }
    else {
        //(bd == ENUM_RHT_BD)
        bdPos = &BS_pd_pos_y[0];
        max = BS_maxVerPdNum - 1;
    }
    mid = (max + 1) / 2;
    TRACE_GSNFP("bd,min,max,mid(pos,cent): %d,%d,%d,%d(%0.1f,%0.1f)", bd, min, max, mid, bdPos[mid], cent);

    for (i = 0; i < MAX_RETRY_CNT; i++)
    {
        //cal diff
        diff = cent - bdPos[mid];
        TRACE_GSNFP(" ...%d,%d,%d (%0.1f, %0.2f)", min, max, mid, bdPos[mid], diff);

        if (GET_ABS(diff) < MAX_DIFF_CENT) {
            break;
        }

        //cal mid
        if (diff > 0) {
            min = mid;
            inc = (max - min) / 2;
            mid = mid + inc;
        }
        else {
            max = mid;
            inc = (max - min) / 2;
            mid = mid - inc;
        }

    }
    TRACE_NOP;

    if (diff > 0) {
        inc = 1;
        end = max;
    }
    else {
        inc = -1;
        end = min;
    }
    diffPre = diff;
    midPre = mid;
    for (i = mid + inc; (i * inc) <= (end * inc); i += inc) {
        diff = cent - bdPos[i];
        TRACE_GSNFP(" ...%d (%0.1f, %0.2f)", i, bdPos[i], diff);
        if (GET_ABS(diff) > GET_ABS(diffPre)) break;
        diffPre = diff;
        midPre = i;
    }
    TRACE_GSNFP(" =>%d", midPre);

    return midPre;
}

ATTR_BACKEND_RAMFUNC
static int cal_start_end_pos(bd_type_t bd, vec_t *cent, start_end_t *stEd)
{
    sen_no_t centSenTmp;
    centSenTmp.x = getSenNoFromPos(BS_curPoly.bdX, &BS_curPoly.stEdX, cent->x);
    centSenTmp.y = getSenNoFromPos(BS_curPoly.bdY, &BS_curPoly.stEdY, cent->y);
    cal_start_end(bd, &centSenTmp, stEd);
    return ((stEd->start + stEd->end) / 2);
}
#endif

#ifdef MULTI_RANGE_RECAL_MIN_MAX //nsmoon@200107
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_multi_recal_min_max	0
#endif
#if (DEBUG_multi_recal_min_max > 0)
#define TRACE_MRMM(...)		TRACE(__VA_ARGS__)
#define TRACENR_MRMM(...)	TRACENR(__VA_ARGS__)
#else
#define TRACE_MRMM(...)
#define TRACENR_MRMM(...)
#endif

ATTR_BACKEND_RAMFUNC
static int multi_recal_min_max(axis_t axis, touch_info_multi_t *touchInfo, pos_minMax2_t *minMaxRet)
{
    ep_buf_t *epa;
    sen_type_t sen;
    vec_t p0, p1, p2, p3, pR;
    //float center, min, max;
    float epa_inst_diff, epa_inst_tmp;
    //epa_inst_t epa_inst_tmp;
    int maxPdNum; //, epa_inst_cnt;
    //int slopeValPlus, slopeValMinus, slopeValTmp;
    //int stopPlus, stopMinus, idxPlus, idxMinus;
    //pos_minMax2_t minMaxTmp;
    int i, j, instCnt;
    int slopeIdx, epIdx, mode, slopeIdxStart = 0, slopeIdxEnd = 0;
    pos_minMax2_t inRange; //nsmoon@200213, do not use it @550
#ifdef USE_CUST_MALLOC //nsmoon@201012
    vec_e_t epaPoly[INITIAL_POLYGON_VERTEX_NO]; //nsmoon@201008
#endif

    if (axis == ENUM_HOR_X) {
        //x-axis
        epa = EPA_ADDR_X;
        maxPdNum = BS_maxHorPdNum - 1;
        //center = (touchInfo->mM.minX + touchInfo->mM.maxX) * 0.5f;
        p2.x = touchInfo->mM.minX - MULTI_CAL_MIN_MAX_TOL;
        p3.x = touchInfo->mM.maxX + MULTI_CAL_MIN_MAX_TOL;
        p2.y = (touchInfo->mM.minY + touchInfo->mM.maxY) * 0.5f;
        p3.y = p2.y;
        inRange.min = touchInfo->mM.minX;
        inRange.max = touchInfo->mM.maxX;
        if (touchInfo->bdX == ENUM_BOT_BD) {
            sen = ENUM_LED;
        }
        else {
            sen = ENUM_PD;
        }
        if (touchInfo->sRx.right <= touchInfo->sRx.left) {
            slopeIdxStart = touchInfo->sRx.right;
            slopeIdxEnd = touchInfo->sRx.left;
        }
        else {
            TRACE_ERROR("ERROR! invalid sRx range: %d, %d", touchInfo->sRx.right, touchInfo->sRx.left);
            return 1; //error
        }
    }
    else {
        //y-axis
        epa = EPA_ADDR_Y;
        maxPdNum = BS_maxVerPdNum - 1;
        //center = (touchInfo->mM.minY + touchInfo->mM.maxY) * 0.5f;
        p2.x = (touchInfo->mM.minX + touchInfo->mM.maxX) * 0.5f;
        p3.x = p2.x;
        p2.y = touchInfo->mM.minY - MULTI_CAL_MIN_MAX_TOL;
        p3.y = touchInfo->mM.maxY + MULTI_CAL_MIN_MAX_TOL;
        inRange.min = touchInfo->mM.minY;
        inRange.max = touchInfo->mM.maxY;
        if (touchInfo->bdY == ENUM_LFT_BD) {
            sen = ENUM_LED;
        }
        else {
            sen = ENUM_PD;
        }
        if (touchInfo->sRy.right <= touchInfo->sRy.left) {
            slopeIdxStart = touchInfo->sRy.right;
            slopeIdxEnd = touchInfo->sRy.left;
        }
        else {
            TRACE_ERROR("ERROR! invalid sRy range: %d, %d", touchInfo->sRy.right, touchInfo->sRy.left);
        }
    }
#if (DEBUG_multi_recal_min_max == 1)
    DEBUG_SHOW_LINE_POS(&p2, &p3, MY_COLOR);
#endif

    for (mode = 0; mode < 2; mode++) {
        //mode: 0(min), 1(max)
        instCnt = 0;
        for (slopeIdx = slopeIdxStart; slopeIdx <= slopeIdxEnd; slopeIdx++) {
            if (epa[slopeIdx].len < 1) continue;
            for (epIdx = 0; epIdx < epa[slopeIdx].len; epIdx++) {
                //IS_DEBUG_FLAG{ TRACE_MRMM("sen,dz,s1,e1: %d,%d(%d,%d)(%d,%d)", sen, epa[slopeIdx].dz[epIdx], epa[slopeIdx].ep[epIdx].s1, epa[slopeIdx].ep[epIdx].e1, epa[slopeIdx].ep[epIdx].s2, epa[slopeIdx].ep[epIdx].e2);}
#if 1 //for test, FIXME
                if (epa[slopeIdx].dz[epIdx] > 0) {
                    if (sen == ENUM_LED) {
                        if (epa[slopeIdx].ep[epIdx].s1 == 0 || epa[slopeIdx].ep[epIdx].s2 == maxPdNum) {
                            continue;
                        }
                    }
                    else {
                        if (epa[slopeIdx].ep[epIdx].e1 == 0 || epa[slopeIdx].ep[epIdx].e2 == maxPdNum) {
                            continue;
                        }
                    }
                } //if (epa[slopeIdx].dz[epIdx] > 0)
#endif
#ifdef USE_CUST_MALLOC //nsmoon@201012
                multi_get_initial_poly(axis, &epa[slopeIdx].ep[epIdx], &epaPoly[0], 0);
                if (mode == 0) {
                    //min
                    p0 = epaPoly[0].vert;
                    p1 = epaPoly[1].vert;
                }
                else {
                    //max
                    p0 = epaPoly[2].vert;
                    p1 = epaPoly[3].vert;
                }
#else
                if (mode == 0) {
                    //min
                    p0 = epa[slopeIdx].poly[epIdx][0].vert;
                    p1 = epa[slopeIdx].poly[epIdx][1].vert;
                }
                else {
                    //max
                    p0 = epa[slopeIdx].poly[epIdx][2].vert;
                    p1 = epa[slopeIdx].poly[epIdx][3].vert;
                }
#endif
#if (DEBUG_multi_recal_min_max == 1)
                DEBUG_SHOW_LINE_POS(&p0, &p1, MY_COLOR - 1);
#endif
                if (BS_line_intersection(&p0, &p1, &p2, &p3, &pR)) {
#if (DEBUG_multi_recal_min_max == 1)
                    DEBUG_SHOW_POS(&pR, 0.2f, 0.2f, 1);
#endif
                    if (instCnt < MAX_INST_BUFF) {
#if 1 //nsmoon@200213
                        if (axis == ENUM_HOR_X) {
                            if (pR.x > (inRange.min - EPSILON) && pR.x < (inRange.max + EPSILON)) {
                                BS_epa_inst[instCnt++] = pR.x;
                            }
                        }
                        else {
                            if (pR.y >(inRange.min - EPSILON) && pR.y < (inRange.max + EPSILON)) {
                                BS_epa_inst[instCnt++] = pR.y;
                            }
                        }
#else
                        BS_epa_inst[instCnt++] = (axis == ENUM_HOR_X) ? pR.x : pR.y;
#endif
                    }
                    else {
                        TRACE_ERROR("ERROR! invalid epa_inst[] cnt %d [%d]", instCnt, BG_frame_no);
                    }
                }
            } //for (j = 0; j < epa[i].len; j++)
        } //for (i = range->right; i <= range->left; i++)

        //sort: min(ascending), max(descending)
        //float inst_i, inst_j;
        for (i = 0; i < (instCnt - 1); i++) {
            for (j = (i + 1); j < instCnt; j++) {
                if ((mode == 0 && BS_epa_inst[i] > BS_epa_inst[j]) || (mode == 1 && BS_epa_inst[i] < BS_epa_inst[j])) {
                    epa_inst_tmp = BS_epa_inst[i];
                    BS_epa_inst[i] = BS_epa_inst[j];
                    BS_epa_inst[j] = epa_inst_tmp;
                }
            } //for (j = (i + 1); j < cnt; j++)
        } //for (i = 0; i < (instCnt - 1); i++)
#if (DEBUG_multi_recal_min_max == 1)
        for (i = 0; i < instCnt; i++) {
            IS_DEBUG_FLAG{ TRACENR_MRMM("%0.1f ", BS_epa_inst[i]);}
        }
#endif
        if (instCnt < 2) {  //2=>10, nsmoon@200326 //nsmoon@200423 10=>2
            //TRACE_ERROR("ERROR! multi_recal_min_max..invalid instCnt: %d(%d,%d)[%d]", instCnt, axis, mode, BG_frame_no);
            TRACE("*1= %d", instCnt);
            return 1; //error
        }
        //grouping
        for (i = 1; i < instCnt; i++) {
            epa_inst_diff = GET_ABS(BS_epa_inst[i] - BS_epa_inst[i - 1]);
            IS_DEBUG_FLAG{ TRACENR_MRMM("(%0.1f,%0.1f)", BS_epa_inst[i - 1], BS_epa_inst[i]);}
            if (epa_inst_diff < MULTI_EPA_INST_DIST) break;
        }
        if (i < instCnt) {
            if (mode == 0) {
                minMaxRet->min = BS_epa_inst[i - 1];
            }
            else {
                minMaxRet->max = BS_epa_inst[i - 1];
            }
        }
        else {
            //TRACE_ERROR("ERROR! multi_recal_min_max..not found min/max: %d(%d,%d)[%d]", instCnt, axis, mode, BG_frame_no);
            TRACE_ERROR("*2= %d", i);
            return 1; //error
        }
        IS_DEBUG_FLAG{
            TRACE_NOP;
        }
    } //for (mode = 0; mode < 2; mode++)
#if 1 //nsmoon@200529, FIXME
    float width = GET_ABS(minMaxRet->max - minMaxRet->min);
    if (width < 4.0f) {
        return 1; //error
    }
#endif
    return 0; //no-error
}
#endif //MULTI_RANGE_RECAL_MIN_MAX

ATTR_BACKEND_RAMFUNC
void BS_epaInitMulti(void)
{
    int i;
	//TRACE("epaInitMulti...");

#ifdef USE_CUST_MALLOC //nsmoon@201012
    for (i = 0; i < BS_max_mum_pd; i++) { //nsmoon@201014
#else
    for (i = 0; i < MAX_NUM_PD; i++) {
#endif
		BG_edge_pattern[i].len = 0;
	}
    BS_edge_pattern_ep_cnt = 0; //init
    BG_edge_pattern_cnt_x = 0; //init
    BG_edge_pattern_cnt_y = 0; //init
}

#ifdef USED_LINE_METHOD_OLD //nsmoon@200119
#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_set_used_line		0
#endif
ATTR_BACKEND_RAMFUNC
static int set_unset_used_led_pd(axis_t axis, int led, int pd, int mode)
{
    uint8_t *lineStat, lineStatMask;
    int lineStatIdx, maxInLineBufLed;

    if (axis == ENUM_HOR_X) {
        lineStat = LINE_STAT_BUF_ADDR_X;
        maxInLineBufLed = MAX_IN_LINE_BUF_LED_X;
    }
    else {
        lineStat = LINE_STAT_BUF_ADDR_Y;
        maxInLineBufLed = MAX_IN_LINE_BUF_LED_Y;
    }

    lineStatIdx = GET_LINE_STAT_IDX(pd, led);
    lineStatMask = GET_LED_BIT_MASK(led);
	if (mode) {
		//used line
    lineStat[lineStatIdx] &= ~lineStatMask;
	}
	else {
		//un-used line
		lineStat[lineStatIdx] |= lineStatMask;
	}
    TRACE_NOP;
    return 0; //no error
}
#endif

#ifdef USED_LINE_METHOD_OLD //nsmoon@200119
static int set_unset_used_line(axis_t axis, sen_type_t sen, ep_no_t *ep, uint8_t *dz, int mode)
{
	int i;
	int s1, s2, e1, e2;
	int maxPdNum, slopeValMax;
#if (DEBUG_set_used_line > 0)
	int sWidth, eWidth;
#endif

	if (axis == ENUM_HOR_X) {
		maxPdNum = BS_maxHorPdNum - 1;
		slopeValMax = BS_slopeValMaxX;
	}
	else {
		maxPdNum = BS_maxVerPdNum - 1;
		slopeValMax = BS_slopeValMaxY;
	}

	s1 = ep->s1;
	s2 = ep->s2;
	e1 = ep->e1;
	e2 = ep->e2;

	if (sen == ENUM_LED) {
		if (s1 != s2) {
			if (s1 == 0 && e1 == 0) {
				if (s2 == maxPdNum && e2 == maxPdNum) {
					TRACE_NOP;
					s2 = *dz;
					e2 = s2 + slopeValMax;
					e2 = GET_MIN(e2, maxPdNum);
				}
				s1 = s2;
				e1 = s1 - slopeValMax;
				e1 = GET_MAX(e1, 0);
			}
			else if (s2 == maxPdNum && e2 == maxPdNum) {
				if (s1 == 0 && e1 == 0) {
					TRACE_NOP;
					s1 = *dz;
					e1 = s1 - slopeValMax;
					e1 = GET_MAX(e1, 0);
				}
				s2 = s1;
				e2 = s2 + slopeValMax;
				e2 = GET_MIN(e2, maxPdNum);
			}
			else {
#if (DEBUG_set_used_line > 1)
				IS_DEBUG_FLAG{ TRACE("***ENUM_LED s1,e1,s2,e2: (%d) %d %d %d %d", axis, s1, e1, s2, e2); };
#endif
				return 0; //skip
			}
		}
#if (DEBUG_set_used_line > 0)
		eWidth = e2 - e1 + 1;
		IS_DEBUG_FLAG{ TRACE("eWidth=(%d)%d(%d:%d,%d-%d,%d)", axis, eWidth, axis, s1, s2, e1, e2); };
#endif
		for (i = e1; i <= e2; i++) {
#if(DEBUG_set_used_line > 1)
			IS_DEBUG_FLAG{ TRACE("axis,pd,led: (%d) %d %d", axis, i, s1); };
#endif
			if (mode) {
				set_unset_used_led_pd(axis, s1, i, 1); //1:set
			}
			else {
				set_unset_used_led_pd(axis, s1, i, 0); //0:un-set
			}
		}
	}
	else {
		//(sen == ENUM_PD)
		if (e1 != e2) {
			if (s1 == 0 && e1 == 0) {
				if (s2 == maxPdNum && e2 == maxPdNum) {
					TRACE_NOP;
					e2 = *dz;
					s2 = e2 + slopeValMax;
					s2 = GET_MIN(s2, maxPdNum);
				}
				e1 = e2;
				s1 = e1 - slopeValMax;
				s1 = GET_MAX(s1, 0);
			}
			else if (s2 == maxPdNum && e2 == maxPdNum) {
				if (s1 == 0 && e1 == 0) {
					TRACE_NOP;
					e1 = *dz;
					s1 = e1 - slopeValMax;
					s1 = GET_MAX(s1, 0);
				}
				e2 = e1;
				s2 = e2 + slopeValMax;
				s2 = GET_MIN(s2, maxPdNum);
			}
			else {
#if (DEBUG_set_used_line > 1)
				IS_DEBUG_FLAG{ TRACE("***ENUM_PD s1,e1,s2,e2: (%d) %d %d %d %d", axis, s1, e1, s2, e2); };
#endif
				return 0; //skip
			}
		}
#if (DEBUG_set_used_line > 0)
		sWidth = s2 - s1 + 1;
		IS_DEBUG_FLAG{ TRACE("sWidth=(%d)%d(%d:%d,%d-%d,%d)", axis, sWidth, axis, s1, s2, e1, e2); };
#endif
		for (i = s1; i <= s2; i++) {
#if (DEBUG_set_used_line > 1)
			IS_DEBUG_FLAG{ TRACE("axis,pd,led: (%d) %d %d", axis, e1, i); };
#endif
			if (mode) {
				set_unset_used_led_pd(axis, i, e1, 1); //1:set
			}
			else {
				set_unset_used_led_pd(axis, i, e1, 0); //0:un-set
			}
		}
	}
	return 0; //done
}

ATTR_BACKEND_RAMFUNC
static int set_used_ep2(void)
{
	//int i, k, maxPdNum, slopeValMax, start, end,
	int k;
	axis_t axis;
	sen_type_t sen;
	ep_no_t *ep;
	uint8_t *dz;
	//int s1, s2, e1, e2;
	uint8_t slopeIdx, epIdx;
	ep_buf_t *epa;
#if (DEBUG_set_used_line > 0)
	int eWidth, sWidth;
#endif

	for (k = 0; k < BS_tmp_used_ep_cnt; k++) {
		axis = BS_tmp_used_ep[k].axis;
		sen = BS_tmp_used_ep[k].sen;
		slopeIdx = BS_tmp_used_ep[k].slopeIdx;
		epIdx = BS_tmp_used_ep[k].epIdx;

		if (axis == ENUM_HOR_X) {
			//maxPdNum = BS_maxHorPdNum - 1;
			//slopeValMax = BS_slopeValMaxX;
			epa = EPA_ADDR_X;
		}
		else {
			//maxPdNum = BS_maxVerPdNum - 1;
			//slopeValMax = BS_slopeValMaxY;
			epa = EPA_ADDR_Y;
		}

		ep = &epa[slopeIdx].ep[epIdx];
		dz = &epa[slopeIdx].dz[epIdx];
		set_unset_used_line(axis, sen, ep, dz, 1); //1:set
#if 0 //for test
		s1 = ep->s1;
		s2 = ep->s2;
		e1 = ep->e1;
		e2 = ep->e2;

		if (sen == ENUM_LED) {
			if (s1 != s2) {
				if (s1 == 0 && e1 == 0) {
					if (s2 == maxPdNum && e2 == maxPdNum) {
						TRACE_NOP;
						s2 = *dz;
						e2 = s2 + slopeValMax;
						e2 = GET_MIN(e2, maxPdNum);
					}
					s1 = s2;
					e1 = s1 - slopeValMax;
					e1 = GET_MAX(e1, 0);
				}
				else if (s2 == maxPdNum && e2 == maxPdNum) {
					if (s1 == 0 && e1 == 0) {
						TRACE_NOP;
						s1 = *dz;
						e1 = s1 - slopeValMax;
						e1 = GET_MAX(e1, 0);
					}
					s2 = s1;
					e2 = s2 + slopeValMax;
					e2 = GET_MIN(e2, maxPdNum);
				}
				else {
#if (DEBUG_set_used_line > 1)
					TRACE("***ENUM_LED s1,e1,s2,e2: (%d) %d %d %d %d", axis, s1, e1, s2, e2);
#endif
					return 0;
				}
			}
#if (DEBUG_set_used_line > 0)
			eWidth = e2 - e1 + 1;
			TRACE("eWidth=%d(%d:%d,%d-%d,%d)", eWidth, axis, s1, s2, e1, e2);
#endif
			for (i = e1; i <= e2; i++) {
#if(DEBUG_set_used_line > 1)
				TRACE("axis,pd,led: (%d) %d %d", axis, i, s1);
#endif
				set_used_line(axis, s1, i);
			}
		}
		else {
			//(sen == ENUM_PD)
			if (e1 != e2) {
				if (s1 == 0 && e1 == 0) {
					if (s2 == maxPdNum && e2 == maxPdNum) {
						TRACE_NOP;
						e2 = *dz;
						s2 = e2 + slopeValMax;
						s2 = GET_MIN(s2, maxPdNum);
					}
                e1 = e2;
                s1 = e1 - slopeValMax;
                s1 = GET_MAX(s1, 0);
            }
            else if (s2 == maxPdNum && e2 == maxPdNum) {
					if (s1 == 0 && e1 == 0) {
						TRACE_NOP;
						e1 = *dz;
						s1 = e1 - slopeValMax;
						s1 = GET_MAX(s1, 0);
					}
                e2 = e1;
                s2 = e2 + slopeValMax;
                s2 = GET_MIN(s2, maxPdNum);
            }
            else {
#if (DEBUG_set_used_line > 1)
                TRACE("***ENUM_PD s1,e1,s2,e2: (%d) %d %d %d %d", axis, s1, e1, s2, e2);
#endif
                return 0;
            }
        }
#if (DEBUG_set_used_line > 0)
		sWidth = s2 - s1 + 1;
		TRACE("sWidth=%d(%d:%d,%d-%d,%d)", sWidth, axis, s1, s2, e1, e2);
#endif
        for (i = s1; i <= s2; i++) {
#if (DEBUG_set_used_line > 1)
				TRACE("axis,pd,led: (%d) %d %d", axis, e1, i);
#endif
            set_used_line(axis, i, e1);
        }
    }
#endif //0
	} //for (k = 0; k < BS_tmp_used_ep_cnt; k++)

    return 0;
}
#endif

#if 0 //reserved
static int unset_used_ep2(bd_type_t bd, ep_min_max_t *minMaxEp)
{
	int axis, slopeIdx, clipIdx;
	int minSenlim, maxSenlim;
	sen_type_t sen;
	ep_buf_t *epa;
	ep_no_t *ep;
	uint8_t *dz;

	if (bd == ENUM_BOT_BD || bd == ENUM_LFT_BD) {
		sen = ENUM_LED;
	}
	else {
		sen = ENUM_PD;
	}

	for (axis = 0; axis < ENUM_AXIS_END; axis++) {
		if (axis == (int)ENUM_HOR_X) {
			epa = EPA_ADDR_X;
		}
		else {
			epa = EPA_ADDR_Y;
		}
        BS_get_clipping_range((axis_t)axis, minMaxEp, &minSenlim, &maxSenlim);
		for (slopeIdx = minSenlim; slopeIdx <= maxSenlim; slopeIdx++) {
			for (clipIdx = 0; clipIdx < epa[slopeIdx].len; clipIdx++) {
				ep = &epa[slopeIdx].ep[clipIdx];
				dz = &epa[slopeIdx].dz[clipIdx];
				set_unset_used_line(axis, sen, ep, dz, 0); //1:unset
			}
		}
	}
	return 0;
}
#endif

#if 0
ATTR_BACKEND_RAMFUNC
static int set_used_dz(axis_t axis, sen_type_t sen, ep_no_t *ep, int8_t *epDz)
{
    int i, maxPdNum, slopeValMax; //start, end,
    int dz = *epDz;
    int s1 = ep->s1;
    int s2 = ep->s2;
    int e1 = ep->e1;
    int e2 = ep->e2;

    if (axis == ENUM_HOR_X) {
        maxPdNum = BS_maxHorPdNum - 1;
        slopeValMax = BS_slopeValMaxX;
    }
    else {
        maxPdNum = BS_maxVerPdNum - 1;
        slopeValMax = BS_slopeValMaxY;
    }

    if (sen == ENUM_LED) {
        if (s1 == 0) {
            s1 = dz;
            e1 = s1 - slopeValMax;
            e1 = GET_MAX(e1, 0);

        }
        if (s2 == maxPdNum) {
            s2 = dz;
            e2 = s2 + slopeValMax;
            e2 = GET_MIN(e2, maxPdNum);
        }
#if (DEBUG_set_used_line == 1)
        TRACE("---ENUM_LED s1,s2,e1,e2: (%d) %d %d %d %d", axis, s1, s2, e1, e2);
#endif
        if (s1 != s2) {
            TRACE_ERROR("ERROR! invalid deadzone ep s1,s2: %d %d [%d]", s1, s2, BG_frame_no);
        }
        for (i = e1; i <= e2; i++) {
            set_used_line(axis, s1, i);
        }
    } //if (sen == ENUM_LED)
    else {
        //(sen == ENUM_PD)
        if (e1 == 0) {
            e1 = dz;
            s1 = e1 - slopeValMax;
            s1 = GET_MAX(s1, 0);

        }
        if (e2 == maxPdNum) {
            e2 = dz;
            s2 = e2 + slopeValMax;
            s2 = GET_MIN(s2, maxPdNum);
        }
#if (DEBUG_set_used_line == 1)
        TRACE("---ENUM_PD s1,s2,e1,e2: (%d) %d %d %d %d", axis, s1, s2, e1, e2);
#endif
        if (e1 != e2) {
            TRACE_ERROR("ERROR! invalid deadzone ep s1,s2: %d %d [%d]", s1, s2, BG_frame_no);
        }
        for (i = s1; i <= s2; i++) {
            set_used_line(axis, i, e1);
        }
    } //(sen == ENUM_PD)

    return 0;
}
#endif

#ifdef USED_LINE_METHOD_OLD //nsmoon@200119
#if 0 //nsmoon@190703
ATTR_BACKEND_RAMFUNC
static void init_save_used_epIdx(void)
{
	memset(save_used_epIdx, 0, MAX_TMP_USED_EPIDX);
	save_used_epIdx_cnt = 0;
}
#endif
#if 1 //nsmoon@190703
ATTR_BACKEND_RAMFUNC
static int is_save_used_epIdx(uint8_t epIdx)
{
	int i;
	for (i = 0; i < save_used_epIdx_cnt; i++) {
		if (save_used_epIdx[i] == epIdx) {
			return 1; //found
		}
	}
	if (i < MAX_TMP_USED_EPIDX) {
		save_used_epIdx[i] = epIdx;
		save_used_epIdx_cnt++;
	}
	else {
		TRACE_ERROR_MEM("ERROR! invalid save_used_epIdx_cnt: %d", save_used_epIdx_cnt);
	}
	return 0; //not-found
}

static int multi_save_used_ep(axis_t axis, sen_type_t sen, uint8_t slopeIdx, uint8_t epIdx)
#else
static int multi_save_used_ep(axis_t axis, sen_type_t sen, ep_no_t *ep)
#endif
{
    if (BS_tmp_used_ep_cnt < MAX_TMP_USED_EP) {
        BS_tmp_used_ep[BS_tmp_used_ep_cnt].axis = axis;
        BS_tmp_used_ep[BS_tmp_used_ep_cnt].sen = sen;
#if 1 //nsmoon@190703
		BS_tmp_used_ep[BS_tmp_used_ep_cnt].slopeIdx = slopeIdx;
		BS_tmp_used_ep[BS_tmp_used_ep_cnt].epIdx = epIdx;
#else
        BS_tmp_used_ep[BS_tmp_used_ep_cnt].ep = ep;
#endif
        BS_tmp_used_ep_cnt++;
        return 0;
    }
    else {
        TRACE_ERROR_MEM("ERROR_MEM! multi_save_used_ep..invalid BS_tmp_used_ep_cnt: %d", BS_tmp_used_ep_cnt);
        return 1; //error
    }
}
#endif

#define EXTEND_EDGE_RANGE_FOR_GHOST //nsmoon@190702
#ifdef EXTEND_EDGE_RANGE_FOR_GHOST
static int is_tp_edge(axis_t axis, int idx)
{
	float cent;
	int edge = 0;

	if (axis == ENUM_HOR_X) {
		cent = (BS_touch_info_multi[idx].mM.minX + BS_touch_info_multi[idx].mM.maxX) * 0.5f;
	}
	else {
		//(axis == ENUM_VER_Y)
		cent = (BS_touch_info_multi[idx].mM.minY + BS_touch_info_multi[idx].mM.maxY) * 0.5f;
	}

    edge = BS_is_edge(axis, cent);
	return edge;
}

static int is_exist_tp_from_edge(axis_t axis, int idx_i, int touch_info_cnt)
{
    int idx_j;
	touch_info_multi_t *nM_i = &BS_touch_info_multi[idx_i];

    for (idx_j = 0; idx_j < touch_info_cnt; idx_j++)
    {
		if (idx_i == idx_j) continue;
		touch_info_multi_t *nM_j = &BS_touch_info_multi[idx_j];
        if ((nM_j->multiStat & MULTISTAT_REMOVED_MASK) != 0) continue; //removed
#if 0 //nsmoon@200407
        IS_DEBUG_FLAG{
            DEBUG_SHOW_MIN_MAX(&nM_j->mM, MY_COLOR-3, 0); //color, mode:1(show it at debug on)
            TRACE_NOP;
        };
#endif
		if (axis == ENUM_HOR_X) {
		if ((nM_i->mM.minY >= (nM_j->mM.minY - MULTI_IS_TP_TOL) && nM_i->mM.minY <= (nM_j->mM.maxY + MULTI_IS_TP_TOL)) &&
			(nM_i->mM.maxY >= (nM_j->mM.minY - MULTI_IS_TP_TOL) && nM_i->mM.maxY <= (nM_j->mM.maxY + MULTI_IS_TP_TOL))) {
            if (IS_NEAR_BLACK_AREA_X(nM_j->mM.minX, MULTI_GHOST_MIN_DIST_SIZE_EDGE) || 
                IS_NEAR_BLACK_AREA_X(nM_j->mM.maxX, MULTI_GHOST_MIN_DIST_SIZE_EDGE)) {
			//if ((nM_j->mM.maxX < (BS_aarea_zero_x + MULTI_GHOST_MIN_DIST_SIZE_EDGE)) ||
			//	(nM_j->mM.minX > (BS_aarea_end_x - MULTI_GHOST_MIN_DIST_SIZE_EDGE))) {
				return 1;
			}
		}
	}
		else {
			//(axis == ENUM_VER_Y)
			if ((nM_i->mM.minX >= (nM_j->mM.minX - MULTI_IS_TP_TOL) && nM_i->mM.minX <= (nM_j->mM.maxX + MULTI_IS_TP_TOL)) &&
				(nM_i->mM.maxX >= (nM_j->mM.minX - MULTI_IS_TP_TOL) && nM_i->mM.maxX <= (nM_j->mM.maxX + MULTI_IS_TP_TOL))) {
                if (IS_NEAR_BLACK_AREA_Y(nM_j->mM.minY, MULTI_GHOST_MIN_DIST_SIZE_EDGE) || 
                    IS_NEAR_BLACK_AREA_Y(nM_j->mM.maxY, MULTI_GHOST_MIN_DIST_SIZE_EDGE)) {
				//if ((nM_j->mM.maxY < (BS_aarea_zero_y + MULTI_GHOST_MIN_DIST_SIZE_EDGE)) ||
				//	(nM_j->mM.minY >(BS_aarea_end_y - MULTI_GHOST_MIN_DIST_SIZE_EDGE))) {
					return 1;
				}
			}
		}
	}
	return 0;
}
#endif

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define TRACE_MIAT(...)    //TRACE(__VA_ARGS__)
#else
#define TRACE_MIAT(...)
#endif
ATTR_BACKEND_RAMFUNC
static int multi_is_adjacent_tp_surrounded(axis_t axis, int idx_i, int idx_j)
{
    float diffR, diffL;
    float width_i, width_j;
    touch_info_multi_t *nM_i = &BS_touch_info_multi[idx_i];
    touch_info_multi_t *nM_j = &BS_touch_info_multi[idx_j];
    float diffMargin = MULTI_GHOST_MIN_DIST_SURR;
    float widthRatio = MULTI_GHOST_WIDTH_RATIO;
    float sizeRatio = MULTI_GHOST_SIZE_RATIO;
	int edge_x = 0, edge_y = 0;
    int sameOppLine = 0;

    //idx_j might be smaller than idx_i

#ifdef EXTEND_EDGE_RANGE_FOR_GHOST
    edge_x = is_tp_edge(ENUM_HOR_X, idx_j);
    edge_y = is_tp_edge(ENUM_VER_Y, idx_j);
#endif
    if (edge_x || edge_y) {
        if (nM_j->multiArea < MULTI_GHOST_SIZE_MIN_PEN_EDGE) {
            diffMargin = MULTI_GHOST_MIN_DIST_SURR_EDGE; //nsmoon@200401,5f-_0130b.dlt-#51
            widthRatio = MULTI_GHOST_WIDTH_PEN_RATIO_EDGE;
            sizeRatio = MULTI_GHOST_SIZE_PEN_RATIO_EDGE;
        }
        else {
            sizeRatio = MULTI_GHOST_SIZE_RATIO_EDGE;
        }
    }

    if (axis == ENUM_HOR_X) {
        diffR = GET_ABS(nM_i->mM.minX - nM_j->mM.maxX);
        diffL = GET_ABS(nM_i->mM.maxX - nM_j->mM.minX);
        sameOppLine = (((nM_i->mM.minY > (nM_j->mM.minY - MULTI_GHOST_MIN_MAX_TOL) && nM_i->mM.minY < (nM_j->mM.maxY + MULTI_GHOST_MIN_MAX_TOL)) &&
            (nM_i->mM.maxY >(nM_j->mM.minY - MULTI_GHOST_MIN_MAX_TOL) && nM_i->mM.maxY < (nM_j->mM.maxY + MULTI_GHOST_MIN_MAX_TOL))) ||
            ((nM_j->mM.minY >(nM_i->mM.minY - MULTI_GHOST_MIN_MAX_TOL) && nM_j->mM.minY < (nM_i->mM.maxY + MULTI_GHOST_MIN_MAX_TOL)) &&
            (nM_j->mM.maxY >(nM_i->mM.minY - MULTI_GHOST_MIN_MAX_TOL) && nM_j->mM.maxY < (nM_i->mM.maxY + MULTI_GHOST_MIN_MAX_TOL))));
                }
    else {
        diffR = GET_ABS(nM_i->mM.minY - nM_j->mM.maxY);
        diffL = GET_ABS(nM_i->mM.maxY - nM_j->mM.minY);
        sameOppLine = (((nM_i->mM.minX >(nM_j->mM.minX - MULTI_GHOST_MIN_MAX_TOL) && nM_i->mM.minX < (nM_j->mM.maxX + MULTI_GHOST_MIN_MAX_TOL)) &&
            (nM_i->mM.maxX >(nM_j->mM.minX - MULTI_GHOST_MIN_MAX_TOL) && nM_i->mM.maxX < (nM_j->mM.maxX + MULTI_GHOST_MIN_MAX_TOL))) ||
            ((nM_j->mM.minX >(nM_i->mM.minX - MULTI_GHOST_MIN_MAX_TOL) && nM_j->mM.minX < (nM_i->mM.maxX + MULTI_GHOST_MIN_MAX_TOL)) &&
            (nM_j->mM.maxX >(nM_i->mM.minX - MULTI_GHOST_MIN_MAX_TOL) && nM_j->mM.maxX < (nM_i->mM.maxX + MULTI_GHOST_MIN_MAX_TOL))));
                }
    IS_DEBUG_FLAG{ TRACE_MIAT(".axis,(idx_i,idx_j)diffR,diffL: %d(%d,%d)%0.1f,%0.1f %d", axis, idx_i, idx_j, diffR, diffL, sameOppLine); };

    if (diffR < diffMargin || diffL < diffMargin) {
        IS_DEBUG_FLAG{ TRACE_MIAT(".surrounded.%d(%0.1f,%0.1f) %d(%0.1f,%0.1f)", idx_i, nM_i->mM.minY, nM_i->mM.maxY, idx_j, nM_j->mM.minY, nM_j->mM.maxY); };
        if (sameOppLine) {
            if (axis == ENUM_HOR_X) {
                width_i = (nM_i->mM.maxY - nM_i->mM.minY) * widthRatio;
                width_j = nM_j->mM.maxY - nM_j->mM.minY;
            }
            else {
                width_i = (nM_i->mM.maxX - nM_i->mM.minX) * widthRatio;
                width_j = nM_j->mM.maxX - nM_j->mM.minX;
            }
            if (nM_i->multiArea < TOUCH_FINGER_SIZESQ) {
                IS_DEBUG_FLAG{ TRACE_MIAT(".marker: %0.1f", nM_i->multiArea); };
                return 1; //found
            }
            IS_DEBUG_FLAG{ TRACE_MIAT(".width: %0.1f (%0.1f)", width_i, width_j); };
            if (width_j < width_i)
            {
                IS_DEBUG_FLAG{ TRACE_MIAT("  =.=>%d", idx_j); };
                return 1; //found
            }
            width_i = nM_i->multiArea * sizeRatio;
            width_j = nM_j->multiArea;
            IS_DEBUG_FLAG{ TRACE_MIAT(".area: %0.1f (%0.1f)", width_i, width_j); };
            if (width_j < width_i)
            {
                IS_DEBUG_FLAG{ TRACE_MIAT(".=>%d", idx_j); };
                return 1; //found
            }
        }
    } //if (diffR < diffMargin || diffL < diffMargin)

    return 0; //not-found
}

#define MULTI_GHOST_MIN_MAX_TOL_SIZE	EPSILON
#define MULTI_GHOST_CENTER_TOL_SIZE     3.0f //1.0f //nsmoon@200406 1.0f=>3.0f
ATTR_BACKEND_RAMFUNC
static int multi_is_adjacent_tp_size(int idx_i, int idx_j, int touch_info_cnt)
{
    //idx_i shoud be less than idx_j
    float diffR, diffL, diffCent;
    touch_info_multi_t *nM_i = &BS_touch_info_multi[idx_i];
    touch_info_multi_t *nM_j = &BS_touch_info_multi[idx_j];
    int tmpRet = 0;
	float tmpRatio, tmpLimit = 0, diffMargin;
    float min_dist_size_near = MULTI_GHOST_MIN_DIST_SIZE_NEAR;
    int isEdgeX = 0, isEdgeY = 0;
    IS_DEBUG_FLAG {
        TRACE_NOP;
    };

    float area_i = nM_i->multiArea;
    float area_j = nM_j->multiArea;

#ifdef EXTEND_EDGE_RANGE_FOR_GHOST
    isEdgeX = (is_tp_edge(ENUM_HOR_X, idx_i) || is_tp_edge(ENUM_HOR_X, idx_j));
    isEdgeY = (is_tp_edge(ENUM_VER_Y, idx_i) || is_tp_edge(ENUM_VER_Y, idx_j));
    if (isEdgeX || isEdgeY) {
        min_dist_size_near = MULTI_GHOST_MIN_DIST_SIZE_NEAR_EDGE;
    }
#endif

    //check dist of x-axis, which is same on y-axis
    //if (((nM_i->mM.minY > (nM_j->mM.minY - MULTI_GHOST_MIN_MAX_TOL_SIZE) && nM_i->mM.minY < (nM_j->mM.maxY + MULTI_GHOST_MIN_MAX_TOL_SIZE)) &&
    //     (nM_i->mM.maxY > (nM_j->mM.minY - MULTI_GHOST_MIN_MAX_TOL_SIZE) && nM_i->mM.maxY < (nM_j->mM.maxY + MULTI_GHOST_MIN_MAX_TOL_SIZE))))
    diffCent = GET_ABS(nM_i->centerPos.y - nM_j->centerPos.y);
    //IS_DEBUG_FLAG{ TRACE_MIAT(".diffCent: %0.1f", diffCent); };
    if (diffCent < MULTI_GHOST_CENTER_TOL_SIZE) //nsmoon@200309
    {
		//included nM_i in nM_j, same y-axis
        diffR = GET_ABS(nM_i->mM.minX - nM_j->mM.maxX);
        diffL = GET_ABS(nM_i->mM.maxX - nM_j->mM.minX);
        IS_DEBUG_FLAG{ TRACE_MIAT("..: %d/%d %0.1f/%0.1f %0.1f/%0.1f (%d/%d)", idx_i, idx_j, nM_i->multiArea, nM_j->multiArea, diffR, diffL, isEdgeX, isEdgeY);}
        if (diffR < min_dist_size_near || diffL < min_dist_size_near) {
            IS_DEBUG_FLAG{ TRACE_MIAT(".x1: %d/%d %0.1f/%0.1f %0.1f/%0.1f (%d/%d)", idx_i, idx_j, nM_i->multiArea, nM_j->multiArea, diffR, diffL, isEdgeX, isEdgeY);}
            tmpRet = 1; //found i
        }
    }

    if (tmpRet == 0) {
        //check dist of y-axis, which is same on x-axis
        //if (((nM_i->mM.minX >(nM_j->mM.minX - MULTI_GHOST_MIN_MAX_TOL_SIZE) && nM_i->mM.minX < (nM_j->mM.maxX + MULTI_GHOST_MIN_MAX_TOL_SIZE)) &&
        //     (nM_i->mM.maxX >(nM_j->mM.minX - MULTI_GHOST_MIN_MAX_TOL_SIZE) && nM_i->mM.maxX < (nM_j->mM.maxX + MULTI_GHOST_MIN_MAX_TOL_SIZE))))
        diffCent = GET_ABS(nM_i->centerPos.x - nM_j->centerPos.x);
        if (diffCent < MULTI_GHOST_CENTER_TOL_SIZE) //nsmoon@200309
        {
            //included nM_i in nM_j, same x-axis
            diffR = GET_ABS(nM_i->mM.minY - nM_j->mM.maxY);
            diffL = GET_ABS(nM_i->mM.maxY - nM_j->mM.minY);
            if (diffR < min_dist_size_near || diffL < min_dist_size_near) {
                IS_DEBUG_FLAG{ TRACE_MIAT(".y1: %d/%d %0.1f/%0.1f %0.1f/%0.1f (%d/%d)", idx_i, idx_j, nM_i->multiArea, nM_j->multiArea, diffR, diffL, isEdgeX, isEdgeY);}
                tmpRet = 1; //found i
            }
        }
    }

	if (tmpRet == 0) {
        //check overlap min/max of y-axis
        if (((nM_i->mM.minY > (nM_j->mM.minY - MULTI_GHOST_MIN_DIST_SIZE_NEAR) && nM_i->mM.minY < (nM_j->mM.maxY + MULTI_GHOST_MIN_DIST_SIZE_NEAR)) ||
             (nM_i->mM.maxY > (nM_j->mM.minY - MULTI_GHOST_MIN_DIST_SIZE_NEAR) && nM_i->mM.maxY < (nM_j->mM.maxY + MULTI_GHOST_MIN_DIST_SIZE_NEAR))) ||
                ((nM_j->mM.minY > (nM_i->mM.minY - MULTI_GHOST_MIN_DIST_SIZE_NEAR) && nM_j->mM.minY < (nM_i->mM.maxY + MULTI_GHOST_MIN_DIST_SIZE_NEAR)) ||
                 (nM_j->mM.maxY > (nM_i->mM.minY - MULTI_GHOST_MIN_DIST_SIZE_NEAR) && nM_j->mM.maxY < (nM_i->mM.maxY + MULTI_GHOST_MIN_DIST_SIZE_NEAR)))) {
		//same y-axis, overlap
#ifdef EXTEND_EDGE_RANGE_FOR_GHOST
            if (isEdgeY) {
                int tmpIdx = (area_i < area_j) ? idx_i : idx_j;
                isEdgeY = is_exist_tp_from_edge(ENUM_VER_Y, tmpIdx, touch_info_cnt);
            }
#endif
			diffMargin = (isEdgeY) ? MULTI_GHOST_MIN_DIST_SIZE_EDGE : MULTI_GHOST_MIN_DIST_SIZE_FAR;
            diffR = GET_ABS(nM_i->mM.minX - nM_j->mM.maxX);
            diffL = GET_ABS(nM_i->mM.maxX - nM_j->mM.minX);
            IS_DEBUG_FLAG{ TRACE_MIAT(".x: %d(%0.1f) %d(%0.1f)", idx_i, diffR, idx_j, diffL); };
            if (diffR < MULTI_GHOST_MIN_DIST_SIZE_MID || diffL < MULTI_GHOST_MIN_DIST_SIZE_MID) {
                IS_DEBUG_FLAG{ TRACE_MIAT(".x2: %d/%d %0.1f/%0.1f %0.1f/%0.1f (%d/%d)", idx_i, idx_j, nM_i->multiArea, nM_j->multiArea, diffR, diffL, isEdgeX, isEdgeY);}
                tmpRet = 2; //nsmoon@190129
            }
            else if (diffR < diffMargin || diffL < diffMargin) {
                IS_DEBUG_FLAG{ TRACE_MIAT(".x3: %d/%d %0.1f/%0.1f %0.1f/%0.1f (%d/%d)", idx_i, idx_j, nM_i->multiArea, nM_j->multiArea, diffR, diffL, isEdgeX, isEdgeY);}
                tmpRet = 3; //found i
            }
        }
    }

    if (tmpRet == 0) {
        //check overlap min/max of x-axis
        if (((nM_i->mM.minX > (nM_j->mM.minX - MULTI_GHOST_MIN_DIST_SIZE_NEAR) && nM_i->mM.minX < (nM_j->mM.maxX + MULTI_GHOST_MIN_DIST_SIZE_NEAR)) ||
             (nM_i->mM.maxX > (nM_j->mM.minX - MULTI_GHOST_MIN_DIST_SIZE_NEAR) && nM_i->mM.maxX < (nM_j->mM.maxX + MULTI_GHOST_MIN_DIST_SIZE_NEAR))) ||
                ((nM_j->mM.minX > (nM_i->mM.minX - MULTI_GHOST_MIN_DIST_SIZE_NEAR) && nM_j->mM.minX < (nM_i->mM.maxX + MULTI_GHOST_MIN_DIST_SIZE_NEAR)) ||
                 (nM_j->mM.maxX > (nM_i->mM.minX - MULTI_GHOST_MIN_DIST_SIZE_NEAR) && nM_j->mM.maxX < (nM_i->mM.maxX + MULTI_GHOST_MIN_DIST_SIZE_NEAR)))) {
            //nM_i->multiStat = 2; nM_j->multiStat = 2; //same x-axis, overlap
#ifdef EXTEND_EDGE_RANGE_FOR_GHOST
            if (isEdgeX) {
                int tmpIdx = (area_i < area_j) ? idx_i : idx_j;
                isEdgeX = is_exist_tp_from_edge(ENUM_HOR_X, tmpIdx, touch_info_cnt);
            }
#endif
            diffMargin = (isEdgeX) ? MULTI_GHOST_MIN_DIST_SIZE_EDGE : MULTI_GHOST_MIN_DIST_SIZE_FAR;
            diffR = GET_ABS(nM_i->mM.minY - nM_j->mM.maxY);
            diffL = GET_ABS(nM_i->mM.maxY - nM_j->mM.minY);
            IS_DEBUG_FLAG{ TRACE_MIAT(".y: %d(%0.1f) %d(%0.1f)", idx_i, diffR, idx_j, diffL); };
            if (diffR < MULTI_GHOST_MIN_DIST_SIZE_MID || diffL < MULTI_GHOST_MIN_DIST_SIZE_MID) {
                IS_DEBUG_FLAG{ TRACE_MIAT(".y2: %d/%d %0.1f/%0.1f %0.1f/%0.1f (%d/%d)", idx_i, idx_j, nM_i->multiArea, nM_j->multiArea, diffR, diffL, isEdgeX, isEdgeY);}
                tmpRet = 2; //nsmoon@190129
            }
            else if (diffR < diffMargin || diffL < diffMargin) {
                IS_DEBUG_FLAG{ TRACE_MIAT(".y3: %d/%d %0.1f/%0.1f %0.1f/%0.1f (%d/%d)", idx_i, idx_j, nM_i->multiArea, nM_j->multiArea, diffR, diffL, isEdgeX, isEdgeY);}
                tmpRet = 3; //found i
            }
        }
    }

    if (tmpRet != 0) {
    if (tmpRet == 1) {
        tmpRatio = (isEdgeX || isEdgeY) ? MULTI_GHOST_NEAR_SIZE_RATIO_EDGE : MULTI_GHOST_NEAR_SIZE_RATIO;
    }
    else if (tmpRet == 2) {
		tmpRatio = (isEdgeX || isEdgeY) ? MULTI_GHOST_MID_SIZE_RATIO_EDGE : MULTI_GHOST_MID_SIZE_RATIO;
    }
        else /*if (tmpRet == 3)*/ {
		tmpRatio = (isEdgeX || isEdgeY) ? MULTI_GHOST_FAR_SIZE_RATIO_EDGE : MULTI_GHOST_FAR_SIZE_RATIO;
        }
        if (area_i < area_j) {
            tmpLimit = area_j * tmpRatio;
            if (area_i < tmpLimit) {
                return idx_i; //remove i
            }
        }
        else {
            tmpLimit = area_i * tmpRatio;
            if (area_j < tmpLimit) {
                return idx_j; //remove j
            }
        }
    }

    return -1; //do-nothing
}

#if (DEAD_CELL_TBL == 1) //nsmoon@191205
ATTR_BACKEND_RAMFUNC
static int multi_is_adjacent_deadcell(int idx_i, int idx_j)
{
	float diffR = 0, diffL = 0;
    float cent_i, cent_j;
	touch_info_multi_t *nM_i = &BS_touch_info_multi[idx_i];
	touch_info_multi_t *nM_j = &BS_touch_info_multi[idx_j];
	int isEdge = 0;
	axis_t axis = ENUM_AXIS_END;

	//x-axis
	if (((nM_i->mM.minY >= (nM_j->mM.minY - MULTI_GHOST_MIN_DIST_SIZE_NEAR) && nM_i->mM.minY <= (nM_j->mM.maxY + MULTI_GHOST_MIN_DIST_SIZE_NEAR)) &&
		(nM_i->mM.maxY >= (nM_j->mM.minY - MULTI_GHOST_MIN_DIST_SIZE_NEAR) && nM_i->mM.maxY <= (nM_j->mM.maxY + MULTI_GHOST_MIN_DIST_SIZE_NEAR)))) {
		diffR = GET_ABS(nM_i->mM.minX - nM_j->mM.maxX);
		diffL = GET_ABS(nM_i->mM.maxX - nM_j->mM.minX);
		cent_i = (nM_i->mM.minX + nM_i->mM.maxX) * 0.5f;
		cent_j = (nM_j->mM.minX + nM_j->mM.maxX) * 0.5f;
		isEdge = is_tp_edge(ENUM_VER_Y, idx_i);
		axis = ENUM_HOR_X;
	}

	//y-axis
	if (isEdge == 0) {
		if (((nM_i->mM.minX >= (nM_j->mM.minX - MULTI_GHOST_MIN_DIST_SIZE_NEAR) && nM_i->mM.minX <= (nM_j->mM.maxX + MULTI_GHOST_MIN_DIST_SIZE_NEAR)) &&
			(nM_i->mM.maxX >= (nM_j->mM.minX - MULTI_GHOST_MIN_DIST_SIZE_NEAR) && nM_i->mM.maxX <= (nM_j->mM.maxX + MULTI_GHOST_MIN_DIST_SIZE_NEAR)))) {
			diffR = GET_ABS(nM_i->mM.minY - nM_j->mM.maxY);
			diffL = GET_ABS(nM_i->mM.maxY - nM_j->mM.minY);
			cent_i = (nM_i->mM.minY + nM_i->mM.maxY) * 0.5f;
			cent_j = (nM_j->mM.minY + nM_j->mM.maxY) * 0.5f;
			isEdge = is_tp_edge(ENUM_HOR_X, idx_i);
			axis = ENUM_VER_Y;
		}
	}

    IS_DEBUG_FLAG{ TRACE_MIAT(".dead.%d(%0.1f/%0.1f) %d(%0.1f/%0.1f)", idx_i, diffR, diffL, idx_j, nM_i->multiArea, nM_j->multiArea); };
    if (isEdge == EDGE_NEAR_LED_X || isEdge == EDGE_NEAR_LED_Y) {
		//near led-side
		if (diffR < MULTI_GHOST_MIN_DIST_SIZE_EDGE || diffL < MULTI_GHOST_MIN_DIST_SIZE_EDGE) {
            if (axis != ENUM_AXIS_END) {
			if (BS_is_dead_cell_pos(axis, ENUM_LED, &cent_i)) {
                return idx_i; //found i
			}
		}
	}
	}
    else if (isEdge == EDGE_NEAR_PD_X || isEdge == EDGE_NEAR_PD_Y) {
		//near pd-side
		if (diffR < MULTI_GHOST_MIN_DIST_SIZE_EDGE || diffL < MULTI_GHOST_MIN_DIST_SIZE_EDGE) {
            if (axis != ENUM_AXIS_END) {
			if (BS_is_dead_cell_pos(axis, ENUM_PD, &cent_i)) {
                return idx_i; //found i
			}
		}
	}
	}
    else if (isEdge == EDGE_INNER_LED_X || isEdge == EDGE_INNER_LED_Y) {
		// inner led-side
		if (diffR < MULTI_GHOST_MIN_DIST_SIZE_FAR || diffL < MULTI_GHOST_MIN_DIST_SIZE_FAR) {
            if (axis != ENUM_AXIS_END) {
			if (BS_is_dead_cell_pos(axis, ENUM_LED, &cent_j)) {
                return idx_j; //found j
			}
			else {
                return idx_i; //found i
			}
		}
	}
	}
    else if (isEdge == EDGE_INNER_PD_X || isEdge == EDGE_INNER_PD_Y) {
		// inner pd-side
		if (diffR < MULTI_GHOST_MIN_DIST_SIZE_FAR || diffL < MULTI_GHOST_MIN_DIST_SIZE_FAR) {
            if (axis != ENUM_AXIS_END) {
			if (BS_is_dead_cell_pos(axis, ENUM_PD, &cent_j)) {
                return idx_j; //found j
			}
			else {
                return idx_i; //found i
			}
		}
	}
	}

    return -1;
}
#endif

static int multi_get_clipping_info(axis_t axis, bd_type_t bdX, bd_type_t bdY, int sideIdx, int polyIdx, initial_polygon_t *initial_polygon)
{
    clipping_range_info_t *clipping_info = &BS_clipping_range_info[(int)axis];
	bd_type_t bd, bd2;
	sen_type_t sensor, sensor2;
	float *senPosX = 0, *senPosY = 0;
	float *senPos2 = 0; //opp sensor
	axis_t axis2; //opp axis

	if (axis == ENUM_HOR_X) {
		axis2 = ENUM_VER_Y;
		bd = bdX;
		bd2 = bdY;
	}
	else {
		//(axis == ENUM_VER_Y)
		axis2 = ENUM_HOR_X;
		bd = bdY;
		bd2 = bdX;
	}

	if (bd == ENUM_BOT_BD || bd == ENUM_LFT_BD) {
		sensor = ENUM_LED;
		if (axis == ENUM_HOR_X) {
			senPosX = &BS_led_pos_x[0];
			senPos2 = &BS_pd_pos_x[0];
		}
		else {
			senPosY = &BS_led_pos_y[0];
			senPos2 = &BS_pd_pos_y[0];
		}
	}
	else {
		sensor = ENUM_PD;
		if (axis == ENUM_HOR_X) {
			senPosX = &BS_pd_pos_x[0];
			senPos2 = &BS_led_pos_x[0];
		}
		else {
			senPosY = &BS_pd_pos_y[0];
			senPos2 = &BS_led_pos_y[0];
		}
	}

	if (bd2 == ENUM_BOT_BD || bd2 == ENUM_LFT_BD) {
		sensor2 = ENUM_LED; //opp axis
		if (axis2 == ENUM_HOR_X) {
			senPosX = &BS_led_pos_x[0];
		}
		else {
			senPosY = &BS_led_pos_y[0];
		}
	}
	else {
		sensor2 = ENUM_PD;
		if (axis2 == ENUM_HOR_X) {
			senPosX = &BS_pd_pos_x[0];
		}
		else {
			senPosY = &BS_pd_pos_y[0];
		}
	}

	clipping_info->bd = bd;
	clipping_info->bd2 = bd2;
	clipping_info->sensor = sensor;
	clipping_info->sensor2 = sensor2; //opp axis pos
	clipping_info->senPosX = senPosX;
	clipping_info->senPosY = senPosY;
	clipping_info->senPos2 = senPos2; //opp sensor pos
	clipping_info->sideIdx = sideIdx;
	clipping_info->polyIdx = polyIdx;

#ifdef MULTI_CLIPPING_RANGE //nsmoon@190702
    if (initial_polygon > 0) {
        ep_min_max_t minMaxEp;
        pos_min_max_t minMaxPos;
        int minSenlim, maxSenlim;
        minMaxEp.minX = initial_polygon->ep.minX;
        minMaxEp.maxX = initial_polygon->ep.maxX;
        minMaxEp.minY = initial_polygon->ep.minY;
        minMaxEp.maxY = initial_polygon->ep.maxY;
        minMaxPos.minX = initial_polygon->pos.minX;
        minMaxPos.maxX = initial_polygon->pos.maxX;
        minMaxPos.minY = initial_polygon->pos.minY;
        minMaxPos.maxY = initial_polygon->pos.maxY;
        BS_get_clipping_range(axis, &minMaxEp, &minMaxPos, &minSenlim, &maxSenlim);
        clipping_info->minSenlim = (uint16_t)minSenlim;
        clipping_info->maxSenlim = (uint16_t)maxSenlim;
        //clipping_info->centerSen = GET_LINE_CENT_INT(minSenlim, maxSenlim);
        //IS_DEBUG_FLAG2{ TRACE_RNG("center,minSenlim,maxSenlim: %d (%d %d)", GET_LINE_CENT_INT(minSenlim, maxSenlim), minSenlim, maxSenlim);};
    }
#endif

	return 0;
}

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_BG_clipping_multi		1	//0:disable, 1:
#endif
#if (DEBUG_BG_clipping_multi > 0)
#define TRACE_CM(...)	TRACE(__VA_ARGS__)
#define TRACENR_CM(...)	//TRACENR(__VA_ARGS__)
#define TRACE_CM2(...)  //TRACE(__VA_ARGS__)
#else
#define TRACE_CM(...)
#define TRACENR_CM(...)
#define TRACE_CM2(...)
#endif

#if 0 //reserved, do not remove
static int multi_is_line_cross_tp(axis_t axis, touch_info_multi_t *touch_info_multi, int pd, int led)
{
    vec_t p0, p1, p2, p3, pR;
    float *ledPos, *pdPos;
    if (axis == ENUM_HOR_X) {
        ledPos = &BS_led_pos_x[0];
        pdPos = &BS_pd_pos_x[0];
        p0.x = ledPos[led];
        p1.x = pdPos[pd];
        p0.y = BS_sensor_zero_y;
        p1.y = BS_sensor_end_y;
        p2.x = touch_info_multi->mM.minX;
        p3.x = touch_info_multi->mM.maxX;
        p2.y = (touch_info_multi->mM.minY + touch_info_multi->mM.maxY) * 0.5f;
        p3.y = p2.y;
    }
    else {
        ledPos = &BS_led_pos_y[0];
        pdPos = &BS_pd_pos_y[0];
        p0.y = ledPos[led];
        p1.y = pdPos[pd];
        p0.x = BS_sensor_end_x;
        p1.x = BS_sensor_zero_x;
        p2.y = touch_info_multi->mM.minY;
        p3.y = touch_info_multi->mM.maxY;
        p2.x = (touch_info_multi->mM.minX + touch_info_multi->mM.maxX) * 0.5f;
        p3.x = p2.x;
    }
    if (BS_line_intersection(&p0, &p1, &p2, &p3, &pR)) {
        return 1; //cross
    }
    return 0; //not-cross
}
#endif//1

static int multi_recal_used_line(axis_t axis, touch_info_multi_t *touch_info_multi, int calTh10)
{
	ep_min_max_t minMaxEp;
	pos_min_max_t minMaxPos, minMaxPos2;
	sen_type_t sensor, sensor2;
	axis_t axis2;
	int minSenlim, maxSenlim;
	int senStOp, senEdOp;
	int pd, led;
	int pd2, led2;
	int slopeVal;
#ifdef FRONTEND_LINE_THRESHOLD //nsmoon@191226
	int ofstIdx, slopeValMax;
	uint8_t th50Cnt;
	uint8_t th10Cnt;
#endif
    int maxSlopeVal, adjVal;
    //int maxPdNum;
    //int tmpCnt = 0; //nsmoon@211223
	IS_DEBUG_FLAG{
		TRACE_NOP;
	};

		if (axis == ENUM_HOR_X) {
			//maxPdNum = BS_maxHorPdNum - 1;
#ifdef FRONTEND_LINE_THRESHOLD //nsmoon@191226
			slopeValMax = BS_slopeValMaxX;
#endif
			axis2 = ENUM_VER_Y;
		}
		else {
			//(axis == ENUM_VER_Y)
			//maxPdNum = BS_maxVerPdNum - 1;
#ifdef FRONTEND_LINE_THRESHOLD //nsmoon@191226
			slopeValMax = BS_slopeValMaxY;
#endif
			axis2 = ENUM_HOR_X;
		}
        multi_get_clipping_info(axis, touch_info_multi->bdX, touch_info_multi->bdY, 0, 0, 0);
		sensor = BS_clipping_range_info[(int)axis].sensor;
		sensor2 = BS_clipping_range_info[(int)axis].sensor2; //opp axis's sensor

#if ((DEBUG_BG_clipping_multi > 0) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)) && defined(DRAW_POLYGON_TEST)
    if (calTh10) {
    	DEBUG_SHOW_MIN_MAX(&touch_info_multi->mM, 0, 0); //color, mode:1(show it at debug on)
    }
#endif

        //get min/max position
		minMaxPos.minX = touch_info_multi->mM.minX;
		minMaxPos.maxX = touch_info_multi->mM.maxX;
		minMaxPos.minY = touch_info_multi->mM.minY;
		minMaxPos.maxY = touch_info_multi->mM.maxY;
#if (BRUSH_MODE_ENABLE == 1)
        if (BS_brushMode) {
            minMaxPos.minX -= LARGE_TOUCH_GAP_BRUSH;
            minMaxPos.maxX += LARGE_TOUCH_GAP_BRUSH;
            minMaxPos.minX = GET_MAX(minMaxPos.minX, 0);
            minMaxPos.maxX = GET_MIN(minMaxPos.maxX, BS_sensor_end_x);
            minMaxPos.minY -= LARGE_TOUCH_GAP_BRUSH;
            minMaxPos.maxY += LARGE_TOUCH_GAP_BRUSH;
            minMaxPos.minY = GET_MAX(minMaxPos.minY, 0);
            minMaxPos.maxY = GET_MIN(minMaxPos.maxY, BS_sensor_end_y);
        }
#endif

		//get sensor number
		if (axis == ENUM_HOR_X) {
        minMaxEp.minX = (uint8_t)BS_getSenNumPos(axis, sensor, minMaxPos.minX, 0); //0:before 
        minMaxEp.maxX = (uint8_t)BS_getSenNumPos(axis, sensor, minMaxPos.maxX, 1); //1:after
        minMaxEp.minY = (uint8_t)BS_getSenNumPos(axis2, sensor2, minMaxPos.minY, 0); //0:before
        minMaxEp.maxY = (uint8_t)BS_getSenNumPos(axis2, sensor2, minMaxPos.maxY, 1); //1:after
		}
		else {
        minMaxEp.minY = (uint8_t)BS_getSenNumPos(axis, sensor, minMaxPos.minY, 0); //0:before
        minMaxEp.maxY = (uint8_t)BS_getSenNumPos(axis, sensor, minMaxPos.maxY, 1); //1:after
        minMaxEp.minX = (uint8_t)BS_getSenNumPos(axis2, sensor2, minMaxPos.minX, 0); //0:before 
        minMaxEp.maxX = (uint8_t)BS_getSenNumPos(axis2, sensor2, minMaxPos.maxX, 1); //1:after
		}
#if 0 //for test
		TRACE("minMaxEp: %d %d %d %d", minMaxEp.minX, minMaxEp.maxX, minMaxEp.minY, minMaxEp.maxY);
		if (axis == ENUM_HOR_X) {
			minMaxEp.minX = BS_getSenNumPos(axis, sensor, minMaxPos.minX, 2);
			minMaxEp.maxX = BS_getSenNumPos(axis, sensor, minMaxPos.maxX, 2);
			minMaxEp.minY = BS_getSenNumPos(axis2, sensor2, minMaxPos.minY, 2);
			minMaxEp.maxY = BS_getSenNumPos(axis2, sensor2, minMaxPos.maxY, 2);
		}
		else {
			minMaxEp.minY = BS_getSenNumPos(axis, sensor, minMaxPos.minY, 2);
			minMaxEp.maxY = BS_getSenNumPos(axis, sensor, minMaxPos.maxY, 2);
			minMaxEp.minX = BS_getSenNumPos(axis2, sensor2, minMaxPos.minX, 2);
			minMaxEp.maxX = BS_getSenNumPos(axis2, sensor2, minMaxPos.maxX, 2);
		}
		TRACE("=>minMaxEp: %d %d %d %d", minMaxEp.minX, minMaxEp.maxX, minMaxEp.minY, minMaxEp.maxY);
#endif
		IS_DEBUG_FLAG{
            if (calTh10) {
			TRACE_NOP;
		}
        };

    BS_get_clipping_range(axis, &minMaxEp, &minMaxPos, &minSenlim, &maxSenlim);

		//save sensor range
		if (axis == ENUM_HOR_X) {
        touch_info_multi->sRx.right = (uint8_t)minSenlim;
        touch_info_multi->sRx.left = (uint8_t)maxSenlim;
        maxSlopeVal= BS_slopeValMaxX;
    }
    else {
        touch_info_multi->sRy.right = (uint8_t)minSenlim;
        touch_info_multi->sRy.left = (uint8_t)maxSenlim;
        maxSlopeVal = BS_slopeValMaxY;
    }
    minMaxPos2.minX = MIN_INITIAL_VAL;
    minMaxPos2.maxX = 0;
        minMaxPos2.minY = MIN_INITIAL_VAL;
        minMaxPos2.maxY = 0;

#ifdef FRONTEND_LINE_THRESHOLD //nsmoon@200119
		th50Cnt = 0;
		th10Cnt = 0;
#endif
#if 0 //for test  //nsmoon@211223
            IS_DEBUG_FLAG2{
            if (axis == ENUM_VER_Y) {
                DEBUG_SHOW_MIN_MAX(&minMaxPos, 1, 0); //color, mode:1(show it at debug on)
            }
        };
#endif
        for (pd = minSenlim; pd <= maxSenlim; pd++) {
#if 0 //for test
            IS_DEBUG_FLAG2{
                TRACE_POLYGON_MULTI(0, 0x21, 0);
                DEBUG_SHOW_MIN_MAX(&minMaxPos, 1, 0); //color, mode:1(show it at debug on)
            };
#endif
            senStOp = BS_get_virtual_range(axis, sensor, &minMaxEp, &minMaxPos, pd, 0); //start
            slopeVal = GET_ABS(senStOp - pd);
            if (slopeVal > maxSlopeVal) {
                //must check maxSlope before using
                adjVal = slopeVal - maxSlopeVal;
                senStOp = (senStOp > pd) ? senStOp - adjVal : senStOp + adjVal;
            }

            senEdOp = BS_get_virtual_range(axis, sensor, &minMaxEp, &minMaxPos, pd, 1); //end
            slopeVal = GET_ABS(senEdOp - pd);
            if (slopeVal > maxSlopeVal) {
                //must check maxSlope before using
                adjVal = slopeVal - maxSlopeVal;
                senEdOp = (senEdOp > pd) ? senEdOp - adjVal : senEdOp + adjVal;
            }
#if 0 //for test
            IS_DEBUG_FLAG{
                if (calTh10) {
                    if (pd == minSenlim || pd == maxSenlim) {
                        if (sensor == ENUM_PD) {
                            pd2 = pd;
                            led2 = senStOp;
                        }
                        else {
                            pd2 = senStOp;
                            led2 = pd;
                        }
                        DEBUG_SHOW_LINE_PD_LED(axis, pd2, led2, MY_COLOR - 2);
                        if (sensor == ENUM_PD) {
                            pd2 = pd;
                            led2 = senEdOp;
                        }
                        else {
                            pd2 = senEdOp;
                            led2 = pd;
                        }
                        DEBUG_SHOW_LINE_PD_LED(axis, pd2, led2, MY_COLOR - 2);
                        TRACE_NOP;
                    }
                }
            }
#endif
#if 0 //for test //nsmoon@211223
            if (axis == ENUM_VER_Y) {
                    TRACE("pd,senStOp,senEdOp= %d %d %d", pd, senStOp, senEdOp);
                    DEBUG_SHOW_LINE_PD_LED(axis, pd, senStOp, MY_COLOR - 2);
                    DEBUG_SHOW_LINE_PD_LED(axis, pd, senEdOp, MY_COLOR - 2);
                    TRACE_NOP;
            }
#endif
            for (led = senStOp; led <= senEdOp; led++) {
                if (sensor == ENUM_PD) {
                    pd2 = pd;
                    led2 = led;
                }
                else {
                    pd2 = led;
                    led2 = pd;
                }
#if 0 //for test
                if (calTh10) {
                    if (led == senStOp || led == senEdOp) {
                        if (multi_is_line_cross_tp(axis, touch_info_multi, pd2, led2) == 0) {
                            //DEBUG_SHOW_LINE_PD_LED(axis, pd2, led2, MY_COLOR-2);
                            continue;
                        }
                    }
                }
#endif
                //tmpCnt++;  //nsmoon@211223
                    BS_set_unset2_used_led_pd(axis, led2, pd2, 1); //1:set
#ifdef FRONTEND_LINE_THRESHOLD //nsmoon@200119
                    if (calTh10) {
                        if (BS_is_set_line_buffer(axis, pd2, led2)) {
                            //DEBUG_SHOW_LINE_PD_LED(axis, pd2, led2, MY_COLOR-3);
                            if (th50Cnt < UINT8_MAX) {
                                th50Cnt++; //max:UINT8_MAX
                            }
                        } //if (BS_is_set_line_buffer(axis, pd2, led2))

                        slopeVal = led2 - pd2;
                        ofstIdx = slopeVal + slopeValMax;
                        if (BS_is_set_threshold(axis, pd2, ofstIdx)) {
                            BS_calMinMaxSen(axis, led2, pd2, &minMaxPos, &minMaxPos2);
                            if (th10Cnt < UINT8_MAX) {
                                th10Cnt++;
                            }
                        }
                    }
#endif
            } //for (led = senStOp; led <= senEdOp; led++)
        } //for (pd = minSenlim; pd <= maxSenlim; pd++)

#if 1 //nsmoon@200119
    if (calTh10) {
        if (axis == ENUM_HOR_X) {
            if (minMaxPos2.minX == MIN_INITIAL_VAL) {
                minMaxPos2.minX = 0.0f; //nsmoon@200302
            }
#ifdef FRONTEND_LINE_THRESHOLD
#if ((DEBUG_BG_clipping_multi > 0) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)) && defined(DRAW_POLYGON_TEST)
            //TRACE("(M)th50CntX,th10CntX= %d,%d (%0.1f,%0.1f)", th50Cnt, th10Cnt, minMaxPos.maxX-minMaxPos.minX, minMaxPos2.maxX-minMaxPos2.minX);
#endif
            touch_info_multi->th50CntX = th50Cnt;
            touch_info_multi->th10CntX = th10Cnt;
            touch_info_multi->mM2.minX = minMaxPos2.minX;
            touch_info_multi->mM2.maxX = minMaxPos2.maxX;
#endif
        }
        else {
            if (minMaxPos2.minY == MIN_INITIAL_VAL) {
                minMaxPos2.minY = 0.0f; //nsmoon@200302
            }
#ifdef FRONTEND_LINE_THRESHOLD
#if ((DEBUG_BG_clipping_multi > 0) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)) && defined(DRAW_POLYGON_TEST)
            //TRACE("(M)th50CntY,th10CntY= %d,%d (%0.1f,%0.1f)", th50Cnt, th10Cnt, minMaxPos.maxY-minMaxPos.minY, minMaxPos2.maxY-minMaxPos2.minY);
#endif
            touch_info_multi->th50CntY = th50Cnt;
            touch_info_multi->th10CntY = th10Cnt;
            touch_info_multi->mM2.minY = minMaxPos2.minY;
            touch_info_multi->mM2.maxY = minMaxPos2.maxY;
#endif
        }
    }
#endif //1
    //TRACE("tmpCnt= %d", tmpCnt);  //nsmoon@211223
    return 0;
}

#if 0 //reserved, do not remove
static int count_touch_info(touch_info_multi_t *touch_info_multi, uint8_t *touch_info_multi_sort, int tpCntSort)
{
    //cal remained touch count
    int sortIdx, idx_i, tmpCnt = 0;
    for (sortIdx = 0; sortIdx < tpCntSort; sortIdx++) {
        idx_i = touch_info_multi_sort[sortIdx];
        if ((touch_info_multi[idx_i].multiStat & MULTISTAT_REMOVED_MASK) != 0) continue; //removed
        tmpCnt++;
    }
    return tmpCnt;
}
#endif

#if 1 //for test
static int count_touch_info2(touch_info_multi_t *touch_info_multi, int startIdx, int endIdx)
{
    //cal remained touch count
    int i, tmpCnt = 0;
    for (i = startIdx; i < endIdx; i++) {
        if ((touch_info_multi[i].multiStat & MULTISTAT_REMOVED_MASK) != 0) continue; //removed
        tmpCnt++;
    }
    return tmpCnt;
}
#endif

#if 1 //nsmoon@200330
static int touch_info_sort(int touch_info_start, int touch_info_cnt)
{
    int tpCntSort = 0;
    int i, j;
    for (i = touch_info_start; i < touch_info_cnt; i++) {
        if ((BS_touch_info_multi[i].multiStat & MULTISTAT_REMOVED_MASK) != 0) {
            continue; //removed
        }
#if (DEBUG_BG_clipping_multi > 1) //for testing
        IS_DEBUG_FLAG{ TRACE_CM2(" %d:(0x%x)%d %0.1f", i, BS_touch_info_multi[i].multiStat, BS_touch_info_multi[i].polLen, BS_touch_info_multi[i].multiArea); };
#endif
        BS_touch_info_multi_sort[tpCntSort++] = (uint8_t)i; //init
    }

    //sort touch info
    for (i = 0; i < (tpCntSort - 1); i++) {
        for (j = (i + 1); j < tpCntSort; j++) {
            int idx_i = BS_touch_info_multi_sort[i];
            int idx_j = BS_touch_info_multi_sort[j];
            float area_i = BS_touch_info_multi[idx_i].multiArea;
            float area_j = BS_touch_info_multi[idx_j].multiArea;
            if ((area_i + EPSILON) < area_j) {
                uint8_t tmp8 = BS_touch_info_multi_sort[i];
                BS_touch_info_multi_sort[i] = BS_touch_info_multi_sort[j];
                BS_touch_info_multi_sort[j] = tmp8;
            }
        } //for (j = (i + 1); j < tpCntSort; j++)
    } //for (i = 0; i < (tpCntSort - 1); i++)

#if (DEBUG_BG_clipping_multi > 1) //for testing
    IS_DEBUG_FLAG{
        TRACE_CM2("touch_info..%d", tpCntSort);
    for (i = 0; i < touch_info_cnt; i++) {
        TRACE_CM2(" %d:(0x%x)%d %0.1f", i, BS_touch_info_multi[i].multiStat, BS_touch_info_multi[i].polLen, BS_touch_info_multi[i].multiArea);
    }
    TRACE_CM2("sort..%d", tpCntSort);
    for (i = 0; i < tpCntSort; i++) {
        j = BS_touch_info_multi_sort[i];
        TRACE_CM2(" %d/%d:(0x%x)%d %0.1f", i, j, BS_touch_info_multi[j].multiStat, BS_touch_info_multi[j].polLen, BS_touch_info_multi[j].multiArea);
    }
    TRACE_NOP;
    };
#endif
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };
    BS_touch_info_multi_cnt_sort = tpCntSort;
    return tpCntSort;
}
#endif

#ifdef MULTI_SKIP_USED_VB //nsmoon@210218
static void initial_lineNo_init(void)
{
    TRACE_MSUV("initial_lineNo_init.. %d", BG_frame_no);
    BS_initial_lineNo_x_cnt = 0;
    BS_initial_lineNo_y_cnt = 0;
}

static int is_exist_initial_lineNo(initial_polygon_t *initial_polygon, int mode)
{
    //mode: 1=return if found any
    int i, ret = 0;
    TRACE_MSUV("is_exist_initial_lineNo..(%d) %x %x", mode, initial_polygon->xNo, initial_polygon->yNo);

    for (i = 0; i < BS_initial_lineNo_x_cnt; i++) {
        if (BS_initial_lineNo_x[i] == initial_polygon->xNo) {
            ret = 1; //found-x
            if (mode) {
                TRACE_MSUV("..exist-x %d (%d)", ret, i);
                return ret;
            }
            break;
        }
    }
    for (i = 0; i < BS_initial_lineNo_y_cnt; i++) {
        if (BS_initial_lineNo_y[i] == initial_polygon->yNo) {
            ret += 2; //found-y
            if (mode) {
                TRACE_MSUV("..exist-y %d (%d)", ret, i);
                return ret;
            }
            break;
        }
    }
    TRACE_MSUV("..exist %d", ret);
    return ret; //not-found
}

static void initial_lineNo_save(initial_polygon_t *initial_polygon)
{
    int ret = is_exist_initial_lineNo(initial_polygon, 0);
    if (ret == 3) {
        return; //exist both
    }
    if (ret != 1) { //not x
        if (BS_initial_lineNo_x_cnt < MAX_TOUCH_LIMIT_MULTI) {
            BS_initial_lineNo_x[BS_initial_lineNo_x_cnt++] = initial_polygon->xNo;
            TRACE_MSUV("initial_lineNo_save..x %d %x", BS_initial_lineNo_x_cnt, initial_polygon->xNo);
        }
        else {
            TRACE_ERROR("ERROR! initial_lineNo_save..invalid BS_initial_lineNo_x_cnt", BS_initial_lineNo_x_cnt);
        }
    }
    if (ret != 2) { //not y
        if (BS_initial_lineNo_y_cnt < MAX_TOUCH_LIMIT_MULTI) {
            BS_initial_lineNo_y[BS_initial_lineNo_y_cnt++] = initial_polygon->yNo;
            TRACE_MSUV("initial_lineNo_save..y %d %x", BS_initial_lineNo_y_cnt, initial_polygon->yNo);
        }
        else {
            TRACE_ERROR("ERROR! initial_lineNo_save..invalid BS_initial_lineNo_y_cnt", BS_initial_lineNo_y_cnt);
        }
    }
}
#endif

#define MIN_MAX_ADJUST	0
ATTR_BACKEND_RAMFUNC
int BG_clipping_multi(int multiLoopCnt)
{
    poly_e_t subjecte, *rese;
    bd_type_t bdX, bdY;
	//ep_min_max_t minMaxEp;
    pos_min_max_t min_max;
#if (DEBUG_BG_clipping_multi > 2)
    sen_no_t centSen;
#endif
    int doneCnt = 0;
    int i, j, ret; //k
    int sideIdx, sideIdxPre = -1;
    int polyIdx, tIdx;
    int touch_info_cnt;
    int touch_info_cnt_start = BS_touch_info_multi_cnt;
    int touch_info_cnt_prev = touch_info_cnt_start;
    //int touch_cnt;
    //int tpCntSort = 0;
    int skipClipping;
    float clippedAreaX = 0; //nsmoon@210308
#if (DEBUG_BG_clipping_multi > 2)
    int wSenX = 0, wSenY = 0;
#endif
    //int retX, retY;
    //pos_minMax2_t minMaxRetX, minMaxRetY;
    //float widthX, widthY;
    //vec_t centerPoint;

    //TRACE_CM("BG_clipping_multi..");
    BG_touch_area[0] = 0;
    BS_touch_info_multi_cnt =  GET_MAX(0, BS_touch_info_multi_cnt);
    BS_touch_info_multi_cnt = GET_MIN(MAX_TOUCH_INFO_MULTI_SIZE, BS_touch_info_multi_cnt);
    touch_info_cnt = BS_touch_info_multi_cnt; //restore
#ifdef USED_LINE_METHOD_OLD //nsmoon@200119
    BS_tmp_used_ep_cnt_max = 0; //init
#endif

    IS_DEBUG_FLAG{
        TRACE_NOP;
    }
#ifdef MULTI_SKIP_USED_VB //nsmoon@210218
    if (multiLoopCnt == 2) {
        initial_lineNo_init(); //multi-2 only
    }
#endif

    for (polyIdx = 0; polyIdx < BS_initial_polygon_cnt; polyIdx++)
    {
#ifdef MULTI_SKIP_USED_VB //nsmoon@210218
        if (multiLoopCnt == 2) {
            TRACE_MSUV("**polyIdx=%d %x %x", polyIdx, BS_initial_polygon[polyIdx].xNo, BS_initial_polygon[polyIdx].yNo);
            if ((BS_initial_polygon[polyIdx].xNo & INITIAL_VB_MERGED) == 0 &&
                    (BS_initial_polygon[polyIdx].yNo & INITIAL_VB_MERGED) == 0) {
                if (is_exist_initial_lineNo(&BS_initial_polygon[polyIdx], 1) != 0) {
                    TRACE_MSUV("..skipped..");
                    continue; //exist
                }
            }
        }
#endif
        sideIdx = (int)BS_initial_polygon[polyIdx].side;
        if (sideIdx == ENUM_BOT_RHT) {
            bdX = ENUM_BOT_BD;
            bdY = ENUM_RHT_BD;
        }
        else if (sideIdx == ENUM_TOP_RHT) {
            bdX = ENUM_TOP_BD;
            bdY = ENUM_RHT_BD;
        }
        else if (sideIdx == ENUM_TOP_LFT) {
            bdX = ENUM_TOP_BD;
            bdY = ENUM_LFT_BD;
        }
        else {
            //(sideIdx == ENUM_BOT_LFT)
            bdX = ENUM_BOT_BD;
            bdY = ENUM_LFT_BD;
        }
#if !defined(TRACE_SAVE_ERROR_MSG)
            IS_DEBUG_FLAG{
                if (1)
                //if (multiLoopCnt == 2 && polyIdx == 0)
                //if (multiLoopCnt == 2)
                {
                    SET_DEBUG_FLAG2;
                    TRACE_NOP;
                }
                else {
                    RESET_DEBUG_FLAG2;
                }
            };
            IS_NOT_DEBUG_FLAG {
                RESET_DEBUG_FLAG2;
            };
#endif
            doneCnt++;

#if (DEBUG_BG_clipping_multi > 2)
            centSen.x = GET_LINE_CENT_INT(minMaxEp.minX, minMaxEp.maxX);
            centSen.y = GET_LINE_CENT_INT(minMaxEp.minY, minMaxEp.maxY);
            wSenX = minMaxEp.maxX - minMaxEp.minX + 1;
            wSenY = minMaxEp.maxY - minMaxEp.minY + 1;
#endif

            //clipping_var_init();
            //clipping_var_reset();
            //clipping_add_initial_polygon(polyIdx);
#ifdef USED_LINE_METHOD_OLD //nsmoon@200119
            BS_tmp_used_ep_cnt = 0; //init
#endif

            multi_get_clipping_info(ENUM_HOR_X, bdX, bdY, sideIdx, polyIdx, &BS_initial_polygon[polyIdx]);
            multi_get_clipping_info(ENUM_VER_Y, bdX, bdY, sideIdx, polyIdx, &BS_initial_polygon[polyIdx]);

            if (sideIdx != sideIdxPre) {
                //SEND_POLY(0, 0x21, 0);
                BS_epaInitMulti();
                BS_packEdgePatternMulti_ind(bdX);
                BS_packEdgePatternMulti_ind(bdY);
                sideIdxPre = sideIdx; //save
            }

#if (DEBUG_BG_clipping_multi > 2)
            IS_DEBUG_FLAG{
                TRACE_CM("X-poly: %d-%d-%d (%d %d) %d %d", polyIdx, sideIdx, bdX, centSen.x, centSen.y, wSenX, wSenY);
                if (polyIdx == 9 && sideIdx == 2 && bdX == 2) {
                    TRACE_NOP;
                }
            }
#endif
            //nsmoon@210216 move to above
            //multi_get_clipping_info(ENUM_HOR_X, bdX, bdY, sideIdx, polyIdx, &BS_initial_polygon[polyIdx]);
            //multi_get_clipping_info(ENUM_VER_Y, bdX, bdY, sideIdx, polyIdx, &BS_initial_polygon[polyIdx]);

            //chk erase first at multiLoopCnt==1
            skipClipping = 0;
        clippedAreaX = 0; //nsmoon@210308
#if 1 //def MULTI_CHK_PALM_ERASER
            if (multiLoopCnt == 1) {
            clipping_var_init();
            clipping_var_reset();
            clipping_add_initial_polygon(polyIdx);
            ret = clipping_multi_remained(ENUM_HOR_X, &BS_initial_polygon[polyIdx], -1); //-1:chk eraser
            if (ret && BS_pclipping_info.eraser_pen) {
                IS_DEBUG_FLAG{
                    TRACE_NOP;
                };
                if (BS_pclipping_info.eraser_pen == 1) {
                BS_pclipping_info.polygon_cnt = 1;
                }
                skipClipping++; //found eraser or pen
            }
            else {
				clipping_var_init(); //nsmoon@200318
                clipping_var_reset();
                clipping_add_initial_polygon(polyIdx);
                ret = clipping_multi_remained(ENUM_VER_Y, &BS_initial_polygon[polyIdx], -1); //-1:chk eraser
                if (ret && BS_pclipping_info.eraser_pen) {
                    IS_DEBUG_FLAG{
                        TRACE_NOP;
                    };
                    if (BS_pclipping_info.eraser_pen == 1) {
                    BS_pclipping_info.polygon_cnt = 1;
                    }
                    skipClipping++; //found eraser or pen
                }
            }
            IS_DEBUG_FLAG{
                TRACE_NOP;
            };
            }
#if 1 //for test, not-clipping @big-eraser
            if (skipClipping) {
                if (BS_pclipping_info.eraser_pen == 1) {
                    //eraser
                    clipping_var_swap();
                }
            }
            else
#endif
#endif //0
            {
                clipping_var_init();
                clipping_var_reset();
                clipping_add_initial_polygon(polyIdx);
                ret = clipping_multi_remained(ENUM_HOR_X, &BS_initial_polygon[polyIdx], multiLoopCnt);
                if (ret < 0) {
                    return -1; //mem-error
                }
    #if 1 //y-axis
            if (ret > 0) //FIXME ??
                {
                if (ret == 1) {
                    //(BS_pclipping_info.polygon_cnt == 1)
                    clippedAreaX = BS_pclipping_info.polygon_area[0];
                }
    #if (DEBUG_BG_clipping_multi > 0)
                    IS_DEBUG_FLAG{
                        //SEND_POLY(0, 0x21, 0);
                        DEBUG_SHOW_BS_POLYGON(MY_COLOR-2);
                        TRACE_NOP;
                    };
    #endif

    #if (DEBUG_BG_clipping_multi > 2)
                    IS_DEBUG_FLAG{
                        TRACE_NOP;
                        TRACE_CM("Y-poly:  %d-%d-%d (%d %d) %d %d", polyIdx, sideIdx, bdY, centSen.x, centSen.y, wSenX, wSenY);
                        if (polyIdx == 9 && sideIdx == 2 && bdY == 1) {
                            TRACE_NOP;
                        }
                    }
    #endif
                    ret = clipping_multi_remained(ENUM_VER_Y, &BS_initial_polygon[polyIdx], multiLoopCnt);
                    if (ret < 0) {
                        return -1; //mem-error
                    }
#if 1 //nsmoon@210308
                if (ret == 1) {
                    //TRACE("clippedAreaX= %f %f (%0.1f)", clippedAreaX, BS_pclipping_info.polygon_area[0], BS_initial_polygon[polyIdx].area);
                    float sizeDiff = GET_ABS(clippedAreaX - BS_pclipping_info.polygon_area[0]);
                    if (sizeDiff < MULTI_NO_CLIPPING_SIZE) {
                        BS_pclipping_info.no_clip_y = 1;
                    }
                }
#endif
                } //if (ret > 0)
    #endif //y-axis
            }
#if (DEBUG_BG_clipping_multi > 0)
            IS_DEBUG_FLAG{
                TRACE_NOP;
               //if (ret > 0) 
                {
                    //SEND_POLY(0, 0x21, 0);
                    DEBUG_SHOW_BS_POLYGON(MY_COLOR-2);
                    TRACE_NOP;
                }
            };
#endif

            //////////////////////////////////////////////////////////////////////////////
            // save used info
            //////////////////////////////////////////////////////////////////////////////
#ifdef USED_LINE_METHOD_OLD //nsmoon@200119
#if 1 //nsmoon@190703, nsmoon@191216 move to below, bug!! do not move
            if (BS_pclipping_info.polygon_cnt > 0) {
                BS_tmp_used_ep_cnt_max = GET_MAX(BS_tmp_used_ep_cnt_max, BS_tmp_used_ep_cnt);
                IS_DEBUG_FLAG{
                    TRACE_NOP;
                };
                set_used_ep2();
#ifdef SHOW_DEBUG_SIZE
                //BG_debug_misc[6] = BS_tmp_used_ep_cnt_max;
                //TRACE("BS_tmp_used_ep_cnt: %d (%d)", BS_tmp_used_ep_cnt, BS_tmp_used_ep_cnt_max);
#endif
            }
#endif //1
#endif
            IS_DEBUG_FLAG{
                TRACE_NOP;
            };

            //////////////////////////////////////////////////////////////////////////////
            // save BS_touch_info_multi
            //////////////////////////////////////////////////////////////////////////////
            touch_info_cnt_prev = touch_info_cnt;
            for (tIdx = 0; tIdx < BS_pclipping_info.polygon_cnt; tIdx++)
            {
                //if (BS_pclipping_info.polygon_state[tIdx] == ENUM_POLYGON_GHOST) continue; //nsmoon@190701, for testing
                subjecte.len = BS_pclipping_info.polygon_length[tIdx];
                subjecte.ve = &(macT2P(BS_pclipping_info.polygon_vertex, tIdx, 0, BS_pclipping_info.polygon_packs));
                //(templateStructure[i*packsPerPoly + j / 8][j % 8])
                rese = &subjecte;
#if (DEBUG_BG_clipping_multi > 1)
                IS_DEBUG_FLAG{
                    SEND_POLY(rese, 0x11, 2);
                };
#endif
                //save touch info
                if (touch_info_cnt < MAX_TOUCH_INFO_MULTI_SIZE) {
                    getTouchMinMax(rese, &min_max);
                    //IS_DEBUG_FLAG{ TRACE_CM(" min_max: (%0.2f,%0.2f) (%0.2f,%0.2f)", min_max.minX, min_max.maxX, min_max.minY, min_max.maxY);}

                    //BS_pclipping_info.touch_idx_saved[touch_info_cnt] = tIdx;  //nsmoon@170202
#ifdef DRAW_POLYGON_TEST
                ret = DEBUG_clipping_var_save_to_touch(touch_info_cnt, rese);
#endif
                    float polygon_area = BS_pclipping_info.polygon_area[tIdx];
#if (DEBUG_BG_clipping_multi > 1) //for testing
                    IS_DEBUG_FLAG{ TRACE_CM("multi_min_max: %d(%0.2f/%d) %0.2f/%0.2f %0.2f/%0.2f", touch_info_cnt, polygon_area, rese->len, min_max.minX, min_max.maxX, min_max.minY, min_max.maxY); };
#endif
                    BS_touch_info_multi[touch_info_cnt].multiStat = 0; //init
                    BS_touch_info_multi[touch_info_cnt].polLen = (uint8_t)rese->len;
                    BS_touch_info_multi[touch_info_cnt].bdX = bdX;
                    BS_touch_info_multi[touch_info_cnt].bdY = bdY;
                    BS_touch_info_multi[touch_info_cnt].mM = min_max;
                    BS_touch_info_multi[touch_info_cnt].centerPos.x = (min_max.minX + min_max.maxX) * 0.5f;
                    BS_touch_info_multi[touch_info_cnt].centerPos.y = (min_max.minY + min_max.maxY) * 0.5f;
                    BS_touch_info_multi[touch_info_cnt].multiArea = polygon_area;
                BS_touch_info_multi[touch_info_cnt].no_clip_y = BS_pclipping_info.no_clip_y; //nsmoon@210308
#ifdef MULTI_SENSOR_RANGE_OLD //nsmoon@200119
                    BS_touch_info_multi[touch_info_cnt].sRx = BS_pclipping_info.polygon_range[tIdx]; //sensor_range_x[tIdx];
                    BS_touch_info_multi[touch_info_cnt].sRy = BS_sensor_range_y[tIdx];
#endif
                    touch_info_cnt++;
                }
                else {
                    TRACE_ERROR("ERROR! invalid touch_info_cnt %d [%d]", touch_info_cnt, BG_frame_no);
                    return -1; //break; //nsmoon@211201
                }
            } //for (tIdx = 0; tIdx < BS_pclipping_info.polygon_cnt; tIdx++)
            if (skipClipping) {
                continue; //nsmoon@200529
                //break;
            }
        IS_DEBUG_FLAG{
            TRACE_NOP;
        };
        if (touch_info_cnt_prev == touch_info_cnt) {
            continue;
        }

#ifdef DEBUG_FUNCTION_ENABLE_ALL
#define DEBUG_MULTI_REMOVE_SMALL_TP		0
#endif
#if 1 //nsmoon@200204
    /////////////////////////////////////////////////////////////////////////
    //remove small edge and size
    /////////////////////////////////////////////////////////////////////////
    #define MIN_POL_VERT_LEN	4
        for (i = touch_info_cnt_prev; i < touch_info_cnt; i++)
        {
        if (BS_touch_info_multi[i].polLen < MIN_POL_VERT_LEN) {
            //no-clipping
            BS_touch_info_multi[i].multiStat |= MULTISTAT_NOCLIPPING;
                IS_DEBUG_FLAG{ TRACE_CM(" =>polLen_i= %d %d", i, BS_touch_info_multi[i].polLen); }
#if (DEBUG_MULTI_REMOVE_SMALL_TP > 0) //def DRAW_POLYGON_TEST  //nsmoon@170202
            IS_DEBUG_FLAG{ DEBUG_SHOW_BS_TOUCH_POLYGON_ONE(i, -1); };
#endif
        }
        else if (BS_touch_info_multi[i].multiArea < MULTI_GHOST_MIN_AREA)
        {
            //remove small tp
            BS_touch_info_multi[i].multiStat |= MULTISTAT_GHOST; //ghost
                IS_DEBUG_FLAG{ TRACE_CM(" =>small..%d", i); };
#if (DEBUG_MULTI_REMOVE_SMALL_TP > 0) //def DRAW_POLYGON_TEST  //nsmoon@170202
            IS_DEBUG_FLAG{ DEBUG_SHOW_BS_TOUCH_POLYGON_ONE(i, -1); };
#endif
    }
        } //for (i = touch_info_cnt_prev; i < touch_info_cnt; i++)
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };
#endif //1

#if 1 //nsmoon@190704
#define DEBUG_MULTI_OVERLAPPED_TOUCH    0
      /////////////////////////////////////////////////////////////////////////
      //remove overlapped touch
      /////////////////////////////////////////////////////////////////////////
        for (i = touch_info_cnt_prev; i < touch_info_cnt; i++)
        {
    if ((BS_touch_info_multi[i].multiStat & MULTISTAT_REMOVED_MASK) != 0) continue; //removed
#if (DEBUG_MULTI_OVERLAPPED_TOUCH > 0) //def DRAW_POLYGON_TEST  //nsmoon@170202
    IS_DEBUG_FLAG{
    SEND_POLY(0, 0x21, MY_COLOR);
            DEBUG_SHOW_BS_TOUCH_POLYGON(touch_info_cnt, MY_COLOR - 4);
            DEBUG_SHOW_MIN_MAX(&BS_touch_info_multi[i].mM, MY_COLOR - 1, 0); //color, mode:1(show it at debug on)
    TRACE_NOP;
    }
#endif
            int startTmp = (multiLoopCnt == 1) ? (i + 1) : 0;
            //for (j = (i+1); j < touch_info_cnt; j++)
            for (j = startTmp; j < touch_info_cnt; j++)
            {
                if (i == j) continue; //nsmoon@200406
        if ((BS_touch_info_multi[j].multiStat & MULTISTAT_REMOVED_MASK) != 0) continue; //removed
#if (DEBUG_MULTI_OVERLAPPED_TOUCH > 0) //def DRAW_POLYGON_TEST  //nsmoon@170202
        IS_DEBUG_FLAG{
                    DEBUG_SHOW_MIN_MAX(&BS_touch_info_multi[j].mM, MY_COLOR - 2, 0); //color, mode:1(show it at debug on)
        };
#endif
                ret = multi_is_overlap_closed_min_max(i, j, 0);
                if (ret >= 0) {
                    BS_touch_info_multi[ret].multiStat |= MULTISTAT_MERGED; //merged
                    IS_DEBUG_FLAG{ TRACE_CM(" #overlap!! %d (%d)", ret, multiLoopCnt); };
#if (DEBUG_MULTI_OVERLAPPED_TOUCH > 0) //def DRAW_POLYGON_TEST  //nsmoon@170202
            IS_DEBUG_FLAG{
                        DEBUG_SHOW_MIN_MAX(&BS_touch_info_multi[ret].mM, MY_COLOR - 3, 0); //color, mode:1(show it at debug on)
                        TRACE_NOP;
            };
#endif
        }
    } //for (j = (i + 1); j < touch_info_cnt; j++)
} //for (i = 0; i < (touch_info_cnt - 1); i++)
IS_DEBUG_FLAG {
    TRACE_NOP;
};
#endif //1
#ifdef MULTI_SKIP_USED_VB //nsmoon@210218
        if (multiLoopCnt == 2) {
            for (i = touch_info_cnt_prev; i < touch_info_cnt; i++) {
                if ((BS_touch_info_multi[i].multiStat & MULTISTAT_REMOVED_MASK) != 0) continue; //removed
                //if ((BS_initial_polygon[polyIdx].xNo & INITIAL_VB_MERGED) == 0)
                {
                    initial_lineNo_save(&BS_initial_polygon[polyIdx]);
                    break; //save once
                }
            }
        }
#endif
    } //for (polyIdx = 0; polyIdx < BS_initial_polygon_cnt; polyIdx++)

#if (DEBUG_BG_clipping_multi > 0) //def DRAW_POLYGON_TEST  //nsmoon@170202
    //SEND_POLY(0, 0x21, MY_COLOR);
    IS_DEBUG_FLAG{
        for (i = 0; i < touch_info_cnt; i++) {
            if ((BS_touch_info_multi[i].multiStat & MULTISTAT_REMOVED_MASK) != 0) {
                //removed
                DEBUG_SHOW_MIN_MAX(&BS_touch_info_multi[i].mM, MY_COLOR - 1, 0);
            }
            else {
                DEBUG_SHOW_MIN_MAX(&BS_touch_info_multi[i].mM, MY_COLOR - 4, 0);
        }
    }
        TRACE_NOP;
    };
#endif

#if 1 //cal used line
    for (i = touch_info_cnt_start; i < touch_info_cnt; i++)
    {
        int idx_i = i; //BS_touch_info_multi_sort[i];
        //include ghost or not? FIXME???
        if ((BS_touch_info_multi[idx_i].multiStat & MULTISTAT_REMOVED_MASK) != 0) continue;
		IS_DEBUG_FLAG{
                TRACE_NOP;
		}
            multi_recal_used_line(ENUM_HOR_X, &BS_touch_info_multi[idx_i], 0); //0:disable calTh10
            multi_recal_used_line(ENUM_VER_Y, &BS_touch_info_multi[idx_i], 0); //0:disable calTh10
    }
#endif

    int tpCntLocal = count_touch_info2(&BS_touch_info_multi[0], touch_info_cnt_start, touch_info_cnt);
	BS_touch_info_multi_cnt = touch_info_cnt;
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };
    //return touch_cnt;
    //TRACE_CM("tpCntLocal=%d (%d)", tpCntLocal, multiLoopCnt);
    return tpCntLocal;
}

#if 1 //nsmoon@200310
#define TRACE_MINE(...)    //TRACE(__VA_ARGS__)
#define MULTI_ERASER_DIST_SQ   (500.0f * 500.0f)
static int multi_is_near_eraser(int tIdx)
{
    //int touch_info_cnt = BS_touch_info_multi_cnt;
    int tpCntSort = BS_touch_info_multi_cnt_sort;
    float distSq;
    int sortIdx; //i

    for (sortIdx = 0; sortIdx < tpCntSort; sortIdx++) {
        int i = BS_touch_info_multi_sort[sortIdx];
        if (i == tIdx) continue;
        if ((BS_touch_info_multi[i].multiStat & MULTISTAT_REMOVED_MASK) != 0) {
            continue; //removed
        }
        TRACE_MINE("..multiArea= (%d) %0.1f/%0.1f", i, BS_touch_info_multi[i].multiArea, TOUCH_ERASER_SIZESQ_XLARGE);
        if (BS_touch_info_multi[i].multiArea > TOUCH_ERASER_SIZESQ_XLARGE)
        {
            distSq = BG_calDistSquare(BS_touch_info_multi[i].centerPos, BS_touch_info_multi[tIdx].centerPos);
            TRACE_MINE("..distSq= (%d) %0.1f/%0.1f", i, distSq, MULTI_ERASER_DIST_SQ);
            if (distSq < MULTI_ERASER_DIST_SQ) {
                return 1; //found
            }
        }
    }
    return 0; //not found
}
#endif

#if 0 //for test
static int multi_is_closed_touchinfo_cent(vec_t *cent)
{
    int i;
    vec_t centerPoint;
    float diffDistX, diffDistY;
    int touch_info_cnt = BS_touch_info_multi_cnt;

    for (i = 0; i < touch_info_cnt; tIdx++) {
        if ((BS_touch_info_multi[i].multiStat & MULTISTAT_REMOVED_MASK) != 0) continue;
        centerPoint = BS_touch_info_multi[i].centerPos;
        diffDistX = GET_ABS(cent->x - centerPoint.x);
        diffDistY = GET_ABS(cent->y - centerPoint.y);
        IS_DEBUG_FLAG{ TRACE("diffDistX,diffDistY: %0.1f/%0.1f", diffDistX, diffDistY); };
        if (diffDistX < MULTI_CLOSED_TOUCHINFO_CENT_MERGE && diffDistY < MULTI_CLOSED_TOUCHINFO_CENT_MERGE) {
            IS_DEBUG_FLAG{ TRACE("multi closed~ %0.1f/%0.1f", diffDistX, diffDistY); };
            return 1; //closed
        }
    } //for (i = 0; i < BG_touch_count; i++)

    return 0; //not-closed
}
#endif

#if (MODEL_SPT != MODEL_KIO_550) && (MODEL_SPT != MODEL_KIO_430) //nsmoon@201228
#define MULTI_ERASER_MERGE
#endif
#ifdef MULTI_ERASER_MERGE //eraser & merge //nsmoon@200512 disable
#define MULTI_MERGE_OVERLAP_GAP			3.0f
static int multi_is_closed_min_max_merge(touch_info_multi_t *ti_a, touch_info_multi_t *ti_b)
{
    int ol_x1, ol_x2, ol_x3, ol_x4, ol_y1, ol_y2, ol_y3, ol_y4;
    float max_overlap_gap = MULTI_MERGE_OVERLAP_GAP;

    //check ovelapped min/max
    //TRACE("%0.1f %0.1f %0.1f %0.1f - %0.1f %0.1f %0.1f %0.1f", a->mM.minX, a->mM.maxX, a->mM.minY, a->mM.maxY, b->mM.minX, b->mM.maxX, b->mM.minY, b->mM.maxY);
    ol_x1 = (ti_a->mM.minX > (ti_b->mM.minX - max_overlap_gap) && ti_a->mM.minX < (ti_b->mM.maxX + max_overlap_gap));
    ol_x2 = (ti_a->mM.maxX > (ti_b->mM.minX - max_overlap_gap) && ti_a->mM.maxX < (ti_b->mM.maxX + max_overlap_gap));
    ol_x3 = (ti_b->mM.minX > (ti_a->mM.minX - max_overlap_gap) && ti_b->mM.minX < (ti_a->mM.maxX + max_overlap_gap));
    ol_x4 = (ti_b->mM.maxX > (ti_a->mM.minX - max_overlap_gap) && ti_b->mM.maxX < (ti_a->mM.maxX + max_overlap_gap));
    ol_y1 = (ti_a->mM.minY > (ti_b->mM.minY - max_overlap_gap) && ti_a->mM.minY < (ti_b->mM.maxY + max_overlap_gap));
    ol_y2 = (ti_a->mM.maxY > (ti_b->mM.minY - max_overlap_gap) && ti_a->mM.maxY < (ti_b->mM.maxY + max_overlap_gap));
    ol_y3 = (ti_b->mM.minY > (ti_a->mM.minY - max_overlap_gap) && ti_b->mM.minY < (ti_a->mM.maxY + max_overlap_gap));
    ol_y4 = (ti_b->mM.maxY > (ti_a->mM.minY - max_overlap_gap) && ti_b->mM.maxY < (ti_a->mM.maxY + max_overlap_gap));

    if ((ol_x1 || ol_x2 || ol_x3 || ol_x4) && (ol_y1 || ol_y2 || ol_y3 || ol_y4)) {
        IS_DEBUG_FLAG{ TRACE("ol_x1,ol_y1: (%d,%d,%d,%d) (%d,%d,%d,%d)", ol_x1, ol_x1, ol_x3, ol_x4, ol_y1, ol_y1, ol_y3, ol_y4); };
        ti_a->mM.minX = GET_MIN(ti_a->mM.minX, ti_b->mM.minX);
        ti_a->mM.maxX = GET_MAX(ti_a->mM.maxX, ti_b->mM.maxX);
        ti_a->mM.minY = GET_MIN(ti_a->mM.minY, ti_b->mM.minY);
        ti_a->mM.maxY = GET_MAX(ti_a->mM.maxY, ti_b->mM.maxY);
        return 1; //overlapped
    }
    return 0; //not-overlapped
}

static int multi_is_closed_center_merge(touch_info_multi_t *ti_a, touch_info_multi_t *ti_b)
{
    float distSq = BG_calDistSquare(ti_a->centerPos, ti_b->centerPos);
    if (distSq < MULTI_CLOSED_TOUCHINFO_CENT_MERGE_SQ) {
    TRACE("distSq: %0.1f/%0.1f (%0.1f,%0.1f)(%0.1f,%0.1f)", distSq, MULTI_CLOSED_TOUCHINFO_CENT_MERGE_SQ, ti_a->centerPos.x, ti_a->centerPos.y, ti_b->centerPos.x, ti_b->centerPos.y);
        ti_a->mM.minX = GET_MIN(ti_a->mM.minX, ti_b->mM.minX);
        ti_a->mM.maxX = GET_MAX(ti_a->mM.maxX, ti_b->mM.maxX);
        ti_a->mM.minY = GET_MIN(ti_a->mM.minY, ti_b->mM.minY);
        ti_a->mM.maxY = GET_MAX(ti_a->mM.maxY, ti_b->mM.maxY);
        return 1; //merged
    }
    return 0; //not closed
}
#endif //1

int BG_clipping_multi_post(int multiLoopCnt)
{
    int i, j, k; //ret
    vec_t centerPoint;
    int touch_info_cnt = BS_touch_info_multi_cnt;
    int sortIdx;
    //int touch_info_cnt_prev = touch_info_cnt;
    int tpCntSort = BS_touch_info_multi_cnt_sort;
    int touchCntOrg = BG_touch_count;
    int touchCntMulti = BG_touch_count;
    float widthX, widthY;
    float widthX10, widthY10;
    int ret;
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };

#if 1 //remove small tp, which is closed with large tp
#define DEBUG_MULTI_SMALL_CLOSED_LARGE  0
    /////////////////////////////////////////////////////////////////////////
    //remove small tp, which is closed with large tp
    /////////////////////////////////////////////////////////////////////////
    //for (i = touch_info_cnt_prev; i < touch_info_cnt; i++)
    for (i = 0; i < touch_info_cnt; i++)
    {
        int idx_i = i; //BS_touch_info_multi_sort[i];
        touch_info_multi_t *nM_i = &BS_touch_info_multi[idx_i];
        if ((nM_i->multiStat & MULTISTAT_REMOVED_MASK) != 0) continue; //removed

        //int startTmp = (multiLoopCnt == 1) ? (i + 1) : 0;
        //for (j = startTmp; j < touch_info_cnt; j++)
        for (j = (i+1); j < touch_info_cnt; j++)
        {
            int idx_j = j; //BS_touch_info_multi_sort[j];
            touch_info_multi_t *nM_j = &BS_touch_info_multi[idx_j];
            if ((nM_j->multiStat & MULTISTAT_REMOVED_MASK) != 0) continue; //removed
            if (idx_i == idx_j) continue; //nsmoon@200406
#if (DEBUG_MULTI_SMALL_CLOSED_LARGE > 0)
            IS_DEBUG_FLAG{
                SEND_POLY(0, 0x21, MY_COLOR);
                DEBUG_SHOW_BS_TOUCH_POLYGON(touch_info_cnt, MY_COLOR - 1);
                DEBUG_SHOW_MIN_MAX(&nM_i->mM, MY_COLOR - 2, 0); //color, mode:1(show it at debug on)
                DEBUG_SHOW_MIN_MAX(&nM_j->mM, MY_COLOR - 4, 0); //color, mode:1(show it at debug on)
                TRACE_NOP;
            };
#endif
            ret = multi_is_adjacent_tp_size(idx_i, idx_j, touch_info_cnt);
            if (ret >= 0) {
                BS_touch_info_multi[ret].multiStat |= MULTISTAT_GHOST; //ghost
                IS_DEBUG_FLAG{ 
                    TRACE_CM(" *small near..%d (%d-%d)", ret, idx_i, idx_j);
                };
#if (DEBUG_MULTI_SMALL_CLOSED_LARGE > 0) //def DRAW_POLYGON_TEST  //nsmoon@170202
                IS_DEBUG_FLAG{
                        DEBUG_SHOW_MIN_MAX(&BS_touch_info_multi[ret].mM, MY_COLOR - 3, 0); //color, mode:1(show it at debug on)
                    TRACE_NOP;
                };
#endif
                if (ret == idx_i) {
                    break;
                }
            }
#if (DEAD_CELL_TBL == 1) //nsmoon@191205, FIXME
            if (BS_inValidTblLenXpd > 0 || BS_inValidTblLenXled > 0 || BS_inValidTblLenYpd > 0 || BS_inValidTblLenYled > 0) {
                ret = multi_is_adjacent_deadcell(idx_i, idx_j);
                if (ret >= 0) {
                    BS_touch_info_multi[ret].multiStat |= MULTISTAT_GHOST; //ghost
                    IS_DEBUG_FLAG{ TRACE_CM(" *deadcell near..%d", ret); }
#if (DEBUG_MULTI_SMALL_CLOSED_LARGE > 0) //def DRAW_POLYGON_TEST  //nsmoon@170202
                    IS_DEBUG_FLAG{
                            DEBUG_SHOW_MIN_MAX(&BS_touch_info_multi[ret].mM, MY_COLOR - 3, 0); //color, mode:1(show it at debug on)
                        TRACE_NOP;
                    };
#endif
                    if (ret == idx_i) {
                        break;
                    }
                }
                //else {
                //    TRACE_ERROR("ERROR! BG_clipping_multi_post..invalid ret");
                //}
            }
#endif
        } //for (j = 0; j < (tpCntSort - 1); j++)
#if (DEBUG_MULTI_SMALL_CLOSED_LARGE > 0) //def DRAW_POLYGON_TEST  //nsmoon@170202
        IS_DEBUG_FLAG{ DEBUG_SHOW_BS_TOUCH_POLYGON_ONE(idx_i, -1); };
#endif
    } //for (i = touch_info_cnt_prev; i < touch_info_cnt; i++)
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };
#endif //1

    tpCntSort = touch_info_sort(0, touch_info_cnt);

#if 1 //remove small tp, which is surrounded with large tp
#define DEBUG_MULTI_SMALL_SURROUNED_LARGE   0
    ///////////////////////////////////////////////////////
    //remove small tp, which is surrounded with large tp
    ///////////////////////////////////////////////////////
    if (tpCntSort > 2) {
        //required three TPs at least
        for (i = 0; i < tpCntSort; i++) {
            //largest one first
            int idx_i = BS_touch_info_multi_sort[i];
            if ((BS_touch_info_multi[idx_i].multiStat & MULTISTAT_REMOVED_MASK) != 0) continue; //removed
            //sacn x-axis
            for (j = (tpCntSort - 1); j > 0; j--) {
                //smallest one first
                int idx_j = BS_touch_info_multi_sort[j];
                if ((BS_touch_info_multi[idx_j].multiStat & MULTISTAT_REMOVED_MASK) != 0) continue; //removed
                if (idx_i == idx_j) {
                    if (BS_touch_info_multi[idx_i].multiArea > TOUCH_FINGER_SIZESQ) {
                        break;
                    }
                    else {
                        continue; //nsmoon@200309
                    }
                }
#if (DEBUG_MULTI_SMALL_SURROUNED_LARGE > 0)
                IS_DEBUG_FLAG{
                    //TRACE_CM("centerPos= (%0.1f, %0.1f)(%0.1f, %0.1f)", BS_touch_info_multi[idx_i].centerPos.x, BS_touch_info_multi[idx_i].centerPos.y, BS_touch_info_multi[idx_j].centerPos.x, BS_touch_info_multi[idx_j].centerPos.y);
                    SEND_POLY(0, 0x21, MY_COLOR);
                    DEBUG_SHOW_BS_TOUCH_POLYGON(touch_info_cnt, MY_COLOR - 4);
                    DEBUG_SHOW_MIN_MAX(&BS_touch_info_multi[idx_i].mM, MY_COLOR - 1, 0); //color, mode:1(show it at debug on)
                    DEBUG_SHOW_MIN_MAX(&BS_touch_info_multi[idx_j].mM, MY_COLOR - 2, 0); //color, mode:1(show it at debug on)
                    TRACE_NOP;
                };
#endif
                int tmpRet = multi_is_adjacent_tp_surrounded(ENUM_HOR_X, idx_i, idx_j);
                if (tmpRet)
                {
                    //sacn y-axis
                    for (k = 0; k < tpCntSort; k++) {
                        //largest one first
                        int idx_k = BS_touch_info_multi_sort[k];
                        if ((BS_touch_info_multi[idx_k].multiStat & MULTISTAT_REMOVED_MASK) != 0) continue; //removed
                        if (idx_k == idx_i) continue;
                        if (idx_k == idx_j) {
                            if (BS_touch_info_multi[idx_i].multiArea > TOUCH_FINGER_SIZESQ) {
                                break;
                            }
                            else {
                                continue; //nsmoon@200309
                            }
                        }
#if (DEBUG_MULTI_SMALL_SURROUNED_LARGE > 0)
                        IS_DEBUG_FLAG{
                        DEBUG_SHOW_MIN_MAX(&BS_touch_info_multi[idx_k].mM, MY_COLOR - 5, 0); //color, mode:1(show it at debug on)
                            TRACE_NOP;
                        };
#endif
                        int tmpRet2 = multi_is_adjacent_tp_surrounded(ENUM_VER_Y, idx_k, idx_j);
                        if (tmpRet2 != 0) {
                            BS_touch_info_multi[idx_j].multiStat |= MULTISTAT_GHOST; //ghost
                            IS_DEBUG_FLAG{ TRACE_CM(" *small surrounded..%d", idx_j); };
#if (DEBUG_MULTI_SMALL_SURROUNED_LARGE > 0)
                            IS_DEBUG_FLAG{
                                DEBUG_SHOW_MIN_MAX(&BS_touch_info_multi[idx_j].mM, MY_COLOR - 3, 0); //color, mode:1(show it at debug on)
                                TRACE_NOP;
                            };
#endif
                            break;
                        }
                    } //for (k = 0; k < touch_info_cnt; k++)
                } //if (tmpRet != 0)
            } //for (j = (tpCntSort -1); j > 0; j--)
        } //for (i = 0; i < tpCntSort; i++)
    } //if (tmpCnt > 2)
#if (DEBUG_BG_clipping_multi > 1) //for testing
    IS_DEBUG_FLAG{
        TRACE_CM("remove small tp surrounded large tp..");
        for (i = 0; i < touch_info_cnt; i++) {
            TRACE_CM(" %d:(0x%x)%d %0.1f", i, BS_touch_info_multi[i].multiStat, BS_touch_info_multi[i].polLen, BS_touch_info_multi[i].multiArea);
        }
        TRACE_NOP;
    };
#endif
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };
#endif //1

#ifdef MULTI_ERASER_MERGE //eraser & merge //nsmoon@200512 disable
#define DEBUG_ERASER_MERGE_ENABLE   0
    //should be clear info flags before using
    for (i = 0; i < touch_info_cnt; i++) {
        //if ((BS_touch_info_multi[i].multiStat & MULTISTAT_REMOVED_MASK) != 0) continue;
        BS_touch_info_multi[i].multiStat &= ~MULTISTAT_SKIP_MASK;
    }

    //for (i = 0; i < (touch_info_cnt - 1); i++)
    for (sortIdx = 0; sortIdx < tpCntSort; sortIdx++)
    {
        i = BS_touch_info_multi_sort[sortIdx];
        if ((BS_touch_info_multi[i].multiStat & MULTISTAT_SKIP) != 0) {
            continue; //skipped
        }
#if 1 //decide to skip recal min/max
        if (BS_touch_info_multi[i].multiArea > TOUCH_ERASER_SIZESQ_NORM) {
            BS_touch_info_multi[i].multiStat |= MULTISTAT_BIG; //big
        }
#endif

        //int sortIdx2;
        //for (sortIdx2 = (sortIdx+1); sortIdx2 < tpCntSort; sortIdx2++)
        for (j = 0; j < touch_info_cnt; j++)
        {
            //include removed touch
            //j = BS_touch_info_multi_sort[sortIdx2];
            if (i == j) continue;
            if ((BS_touch_info_multi[j].multiStat & MULTISTAT_SKIP) != 0) {
                continue; //skipped
            }
#if 0 //for test
            if (BS_touch_info_multi[j].multiArea < TOUCH_ERASER_SIZESQ_MIN) {
                BS_touch_info_multi[j].multiStat |= MULTISTAT_SMALL; //big
                continue;
            }
#endif
#if (DEBUG_ERASER_MERGE_ENABLE > 0) //def DRAW_POLYGON_TEST
            IS_DEBUG_FLAG{
            SEND_POLY(0, 0x21, MY_COLOR);
            DEBUG_SHOW_BS_TOUCH_POLYGON(touch_info_cnt, MY_COLOR -4);
            DEBUG_SHOW_MIN_MAX(&BS_touch_info_multi[i].mM, MY_COLOR-1, 0); //color, mode:1(show it at debug on)
            DEBUG_SHOW_MIN_MAX(&BS_touch_info_multi[j].mM, MY_COLOR - 2, 0); //color, mode:1(show it at debug on)
            TRACE_NOP;
            };
#endif	
        int merged = 0;
            if (BS_touch_info_multi[i].multiArea > TOUCH_ERASER_SIZESQ_MERGE) {
                if (multi_is_closed_center_merge(&BS_touch_info_multi[i], &BS_touch_info_multi[j])) {
                    BS_touch_info_multi[j].multiStat |= MULTISTAT_GHOST; //big
                    BS_touch_info_multi[j].multiStat |= MULTISTAT_SKIP; //small
                    widthX = GET_ABS(BS_touch_info_multi[i].mM.maxX - BS_touch_info_multi[i].mM.minX);
                    widthY = GET_ABS(BS_touch_info_multi[i].mM.maxY - BS_touch_info_multi[i].mM.minY);
                    BS_touch_info_multi[i].multiArea = widthX * widthY; //recal area
                    IS_DEBUG_FLAG{ TRACE("MERGED-1"); };
                    merged++;
                } //if (BS_touch_info_multi[i].multiArea > TOUCH_ERASER_SIZE_MERGE)
            }
            else if (BS_touch_info_multi[i].multiArea > TOUCH_ERASER_SIZESQ_NORM) {
                if (multi_is_closed_min_max_merge(&BS_touch_info_multi[i], &BS_touch_info_multi[j])) {
                    //IS_DEBUG_FLAG{ TRACE("multiArea= %0.1f %0.1f", BS_touch_info_multi[i].multiArea, BS_touch_info_multi[j].multiArea);};
                    BS_touch_info_multi[j].multiStat |= MULTISTAT_GHOST; //big
                    BS_touch_info_multi[j].multiStat |= MULTISTAT_SKIP; //small
                    widthX = GET_ABS(BS_touch_info_multi[i].mM.maxX - BS_touch_info_multi[i].mM.minX);
                    widthY = GET_ABS(BS_touch_info_multi[i].mM.maxY - BS_touch_info_multi[i].mM.minY);
                    BS_touch_info_multi[i].multiArea = widthX * widthY; //recal area
                    IS_DEBUG_FLAG{ TRACE("MERGED-2"); };
                    merged++;
                } //if (BS_touch_info_multi[i].multiArea > TOUCH_ERASER_SIZE_MERGE)
            }
        if (merged) {
#if (DEBUG_ERASER_MERGE_ENABLE > 0) //def DRAW_POLYGON_TEST
                IS_DEBUG_FLAG{
            DEBUG_SHOW_MIN_MAX(&BS_touch_info_multi[i].mM, 2, 0); //color, mode:1(show it at debug on)
            TRACE_NOP;
                };
#endif
                sortIdx--; //i--; //try again
                break;
        }
        } //for (j = 0; j < touch_info_cnt; j++)
    } //for (i = 0; i < touch_info_cnt; i++)
#endif

#ifdef MULTI_RANGE_RECAL_MIN_MAX //nsmoon@200107
    int retX, retY;
    pos_minMax2_t minMaxRetX, minMaxRetY;
    tpCntSort = touch_info_sort(0, touch_info_cnt);

    ////////////////////////////////////
    //recalculate min/max
    ////////////////////////////////////
    //for (i = 0; i < touch_info_cnt; i++)
    for (sortIdx = 0; sortIdx < tpCntSort; sortIdx++)
    {
        i = BS_touch_info_multi_sort[sortIdx];
        if ((BS_touch_info_multi[i].multiStat & MULTISTAT_REMOVED_MASK) != 0 ||
            (BS_touch_info_multi[i].multiStat & MULTISTAT_BIG) != 0) continue; //removed or big
        //if ((BS_touch_info_multi[i].multiStat & MULTISTAT_BIG) != 0) continue; //big
        minMaxRetX.min = 0; minMaxRetX.max = 0;
        minMaxRetY.min = 0; minMaxRetY.max = 0;
        retX = multi_recal_min_max(ENUM_HOR_X, &BS_touch_info_multi[i], &minMaxRetX);
        retY = multi_recal_min_max(ENUM_VER_Y, &BS_touch_info_multi[i], &minMaxRetY);
#if (DEBUG_BG_clipping_multi > 0) //def DRAW_POLYGON_TEST
        DEBUG_SHOW_MIN_MAX(&BS_touch_info_multi[i].mM, MY_COLOR-3, 0); //color, mode:1(show it at debug on)
        IS_DEBUG_FLAG{
            TRACE_NOP;
    };
#endif
#if 0 //(DEBUG_BG_clipping_multi > 0) //def DRAW_POLYGON_TEST
        DEBUG_SHOW_MIN_MAX(&BS_touch_info_multi[i].mM, 2, 0); //color, mode:1(show it at debug on)
#endif
        //should be saved min/max after processing X and Y
        if (retX == 0) {
            IS_DEBUG_FLAG{ TRACE_CM2("X::%d(%0.1f,%0.1f)=>(%0.1f,%0.1f)", i, BS_touch_info_multi[i].mM.minX, BS_touch_info_multi[i].mM.maxX, minMaxRetX.min, minMaxRetX.max);}
            BS_touch_info_multi[i].mM.minX = minMaxRetX.min;
            BS_touch_info_multi[i].mM.maxX = minMaxRetX.max;
        }
        if (retY == 0) {
            IS_DEBUG_FLAG{ TRACE_CM2("Y::%d(%0.1f,%0.1f)=>(%0.1f,%0.1f)", i, BS_touch_info_multi[i].mM.minY, BS_touch_info_multi[i].mM.maxY, minMaxRetY.min, minMaxRetY.max);}
            BS_touch_info_multi[i].mM.minY = minMaxRetY.min;
            BS_touch_info_multi[i].mM.maxY = minMaxRetY.max;
        }
#if 0 //(DEBUG_BG_clipping_multi > 0) //def DRAW_POLYGON_TEST
        DEBUG_SHOW_MIN_MAX(&BS_touch_info_multi[i].mM, MY_COLOR, 0); //color, mode:1(show it at debug on)
#endif
#if 1 //nsmoon@190628
        widthX = GET_ABS(BS_touch_info_multi[i].mM.maxX - BS_touch_info_multi[i].mM.minX);
        widthY = GET_ABS(BS_touch_info_multi[i].mM.maxY - BS_touch_info_multi[i].mM.minY);
        BS_touch_info_multi[i].multiArea = widthX * widthY; //update multiArea with width!!!
        //IS_DEBUG_FLAG{ TRACE_CM("retX,retY,widthX,widthY= %d(%d,%d)(%0.1f,%0.1f)", i, retX, retY, widthX, widthY); }
#endif
#if 0 //nsmoon@191216, FIXME, unused line problem!!!
        //TRACE_START_SIZE("widthX,retY,widthY,retX:%d(%0.1f,%d),(%0.1f,%d)", i , widthX, retY, widthY, retX);
        if ((widthX < MULTI_LIMIT_LIMIT_WIDTH && retY) || (widthY < MULTI_LIMIT_LIMIT_WIDTH && retX)) {
            BS_touch_info_multi[i].multiStat |= MULTISTAT_GHOST; //ghost
        }
#endif
        TRACE_NOP;
    }
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };
#endif //MULTI_RANGE_RECAL_MIN_MAX

    //SEND_POLY(0, 0x21, MY_COLOR);
#if 0 //for showing input lines
    BS_debug_inlines();
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };
#endif

    ////////////////////////////////////
    //calculate BG_touch_data
    ////////////////////////////////////
    //for (i = 0; i < touch_info_cnt; i++) {
    for (sortIdx = 0; sortIdx < tpCntSort; sortIdx++) {
        i = BS_touch_info_multi_sort[sortIdx];
        if ((BS_touch_info_multi[i].multiStat & MULTISTAT_REMOVED_MASK) != 0) {
            continue; //removed
        }
#if 1 //cal threshold count
        multi_recal_used_line(ENUM_HOR_X, &BS_touch_info_multi[i], 1); //1:enable calTh10
        multi_recal_used_line(ENUM_VER_Y, &BS_touch_info_multi[i], 1); //1:enable calTh10
#endif
        if (touchCntMulti < MAX_TOUCH_LIMIT_MULTI)
        {
            //BS_pclipping_info.touch_idx_saved[touchCntMulti] = tIdx;  //nsmoon@170202
#if 0 //def DRAW_POLYGON_TEST
            DEBUG_clipping_var_save_to_touch(touchCntMulti, rese);
#endif
#if (DEBUG_BG_clipping_multi > 0) //def DRAW_POLYGON_TEST
            DEBUG_SHOW_MIN_MAX(&BS_touch_info_multi[i].mM, MY_COLOR, 0); //color, mode:1(show it at debug on)
            IS_DEBUG_FLAG{
                TRACE_NOP;
            };
#endif
            centerPoint.x = (BS_touch_info_multi[i].mM.minX + BS_touch_info_multi[i].mM.maxX) * 0.5f;
            centerPoint.y = (BS_touch_info_multi[i].mM.minY + BS_touch_info_multi[i].mM.maxY) * 0.5f;
#if 0 //nsmoon@190325, FIXME
            centerPoint.x = GET_MAX(centerPoint.x, BS_aarea_zero_x);
            centerPoint.x = GET_MIN(centerPoint.x, BS_aarea_end_x);
            centerPoint.y = GET_MAX(centerPoint.y, BS_aarea_zero_y);
            centerPoint.y = GET_MIN(centerPoint.y, BS_aarea_end_y);
#endif
#if 1 //nsmoon@190325, FIXME
            if (IS_NOT_ACTIVE_AREA(centerPoint.x, centerPoint.y)) {
                IS_DEBUG_FLAG { TRACE("BG_clipping_multi_post..out of black area: %0.1f %0.1f", centerPoint.x, centerPoint.y); };
#ifdef HOR_EDGE_TOUCH_ENABLE
                if (multiLoopCnt == 1) {
                    BG_touch_data_edge.x = centerPoint.x;
                    BG_touch_data_edge.y = centerPoint.y;
                }
                else { //nsmoon@200504
                    int ipIdx;
                    //TRACE("BS_initial_polygon_cnt= %d", BS_initial_polygon_cnt);
#if 1 //nsmoon@210512 bug-fix
                    float xSize, ySize, area, areaMax = 0;
                    int areaMaxIdx = -1;
                    for (ipIdx = 0; ipIdx < BS_initial_polygon_cnt; ipIdx++) {
                        xSize = GET_ABS(BS_initial_polygon[ipIdx].pos.maxX + BS_initial_polygon[ipIdx].pos.minX);
                        ySize = GET_ABS(BS_initial_polygon[ipIdx].pos.maxY + BS_initial_polygon[ipIdx].pos.minY);
                        area = xSize * ySize;
                        if (area > areaMax) {
                            areaMax = area;
                            areaMaxIdx = ipIdx;
                        }
                    }
                    if (areaMaxIdx >= 0) {
                        centerPoint.x = (BS_touch_info_multi[areaMaxIdx].mM.minX + BS_touch_info_multi[areaMaxIdx].mM.maxX) * 0.5f;
                        centerPoint.y = (BS_touch_info_multi[areaMaxIdx].mM.minY + BS_touch_info_multi[areaMaxIdx].mM.maxY) * 0.5f;
                        if (IS_NOT_ACTIVE_AREA(centerPoint.x, centerPoint.y)) {
                            BG_touch_data_edge.x = centerPoint.x;
                            BG_touch_data_edge.y = centerPoint.y;
                        }
                        else {
                            //TRACE("=> %0.1f %0.1f", BG_touch_data_edge.x, BG_touch_data_edge.y);
                            BG_touch_data_edge.x = 0;
                            BG_touch_data_edge.y = 0;
                        }
                    }
                    else {
                        BG_touch_data_edge.x = 0;
                        BG_touch_data_edge.y = 0;
                    }
#else
                    for (ipIdx = 0; ipIdx < BS_initial_polygon_cnt; ipIdx++) {
                        BG_touch_data_edge.x = (BS_initial_polygon[ipIdx].pos.minX + BS_initial_polygon[ipIdx].pos.maxX) * 0.5f;
                        BG_touch_data_edge.y = (BS_initial_polygon[ipIdx].pos.minY + BS_initial_polygon[ipIdx].pos.maxY) * 0.5f;
                        if (IS_NOT_ACTIVE_AREA(centerPoint.x, centerPoint.y)) {
                            break;
                        }
                        else {
                            //TRACE("=> %0.1f %0.1f", BG_touch_data_edge.x, BG_touch_data_edge.y);
                            BG_touch_data_edge.x = 0;
                            BG_touch_data_edge.y = 0;
                        }
                    }
#endif
                }
#endif
                continue;
            }
#endif
#if defined(DEBUG_FUNCTION_ENABLE_RELEASE)
            DEBUG_SHOW_CENT_GRID(&centerPoint, 5, 2);
            DEBUG_SHOW_MIN_MAX(&BS_touch_info_multi[i].mM, 1/*MY_COLOR*/, 0);
#endif
            BG_touch_data[touchCntMulti] = centerPoint;
            BG_touch_area[touchCntMulti] = BS_touch_info_multi[i].multiArea;
            //BG_touch_size[touchCntMulti].xSize = BS_touch_info_multi[i].mM.maxX - BS_touch_info_multi[i].mM.minX + 1;
            //BG_touch_size[touchCntMulti].ySize = BS_touch_info_multi[i].mM.maxY - BS_touch_info_multi[i].mM.minY + 1;
            widthX = GET_ABS(BS_touch_info_multi[i].mM.maxX - BS_touch_info_multi[i].mM.minX);
            widthY = GET_ABS(BS_touch_info_multi[i].mM.maxY - BS_touch_info_multi[i].mM.minY);
#ifdef FRONTEND_LINE_THRESHOLD
            if (BS_touch_info_multi[i].th10CntX > 0) {
                widthX10 = GET_ABS(BS_touch_info_multi[i].mM2.maxX - BS_touch_info_multi[i].mM2.minX);
            }
            else {
                widthX10 = 0;
            }
            if (BS_touch_info_multi[i].th10CntY > 0) {
                widthY10 = GET_ABS(BS_touch_info_multi[i].mM2.maxY - BS_touch_info_multi[i].mM2.minY);
            }
            else {
                widthY10 = 0;
            }
#endif
#if 1 //adjust near eraser, nsmoon@200310
            IS_DEBUG_FLAG {
                TRACE_NOP;
            };
//#define MULTI_MARKER_SIZE_MAX       90.0f
#define MULTI_MARKER_WIDTH_MIN      2.0f //3.0f nsmoon@200923 3.0=>2.0
#define MULTI_MARKER_WIDTH_MAX      8.0f
#define MULTI_MARKER_SIZE_RATIO_MIN 2.0f
#define MULTI_PEN_WIDTH_MAX         3.0f //nsmoon@210317
#if (MODEL_SPT == MODEL_CTSO_430_V100)
#define MULTI_MARKER_ADJ_SIZE_RATIO 1.2f //nsmoon@210308
#else
#define MULTI_MARKER_ADJ_SIZE_RATIO 1.5f
#endif
            float widthRatio = (widthX > widthY) ? (widthX / widthY) : (widthY / widthX);
            float minSize = GET_MIN(widthX, widthY); //nsmoon@200602
            float adjSizeRatio = MULTI_MARKER_ADJ_SIZE_RATIO;
#if (MODEL_SPT == MODEL_CTSK_850_V100) || (MODEL_SPT == MODEL_CTKS_750_V140) || (MODEL_SPT == MODEL_CTSI_550_SINGLE_V100) || (MODEL_SPT == MODEL_CTSI_650_V100) || \
            (MODEL_SPT == MODEL_CTSI_N750_V100) || (MODEL_SPT == MODEL_CTSI_N650_V100) //nsmoon@230411
            if (BS_touch_info_multi[i].no_clip_y) {
                if (minSize > MULTI_PEN_WIDTH_MAX) {
                    adjSizeRatio = MULTI_MARKER_ADJ_SIZE_RATIO;
                }
                else {
                    adjSizeRatio = 1.0f;
                }
            }
#else
            adjSizeRatio = (BS_touch_info_multi[i].no_clip_y) ? 1.0f : MULTI_MARKER_ADJ_SIZE_RATIO; //nsmoon@210308
#endif
            //TRACE_SIZE_ADJ("minSize,widthRatio= (%d)%0.1f/%0.2f/%0.1f(%0.1f,%0.1f)", i, minSize, widthRatio, adjSizeRatio, widthX, widthY);
            //if (BS_touch_info_multi[i].multiArea < MULTI_MARKER_SIZE_MAX)
            if (minSize > MULTI_MARKER_WIDTH_MIN && minSize < MULTI_MARKER_WIDTH_MAX)
            {
                if (multi_is_near_eraser(i)) {
                    if (widthRatio > MULTI_MARKER_ADJ_SIZE_RATIO) {
                        if (widthX > widthY) {
                            widthX = widthY * adjSizeRatio;
                            widthX10 = widthY10 * adjSizeRatio; //nsmoon@200425, FIXME
                        }
                        else {
                            widthY = widthX * adjSizeRatio;
                            widthY10 = widthX10 * adjSizeRatio; //nsmoon@200425, FIXME
                        }
                        TRACE_SIZE_ADJ("=>adjust multi: %0.1f,%0.1f %0.1f,%0.1f", widthX, widthY, widthX10, widthY10);
                    }
                }
            }
#endif
            BG_touch_size[touchCntMulti].xSize = widthX;
            BG_touch_size[touchCntMulti].ySize = widthY;
            BG_multi_fine[touchCntMulti] = 1; //for debugging
#ifdef FRONTEND_LINE_THRESHOLD
            BG_touch_lineCnt[touchCntMulti].th50CntX = BS_touch_info_multi[i].th50CntX;
            BG_touch_lineCnt[touchCntMulti].th10CntX = BS_touch_info_multi[i].th10CntX;
            BG_touch_lineCnt[touchCntMulti].th50CntY = BS_touch_info_multi[i].th50CntY;
            BG_touch_lineCnt[touchCntMulti].th10CntY = BS_touch_info_multi[i].th10CntY;
            BG_touch_lineCnt[touchCntMulti].th10WidthX = widthX10;
            BG_touch_lineCnt[touchCntMulti].th10WidthY = widthY10;
#endif
//#ifdef FRONTEND_LINE_THRESHOLD
            //BG_touch_threshold[touchCntMulti] = 0; //FIXME
//#endif
            touchCntMulti++;
#if (DEBUG_BG_clipping_multi > 0) //def DRAW_POLYGON_TEST //center point for testing
            IS_DEBUG_FLAG
            {
        #ifdef DRAW_POLYGON_TEST
            poly_e_t debug_poly;
            debug_poly.ve = &BS_debugVert[0];
            debug_poly.len = INITIAL_POLYGON_VERTEX_NO;
            debug_poly.ve[0].vert.x = centerPoint.x - DEBUG_POS_WIDTH;
            debug_poly.ve[0].vert.y = centerPoint.y - DEBUG_POS_WIDTH;
            debug_poly.ve[1].vert.x = centerPoint.x - DEBUG_POS_WIDTH;
            debug_poly.ve[1].vert.y = centerPoint.y + DEBUG_POS_WIDTH;
            debug_poly.ve[2].vert.x = centerPoint.x + DEBUG_POS_WIDTH;
            debug_poly.ve[2].vert.y = centerPoint.y + DEBUG_POS_WIDTH;
            debug_poly.ve[3].vert.x = centerPoint.x + DEBUG_POS_WIDTH;
            debug_poly.ve[3].vert.y = centerPoint.y - DEBUG_POS_WIDTH;
            SEND_POLY(&debug_poly, 0x11, MY_COLOR);
        #endif
            }; //nsmoon@170125
#endif
        }
        else {
            //TRACE_ERROR("ERROR! invalid touchCntMulti(2) %d [%d]", touchCntMulti, BG_frame_no);
            TRACE_ERROR("OTC");
            break;
        }
    } //for (i = 0; i < touch_info_cnt; i++) {
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };

//L_BG_clipping_multi_01:
#if (DEBUG_BG_clipping_multi > 0) //def DRAW_POLYGON_TEST
    TRACE_CM2("touchCntMulti= %d (%d) [%d]", touchCntMulti, touch_info_cnt, BG_frame_no);
    IS_DEBUG_FLAG{
        TRACE_NOP;
    };
#endif

#ifdef FINE_RULE_1A_TEST
    if (touchCntMulti > 0) {
        for (i = 0; i < touchCntMulti; i++) {
            BG_debug_touch_data[i] = BG_touch_data[i];
            BG_debug_touch_area[i] = BG_touch_area[i];
            BG_debug_touch_size[i] = BG_touch_size[i];
            BG_debug_multi_fine[i] = BG_multi_fine[i];
            BG_debug_touch_area_minmax[i].min = GET_MIN(BG_debug_touch_area_minmax[i].min, BG_touch_area[i]);
            BG_debug_touch_area_minmax[i].max = GET_MAX(BG_debug_touch_area_minmax[i].max, BG_touch_area[i]);
        }
    }
#endif

    //BS_adj_used_lines_tp(touchCntOrg, touchCntMulti, ADJUST_USED_LINE_MULTI); //adjust used line, nsmoon@200331 //BG_clipping_multi_post
    BG_touch_count_multi = touchCntMulti - touchCntOrg;
    if (BG_touch_count_multi > 0) { //nsmoon@220118
    BS_adj_used_lines_tp(touchCntOrg, touchCntMulti, ADJUST_USED_LINE_MULTI); //adjust used line, nsmoon@200331 //BG_clipping_multi_post
    }
    BG_touch_count = touchCntMulti;
    return BG_touch_count_multi;
}

//////////////////////////////////////////////////////////

ATTR_BACKEND_RAMFUNC
void BG_epaInit(void)
{
    int i;
    ep_buf_t *epaX = &BS_edge_pattern_x[0];
    ep_buf_t *epaY = &BS_edge_pattern_y[0];

    //TRACE("BG_epaInit...");
    BS_edge_pattern_ep_cnt = 0; //init

    //x-axis
    //for (i = 0; i < MAX_INIT_EPA_SIZE; i++)
    for (i = 0; i < EPA_IDX_LEN; i++)
    {
        epaX[i].len = 0;
#ifdef USE_CUST_MALLOC //nsmoon@201012
        if ((BS_edge_pattern_ep_cnt + MAX_INIT_EP_COUNT) < BS_max_ep_count) { //nsmoon@201014
#else
        if ((BS_edge_pattern_ep_cnt + MAX_INIT_EP_COUNT) < MAX_EP_COUNT) {
#endif
            epaX[i].ep = &BS_edge_pattern_ep[BS_edge_pattern_ep_cnt];
#ifndef USE_CUST_MALLOC //nsmoon@201012
            epaX[i].poly = &BS_edge_pattern_poly[BS_edge_pattern_ep_cnt]; //nsmoon@201008
#endif
            BS_edge_pattern_ep_cnt += MAX_INIT_EP_COUNT;
        }
        else {
            TRACE_ERROR("ERROR! X invalid BS_edge_pattern_ep_cnt %d", BS_edge_pattern_ep_cnt);
            break;
        }
        //TRACE("&epaX[].ep[0]: (%d) %x", i, &epaX[i].ep[0]);
    }

    //y-axis
    //for (i = 0; i < MAX_INIT_EPA_SIZE; i++)
    for (i = 0; i < EPA_IDX_LEN; i++)
    {
        epaY[i].len = 0;
#ifdef USE_CUST_MALLOC //nsmoon@201012
        if ((BS_edge_pattern_ep_cnt + MAX_INIT_EP_COUNT) < BS_max_ep_count) {  //nsmoon@201014
#else
        if ((BS_edge_pattern_ep_cnt + MAX_INIT_EP_COUNT) < MAX_EP_COUNT) {
#endif
            epaY[i].ep = &BS_edge_pattern_ep[BS_edge_pattern_ep_cnt];
#ifndef USE_CUST_MALLOC //nsmoon@201012
            epaY[i].poly = &BS_edge_pattern_poly[BS_edge_pattern_ep_cnt]; //nsmoon@201008
#endif
            BS_edge_pattern_ep_cnt += MAX_INIT_EP_COUNT;
        }
        else {
            TRACE_ERROR("ERROR! Y invalid BS_edge_pattern_ep_cnt %d", BS_edge_pattern_ep_cnt);
            break;
        }
        //TRACE("&epaY[].ep[0]: (%d) %x", i, &epaY[i].ep[0]);
    }
}

ATTR_BACKEND_RAMFUNC
side_type_t BS_getSideFromPos(vec_t *pos)
{
    side_type_t side;
    if (pos->x < BS_half_end_x && pos->y < BS_half_end_y) {
        side = ENUM_BOT_RHT;
    }
    else if (pos->x < BS_half_end_x && pos->y > BS_half_end_y) {
        side = ENUM_TOP_RHT;
    }
    else if (pos->x > BS_half_end_x && pos->y > BS_half_end_y) {
        side = ENUM_TOP_LFT;
    }
    else {
        side = ENUM_BOT_LFT;
    }
    return side;
}
// *****************************************************************************
/*End of File*/
