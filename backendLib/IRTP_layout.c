/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    IRTP_layout.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */
#ifndef  _irtp_layout_c
#define  _irtp_layout_c
#endif

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include "backend_main.h"
#include "IRTP_layout.h"

#ifdef USE_CUST_MALLOC //nsmoon@201012
ATTR_BACKEND_RAM int BS_maxHorPdNum, BS_maxVerPdNum; //nsmoon@201013
#else
ATTR_BACKEND_RAM uint16_t BS_maxHorPdNum;  //nsmoon@170403a
ATTR_BACKEND_RAM uint16_t BS_maxVerPdNum;
#endif

ATTR_BACKEND_RAM float BS_sensor_zero_x;
ATTR_BACKEND_RAM float BS_sensor_end_x;  //(sensor_pos_x[] + sensor_pos_x[0])
ATTR_BACKEND_RAM float BS_sensor_zero_y;
ATTR_BACKEND_RAM float BS_sensor_end_y;  //(sensor_pos_y[] + sensor_pos_y[0])
ATTR_BACKEND_RAM float BS_aarea_zero_x;  // Active Area
ATTR_BACKEND_RAM float BS_aarea_end_x;   // sensor_end_x - aarea_zero_x
ATTR_BACKEND_RAM float BS_aarea_zero_y;
ATTR_BACKEND_RAM float BS_aarea_end_y;
ATTR_BACKEND_RAM float *BS_pd_pos_x, *BS_pd_pos_y;
ATTR_BACKEND_RAM float *BS_led_pos_x, *BS_led_pos_y;
ATTR_BACKEND_RAM float BS_half_end_x;
ATTR_BACKEND_RAM float BS_half_end_y;
#if (LARGE_PITCH_TBL == 1) //nsmoon@200108
ATTR_BACKEND_RAM float BS_large_pitch_zero_x;
ATTR_BACKEND_RAM float BS_large_pitch_end_x;
ATTR_BACKEND_RAM float BS_large_pitch_zero_y;
ATTR_BACKEND_RAM float BS_large_pitch_end_y;
#endif
/* *****************************************************************************
 End of File
 */
