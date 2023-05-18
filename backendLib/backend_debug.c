/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Fine Touch Routine

  @File Name
    fineTouch.c

  @Summary
    Fine Touch Routine

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */
#include "backend_main.h"
#include "backend_multi.h"
#include "backend_fine.h"
#include "backend_debug.h"
#include "IRTP_layout.h"
#ifdef DEBUG_GET_TIME_DIFF_ENABLE //nsmoon@220119
#include <stdio.h>
#endif

///////////////////////////////////////////////////////////////////////////
// DEBUG variable
///////////////////////////////////////////////////////////////////////////
#ifdef DRAW_POLYGON_TEST
ATTR_BACKEND_RAM3 vec_e_t  BS_debugVert[MAX_DEBUG_VERT];
#endif

ATTR_BACKEND_RAM2 int maxVertex, maxProcessCnt, maxSubjCnt, maxClipIdx;
#ifdef DRAW_POLYGON_TEST
ATTR_BACKEND_RAM static unsigned int debug_color;
#endif
//ATTR_BACKEND_RAM static char debugTimeDiff[128];
#ifdef DEBUG_GET_TIME_DIFF_ENABLE //nsmoon@220119
ATTR_BACKEND_RAM int debugTimeDiffCnt, s_time_diff_cnt;
ATTR_BACKEND_RAM debug_time_diff_t debugTimeDiff[DEBUG_TIME_DIFF_MAX];
#endif

#if defined(_WIN32) || defined(WIN32)  //FIXME nsmoon@170111
#include <stdio.h>
#define TRACE_FILE_NAME "trace.txt"
#if (DEBUG_TOOL_QT == 1)
//FIXME
#else
static BOOL _writeTraceToFile(char *dbgStr, int dbgStrLen)
{
	HANDLE hFile;
	BOOL bErrorFlag = FALSE;
	DWORD dwPos, dwBytesWritten;
    if (dbgStrLen < 1) {
        TRACE_ERROR("ERROR: _writeTraceToFile()..invalid dbgStrLen %d\n", dbgStrLen);
        return FALSE;
    }

	hFile = CreateFile(TRACE_FILE_NAME,         // name of the write
		GENERIC_WRITE,          // open for writing
		0,                      // do not share
		NULL,                   // default security
		OPEN_ALWAYS,             // create new file only
		FILE_ATTRIBUTE_NORMAL,  // normal file
		NULL);                  // no attr. template

	if (hFile == INVALID_HANDLE_VALUE)
	{
		TRACE_ERROR("ERROR: _writeTraceToFile()..CreateFile(): %s (%s)[%d]\n", TRACE_FILE_NAME, myGetLastErrorString(), BG_frame_no);
		return bErrorFlag;
	}

	dwPos = SetFilePointer(hFile, 0, NULL, FILE_END);

	bErrorFlag = WriteFile(
		hFile,           // open file handle
		dbgStr,      // start of data to write
		dbgStrLen,  // number of bytes to write
		&dwBytesWritten, // number of bytes that were written
		NULL);
	//NULL);            // no overlapped structure

	if (FALSE == bErrorFlag)
	{
		TRACE_ERROR("ERROR: _writeTraceToFile()..WriteFile(): %s (%s)[%d]\n", TRACE_FILE_NAME, myGetLastErrorString(), BG_frame_no);
	}
	else
	{
		if (dwBytesWritten != (DWORD)dbgStrLen)
		{
			// This is an error because a synchronous write that results in
			// success (WriteFile returns TRUE) should write all data as
			// requested. This would not necessarily be the case for
			// asynchronous writes.
			TRACE_ERROR("ERROR: dwBytesWritten != dwBytesToWrite\n");
			bErrorFlag = FALSE;
		}
		else
		{
			bErrorFlag = TRUE;
		}
	}

	CloseHandle(hFile);
	return bErrorFlag;
}
#endif //DEBUG_TOOL_QT

#if (DEBUG_TOOL_QT == 1)
#if 1 //nsmoon@181001
ATTR_BACKEND_RAM polygon_tmp_t polygon_saved[MAX_PS_SIZE];
ATTR_BACKEND_RAM int polgon_saved_idx;
ATTR_BACKEND_RAM int bStartAddPol;
#if 0 //for testing
#define MY_RGB(r,g,b) ((unsigned long)(((unsigned char)(r)|((unsigned short)((unsigned char)(g))<<8))|(((unsigned long)(unsigned char)(b))<<16)))
unsigned long rgb_color[MY_COLOR + 1] = {
	0, //0 //black
	MY_RGB(255,0,0),	//1 //blue
	MY_RGB(255,0,255),	//2 //magenta
	MY_RGB(192,192,192),	//3 //silver
	MY_RGB(128,128,128),	//4 //gray
	MY_RGB(0,0,128),	//5 //maroon
	MY_RGB(0,128,128),	//6 //oliver
	MY_RGB(0,128,0),	//7 //green
	MY_RGB(128,0,128),	//8 //purple
	MY_RGB(128,128,0),	//9 //teal
	MY_RGB(0,255,255),	//10 //yellow
	MY_RGB(255,255,0),	//11 //cyan
	MY_RGB(0,255,0),	//12 //lime
	MY_RGB(0,0,255) //13 //red
};
#endif //0

void init_polygon_saved()
{
	for (int i = 0; i < MAX_PS_SIZE; i++) {
		polygon_saved[i].len = 0;
		polygon_saved[i].flag = PS_NUSED;
	}
	polgon_saved_idx = 0;
}

int add_polygon_saved(poly_e s, int flag, unsigned long color)
{
    //TRACE("<%d,%d>", polgon_saved_idx, bStartAddPol);
    if (polgon_saved_idx >= 0 && polgon_saved_idx < MAX_PS_SIZE) {
	if (bStartAddPol != 0) {
        //TRACE("flag,len,color= %d %d %d", flag, s->len, color);
		polygon_saved[polgon_saved_idx].flag = flag;
		polygon_saved[polgon_saved_idx].len = s->len;
		polygon_saved[polgon_saved_idx].color = color;
		if (s->len >= MAX_SLOPE_COUNT) {
			TRACE_ERROR("ERROR! add_polygon_saved..invalid s->len %d", s->len);
			s->len = MAX_SLOPE_COUNT;
		}
		for (int i = 0; i < s->len; i++) {
			polygon_saved[polgon_saved_idx].vert[i].x = s->ve[i].vert.x;
			polygon_saved[polgon_saved_idx].vert[i].y = s->ve[i].vert.y;
		}
		polgon_saved_idx++;
		return TRUE;
	}
	}
    else {
        TRACE_ERROR("Error! add_polygon_saved...polgon_saved_idx is %d => 0", polgon_saved_idx);
        //polgon_saved_idx = 0;  //reset
        return FALSE;
    }
	return FALSE;
}

int is_myDrawPolygon(void)
{
	if (polgon_saved_idx > 0 && bStartAddPol == FALSE) {
		return TRUE;
	}
	return FALSE;
}

int _myDrawPolygon(poly_e res, uint8_t flag, uint8_t color)
{
	if (flag == PS_START) {
		bStartAddPol = TRUE;
		init_polygon_saved();
		//TRACE("Start drawing polygon ##");
		return TRUE;
	}
	else if (flag == PS_END) {
		bStartAddPol = FALSE;
		//SendMessage(hWndMain, WM_POLY_DATA, 0, 0);
		return TRUE;
	}
	else if (flag & PS_RESTART) {
		polgon_saved_idx = 0;
	}

	if (res == NULL) {
		//TRACE("_myDrawPolygon... res is NULL !");
		return FALSE;
	}
	else if (polgon_saved_idx < 0 || polgon_saved_idx >= MAX_PS_SIZE) {
		TRACE_ERROR("Error! _myDrawPolygon...polgon_saved_idx is %d", polgon_saved_idx);
		return FALSE;
	}

	if (color > MY_COLOR) color = MY_COLOR;
    //TRACE("add_polygon_saved.. %d, %x, %d", res->len, flag, color);
	add_polygon_saved(res, flag, color);
	//add_polygon_saved(res, flag, rgb_color[color]);

#ifdef DRAW_POLY_ONE_BY_ONE
	if (flag & PS_FLUSH) {
		//SendMessage(hWndMain, WM_POLY_DATA, 0, 0);
	}
#endif

	return TRUE;
}
#endif
#else //DEBUG_TOOL_QT
#define BUFFCOUNT 1024
static char dbgStr[BUFFCOUNT];
void _myTrace(char const *format, ...)
{
	//int dbgStrLen;
	va_list argptr;
	va_start(argptr, format);
	//wvsprintf(dbgStr, format, argptr);
	vsnprintf(dbgStr, sizeof(dbgStr), format, argptr);
	va_end(argptr);
	//strcat_s(dbgStr, 2, "\r\n");
	OutputDebugString(dbgStr);
	OutputDebugString("\r\n");
#if 0 //for testing
	dbgStrLen = strlen(dbgStr);
	_writeTraceToFile(dbgStr, dbgStrLen);
	_writeTraceToFile("\r\n", 2);
#endif
}

void _myTraceNR(char const *format, ...)
{
	//int dbgStrLen;
	va_list argptr;
	va_start(argptr, format);
	//wvsprintf(dbgStr, format, argptr);
	vsnprintf(dbgStr, sizeof(dbgStr), format, argptr);
	va_end(argptr);
	OutputDebugString(dbgStr);
#if 0 //for testing
	dbgStrLen = strlen(dbgStr);
	_writeTraceToFile(dbgStr, dbgStrLen);
#endif
}

void _myTraceWrite(char const *format, ...)
{
	int dbgStrLen;
	va_list argptr;
	va_start(argptr, format);
	//wvsprintf(dbgStr, format, argptr);
	vsnprintf(dbgStr, sizeof(dbgStr), format, argptr);
	va_end(argptr);
	//OutputDebugString(dbgStr);
	//strcat_s(dbgStr, 2, "\r\n");
	dbgStrLen = (int)strlen(dbgStr);
	_writeTraceToFile(dbgStr, dbgStrLen);
	_writeTraceToFile("\r\n", 2);
}

void _myTraceWriteNR(char const *format, ...)
{
	int dbgStrLen;
	va_list argptr;
	va_start(argptr, format);
	//wvsprintf(dbgStr, format, argptr);
	vsnprintf(dbgStr, sizeof(dbgStr), format, argptr);
	va_end(argptr);
	//OutputDebugString(dbgStr);
	dbgStrLen = (int)strlen(dbgStr);
	_writeTraceToFile(dbgStr, dbgStrLen);
}

#define MAX_LAST_ERROR_MSG	2048
ATTR_BACKEND_RAM static char lastErrorMsg[MAX_LAST_ERROR_MSG];
const char *myGetLastErrorString(void)
{
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /*(LPWSTR)*/&lastErrorMsg[0], MAX_LAST_ERROR_MSG, NULL);
	return lastErrorMsg;
}
const char *myGetLastErrorString2(DWORD error)
{
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /*(LPWSTR)*/&lastErrorMsg[0], MAX_LAST_ERROR_MSG, NULL);
	return lastErrorMsg;
}
#endif //DEBUG_TOOL_QT

#else // defined(_WIN32) || defined(WIN32)
#ifdef DRAW_POLYGON_TEST //nsmoon@170205
#define BUFFCOUNT 1024
ATTR_BACKEND_RAM static uint8_t tmpBuf[MAX_POLY_TEMP_BUFF];
ATTR_BACKEND_RAM static char dbgStr[BUFFCOUNT];
ATTR_BACKEND_RAM static uint8_t tmpBuf[MAX_POLY_TEMP_BUFF];
ATTR_BACKEND_RAM static uint8_t send_polygon_tmp_buf[MAX_POLY_TEMP_BUFF];
#endif

#if 0 //(MODEL_MCU == STM32H7)
void _myTrace(char const *format, ...)
{
	//int dbgStrLen;
	va_list argptr;
	va_start(argptr, format);
	//wvsprintf(dbgStr, format, argptr);
	vsnprintf(dbgStr, sizeof(dbgStr), format, argptr);
	va_end(argptr);
	//strcat_s(dbgStr, 2, "\r\n");
	puts(dbgStr);
	puts("\r\n");
}

void _myTraceNR(char const *format, ...)
{
	//int dbgStrLen;
	va_list argptr;
	va_start(argptr, format);
	//wvsprintf(dbgStr, format, argptr);
	vsnprintf(dbgStr, sizeof(dbgStr), format, argptr);
	va_end(argptr);
	puts(dbgStr);
}
#endif
#ifdef DRAW_POLYGON_TEST //nsmoon@170205
int send_edgeptn_data_to_usb(uint16_t pdS, uint16_t pdE, int8_t slopeIdxAll, uint8_t flag, unsigned long color)
{
	int i, j, idx;
	poly_e_t poly;
	vec_e_t ve[5];
	int16_t ledS, ledE;
	int8_t slopeIdx;
	uint16_t tmpLength;
	uint32_t tmpLongX, tmpLongY;
	uint8_t tmpChar;

	if (flag == PS_START || flag == PS_END) {
		tmpBuf[0] = flag; //flag
		add_to_poly_tx_buffer(&tmpBuf[0], 1);
		return 1;
	}

	if (IS_ON_X_AXIS_SLOPE(slopeIdxAll)) {
		if (pdS >= BS_maxHorPdNum || pdE >= BS_maxHorPdNum) {
			TRACE("Error! send_edgeptn_data_to_usb... invalid pdS %d, pdE %d >= %d @slope %d", pdS, pdE, BS_maxHorPdNum, slopeIdxAll);
			return FALSE;
		}
		slopeIdx = slopeIdxAll;
	}
	else {
		if (pdS >= BS_maxVerPdNum || pdE >= BS_maxVerPdNum) {
			TRACE("Error! send_edgeptn_data_to_usb... invalid pdS %d, pdE %d >= %d @slope %d", pdS, pdE, BS_maxVerPdNum, slopeIdxAll);
			return FALSE;
		}
		slopeIdx = slopeIdxAll - (TotalNumberOfSlopes / 2);
	}
	if (slopeIdx < 0 || slopeIdx >= (TotalNumberOfSlopes / 2)) {
		TRACE("Error! send_edgeptn_data_to_usb... invalid slopeIdx %d", slopeIdx);
		return FALSE;
	}
	ledS = pdS + pdOffsetTbl[(uint8_t)slopeIdx];
	ledE = pdE + pdOffsetTbl[(uint8_t)slopeIdx];

	if (ledS < 0 || ledE < 0) {
		TRACE("Error! send_edgeptn_data_to_usb... invalid ledS %d, ledE %d < 0 @ %d, %d", ledS, ledE, pdS, pdE, slopeIdx, pdOffsetTbl[(uint8_t)slopeIdx]);
		return FALSE;
	}

	//TRACE("pd:%d, led:%d, ", pd, led);
	poly.ve = &ve[0];
	poly.len = 4;
	if (IS_ON_X_AXIS_SLOPE(slopeIdxAll)) {
		if (ledS >= BS_maxHorPdNum || ledE >= BS_maxHorPdNum) {
			TRACE("Error! send_edgeptn_data_to_usb... invalid ledS %d, ledE %d >= %d @ %d %d %d %d", ledS, ledE, BS_maxHorPdNum, pdS, pdE, slopeIdx, pdOffsetTbl[(uint8_t)slopeIdx]);
			return FALSE;
		}
		poly.ve[0].vert.x = (float)BS_sensor_pos_x[BS_maxHorPdNum - 1 - pdS];
		poly.ve[0].vert.y = (float)BS_sensor_zero_y;
		poly.ve[1].vert.x = (float)BS_sensor_pos_x[BS_maxHorPdNum - 1 - ledS];
		poly.ve[1].vert.y = (float)BS_sensor_end_y;

		poly.ve[2].vert.x = (float)BS_sensor_pos_x[BS_maxHorPdNum - 1 - ledE];
		poly.ve[2].vert.y = (float)BS_sensor_end_y;
		poly.ve[3].vert.x = (float)BS_sensor_pos_x[BS_maxHorPdNum - 1 - pdE];
		poly.ve[3].vert.y = (float)BS_sensor_zero_y;
	}
	else {
		if (ledS >= BS_maxVerPdNum || ledE >= BS_maxVerPdNum) {
			TRACE("Error! send_edgeptn_data_to_usb... invalid ledS %d, ledE %d >= %d @ %d %d %d %d", ledS, ledE, BS_maxVerPdNum, pdS, pdE, slopeIdx, BS_pdOffsetTbl[(uint8_t)slopeIdx]);
			return FALSE;
		}
		poly.ve[0].vert.x = (float)BS_sensor_end_x;
		poly.ve[0].vert.y = (float)(BS_sensor_end_y - BS_sensor_pos_y[pdS]);
		poly.ve[1].vert.x = (float)BS_sensor_zero_x;
		poly.ve[1].vert.y = (float)(BS_sensor_end_y - BS_sensor_pos_y[ledS]);

		poly.ve[2].vert.x = (float)BS_sensor_zero_x;
		poly.ve[2].vert.y = (float)(BS_sensor_end_y - BS_sensor_pos_y[ledE]);
		poly.ve[3].vert.x = (float)BS_sensor_end_x;
		poly.ve[3].vert.y = (float)(BS_sensor_end_y - BS_sensor_pos_y[pdE]);
	}

	idx = 0;
	tmpBuf[idx++] = flag; //0 flag
	tmpLength = (uint8_t)poly.len;
	if (tmpLength >= (MAX_POLY_TEMP_BUFF - 4)) {
		tmpLength = (MAX_POLY_TEMP_BUFF - 5);
		TRACE_ERROR("Error! send_edgeptn_data_to_usb... invalid tmpLength %d", tmpLength);
	}
	tmpBuf[idx++] = tmpLength; //1 len
	for (i = 0; i < poly.len; i++) {
		tmpLongX = *(unsigned long *)&poly.ve[i].vert.x;
		//pByte = (unsigned char *)&tmpLong;
		for (j = 0; j < sizeof(float); j++) {
			tmpChar = (unsigned char)(tmpLongX >> (8 * j));
			tmpBuf[idx++] = tmpChar;
		}
		tmpLongY = *(unsigned long *)&poly.ve[i].vert.y;
		//pByte = (unsigned char *)&tmpLong;
		for (j = 0; j < sizeof(float); j++) {
			tmpChar = (unsigned char)(tmpLongY >> (8 * j));
			tmpBuf[idx++] = tmpChar;
		}
	}

	add_to_poly_tx_buffer(&tmpBuf[0], idx);
	//TRACENR("send_edgeptn_data_to_usb... %d %d (%0.1f %0.1f) (%0.1f %0.1f) ", pdS, ledS, poly.ve[0].vert.x, poly.ve[0].vert.y, poly.ve[1].vert.x, poly.ve[1].vert.y);
	//TRACE("%d %d (%0.1f %0.1f) (%0.1f %0.1f)", pdE, ledE, poly.ve[2].vert.x, poly.ve[2].vert.y, poly.ve[3].vert.x, poly.ve[3].vert.y);
	return 1;
}

int send_polygon_data_to_usb(poly_e poly, uint8_t flag, unsigned long color)
{
	int i, j, idx;
	//unsigned char *pByte;
	uint32_t tmpLongX, tmpLongY;
	uint16_t tmpLength;
	uint8_t tmpChar;

	//TRACE("send_polygon_data_to_usb...");
	if (flag == PS_START || flag == PS_END) {
		send_polygon_tmp_buf[0] = flag; //flag
		add_to_poly_tx_buffer(&send_polygon_tmp_buf[0], 1);
		return 1;
	}

	idx = 0;
	send_polygon_tmp_buf[idx++] = flag; //0 flag
	tmpLength = (uint8_t)poly->len;
	if (tmpLength > (MAX_POLY_TEMP_BUFF - 1)) {
		tmpLength = (MAX_POLY_TEMP_BUFF - 1);
	}
	else if (tmpLength == 0) {
		// nothing to send
		return 0;
	}
	send_polygon_tmp_buf[idx++] = tmpLength; //1

											 //TRACENR("tmpLength %d ", tmpLength);
	for (i = 0; i < poly->len; i++) {
		tmpLongX = *(unsigned long *)&poly->ve[i].vert.x;
		for (j = 0; j < sizeof(float); j++) {
			tmpChar = (unsigned char)(tmpLongX >> (8 * j));
			send_polygon_tmp_buf[idx++] = tmpChar;
		}
		tmpLongY = *(unsigned long *)&poly->ve[i].vert.y;
		for (j = 0; j < sizeof(float); j++) {
			tmpChar = (unsigned char)(tmpLongY >> (8 * j));
			send_polygon_tmp_buf[idx++] = tmpChar;
		}
		//TRACENR("(%04x:%0.1f,%04x:%0.1f) ", tmpLongX, poly->ve[i].vert.x, tmpLongY, poly->ve[i].vert.y);
	}
	//TRACENR("\r\n");

	add_to_poly_tx_buffer(&send_polygon_tmp_buf[0], idx);
	return 1;
}
#endif

#if 0
int send_polygon_data_to_serial(poly_e poly, uint8_t flag)
{
	int i, j;
	//unsigned char *pByte;
	unsigned long tmpLong, tmpLongX, tmpLongY;
	unsigned char tmpChar;
	static unsigned char cnt;

	Print_char_Debug(0x12);
	Print_char_Debug(0x34);
	Print_char_Debug(0x56);

	// send flag
	Print_char_Debug(flag);
	if (flag == 0xFD || flag == 0xFE) {
		if (flag == 0xFD) cnt = 0;
		Print_char_Debug(cnt);
		return 1;
	}
	cnt++;

	// send length
	tmpLong = *(unsigned long *)&poly->len;
	//pByte = (unsigned char *)&tmpLong;
	for (j = 0; j < sizeof(int); j++) {
		tmpChar = (unsigned char)(tmpLong >> (8 * j));
		Print_char_Debug(tmpChar);
	}
	if (tmpLong == 0) return 0;

	// send pos
	for (i = 0; i < poly->len; i++) {
		tmpLongX = *(unsigned long *)&poly->ve[i].vert.x;
		//pByte = (unsigned char *)&tmpLong;
		for (j = 0; j < sizeof(float); j++) {
			tmpChar = (unsigned char)(tmpLongX >> (8 * j));
			Print_char_Debug(tmpChar);
		}
		tmpLongY = *(unsigned long *)&poly->ve[i].vert.y;
		//pByte = (unsigned char *)&tmpLong;
		for (j = 0; j < sizeof(float); j++) {
			tmpChar = (unsigned char)(tmpLongY >> (8 * j));
			Print_char_Debug(tmpChar);
		}
	}
	return 1;
}

int send_touch_data_to_serial(vec touch, uint8_t flag, int len)
{
	int j;
	//unsigned char *pByte;
	unsigned long tmpLong, tmpLongX, tmpLongY;
	unsigned char tmpChar;

	Print_char_Debug(0x12);
	Print_char_Debug(0x34);
	Print_char_Debug(0x56);

	// send flag
	Print_char_Debug(flag);

	// send length
	tmpLong = *(unsigned long *)&len;
	for (j = 0; j < sizeof(int); j++) {
		tmpChar = (unsigned char)(tmpLong >> (8 * j));
		Print_char_Debug(tmpChar);
	}

	if (flag == 0xFD || flag == 0xFE) {
		return 1;
	}

	// send pos
	tmpLongX = *(unsigned long *)&touch->x;
	for (j = 0; j < sizeof(float); j++) {
		tmpChar = (unsigned char)(tmpLongX >> (8 * j));
		Print_char_Debug(tmpChar);
	}
	tmpLongY = *(unsigned long *)&touch->y;
	for (j = 0; j < sizeof(float); j++) {
		tmpChar = (unsigned char)(tmpLongY >> (8 * j));
		Print_char_Debug(tmpChar);
	}
	return 1;
}
#endif
#endif // defined(_WIN32) || defined(WIN32)

///////////////////////////////////////////////////////////////////////////
// DEBUG functions
///////////////////////////////////////////////////////////////////////////
#ifdef DEBUG_GET_TIME_DIFF_ENABLE //nsmoon@220119
extern uint32_t s_belib_time_diff(void);
void DEBUG_get_time_diff_init(void)
{
    int i;
    s_time_diff_cnt = 0;
    debugTimeDiffCnt = 0;
    for (i = 0; i < DEBUG_TIME_DIFF_MAX; i++) {
        debugTimeDiff[i].seqStr[0] = '\0';
        debugTimeDiff[i].timeDiff = 0;
    }
}

void DEBUG_get_time_diff(char *seqStr)
{
	uint32_t time_diff;
	char *tmpStr;
    if (debugTimeDiffCnt < DEBUG_TIME_DIFF_MAX) {
        time_diff = s_belib_time_diff();
        tmpStr = &debugTimeDiff[debugTimeDiffCnt].seqStr[0];
        debugTimeDiff[debugTimeDiffCnt].timeDiff = time_diff;
        snprintf(tmpStr, DEBUG_TIME_DIFF_STR_LEN, "%s", seqStr);
        debugTimeDiffCnt++;
    }
    else {
        TRACE_ERROR("ERROR! invalid debugTimeDiffCnt %d", debugTimeDiffCnt);
    }
}

#define DEBUG_SLOW_TIME_DIFF_SHOW_INTERVAL	300000
void DEBUG_slow_time_diff_show(void)
{
    int i, totalTime = 0;
    DEBUG_GET_TIME_DIFF(DEBUG_TIME_DIFF_END);

    if (s_time_diff_cnt >= DEBUG_SLOW_TIME_DIFF_SHOW_INTERVAL) {
    	s_time_diff_cnt = 0;
        for (i = 0; i < debugTimeDiffCnt; i++) {
        	if (i > 0) {
        		totalTime += debugTimeDiff[i].timeDiff;
        	}
        	TRACE_TIME_DIFF("%s/%d ", debugTimeDiff[i].seqStr, debugTimeDiff[i].timeDiff);
        }
        TRACE_TIME_DIFF("%d\r\n", totalTime);
    }
    else {
        for (i = 0; i < debugTimeDiffCnt; i++) {
    		totalTime += debugTimeDiff[i].timeDiff;
        }
      	s_time_diff_cnt += totalTime;
    }

    debugTimeDiffCnt = 0;
}
#endif //DEBUG_GET_TIME_DIFF_ENABLE

#ifdef DRAW_POLYGON_TEST
void DEBUG_show_poly_(vec_e_t *poly, int len, int color)
{
    IS_NOT_DEBUG_FLAG return;
    poly_e_t tmpPoly;
    tmpPoly.len = len;
    tmpPoly.ve = poly;
    SEND_POLY(&tmpPoly, 0x11, (uint8_t)color);
    TRACE_NOP;
}

#if 0 //for testing
void DEBUG_show_epa_ep_(axis_t axis, ep_no_t *epNo, int color)
{
    IS_NOT_DEBUG_FLAG return;

    poly_e_t tmpPoly;
    float padS1 = 0.0f, padS2 = 0.0f, padE1 = 0.0f, padE2 = 0.0f;

    tmpPoly.len = INITIAL_POLYGON_VERTEX_NO;
    tmpPoly.ve = &BS_debugVert[0];

    if (axis == ENUM_HOR_X) {
#if 0 //for testing
        if (color == 0) {
            if (epNo->s1 - 1 >= 0) {
                padS1 = (BS_led_pos_x[epNo->s1] - BS_led_pos_x[epNo->s1 - 1]) * 0.5f;
            }
            if (epNo->e1 - 1 >= 0) {
                padE1 = (BS_led_pos_x[epNo->e1] - BS_led_pos_x[epNo->e1 - 1]) * 0.5f;
            }
            if (epNo->s2 + 1 < BS_maxHorPdNum) {
                padS2 = (BS_led_pos_x[epNo->s2 + 1] - BS_led_pos_x[epNo->s2]) * 0.5f;
            }
            if (epNo->e2 + 1 < BS_maxHorPdNum) {
                padE2 = (BS_led_pos_x[epNo->e2 + 1] - BS_led_pos_x[epNo->e2]) * 0.5f;
            }
        }
#endif
        tmpPoly.ve[0].vert.x = BS_led_pos_x[epNo->s1] - padS1; //bottom-right
        tmpPoly.ve[1].vert.x = BS_pd_pos_x[epNo->e1] - padE1; //top-right
        tmpPoly.ve[2].vert.x = BS_pd_pos_x[epNo->e2] + padE2; //top-left
        tmpPoly.ve[3].vert.x = BS_led_pos_x[epNo->s2] + padS2; //bottom-left
        tmpPoly.ve[0].vert.y = BS_sensor_zero_y; //bottom-right
        tmpPoly.ve[1].vert.y = BS_sensor_end_y; //top-right
        tmpPoly.ve[2].vert.y = BS_sensor_end_y; //top-left
        tmpPoly.ve[3].vert.y = BS_sensor_zero_y; //bottom-left
    }
    else {
#if 0 //for testing
        if (color == 0) {
            if (epNo->s1 - 1 >= 0) {
                padS1 = (BS_led_pos_y[epNo->s1] - BS_led_pos_y[epNo->s1 - 1]) * 0.5f;
            }
            if (epNo->e1 - 1 >= 0) {
                padE1 = (BS_led_pos_y[epNo->e1] - BS_led_pos_y[epNo->e1 - 1]) * 0.5f;
            }
            if (epNo->s2 + 1 < BS_maxVerPdNum) {
                padS2 = (BS_led_pos_y[epNo->s2 + 1] - BS_led_pos_y[epNo->s2]) * 0.5f;
            }
            if (epNo->e2 + 1 < BS_maxVerPdNum) {
                padE2 = (BS_led_pos_y[epNo->e2 + 1] - BS_led_pos_y[epNo->e2]) * 0.5f;
            }
        }
#endif
        tmpPoly.ve[0].vert.x = BS_sensor_zero_x; //bottom-right
        tmpPoly.ve[1].vert.x = BS_sensor_zero_x; //top-right
        tmpPoly.ve[2].vert.x = BS_sensor_end_x; //top-left
        tmpPoly.ve[3].vert.x = BS_sensor_end_x; //bottom-left
        tmpPoly.ve[0].vert.y = BS_led_pos_y[epNo->e1] - padE1; //bottom-right
        tmpPoly.ve[1].vert.y = BS_pd_pos_y[epNo->e2] + padE2; //top-right
        tmpPoly.ve[2].vert.y = BS_pd_pos_y[epNo->s2] + padS2; //top-left
        tmpPoly.ve[3].vert.y = BS_led_pos_y[epNo->s1] - padS1; //bottom-left
    }
#if 0 //for testing
    for (int i = 0; i < tmpPoly.len; i++)
    {
        TRACENR("<<%0.1f,%0.1f>> ", tmpPoly.ve[i].vert.x, tmpPoly.ve[i].vert.y);
    }
    TRACENR("\r\n");
#endif

    SEND_POLY(&tmpPoly, 0x11, color);
    TRACE_NOP;
}
#endif //0

void DEBUG_show_line_pd_led_(axis_t axis, int pd, int led, int color)
{
    IS_NOT_DEBUG_FLAG return;
    poly_e_t tmpPoly;
    tmpPoly.len = 2;
    tmpPoly.ve = &BS_debugVert[0];
    //draw_line(posX(BS_pd_pos_x[pd]), posY(BS_sensor_end_y), posX(BS_led_pos_x[led]), posY(BS_sensor_zero_y), 0);
    if (axis == ENUM_HOR_X) {
        tmpPoly.ve[0].vert.x = BS_led_pos_x[led]; //bottom
        tmpPoly.ve[0].vert.y = BS_sensor_zero_y;
        tmpPoly.ve[1].vert.x = BS_pd_pos_x[pd];  //top
        tmpPoly.ve[1].vert.y = BS_sensor_end_y;
    }
    else {
        tmpPoly.ve[0].vert.x = BS_sensor_end_x; //left
        tmpPoly.ve[0].vert.y = BS_led_pos_y[led];
        tmpPoly.ve[1].vert.x = BS_sensor_zero_x; //right
        tmpPoly.ve[1].vert.y = BS_pd_pos_y[pd];
    }
    //TRACE("DEBUG_show_line_ep..axis,pd,led: %d %d %d %0.1f, %0.1f, %0.1f, %0.1f", axis, pd, led, tmpPoly.ve[0].vert.x, tmpPoly.ve[0].vert.y, tmpPoly.ve[1].vert.x, tmpPoly.ve[01].vert.y);
#if 0 //for testing, ,
    for (int i = 0; i < tmpPoly.len; i++)
    {
        TRACENR("<<%0.1f,%0.1f>> ", tmpPoly.ve[i].vert.x, tmpPoly.ve[i].vert.y);
    }
    TRACENR("\r\n");
#endif

    SEND_POLY(&tmpPoly, 0x11, (uint8_t)color);
    TRACE_NOP;
}

void DEBUG_show_line_idx_(axis_t axis, int idx, int color)
{
    int pd, led;
    if (BS_getSenInBuf(axis, idx, &pd, &led) < 0) {
        TRACE_ERROR("ERROR! DEBUG_show_line_idx_..BS_getSenInBuf() [%d]", BG_frame_no);
        TRACE_NOP;
    }
    else {
        DEBUG_SHOW_LINE_PD_LED(axis, pd, led, color);
    }
}

#if 0 //for test
void DEBUG_show_pd_led_(axis_t axis, int pd, int led, int color)
{
    IS_NOT_DEBUG_FLAG return;
    poly_e_t tmpPoly;

    tmpPoly.len = 2;
    tmpPoly.ve = &BS_debugVert[0];
    if (axis == ENUM_HOR_X) {
        tmpPoly.ve[0].vert.x = BS_led_pos_x[led]; //bottom
        tmpPoly.ve[0].vert.y = BS_sensor_zero_y;
        tmpPoly.ve[1].vert.x = BS_led_pos_x[led];
        tmpPoly.ve[1].vert.y = BS_sensor_zero_y - DEBUG_LED_PD_LINE_LEN;
    }
    else {
        tmpPoly.ve[0].vert.x = BS_sensor_end_x;
        tmpPoly.ve[0].vert.y = BS_led_pos_y[led];
        tmpPoly.ve[1].vert.x = BS_sensor_end_x + DEBUG_LED_PD_LINE_LEN;
        tmpPoly.ve[1].vert.y = BS_led_pos_y[led];
    }
    SEND_POLY(&tmpPoly, 0x11, color);

    if (axis == ENUM_HOR_X) {
        tmpPoly.ve[0].vert.x = BS_pd_pos_x[pd]; //bottom
        tmpPoly.ve[0].vert.y = BS_sensor_end_y;
        tmpPoly.ve[1].vert.x = BS_pd_pos_x[pd];
        tmpPoly.ve[1].vert.y = BS_sensor_end_y + DEBUG_LED_PD_LINE_LEN;
    }
    else {
        tmpPoly.ve[0].vert.x = BS_sensor_zero_x;
        tmpPoly.ve[0].vert.y = BS_pd_pos_y[pd];
        tmpPoly.ve[1].vert.x = BS_sensor_zero_x - DEBUG_LED_PD_LINE_LEN;
        tmpPoly.ve[1].vert.y = BS_pd_pos_y[pd];
    }
    SEND_POLY(&tmpPoly, 0x11, color);

    TRACE_NOP;
}
#endif

#define DEBUG_LED_PD_LINE_LEN 10.0f
void DEBUG_show_start_end_(bd_type_t bd, int start, int end, int color)
{
    IS_NOT_DEBUG_FLAG return;
    poly_e_t tmpPoly;
    float *bdPos;

    tmpPoly.len = 2;
    tmpPoly.ve = &BS_debugVert[0];

    if (bd == ENUM_BOT_BD) {
        bdPos = &BS_led_pos_x[0];
        tmpPoly.ve[0].vert.x = bdPos[start]; //bottom
        tmpPoly.ve[0].vert.y = BS_sensor_zero_y;
        tmpPoly.ve[1].vert.x = bdPos[start];
        tmpPoly.ve[1].vert.y = BS_sensor_zero_y + DEBUG_LED_PD_LINE_LEN;
        SEND_POLY(&tmpPoly, 0x11, (uint8_t)color);
        tmpPoly.ve[0].vert.x = bdPos[end]; //bottom
        tmpPoly.ve[0].vert.y = BS_sensor_zero_y;
        tmpPoly.ve[1].vert.x = bdPos[end];
        tmpPoly.ve[1].vert.y = BS_sensor_zero_y + DEBUG_LED_PD_LINE_LEN;
        SEND_POLY(&tmpPoly, 0x11, (uint8_t)color);
    }
    else if (bd == ENUM_TOP_BD) {
        bdPos = &BS_pd_pos_x[0];
        tmpPoly.ve[0].vert.x = bdPos[start]; //bottom
        tmpPoly.ve[0].vert.y = BS_sensor_end_y;
        tmpPoly.ve[1].vert.x = bdPos[start];
        tmpPoly.ve[1].vert.y = BS_sensor_end_y - DEBUG_LED_PD_LINE_LEN;
        SEND_POLY(&tmpPoly, 0x11, (uint8_t)color);
        tmpPoly.ve[0].vert.x = bdPos[end]; //bottom
        tmpPoly.ve[0].vert.y = BS_sensor_end_y;
        tmpPoly.ve[1].vert.x = bdPos[end];
        tmpPoly.ve[1].vert.y = BS_sensor_end_y - DEBUG_LED_PD_LINE_LEN;
        SEND_POLY(&tmpPoly, 0x11, (uint8_t)color);
    }
    else if (bd == ENUM_LFT_BD) {
        bdPos = &BS_led_pos_y[0];
        tmpPoly.ve[0].vert.x = BS_sensor_end_x;
        tmpPoly.ve[0].vert.y = bdPos[start];
        tmpPoly.ve[1].vert.x = BS_sensor_end_x - DEBUG_LED_PD_LINE_LEN;
        tmpPoly.ve[1].vert.y = bdPos[start];
        SEND_POLY(&tmpPoly, 0x11, (uint8_t)color);
        tmpPoly.ve[0].vert.x = BS_sensor_end_x;
        tmpPoly.ve[0].vert.y = bdPos[end];
        tmpPoly.ve[1].vert.x = BS_sensor_end_x - DEBUG_LED_PD_LINE_LEN;
        tmpPoly.ve[1].vert.y = bdPos[end];
        SEND_POLY(&tmpPoly, 0x11, (uint8_t)color);
    }
    else {
        //(bd == ENUM_RHT_BD)
        bdPos = &BS_pd_pos_y[0];
        tmpPoly.ve[0].vert.x = BS_sensor_zero_x;
        tmpPoly.ve[0].vert.y = bdPos[start];
        tmpPoly.ve[1].vert.x = BS_sensor_zero_x + DEBUG_LED_PD_LINE_LEN;
        tmpPoly.ve[1].vert.y = bdPos[start];
        SEND_POLY(&tmpPoly, 0x11, (uint8_t)color);
        tmpPoly.ve[0].vert.x = BS_sensor_zero_x;
        tmpPoly.ve[0].vert.y = bdPos[end];
        tmpPoly.ve[1].vert.x = BS_sensor_zero_x + DEBUG_LED_PD_LINE_LEN;
        tmpPoly.ve[1].vert.y = bdPos[end];
        SEND_POLY(&tmpPoly, 0x11, (uint8_t)color);
    }
    TRACE_NOP;
}

#define DEBUG_POS_SIZE	0.2f
void DEBUG_show_pos_(vec pos, float wx, float wy, int color)
{
    IS_NOT_DEBUG_FLAG return;
    poly_e_t tmpPoly;
    tmpPoly.len = 4;
    tmpPoly.ve = &BS_debugVert[0];
    tmpPoly.ve[0].vert.x = pos->x - wx;
    tmpPoly.ve[0].vert.y = pos->y - wy;
    tmpPoly.ve[1].vert.x = pos->x - wx;
    tmpPoly.ve[1].vert.y = pos->y + wy;
    tmpPoly.ve[2].vert.x = pos->x + wx;
    tmpPoly.ve[2].vert.y = pos->y + wy;
    tmpPoly.ve[3].vert.x = pos->x + wx;
    tmpPoly.ve[3].vert.y = pos->y - wy;
    SEND_POLY(&tmpPoly, 0x11, (uint8_t)color);
    TRACE_NOP;
}

void DEBUG_show_line_axis_pos_(axis_t axis, float ledPos, float pdPos, int color)
{
    IS_NOT_DEBUG_FLAG return;
    poly_e_t tmpPoly;
    tmpPoly.ve = &BS_debugVert[0];
    tmpPoly.len = 2;
    if (axis == ENUM_HOR_X) {
        tmpPoly.ve[0].vert.x = ledPos;
        tmpPoly.ve[0].vert.y = BS_sensor_zero_y;
        tmpPoly.ve[1].vert.x = pdPos;
        tmpPoly.ve[1].vert.y = BS_sensor_end_y;
    }
    else {
        tmpPoly.ve[0].vert.x = BS_sensor_end_x;
        tmpPoly.ve[0].vert.y = ledPos;
        tmpPoly.ve[1].vert.x = BS_sensor_zero_x;
        tmpPoly.ve[1].vert.y = pdPos;
    }
    SEND_POLY(&tmpPoly, 0x11, (uint8_t)color);
    TRACE_NOP;
}

void DEBUG_show_line_(vec line, int color)
{
    IS_NOT_DEBUG_FLAG return;
    poly_e_t tmpPoly;
    tmpPoly.ve = &BS_debugVert[0];
    tmpPoly.len = 2;
    tmpPoly.ve[0].vert.x = line[0].x;
    tmpPoly.ve[0].vert.y = line[0].y;
    tmpPoly.ve[1].vert.x = line[1].x;
    tmpPoly.ve[1].vert.y = line[1].y;
    SEND_POLY(&tmpPoly, 0x11, (uint8_t)color);
    TRACE_NOP;
}

void DEBUG_show_line_pos_(vec p0, vec p1, int color)
{
    IS_NOT_DEBUG_FLAG return;
    poly_e_t tmpPoly;
    tmpPoly.ve = &BS_debugVert[0];
    tmpPoly.len = 2;
    tmpPoly.ve[0].vert.x = p0->x;
    tmpPoly.ve[0].vert.y = p0->y;
    tmpPoly.ve[1].vert.x = p1->x;
    tmpPoly.ve[1].vert.y = p1->y;
    SEND_POLY(&tmpPoly, 0x11, (uint8_t)color);
    TRACE_NOP;
}

void DEBUG_show_min_max_(pos_min_max_t *pos, int color, int mode)
{
    if (mode) {
        IS_NOT_DEBUG_FLAG return;
    }
    poly_e_t tmpPoly;
    tmpPoly.ve = &BS_debugVert[0];
    tmpPoly.len = 2;
    tmpPoly.ve[0].vert.x = pos->minX;
    tmpPoly.ve[0].vert.y = pos->minY;
    tmpPoly.ve[1].vert.x = pos->minX;
    tmpPoly.ve[1].vert.y = pos->maxY;
    SEND_POLY(&tmpPoly, 0x11, (uint8_t)color);

    tmpPoly.ve[0].vert.x = pos->minX;
    tmpPoly.ve[0].vert.y = pos->maxY;
    tmpPoly.ve[1].vert.x = pos->maxX;
    tmpPoly.ve[1].vert.y = pos->maxY;
    SEND_POLY(&tmpPoly, 0x11, (uint8_t)color);

    tmpPoly.ve[0].vert.x = pos->maxX;
    tmpPoly.ve[0].vert.y = pos->maxY;
    tmpPoly.ve[1].vert.x = pos->maxX;
    tmpPoly.ve[1].vert.y = pos->minY;
    SEND_POLY(&tmpPoly, 0x11, (uint8_t)color);

    tmpPoly.ve[0].vert.x = pos->maxX;
    tmpPoly.ve[0].vert.y = pos->minY;
    tmpPoly.ve[1].vert.x = pos->minX;
    tmpPoly.ve[1].vert.y = pos->minY;
    SEND_POLY(&tmpPoly, 0x11, (uint8_t)color);
    TRACE_NOP;
}

#define DEBUG_GRID_GAP  1.0f
void DEBUG_show_cent_grid_(vec_t *cent, int gridNum, int color)
{
    vec_t p0, p1;
    float gridSize = gridNum * DEBUG_GRID_GAP;
    int i;

    //draw x-axis
    p0.x = cent->x - gridSize;
    p0.y = cent->y;
    p1.x = cent->x + gridSize;
    p1.y = cent->y;
    DEBUG_SHOW_LINE_POS(&p0, &p1, color);
    for (i = 0; i <= gridNum; i++) {
        p0.x = cent->x + (i * DEBUG_GRID_GAP);
        p0.y = cent->y + DEBUG_GRID_GAP;
        p1.x = cent->x + (i * DEBUG_GRID_GAP);
        p1.y = cent->y - DEBUG_GRID_GAP;
        DEBUG_SHOW_LINE_POS(&p0, &p1, color);
        p0.x = cent->x - (i * DEBUG_GRID_GAP);
        p0.y = cent->y + DEBUG_GRID_GAP;
        p1.x = cent->x - (i * DEBUG_GRID_GAP);
        p1.y = cent->y - DEBUG_GRID_GAP;
        DEBUG_SHOW_LINE_POS(&p0, &p1, color);
    }

    //draw y-axis
    p0.y = cent->y - gridSize;
    p0.x = cent->x;
    p1.y = cent->y + gridSize;
    p1.x = cent->x;
    DEBUG_SHOW_LINE_POS(&p0, &p1, color);
    for (i = 0; i <= gridNum; i++) {
        p0.y = cent->y + (i * DEBUG_GRID_GAP);
        p0.x = cent->x + DEBUG_GRID_GAP;
        p1.y = cent->y + (i * DEBUG_GRID_GAP);
        p1.x = cent->x - DEBUG_GRID_GAP;
        DEBUG_SHOW_LINE_POS(&p0, &p1, color);
        p0.y = cent->y - (i * DEBUG_GRID_GAP);
        p0.x = cent->x + DEBUG_GRID_GAP;
        p1.y = cent->y - (i * DEBUG_GRID_GAP);
        p1.x = cent->x - DEBUG_GRID_GAP;
        DEBUG_SHOW_LINE_POS(&p0, &p1, color);
    }
}

void DEBUG_show_min_max_pos_(axis_t axis, vec_t *inst, int minIdx, int maxIdx, int color)
{
    IS_NOT_DEBUG_FLAG return;
    float minPos, maxPos;
    vec_t p0, p1;

    if (axis == ENUM_HOR_X) {
        minPos = inst[minIdx].x;
        maxPos = inst[maxIdx].x;
        p0.x = minPos;
        p0.y = inst[minIdx].y - 5.0f;
        p1.x = minPos;
        p1.y = inst[minIdx].y + 5.0f;
        DEBUG_SHOW_LINE_POS(&p0, &p1, color);
        p0.x = maxPos;
        p0.y = inst[maxIdx].y - 5.0f;
        p1.x = maxPos;
        p1.y = inst[maxIdx].y + 5.0f;
        DEBUG_SHOW_LINE_POS(&p0, &p1, color);
    }
    else {
        minPos = inst[minIdx].y;
        maxPos = inst[maxIdx].y;
        p0.y = minPos;
        p0.x = inst[minIdx].x - 5.0f;
        p1.y = minPos;
        p1.x = inst[minIdx].x + 5.0f;
        DEBUG_SHOW_LINE_POS(&p0, &p1, color);
        p0.y = maxPos;
        p0.x = inst[maxIdx].x - 5.0f;
        p1.y = maxPos;
        p1.x = inst[maxIdx].x + 5.0f;
        DEBUG_SHOW_LINE_POS(&p0, &p1, color);
    }
}

void DEBUG_show_line_belonged_(axis_t axis, uint8_t *inst, uint8_t len, int color)
{
    IS_NOT_DEBUG_FLAG return;
    int j, pd, led;
    if (len <= 0) return;

    for (j = 0; j < len; j++) {
        int lineIdx = inst[j];
        if (BS_getSenInBuf(axis, lineIdx, &pd, &led) < 0) {
            TRACE_ERROR("ERROR! DEBUG_show_line_belonged_..BS_getSenInBuf() [%d]", BG_frame_no);
            TRACE_NOP;
        }
        else {
            DEBUG_SHOW_LINE_PD_LED(axis, pd, led, color);
        }
    }
}

void DEBUG_show_BS_initial_polygon_(void)
{
    IS_NOT_DEBUG_FLAG return;
    int i;
    for (i = 0; i < BS_initial_polygon_cnt; i++) {
        DEBUG_SHOW_MIN_MAX(&BS_initial_polygon[i].pos, 3, 1);
    }
}

void DEBUG_show_BS_subject_polygon_(int color)
{
    IS_NOT_DEBUG_FLAG return;
    int subjIdx;
    poly_e_t subjecte;

    for (subjIdx = 0; subjIdx < BS_pclipping_info.subject_cnt; subjIdx++)
    {
        subjecte.len = BS_pclipping_info.subject_length[subjIdx];
        subjecte.ve = &(macT2P(BS_pclipping_info.subject_vertex, subjIdx, 0, BS_pclipping_info.subject_packs));
        SEND_POLY(&subjecte, 0x11, (uint8_t)color);
    }
    TRACE_NOP;
}

void DEBUG_show_BS_polygon_(int color)
{
    IS_NOT_DEBUG_FLAG return;
    int polyIdx;
    poly_e_t polygone;

    for (polyIdx = 0; polyIdx < BS_pclipping_info.polygon_cnt; polyIdx++)
    {
        polygone.len = BS_pclipping_info.polygon_length[polyIdx];
        polygone.ve = &(macT2P(BS_pclipping_info.polygon_vertex, polyIdx, 0, BS_pclipping_info.polygon_packs));
        SEND_POLY(&polygone, 0x11, (uint8_t)color);
    }
    TRACE_NOP;
}

#if 0 //for testing
void DEBUG_show_BS_touch_polygon_(int color)
{
    IS_NOT_DEBUG_FLAG return;
    int polyIdx;
    poly_e_t polygone;

    for (polyIdx = 0; polyIdx < BS_touch_info_multi_cnt; polyIdx++) {
        polygone.len = DEBUG_touch_polygon_len[polyIdx];
        polygone.ve = &(macT2P(DEBUG_touch_polygon, polyIdx, 0, MAX_SAVE_POLY_PACK_NO));
        //IS_DEBUG_FLAG
        SEND_POLY(&polygone, 0x11, color);
    }
    TRACE_NOP;
}
#endif

void DEBUG_show_BS_touch_polygon_(int maxCnt, int color)
{
    IS_NOT_DEBUG_FLAG return;
    int subjIdx;
    poly_e_t subjecte;

    for (subjIdx = 0; subjIdx < maxCnt; subjIdx++)
    {
        subjecte.len = DEBUG_touch_polygon_len[subjIdx];
        subjecte.ve = &(macT2P(DEBUG_touch_polygon, subjIdx, 0, MAX_SAVE_POLY_PACK_NO));
        SEND_POLY(&subjecte, 0x11, (uint8_t)color);
		TRACE_NOP;
	}
    }

void DEBUG_show_BS_touch_polygon_one_(int subjIdx, int color)
{
	IS_NOT_DEBUG_FLAG return;
	poly_e_t subjecte;
	subjecte.len = DEBUG_touch_polygon_len[subjIdx];
	subjecte.ve = &(macT2P(DEBUG_touch_polygon, subjIdx, 0, MAX_SAVE_POLY_PACK_NO));
    if (color < 0) {
        SEND_POLY(&subjecte, 0x11, (uint8_t)subjIdx);
    }
    else {
        SEND_POLY(&subjecte, 0x11, (uint8_t)color);
    }
    TRACE_NOP;
}

void DEBUG_show_touch_point_(void)
{
    pos_min_max_t minMaxTmp;
    float halfWidthX, halfWidthY;
    int i;
    for (i = 0; i < BG_touch_count; i++) {
        halfWidthX = BG_touch_size[i].xSize * 0.5f;
        halfWidthY = BG_touch_size[i].ySize * 0.5f;
        minMaxTmp.minX = BG_touch_data[i].x - halfWidthX;
        minMaxTmp.maxX = BG_touch_data[i].x + halfWidthX;
        minMaxTmp.minY = BG_touch_data[i].y - halfWidthY;
        minMaxTmp.maxY = BG_touch_data[i].y + halfWidthY;
        DEBUG_SHOW_MIN_MAX(&minMaxTmp, MY_COLOR, 1);
    }
}
#endif //DRAW_POLYGON_TEST
////////////////////////////////////////////
/*end of file */
