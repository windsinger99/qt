/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Lispect

  @File Name
    backend_debug.h

  @Summary
    debug definitions for backend

  @Description
    Describe the purpose of this file.
*/
/* ************************************************************************** */
#ifndef _BACKEND_DEBUG_H    /* Guard against multiple inclusion */
#define _BACKEND_DEBUG_H

#include "backend_common.h"
#include "backend_utils.h"

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

//#define DEBUG_GET_TIME_DIFF_ENABLE //for test only //nsmoon@220119

//For Debugging
#if defined(_WIN32) || defined(WIN32)
#if (_DEBUG == 1) || defined(DEBUG) || ((DEBUG_TOOL_QT == 1) && !defined(QT_NO_DEBUG))
#ifndef DEBUG_FRAME_NO
#define DEBUG_FRAME_NO      0
#endif
#if defined(QT_NO_DEBUG) && (QT_NO_DEBUG == 1)
//#error "Release Build"
#else
//#error "Debug Build"
#endif

#ifndef DEBUG_FRAME_PAUSE
#if (DEBUG_FRAME_NO > 0)
#if (DEBUG_TOOL_QT == 1) && !defined(QT_NO_DEBUG)
#define DEBUG_FRAME_PAUSE   1 //1:enable
#else
#define DEBUG_FRAME_PAUSE   0 //1:enable
#endif
#else
#if (DEBUG_TOOL_QT == 1) && !defined(QT_NO_DEBUG)
#define DEBUG_FRAME_PAUSE   1 //1:enable
#else
#define DEBUG_FRAME_PAUSE   0
#endif
#endif
#endif
//#define DEBUG_SKIP_FRAME_NO (DEBUG_FRAME_NO - 1)
#define DEBUG_MESSAGE_ENABLE
#define DEBUG_EP_TEST   //nsmoon@210216
#else
#define DEBUG_FRAME_NO      0
#define DEBUG_FRAME_PAUSE   0 //1:enable
//#define TRACE_SAVE_ERROR_MSG
//#define DEBUG_MESSAGE_ENABLE
#endif

//#define BRUSH_MODE_TEST_ON  //enable brush test mode
#if (_DEBUG == 1)
#define DRAW_POLY_ONE_BY_ONE
#endif
#if (DEBUG_TOOL_QT == 1)
#ifdef DEBUG_MESSAGE_ENABLE
#define DEBUG_FUNCTION_ENABLE_ALL      //nsmoon@191226
#endif //DEBUG_MESSAGE_ENABLE
#define DEBUG_FUNCTION_ENABLE_RELEASE  //nsmoon@200114
#else
#define DEBUG_FUNCTION_ENABLE_ALL      //nsmoon@191226
#define DEBUG_FUNCTION_ENABLE_RELEASE  //nsmoon@200114
#endif
#else //defined(_WIN32) || defined(WIN32)
#define DEBUG_FRAME_NO      0
#define DEBUG_FRAME_PAUSE   0 //1 //1:enable
#if (_DEBUG == 1) || defined(DEBUG)
#define DEBUG_MESSAGE_ENABLE
#define DEBUG_FUNCTION_ENABLE_ALL
#endif
#endif

//----------------------------------------------------------------------
// DRAW_LINE_TEST
#if (DRAW_LINE_TEST == 1)
//#define FORCED_NUM_PD //for inverse test
#endif

//----------------------------------------------------------------------
// DRAW_POLYGON_TEST
#ifdef DRAW_POLYGON_TEST
  #define DPT_LINE_DATA      // PC can receive line data
  //#define DPT_POLYGON_DATA      // PC can receive polygon data
#endif
#ifdef DPT_LINE_DATA
  #define DRAW_EP_TEST_01 // Draw EP test no.01
#endif
//#define DRAW_POLYGON_TEST2  //draw polygon test

////////////////////////////////////////////
// DEBUG functions
//#define MAX_DEBUG_VERT	(MAX_SLOPE_COUNT * 2)
#ifdef DRAW_POLYGON_TEST
#define DEBUG_COLOR                     (debug_color % (MY_COLOR-3))
#define DEBUG_COLOR_CHANGE              (debug_color++)
#define DEBUG_SHOW_POS                  DEBUG_show_pos_
#define DEBUG_SHOW_LINE_PD_LED          DEBUG_show_line_pd_led_
#define DEBUG_SHOW_LINE_IDX             DEBUG_show_line_idx_
#define DEBUG_SHOW_LINE_POS             DEBUG_show_line_pos_
#define DEBUG_SHOW_LINE_BELONGED        DEBUG_show_line_belonged_
#define DEBUG_SHOW_LINE_AXIS_POS        DEBUG_show_line_axis_pos_
#define DEBUG_SHOW_MIN_MAX              DEBUG_show_min_max_
#define DEBUG_SHOW_MIN_MAX_POS          DEBUG_show_min_max_pos_
#define DEBUG_SHOW_BS_POLYGON           DEBUG_show_BS_polygon_
#define DEBUG_SHOW_BS_TOUCH_POLYGON		DEBUG_show_BS_touch_polygon_
#define DEBUG_SHOW_BS_TOUCH_POLYGON_ONE	DEBUG_show_BS_touch_polygon_one_
#define DEBUG_SHOW_POLY                 DEBUG_show_poly_
#define DEBUG_SHOW_START_END            DEBUG_show_start_end_
#define DEBUG_SHOW_BS_INITIAL_POLYGON	DEBUG_show_BS_initial_polygon_
#define DEBUG_SHOW_BS_SUBJECT_POLYGON	DEBUG_show_BS_subject_polygon_
#define DEBUG_SHOW_CENT_GRID            DEBUG_show_cent_grid_
#define DEBUG_SHOW_TOUCH_POINT          DEBUG_show_touch_point_
#else
#define DEBUG_COLOR(...)
#define DEBUG_COLOR_CHANGE(...)
#define DEBUG_SHOW_POS(...)
#define DEBUG_SHOW_LINE_PD_LED(...)
#define DEBUG_SHOW_LINE_IDX(...)
#define DEBUG_SHOW_LINE_POS(...)
#define DEBUG_SHOW_LINE_BELONGED(...)
#define DEBUG_SHOW_LINE_AXIS_POS(...)
#define DEBUG_SHOW_MIN_MAX(...)
#define DEBUG_SHOW_MIN_MAX_POS(...)
#define DEBUG_SHOW_BS_POLYGON(...)
#define DEBUG_SHOW_BS_TOUCH_POLYGON(...)
#define DEBUG_SHOW_BS_TOUCH_POLYGON_ONE(...)
#define DEBUG_SHOW_POLY(...)
#define DEBUG_SHOW_START_END(...)
#define DEBUG_SHOW_BS_INITIAL_POLYGON(...)
#define DEBUG_SHOW_BS_SUBJECT_POLYGON(...)
#define DEBUG_SHOW_TOUCH_POINT
#endif

#ifdef DEBUG_GET_TIME_DIFF_ENABLE  //nsmoon@220119
#define DEBUG_GET_TIME_DIFF_INIT()        DEBUG_get_time_diff_init()
#define DEBUG_GET_TIME_DIFF_SHOW()	      DEBUG_slow_time_diff_show()
#define DEBUG_GET_TIME_DIFF(...)	      DEBUG_get_time_diff(__VA_ARGS__)
#else
#define DEBUG_GET_TIME_DIFF_INIT()
#define DEBUG_GET_TIME_DIFF_SHOW()
#define DEBUG_GET_TIME_DIFF(...)
#endif

#define DEBUG_TIME_DIFF_STR_LEN     4
#define DEBUG_TIME_DIFF_START       "@"
#define DEBUG_TIME_DIFF_MULTI       "M"
#define DEBUG_TIME_DIFF_MULTI_PRE   "M0"
#define DEBUG_TIME_DIFF_MULTI_POST  "M1"
#define DEBUG_TIME_DIFF_FINE        "F"
#define DEBUG_TIME_DIFF_FINE_PRE    "F0"
#define DEBUG_TIME_DIFF_XY          "X"
#define DEBUG_TIME_DIFF_XY_PRE      "X0"
#define DEBUG_TIME_DIFF_SHADOW      "S"
#define DEBUG_TIME_DIFF_SHADOW_PRE  "S0"
#define DEBUG_TIME_DIFF_EDGE        "E"
#define DEBUG_TIME_DIFF_EDGE_PRE    "E0"
#define DEBUG_TIME_DIFF_BRUSH       "B"
#define DEBUG_TIME_DIFF_BRUSH_PRE   "B0"
#define DEBUG_TIME_DIFF_END         "$"
#define DEBUG_TIME_DIFF_MAX         10

/////////////////////////////////////////////////////
// For Debugging
//
#define MY_COLOR  30
#ifdef USE_CUST_MALLOC //nsmoon@201012
#define MAX_SLOPE_COUNT    ((0xFF / 2) - 1) //nsmoon@201022
#endif

#if defined(_WIN32) || defined(WIN32)  //FIXME nsmoon@170111
#include <windows.h>

#if (DEBUG_TOOL_QT == 1)
//extern unsigned long debug_cnt;
extern void _myTrace(char const *format, ...);
extern void _myTraceNR(char const *format, ...);
//extern void _myTraceWrite(char const *format, ...);
//extern void _myTraceWriteNR(char const *format, ...);
extern int BG_debug_flag, BG_debug_flag2, BS_debug_nop;
//extern uint32_t DEBUG_get_tick_cnt(int mode);
//#define DEBUG_TICK_COUNT(a)    DEBUG_get_tick_cnt(a)
#define IS_DEBUG_FLAG    if(BG_debug_flag)
#define IS_DEBUG_FLAG2    if(BG_debug_flag2)
#define SET_DEBUG_FLAG2   (BG_debug_flag2 = 1)
#define RESET_DEBUG_FLAG2   (BG_debug_flag2 = 0)
#define IS_NOT_DEBUG_FLAG    if(!BG_debug_flag)
#ifdef DEBUG_MESSAGE_ENABLE
#define TRACE(...) _myTrace(__VA_ARGS__) //_myTraceWrite
#define TRACENR(...) _myTraceNR(__VA_ARGS__) //_myTraceWriteNR
#define TRACE_ERROR(...) _myTrace(__VA_ARGS__) //_myTraceWrite
#define TRACENR_ERROR(...) _myTraceNR(__VA_ARGS__) //_myTraceWrite
#define TRACE_ERROR_MEM(...) _myTrace(__VA_ARGS__)
#define TRACE_NOP (BS_debug_nop++)
#define TRACE_START_SIZE(...) _myTrace(__VA_ARGS__) //_myTraceWrite
#define TRACE_RELEASE(...) _myTrace(__VA_ARGS__)
#define TRACE_MALLOC(...) _myTrace(__VA_ARGS__)
#define TRACE_SIZE_ADJ(...) _myTrace(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACENR(...)
#define TRACE_ERROR(...) _myTrace(__VA_ARGS__)
#define TRACENR_ERROR(...)
#define TRACE_ERROR_MEM(...) _myTrace(__VA_ARGS__)
#define TRACE_NOP
#define TRACE_START_SIZE(...) //_myTrace(__VA_ARGS__) //_myTraceWrite
#define TRACE_RELEASE(...) _myTrace(__VA_ARGS__)
#define TRACE_MALLOC(...) //_myTrace(__VA_ARGS__)
#define TRACE_SIZE_ADJ(...)
#endif
#define START_STOP_POLY_TRACE(a) _myDrawPolygon(0, (a), 0)
#define SEND_POLY(...) _myDrawPolygon(__VA_ARGS__)
#define TRACE_POLYGON(...)  _myDrawPolygon(__VA_ARGS__)
#define TRACE_POLYGON_MULTI(...)     _myDrawPolygon(__VA_ARGS__)
#define TRACE_POLYGON_POLYCLIP(...)  //_myDrawPolygon(__VA_ARGS__)
#define TRACE_POLYGON_TMP(...)    //_myDrawPolygon //debug unbalance edge pattern
#define SEND_TOUCH //_myDrawTouch

#if 1 //nsmoon@181001
#define MAX_PS_SIZE (4096*50)
#define PS_NUSED    0
#define PS_START    0xFD
#define PS_END      0xFE
#define PS_FLUSH    0x01
#define PS_APPEND   0x10
#define PS_RESTART  0x20

typedef struct {
    int len;
    int flag;
    unsigned long color;
    vec_t vert[MAX_SLOPE_COUNT];
} polygon_tmp_t;

extern polygon_tmp_t polygon_saved[MAX_PS_SIZE];
extern int polgon_saved_idx;
extern int bStartAddPol;

extern int _myDrawPolygon(poly_e res, uint8_t flag, uint8_t color);
extern int is_myDrawPolygon(void);
#endif

#else //DEBUG_TOOL_QT
//#include <windows.h>
//extern unsigned long debug_cnt;
extern void _myTrace(char const *format, ...);
extern void _myTraceNR(char const *format, ...);
extern void _myTraceWrite(char const *format, ...);
extern void _myTraceWriteNR(char const *format, ...);
extern int _myDrawPolygon(poly_e res, uint8_t flag, uint8_t color);
//extern BOOLEAN _myDrawPolygon2(poly_e subj, poly_e clip);
//extern BOOLEAN _myDrawEdgePtn2(uint16_t pd, uint16_t led, int8_t slope, uint8_t flag, uint8_t color);
//extern BOOLEAN _myDrawTouch(vec touchData, uint8_t flag, uint8_t color);

#if !defined(TRACE_SAVE_ERROR_MSG)
extern int BG_debug_flag, BG_debug_flag2, BS_debug_nop;
#define IS_DEBUG_FLAG    if(BG_debug_flag)
#define IS_DEBUG_FLAG2    if(BG_debug_flag2)
#define SET_DEBUG_FLAG2   (BG_debug_flag2 = 1)
#define RESET_DEBUG_FLAG2   (BG_debug_flag2 = 0)
#define IS_NOT_DEBUG_FLAG    if(!BG_debug_flag)
#define TRACE(...) _myTrace(__VA_ARGS__)
#define TRACENR(...) _myTraceNR(__VA_ARGS__)
#define TRACE_ERROR(...) _myTrace(__VA_ARGS__)
#define TRACENR_ERROR(...) _myTraceNR(__VA_ARGS__)
#define TRACE_ERROR_MEM(...) _myTrace(__VA_ARGS__)
#define TRACE_START_SIZE(...) _myTrace(__VA_ARGS__)
#define TRACE_NOP (BS_debug_nop++)
//#define START_STOP_EDGEPTN_TRACE(a) //_myDrawEdgePtn2(0, 0, 0, a, 0)
#define TRACE_EDGEPTN(...) //_myDrawEdgePtn2(__VA_ARGS__)
#define TRACE_EDGEPTN2(...) //_myDrawEdgePtn2(__VA_ARGS__)
#define TRACE_MALLOC(...) _myTrace(__VA_ARGS__)
#define TRACE_SIZE_ADJ(...) _myTrace(__VA_ARGS__)
#define START_STOP_POLY_TRACE(a) _myDrawPolygon(0, (a), 0)
#define SEND_POLY(...) _myDrawPolygon(__VA_ARGS__)
#define TRACE_POLYGON(...)  _myDrawPolygon(__VA_ARGS__)
#define TRACE_POLYGON_MULTI(...)     _myDrawPolygon(__VA_ARGS__)
#define TRACE_POLYGON_POLYCLIP(...)  //_myDrawPolygon(__VA_ARGS__)
#define TRACE_POLYGON_TMP(...)    //_myDrawPolygon(__VA_ARGS__) //debug unbalance edge pattern
#define SEND_TOUCH(...) //_myDrawTouch(__VA_ARGS__)
#define TRACE_RELEASE(...)
#else
#define IS_DEBUG_FLAG    if(0)
#define IS_DEBUG_FLAG2   if(0)
#define SET_DEBUG_FLAG2
#define RESET_DEBUG_FLAG2
#define IS_NOT_DEBUG_FLAG	if(0)
#define TRACE(...) _myTraceWrite(__VA_ARGS__)
#define TRACENR(...) _myTraceWriteNR(__VA_ARGS__)
#define TRACE_ERROR(...) _myTraceWrite(__VA_ARGS__)
#define TRACENR_ERROR(...) _myTraceWriteNR(__VA_ARGS__)
#define TRACE_ERROR_MEM(...) //_myTraceWrite(__VA_ARGS__)
#define TRACE_START_SIZE(...) //_myTrace(__VA_ARGS__)
#define TRACE_NOP
#define START_STOP_EDGEPTN_TRACE(...)
#define TRACE_EDGEPTN(...)
#define TRACE_EDGEPTN2(...)
#define TRACE_MALLOC(...) _myTrace(__VA_ARGS__)
#define TRACE_SIZE_ADJ(...)
#define START_STOP_POLY_TRACE(...)
#define SEND_POLY(...)
#define TRACE_POLYGON(...)
#define TRACE_POLYGON_MULTI(...)
#define TRACE_POLYGON_POLYCLIP(...)
#define TRACE_POLYGON_TMP(...)
#define SEND_TOUCH(...)
#define TRACE_RELEASE(...)
#endif //_DEBUG
#endif //DEBUG_TOOL_QT
extern const char *myGetLastErrorString();
extern const char *myGetLastErrorString2(DWORD error);

#else //defined(_WIN32) || defined(WIN32)
#define PS_START        0xFD
#define PS_END          0xFE
#define IS_DEBUG_FLAG   //if(0)
#define IS_DEBUG_FLAG2  //if(0)
#define SET_DEBUG_FLAG2
#define RESET_DEBUG_FLAG2
#define IS_NOT_DEBUG_FLAG	if(1)
#define TRACE_NOP
#ifdef DEBUG_MESSAGE_ENABLE //for debug
#if (MODEL_MCU == PIC_32)
//extern void SYS_DEBUG_Print(const char *format, ...);
extern int printf(const char *format, ...);
#define TRACE(...) printf(__VA_ARGS__);printf("\r\n")
#define TRACENR(...) printf(__VA_ARGS__)
#define TRACE_ERROR(...) printf(__VA_ARGS__);printf("\r\n")
#define TRACE_ERROR_MEM(...) printf(__VA_ARGS__);printf("\r\n")
#define TRACE_MALLOC(...) printf(__VA_ARGS__);printf("\r\n")
#define TRACE_SIZE_ADJ(...) printf(__VA_ARGS__);printf("\r\n")
#elif (MODEL_MCU == STM32H7)
#define TRACE(...) printf(__VA_ARGS__)
#define TRACENR(...) printf(__VA_ARGS__)
#define TRACE_ERROR(...) printf(__VA_ARGS__)
#define TRACE_ERROR_MEM(...) //printf(__VA_ARGS__)
#define TRACE_MALLOC(...) SYS_DEBUG_Print(__VA_ARGS__)
#elif (MODEL_MCU == AM335X)
extern void UARTprintf(const char *pcString, ...);
#define TRACE(...)              //UARTprintf(__VA_ARGS__)
#define TRACENR(...)            //UARTprintf(__VA_ARGS__)
#define TRACE_ERROR(...)        UARTprintf(__VA_ARGS__)
#define TRACE_ERROR_MEM(...)    UARTprintf(__VA_ARGS__)
#define TRACE_MALLOC(...)
#elif (MODEL_MCU == RT1052) || (MODEL_MCU == RT1064)
extern int DbgConsole_Printf(const char *formatString, ...);
#if 0 //for test
#define TRACE(...)
#define TRACENR(...)
#define TRACE_ERROR(...)
#define TRACE_ERROR_MEM(...)
#define TRACE_START_SIZE(...)
#define TRACEYY(...)		DbgConsole_Printf(__VA_ARGS__);DbgConsole_Printf("\r\n")
#define TRACE_MALLOC(...) SYS_DEBUG_Print(__VA_ARGS__)
#else
#define TRACE(...) DbgConsole_Printf(__VA_ARGS__);DbgConsole_Printf("\r\n")
#define TRACENR(...) DbgConsole_Printf(__VA_ARGS__)
#define TRACE_ERROR(...) DbgConsole_Printf(__VA_ARGS__);DbgConsole_Printf("\r\n")
#define TRACE_ERROR_MEM(...) DbgConsole_Printf(__VA_ARGS__);DbgConsole_Printf("\r\n")
#define TRACE_START_SIZE(...) DbgConsole_Printf(__VA_ARGS__); //DbgConsole_Printf("\r\n")
#define TRACEYY(...)
#define TRACE_RELEASE(...)
#define TRACE_MALLOC(...) DbgConsole_Printf(__VA_ARGS__);DbgConsole_Printf("\r\n")
#define TRACE_SIZE_ADJ(...) DbgConsole_Printf(__VA_ARGS__);DbgConsole_Printf("\r\n")
#endif //1
#endif
#else //DEBUG_MESSAGE_ENABLE
#define TRACE(...)
#define TRACENR(...)
#define TRACEYY(...)
#if 0 //(MODEL_MCU == RT1052) || (MODEL_MCU == RT1064)
extern int DbgConsole_Printf(const char *formatString, ...);
#define TRACE_ERROR(...) DbgConsole_Printf(__VA_ARGS__);DbgConsole_Printf("\r\n")
#define TRACE_ERROR_MEM(...) DbgConsole_Printf(__VA_ARGS__);DbgConsole_Printf("\r\n")
#define TRACE_START_SIZE(...) //DbgConsole_Printf(__VA_ARGS__);
#define TRACE_RELEASE(...)
#define TRACE_MALLOC(...)
#define TRACE_SIZE_ADJ(...) DbgConsole_Printf(__VA_ARGS__);DbgConsole_Printf("\r\n")
#else
#if (MODEL_MCU == AM335X)
extern void UARTprintf(const char *pcString, ...);
#define TRACE_ERROR(...)        UARTprintf(__VA_ARGS__)
#define TRACE_ERROR_MEM(...)    UARTprintf(__VA_ARGS__)
#else
#ifdef TRACE_ERROR_ENABLE //nsmoon@211126
extern int DbgConsole_Printf(const char *formatString, ...);
#define TRACENR_ERROR(...) DbgConsole_Printf(__VA_ARGS__)
#define TRACE_ERROR(...) DbgConsole_Printf(__VA_ARGS__);DbgConsole_Printf("\r\n")
#define TRACE_ERROR_MEM(...) DbgConsole_Printf(__VA_ARGS__);DbgConsole_Printf("\r\n")
#else
extern int DbgConsole_Printf(const char *formatString, ...);
#define TRACE_ERROR(...)       //DbgConsole_Printf(__VA_ARGS__)
#define TRACE_ERROR_MEM(...)   //DbgConsole_Printf(__VA_ARGS__)
#define TRACENR_ERROR(...)
#endif
#endif
#define TRACE_START_SIZE(...)
#define TRACE_RELEASE(...)
#define TRACE_MALLOC(...)
#define TRACE_SIZE_ADJ(...)
#endif
#endif
#ifdef DRAW_POLYGON_TEST
#define START_STOP_EDGEPTN_TRACE(a) //send_edgeptn_data_to_usb(0, 0, 0, a, 0)
#define TRACE_EDGEPTN(a, b, c, d, e) //send_edgeptn_data_to_usb(a, b, c, d, e)
#define TRACE_EDGEPTN2(a, b, c, d, e) //send_edgeptn_data_to_usb(a, b, c, d, e)

#define START_STOP_POLY_TRACE(a) send_polygon_data_to_usb(0, (a), 0)
#define SEND_POLY(a, b, c) send_polygon_data_to_usb(a, b, c)
#define TRACE_POLYGON(a, b, c) //send_polygon_data_to_usb(a, b, c)
#define TRACE_POLYGON2(fmt, args...)
#define TRACE_POLYGON3(fmt, args...)
#define TRACE_POLYGON4(fmt, args...) //send_polygon_data_to_usb(a, b, c)
#define TRACE_POLYGON5(fmt, args...) //send_polygon_data_to_usb(a, b, c)
#define TRACE_POLYGON6(fmt, args...) //send_polygon_data_to_usb(a, b, c)
#define TRACE_POLYGON_TMP(fmt, args...) //send_polygon_data_to_usb(a, b, c)
#else
#define START_STOP_EDGEPTN_TRACE(fmt, args...)
#define TRACE_EDGEPTN(fmt, args...)
#define TRACE_EDGEPTN2(fmt, args...)

#define START_STOP_POLY_TRACE(fmt, args...)
#define SEND_POLY(fmt, args...)
#define TRACE_POLYGON(fmt, args...)
#define TRACE_POLYGON_MULTI(fmt, args...)
#define TRACE_POLYGON_POLYCLIP(fmt, args...)
#define TRACE_POLYGON_TMP(fmt, args...)
#endif
#define SEND_TOUCH(a, b, c) //send_touch_data_to_serial(a, b, c)

#ifdef DRAW_POLYGON_TEST //nsmoon@170205
extern int send_edgeptn_data_to_usb(uint16_t pd, uint16_t led, int8_t slope, uint8_t flag, unsigned long color);
extern int send_polygon_data_to_serial(poly_e poly, unsigned char flag);
int send_polygon_data_to_usb(poly_e poly, uint8_t flag, unsigned long color);
extern int send_touch_data(vec touch, unsigned char flag, int len);
extern void	Print_char_Debug(unsigned char Send_Data);
extern void	Print_Hex_Data_Debug(unsigned char Send_Data);
#endif

#endif // defined(_WIN32) || defined(WIN32)

#ifdef DRAW_POLYGON_TEST
#define MAX_DEBUG_VERT	(MAX_SLOPE_COUNT * 2)
extern vec_e_t  BS_debugVert[MAX_DEBUG_VERT];
#endif

#ifdef DRAW_POLYGON_TEST
extern void DEBUG_show_pos_(vec pos, float wx, float wy, int color);
extern void DEBUG_show_line_(vec line, int color);
extern void DEBUG_show_line_pd_led_(axis_t axis, int pd, int led, int color);
extern void DEBUG_show_line_idx_(axis_t axis, int idx, int color);
extern void DEBUG_show_line_pos_(vec p0, vec p1, int color);
extern void DEBUG_show_line_belonged_(axis_t axis, uint8_t *inst, uint8_t len, int color);
extern void DEBUG_show_line_axis_pos_(axis_t axis, float ledPos, float pdPos, int color);
extern void DEBUG_show_min_max_(pos_min_max_t *pos, int color, int mode);
extern void DEBUG_show_cent_grid_(vec_t *cent, int gridNum, int color);
extern void DEBUG_show_min_max_pos_(axis_t axis, vec_t *inst, int minIdx, int maxIdx, int color);
extern void DEBUG_show_BS_polygon_(int color);
extern void DEBUG_show_BS_touch_polygon_(int maxCnt, int color);
extern void DEBUG_show_BS_touch_polygon_one_(int subjIdx, int color);
extern void DEBUG_show_poly_(vec_e_t *poly, int len, int color);
extern void DEBUG_show_start_end_(bd_type_t bd, int start, int end, int color);
extern void DEBUG_show_BS_initial_polygon_(void);
extern void DEBUG_show_BS_subject_polygon_(int color);
extern void DEBUG_show_touch_point_(void);
#endif

#if 1 //nsmoon@220119
typedef struct {
    char seqStr[DEBUG_TIME_DIFF_STR_LEN];
    int timeDiff;
} debug_time_diff_t;
#endif

#ifdef DEBUG_GET_TIME_DIFF_ENABLE
extern void DEBUG_get_time_diff(char *seqStr);
extern void DEBUG_get_time_diff_init(void);
extern void DEBUG_slow_time_diff_show(void);

#if defined(_WIN32) || defined(WIN32)
extern void _myTraceNR(char const *format, ...);
#define TRACE_TIME_DIFF(...) _myTraceNR(__VA_ARGS__)
#elif (MODEL_MCU == PIC_32)
extern int printf(const char *format, ...);
#define TRACE_TIME_DIFF(...) printf(__VA_ARGS__)
#elif (MODEL_MCU == RT1052) || (MODEL_MCU == RT1064)
extern int DbgConsole_Printf(const char *formatString, ...);
#define TRACE_TIME_DIFF(...) DbgConsole_Printf(__VA_ARGS__)
#else
#error "MODEL_MCU is not defined for TRACE_TIME_DIFF..."
#endif
#else
#define TRACE_TIME_DIFF(...)
#endif

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _BACKEND_DEBUG_H */
/*end of file*/
