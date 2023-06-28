/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Lispect

  @File Name
    backend_common.h

  @Summary
    common definitions for backend configuation

  @Description
    Describe the purpose of this file.
*/
/* ************************************************************************** */
#ifndef _BACKEND_COMMON_H    /* Guard against multiple inclusion */
#define _BACKEND_COMMON_H
/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////
// Models
///////////////////////////////////////////////////////////
#define MODEL_KIO_430			      430  //samsung kiosk, NXP
#define MODEL_CTSO_430_V100           4301 //samsung nxp+pic

#define MODEL_SPT_550		550  //lispect V1, STM
#define MODEL_IRT_550		5501 //chemtronics V2, NXP
#define MODEL_KIO_550			5502 //samsung kiosk, NXP

#define MODEL_CTSI_650_V100  	      6510 //samsung, nxp+pic

#define MODEL_SPT_750		        750  //lispect V1, NXP+PIC
#define MODEL_CTKS_750_V120		7501 //samsung V120, nxp+pic
#define MODEL_CTKS_750_V130		7502 //samsung V130, nxp+pic
#define MODEL_CTKS_750_V140		7503 //samsung V140, nxp+pic, frontend_reset
#define MODEL_CTKC_750			7510 //KTC, NXP

#define MODEL_CTSK_850_V100     8501 //samsung, nxp+pic

#define MODEL_CTSI_550_SINGLE_V100  	55010 //samsung, pic single chipsork

#define MODEL_CTSI_N750_V100    75000 //samsung-wmc, nxp-one-chip
#define MODEL_CTSI_N650_V100    65000 //samsung-wmc, nxp-one-chip

#define MODEL_SPT			MODEL_CTSI_N650_V100 //MODEL_CTSI_N750_V100

//#define GN65_TEST //nsmoon@221116 for GN65
//#define N75_TEST //nsmoon@230328 for N75
#define N65_TEST //nsmoon@230411 for N65

///////////////////////////////////////////////////////////
// Touch Size
///////////////////////////////////////////////////////////
#define MODEL_TOUCH_SIZE_43		43
#define MODEL_TOUCH_SIZE_55		55
#define MODEL_TOUCH_SIZE_65		65    
#define MODEL_TOUCH_SIZE_75		75
#define MODEL_TOUCH_SIZE_85		85

///////////////////////////////////////////////////////////
// MCUs
///////////////////////////////////////////////////////////
#define X86				0
#define PIC_32			10
#define STM32H7			20
#define RT1052			30
#define RT1064			31
#define AM335X          40

///////////////////////////////////////////////////////////
//common parameter for model
///////////////////////////////////////////////////////////
#define DRAW_LINE_TEST	0  // DLT
#define SECOND_OFST_TBL		0 //nsmoon@190624
#define DEAD_CELL_TBL       1 //nsmoon@190929
//#define SMOOTH_SIZE_ENABLE //nsmoon@190729
//#define INITIAL_LINE_TEST_MULTI

#define USE_CUST_MALLOC //nsmoon@201012
#define MAX_BS_HEAP_SIZE    0x30000

//#define VIRTUAL_EP_MERGE_ENABLE
//#define MULTI_PACK_EP3_ENABLE
#define MULTI_PACK_EP4_ENABLE
//#define FINE_INITIAL_LINE_CENT //nsmoon@230418 //nsmoon@230613 no-used

//#define X_SLOPE_55  //nsmoon@200318
#ifndef DEBUG_UNBLOCKED_LINE
#define FINE_OLD_METHOD_2PEN
#endif
//#define USE_SENSOR_LIST //nsmoon@200327, for test
#define MULTI_SEN_CNT_EVEN_SKIP_ENABLE //nsmoon@200330
#define MULTI_VIRTUAL_INITIALL_MERGE_ENABLE //nsmoon@200407
#define DUPLICATE_TH10 //nsmoon@200511
#define MULTI_SKIP_USED_VB    //nsmoon@210218
//#define FINE_INITIAL_LINE_NEW   //nsmoon@211119 //nsmoon@220124 not-used
//#define TRACE_ERROR_ENABLE //nsmoon@211126
//#define ADJUST_MIN_MAX_FINE_NEW //nsmoon@220308a //for jitter test
#define FINE_REMOVE_GHOST_NEW //nsmoon@230613

#if !defined(MODEL_SPT)
#error "Not defined MODEL_SPT !!!"
#endif
#if (MODEL_SPT == MODEL_CTKC_750)
#define USE_VALID_OFST_TBL	1 //nsmoon@191129
#endif

#if (MODEL_SPT == MODEL_KIO_550) || (MODEL_SPT == MODEL_KIO_430) //nsmoon@200402
#define BRUSH_MODE_ENABLE   0 //nsmoon@200227
#else
#define HOR_EDGE_TOUCH_ENABLE  //nsmoon@200415
#define BRUSH_MODE_ENABLE   1 //nsmoon@200227
#endif

#if !defined(MODEL_SPT)
#error "Not defined MODEL_SPT !!!"
#endif

//#if defined(DEBUG_FUNCTION_ENABLE_ALL) || defined(DEBUG_FUNCTION_ENABLE_RELEASE)
//#define FINE_RULE_1A_TEST
//#endif
///////////////////////////////////////////////////////////

#if (MODEL_SPT == MODEL_CTKS_750_V130) || (MODEL_SPT == MODEL_CTKS_750_V140) || (MODEL_SPT == MODEL_CTSI_650_V100)  || \
    (MODEL_SPT == MODEL_CTSI_N750_V100) || (MODEL_SPT == MODEL_CTSI_N650_V100) //nsmoon@230411
#ifndef USE_CUST_MALLOC //nsmoon@201012
#define MODEL_SPT_X_CELL 	161 //nsmoon@201013
#define MODEL_SPT_Y_CELL 	107 //nsmoon@201013
#ifdef X_SLOPE_55
#define MODEL_SPT_X_SLOPE	55
#else
#define MODEL_SPT_X_SLOPE	40 //nsmoon@201013
#endif
#define MODEL_SPT_Y_SLOPE	63 //40 //nsmoon@201013
#endif
#define MODEL_TOUCH_SIZE	MODEL_TOUCH_SIZE_75
#define MAX_TOUCH_LIMIT_FINE        4 //nsmoon@210119=>8, for test
#define MAX_TOUCH_LIMIT_MULTI       20 //10
#elif (MODEL_SPT == MODEL_CTSI_550_SINGLE_V100) //nsmoon@211116
#define MODEL_TOUCH_SIZE	MODEL_TOUCH_SIZE_55
#define MAX_TOUCH_LIMIT_FINE        4 //nsmoon@210119=>8, for test
#define MAX_TOUCH_LIMIT_MULTI       20 //10    
#elif (MODEL_SPT == MODEL_CTSK_850_V100) //nsmoon@200422
#ifndef USE_CUST_MALLOC //nsmoon@201012
#define MODEL_SPT_X_CELL 	167
#define MODEL_SPT_Y_CELL 	117
#define MODEL_SPT_X_SLOPE	42
#define MODEL_SPT_Y_SLOPE	63
#endif
#define MODEL_TOUCH_SIZE	MODEL_TOUCH_SIZE_85
#define MAX_TOUCH_LIMIT_FINE        4 //nsmoon@210119=>8, for test
#define MAX_TOUCH_LIMIT_MULTI       20 //10
#elif (MODEL_SPT == MODEL_CTSO_430_V100) //nsmoon@210222
#define MODEL_TOUCH_SIZE	MODEL_TOUCH_SIZE_43
#define MAX_TOUCH_LIMIT_FINE        4
#define MAX_TOUCH_LIMIT_MULTI       10
#elif (MODEL_SPT == MODEL_KIO_550) || (MODEL_SPT == MODEL_KIO_430) //nsmoon@200402
#ifndef USE_CUST_MALLOC //nsmoon@201012
#define MODEL_SPT_X_CELL 	64
#define MODEL_SPT_Y_CELL 	45
#define MODEL_SPT_X_SLOPE	18 //25
#define MODEL_SPT_Y_SLOPE	40 //45
#endif
#define MODEL_TOUCH_SIZE	MODEL_TOUCH_SIZE_55
#define MAX_TOUCH_LIMIT_FINE        4
#define MAX_TOUCH_LIMIT_MULTI       10
#elif (MODEL_SPT == MODEL_CTKC_750) //nsmoon@191202
#define MODEL_SPT_X_CELL 	130
#define MODEL_SPT_Y_CELL 	90
#define MODEL_SPT_X_SLOPE	35
#define MODEL_SPT_Y_SLOPE	63
#define MODEL_TOUCH_SIZE	MODEL_TOUCH_SIZE_75
#define MAX_TOUCH_LIMIT_FINE        4
#define MAX_TOUCH_LIMIT_MULTI       10
#else
#error "not defined MODEL_SPT"
#endif

/////////////////////////////////////////////////////
// Compile Options
//
//#define ENABLE_CHECK_INVALID_PD //nsmoon@170205
//#define LARGE_SEN_GAP_16 //nsmoon@190313

//#define CLIPPING_CHK_ADJ_VERT //remove short edge of polygon, //nsmoon@190320 OFF
//#define DEBUG_EXPECTATION_TRACKING
//#define FINE_DATA_CHECK //debug@190420-1, ignore fine data

#if defined(_WIN32) || defined(WIN32)
#define MODEL_MCU			X86
#else
#if (MODEL_SPT == MODEL_IRT_550)
#define MODEL_MCU			RT1064
#elif (MODEL_SPT == MODEL_SPT_550)
#define MODEL_MCU			STM32H7
#elif (MODEL_SPT == MODEL_SPT_750)
#define MODEL_MCU			RT1052
#elif (MODEL_SPT == MODEL_CTKS_750_V120) //nsmoon@191212
#define MODEL_MCU			RT1064
#elif (MODEL_SPT == MODEL_CTKS_750_V130) || (MODEL_SPT == MODEL_CTKS_750_V140) || (MODEL_SPT == MODEL_CTSI_650_V100) || \
    (MODEL_SPT == MODEL_CTSI_N750_V100) || (MODEL_SPT == MODEL_CTSI_N650_V100) //nsmoon@230411
#define MODEL_MCU			RT1064
#elif (MODEL_SPT == MODEL_CTSI_550_SINGLE_V100) //nsmoon@211116
#define MODEL_MCU			PIC_32
#elif (MODEL_SPT == MODEL_CTSK_850_V100) //nsmoon@200422
#define MODEL_MCU			RT1064
#elif (MODEL_SPT == MODEL_CTSO_430_V100) //nsmoon@210222
#define MODEL_MCU			RT1064
#elif (MODEL_SPT == MODEL_KIO_550) || (MODEL_SPT == MODEL_KIO_430) //nsmoon@200402
#define MODEL_MCU			RT1064
#elif (MODEL_SPT == MODEL_CTKC_750) //nsmoon@191202
#define MODEL_MCU			RT1064
#else
#error "not defined MODEL_SPT"
#endif
#endif //defined(_WIN32) || defined(WIN32)
#if !defined(MODEL_MCU)
#error "Not defined MODEL_MCU !!!"
#endif

#if (MODEL_MCU == X86)
///////////////////////////////////////////////////////
// debug tool
#define DEBUG_TOOL_QT       1  //1:QT, 0:VS
#define USE_ADC_DATA	0
#if (MODEL_SPT != MODEL_KIO_550) && (MODEL_SPT != MODEL_KIO_430) //nsmoon@200402
#define FRONTEND_LINE_THRESHOLD
#endif
#if !defined(DEBUG_TOOL_QT)
#error "Not defined DEBUG_TOOL_QT !!!"
#endif
#if (DEBUG_TOOL_QT == 1)
#define TEMP_FILE_WRITE
//#define SHOW_DEBUG_TRACKING
//#define SHOW_DEBUG_SIZE_VAR
//#define SHOW_DEMO_SIZE
#define SHOW_DEBUG_SIZE_DOWN_0205
//#define DEBUG_INITIAL_SIZESQ
//#define SIZE_FILE_WRITE
//#define SIZE_POLYGON_SHOW
#endif
////////////////////////////////////////////////////////////////////////
// draw line : DRAW_LINE_TEST
// draw poly : line data + backend => DRAW_POLYGON_TEST + DRAW_LINE_TEST
// draw poly : poly data           => DRAW_POLYGON_TEST + POLYGON_DATA
////////////////////////////////////////////////////////////////////////
// Draw line or polygon test
#if defined(DRAW_LINE_TEST) && (DRAW_LINE_TEST == 0)
   #if 1 //(QT_NO_DEBUG == 0)
#define DRAW_POLYGON_TEST // DPT
#endif
#endif
#elif (MODEL_MCU == PIC_32)
  #define USE_ADC_DATA	0
  #define FRONTEND_LINE_THRESHOLD
#elif (MODEL_MCU == STM32H7)
#define USE_ADC_DATA	0
 #if (USE_ADC_DATA == 1)
  #define FRONTEND_ADC_V2
 #endif
#elif (MODEL_MCU == AM335X)
 #define USE_ADC_DATA  0 //FIXME
 #define FRONTEND_LINE_THRESHOLD
#elif (MODEL_MCU == RT1052) || (MODEL_MCU == RT1064)
  #define USE_ADC_DATA	0
#if (MODEL_SPT != MODEL_KIO_550) && (MODEL_SPT != MODEL_KIO_430) //nsmoon@200402
  #define FRONTEND_LINE_THRESHOLD
 #endif
#endif

#ifdef FRONTEND_LINE_THRESHOLD
#define MIN_THRESHOLD_CNT		(3)
#define MIN_THRESHOLD_CNT_EDGE	(1)
#endif

#if defined(_WIN32) || defined(WIN32) || (MODEL_MCU == PIC_32) //nsmoon@211116
#define ATTR_DTCM_RAM
#define ATTR_ITCM_RAM
#define ATTR_BACKEND_RAM
#define ATTR_BACKEND_RAM2
#define ATTR_BACKEND_RAM3
#define ATTR_BACKEND_RAMFUNC
#define ATTR_BACKEND_CONST  const
#elif (MODEL_MCU == STM32H7)
//#define ATTR_DTCM_RAM __attribute__((section(".dtcm_ram")))
//#define ATTR_ITCM_RAM __attribute__((section(".itcm_ram")))
#define ATTR_BACKEND_RAM __attribute__((section(".backend_ram")))
#define ATTR_BACKEND_RAM2 ATTR_BACKEND_RAM
#define ATTR_BACKEND_RAM3 ATTR_BACKEND_RAM
#define ATTR_BACKEND_RAMFUNC
#define ATTR_BACKEND_CONST	const
#elif (MODEL_MCU == AM335X)
#define ATTR_BACKEND_RAM
#define ATTR_BACKEND_RAM2
#define ATTR_BACKEND_RAM3
#define ATTR_BACKEND_RAMFUNC
#define ATTR_BACKEND_CONST  __attribute__((aligned(4))) const
#elif (MODEL_MCU == RT1052) || (MODEL_MCU == RT1064)
#define ATTR_BACKEND_RAM
#define ATTR_BACKEND_RAM2 __attribute__((section(".bss.$RAM2")))  //SRAM_ITC
#define ATTR_BACKEND_RAM3 __attribute__((section(".bss.$RAM3")))  //SRAM_OC
#define ATTR_BACKEND_RAMFUNC
#define ATTR_BACKEND_CONST	__attribute__((aligned(4))) const
#else
#error "Not defined MODEL_MCU !!!"
#define ATTR_DTCM_RAM
#define ATTR_ITCM_RAM
#define ATTR_BACKEND_RAM
#define ATTR_BACKEND_RAM2
#define ATTR_BACKEND_RAM3
#define ATTR_BACKEND_RAMFUNC
#define ATTR_BACKEND_CONST	const
#endif

#if !defined(DRAW_LINE_TEST)
#error "Not defined DRAW_LINE_TEST !!!"
#endif

#if defined(DRAW_LINE_TEST) && (DRAW_LINE_TEST == 1) //|| defined(DRAW_POLYGON_TEST)
#define X_NUM_PD				255
#define Y_NUM_PD				255
#define MAX_X_SLOPE_VAL			127
#define MAX_Y_SLOPE_VAL			127
#else
#ifndef USE_CUST_MALLOC //nsmoon@201012
#define X_NUM_PD				MODEL_SPT_X_CELL //158 //142 //nsmoon@201013
#define Y_NUM_PD				MODEL_SPT_Y_CELL //96  // //nsmoon@201013
#define MAX_X_SLOPE_VAL			MODEL_SPT_X_SLOPE //40  //nsmoon@201013
#define MAX_Y_SLOPE_VAL			MODEL_SPT_Y_SLOPE //63 //40  //nsmoon@201013
#define MAX_X_SLOPE_IDX         ((MAX_X_SLOPE_VAL * 2) + 1)  //81:-40~0~+40
#define MAX_Y_SLOPE_IDX         ((MAX_Y_SLOPE_VAL * 2) + 1)  //127:-63~0~+63
#define MAX_INIT_EPA_SIZE		(MAX_X_SLOPE_VAL + 1) //nsmoon@201013
#endif
#endif

#if (SECOND_OFST_TBL == 1) //nsmoon@190625
#define MAX_2ND_SLOPE_IDX		15
#endif

#ifndef USE_CUST_MALLOC //nsmoon@201012
#define MAX_SLOPE_BYTE_X        ((MAX_X_SLOPE_IDX + 7) / 8)
#define MAX_SLOPE_BYTE_Y        ((MAX_Y_SLOPE_IDX + 7) / 8)
#define MAX_NUM_PD				(X_NUM_PD + Y_NUM_PD) //nsmoon@201013
#define MAX_NUM_X_SLOPES		MAX_X_SLOPE_IDX
#define MAX_NUM_Y_SLOPES		MAX_Y_SLOPE_IDX
#define MAX_SLOPE_COUNT			MAX_NUM_Y_SLOPES
#define MAX_P1_P2_VE			(MAX_SLOPE_COUNT * 2)
#endif

#define MAX_EP_COUNT			(MAX_NUM_PD * ALLOWABLE_TOUCH_BE * 3) //nsmoon@190725, FIXME

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _BACKEND_COMMON_H */
/*end of file*/
