/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */
#ifndef _IRTP_LAYOUT_H    /* Guard against multiple inclusion */
#define _IRTP_LAYOUT_H

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

// Misc Monitor active area margin, nsmoon@170119
#define	MISC_MONITOR_MARGIN_X	0.0f // (float)(13.76)
#define	MISC_MONITOR_MARGIN_Y	0.0f // (float)(13.76)

//////////////////////////////////////////////////////
// Exported Global variable prototypes
extern float BS_sensor_zero_x;
extern float BS_sensor_end_x;
extern float BS_sensor_zero_y;
extern float BS_sensor_end_y;
extern float BS_aarea_zero_x;
extern float BS_aarea_end_x;
extern float BS_aarea_zero_y;
extern float BS_aarea_end_y;
extern float *BS_pd_pos_x, *BS_pd_pos_y;
extern float *BS_led_pos_x, *BS_led_pos_y;
extern float BS_half_end_x;
extern float BS_half_end_y;
#if (LARGE_PITCH_TBL == 1) //nsmoon@200108
extern float BS_large_pitch_zero_x;
extern float BS_large_pitch_end_x;
extern float BS_large_pitch_zero_y;
extern float BS_large_pitch_end_y;
#endif

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _IRTP_LAYOUT_H */

/* *****************************************************************************
 End of File
 */
