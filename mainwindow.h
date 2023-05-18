#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <CTouchPanelView.h>
#include <ctrldialog.h>
#include <QAbstractEventDispatcher>
#include <QtDebug>
#include <windows.h>
#include <dbt.h>
#include <QLabel>

#include <QTemporaryFile>
#include <QKeyEvent>
#include <QImage>
#include <QFile>
#include <QDateTime>
#include "WorkThread.h"
#include "backend_draw_thread.h"

extern "C"
{
#include "backend_postp.h" //for test
}

//#define ENABLE_NEW_PD_FORMAT //nsmoon@170215
//#define DLT_ALL_SLOPE_FORMAT //nsmoon@170828a
//#define FRONTEND_AAREA

//#define DEBUG_WRITE_INPUT_BUFF //nsmoon@200629

#ifndef BACKEND_H_
#ifdef FRONTEND_LINE_THRESHOLD
#define LINE_THRESHOLD_SIZE		(1)
#define LINE_THRESHOLD_VALUE	85 //90 //50 //(60) //(100) //(95)//(90)
#endif
#endif
#if 1 //nsmoon@210430
#define MAX_REMAINED_FRAME_CNT   (30)
#define MAX_REMAINED_LINE_FRAME  (10)
#endif
#if 1 //nsmoon@191205
//#define USE_WIN_USB
#define USE_USB_LIB
#endif

#if defined(USE_WIN_USB)
#include <Winusb.h>
#include <setupapi.h>
#elif defined(USE_USB_LIB)
#include "hidapi.h"
#endif

extern "C"
{
#if defined(_WIN32) || defined(WIN32)
#include "backend.h"
#else
#include "front_if.h"
#endif
}

#define PRG_NAME         "Backend_CTSK_Qt0650_test"

#if defined(USE_WIN_USB)
#define VID_HEX  0x04D8
#define PID_HEX  0x0053 //0x0320
#elif defined(USE_USB_LIB)
#define VID_HEX  0x1FC9
#if (MODEL_SPT == MODEL_CTKS_750_V130)
#define PID_HEX  0x0751
#elif (MODEL_SPT == MODEL_CTKS_750_V140) //nsmoon@200118
#define PID_HEX  0x0753 //0x0751
//#define PID_HEX  0x0761
#elif (MODEL_SPT == MODEL_CTSO_430_V100) //nsmoon@210222
#define PID_HEX  0x0752
#elif (MODEL_SPT == MODEL_CTSK_850_V100) //nsmoon@210510
#define PID_HEX  0x0760 //0x0750
#elif (MODEL_SPT == MODEL_CTSK_650_V100)
#define PID_HEX  0x0753
#else
#error "not define PID_HEX"
#endif
#define INTERFACE_NUMBER        4 //5 //diagnostic //4:composite
#define REPORT_ID               6 //diagnostic
#endif

#define USB_CMD_OFST_DATA		(0x80)
#define USB_CMD_LINE_DATA		(0x81)
#define USB_CMD_STOP_DATA		(0x82)
#ifdef FRONTEND_LINE_THRESHOLD
#define USB_CMD_LINE_THRESHOLD	(0x84) //FIXME
#endif

#define USB_OUT_BUF_LEN         (64*2)
#define USB_OUT_DLT_CMD_SIZE	(10) //without 16bit crc

#if defined(USE_WIN_USB)
#define USB_PACKET_SIZE         512 //64=>512
#define USB_RETRY_COUNT         50  //0x32
#elif defined(USE_USB_LIB)
#define USB_PACKET_SIZE         64
#define USB_RETRY_COUNT         100  //0x32
#if (_DEBUG == 1)
#define USB_READ_TIMEOUT        50
#else
#define USB_READ_TIMEOUT        10
#endif
//target mode
#define TARGET_NORMAL           0
#define TARGET_DLT_REQ          1
#define TARGET_DLT_OK           2
//normal command
#define USB_NORCMD_DLT          0x11 //change to DLT
#define USB_NORCMD_NORMAL       0x12 //change to normal
#define USB_NORCMD_DUMMY        0x0
//normal response
#define TARGET_RES_OK           0
#define TARGET_RES_ERR          0xFF
#if 1 //ndef BACKEND_H_
#define MAX_USB_OUT_BUF_SIZE	(USB_PACKET_SIZE+1)
#define USB_CMD_LEN             (10) //without 16bit crc
#endif
#define NOR_RES_SIZE            (5)
#endif //USE_USB_LIB

#if 1 //ndef BACKEND_H_
#define MAX_NUM_PD_X_DLT	    255
#define MAX_NUM_PD_Y_DLT		255
#define MAX_NUM_X_SLOPES		127
#define MAX_NUM_Y_SLOPES		127
#define MAX_X_SLOPE_VAL 		((MAX_NUM_X_SLOPES - 1)/ 2)
#define MAX_Y_SLOPE_VAL 		((MAX_NUM_Y_SLOPES - 1)/ 2)
#define MAX_SLOPE_BYTE_X        ((MAX_NUM_X_SLOPES - 1) / 8 + 1)
#define MAX_SLOPE_BYTE_Y        ((MAX_NUM_Y_SLOPES - 1) / 8 + 1)
#define MAX_LINE_DATA_X			(MAX_NUM_PD_X_DLT * MAX_SLOPE_BYTE_X)
#define MAX_LINE_DATA_Y			(MAX_NUM_PD_Y_DLT * MAX_SLOPE_BYTE_Y)
#define USB_CRC_SIZE			2 //bytes

#if (DRAW_LINE_TEST == 0)
#define X_NUM_PD                MAX_NUM_PD_X_DLT
#define Y_NUM_PD                MAX_NUM_PD_Y_DLT
//#define MAX_X_SLOPE_IDX         ((MAX_X_SLOPE_VAL * 2) + 1)  //81:-40~0~+40
//#define MAX_Y_SLOPE_IDX         ((MAX_Y_SLOPE_VAL * 2) + 1)  //127:-63~0~+63
#endif

#ifdef FRONTEND_LINE_THRESHOLD
#define MAX_HOR_LINE_THRESHOLD	(MAX_LINE_DATA_X * LINE_THRESHOLD_SIZE)
#define MAX_VER_LINE_THRESHOLD	(MAX_LINE_DATA_Y * LINE_THRESHOLD_SIZE)
#define MAX_EXB_SEND_BUF_SIZE   (PD_DATA_IDX2 +  MAX_LINE_DATA_X + MAX_LINE_DATA_Y + MAX_HOR_LINE_THRESHOLD + MAX_VER_LINE_THRESHOLD + USB_CRC_SIZE)
#endif
//#define MAX_EXB_SEND_BUF_SIZE   (4 + (((X_NUM_PD * MAX_SLOPE_BYTE_X) + (Y_NUM_PD * MAX_SLOPE_BYTE_Y))*2) + (4*8)) //(4*8):active area
//#define MAX_EXB_SEND_BUF_SIZE   (4 + (X_NUM_PD * MAX_SLOPE_BYTE_X) + (Y_NUM_PD * MAX_SLOPE_BYTE_Y) + (4*8)) //(4*8):active area
#endif

#ifdef BACKEND_H_
#define MAX_HOR_TOUCH_PD_WIN32  (MAX_NUM_PD_X_DLT * MAX_X_SLOPE_VAL)
#define MAX_VER_TOUCH_PD_WIN32  (MAX_NUM_PD_Y_DLT * MAX_Y_SLOPE_VAL)
#else
#define MAX_HOR_TOUCH_PD_WIN32  (MAX_NUM_PD_X_DLT * MAX_NUM_X_SLOPES)
#define MAX_VER_TOUCH_PD_WIN32  (MAX_NUM_PD_Y_DLT * MAX_NUM_Y_SLOPES)
#endif

#define NUM_OF_PACKET			(((MAX_EXB_SEND_BUF_SIZE + (USB_PACKET_SIZE - 1)) / USB_PACKET_SIZE) + 1)
#define USB_WAIT_TIME			-1
#define USB_PROCESS_BUFF		(NUM_OF_PACKET * USB_PACKET_SIZE * 2)

#define PD_OFST_HEADER  0xA0   //nsmoon@170527a
#define PD_DATA_HEADER  0xA5   //nsmoon@170329a bf => a5
#define PD_DATA_IDX  8  //PD-data start, 0~7  header

#if 1 //def DLT_ALL_SLOPE_FORMAT //nsmoon@170828a
#define PD_DATA_IDX2 4 //PD-data start
#else
#define PD_DATA_IDX2 12 //PD-data start, 8~11 pdX, pdY len
#endif

#define PD_IDX_NUM_OF_PD_X  4
#define PD_IDX_NUM_OF_PD_Y  6
#define PD_IDX_SCAN_MODE    8  //nsmoon@170527
#define PD_IDX_OFST_LEN_X   9
#define PD_IDX_OFST_LEN_Y   10
#define PD_IDX_VAR_HD		11 //idx of variable header

#if (DEAD_CELL_TBL == 1) //nsmoon@190926
#define PD_IDX_DCELL_LEN_X_LED  11
#define PD_IDX_DCELL_LEN_X_PD   12
#define PD_IDX_DCELL_LEN_Y_LED  13
#define PD_IDX_DCELL_LEN_Y_PD   14
#if 1 //(SECOND_OFST_TBL == 1) //nsmoon@190624
#define PD_IDX_OFST_LEN_X2      15
#define PD_IDX_OFST_LEN_Y2      16
#define PD_IDX_SEN_LEN_X2       17
#define PD_IDX_SEN_LEN_Y2       18
#define PD_IDX_POS_START        19 //position data
#else
#define PD_IDX_POS_START        15 //position data
#endif
#else
#define PD_IDX_OFST_X       11 //offset data
#endif //DEAD_CELL_TBL

#if 1 //def ENABLE_NEW_PD_FORMAT
#define PD_DATA_SIZE 2 //PD-data size, 2-byte
#else
#define PD_DATA_SIZE 3 //PD-data size, 3-byte
//#define PD_DATA_SIZE 4 //PD-data size, 4-byte
#endif

#define PD_DATA_MSB     18 //PD-data max, bit18~0
#define PD_DATA_CTB     9  //PD-data center, bit8 is center, offset 0
#define PD_DATA_BITMASK 0x7ffff //bit18~0

#ifdef MY_SERIAL_TEST
#define MAX_BUFFER SERIAL_MAXBUF_SIZE
#else
#define MAX_BUFFER (32*1024)
#endif

#define MAX_SHIFT_TBL_BYTE  8
#define MAX_SHIFT_TBL       64

#define GROUP_SIZE          10
#define SUBGROUP_SIZE       5
#define MARGIN_LEFT         30 //20
#define MARGIN_TOP          30 //20

#define BGR_2_QRGB(b, g, r) qRgb((r),(g),(b))
#define VID_PID_LEN         10

#ifndef BACKEND_H_
#define TEST_LOGICAL_X_MAX  19354 //4096
#define TEST_LOGICAL_Y_MAX  10886 //2304
#endif

#define MAX_USB_IN_BUFF	1

#ifndef MY_COLOR
#define MY_COLOR    30 //13
#endif

#define MAX_NUM_2ND_SENSOR_X	20
#define MAX_NUM_2ND_SENSOR_Y	20

#define MAX_BG_DEBUG_MISC       (ALLOWABLE_TOUCH_BE * 2) //20

void makeLogFile();
void destoryLogFile();

typedef struct {
    //float x, y;
    uint16_t x, y;
    uint16_t xSize, ySize;
    uint8_t id, stat;
} touch_point_t;

#if 1 //def DLT_ALL_SLOPE_FORMAT //nsmoon@170828a
struct disp_param {
    int nWidth; //hor disp size
    int nHeight; //ver disp size
    float ratioX, ratioY;
    int nMaxPdX; //X-axis PD-num
    int nMaxPdY; //Y-axis PD-num
};
#else
struct disp_param {
    int nWidth; //hor disp size
    int nHeight; //ver disp size
    float ratioX, ratioY;
    int nMaxPdX; //X-axis PD-num
    int nMaxPdY; //Y-axis PD-num
    int nDx, nDy;
    int x1, x2, y1, y2;
};
#endif

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//void set_debug_func(DEF_MIN_MAX *before, DEF_MIN_MAX *after);

#ifdef __cplusplus
}
#endif

typedef enum {
    TOUCH_DISP_COLOR_ID = 0,
    TOUCH_DISP_COLOR_SIZE = 1,
    TOUCH_DISP_COLOR_END = 2
} touch_disp_color_t;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static MainWindow *mainWindow;
    static MainWindow *instance() {
        if (mainWindow == NULL)
            mainWindow = new MainWindow;
        return mainWindow;
    }

    //////////////////////////////////////////
    void InitLine();
    void InitMarkLines();

    char* GetConfigData(int* nLen, int nType);
    int GetTouchData(char* pBuf, int nLen);
    //static void Process(const void* obj);
    void OpenNewFileName();
    void DataProcess();
    void set_debug_func(DEF_MIN_MAX *before, DEF_MIN_MAX *after);

    Backend_Draw_Thread *m_Backend_Draw_Thread;
    Backend_Wait_Thread *m_Backend_Wait_Thread;

public slots:
    bool usb_check_n_open(void);
    bool USB_open(void);
    bool USB_close(void);
    bool drawOutPoly(void);
    bool threadSlotDrawOutPoly(float x0, float y0, float x1, float y1, unsigned long color__, int wakeStatus);
    bool threadSlotDrawOutOnePoly(float x0, float y0, float x1, float y1, unsigned long color__, int wakeStatus);
    void drawTouchPoint(touch_point_t *tp);
#if defined(USE_WIN_USB) //nsmoon@190124
    bool CheckIfPresentAndGetUSBDevicePath(void);
#elif defined(USE_USB_LIB)
    int USB_enumerate(void);
#endif

signals:
    bool CallUsbOpen(void);
    bool CallUsbClose(void);
    bool CallDrawOutPoly(void);
    void CallDrawTouchPoint(touch_point_t *tp);
    //void call_draw_line(float x0, float y0, float x1, float y1, unsigned long color);

protected:
    void closeEvent(QCloseEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;

    static void hidThrdUsb(const void* obj);
    static bool bHidConnected;
    static int num_of_dlt_pkt;
    static bool bPause;
    static bool bFirstStart;
    static bool bOneStep;
    static bool bRequest;
    static bool bInitOfst;
    static bool bChangeThreshold;

#if defined(USE_WIN_USB) //nsmoon@190124
    PSP_DEVICE_INTERFACE_DETAIL_DATA DetailedInterfaceDataStructure = new SP_DEVICE_INTERFACE_DETAIL_DATA;	//Global
    HANDLE MyDeviceHandle;
    WINUSB_INTERFACE_HANDLE MyWinUSBInterfaceHandle;
#elif defined(USE_USB_LIB)
    hid_device *hUsb_hid_dev;  //USB HID device
    static int nDltNormal;
    int HID_RecvNorRes(void);
    unsigned char aUsbInTmp[USB_PACKET_SIZE];
    char usbPath[MAX_PATH];
#endif

    GUID genericUSBGUID;

    static int nBufNumber; //pingpong buuer bumber
    static bool bWriteFile, bWriteFile2;
    static bool bFromFile;
    static bool bDrawOutPoly;
    static bool bDrawTouchPoint;
    static bool AttachedState;
    static unsigned long usb_time_out;
    //static unsigned char aUsbIn[MAX_USB_IN_BUFF][USB_PROCESS_BUFF];
    static unsigned char *aUsbIn[MAX_USB_IN_BUFF];
    static unsigned char *HidReceiveBuf;
    static int HidReceiveLen;
    static int readFileSize;

    unsigned char aUsbOut[USB_OUT_BUF_LEN];
    int slope_byte_x, slope_byte_y;
    uint8_t maxSlopeX, maxSlopeY;
    uint8_t maxSlopeX2, maxSlopeY2;
    int offsetTblLenX, offsetTblLenY;
#if 0 //nsmoon@201012
    int8_t offset_table_x[MAX_NUM_SLOPES_DLT_X];
    int8_t offset_table_y[MAX_NUM_SLOPES_DLT_Y];
#endif
#if (DEAD_CELL_TBL == 1) //nsmoon@190829
    DEF_DEADCELL_INFO testDeadCell; //nsmoon@190829
    uint8_t deadCellHorLed[MAX_NUM_PD_X_DLT];
    uint8_t deadCellHorPd[MAX_NUM_PD_X_DLT];
    uint8_t deadCellVerLed[MAX_NUM_PD_Y_DLT];
    uint8_t deadCellVerPd[MAX_NUM_PD_Y_DLT];
#endif
#if (SECOND_OFST_TBL == 1) //nsmoon@190624
    //int8_t offset_table_x2[MAX_NUM_SLOPES_DLT_X];
    //int8_t offset_table_y2[MAX_NUM_SLOPES_DLT_Y];
    uint8_t sensor_table_x2[MAX_NUM_2ND_SENSOR_X];
    uint8_t sensor_table_y2[MAX_NUM_2ND_SENSOR_Y];
#endif

    DEF_PD_INFO testInputBuffer;
    DEF_DATA_INFO testOuputBuffer;
    DEF_DATA_INFO2 testOuputBuffer2;
    DEF_PD ver_touch_pd_win32[MAX_VER_TOUCH_PD_WIN32];
    DEF_PD hor_touch_pd_win32[MAX_HOR_TOUCH_PD_WIN32];
#ifdef FRONTEND_LINE_THRESHOLD
    uint8_t hor_line_threshold_win32[MAX_HOR_TOUCH_PD_WIN32];
    uint8_t ver_line_threshold_win32[MAX_VER_TOUCH_PD_WIN32];
#endif
    next_scan_t   testNextScan;
    uint64_t shiftTbl64[MAX_SHIFT_TBL];
    disp_param disp;
    const char *vidText; //[VID_PID_LEN];
    const char *pidText; //[VID_PID_LEN];
    unsigned short VendorID;
    unsigned short ProductID;
    int interfaceNum, reportId;
    bool bMsg;
    bool bDispInverse;
    bool bRecord;
    touch_point_t touchPoint[ALLOWABLE_TOUCH_BE*2];
    touch_point_t touchPointPre[ALLOWABLE_TOUCH_BE*2];
    int touchPointCnt, touchPointPreCnt;

    const uint16_t crc_table[16] =
    {
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
        0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef
    };

    const uint8_t bit_mask_8[MAX_SHIFT_TBL] =
    {
        0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
    };

    DWORD USB_SendRequest(unsigned char Command);
    DWORD USB_readData(unsigned char *InPtr, int *nResult, int mode);
    uint32_t USB_CalculateCrc(uint8_t *data, uint32_t len);
    int check_header_line(unsigned char *InPtr, int *totalPacketSize, int *nPacketStart);
    int check_header_ofst(unsigned char *InPtr, int *totalPacketSize, int *nPacketStart);
    void get_slope_size(void);
    void get_slope_val(axis_t axis, int stIdx, int pd, unsigned char *pRcv, slope_mask_t *slopeVal);
    void initShiftTbl64(void);
    void get_slope_mask(axis_t axis, uint16_t slope_idx, slope_mask_t *sm);
    void hidThrdUsb_init(void);
    void set_ofst_data(int bufIdx);
    int backend_process_line_data(int bufIdx);
    bool nativeEvent(const QByteArray & eventType, void * message, long * result);
    void setScreenSize(void);
    void initDispParm(void);
    bool draw_pd_data(int bufIdx, BOOLEAN bDraw);
    int drawPdData(axis_t axis, int pd, int offSetIdx, int pdIdx);
    void draw_line(float x0, float y0, float x1, float y1, unsigned long color);
    void remove_draw_line(void);
    bool draw_axis(void);
    float posX(float pos);
    float posY(float pos);
    int RestoreDisplayPos(int x, int y, vec_t *pos);
    QColor GetDisplayColor(unsigned long color);
    void allow_system_sleep_on(bool allow);
    void savedFileOpen(void);
    void resetTouchStatus(void);
    int makeInputBuffer(axis_t axis, int pd, int offSetIdx, int pdIdx);
#if 0 //(SECOND_OFST_TBL == 1) //nsmoon@190625
int conv_pd_to_byteidx(axis_t axis, uint8_t *buff, in_line_t *line, in_dlt_buf_idx_t *bufIdx);
int is_blocked_line_in_buff(axis_t axis, uint8_t *buff, int pd, int led);
int set_blocked_line_in_buff(axis_t axis, uint8_t *buff, int pd, int led);
int ofst2_sensor_deadline(axis_t axis, uint8_t *buff);
#endif
#if (DEAD_CELL_TBL == 1) //nsmoon@190829
    bool is_dead_cell(uint8_t *tbl, int len, int pd);
#endif

#if 1 //for debugging
    void showDebugLabel(touch_point_t *tp);
    QLabel *debugLabel[MAX_BG_DEBUG_MISC];
    //int debugSortIdx[MAX_BUFFER];
#endif
#if 1 //nsmoon@190320
    QString textTouch;
    void showTouchLabel(void);
    QLabel *touchLabel[MAX_BG_DEBUG_MISC];
    QLabel *recordLabel;
#endif

    int lineThresholdValue;
    touch_disp_color_t touchDisplayColor;

    ////////////////////////////////////////
    QList<QGraphicsLineItem*> lineLst;
    QList<QGraphicsLineItem*> touchLst;

#if 1 //nsmoon@180711
    void DebugProcess();
    void showDebug(void);
    void showSensors(void);
    int debugLineCnt;
    int eTypeDebug;
#endif

    int nLineCount;
    int nLineWidth;
    int nOAWidth;
    int nOAHeight;

    bool bShowEnable;
    bool bShowFrameNo;
    int  nShowReverse;
    int  nShowRemained;
    int  nShowEnableVertical;

    uint32_t rmFrameCnt; //remained line //nsmoon@210430

#ifdef TEMP_FILE_WRITE
    QTemporaryFile m_soureFile;
#endif
    QString m_CreateFilePath;
    QString m_ReadFilePath;
    QFile* m_file;

    QFile* m_NewFile;
    void newFileWrite(char const *format, ...);

#ifdef DEBUG_WRITE_INPUT_BUFF //nsmoon@200629
    void debugWriteInputBuff(DEF_PD_INFO *inBuf);
    void debugPrintInputBuff(void);
    int debugReadInputBuff(DEF_PD_INFO *inBuf);
#endif

    const QColor colorQt[MY_COLOR + 1] = {
        BGR_2_QRGB(0, 0, 0),		//0 //black
        BGR_2_QRGB(255, 0, 0),		//1 //blue
        BGR_2_QRGB(255, 0, 255),	//2 //magenta
        BGR_2_QRGB(0, 0, 128),		//3 //maroon
        BGR_2_QRGB(0, 128, 128),	//4 //oliver
        BGR_2_QRGB(128, 0, 128),	//5 //purple
        BGR_2_QRGB(128, 128, 0),	//6 //teal
        BGR_2_QRGB(192, 192, 192),	//7 //
        BGR_2_QRGB(0, 255, 0),		//8 //lime
        BGR_2_QRGB(0, 128, 0),		//9 //green
        BGR_2_QRGB(0, 0, 139),		//10//dark red
        BGR_2_QRGB(42, 42, 165),	//11//brown
        BGR_2_QRGB(0, 100, 0),		//12//dark green
        BGR_2_QRGB(32, 165, 32),	//13//golden rod
        BGR_2_QRGB(0, 140, 255),	//14//dark orange
        BGR_2_QRGB(139, 139, 205),	//15//dark cyan
        BGR_2_QRGB(87, 139, 46),	//16//sea green
        BGR_2_QRGB(114, 128, 250),	//17//salmon
        BGR_2_QRGB(255, 191, 0),	//18//deep sky blue
        BGR_2_QRGB(205, 90, 106),	//19//slate blue
        BGR_2_QRGB(45, 82, 160),	//20//sienna
        BGR_2_QRGB(143, 143, 188),	//21//rosy brown
        BGR_2_QRGB(135, 184, 222),	//22//burly wood
        BGR_2_QRGB(208, 224, 64),	//23//turquoise
        BGR_2_QRGB(34, 139, 34),	//24//forest green
        BGR_2_QRGB(128, 128, 240),	//25//light coral

        BGR_2_QRGB(128, 128, 128),	//26 //gray
        BGR_2_QRGB(0, 255, 255),	//27 //yellowa
        BGR_2_QRGB(255, 255, 0),	//28 //cyan
        BGR_2_QRGB(0, 255, 0),      //29 //lime
        BGR_2_QRGB(0, 0, 255)       //30 //red
    };

    QImage* m_pBackGround;
    WorkThread* pWorkThreak;
    CTouchPanelView* view;
    QDateTime dateTime[2];
    int dateTimeCnt;

};

#endif // MAINWINDOW_H
