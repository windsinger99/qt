/*
 * backend.h
 *
 *  Created on: Jan 11, 2019
 *      Author: Namsu Mun
 */
#ifndef BACKEND_H_
#define BACKEND_H_

#define MODEL_SPT_750			750
#define MODEL_CTKS_750_V120		7501 //samsung V120, nxp+pic
#define MODEL_CTKS_750_V130		7502 //samsung V120, nxp+pic
#define MODEL_CTKS_750_V140		7503 //samsung V120, nxp+pic, frontend_reset
#define MODEL_CTSK_850_V100     8501 //samsung, nxp+pic
#define MODEL_CTSK_650_V100     6501 //samsung, nxp+pic
#define MODEL_SPT				MODEL_CTSK_650_V100 //MODEL_CTKS_750_V120

#if (MODEL_SPT == MODEL_SPT_750)
//#define CTKS_750_V110
#define FRONTEND_CTKS_750_V111
#define FRONTEND_LINE_THRESHOLD
#elif (MODEL_SPT == MODEL_CTKS_750_V120)
#define FRONTEND_CTKS_750_V120 //frontend version
#define FRONTEND_LINE_THRESHOLD
#elif (MODEL_SPT == MODEL_CTKS_750_V130)
#define FRONTEND_CTKS_750_V130 //frontend version
#define FRONTEND_LINE_THRESHOLD
#elif (MODEL_SPT == MODEL_CTKS_750_V140)
#define FRONTEND_CTKS_750_V140 //frontend version
#define FRONTEND_LINE_THRESHOLD
#define FRONTEND_RESET_ENABLE //for new boot loader
#elif (MODEL_SPT == MODEL_CTSK_850_V100)
#define FRONTEND_CTSK_850_V100 //frontend version
#define FRONTEND_LINE_THRESHOLD
//#define CTSO_0850_PV		// 850 pv only
#ifndef CTSO_0850_PV
#define FRONTEND_RESET_ENABLE
#endif
#elif (MODEL_SPT == MODEL_CTSK_650_V100)
#define FRONTEND_CTKS_650_V100 //frontend version
#define FRONTEND_LINE_THRESHOLD
#define FRONTEND_RESET_ENABLE //for new boot loader
#endif

#define FRONTEND_NO_ADC
#define FRONTEND_AAREA
#define DEAD_CELL_TBL	1
#define SECOND_OFST_TBL	1
#define USE_FW_KEY_VALIDATION	(0U) //nsmoon@200720 not-use for ss
#define BRUSH_MODE_ENABLE		(1U)
#define FLIP2_COMPATIBLE_DESCRIPTOR	(1U)
#define HOR_EDGE_TOUCH_ENABLE  //nsmoon@200415
//#define FRONTEND_RESET_ENABLE  //nsmoon@200609
//#define BM_SCROLL			  // YJ_20200610
#define TOUCH_SCROLL_INFO_ENABLE
#define NORMAL_RESET_DELAY	//nsmoon@200810
#define FRONTEND_NO_CRC //nsmoon@210311
#define FRONTEND_FORCED_AGC	//YJ@210624


#if defined(DEBUG)
#define DEBUG_FUNCTION_ENABLE_ALL
#endif

#if (MODEL_SPT == MODEL_CTSK_850_V100)
#ifdef FRONTEND_LINE_THRESHOLD
#define LINE_THRESHOLD_SIZE		(1)
#define LINE_THRESHOLD_VALUE	(85)	//(95)	//(90) 75_R0004		//90
#endif
#else
#ifdef FRONTEND_LINE_THRESHOLD
#define LINE_THRESHOLD_SIZE		(1)
#define LINE_THRESHOLD_VALUE	(50) //(95)	//(90) R0004		//90
#endif
#endif

//For Debugging
#if defined(_WIN32) || defined(WIN32)
#define DEBUG_TOOL_QT       1
#if defined(DEBUG) || defined(_DEBUG) || defined(QT_DEBUG)
#define DEBUG_FUNCTION_ENABLE_ALL
#endif
#if (_DEBUG == 1) || defined(DEBUG) || (QT_NO_DEBUG == 0)
#ifndef DEBUG_FRAME_NO
#define DEBUG_FRAME_NO      0
#endif
#ifndef DEBUG_FRAME_PAUSE
#define DEBUG_FRAME_PAUSE   0 //1 //1:enable
#endif
#else
#define DEBUG_FRAME_NO      0
#define DEBUG_FRAME_PAUSE   0 //1:enable
#endif
#endif

#if (_DEBUG == 1) || defined(DEBUG) || !defined(QT_NO_DEBUG)
//#error FIXME
#define ENABLE_DEBUG_MESSAGE	1
#endif
#if (DEBUG_TOOL_QT == 1)
#define TEMP_FILE_WRITE
#define DEBUG_MESSAGE_ENABLE
#define SHOW_DEBUG_SIZE_DOWN_0205
#endif

#if (ENABLE_DEBUG_MESSAGE == 1)
#define TRACE(...) _myTrace(__VA_ARGS__) //_myTraceWrite
#define TRACENR(...) _myTraceNR(__VA_ARGS__) //_myTraceWriteNR
#define TRACE_ERROR(...) _myTrace(__VA_ARGS__) //_myTraceWrite
#define TRACENR_ERROR(...) _myTraceNR(__VA_ARGS__) //_myTraceWrite
#define TRACE_ERROR_MEM(...) _myTrace(__VA_ARGS__)
#define TRACE_NOP (BS_debug_nop++)
#define TRACE_START_SIZE(...) _myTrace(__VA_ARGS__) //_myTraceWrite
#define TRACE_RELEASE(...) _myTrace(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACENR(...)
#define TRACE_ERROR(...)
#define TRACENR_ERROR(...)
#define TRACE_ERROR_MEM(...)
#define TRACE_NOP
#define TRACE_START_SIZE(...) //_myTrace(__VA_ARGS__) //_myTraceWrite
#define TRACE_RELEASE(...) _myTrace(__VA_ARGS__)
#endif

#define PRG_VERSION			"nxp_backend@190125A"
#define FRONTEND_VID_HEX	0x04D8
#define FRONTEND_PID_HEX	0x0053 //0x0320
#define FRONTEND_BT_PID_HEX	0x003C //bootloader pid

#define MAX_SENSOR_POS_SIZE			500
#define USB_HOST_PACKET_SIZE		(512U)
#define USB_HOSTPACKET_SIZE_FEBT	(64U)

#define PD_DATA_HEADER			0xA5
#define PD_OFST_HEADER			0xA0
#define PD_DATA_HEADER			0xA5
#define PD_DATA_IDX				8  //PD-data start, 0~7  header
#define PD_IDX_NUM_OF_PD_X		4
#define PD_IDX_NUM_OF_PD_Y		6
#define PD_IDX_SCAN_MODE		8
#define PD_IDX_OFST_LEN_X		9
#define PD_IDX_OFST_LEN_Y		10
#define PD_IDX_VAR_HD			11 //idx of variable header

#if (DEAD_CELL_TBL == 1) //nsmoon@190926
#define PD_IDX_DCELL_LEN_X_LED  11
#define PD_IDX_DCELL_LEN_X_PD   12
#define PD_IDX_DCELL_LEN_Y_LED  13
#define PD_IDX_DCELL_LEN_Y_PD   14
#endif

#if 1 //nsmoon@200706
#define PD_IDX_VER_LEN			17 //same as old PD_IDX_SEN_LEN_X2
#endif

#if (SECOND_OFST_TBL == 1) //nsmoon@190624
#define PD_IDX_OFST_LEN_X2      15
#define PD_IDX_OFST_LEN_Y2      16
#define PD_IDX_SEN_LEN_X2       17
#define PD_IDX_SEN_LEN_Y2       18
#define PD_IDX_POS_START        19 //position data
#else
#define PD_IDX_POS_START        15 //position data
#endif
#define PD_IDX_LED_POS_X		(PD_IDX_POS_START)
#define PD_IDX_PD_POS_X			(PD_IDX_LED_POS_X + g_back_conf.numPdX)
#define PD_IDX_LED_POS_Y		(PD_IDX_PD_POS_X + g_back_conf.numPdX)
#define PD_IDX_PD_POS_Y			(PD_IDX_LED_POS_Y + g_back_conf.numPdY)

#define PD_DATA_IDX2			4 //PD-data start

#define USB_OFST_CMD			0x80
#define USB_DATA_CMD			0x81
#define USB_DATA_ADC_CMD		0x82
#define USB_DATA_SEN_LIFE_TEST  0x83
#define USB_DATA_THRESHOLD 		0x84 //nsmoon@191212
#define USB_RESET_CMD			0x0A //nsmoon@191022
#define USB_REQ_SCAN_RUN		0xA1 //DL-TEST@0604
#define USB_REQ_SCAN_STOP		0xA2 //DL-TEST@0604
#define USD_MAX_RECV_IDL_CNT	10

#if !defined(_WIN32) && !defined(WIN32)
#if (MODEL_SPT == MODEL_CTKS_750_V120) //nsmoon@191212
#define MAX_NUM_PD_X_DLT		159
#define MAX_NUM_PD_Y_DLT		97
#elif (MODEL_SPT == MODEL_CTKS_750_V130) //nsmoon@200118
#define MAX_NUM_PD_X_DLT		161
#define MAX_NUM_PD_Y_DLT		107
#define MAX_X_SLOPE_VAL			40 //81, 8-bits
#define MAX_Y_SLOPE_VAL			63 //127, 8-bits
#elif (MODEL_SPT == MODEL_CTKS_750_V140) //nsmoon@200118
#define MAX_NUM_PD_X_DLT		150
#define MAX_NUM_PD_Y_DLT		105
#define MAX_X_SLOPE_VAL			42 //85, 8-bits
#define MAX_Y_SLOPE_VAL			63 //127, 8-bits
#elif (MODEL_SPT == MODEL_CTSK_850_V100) //nsmoon@200118
#define MAX_NUM_PD_X_DLT		167
#define MAX_NUM_PD_Y_DLT		117
#define MAX_X_SLOPE_VAL			42 //256, 8-bits
#define MAX_Y_SLOPE_VAL			63 //256, 8-bits
#elif  (MODEL_SPT ==MODEL_CTSK_650_V100)
#define MAX_NUM_PD_X_DLT		126
#define MAX_NUM_PD_Y_DLT		85
#define MAX_X_SLOPE_VAL			40 //35//40 //256, 8-bits
#define MAX_Y_SLOPE_VAL			63 //256, 8-bits
#else
#define MAX_NUM_PD_X_DLT		158 //142 //500
#define MAX_NUM_PD_Y_DLT		96 //84  //400
#endif
#define MAX_NUM_SLOPES_DLT_X	(MAX_X_SLOPE_VAL * 2 + 1)
#define MAX_NUM_SLOPES_DLT_Y	(MAX_Y_SLOPE_VAL * 2 + 1)
#define MAX_VER_TOUCH_PD		(MAX_NUM_PD_Y_DLT * MAX_NUM_SLOPES_DLT_Y)
#define MAX_HOR_TOUCH_PD		(MAX_NUM_PD_X_DLT * MAX_NUM_SLOPES_DLT_X)

#if (DEAD_CELL_TBL == 1) //nsmoon@190829
#define MAX_DEADCELL_SIZE_X		(MAX_NUM_PD_X_DLT)
#define MAX_DEADCELL_SIZE_Y		(MAX_NUM_PD_Y_DLT)
#endif
#if (SECOND_OFST_TBL == 1) //nsmoon@190624
#define MAX_NUM_2ND_SENSOR_X	20
#define MAX_NUM_2ND_SENSOR_Y	20
#endif

#define USB_CRC_SIZE			2 //bytes

#define MAX_SHIFT_TBL_BYTE		8
#define MAX_SHIFT_TBL			64

#if 1 //nsmoon@191119
#define NUM_ADC_BUFF			3
#define USB_CMD_LEN				10
#define USB_PARAM_LEN			(USB_CMD_LEN - 2)
#endif

#define MAX_SLOPE_BYTE_X        ((MAX_NUM_SLOPES_DLT_X + 7) / 8)
#define MAX_SLOPE_BYTE_Y        ((MAX_NUM_SLOPES_DLT_Y + 7) / 8)
#define MAX_LINE_DATA_X			(MAX_NUM_PD_X_DLT * MAX_SLOPE_BYTE_X)
#define MAX_LINE_DATA_Y			(MAX_NUM_PD_Y_DLT * MAX_SLOPE_BYTE_Y)
//#define MAX_EXB_SEND_BUF_SIZE   (PD_DATA_IDX2 + (MAX_NUM_PD_X_DLT * MAX_SLOPE_BYTE_X) + (MAX_NUM_PD_Y_DLT * MAX_SLOPE_BYTE_Y) + (MAX_DEADCELL_SIZE_X * 2) + (MAX_NUM_2ND_SENSOR_X * 2))
#ifdef FRONTEND_LINE_THRESHOLD
#define MAX_HOR_LINE_THRESHOLD	(MAX_LINE_DATA_X * LINE_THRESHOLD_SIZE)
#define MAX_VER_LINE_THRESHOLD	(MAX_LINE_DATA_Y * LINE_THRESHOLD_SIZE)
#define MAX_EXB_SEND_BUF_SIZE   (PD_DATA_IDX2 +  MAX_LINE_DATA_X + MAX_LINE_DATA_Y + MAX_HOR_LINE_THRESHOLD + MAX_VER_LINE_THRESHOLD + USB_CRC_SIZE)
#else
#define MAX_EXB_SEND_BUF_SIZE   (PD_DATA_IDX2 + (MAX_NUM_PD_X_DLT * MAX_SLOPE_BYTE_X) + (MAX_NUM_PD_Y_DLT * MAX_SLOPE_BYTE_Y) + ((MAX_NUM_PD_X_DLT + MAX_NUM_PD_Y_DLT) * NUM_ADC_BUFF) + USB_CRC_SIZE)
#endif

#define MAX_USB_IN_PACKET_CNT	(((MAX_EXB_SEND_BUF_SIZE + (USB_HOST_PACKET_SIZE - 1)) / USB_HOST_PACKET_SIZE))
#define MAX_USB_IN_BUF_SIZE		(USB_HOST_PACKET_SIZE * MAX_USB_IN_PACKET_CNT)
#define MAX_USB_OUT_BUF_SIZE	(USB_HOST_PACKET_SIZE)
#define MAX_OFST_DATA_SIZE		(PD_IDX_OFST_X + (MAX_NUM_SLOPES_DLT_X + MAX_NUM_SLOPES_DLT_Y) + (4 * MAX_NUM_PD_X_DLT * 2) + (4 * MAX_NUM_PD_Y_DLT * 2) + (4 * 8))
#endif

#ifdef FRONTEND_CTKS_650_V100 //FIXME
#define TEST_LOGICAL_X_MAX		23333 //23217	//2321.74 =1451.09*1.6
#define TEST_LOGICAL_Y_MAX		13330 //13217	//1321.776 = 826.11*1.6
#define TEST_PHYSICAL_X_MAX		14583 //14511	//1451.09
#define TEST_PHYSICAL_Y_MAX		8331  //8261	//826.11
#endif
#ifdef FRONTEND_CTKS_750_V110 //FIXME
#define TEST_LOGICAL_X_MAX		27410 //27248=17030*1.6
#define TEST_LOGICAL_Y_MAX		15930 //15800=9875*1.6
#define TEST_PHYSICAL_X_MAX		17132 //SPT750-V1 17030
#define TEST_PHYSICAL_Y_MAX		9959  //SPT750-V1 9875
#define TEST_AA_WIDTH			1649  //SMT-LG PANNEL
#define TEST_AA_HEIGHT			926
#endif
#ifdef FRONTEND_CTKS_750_V111 //FIXME
#define TEST_LOGICAL_X_MAX		27454 //27248=17159*1.6
#define TEST_LOGICAL_Y_MAX		15931 //15931=9957*1.6
//#define TEST_LOGICAL_X_MAX		30886 //30886=17159*1.8
//#define TEST_LOGICAL_Y_MAX		17922 //17922=9957*1.8
//#define TEST_LOGICAL_X_MAX		24022 //30886=17159*1.4
//#define TEST_LOGICAL_Y_MAX		13939 //17922=9957*1.4
#define TEST_PHYSICAL_X_MAX		17159 //SPT750-V1 17030
#define TEST_PHYSICAL_Y_MAX		9957  //SPT750-V1 9875
#define TEST_AA_WIDTH			1649  //SMT-LG PANNEL
#define TEST_AA_HEIGHT			926
#endif
#ifdef FRONTEND_CTKS_750_V120 //FIXME
#define TEST_LOGICAL_X_MAX		27121 //27121=16951*1.6
#define TEST_LOGICAL_Y_MAX		15566 //15566=9729*1.6
#define TEST_PHYSICAL_X_MAX		16951 //SPT750-V1 17030
#define TEST_PHYSICAL_Y_MAX		9729  //SPT750-V1 9875
//#define TEST_AA_WIDTH			1649  //SMT-LG PANNEL
//#define TEST_AA_HEIGHT		926
#endif
#ifdef FRONTEND_CTKS_750_V130 //FIXME
#define TEST_LOGICAL_X_MAX		27296 //27296=17060*1.6
#define TEST_LOGICAL_Y_MAX		15742//15742=9839*1.6
#define TEST_PHYSICAL_X_MAX		17060 //SPT750-V1 17030
#define TEST_PHYSICAL_Y_MAX		9839  //SPT750-V1 9875
#endif
#ifdef FRONTEND_CTKS_750_V140 //FIXME
#define TEST_LOGICAL_X_MAX		26403	//26403.84 =1650.24*1.6
#define TEST_LOGICAL_Y_MAX		14852	//14852.16 = 928.26*1.6
#define TEST_PHYSICAL_X_MAX		16502	//16502	//1650.24 = 1679.72 -29.48
#define TEST_PHYSICAL_Y_MAX		9282	//9282	//928.26 = 957.73-29.47
#endif

#ifdef FRONTEND_CTSK_850_V100 //FIXME
#if 1
#define TEST_LOGICAL_X_MAX		30896 //30896=19310*1.6
#define TEST_LOGICAL_Y_MAX		17792 //17792=11120*1.6
//#define TEST_LOGICAL_X_MAX		34560 //34560=1920*18
//#define TEST_LOGICAL_Y_MAX		19440 //19440=1080*18
#define TEST_PHYSICAL_X_MAX		19310
#define TEST_PHYSICAL_Y_MAX		11120
#else
#define TEST_LOGICAL_X_MAX		48000
#define TEST_LOGICAL_Y_MAX		30000
#define TEST_PHYSICAL_X_MAX		16000
#define TEST_PHYSICAL_Y_MAX		10000
#endif
#endif

#define ALLOWABLE_TOUCH_BE		(20)	//(15)
#define ALLOWABLE_TOUCH_IO		(ALLOWABLE_TOUCH_BE*2) //10-multi,4-fine
#define	BYTES_PER_POS			10 // 10 bytes per position
#define TP_PER_PACKET			5  // 5 TP per packet

#ifndef LOBYTE_16
#define LOBYTE_16(x)  ((uint8_t)((x) & 0x00FF))
#endif
#ifndef HIBYTE_16
#define HIBYTE_16(x)  ((uint8_t)(((x) & 0xFF00) >> 8))
#endif
#ifndef GET_MAX
#define GET_MAX(a, b)			(((a) > (b)) ? (a) : (b))
#endif
#ifndef GET_MIN
#define GET_MIN(a, b)			(((a) < (b)) ? (a) : (b))
#endif
#ifndef GET_ABS
#define GET_ABS(a)				(((a) < 0) ? -(a) : (a))
#endif

#define MIN_INITIAL_VAL         9999
#define MIN_INITIAL_VAL_SQ      90000000 //3000*3000
#define EPSILON					0.0001f //do not change
//#define IS_DEBUG_FLAG
//#define TRACE_NOP

#if !defined(_WIN32) && !defined(WIN32) //gpio, nsmoon@190116
#include "fsl_gpio.h"
#include "board.h"
/* for debugging */
#define DEBUG_LED_GPIO		BOARD_USER_LED_GPIO
#define DEBUG_LED_GPIO_PIN	BOARD_USER_LED_GPIO_PIN
#define DEBUG_LED_TOGGLE	GPIO_PortToggle(DEBUG_LED_GPIO, 1u << DEBUG_LED_GPIO_PIN)
#define DEBUG_LED_ON		GPIO_PortSet(DEBUG_LED_GPIO, 1u << DEBUG_LED_GPIO_PIN)
#define DEBUG_LED_OFF		GPIO_PortClear(DEBUG_LED_GPIO, 1u << DEBUG_LED_GPIO_PIN)
#define DEBUG_DELAY_COUNT	8000000
#define DEBUG_DELAY_1M \
	DEBUG_LED_TOGGLE; \
    volatile uint32_t debugCnt = 0; \
	for (debugCnt = 0; debugCnt < 1000000; debugCnt++) { \
	    __asm("NOP"); /* delay */ \
	} /*sys clock check, -O0, 8-instuctions */
#endif //gpio

#if defined(_WIN32) || defined(WIN32)
#define ATTR_DTCM_RAM
#define ATTR_ITCM_RAM
#define ATTR_BACKEND_RAM
#define ATTR_BACKEND_RAM2
#define ATTR_BACKEND_RAM3
#define ATTR_BACKEND_CONST
#define ATTR_BACKEND_RAMFUNC
#else
#define ATTR_BACKEND_RAM2	__attribute__((section(".bss.$RAM2")))  //SRAM_ITC
#define ATTR_BACKEND_RAM3	__attribute__((section(".bss.$RAM3")))  //SRAM_OC
#define ATTR_BACKEND_CONST	__attribute__((aligned(4))) const
#define ATTR_BACKEND_RAMFUNC
#endif

#if defined(_WIN32) || defined(WIN32)
#define TRACE_RELEASE(...) _myTrace(__VA_ARGS__)
#else
#define TRACE_RELEASE(...)
#endif

#if defined(_WIN32) || defined(WIN32)  //FIXME nsmoon@170111
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
#endif //defined(_WIN32) || defined(WIN32)

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

typedef enum
{
    INIT_SCAN = 0,
    NORMAL_SCAN = 1,
} SCAN_STATES;

typedef struct {
    SCAN_STATES currScan;
    SCAN_STATES nextScan;
    uint16_t numTouch;
    uint16_t x1, y1;
    uint16_t x2, y2;
#if (BRUSH_MODE_ENABLE == 1) //brush mode, FIXME
    uint8_t brushMode;
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

#if 1 //refer to backend_util.h
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

typedef struct {
    uint16_t numPdX, numPdY;   //num of pd
    uint8_t  numOfsX, numOfsY; //num of offset
    uint8_t  maxOfsX, maxOfsY; //max offset //nsmoon@210603
    uint8_t slope_byte_x, slope_byte_y; //num of slope byte
} backend_config_t;

typedef struct {
    uint64_t upper;
    uint64_t lower;
} slope_mask_t;

typedef enum
{
    ENUM_HOR_X = 0,
    ENUM_VER_Y = 1,
} axis_t;

typedef enum
{
    backendDataOffset = 0x00,
    backendDataLine = 0x01,
    backendDataLineAdc = 0x02,
    backendDataLineAdcReady = 0x03,
    backendDataIdle,
    backendDataError = 0xFF
} backend_data_t;

////////////////////////////////////
//extern backend functions
////////////////////////////////////
#if (SECOND_OFST_TBL == 1) //nsmoon@190624
#if (DEAD_CELL_TBL == 1) //nsmoon@190829
int BG_init_backend(
    DEF_TP_LAYOUT_INFO *tpLayout, 	//sensor laypout
    DEF_DEADCELL_INFO  *InvalidTbl,			//not used yet
    uint16_t  init_flag          // reserved: 0
);
#else
int BG_init_backend(
    DEF_TP_LAYOUT_INFO *tpLayout, 	//sensor laypout
    DEF_PD_INFO  *ValidTbl,			//not used yet
    uint16_t  init_flag          // reserved: 0
);
#endif

extern BACKEND_STATUS BG_call_backend2(
    DEF_PD_INFO *InBuf,       // (hor_pd_num + ver_pd_num)
    DEF_DATA_INFO2	*OutBuf,
    next_scan_t *nextScan     // next scan infomation
);
#endif //1

#if defined(_WIN32) || defined(WIN32)
//extern int BS_offsetTblLenX, BS_offsetTblLenY;
//extern int BS_slopeValMaxX, BS_slopeValMaxY;
extern unsigned int BG_frame_no;
#endif

//////////////////////////////////////
//for debugging
//////////////////////////////////////
#if defined(_WIN32) || defined(WIN32)
extern int BG_show_ep;
//extern int BG_show_line;
extern int BG_show_line_x;
extern int BG_show_line_y;
extern int BG_show_line_init;
extern int BG_show_line_all;
extern int BG_show_line_th10;
extern int BG_show_line_rem;
//extern int BG_show_debug;
//extern int BG_show_debug_max;
extern int BG_debug_pause;

//#define MAX_SLOPE_COUNT		MAX_NUM_SLOPES_DLT_Y //nsmoon@201012
#define MAX_PS_SIZE			(4096*50)

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

#define START_STOP_POLY_TRACE(a) _myDrawPolygon(0, a, 0)
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
#endif

#define MAX_SLOPE_COUNT    ((0xFF / 2) - 1) //nsmoon@201022
typedef struct {
    int len;
    int flag;
    unsigned long color;
    vec_t vert[MAX_SLOPE_COUNT];
} polygon_tmp_t;
extern polygon_tmp_t polygon_saved[MAX_PS_SIZE];
extern int polgon_saved_idx;
extern int bStartAddPol;

#define MAX_BG_DEBUG_MISC   (ALLOWABLE_TOUCH_BE * 2) //20
extern int BG_debug_misc[MAX_BG_DEBUG_MISC];

extern int is_myDrawPolygon(void);
#endif
#endif
//////////////////////////////////////

#endif /* BACKEND_H_ */
