/*
 * mainwindow.cpp
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QGraphicsView>
#include <QApplication>
#include <QDesktopWidget>
#include <QGraphicsLineItem>
#include <QFileDialog>
#include <synchapi.h>
#include <QIODevice>
#include <QPolygonF>
//#include <QDebug>
#include <QSettings>
#include <QMessageBox>
#include "backend_draw_thread.h"

extern "C"
{
#include "backend.h"
#include "backend_postp.h"

typedef struct {
    vec_t vert;
    uint16_t edgeId;
} vec_e_t, *vec_e;

typedef struct {
    int len, alloc;
    vec_e ve;
} poly_e_t, *poly_e;
extern int _myDrawPolygon(poly_e res, uint8_t flag, uint8_t color);
}

DEF_MIN_MAX min_max_before;
DEF_MIN_MAX min_max_after;

///////////////////////////////////////////////////////
/// static variables for hidThrdUsb()
bool MainWindow::bHidConnected;
int MainWindow::num_of_dlt_pkt;
bool MainWindow::bPause;
bool MainWindow::bFirstStart;
bool MainWindow::bOneStep;
bool MainWindow::bRequest;
bool MainWindow::bInitOfst;
bool MainWindow::bChangeThreshold;
int MainWindow::nBufNumber; //pingpong buuer bumber
bool MainWindow::bWriteFile;
bool MainWindow::bWriteFile2;
bool MainWindow::bFromFile;
int MainWindow::readFileSize;
bool MainWindow::bDrawOutPoly;
bool MainWindow::bDrawTouchPoint;
bool MainWindow::AttachedState;
unsigned long MainWindow::usb_time_out;
//unsigned char MainWindow::aUsbIn[MAX_USB_IN_BUFF][USB_PROCESS_BUFF];
unsigned char *MainWindow::aUsbIn[MAX_USB_IN_BUFF];
unsigned char *MainWindow::HidReceiveBuf;
int MainWindow::HidReceiveLen;
#if defined(USE_USB_LIB)
int MainWindow::nDltNormal;
#endif

QFile *logFile;
QTextStream *logOut;
///////////////////////////////////////////////////////

//#define DEBUG_WRITE_INPUT_BUFF_GNERATE
#ifdef DEBUG_WRITE_INPUT_BUFF //nsmoon@200629
#define DEBUG_FRAME_COUNT       100
#define DEBUG_MAX_TEST_DATA     300
#define DEBUG_FRAME_NO_START    281
#define DEBUG_FRAME_NO_END      (DEBUG_FRAME_NO_START + DEBUG_FRAME_COUNT)
int debug_frame_no;
#ifdef DEBUG_WRITE_INPUT_BUFF_GNERATE //for generating test data
#define DEBUG_MAX_THRESHOLD_X   (MODEL_SPT_X_CELL * ((MODEL_SPT_X_SLOPE * 2 + 1 + 7) / 8))
#define DEBUG_MAX_THRESHOLD_Y   (MODEL_SPT_Y_CELL * ((MODEL_SPT_Y_SLOPE * 2 + 1 + 7) / 8))
DEF_PD_INFO s_debug_inBuf[DEBUG_FRAME_COUNT];
DEF_PD	debug_ver_touch_pd[DEBUG_FRAME_COUNT][DEBUG_MAX_TEST_DATA];
DEF_PD	debug_hor_touch_pd[DEBUG_FRAME_COUNT][DEBUG_MAX_TEST_DATA];
uint8_t debug_threshold_x[DEBUG_FRAME_COUNT][DEBUG_MAX_THRESHOLD_X];
uint8_t debug_threshold_y[DEBUG_FRAME_COUNT][DEBUG_MAX_THRESHOLD_Y];
#else
#include "test_data.h"
#endif
#endif
///////////////////////////////////////////////////////

MainWindow *MainWindow::mainWindow = NULL;

#define OFST_DATA_SIZE     4096
static char ofstData[OFST_DATA_SIZE];
static int ofstDataCnt;

#if 1 //nsmoon@180227
#define DEBUG_BUFF_SIZE     1024 //(4096*2)
static char dbgStr[DEBUG_BUFF_SIZE];
//unsigned long traceNopCnt;

#define MAX_LAST_ERROR_MSG	2048
static char lastErrorMsg[MAX_LAST_ERROR_MSG];
//static wchar_t lastErrorMsgW[MAX_LAST_ERROR_MSG];
const char *myGetLastErrorString(void)
{
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&lastErrorMsg[0], MAX_LAST_ERROR_MSG, NULL);
    return lastErrorMsg;
}
const char *myGetLastErrorString2(DWORD error)
{
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&lastErrorMsg[0], MAX_LAST_ERROR_MSG, NULL);
    return lastErrorMsg;
}

void _myTrace(char const *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    //wvsprintf(dbgStr, format, argptr);
    vsnprintf(dbgStr, sizeof(dbgStr), format, argptr);
    va_end(argptr);
    //OutputDebugString(dbgStr);
    qDebug(dbgStr);
}

void _myTraceNR(char const *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    //wvsprintf(dbgStr, format, argptr);
    vsnprintf(dbgStr, sizeof(dbgStr), format, argptr);
    va_end(argptr);
    //OutputDebugString(dbgStr);
    qDebug(dbgStr);
    //puts(dbgStr);
}

void makeLogFile()
{
    /*QString filename = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmsszzz") + ".txt";
    logFile = new QFile(filename);
    if(!logFile->open(QIODevice::Append | QIODevice::Text)) {
        return;
    }
    logOut = new QTextStream(logFile);*/
}

void destoryLogFile()
{
    //logFile->close();
}


void _mySaveLog(char const *format, ...)
{
    /*va_list argptr;
    va_start(argptr, format);
    vsnprintf(dbgStr, sizeof(dbgStr), format, argptr);
    va_end(argptr);

    QString curtime = QDateTime::currentDateTime().toString("yyyy MM dd hh mm ss zzz");
                        //context.file, context.line, context.function

    *logOut << curtime << " : " << dbgStr << "\n";*/
}

void debug_func(DEF_MIN_MAX *before, DEF_MIN_MAX *after)
{
    MainWindow::mainWindow->set_debug_func(before, after);
}

void BG_set_debug_func(void (*fuction)(void));

#endif //nsmoon@180227

#define LABEL_TP_TOP  30
#define LABEL_TP_LFT  30
#define LABEL_DBG_TOP  600
#define LABEL_DBG_LFT  50
#define LABEL_RAW_GAP  20
#define LABEL_TP_WIDTH   500
#define LABEL_DBG_WIDTH  1000

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
#ifdef SPEED_TEST_FLOAT
    executeSpeedTest_init();
    while(1) {
        executeSpeedTest();
    }
#endif
#if 0 //for test //nsmoon@210430
    TRACE("sizeof(next_scan_t)= %d", sizeof(next_scan_t));
    TRACE("sizeof(testNextScan)= %d", sizeof(testNextScan));
    TRACE("sizeof(testNextScan.currScan)= %d", sizeof(testNextScan.currScan));
    TRACE("sizeof(testNextScan.nextScan)= %d", sizeof(testNextScan.nextScan));
    TRACE("sizeof(testNextScan.brushMode)= %d", sizeof(testNextScan.brushMode));
    TRACE("sizeof(testNextScan.numTouch)= %d", sizeof(testNextScan.numTouch));
    TRACE("sizeof(testNextScan.x1)= %d", sizeof(testNextScan.x1));
#endif
    char ini_file_path[MAX_PATH];
    snprintf(ini_file_path, MAX_PATH, "%s.ini", PRG_NAME);
    TRACE_RELEASE("ini_file_path=(%s)", ini_file_path);
    QString usbKey("USB/");
    QSettings *settings = 0;
    settings = new QSettings( ini_file_path, QSettings::IniFormat );
    QString vidStr = settings->value( usbKey + "VID", "r").toString();
    interfaceNum = settings->value( usbKey + "INTERFACE", "r").toInt();
    if (interfaceNum == 0) {
        interfaceNum = INTERFACE_NUMBER;
    }
    reportId = settings->value( usbKey + "REPORTID", "r").toInt();
    vidText = vidStr.toStdString().c_str();
    bool bStatus = false;
    VendorID = vidStr.toUInt(&bStatus,16);
    TRACE_RELEASE("vid= 0x%x %d %d", VendorID, interfaceNum, reportId);

    if (!VendorID) VendorID = 0x1FC9;
    ui->setupUi(this);
    //QLocale::setDefault(QLocale::English);

    bHidConnected = false;
    bPause = false;
    bFirstStart = true;
    bOneStep = true;
    bRequest = true;
    bInitOfst = false;
    bChangeThreshold = false;
    bWriteFile = false;
    num_of_dlt_pkt = 0;
    //VendorID = VID_HEX;
    //ProductID = PID_HEX;
    bMsg = false;
    bDispInverse = false;
    bDrawOutPoly = false;
    bDrawTouchPoint = false;
    bFromFile = false;
    dateTimeCnt = -1; //init

    AttachedState = false;
    touchDisplayColor = TOUCH_DISP_COLOR_SIZE;
    lineThresholdValue = LINE_THRESHOLD_VALUE;
    rmFrameCnt = 0; //remained line //nsmoon@210430
#if defined(USE_WIN_USB)
    MyDeviceHandle = INVALID_HANDLE_VALUE;    
#elif defined(USE_USB_LIB)
    hUsb_hid_dev = NULL;  //USB HID device
    nDltNormal = TARGET_NORMAL;
#endif

    resetTouchStatus(); //nsmoon@220125

    scene = new QGraphicsScene(this);

    this->showFullScreen();
#if 1 //(QT_NO_DEBUG == 1) //nsmoon@180512
    if (QApplication::desktop()->screenCount() > 1) {
        QRect scr2ndRect = QApplication::desktop()->screenGeometry(0);
        this->setGeometry(scr2ndRect);
        scene->setSceneRect(0, 0, scr2ndRect.width(), scr2ndRect.height());
    }
    else {
        scene->setSceneRect(0, 0, QApplication::desktop()->width(), QApplication::desktop()->height());
    }
    view = new CTouchPanelView();
    this->setCentralWidget(view);
    view->setScene(scene);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    qDebug("desktop()->width() = %d, desktop()->height() = %d", QApplication::desktop()->width(), QApplication::desktop()->height());
    qDebug("this->scene->width() = %0.2f, this->scene->height() = %0.2f", this->scene->width(), this->scene->height());
#else
    view = new CTouchPanelView();
    this->setCentralWidget(view);
    view->setScene(&scene);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
#endif

    setScreenSize();
#if 0 //def TEMP_FILE_WRITE
    m_soureFile.open();
#endif
    allow_system_sleep_on(false);

#if 1 //for debugging
    int i;
    int label_dbg_top_pos = this->scene->height() / 3;
    for (i = 0; i < MAX_BG_DEBUG_MISC; i++) {
        debugLabel[i] = new QLabel(this);
        debugLabel[i]->setStyleSheet("QLabel { color : blue; }");
        debugLabel[i]->move(LABEL_DBG_LFT, /*LABEL_DBG_TOP*/label_dbg_top_pos+(i*LABEL_RAW_GAP));
        debugLabel[i]->setFixedWidth(LABEL_DBG_WIDTH);
        debugLabel[i]->setFont(QFont("SansSerif", 18));//12));
        debugLabel[i]->show();
    }
#endif
#if 1 //for debugging
    for (int i = 0; i < MAX_BG_DEBUG_MISC; i++) {
        touchLabel[i] = new QLabel(this);
        touchLabel[i]->setStyleSheet("QLabel { color : blue; }");
        //touchLabel[i]->move(0, 0+(i*20));
        touchLabel[i]->move(LABEL_TP_LFT+10, LABEL_TP_TOP+(i*LABEL_RAW_GAP));
        touchLabel[i]->setFixedWidth(LABEL_TP_WIDTH);
        touchLabel[i]->setFont(QFont("SansSerif", 18));//10));
        touchLabel[i]->show();
    }
#endif
#if 1 //recording
    recordLabel = new QLabel(this);
    recordLabel->setStyleSheet("QLabel { color : black; }");
    recordLabel->move(this->scene->width()-10, 20);
    recordLabel->setFixedWidth(LABEL_TP_WIDTH);
    recordLabel->setFont(QFont("SansSerif", 18));//10));z
    recordLabel->show();
#endif

    connect(this, SIGNAL(CallUsbOpen()), this, SLOT(USB_open()), Qt::QueuedConnection);
    connect(this, SIGNAL(CallUsbClose()), this, SLOT(USB_close()), Qt::QueuedConnection);
#ifndef DRAW_THREAD
    connect(this, SIGNAL(CallDrawOutPoly()), this, SLOT(drawOutPoly()), Qt::QueuedConnection);
#endif
    connect(this, SIGNAL(CallDrawTouchPoint(touch_point_t*)), this, SLOT(drawTouchPoint(touch_point_t*)), Qt::QueuedConnection);

    pWorkThreak = new WorkThread(hidThrdUsb, this);
    pWorkThreak->start(QThread::NormalPriority);

    makeLogFile();

#ifdef DRAW_THREAD
    m_Backend_Draw_Thread = new Backend_Draw_Thread();
    connect(m_Backend_Draw_Thread, SIGNAL(threadSigDrawOutPoly(float, float, float, float, unsigned long, int)), this, SLOT(threadSlotDrawOutPoly(float, float, float, float, unsigned long, int)), Qt::QueuedConnection);
    connect(m_Backend_Draw_Thread, SIGNAL(threadSigDrawOutOnePoly(float, float, float, float, unsigned long, int)), this, SLOT(threadSlotDrawOutOnePoly(float, float, float, float, unsigned long, int)), Qt::QueuedConnection);
    m_Backend_Draw_Thread->startDrawThread();

    m_Backend_Wait_Thread = new Backend_Wait_Thread();
    m_Backend_Wait_Thread->startWaitThread();
#endif
}

void MainWindow::newFileWrite(char const *format, ...)
{
#define TEMP_STR_LEN    128
    char tempStr[TEMP_STR_LEN];
    va_list argptr;
    va_start(argptr, format);
    vsnprintf(tempStr, sizeof(tempStr), format, argptr);
    va_end(argptr);
    //qDebug(tempStr);

    int len = strlen(tempStr);
    if ((len+2) < TEMP_STR_LEN) {
        tempStr[len] = '\n';
        //tempStr[len+1] = '\n';
    }
    m_NewFile->write((const char*)tempStr, (len+1));
}

void MainWindow::allow_system_sleep_on(bool allow)
{
    if (allow) {
        //allow system sleep on
        SetThreadExecutionState(ES_CONTINUOUS); // set state back to normal
    }
    else {
        //not allow system sleep on
        // try this for vista, it will fail on XP
        if (SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED | ES_DISPLAY_REQUIRED) == 0)
        {
            if (SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED) == 0) {
                // try XP variant as well just to make sure
                SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED);
            }
        }  // if
    }
}

void MainWindow::OpenNewFileName()
{
    int i = 0;
    QString str;
    QString filePath = m_CreateFilePath;
    if(m_CreateFilePath != NULL)
    {
        while(1)
        {
            filePath = m_CreateFilePath;
            filePath += "_" + str.number(i, 10) + ".txt";
            m_NewFile = new QFile(filePath);
            if (m_NewFile->exists() == 0)
            {
                m_NewFile->open(QIODevice::WriteOnly | QIODevice::Text);
                break;
            }
            m_NewFile->close();
            i++;
        }
    }
    return;
}

#ifdef LINE_DRAW_TEST //nsmoon@180711
#if (QT_NO_DEBUG == 0) //nsmoon@180512
void MainWindow::showSensors(void)
{
    vec_t *posTop = &BG_pos_top_led[0];
    vec_t *posBot = &BG_pos_bottom_led[0];
    vec_t *posRht = &BG_pos_right_led[0];
    vec_t *posLft = &BG_pos_left_led[0];
    vec_t startLI, endLI;
    QPen pen;
    int i;
    //x-axis
    pen.setColor("blue");
    for (i = 0; i < MAX_NUM_Y_PD; i++) {
        if (i % 5 == 0) {
            pen.setWidth(2);
        }
        else {
            pen.setWidth(1);
        }
        startLI.x = posRht[i].x;
        endLI.x = startLI.x - 3;
        startLI.y = posRht[i].y;
        endLI.y = startLI.y;
        scene->addLine(GetDisplayX(startLI.x), GetDisplayY(startLI.y), GetDisplayX(endLI.x), GetDisplayY(endLI.y), pen);

        startLI.x = posLft[i].x;
        endLI.x = startLI.x + 3;
        startLI.y = posLft[i].y;
        endLI.y = startLI.y;
        scene->addLine(GetDisplayX(startLI.x), GetDisplayY(startLI.y), GetDisplayX(endLI.x), GetDisplayY(endLI.y), pen);
    }

    for (i = 0; i < MAX_NUM_X_PD; i++) {
        if (i % 5 == 0) {
            pen.setWidth(2);
        }
        else {
            pen.setWidth(1);
        }

        startLI.x = posTop[i].x;
        endLI.x = startLI.x;
        startLI.y = posTop[i].y;
        endLI.y = startLI.y - 3;
        scene->addLine(GetDisplayX(startLI.x), GetDisplayY(startLI.y), GetDisplayX(endLI.x), GetDisplayY(endLI.y), pen);

        startLI.x = posBot[i].x;
        endLI.x = startLI.x;
        startLI.y = posBot[i].y;
        endLI.y = startLI.y + 3;
        scene->addLine(GetDisplayX(startLI.x), GetDisplayY(startLI.y), GetDisplayX(endLI.x), GetDisplayY(endLI.y), pen);
    }
#endif
}

#define MAX_ARR_LINE_IDX    100
void MainWindow::showDebug()
{
    stLine tmpPL;
    int ledBot, pdTop, ledLft, pdRht;

#if 0 //for testing
    int dispLine;
    static int dispLineSav = 0;
    int arrLineIdx[MAX_ARR_LINE_IDX] = {0, 3457, 3458, 5908, 5909, 9369, 9370, (nLineCount - 1), -1};
    if (debugLineCnt >= MAX_ARR_LINE_IDX) {
        debugLineCnt = 0;
    }
    else if (arrLineIdx[debugLineCnt] < 0) {
        debugLineCnt = 0;
    }
    dispLine = arrLineIdx[debugLineCnt++];
    tmpPL = pLines[dispLine];
    ledBot = BG_get_idx_pos_arry(ENUM_BOT_LED, &tmpPL);
    pdTop = BG_get_idx_pos_arry(ENUM_TOP_PD, &tmpPL);
    ledLft = BG_get_idx_pos_arry(ENUM_LFT_LED, &tmpPL);
    pdRht = BG_get_idx_pos_arry(ENUM_RHT_PD, &tmpPL);

    static QGraphicsItem* textItem = NULL;
    QString text;
    text.append("(").append(QString::number(dispLine)).append("/")
            .append(QString::number(nLineCount)).append(")")
            .append(" ledBot:").append(QString::number(ledBot))
            .append(" pdTop:").append(QString::number(pdTop))
            .append(" ledLft:").append(QString::number(ledLft))
            .append(" pdRht:").append(QString::number(pdRht));
    if(textItem != NULL)
    {
        this->scene->removeItem(textItem);
    }
    textItem = this->scene->addText(text);

    lineLst.at(dispLineSav)->setVisible(false);
    lineLst.at(dispLine)->setVisible(true);
    dispLineSav = dispLine;
#endif
#if 0
    int dispLine;
    static int dispLineSav = 0;
    static int ledBotCnt, pdTopCnt, ledLftCnt, pdRhtCnt;
    while(1)
    {
        if (debugLineCnt >= nLineCount) {
            debugLineCnt = 0;
        }
        if (debugLineCnt == 0) {
            ledBotCnt = 0;
            pdTopCnt = 0;
            ledLftCnt = 0;
            pdRhtCnt = 0;
        }
        dispLine = debugLineCnt++;
        tmpPL = pLines[dispLine];
        ledBot = BG_get_idx_pos_arry(ENUM_BOT_LED, &tmpPL);
        pdTop = BG_get_idx_pos_arry(ENUM_TOP_PD, &tmpPL);
        ledLft = BG_get_idx_pos_arry(ENUM_LFT_LED, &tmpPL);
        pdRht = BG_get_idx_pos_arry(ENUM_RHT_PD, &tmpPL);

        if (ledBot == 0) {
            ledBotCnt++;
        }
        if (ledLft == 0) {
            ledLftCnt++;
        }
        if (pdTop == 95) {
            pdTopCnt++;
        }
        if (pdRht == 71) {
            pdRhtCnt++;
        }
        //TRACE("debugLineCnt,ledBot,pdTop,ledLft,pdRht:%d,%d,%d,%d,%d",debugLineCnt,ledBot,pdTop,ledLft,pdRht);
        if (ledBotCnt == 1 || pdTopCnt == 1 || pdRhtCnt == 1 || ledLftCnt == 1) {
            break;
        }
    }

    static QGraphicsItem* textItem = NULL;
    QString text;
    text.append("(").append(QString::number(dispLine)).append("/")
            .append(QString::number(nLineCount)).append(")")
            .append(" ledBot:").append(QString::number(ledBot))
            .append(" pdTop:").append(QString::number(pdTop))
            .append(" ledLft:").append(QString::number(ledLft))
            .append(" pdRht:").append(QString::number(pdRht));
    if(textItem != NULL)
    {
        this->scene->removeItem(textItem);
    }
    textItem = this->scene->addText(text);

    lineLst.at(dispLineSav)->setVisible(false);
    lineLst.at(dispLine)->setVisible(true);
    dispLineSav = dispLine;
#endif
#if 1
    start_end_t *lineInfoAll;
    static int dispSenNo = -1;
    static int dispBd = 0;
    int senVal, maxSenVal, dispCnt, endTmp;

    if (dispBd == 0 || dispBd == 2) {
        maxSenVal = BOTTOM_NUM_LED;
    }
    else {
        maxSenVal = LEFT_NUM_LED;
    }

    dispSenNo++;
    if (dispSenNo >= maxSenVal) {
        dispSenNo = 0;
        dispBd++;
        if (dispBd >= 4) dispBd = 0;
    }

    dispCnt = 0;
    for (int i = 0; i < nLineCount; i++)
    {
        tmpPL = pLines[i];
        ledBot = BG_get_idx_pos_arry(ENUM_BOT_BD, &tmpPL);
        pdTop = BG_get_idx_pos_arry(ENUM_TOP_BD, &tmpPL);
        ledLft = BG_get_idx_pos_arry(ENUM_LFT_BD, &tmpPL);
        pdRht = BG_get_idx_pos_arry(ENUM_RHT_BD, &tmpPL);
        if (dispBd == 0) {
            senVal = ledBot;
        }
        else if (dispBd == 1) {
            senVal = ledLft;
        }
        else if (dispBd == 2) {
            senVal = pdTop;
        }
        else {
            senVal = pdRht;
        }

        if (senVal == dispSenNo) {
            dispCnt++;
            static QGraphicsItem* textItem = NULL;
            QString text;
            text.append("(").append(QString::number(i)).append("/")
                    .append(QString::number(nLineCount)).append(")")
                    .append(" dispCnt:").append(QString::number(dispCnt))
                    .append(" ledBot:").append(QString::number(ledBot))
                    .append(" pdTop:").append(QString::number(pdTop))
                    .append(" ledLft:").append(QString::number(ledLft))
                    .append(" pdRht:").append(QString::number(pdRht));
            if(textItem != NULL)
            {
                this->scene->removeItem(textItem);
            }
            textItem = this->scene->addText(text);
            lineLst.at(i)->setVisible(true);
            lineInfoAll = &BG_line_info_all[i];
            endTmp = BG_get_sen_end(dispBd, lineInfoAll);
            //BG_check_dead_zone(dispBd, lineInfoAll);
            TRACE("ledBot,pdTop,ledLft,pdRht,endTmp,%d,%d,%d,%d,(,%d,)", ledBot,pdTop,ledLft,pdRht,endTmp);
        }
        else {
            lineLst.at(i)->setVisible(false);
        }
    } //for (int i = 0; i < nLineCount; i++)

    int dzCnt = BG_dead_zone[dispBd][dispSenNo].dzCnt;
    for (int i = 0; i < dzCnt; i++) {
        int st = BG_dead_zone[dispBd][dispSenNo].dz[i].st;
        int ed = BG_dead_zone[dispBd][dispSenNo].dz[i].ed;
        TRACE("..BG_dead_zone[][].dz[].st,ed:,%d,%d,(,%d,),%d,%d", dispBd, dispSenNo, i, st, ed);
    }
#endif
}
#endif

MainWindow::~MainWindow()
{
    allow_system_sleep_on(true);
    //CBulkCom::ExitDebug();
    //delete m_pColorWithValue;
    //delete ow;
    destoryLogFile();
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
#if 0 //def TEMP_FILE_WRITE
    if(bFromFile == false)
    {
        QString filePath = QFileDialog::getSaveFileName(this, tr(" "), QDir::currentPath(), tr("*.dlt"));
        if(!filePath.isNull())
        {
            m_soureFile.copy(filePath);
        }
    }
#endif
    QMainWindow::closeEvent(e);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    //TRACE("keyPressEvent: 0x%x", e->key());
    switch(e->key())
    {
    case Qt::Key_Control:
    {
        this->view->setDragMode(QGraphicsView::ScrollHandDrag);
    }
        break;

    case Qt::Key_1:
    {
        this->bOneStep = false;
        BG_debug_draw_flag = 0;
#ifdef DRAW_THREAD
        if (m_Backend_Draw_Thread->isRunning()) {
            m_Backend_Draw_Thread->stopDrawThread();
            m_Backend_Wait_Thread->stopWaitThread();
        }
#endif
    }
        break;

    case Qt::Key_Period:
    {
        if (!bChangeThreshold) {
            bChangeThreshold = true;
        lineThresholdValue++;
        lineThresholdValue = GET_MIN(lineThresholdValue, 99);
    }
    }
        break;

    case Qt::Key_Comma:
    {
        if (!bChangeThreshold) {
            bChangeThreshold = true;
        lineThresholdValue--;
        lineThresholdValue = GET_MAX(lineThresholdValue, 1);
    }
    }
        break;

    default:
    {
        QMainWindow::keyReleaseEvent(e);
    }
        break;
    }
}

#define HELP_STRING "\
'O': Open USB \r\n\
'Z': Open Captured File\r\n\
'B': Brush,Pen&Marker Mode\r\n\
'W': Capture Start/Stop\r\n\
'P': Play/Pause\r\n\
'1' : One Step\r\n\
'D' : Touch Color Id/Size\r\n\
'R' : Remained Line On/Off \r\n\
'A' : All Line On/Off \r\n\
'>' : Thr10 Up\r\n\
'<' : Thr10 Down\r\n\
'ESC' : Exit"

void MainWindow::keyReleaseEvent(QKeyEvent *e)
{
    int key_in = e->key();
    //TRACE("keyReleaseEvent: 0x%x", key_in);
    switch(key_in)
    {
    case Qt::Key_2:
    {
#ifdef DRAW_THREAD
         wakeUpForWaitThread = 1;
         stopWaitThread = 0;
         BG_debug_draw_flag = 1;
         if (!m_Backend_Draw_Thread->isRunning()) {
             m_Backend_Draw_Thread->startDrawThread();
             m_Backend_Wait_Thread->startWaitThread();
         }
#endif
    }
        break;
    case Qt::Key_3:
    {
#ifdef DRAW_THREAD
        stopWaitThread = 1;
        BG_debug_draw_flag = 1;

        if (!m_Backend_Draw_Thread->isRunning()) {
            m_Backend_Draw_Thread->startDrawThread();
            m_Backend_Wait_Thread->startWaitThread();
        }
#endif
    }
        break;

    case Qt::Key_H:
    {
        QMessageBox::information(this, tr("Help"), tr(HELP_STRING));
    }
        break;

#if (BRUSH_MODE_ENABLE == 1)
    case Qt::Key_B:
    {
        if (testNextScan.brushMode == 1) {
            testNextScan.brushMode = 0;
        }
        else {
            testNextScan.brushMode = 1;
        }
    }
        break;
#endif

    case Qt::Key_P:
    {
        this->bPause = !this->bPause;
        this->bOneStep = true;
    }
        break;

    case Qt::Key_Z:
    {
        savedFileOpen();
        this->bFirstStart = true;
        this->bPause = false;
        this->bOneStep = false;
    }
        break;

    case Qt::Key_O:
    {
#if 1 //for testing
        usb_check_n_open();
        this->bFirstStart = false;
        this->bPause = false;
#endif
    }
        break;

    case Qt::Key_D:
    {
        if (this->touchDisplayColor == TOUCH_DISP_COLOR_ID) {
            this->touchDisplayColor = TOUCH_DISP_COLOR_SIZE;
        }
        else {
            this->touchDisplayColor = TOUCH_DISP_COLOR_ID;
        }
        //BG_show_debug++;
        //if (BG_show_debug > BG_show_debug_max) BG_show_debug = 0;
    }
        break;

#ifdef TEMP_FILE_WRITE
    case Qt::Key_S:
    case Qt::Key_W:
    {
        if (bInitOfst && !bFromFile) {
            if (bWriteFile) {
                bWriteFile = false;
#ifdef TEMP_FILE_WRITE
                QString filePath = QFileDialog::getSaveFileName(this, tr(" "), QDir::currentPath(), tr("*.dlt"));
                if(!filePath.isNull()) {
                    m_soureFile.copy(filePath);
                }
#endif
            }
            else {
#ifdef TEMP_FILE_WRITE
                m_soureFile.open();
                m_soureFile.write(ofstData, ofstDataCnt);
#endif
                bWriteFile = true;
            }
            if (key_in == Qt::Key_S) {
                touchLabel[0]->setStyleSheet("QLabel { color : red; }");
                bWriteFile2 = bWriteFile;
            }
        }
    }
        break;
#endif

    case Qt::Key_C:
    {
        foreach(QGraphicsLineItem* item, touchLst)
        {
            this->scene->removeItem(item);
        }
        touchLst.clear();
    }
        break;

    case Qt::Key_Control:
    {
        this->view->setDragMode(QGraphicsView::RubberBandDrag);
    }
        break;

    case Qt::Key_Escape:
    {
        this->bPause = true;
        this->bOneStep = true;
        this->close();
    }
        break;

    case Qt::Key_A:
    {
        BG_show_line_all = (BG_show_line_all == 0) ? 1 : 0;
            if (BG_show_line_all && BG_show_line_rem) {
                BG_show_line_rem = 0;
            }
            TRACE("BG_show_line_all,BG_show_line_rem= %d %d", BG_show_line_all, BG_show_line_rem);
    }
        break;

    case Qt::Key_R:
    {
        BG_show_line_rem = (BG_show_line_rem == 0) ? 1 : 0;
            if (BG_show_line_all && BG_show_line_rem) {
                BG_show_line_all = 0;
            }
            TRACE("BG_show_line_all,BG_show_line_rem= %d %d", BG_show_line_all, BG_show_line_rem);
    }
        break;

    case Qt::Key_E:
    {
        BG_show_ep = (BG_show_ep == 0) ? 1 : 0;
    }
        break;

    case Qt::Key_V:
    {
        this->nShowEnableVertical++;
        if (this->nShowEnableVertical == 3) {
            this->nShowEnableVertical = 0;
        }
    }
        break;

    case Qt::Key_F:
    {
        this->nShowReverse++;
        if (this->nShowReverse == 3) {
            this->nShowReverse = 0;
            this->bShowEnable = true;
        }
        else {
            this->bShowEnable = false;
        }
    }
        break;

    case Qt::Key_X:
    {
#if 0 //for testing
        CtrlDialog *dlg = new CtrlDialog();
        dlg->setModal(true);
        dlg->show();
#endif
#if 0
        this->view->setDragMode(QGraphicsView::RubberBandDrag);
#endif
    }
        break;

    default:
    {
        QMainWindow::keyReleaseEvent(e);
    }
        break;
    }
}

void MainWindow::savedFileOpen(void)
{
    if (!AttachedState)
    {
        m_ReadFilePath = QFileDialog::getOpenFileName(this, tr("Saved File Open"), QDir::currentPath(), tr("*.dlt"));
        if(!m_ReadFilePath.isNull())
        {
            m_file = new QFile(m_ReadFilePath);
            m_file->open(QIODevice::ReadOnly);
            if(m_file->isOpen())
            {
                hidThrdUsb_init();
                bFromFile = true;
                bHidConnected = true;
                TRACE_RELEASE("savedFileOpen.. %s", m_ReadFilePath.toStdString().c_str());
            }
            else {
                bFromFile = false;
                bHidConnected = false;
            }
        }
    }
}

#if defined(USE_WIN_USB)
DWORD MainWindow::USB_SendRequest(unsigned char Command)
{
    unsigned char *pa = aUsbOut;
    ULONG BytesWritten = 0;
    OVERLAPPED OverlappedWriteStructure;
    BOOL bRet;
    DWORD error;

    //TRACE("USB_SendRequest... %x", Command);
#ifdef HID_THRD_TEST
    //unsigned char test_data_offset[TEST_OFFSET_BUFF_LEN];
    //unsigned char test_data_line[TEST_DATA_BUFF_LEN];
    if (sizeof(aUsbIn[nBufNumber]) < TEST_DATA_BUFF_LEN) {
        TRACE_ERROR("ERROR! aUsbIn is too small %d (%d)", sizeof(aUsbIn[nBufNumber]), TEST_DATA_BUFF_LEN);
        return FALSE; //error
    }
    if (Command == USB_CMD_OFST_DATA)
    {
        memcpy(aUsbIn[nBufNumber], test_data_offset, TEST_OFFSET_BUFF_LEN);
    }
    else if (Command == USB_CMD_LINE_DATA)
    {
        memcpy(aUsbIn[nBufNumber], test_data_line, TEST_DATA_BUFF_LEN);
    }
    return TRUE; //error
#else
    if (AttachedState == FALSE) {
        return -1; //error
    }

    memset(aUsbOut, 0, USB_OUT_DLT_CMD_SIZE);

    pa[0] = 0x00;
    pa[1] = Command;
#ifdef FRONTEND_LINE_THRESHOLD
    pa[2] = lineThresholdValue; //LINE_THRESHOLD_VALUE;
#else
    pa[2] = 0x01; //reserved
#endif

    uint16_t crc = USB_CalculateCrc(&pa[0], 3);
    pa[3] = (uint8_t)crc;
    pa[4] = (crc >> 8);

    //Initialize the events and overlapped structures to their proper states before we use them.
    HANDLE IOEvent = CreateEvent(NULL, TRUE, FALSE, NULL);	//Manual reset, initially non-signalled state
    ResetEvent(IOEvent);
    OverlappedWriteStructure.Internal = 0;
    OverlappedWriteStructure.InternalHigh = 0;
    OverlappedWriteStructure.Offset = 0;
    OverlappedWriteStructure.OffsetHigh = 0;
    OverlappedWriteStructure.Pointer = 0;
    OverlappedWriteStructure.hEvent = IOEvent;

    //Actually start sending the data.
    //Send data OUT (of the host) on endpoint 1 (pipe ID = 0x01)
    bRet = WinUsb_WritePipe(MyWinUSBInterfaceHandle, 0x01, aUsbOut, USB_OUT_DLT_CMD_SIZE, &BytesWritten, &OverlappedWriteStructure);
    error = GetLastError();
    if (!bRet) {
        if (error != ERROR_IO_PENDING) {
            TRACENR_ERROR("ERROR! USB_SendRequest.1.WinUsb_WritePipe: %s", myGetLastErrorString());
        }
        return error; //error
    }

    //Wait for the data to finish being transferred.
    //Blocking function until complete or timeout occurs.
    WaitForSingleObject(IOEvent, 9000);
    CloseHandle(IOEvent);

    //Alternative method of waiting until the data is finished being transferred.
    //while(!HasOverlappedIoCompleted(&OverlappedWriteStructure) || !HasOverlappedIoCompleted(&OverlappedWriteStructure2) || !HasOverlappedIoCompleted(&OverlappedWriteStructure3));

    //At this point, all of the data was sent (assuming no error conditions, ex: user unplugged
    //the USB cable during the transmission).  A complete application should ideally use error case
    //checking/handling code to ensure a smooth user experience, even in the event of the unexpected.
    //CumulativeBytesXferred += OverlappedWriteStructure.InternalHigh;

    if (error != ERROR_IO_PENDING) {
        TRACENR_ERROR("ERROR! USB_SendRequest.2.WinUsb_WritePipe: %s", myGetLastErrorString2(error));
        return error; //error
    }
    return error; //no-error
#endif
}

DWORD MainWindow::USB_readData(unsigned char *InPtr, int *nResult, int mode)
{
    bool bRet;
    DWORD error;
    *nResult = 0;

    if (AttachedState == FALSE) {
        return -1; //error
    }

    //Initialize the events and overlapped structures to their proper states before we use them.
    if (mode) {
        OVERLAPPED OverlappedReadStructure;
        HANDLE IOEvent = CreateEvent(NULL, TRUE, FALSE, NULL);	//Manual reset, initially non-signalled state
        ResetEvent(IOEvent);
        OverlappedReadStructure.Internal = 0;
        OverlappedReadStructure.InternalHigh = 0;
        OverlappedReadStructure.Offset = 0;
        OverlappedReadStructure.OffsetHigh = 0;
        OverlappedReadStructure.Pointer = 0;
        OverlappedReadStructure.hEvent = IOEvent;
        bRet = WinUsb_ReadPipe(MyWinUSBInterfaceHandle, 0x81, InPtr, USB_PACKET_SIZE, (PULONG)nResult, &OverlappedReadStructure);

        //Wait for the data to finish being transferred.
        //Blocking function until complete or timeout occurs.
        WaitForSingleObject(IOEvent, 9);
        CloseHandle(IOEvent);

        //Alternative method of waiting until the data is finished being transferred.
        //while(!HasOverlappedIoCompleted(&OverlappedWriteStructure) || !HasOverlappedIoCompleted(&OverlappedWriteStructure2) || !HasOverlappedIoCompleted(&OverlappedWriteStructure3));
        //At this point, all of the data was sent (assuming no error conditions, ex: user unplugged
        //the USB cable during the transmission).  A complete application should ideally use error case
        //checking/handling code to ensure a smooth user experience, even in the event of the unexpected.
        //CumulativeBytesXferred += OverlappedWriteStructure.InternalHigh;
    }
    else {
        bRet = WinUsb_ReadPipe(MyWinUSBInterfaceHandle, 0x81, InPtr, USB_PACKET_SIZE, (PULONG)nResult, NULL);
    }

    error = GetLastError();
    if (!bRet || error != ERROR_SUCCESS) {
        TRACENR_ERROR("ERROR! USB_readData..WinUsb_ReadPipe: %d %s", mode, myGetLastErrorString2(error));
    }
    return error;
}
#endif //USE_WIN_USB

#if defined(USE_USB_LIB)
DWORD MainWindow::USB_SendRequest(unsigned char Command)
{
    unsigned char *pa = aUsbOut;
    if (AttachedState == FALSE) {
        return -1; //error
    }

    //TRACE("USB_SendRequest... %x", Command);
    memset(aUsbOut, 0, USB_CMD_LEN);

    pa[0] = REPORT_ID;
    pa[1] = Command;
#ifdef FRONTEND_LINE_THRESHOLD
    pa[2] = lineThresholdValue; //LINE_THRESHOLD_VALUE;
#else
    pa[2] = 0x01; //reserved
#endif

    uint16_t crc = USB_CalculateCrc(&pa[0], USB_CMD_LEN);
    pa[USB_CMD_LEN] = (uint8_t)crc;
    pa[USB_CMD_LEN + 1] = (crc >> 8);

#if 0 //dump cmd
    TRACENR("HID_SendNorCmd..%x %x: ", Command, Param);
    for (int j = 0; j < NOR_RES_SIZE; j++) {
        TRACENR("%02x ", aUsbOut[j]);
    }
    TRACE(".");
#endif

    //WaitForSingleObject(hHidMutex, INFINITE);
    int res = hid_write(hUsb_hid_dev, aUsbOut, MAX_USB_OUT_BUF_SIZE);
    if (res < 0) {
        TRACE("Error! HID_SendRequest... hid_write() (%s)", myGetLastErrorString());
    }
    //ReleaseMutex(hHidMutex);
    return (res < 0) ? TRUE : FALSE;
}

DWORD MainWindow::USB_readData(unsigned char *InPtr, int *nResult, int mode)
{
    if (hUsb_hid_dev == NULL) {
        TRACE_ERROR("ERROR! USB_readData..invalid hUsb_hid_dev %x (%d)", hUsb_hid_dev, mode);
        return -2; //error
    }

    (*nResult) = hid_read_timeout(hUsb_hid_dev, &aUsbInTmp[0], USB_PACKET_SIZE, USB_READ_TIMEOUT);
    if ((*nResult) == 0) {
        return ERROR_SUCCESS; //idle
    }
    else if ((*nResult) < 0) {
        DWORD error = GetLastError();
        TRACE_ERROR("ERROR! ..hid_read_timeout: %s", myGetLastErrorString2(error));
        return error; //error
    }

    if ((*nResult) != USB_PACKET_SIZE) {
        TRACE_ERROR("ERROR! invalid nResult: %d", nResult);
        (*nResult) = -3;
        return -3; //error
    }
    if (aUsbInTmp[0] == REPORT_ID) {
        memcpy(InPtr, &aUsbInTmp[1], (USB_PACKET_SIZE - 1));
        (*nResult) = (USB_PACKET_SIZE - 1);
    }

    return ERROR_SUCCESS;
}

int MainWindow::HID_RecvNorRes(void)
{
    int nResult = hid_read_timeout(hUsb_hid_dev, &aUsbIn[nBufNumber][0], USB_PACKET_SIZE, USB_READ_TIMEOUT);
#if 0 //dump cmd
    TRACENR("HID_RecvNorRes.. ");
    for (int j = 0; j < NOR_RES_SIZE; j++) {
        TRACENR("%02x ", aUsbIn[nBufNumber][j]);
    }
    TRACE(".");
#endif
    if (aUsbIn[nBufNumber][0] != REPORT_ID) {
        TRACE_ERROR("ERROR! HID_RecvNorRes..invalid ReportID: %d", REPORT_ID);
        return TARGET_RES_ERR;
    }
    int res = aUsbIn[nBufNumber][1];
#if 0
    uint16_t crc_cal = USB_CalculateCrc(&aUsbIn[nBufNumber][0], (NOR_RES_SIZE - 2));
    uint16_t crc_get = aUsbIn[nBufNumber][NOR_RES_SIZE - 1];
    crc_get = (crc_get << 8) | aUsbIn[nBufNumber][NOR_RES_SIZE - 2];
    if (crc_cal != crc_get) {
        TRACE_ERROR("ERROR! HID_RecvNorRes..invalid CRC: %d, %04x, %04x", nResult, crc_cal, crc_get);
        return TARGET_RES_ERR;
    }
#endif
    return res;
}
#endif //USE_USB_LIB


uint32_t MainWindow::USB_CalculateCrc(uint8_t *data, uint32_t len)
{
    uint32_t i;
    uint16_t crc = 0;

    while (len--)
    {
        i = (crc >> 12) ^ (*data >> 4);
        crc = crc_table[i & 0x0F] ^ (crc << 4);
        i = (crc >> 12) ^ (*data >> 0);
        crc = crc_table[i & 0x0F] ^ (crc << 4);
        data++;
    }

    return (crc & 0xFFFF);
}

#if defined(DPT_POLYGON_DATA)
////////////////////////////////////////////
// check header for draw_poly_data
int check_header_poly(unsigned char *InPtr, int nReceivedCount, int *totalPacketSize, int *nTotalPacketNumber, int *nPacketStart)
{
    int len;

    if (InPtr[0] != 0 || InPtr[1] != 0 || InPtr[2] != 0x12 || InPtr[3] != PD_DATA_HEADER)
    {
        TRACE("Error! hidThrdUsb().0.invalid header %02x %02x %02x %02x", InPtr[0], InPtr[1], InPtr[2], InPtr[3]);
        return 1; //error
    }

    len = (int)((InPtr[POLYGON_DATA_LEN_IDX]) | (InPtr[POLYGON_DATA_LEN_IDX + 1] << 8) |
        (InPtr[POLYGON_DATA_LEN_IDX + 2] << 16) | (InPtr[POLYGON_DATA_LEN_IDX + 3] << 24));
    *nTotalPacketNumber = len;
    //TRACE("check_header_poly... %d", len);
    //TRACE("check_header... nTotalPacketNumber=%d", nTotalPacketNumber);
    //nResult -= (PD_DATA_IDX2 - 1); //not include header size
    *nPacketStart = nReceivedCount;

    return 0; //no error
}
#endif

void MainWindow::initShiftTbl64(void)
{
    int i;
    uint64_t sm = 1;

    for (i = 0; i < MAX_SHIFT_TBL; i++)
    {
        shiftTbl64[i] = sm << i;
    }
}

void MainWindow::get_slope_size(void)
{
    slope_byte_x = ((offsetTblLenX - 1) / 8 + 1);
    slope_byte_y = ((offsetTblLenY - 1) / 8 + 1);
}

void MainWindow::get_slope_val(axis_t axis, int stIdx, int pd, unsigned char *pRcv, slope_mask_t *slopeVal)
{
    uint64_t uint64Tmp;
    //uint64_t slopeIdx = 0;
    //unsigned char *pRcvTmp;
    int j, idx, slopeByte;

    if (axis == ENUM_HOR_X) {
        slopeByte = slope_byte_x;
    }
    else {
        slopeByte = slope_byte_y;
    }

    idx = stIdx + (pd * slopeByte);
    slopeVal->upper = 0;
    slopeVal->lower = 0;

    for (j = 0; j < slopeByte; j++)
    {
        uint64Tmp = (uint64_t)pRcv[idx + j];
        if (j >= MAX_SHIFT_TBL_BYTE) {
            slopeVal->upper |= uint64Tmp << ((j - MAX_SHIFT_TBL_BYTE) * 8);
        }
        else {
            //test_data[offIdx] |= (uint8_t)((slopeVal.lower >> j) & 0xFF);
            //LSB first
            slopeVal->lower |= uint64Tmp << (j * 8);
        }
    }
    //TRACE("slopeVal->upper,slopeVal->lower: %016llx %016llx", slopeVal->upper, slopeVal->lower);

    if (bDispInverse) {
        slopeVal->upper = ~slopeVal->upper;
        slopeVal->lower = ~slopeVal->lower;
    }
}

int MainWindow::check_header_line(unsigned char *InPtr, int *totalPacketSize, int *nPacketStart)
{
    if (InPtr[0] != 0 || InPtr[1] != 0 || InPtr[2] != 0 || InPtr[3] != PD_DATA_HEADER) {
        //TRACE("Error! check_header_line...invalid header %d,%d,%d,%d,%d", InPtr[0], InPtr[1], InPtr[2], InPtr[3], InPtr[4]);
        TRACENR(",");
        return 1; //error
    }

    get_slope_size();
    (*totalPacketSize) = PD_DATA_IDX2 + (disp.nMaxPdX * slope_byte_x) + (disp.nMaxPdY * slope_byte_y);
#ifdef FRONTEND_LINE_THRESHOLD
    (*totalPacketSize) += (disp.nMaxPdX * slope_byte_x) + (disp.nMaxPdY * slope_byte_y);
#endif
    *nPacketStart = 0;

    return 0; //no error
}

void MainWindow::get_slope_mask(axis_t axis, uint16_t slope_idx, slope_mask_t *sm)
{
    //uint64_t sm1 = 1;
    int offset_size;

    if (axis == ENUM_HOR_X) {
        offset_size = offsetTblLenX;
    }
    else {
        offset_size = offsetTblLenY;
    }

    if (offset_size < 10 || offset_size > (MAX_SHIFT_TBL * 2)) {
        TRACE_ERROR("ERROR! invalid offset_size %d", offset_size);
        return;
    }

    sm->lower = 0;
    sm->upper = 0;
    if (slope_idx >= offset_size) {
        TRACE_ERROR("ERROR! invalid slope_idx %d (%d)", slope_idx, offset_size);
        return;
    }

    if (slope_idx >= MAX_SHIFT_TBL) {
        //sm->upper = sm1 << (upper_bit - 1 - slope_idx);
        sm->upper = shiftTbl64[slope_idx - MAX_SHIFT_TBL];
        sm->lower = 0;
    }
    else {
        sm->upper = 0;
        sm->lower = shiftTbl64[slope_idx];
    }
}

////////////////////////////////////////////
// check header for offset
int MainWindow::check_header_ofst(unsigned char *InPtr, int *totalPacketSize, int *nPacketStart)
{
    uint8_t lenX, lenY;
    int idx;
    for (idx = 0; idx < (USB_PACKET_SIZE - PD_IDX_VAR_HD); idx += 4) {
        if (InPtr[idx] != 0 || InPtr[idx + 1] != 0 || InPtr[idx + 2] != 0 || InPtr[idx + 3] != PD_OFST_HEADER) {
            continue;
        }
        disp.nMaxPdX = ((InPtr[idx + PD_IDX_NUM_OF_PD_X] << 8) & 0xff00) | InPtr[idx + PD_IDX_NUM_OF_PD_X + 1]; //0xbf
        disp.nMaxPdY = ((InPtr[idx + PD_IDX_NUM_OF_PD_Y] << 8) & 0xff00) | InPtr[idx + PD_IDX_NUM_OF_PD_Y + 1]; //0x6c
        if (disp.nMaxPdX < 10 || disp.nMaxPdY < 10 || disp.nMaxPdX > MAX_SENSOR_POS_SIZE || disp.nMaxPdY > MAX_SENSOR_POS_SIZE) {
            TRACE_ERROR("ERROR! invlaid disp.nMaxPdX,nMaxPdY %d %d %d", idx, disp.nMaxPdX, disp.nMaxPdY);
            continue;
        }
        lenX = (uint8_t)InPtr[idx + PD_IDX_OFST_LEN_X];
        lenY = (uint8_t)InPtr[idx + PD_IDX_OFST_LEN_Y];
        if (lenX == 0 || lenY == 0) {
            TRACE("ERROR! check_header_ofst..invalid lenX,lenY %d %d %d", idx, lenX, lenY);
            continue;
        }
        *nPacketStart = idx;
        break; //found
    }
    if (idx >= (USB_PACKET_SIZE - PD_IDX_VAR_HD)) {
        //TRACE("Error! check_header_ofst..invalid header %d %02x %02x %02x %02x %02x", idx, InPtr[idx + 0], InPtr[idx + 1], InPtr[idx + 2], InPtr[idx + 3], InPtr[idx + 4]);
        return 1; //error
    }
#if 1 //nsmoon@191029
    //(float x_pos_led+pd) + (float y_pos_led+pd) + (float x_zero + x_end + y_zero + y_end) + (float x_zero_aa + x_end_aa + y_zero_aa + y_end_aa) + senX2 + senY2
    *totalPacketSize = PD_IDX_POS_START + (4 * disp.nMaxPdX * 2) + (4 * disp.nMaxPdY * 2) + (4 * 4) + (4 * 4);
#if (DEAD_CELL_TBL == 1) //nsmoon@190926
    uint8_t deadXled = (uint8_t)InPtr[PD_IDX_DCELL_LEN_X_LED];
    uint8_t deadXpd = (uint8_t)InPtr[PD_IDX_DCELL_LEN_X_PD];
    uint8_t deadYled = (uint8_t)InPtr[PD_IDX_DCELL_LEN_Y_LED];
    uint8_t deadYpd = (uint8_t)InPtr[PD_IDX_DCELL_LEN_Y_PD];
    *totalPacketSize += (deadXled + deadXpd + deadYled + deadYpd);
#endif //DEAD_CELL_TBL
#if 1 //(SECOND_OFST_TBL == 1) //nsmoon@190624
    uint8_t senX2 = (uint8_t)InPtr[PD_IDX_SEN_LEN_X2];
    uint8_t senY2 = (uint8_t)InPtr[PD_IDX_SEN_LEN_Y2];
    *totalPacketSize += (senX2 + senY2);
#endif
#else
#if 1 //def FRONTEND_AAREA //nsmoon@190325
    *totalPacketSize = PD_IDX_OFST_X + lenX + lenY + (4 * disp.nMaxPdX * 2) + (4 * disp.nMaxPdY * 2) + (4 * 8);
#else
    *totalPacketSize = PD_IDX_OFST_X + lenX + lenY + (4 * disp.nMaxPdX * 2) + (4 * disp.nMaxPdY * 2);
#endif
#endif //1
    num_of_dlt_pkt = (*totalPacketSize + (USB_PACKET_SIZE - 1)) / USB_PACKET_SIZE;
    TRACE("check_header_ofst...*totalPacketSize,*nPacketStart,num_of_dlt_pkt: %d (%d)(%d)", *totalPacketSize, *nPacketStart, num_of_dlt_pkt);

    return 0; //no error
}

void MainWindow::hidThrdUsb_init(void)
{
    initShiftTbl64();
    //bHidConnected = true; //thread run
    bInitOfst = false; //nsmoon@170527a
}

bool MainWindow::usb_check_n_open(void)
{
    //Check and make sure at least one device with matching VID/PID is attached
#if defined(USE_WIN_USB)
    if (CheckIfPresentAndGetUSBDevicePath())
    {
        if (MyDeviceHandle != INVALID_HANDLE_VALUE) {
            TRACE("Already opened USB port!");
            return false;
        }
        emit CallUsbOpen();
    }
    else {
        //TRACE("Closed USB port!");
        if (AttachedState) {
            emit CallUsbClose();
        }
        AttachedState = FALSE;
        bHidConnected = FALSE; //thread run
    }
#elif defined(USE_USB_LIB)
    if (USB_enumerate())
    {
        if (hUsb_hid_dev) {
            TRACE("Already opened USB port!");
            return false;
        }
        emit CallUsbOpen();
    }
    else {
        //TRACE("Closed USB port!");
        if (AttachedState) {
            emit CallUsbClose();
        }
        AttachedState = FALSE;
        bHidConnected = FALSE; //thread run
    }
#endif
    return true;
}

#if defined(USE_WIN_USB)
bool MainWindow::USB_open(void)
{
    //get_vid_pid();
    //We now have the proper device path, and we can finally open a device handle to the device.
    //WinUSB requires the device handle to be opened with the FILE_FLAG_OVERLAPPED attribute.
    MyDeviceHandle = CreateFile((DetailedInterfaceDataStructure->DevicePath), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
    DWORD ErrorStatus = GetLastError();
    if (ErrorStatus == ERROR_SUCCESS)
    {
        //Now get the WinUSB interface handle by calling WinUsb_Initialize() and providing the device handle.
        BOOL BoolStatus = WinUsb_Initialize(MyDeviceHandle, &MyWinUSBInterfaceHandle);
        if (BoolStatus == TRUE) {
            hidThrdUsb_init();
            AttachedState = TRUE;
            bHidConnected = TRUE; //thread run
            TRACE("USB opened...");
            Sleep(100);
            return TRUE;
        }
        else {
            TRACE("USB device detect, but not found or open error !!");
        }
    }
    else {
        TRACE_ERROR("ERROR! CreateFile.. %d (%s)", ErrorStatus, myGetLastErrorString2(ErrorStatus));
    }
    AttachedState = FALSE;
    bHidConnected = FALSE; //thread run
    return FALSE;
}

bool MainWindow::USB_close(void)
{
    CloseHandle(MyDeviceHandle);
    WinUsb_Free(MyWinUSBInterfaceHandle);
    MyDeviceHandle = INVALID_HANDLE_VALUE;
    return true;
}

#if 1 //nsmoon@190123
bool MainWindow::CheckIfPresentAndGetUSBDevicePath(void)
{
    /*
    Before we can "connect" our application to our USB embedded device, we must first find the device.
    A USB bus can have many devices simultaneously connected, so somehow we have to find our device, and only
    our device.  This is done with the Vendor ID (VID) and Product ID (PID).  Each USB product line should have
    a unique combination of VID and PID.
    Microsoft has created a number of functions which are useful for finding plug and play devices.  Documentation
    for each function used can be found in the MSDN library.  We will be using the following functions:
    SetupDiGetClassDevs()					//provided by setupapi.dll, which comes with Windows
    SetupDiEnumDeviceInterfaces()			//provided by setupapi.dll, which comes with Windows
    GetLastError()							//provided by kernel32.dll, which comes with Windows
    SetupDiDestroyDeviceInfoList()			//provided by setupapi.dll, which comes with Windows
    SetupDiGetDeviceInterfaceDetail()		//provided by setupapi.dll, which comes with Windows
    SetupDiGetDeviceRegistryProperty()		//provided by setupapi.dll, which comes with Windows
    malloc()								//part of C runtime library, msvcrt.dll?
    CreateFile()							//provided by kernel32.dll, which comes with Windows
    We will also be using the following unusual data types and structures.  Documentation can also be found in
    the MSDN library:
    PSP_DEVICE_INTERFACE_DATA
    PSP_DEVICE_INTERFACE_DETAIL_DATA
    SP_DEVINFO_DATA
    HDEVINFO
    HANDLE
    GUID
    The ultimate objective of the following code is to call WinUsb_Initialize(), which gets an interface handle
    to a specific interface on a USB device.  This handle will be needed later when calling WinUsb_WritePipe()
    and WinUsb_ReadPipe().  These functions are used to actually send and receive application related data
    to/from the USB peripheral device.
    However, in order to call WinUsb_Initialize(), we first need to get the device path for the USB device
    with the correct VID and PID.  Getting the device path is a multi-step round about process, which
    requires calling several of the SetupDixxx() functions provided by setupapi.dll.
    */

    //Globally Unique Identifier (GUID). Windows uses GUIDs to identify things.  This GUID needs to match
    //the GUID that is used in the .INF file used to install the WinUSB driver onto the system.
    //The INF file creates a register entry which associates a GUID with the WinUSB device.  In order for
    //a user mode application (such as this one) to find the USB device on the bus, it needs to known the
    //correct GUID that got put into the registry.
    //GUID InterfaceClassGuid = { 0x58D07210, 0x27C1, 0x11DD, 0xBD, 0x0B, 0x08, 0x00, 0x20, 0x0C, 0x9A, 0x66 };
    GUID InterfaceClassGuid = genericUSBGUID;

    HDEVINFO DeviceInfoTable = INVALID_HANDLE_VALUE;
    PSP_DEVICE_INTERFACE_DATA InterfaceDataStructure = new SP_DEVICE_INTERFACE_DATA;
//	PSP_DEVICE_INTERFACE_DETAIL_DATA DetailedInterfaceDataStructure = new SP_DEVICE_INTERFACE_DETAIL_DATA;	//Global
    SP_DEVINFO_DATA DevInfoData;

    DWORD InterfaceIndex = 0;
    //DWORD StatusLastError = 0;
    DWORD dwRegType;
    DWORD dwRegSize;
    DWORD StructureSize = 0;
    PBYTE PropertyValueBuffer;
    bool MatchFound = false;
    DWORD ErrorStatus;
    //BOOL BoolStatus = FALSE;
    DWORD LoopCounter = 0;

#ifdef USE_CSTRING
    CString DeviceIDToFind; // = MY_DEVICE_ID;
    DeviceIDToFind.Format(_T("Vid_%04x&Pid_%04x"), VendorID, ProductID); //"Vid_04d8&Pid_003f"
#else
    QString DeviceIDToFind;
    DeviceIDToFind.sprintf("Vid_%04x&Pid_%04x", VendorID, ProductID);
    //char DeviceIDToFind[20]; // = MY_DEVICE_ID;
    //sprintf_s(DeviceIDToFind, "VID_%04X&PID_%04X", VendorID, ProductID); //"Vid_04d8&Pid_003f"
#endif

    //First populate a list of plugged in devices (by specifying "DIGCF_PRESENT"), which are of the specified class GUID.
    DeviceInfoTable = SetupDiGetClassDevs(&InterfaceClassGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    //Now look through the list we just populated.  We are trying to see if any of them match our device.
    while (true)
    {
        InterfaceDataStructure->cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        if (SetupDiEnumDeviceInterfaces(DeviceInfoTable, NULL, &InterfaceClassGuid, InterfaceIndex, InterfaceDataStructure))
        {
            ErrorStatus = GetLastError();
            if (ErrorStatus == ERROR_NO_MORE_ITEMS)	//Did we reach the end of the list of matching devices in the DeviceInfoTable?
            {	//Cound not find the device.  Must not have been attached.
                SetupDiDestroyDeviceInfoList(DeviceInfoTable);	//Clean up the old structure we no longer need.
                return FALSE;
            }
        }
        else	//Else some other kind of unknown error ocurred...
        {
            ErrorStatus = GetLastError();
            SetupDiDestroyDeviceInfoList(DeviceInfoTable);	//Clean up the old structure we no longer need.
            return FALSE;
        }

        //Now retrieve the hardware ID from the registry.  The hardware ID contains the VID and PID, which we will then
        //check to see if it is the correct device or not.

        //Initialize an appropriate SP_DEVINFO_DATA structure.  We need this structure for SetupDiGetDeviceRegistryProperty().
        DevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
        SetupDiEnumDeviceInfo(DeviceInfoTable, InterfaceIndex, &DevInfoData);

        //First query for the size of the hardware ID, so we can know how big a buffer to allocate for the data.
        SetupDiGetDeviceRegistryProperty(DeviceInfoTable, &DevInfoData, SPDRP_HARDWAREID, &dwRegType, NULL, 0, &dwRegSize);

#ifdef USE_CSTRING
        //Allocate a buffer for the hardware ID.
        PropertyValueBuffer = (BYTE *)malloc(dwRegSize);
        if (PropertyValueBuffer == NULL)	//if null, error, couldn't allocate enough memory
        {	//Can't really recover from this situation, just exit instead.
            SetupDiDestroyDeviceInfoList(DeviceInfoTable);	//Clean up the old structure we no longer need.
            return FALSE;
        }

        //Retrieve the hardware IDs for the current device we are looking at.  PropertyValueBuffer gets filled with a
        //REG_MULTI_SZ (array of null terminated strings).  To find a device, we only care about the very first string in the
        //buffer, which will be the "device ID".  The device ID is a string which contains the VID and PID, in the example
        //format "Vid_04d8&Pid_003f".
        SetupDiGetDeviceRegistryProperty(DeviceInfoTable, &DevInfoData, SPDRP_HARDWAREID, &dwRegType, PropertyValueBuffer, dwRegSize, NULL);

        //Now check if the first string in the hardware ID matches the device ID of my USB device.
        //TRACE("PropertyValueBuffer: %s", PropertyValueBuffer);
        CString DeviceIDFromRegistry = PropertyValueBuffer;

        //No longer need the PropertyValueBuffer, free the memory to prevent potential memory leaks
        free(PropertyValueBuffer);

        //Convert both strings to lower case.  This makes the code more robust/portable accross OS Versions
        DeviceIDFromRegistry = DeviceIDFromRegistry.MakeLower();
        DeviceIDToFind = DeviceIDToFind.MakeLower();

        //Now check if the hardware ID we are looking at contains the correct VID/PID
        MatchFound = DeviceIDFromRegistry.Find(DeviceIDToFind);
#else
        QByteArray PropertyValueBuffer(dwRegSize, 0);
        SetupDiGetDeviceRegistryProperty(DeviceInfoTable, &DevInfoData, SPDRP_HARDWAREID, &dwRegType,
                                         reinterpret_cast<unsigned char*>(PropertyValueBuffer.data()), dwRegSize, NULL);
        MatchFound = false;
        if (dwRegSize) {
            int i = 0;
            for (;;) {
                QString s = QString::fromWCharArray((const wchar_t *)PropertyValueBuffer.constData() + i);
                i += s.length() + 1;
                if (s.isEmpty()) break;
                if (s.contains(DeviceIDToFind, Qt::CaseInsensitive)) {
                    MatchFound = true;
                    break;
                }
            }
        }
  #if 0
        char *ptr = strstr((char *)PropertyValueBuffer, (char *)DeviceIDToFind);
        TRACE("ptr: %s", ptr);
        if (ptr == NULL) {
            MatchFound = false;
        }
        else {
            MatchFound = true;
        }
  #endif
#endif

        if (MatchFound == true)
        {
            //Device must have been found.  Open WinUSB interface handle now.  In order to do this, we will need the actual device path first.
            //We can get the path by calling SetupDiGetDeviceInterfaceDetail(), however, we have to call this function twice:  The first
            //time to get the size of the required structure/buffer to hold the detailed interface data, then a second time to actually
            //get the structure (after we have allocated enough memory for the structure.)
            DetailedInterfaceDataStructure->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

            //First call populates "StructureSize" with the correct value
            SetupDiGetDeviceInterfaceDetail(DeviceInfoTable, InterfaceDataStructure, NULL, NULL, &StructureSize, NULL);
            DetailedInterfaceDataStructure = (PSP_DEVICE_INTERFACE_DETAIL_DATA)(malloc(StructureSize));		//Allocate enough memory
            if (DetailedInterfaceDataStructure == NULL)	//if null, error, couldn't allocate enough memory
            {	//Can't really recover from this situation, just exit instead.
                SetupDiDestroyDeviceInfoList(DeviceInfoTable);	//Clean up the old structure we no longer need.
                return FALSE;
            }
            DetailedInterfaceDataStructure->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            //Now call SetupDiGetDeviceInterfaceDetail() a second time to receive the goods.
            SetupDiGetDeviceInterfaceDetail(DeviceInfoTable, InterfaceDataStructure, DetailedInterfaceDataStructure, StructureSize, NULL, NULL);

            //We now have the proper device path, and we can finally open a device handle to the device.
            //WinUSB requires the device handle to be opened with the FILE_FLAG_OVERLAPPED attribute.
            SetupDiDestroyDeviceInfoList(DeviceInfoTable);	//Clean up the old structure we no longer need.
            return TRUE;
        }

        InterfaceIndex++;
        //Keep looping until we either find a device with matching VID and PID, or until we run out of devices to check.
        //However, just in case some unexpected error occurs, keep track of the number of loops executed.
        //If the number of loops exceeds a very large number, exit anyway, to prevent inadvertent infinite looping.
        LoopCounter++;
        if (LoopCounter == 10000000)	//Surely there aren't more than 10 million devices attached to any forseeable PC...
        {
            return FALSE;
        }

    }//end of while(true)
}
#endif
#endif //USE_WIN_USB

#if defined(USE_USB_LIB)
int MainWindow::USB_enumerate(void)
{
    int ret = hid_enumerate2(VendorID, ProductID, interfaceNum, &usbPath[0]);
    return ret; //0:not-found
}

bool MainWindow::USB_open(void)
{
    hUsb_hid_dev = hid_open2(usbPath);
    if (hUsb_hid_dev)
    {
        TRACE("USB_open...");
        Sleep(100);
        hidThrdUsb_init();
        AttachedState = TRUE;
        bHidConnected = TRUE; //thread run
        nDltNormal = TARGET_NORMAL;
        USB_SendRequest(USB_NORCMD_DUMMY); //nsmoon@191113
        return TRUE;
    }

    AttachedState = FALSE;
    bHidConnected = FALSE; //thread run
    TRACE("USB device detect, but not found or open error !!");
    return FALSE;
}

bool MainWindow::USB_close(void)
{
    TRACE("USB_close...");
    if (hUsb_hid_dev) {
        //if (hid_close(hUsb_hid_dev)) {
        //    hUsb_hid_dev = NULL;
        //}
        hid_close(hUsb_hid_dev);
        hUsb_hid_dev = NULL;
    }
    return TRUE; //no-error
}
#endif //USE_USB_LIB

#if 1
/*******************************************
*             WINDOWS EVENTS
********************************************/
/*We use the first WM_PAINT event to get the handle of main window
  and pass it to RegisterDeviceNotification function.
  It not possible to do this in the contructor because the
  main window does not exist yet.
  WM_DEVICECHANGE event notify us that a device is attached or detached */
//#define HID_CLASSGUID {0x4d1e55b2, 0xf16f, 0x11cf,{ 0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30}}
//#define HID_CLASSGUID  {0x745a17a0, 0x74d3, 0x11d0, 0xb6fe, 0x00a0c90f57da}
bool MainWindow::nativeEvent(const QByteArray & eventType, void * message, long * result)
{
    //Generic Globally Unique Identifier (GUID) common for all USB peripheral devices
    //GUID genericUSBGUID = { 0xa5dcbf10, 0x6530, 0x11d2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED };
    genericUSBGUID = { 0x58D07210, 0x27C1, 0x11DD, 0xBD, 0x0B, 0x08, 0x00, 0x20, 0x0C, 0x9A, 0x66 };  //nsmoon@190123
    MSG *msg = static_cast< MSG * > (message);
    int msgType = msg->message;
    QString myVidPid = "VID_04D8&PID_0320";
    //text.append("VID_").append(QString::number(VID_HEX)).append("&PID_").append(QString::number(VID_HEX));

    if(msgType == WM_PAINT)
    {
        if(!bMsg)   //Only the first WM_PAINT
        {
            GUID InterfaceClassGuid = genericUSBGUID;
            DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
            ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
            NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
            NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
            NotificationFilter.dbcc_classguid = InterfaceClassGuid;
            HWND hw = (HWND) this->effectiveWinId();   //Main window handle
            HDEVNOTIFY hDevNotify = RegisterDeviceNotification(hw, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
            if (!hDevNotify)
            {
                DWORD le = GetLastError();
                TRACE("Error! RegisterDeviceNotificationA() failed, le = %x", le);
                return false;
            }//if
            bMsg = true;
            return true;
        }
    }
    if(msgType == WM_DEVICECHANGE)
    {
        return usb_check_n_open();
    }
    return false;
}
#endif

#define DEBUG_set_ofst_data		0
void MainWindow::set_ofst_data(int bufIdx)
{
    unsigned char *pRcv = HidReceiveBuf;
    int i, idx, nextIdx, total;
    DEF_TP_LAYOUT_INFO tp_layout_info;
    unsigned long tmpX, tmpY;

    num_of_dlt_pkt = 0;

#if 1 //nsmoon@191029
#if (DEAD_CELL_TBL > 0) //nsmoon@190926
    uint8_t deadLenXled = pRcv[PD_IDX_DCELL_LEN_X_LED];
    uint8_t deadLenXpd = pRcv[PD_IDX_DCELL_LEN_X_PD];
    uint8_t deadLenYled = pRcv[PD_IDX_DCELL_LEN_Y_LED];
    uint8_t deadLenYpd = pRcv[PD_IDX_DCELL_LEN_Y_PD];
#define MAX_DEAD_CELL_RATIO     1
    if (deadLenXled > (uint8_t)(disp.nMaxPdX / MAX_DEAD_CELL_RATIO)) {
        TRACE_ERROR("ERROR! invalid deadLenXled: %d", deadLenXled);
        deadLenXled = (uint8_t)(disp.nMaxPdX / MAX_DEAD_CELL_RATIO);
    }
    if (deadLenXpd > (uint8_t)(disp.nMaxPdX / MAX_DEAD_CELL_RATIO)) {
        TRACE_ERROR("ERROR! invalid deadLenXpd: %d", deadLenXpd);
        deadLenXpd = (uint8_t)(disp.nMaxPdX / MAX_DEAD_CELL_RATIO);
    }
    if (deadLenYled > (uint8_t)(disp.nMaxPdY / MAX_DEAD_CELL_RATIO)) {
        TRACE_ERROR("ERROR! invalid deadLenYled: %d", deadLenYled);
        deadLenYled = (uint8_t)(disp.nMaxPdY / MAX_DEAD_CELL_RATIO);
    }
    if (deadLenYpd > (uint8_t)(disp.nMaxPdY / MAX_DEAD_CELL_RATIO)) {
        TRACE_ERROR("ERROR! invalid deadLenYpd: %d", deadLenYpd);
        deadLenYpd = (uint8_t)(disp.nMaxPdY / MAX_DEAD_CELL_RATIO);
    }
#endif

#if 1 //(SECOND_OFST_TBL == 1) //nsmoon@190624
    //max offset X and Y
    maxSlopeX = pRcv[PD_IDX_OFST_LEN_X];
    maxSlopeY = pRcv[PD_IDX_OFST_LEN_Y];
    if (maxSlopeX < 10 || maxSlopeX > MAX_X_SLOPE_VAL)
    {
        TRACE_ERROR("ERROR! set_ofst_data..invalid maxSlopeX %d", maxSlopeX);
        return;
    }
    if (maxSlopeY < 10 || maxSlopeY > MAX_Y_SLOPE_VAL)
    {
        TRACE_ERROR("ERROR! set_ofst_data..invalid maxSlopeY %d", maxSlopeY);
        return;
    }

    //max 2nd offset X and Y
    maxSlopeX2 = pRcv[PD_IDX_OFST_LEN_X2];
    maxSlopeY2 = pRcv[PD_IDX_OFST_LEN_Y2];
    if (maxSlopeX2 > MAX_X_SLOPE_VAL) {
        TRACE_ERROR("ERROR! invalid maxSlopeX2: %d", maxSlopeX2);
        maxSlopeX2 = MAX_X_SLOPE_VAL - 1;
    }
    if (maxSlopeY2 > MAX_Y_SLOPE_VAL) {
        TRACE_ERROR("ERROR! invalid maxSlopeY2: %d", maxSlopeY2);
        maxSlopeY2 = MAX_Y_SLOPE_VAL - 1;
    }

    //sensor table Length
    uint8_t senX2 = pRcv[PD_IDX_SEN_LEN_X2];
    uint8_t senY2 = pRcv[PD_IDX_SEN_LEN_Y2];
#if 1 //(SECOND_OFST_TBL == 1) //nsmoon@190624
    if (senX2 != 0) {
        TRACE_ERROR("ERROR! invalid senX2: %d", senX2);
        senX2 = 0;
    }
    if (senY2 != 0) {
        TRACE_ERROR("ERROR! invalid senY2: %d", senY2);
        senY2 = 0;
    }
#else
    if (senX2 >= MAX_NUM_2ND_SENSOR_X) {
        TRACE_ERROR("ERROR! invalid senX2: %d", senX2);
        senX2 = MAX_NUM_2ND_SENSOR_X - 1;
    }
    if (senY2 >= MAX_NUM_2ND_SENSOR_Y) {
        TRACE_ERROR("ERROR! invalid senY2: %d", senY2);
        senY2 = MAX_NUM_2ND_SENSOR_Y - 1;
    }
#endif
    nextIdx = PD_IDX_POS_START;
#endif
#else
    ofs_tbl_t offsetTblX, offsetTblY;
    //offset table Length
    uint8_t lenX = pRcv[PD_IDX_OFST_LEN_X];
    if (lenX < 10)
    {
        TRACE_ERROR("ERROR! set_ofst_data..invalid lenX %d", lenX);
        return;
        //lenX = MAX_NUM_SLOPES_DLT_X;
    }

    uint8_t lenY = pRcv[PD_IDX_OFST_LEN_Y];
    if (lenY < 10)
    {
        TRACE_ERROR("ERROR! set_ofst_data..invalid lenX %d", lenX);
        return;
        //lenX = MAX_NUM_SLOPES_DLT_X;
    }

    nextIdx = PD_IDX_OFST_X;
    for (i = 0; i < lenX; i++)
    {
        idx = nextIdx + i;
        offset_table_x[i] = pRcv[idx];
#if (DEBUG_set_ofst_data == 1)
        TRACENR("%d ", offset_table_x[i]);
#endif
    }
    offsetTblX.tbl = (int8_t *)&offset_table_x[0];
    offsetTblX.len = lenX;
#if (DEBUG_set_ofst_data == 1)
    TRACE("=>lenX: %d", lenX);
#endif

    //Y offset table
    nextIdx = idx + 1;
    for (i = 0; i < lenY; i++)
    {
        idx = nextIdx + i;
        offset_table_y[i] = pRcv[idx];
#if (DEBUG_set_ofst_data == 1)
        TRACENR("%d ", offset_table_y[i]);
#endif
    }
    offsetTblY.tbl = (int8_t *)&offset_table_y[0];
    offsetTblY.len = lenY;
#if (DEBUG_set_ofst_data == 1)
    TRACE("=>lenY: %d", lenY);
#endif
    nextIdx = idx + 1;
#endif //1

#if (DEBUG_set_ofst_data == 1)
    TRACE("nextIdx = %d", nextIdx);
#endif
    //X led pos (bottom)
    for (i = 0; i < disp.nMaxPdX; i++)
    {
        tmpX = pRcv[nextIdx + 0] | (pRcv[nextIdx + 1] << 8) | (pRcv[nextIdx + 2] << 16) | (pRcv[nextIdx + 3] << 24);
        s_led_pos_x[i] = *(float *)&tmpX;
        //TRACENR("%02x(%0.2f) ", tmpX, led_pos_x__[i]);
        nextIdx += 4;
    }
    //TRACENR("\r\n");
#if (DEBUG_set_ofst_data > 0)
    TRACE("led_pos_x__: %f, %f, %f", led_pos_x__[0], led_pos_x__[1], led_pos_x__[disp.nMaxPdX - 1]);
#endif

    //X pd pos (bottom)
    for (i = 0; i < disp.nMaxPdX; i++)
    {
        tmpX = pRcv[nextIdx + 0] | (pRcv[nextIdx + 1] << 8) | (pRcv[nextIdx + 2] << 16) | (pRcv[nextIdx + 3] << 24);
        //TRACENR("%x ", tmpX);
        s_pd_pos_x[i] = *(float *)&tmpX;
        nextIdx += 4;
    }
    //TRACENR("\r\n");
#if (DEBUG_set_ofst_data > 0)
    TRACE("pd_pos_x__: %f, %f, %f", pd_pos_x__[0], pd_pos_x__[1], pd_pos_x__[disp.nMaxPdX - 1]);
#endif

    //Y led pos (left)
    for (i = 0; i < disp.nMaxPdY; i++)
    {
        tmpY = pRcv[nextIdx + 0] | (pRcv[nextIdx + 1] << 8) | (pRcv[nextIdx + 2] << 16) | (pRcv[nextIdx + 3] << 24);
        //TRACENR("%x ", tmpY);
        s_led_pos_y[i] = *(float *)&tmpY;
        nextIdx += 4;
    }
    //TRACENR("\r\n");
#if (DEBUG_set_ofst_data > 0)
    TRACE("led_pos_y__: %f, %f, %f", led_pos_y__[0], led_pos_y__[1], led_pos_y__[disp.nMaxPdY - 1]);
#endif

    //Y pd pos (right)
    for (i = 0; i < disp.nMaxPdY; i++)
    {
        tmpY = pRcv[nextIdx + 0] | (pRcv[nextIdx + 1] << 8) | (pRcv[nextIdx + 2] << 16) | (pRcv[nextIdx + 3] << 24);
        //TRACENR("%x ", tmpY);
        s_pd_pos_y[i] = *(float *)&tmpY;
        nextIdx += 4;
    }
    //TRACENR("\r\n");
#if (DEBUG_set_ofst_data > 0)
    TRACE("pd_pos_y__: %f, %f, %f", pd_pos_y__[0], pd_pos_y__[1], pd_pos_y__[disp.nMaxPdY - 1]);
#endif

#if 1 //def FRONTEND_AAREA //nsmoon@190325
    //sensor X zero
    tmpY = pRcv[nextIdx + 0] | (pRcv[nextIdx + 1] << 8) | (pRcv[nextIdx + 2] << 16) | (pRcv[nextIdx + 3] << 24);
    s_sensor_zero_x = *(float *)&tmpY;
    nextIdx += 4;
    //sensor X end
    tmpY = pRcv[nextIdx + 0] | (pRcv[nextIdx + 1] << 8) | (pRcv[nextIdx + 2] << 16) | (pRcv[nextIdx + 3] << 24);
    s_sensor_end_x = *(float *)&tmpY;
    nextIdx += 4;
    //sensor Y zero
    tmpY = pRcv[nextIdx + 0] | (pRcv[nextIdx + 1] << 8) | (pRcv[nextIdx + 2] << 16) | (pRcv[nextIdx + 3] << 24);
    s_sensor_zero_y = *(float *)&tmpY;
    nextIdx += 4;
    //sensor Y end
    tmpY = pRcv[nextIdx + 0] | (pRcv[nextIdx + 1] << 8) | (pRcv[nextIdx + 2] << 16) | (pRcv[nextIdx + 3] << 24);
    s_sensor_end_y = *(float *)&tmpY;
    nextIdx += 4;
    //active area X zero
    tmpY = pRcv[nextIdx + 0] | (pRcv[nextIdx + 1] << 8) | (pRcv[nextIdx + 2] << 16) | (pRcv[nextIdx + 3] << 24);
    s_aarea_zero_x = *(float *)&tmpY;
    nextIdx += 4;
    //active area X end
    tmpY = pRcv[nextIdx + 0] | (pRcv[nextIdx + 1] << 8) | (pRcv[nextIdx + 2] << 16) | (pRcv[nextIdx + 3] << 24);
    s_aarea_end_x = *(float *)&tmpY;
    nextIdx += 4;
    //active area Y zero
    tmpY = pRcv[nextIdx + 0] | (pRcv[nextIdx + 1] << 8) | (pRcv[nextIdx + 2] << 16) | (pRcv[nextIdx + 3] << 24);
    s_aarea_zero_y = *(float *)&tmpY;
    nextIdx += 4;
    //active area Y end
    tmpY = pRcv[nextIdx + 0] | (pRcv[nextIdx + 1] << 8) | (pRcv[nextIdx + 2] << 16) | (pRcv[nextIdx + 3] << 24);
    s_aarea_end_y = *(float *)&tmpY;
    nextIdx += 4;
#else
    tmpX = s_pd_pos_x[disp.nMaxPdX - 1] + s_pd_pos_x[0];
    tmpY = s_pd_pos_y[disp.nMaxPdY - 1] + s_pd_pos_y[0];
    s_sensor_end_x = s_led_pos_x[disp.nMaxPdX - 1] + s_led_pos_x[0];
    s_sensor_end_y = s_led_pos_y[disp.nMaxPdY - 1] + s_led_pos_y[0];
    s_sensor_end_x = (s_sensor_end_x >= tmpX) ? s_sensor_end_x : tmpX;
    s_sensor_end_y = (s_sensor_end_y >= tmpY) ? s_sensor_end_y : tmpY;
    s_aarea_end_x = s_sensor_end_x;
    s_aarea_end_y = s_sensor_end_y;
#endif
#if (DEBUG_set_ofst_data > 0)
    TRACE("s_sensor_end_x,s_sensor_end_y: %f, %f", s_sensor_end_x, s_sensor_end_y);
    TRACE("s_aarea_zero_x,s_aarea_end_x,s_aarea_zero_y,s_aarea_end_y: %f, %f, %f, %f", s_aarea_zero_x, s_aarea_end_x, s_aarea_zero_y, s_aarea_end_y);
#endif

#if 1 //nsmoon@191029
#if (DEAD_CELL_TBL == 1) //nsmoon@190624
    sen_tbl_t deadTblXled, deadTblXpd;
    sen_tbl_t deadTblYled, deadTblYpd;

    //X-led table for dead cell
    for (i = 0; i < deadLenXled; i++)
    {
        idx = nextIdx + i;
        deadCellHorLed[i] = pRcv[idx];
#if (DEBUG_set_ofst_data > 0)
        TRACENR("%d ", deadCellHorLed[i]);
#endif
    }
    deadTblXled.len = deadLenXled;
    deadTblXled.tbl = &deadCellHorLed[0];
#if (DEBUG_set_ofst_data > 0)
    TRACE("=>deadTblXled.len: %d", deadTblXled.len);
#endif
    nextIdx += deadLenXled;

    //X-pd table for dead cell
    for (i = 0; i < deadLenXpd; i++)
    {
        idx = nextIdx + i;
        deadCellHorPd[i] = pRcv[idx];
#if (DEBUG_set_ofst_data > 0)
        TRACENR("%d ", deadCellHorPd[i]);
#endif
    }
    deadTblXpd.len = deadLenXpd;
    deadTblXpd.tbl = &deadCellHorPd[0];
#if (DEBUG_set_ofst_data > 0)
    TRACE("=>deadTblXpd.len: %d", deadTblXpd.len);
#endif
    nextIdx += deadLenXpd;

    //Y-led table for dead cell
    for (i = 0; i < deadLenYled; i++)
    {
        idx = nextIdx + i;
        deadCellVerLed[i] = pRcv[idx];
#if (DEBUG_set_ofst_data > 0)
        TRACENR("%d ", deadCellVerLed[i]);
#endif
    }
    deadTblYled.len = deadLenYled;
    deadTblYled.tbl = &deadCellVerLed[0];
#if (DEBUG_set_ofst_data > 0)
    TRACE("=>deadTblYled.len: %d", deadTblYled.len);
#endif
    nextIdx += deadLenYled;

    //Y-pd table for dead cell
    for (i = 0; i < deadLenYpd; i++)
    {
        idx = nextIdx + i;
        deadCellVerPd[i] = pRcv[idx];
#if (DEBUG_set_ofst_data > 0)
        TRACENR("%d ", deadCellVerPd[i]);
#endif
    }
    deadTblYpd.len = deadLenYpd;
    deadTblYpd.tbl = &deadCellVerPd[0];
#if (DEBUG_set_ofst_data > 0)
    TRACE("=>deadTblYpd.len: %d", deadTblYpd.len);
#endif
    nextIdx += deadLenYpd;

#endif //DEAD_CELL_TBL

    sen_tbl_t senTblX2, senTblY2; //reserved
#if (SECOND_OFST_TBL == 0) //nsmoon@190624
    if (senX2 > 0 || senY2 > 0) {
        TRACE_ERROR("ERROR! invalid senX2,senY2 %d %d", senX2,senY2);
        senX2 = senY2 = 0;
    }
#endif
#if (SECOND_OFST_TBL == 1) //nsmoon@190624
    //sen_tbl_t senTblX2, senTblY2;
    //X-led table for 2nd ofst
    for (i = 0; i < senX2; i++)
    {
        idx = nextIdx + i;
        sensor_table_x2[i] = pRcv[idx];
#if (DEBUG_set_ofst_data > 0)
        TRACENR("%d ", sensor_table_x2[i]);
#endif
    }
    senTblX2.len = senX2;
    senTblX2.tbl = &sensor_table_x2[0];
#if (DEBUG_set_ofst_data > 0)
    TRACE("=>senTblX2.len: %d", senTblX2.len);
#endif
    nextIdx += senX2;

    //Y-led table for 2nd ofst
    for (i = 0; i < senY2; i++)
    {
        idx = nextIdx + i;
        sensor_table_y2[i] = pRcv[idx];
#if (DEBUG_set_ofst_data > 0)
        TRACENR("%d ", sensor_table_y2[i]);
#endif
    }
    senTblY2.len = senY2;
    senTblY2.tbl = &sensor_table_y2[0];
#if (DEBUG_set_ofst_data > 0)
    TRACE("=>senTblY2.len: %d", senTblY2.len);
#endif
    nextIdx += senY2;
#endif //SECOND_OFST_TBL
#endif //1
#if 0 //nsmoon@210112
    if (TEST_PHYSICAL_X_MAX != s_sensor_end_x) {
        TRACE_RELEASE("ERROR! invalid TEST_PHYSICAL_X_MAX: %d(%d)", TEST_PHYSICAL_X_MAX, s_sensor_end_x);
    }
    else {
        TRACE_RELEASE("TEST_PHYSICAL_X_MAX: %d(%d)", TEST_PHYSICAL_X_MAX, s_sensor_end_x);
    }
    if (TEST_PHYSICAL_Y_MAX != s_sensor_end_y) {
        TRACE_RELEASE("ERROR! invalid TEST_PHYSICAL_Y_MAX: %d(%d)", TEST_PHYSICAL_Y_MAX, s_sensor_end_y);
    }
    else {
        TRACE_RELEASE("TEST_PHYSICAL_Y_MAX: %d(%d)", TEST_PHYSICAL_Y_MAX, s_sensor_end_y);
    }
    if ((int)(TEST_PHYSICAL_X_MAX*1.6) != TEST_LOGICAL_X_MAX) {
        TRACE_RELEASE("ERROR! invalid TEST_LOGICAL_X_MAX: %d(%d)", TEST_LOGICAL_X_MAX, (int)(TEST_PHYSICAL_X_MAX*1.6));
    }
    else {
        TRACE_RELEASE("TEST_LOGICAL_X_MAX: %d", TEST_LOGICAL_X_MAX);
    }
    if ((int)(TEST_PHYSICAL_Y_MAX*1.6) != TEST_LOGICAL_Y_MAX) {
        TRACE_RELEASE("ERROR! invalid TEST_LOGICAL_Y_MAX: %d(%d)", TEST_LOGICAL_Y_MAX, (int)(TEST_PHYSICAL_Y_MAX*1.6));
    }
    else {
        TRACE_RELEASE("TEST_LOGICAL_Y_MAX: %d", TEST_LOGICAL_Y_MAX);
    }
#endif

    s_logical_max_x = TEST_LOGICAL_X_MAX;
    s_logical_max_y = TEST_LOGICAL_Y_MAX;
    offsetTblLenX = (maxSlopeX * 2) + 1;
    offsetTblLenY = (maxSlopeY * 2) + 1;

    tp_layout_info.sensor_zero_x = s_sensor_zero_x;
    tp_layout_info.sensor_end_x = s_sensor_end_x;
    tp_layout_info.sensor_zero_y = s_sensor_zero_y;
    tp_layout_info.sensor_end_y = s_sensor_end_y;
    tp_layout_info.aarea_zero_x = s_aarea_zero_x;
    tp_layout_info.aarea_end_x = s_aarea_end_x;
    tp_layout_info.aarea_zero_y = s_aarea_zero_y;
    tp_layout_info.aarea_end_y = s_aarea_end_y;
    tp_layout_info.pd_pos_x = (float *)&s_pd_pos_x[0];
    tp_layout_info.pd_pos_y = (float *)&s_pd_pos_y[0];
    tp_layout_info.led_pos_x = (float *)&s_led_pos_x[0];
    tp_layout_info.led_pos_y = (float *)&s_led_pos_y[0];
    tp_layout_info.hor_pd_num = disp.nMaxPdX;
    tp_layout_info.ver_pd_num = disp.nMaxPdY;
    tp_layout_info.maxOfstValX = maxSlopeX;
    tp_layout_info.maxOfstValY = maxSlopeY;
    tp_layout_info.maxOfstValX2 = maxSlopeX2;
    tp_layout_info.maxOfstValY2 = maxSlopeY2;
    tp_layout_info.senTblX2 = senTblX2;
    tp_layout_info.senTblY2 = senTblY2;
    tp_layout_info.logical_x_size = s_logical_max_x;
    tp_layout_info.logical_y_size = s_logical_max_y;

#if (DEAD_CELL_TBL == 1) //nsmoon@190829
    testDeadCell.hor_dead_pd = deadTblXpd.tbl;
    testDeadCell.hor_dead_led = deadTblXled.tbl;
    testDeadCell.ver_dead_pd = deadTblYpd.tbl;
    testDeadCell.ver_dead_led = deadTblYled.tbl;
#ifdef TEST_DEAD_CELL_TBL //for test
    testDeadCell.hor_pd_len = 1;
    testDeadCell.hor_led_len = 1;
    testDeadCell.ver_pd_len = 1;
    testDeadCell.ver_led_len = 1;
    testDeadCell.hor_dead_pd[0] = 10;
    testDeadCell.hor_dead_led[0] = 11;
    testDeadCell.ver_dead_pd[0] = 20;
    testDeadCell.ver_dead_led[0] = 21;
#else
    testDeadCell.hor_pd_len = deadTblXpd.len;
    testDeadCell.hor_led_len = deadTblXled.len;
    testDeadCell.ver_pd_len = deadTblYpd.len;
    testDeadCell.ver_led_len = deadTblYled.len;
#endif
#endif

#if 1 //for testing
    TRACE_RELEASE("g_back_conf.numPdX,,numPdY=%d, %d", disp.nMaxPdX, disp.nMaxPdY);
    TRACE_RELEASE("g_back_conf.numOfsX,slope_byte_x,slope_byte_y=(%d,%d)(%d,%d)", maxSlopeX, maxSlopeY, slope_byte_x, slope_byte_y);
    TRACE_RELEASE("s_sensor_end_x,sensor_end_y=%0.2f, %0.2f", s_sensor_end_x, s_sensor_end_y);
    TRACE_RELEASE("s_aarea_zero_x,s_aarea_zero_y=%0.2f, %0.2f", s_aarea_zero_x, s_aarea_zero_y);
    TRACE_RELEASE("s_aarea_end_x,s_aarea_end_y=%0.2f, %0.2f", s_aarea_end_x, s_aarea_end_y);
    TRACE_RELEASE("s_pd_pos_x[0],s_pd_pos_y[0]=%0.2f, %0.2f", s_pd_pos_x[0], s_pd_pos_y[0]);
    TRACE_RELEASE("s_led_pos_x[0],s_led_pos_y[0]=%0.2f, %0.2f", s_led_pos_x[0], s_led_pos_y[0]);
    TRACE_RELEASE("s_pd_pos_x[n],s_pd_pos_y[n]=%f, %f", s_pd_pos_x[disp.nMaxPdX - 1], s_pd_pos_y[disp.nMaxPdY - 1]);
    TRACE_RELEASE("s_led_pos_x[n],s_led_pos_y[n]=%f, %f", s_led_pos_x[disp.nMaxPdX - 1], s_led_pos_y[disp.nMaxPdY - 1]);
#endif

    s_init_post_processing();

#if 1 //nsmoon@191029
#if (DEAD_CELL_TBL == 1) //nsmoon@190829
    int ret = BG_init_backend(&tp_layout_info, &testDeadCell, 0);
    if (ret) {
        TRACE_ERROR("ERROR! BG_init_backend..", ret);
    }
#else
    BG_init_backend(&tp_layout_info, 0, 0);
#endif
#else
    BG_init_backend(disp.nMaxPdX, disp.nMaxPdY, &offsetTblX, &offsetTblY, 0, &tp_layout_info, 0, TEST_LOGICAL_X_MAX, TEST_LOGICAL_Y_MAX);
#endif
    get_slope_size();
#if (DEBUG_set_ofst_data == 1)
    TRACE("offsetTblLenX,offsetTblLenY,slope_byte_x,slope_byte_y: %d,%d %d,%d", offsetTblLenX, offsetTblLenY, slope_byte_x, slope_byte_y);
#endif
    total = PD_DATA_IDX2 + (disp.nMaxPdX * slope_byte_x) + (disp.nMaxPdY * slope_byte_y);
#ifdef FRONTEND_LINE_THRESHOLD
    total += (disp.nMaxPdX * slope_byte_x) + (disp.nMaxPdY * slope_byte_y);
#endif
    TRACE("total=%d", total);

    num_of_dlt_pkt = (total + USB_PACKET_SIZE - 1) / USB_PACKET_SIZE;
    bInitOfst = true;
#if (BRUSH_MODE_ENABLE == 1)
    testNextScan.brushMode = 0; //0: disable brush mode
#endif
}

void MainWindow::setScreenSize(void)
{
    disp.nWidth = this->scene->width();
    disp.nHeight = this->scene->height();
}

void MainWindow::initDispParm(void)
{
    disp.ratioX = disp.nWidth / (s_sensor_end_x + MARGIN_LEFT * 2);
    disp.ratioY = disp.nHeight / (s_sensor_end_y + MARGIN_TOP * 2);
}

int MainWindow::drawPdData(axis_t axis, int pd, int offSetIdx, int pdIdx)
{
    int led; //maxPd
#if 1 //nsmoon@201012
    int maxSlopeIdx = (axis == ENUM_HOR_X) ? offsetTblLenX : offsetTblLenY;
#else
    int maxSlopeIdx = (axis == ENUM_HOR_X) ? MAX_X_SLOPE_IDX : MAX_Y_SLOPE_IDX;
#endif
    if (offSetIdx < 0 || offSetIdx >= maxSlopeIdx) {
        TRACE_ERROR("ERROR! invalid offSetIdx= %d", offSetIdx);
        return 0; //error
    }

    if (axis == ENUM_HOR_X) {
        //led = pd + BS_offsetTblX[offSetIdx];
        led = pd + (offSetIdx - maxSlopeX);
        if (led >= 0 && led < disp.nMaxPdX)
        {
            //TRACE("drawPdData...pd,led,x1,y1,x2,y2: %d, %d, %0.1f, %0.1f, %0.1f, %0.1f", pd, led, posX(BS_pd_pos_x[pd]), posY(BS_sensor_end_y), posX(BS_led_pos_x[led]), posY(BS_sensor_zero_y));
            draw_line(posX(s_pd_pos_x[pd]), posY(s_sensor_end_y), posX(s_led_pos_x[led]), posY(s_sensor_zero_y), 0);
            TRACE_NOP;
        }
        else {
            if (!bDispInverse) {
                TRACE_ERROR("ERROR! drawPdData...invalid X led %d", led);
                return 0; //error
            }
        }
    }
    else {
        //y-axis
        //led = pd + BS_offsetTblY[offSetIdx];
        led = pd + (offSetIdx - maxSlopeY);
        if (led >= 0 && led < disp.nMaxPdY)
        {
            draw_line(posX(s_sensor_zero_x), posY(s_pd_pos_y[pd]), posX(s_sensor_end_x), posY(s_led_pos_y[led]), 0);
        }
        else {
            if (!bDispInverse) {
                TRACE_ERROR("ERROR! drawPdData...invalid Y led %d", led);
                return 0; //error
            }
        }
    }
    return 1; //no-error
}

void MainWindow::remove_draw_line(void)
{
#if 0 //for test
    this->scene->clear();
    this->view->viewport()->update();
#else
    int cnt = 0;
    foreach(QGraphicsLineItem* item, lineLst)
    {
        if (item != Q_NULLPTR) {
            this->scene->removeItem(item);
            cnt++;
        }
    }
#endif
    //TRACE("remove_draw_line: %d", cnt);
    lineLst.clear();
}

void MainWindow::draw_line(float x0, float y0, float x1, float y1, unsigned long color__)
{
    QPen pen;
    QColor color = GetDisplayColor(color__);

    //pen.setColor("black");
    pen.setColor(color);
    pen.setWidth(0);

    //scene->addLine(QLineF(x0, y0, x1, y1), pen);
    lineLst.push_back(scene->addLine(QLineF(x0, y0, x1, y1), pen));
    lineLst.last()->setVisible(true);
    //TRACE("i,eTypeDebug: %d %d", i, eTypeDebug);
}

bool MainWindow::drawOutPoly() {
    //setScreenSize();
    //initDispParm();
    remove_draw_line();

    float x1, x2, y1, y2;
    int i, j, k;

    draw_axis();

    //TRACE("drawOutMy()..%d", polgon_saved_idx);
    for (j = 0; j < polgon_saved_idx; j++)
    {
        //TRACE("..len: (%d) %d", j, polygon_saved[j].len);
        for (i = 0; i < polygon_saved[j].len; i++) {
            if (polygon_saved[j].len == 2 && i > 0) break;
            k = (i < polygon_saved[j].len - 1) ? (i + 1) : (0);
            //TRACE("drawOutPoly...1: %d, %d, %d, %0.1f, %0.1f, %0.1f, %0.1f", j, i, k, polygon_saved[j].vert[i].x, polygon_saved[j].vert[i].y, polygon_saved[j].vert[j].x, polygon_saved[j].vert[j].y);
            x1 = posX(polygon_saved[j].vert[i].x);
            y1 = posY(polygon_saved[j].vert[i].y);
            y2 = posY(polygon_saved[j].vert[k].y);
            x2 = posX(polygon_saved[j].vert[k].x);
            //TRACE("drawOutPoly...2: %d, %d, %d, %0.1f, %0.1f, %0.1f, %0.1f", j, i, k, x1, y1, x2, y2);
            draw_line(x1, y1, x2, y2, polygon_saved[j].color);
        }
    }
    bDrawOutPoly = false;
    return true;
}

bool MainWindow::threadSlotDrawOutPoly(float x0, float y0, float x1, float y1, unsigned long color__, int wakeStatus) {
    //setScreenSize();
    //initDispParm();
    float x2, x3, y2, y3;

    x2 = posX(x0);
    x3 = posX(x1);
    y2 = posY(y0);
    y3 = posY(y1);

    remove_draw_line();

    draw_axis();

    draw_line(x2, y2, x3, y3, color__);

    if (!wakeStatus) return true;

    if(draw_pause == 1) {
        if (polgon_draw_idx == polgon_saved_idx) {
            waitStatusForWaitThread = 1;
        }
    }

    return true;
}

bool MainWindow::threadSlotDrawOutOnePoly(float x0, float y0, float x1, float y1, unsigned long color__, int wakeStatus) {

    float x2, x3, y2, y3;

    x2 = posX(x0);
    x3 = posX(x1);
    y2 = posY(y0);
    y3 = posY(y1);

    draw_line(x2, y2, x3, y3, color__);

    if (!wakeStatus) return true;

    if(draw_pause == 1) {
        if (polgon_draw_idx == polgon_saved_idx) {
            waitStatusForWaitThread = 1;
        }
    }

    return true;
}

float MainWindow::posX(float pos)
{
    float x = ((s_sensor_end_x - pos) + MARGIN_LEFT) * disp.ratioX;
    if (x < 0 || x >= disp.nWidth) {
        TRACE_ERROR("ERROR! invalid x %0.2f", x);
    }

    if (x < 0) {
        x = 0;
    }
    else if (x >= disp.nWidth) {
        x = (float)disp.nWidth - 1;
    }
    return (x);
}

float MainWindow::posY(float pos)
{
    //float y = (pos + MARGIN_TOP) * disp.ratioY;
    float y = ((s_sensor_end_y - pos) + MARGIN_TOP) * disp.ratioY;
    if (y < 0 || y >= disp.nHeight) {
        //TRACE("y= %0.2f %0.2f %0.2f %0.2f", y, s_sensor_end_y, pos, disp.ratioY);
        TRACE_ERROR("ERROR! invalid y %0.2f", y);
    }

    if (y < 0) {
        y = 0;
    }
    else if (y >= disp.nHeight) {
        y = (float)disp.nHeight - 1;
    }
    return (y);
}

#if 1 //def DLT_ALL_SLOPE_FORMAT //nsmoon@170828a
bool MainWindow::draw_axis(void)
{
    // Draw a grid background.
    unsigned long line_color;
    //float x1, y1, x2, y2;

    line_color = MY_COLOR - 2; //green
    draw_line(posX(s_aarea_zero_x), posY(s_aarea_zero_y), posX(s_aarea_zero_x), posY(s_aarea_end_y), line_color); //vertical left
    draw_line(posX(s_aarea_zero_x), posY(s_aarea_end_y), posX(s_aarea_end_x), posY(s_aarea_end_y), line_color);   //horizontal top
    draw_line(posX(s_aarea_end_x), posY(s_aarea_end_y), posX(s_aarea_end_x), posY(s_aarea_zero_y), line_color);   //vertical right
    draw_line(posX(s_aarea_end_x), posY(s_aarea_zero_y), posX(s_aarea_zero_x), posY(s_aarea_zero_y), line_color); //horizontal bottom

    line_color = 0; //black
    draw_line(posX(s_sensor_zero_x), posY(s_sensor_zero_y), posX(s_sensor_zero_x), posY(s_sensor_end_y), line_color); //vertical left
    draw_line(posX(s_sensor_zero_x), posY(s_sensor_end_y), posX(s_sensor_end_x), posY(s_sensor_end_y), line_color);   //horizontal top
    draw_line(posX(s_sensor_end_x), posY(s_sensor_end_y), posX(s_sensor_end_x), posY(s_sensor_zero_y), line_color);   //vertical right
    draw_line(posX(s_sensor_end_x), posY(s_sensor_zero_y), posX(s_sensor_zero_x), posY(s_sensor_zero_y), line_color); //horizontal bottom

    for (int i = 0; i < disp.nMaxPdX; i++) {
        if (i % GROUP_SIZE == 0) {
            draw_line(posX(s_led_pos_x[i]), posY(s_sensor_zero_y),  posX(s_led_pos_x[i]), (posY(s_sensor_zero_y - MARGIN_TOP / 2)), line_color);
            draw_line(posX(s_pd_pos_x[i]), posY(s_sensor_end_y), posX(s_pd_pos_x[i]), (posY(s_sensor_end_y + MARGIN_TOP / 2)), line_color);
        }
        else {
            draw_line(posX(s_led_pos_x[i]), posY(s_sensor_zero_y), posX(s_led_pos_x[i]), (posY(s_sensor_zero_y - MARGIN_TOP / 3)), line_color);
            draw_line(posX(s_pd_pos_x[i]), posY(s_sensor_end_y), posX(s_pd_pos_x[i]), (posY(s_sensor_end_y + MARGIN_TOP / 3)), line_color);
        }
    }
    //TRACE(" s_sensor_end_x, s_sensor_pos_x[%d]: %f,%f", disp.nMaxPdX - 1, s_sensor_end_x, s_sensor_pos_x[disp.nMaxPdX - 1]);

    for (int i = 0; i < disp.nMaxPdY; i++) {
#if 0
        if (i == 0) {
            line_color = 0;
        }
        else {
            line_color = RGB(255, 0, 0); //set_color((((i - 1) / SUBGROUP_SIZE) % SUBGROUP_SIZE) + 1);
        }
#endif
        if (i % GROUP_SIZE == 0) {
            draw_line(posX(s_sensor_end_x), posY(s_led_pos_y[i]), posX(s_sensor_end_x + MARGIN_TOP / 2), posY(s_led_pos_y[i]), line_color);
            draw_line(posX(s_sensor_zero_x), posY(s_pd_pos_y[i]), posX(s_sensor_zero_x - MARGIN_TOP / 2), posY(s_pd_pos_y[i]), line_color);
        }
        else {
            draw_line(posX(s_sensor_end_x), posY(s_led_pos_y[i]), posX(s_sensor_end_x + MARGIN_TOP / 3), posY(s_led_pos_y[i]), line_color);
            draw_line(posX(s_sensor_zero_x), posY(s_pd_pos_y[i]), posX(s_sensor_zero_x - MARGIN_TOP / 3), posY(s_pd_pos_y[i]), line_color);
        }
    }
    //TRACE(" s_sensor_end_y, s_sensor_pos_y[%d]: %f,%f", disp.nMaxPdY - 1, s_sensor_end_y, s_sensor_pos_y[disp.nMaxPdY - 1]);

    return(true);
}
#endif

int MainWindow::makeInputBuffer(axis_t axis, int pd, int offSetIdx, int pdIdx)
{
    int led, len = 0; //maxPd

    if (axis == ENUM_HOR_X) {
        //led = pd + BS_offsetTblX[offSetIdx];
        led = pd + (offSetIdx - maxSlopeX);
        if (led >= 0 && led < disp.nMaxPdX)
        {
            if (pdIdx < MAX_HOR_TOUCH_PD_WIN32) {
                testInputBuffer.hor_touch_pd[pdIdx].pd = (uint16_t)pd;
                testInputBuffer.hor_touch_pd[pdIdx].led = (uint16_t)offSetIdx; //slope index
                len++;
            }
            else {
                TRACE_ERROR("ERROR! invalid hor len %d", pdIdx);
            }
        }
        else {
            //TRACE_ERROR("ERROR! drawPdData...invalid X led %d", led);
        }
    }
    else {
        //y-axis
        //led = pd + BS_offsetTblY[offSetIdx];
        led = pd + (offSetIdx - maxSlopeY);
        if (led >= 0 && led < disp.nMaxPdY)
        {
            if (pdIdx < MAX_VER_TOUCH_PD_WIN32) {
                testInputBuffer.ver_touch_pd[pdIdx].pd = (uint16_t)pd;
                testInputBuffer.ver_touch_pd[pdIdx].led = (uint16_t)offSetIdx; //slope index
                len++;
            }
            else {
                TRACE_ERROR("ERROR! invalid ver len %d", pdIdx);
            }
        }
        else {
            //TRACE_ERROR("ERROR! drawPdData...invalid Y led %d", led);
        }
    }
    return len;
}

#if 0 //(SECOND_OFST_TBL == 1) //nsmoon@190625
#define DEBUG_ofst2_sensor_deadline		0
int MainWindow::conv_pd_to_byteidx(axis_t axis, uint8_t *buff, in_line_t *line, in_dlt_buf_idx_t *bufIdx)
{
    int slopeValMax, slopeMaxIdx;
    int slopeVal, slopeIdx;
    int slopeByte, buffStIdx;

    if (axis == ENUM_HOR_X) {
        slopeValMax = maxSlopeX;
        slopeMaxIdx = offsetTblLenX;
        slopeByte = slope_byte_x;
    }
    else {
        slopeValMax = BS_slopeValMaxY;
        slopeMaxIdx = BS_offsetTblLenY;
        slopeByte = slope_byte_y;
    }

    buffStIdx = line->pd * slopeByte;
    slopeVal = line->led - line->pd;
    slopeIdx = slopeValMax + slopeVal;
    bufIdx->byteIdx = slopeIdx / 8;
    bufIdx->bitIdx = slopeIdx % 8;

    return 0;
}

int MainWindow::is_blocked_line_in_buff(axis_t axis, uint8_t *buff, int pd, int led)
{
    //int slopeValMax, slopeMaxIdx;
    //int slopeIdx, slopeVal,
    int slopeByte, buffStIdx;
    in_line_t line;
    in_dlt_buf_idx_t bufIdx;

    slopeByte = (axis == ENUM_HOR_X) ? slope_byte_x : slope_byte_y;
    buffStIdx = pd * slopeByte;
    line.pd = pd;
    line.led = led;

    conv_pd_to_byteidx(axis, buff, &line, &bufIdx);
    if ((buff[buffStIdx + bufIdx.byteIdx] & bit_mask_8[bufIdx.bitIdx]) != 0x00) {
        //blocked
        return 1;
    }
    return 0; //not-blocked
}

int MainWindow::set_blocked_line_in_buff(axis_t axis, uint8_t *buff, int pd, int led)
{
    //int slopeValMax, slopeMaxIdx;
    //int slopeVal, slopeIdx;
    int slopeByte, buffStIdx;
    in_line_t line;
    in_dlt_buf_idx_t bufIdx;

    slopeByte = (axis == ENUM_HOR_X) ? slope_byte_x : slope_byte_y;
    buffStIdx = pd * slopeByte;
    line.pd = pd;
    line.led = led;

    conv_pd_to_byteidx(axis, buff, &line, &bufIdx);
    buff[buffStIdx + bufIdx.byteIdx] |= bit_mask_8[bufIdx.bitIdx];

    return 0;
}

int MainWindow::ofst2_sensor_deadline(axis_t axis, uint8_t *buff)
{
    int i, k, m;
    int pd, led, maxPd, stPd, edPd;
    int slopeValMax, slopeValMax2;
    uint8_t *senTbl2;
    int senTblLen2;

    if (axis == ENUM_HOR_X) {
        slopeValMax = maxSlopeX;
        slopeValMax2 = BS_slopeValMaxX2;
        maxPd = BS_maxHorPdNum;
        senTbl2 = &BS_senTblX2[0];
        senTblLen2 = BS_senTblLenX2;
    }
    else {
        slopeValMax = maxSlopeY;
        slopeValMax2 = BS_slopeValMaxY2;
        maxPd = BS_maxVerPdNum;
        senTbl2 = &BS_senTblY2[0];
        senTblLen2 = BS_senTblLenY2;
    }

    for (i = 0; i < senTblLen2; i++) {
        led = senTbl2[i];
#if 1 //nsmoon@190910
        for (pd = 0; pd < maxPd; pd++) {
            int slopeVal = GET_ABS(led - pd);
            if (slopeVal > slopeValMax2 && slopeVal <= slopeValMax) {
                //if (is_blocked_line_in_buff(axis, buff, pd, led))
                {
                    //blocked line
#if (DEBUG_ofst2_sensor_deadline > 0)
                    TRACE("pd-led: %d-%d", pd, led);
#endif
                    set_blocked_line_in_buff(axis, buff, pd, led);
                }
            }
        }
#else //for test
        for (k = 0; k < 2; k++) {
            //k=0: +slope, k=1: -slope
            pd = (k) ? (led - slopeValMax2) : (led + slopeValMax2);
            if (pd < 0 || pd >= maxPd) continue;
            if (is_blocked_line_in_buff(axis, buff, pd, led)) {
                //blocked line
#if (DEBUG_ofst2_sensor_deadline > 0)
                TRACE("pd-led: %d-%d", pd, led);
#endif
                stPd = (k) ? (led - slopeValMax) : (pd + 1);
                edPd = (k) ? (pd - 1) : (led + slopeValMax);
                for (m = stPd; m <= edPd; m++) {
                    set_blocked_line_in_buff(axis, buff, m, led);
                }
            }
        }
#endif
    } //for (i = 0; i < senTblLen2; i++)

    return 0; //not-found
}
#endif

#if (DEAD_CELL_TBL == 1) //nsmoon@190829
bool MainWindow::is_dead_cell(uint8_t *tbl, int len, int pd)
{
    uint8_t sen;
    for (int i = 0; i < len; i++) {
        sen = tbl[i];
        if (sen == pd) {
            return true;
        }
    }
    return false;
}
#endif

bool MainWindow::draw_pd_data(int nBufNumber, BOOLEAN bDraw)
{
    unsigned char *pRcv = HidReceiveBuf;
    int k, stIdx;
    int pd, len, nMaxPd, maxOfstIdx; //led
    //uint64_t slopeIdx, tmp64;
    //uint8_t offset_size = (TotalNumberOfSlopes / 2);
#if (DEAD_CELL_TBL == 1) //nsmoon@190926
    //int8_t *offsetTbl;
    uint8_t *deadCellPd, *deadCellLed;
    uint8_t deadCellPdLen, deadCellLedLen;
#endif
    int len_line_data_x = disp.nMaxPdX * slope_byte_x;
    int len_line_data_y = disp.nMaxPdY * slope_byte_y;

    //init test input buffer
    testInputBuffer.hor_len = 0;
    testInputBuffer.ver_len = 0;
    testInputBuffer.hor_touch_pd = &hor_touch_pd_win32[0];
    testInputBuffer.ver_touch_pd = &ver_touch_pd_win32[0];
#ifdef FRONTEND_LINE_THRESHOLD
    testInputBuffer.threshold_x = &hor_line_threshold_win32[0];
    testInputBuffer.threshold_y = &ver_line_threshold_win32[0];
#endif
    //testNextScan.currScan = g_scan_mode.currScan;

    //setScreenSize();
    //initDispParm(); //init prameter for display
    //remove_draw_line();

    if (bDraw) {
        draw_axis(); //draw x,y-axis
    }
    get_slope_size();

#if 1 //for test
    axis_t axis;
    int slope_byte, bufIdx;

    stIdx = PD_DATA_IDX2;
    len = 0;
    for (int m = 0; m < (disp.nMaxPdX + disp.nMaxPdY); m++)
    {
        if (m == disp.nMaxPdX) {
            //x-axis done, save and reset
            testInputBuffer.hor_len = len;
            stIdx = PD_DATA_IDX2 + len_line_data_x;
            len = 0;
        }

        if (m < disp.nMaxPdX) {
            //x-axis
            pd = m;
            axis = ENUM_HOR_X;
            slope_byte = slope_byte_x;
            nMaxPd = disp.nMaxPdX;
            maxOfstIdx = offsetTblLenX;
#if (DEAD_CELL_TBL == 1) //nsmoon@190926
            //offsetTbl = &BS_offsetTblX[0];
            deadCellPd = testDeadCell.hor_dead_pd;
            deadCellLed = testDeadCell.hor_dead_led;
            deadCellPdLen = testDeadCell.hor_pd_len;
            deadCellLedLen = testDeadCell.hor_led_len;
#endif
        }
        else {
            //y-xis
            pd = m - disp.nMaxPdX;
            axis = ENUM_VER_Y;
            slope_byte = slope_byte_y;
            nMaxPd = disp.nMaxPdY;
            maxOfstIdx = offsetTblLenY;
#if (DEAD_CELL_TBL == 1) //nsmoon@190926
            //offsetTbl = &BS_offsetTblY[0];
            deadCellPd = testDeadCell.ver_dead_pd;
            deadCellLed = testDeadCell.ver_dead_led;
            deadCellPdLen = testDeadCell.ver_pd_len;
            deadCellLedLen = testDeadCell.ver_led_len;
#endif
        }
        bufIdx = stIdx + (pd * slope_byte);
        pRcv = &HidReceiveBuf[bufIdx];
#if 0 //(SECOND_OFST_TBL == 1) //nsmoon@190625
        if (bDispInverse) {
            ofst2_sensor_deadline((axis_t)axis, &pRcv[stIdx]);
        }
#endif

        for (int byteIdx = 0; byteIdx < slope_byte; byteIdx++) {
            uint8_t tmp8 = pRcv[byteIdx];
#if 1 //nsmoon@190926
            //make input buffer
            if (tmp8) {
                for (int bitIdx = 0; bitIdx < MAX_SHIFT_TBL_BYTE; bitIdx++) {
                    if (tmp8 & bit_mask_8[bitIdx]) {
                        int ofstIdx = (byteIdx * 8) + bitIdx;
                        if (ofstIdx >= maxOfstIdx) continue;
                        if (makeInputBuffer((axis_t)axis, pd, ofstIdx, len)) {
                            len++;
                        }
                    }
                } //for (bitIdx = 0; bitIdx < 8; bitIdx++)
            }
#endif
#if (DEAD_CELL_TBL == 1) //nsmoon@190926
            if (is_dead_cell(deadCellPd, deadCellPdLen, pd)) {
                tmp8 = 0xff; //make dead cell
            }
#endif
            if (bDispInverse) {
                tmp8 = ~tmp8;
            }

#if (DEAD_CELL_TBL == 1) //nsmoon@190926
            int tmpCnt = 0;
            for (int bitIdx = 0; bitIdx < 8; bitIdx++) {
                int ofstIdx = (byteIdx * 8) + bitIdx;
                //int led = pd + offsetTbl[ofstIdx];
                int slopeValMax = (axis == ENUM_HOR_X) ? maxSlopeX : maxSlopeY;
                int led = pd + (ofstIdx - slopeValMax);
                if (led >= 0 && led < nMaxPd) {
                    if (is_dead_cell(deadCellLed, deadCellLedLen, led)) {
                        if (bDispInverse) {
                            tmp8 &= ~bit_mask_8[bitIdx];
                        }
                        else {
                            tmp8 |= bit_mask_8[bitIdx];
                        }
                        if (++tmpCnt == deadCellLedLen) {
                            break;
                        }
                    }
                }
                else {
                    //TRACE_ERROR("ERROR! draw_pd_data...invalid X led %d", led);
                    continue;
                }
            }
#endif //DEAD_CELL_TBL
#if (DRAW_LINE_TEST == 1)
            //draw line data
            if (tmp8) {
                //TRACE("pd,i: %d %d %02x", pd, byteIdx, tmp8);
                for (int bitIdx = 0; bitIdx < MAX_SHIFT_TBL_BYTE; bitIdx++) {
                    if (tmp8 & bit_mask_8[bitIdx]) {
                        int ofstIdx = (byteIdx * 8) + bitIdx;
                        //TRACE("bitIdx,ofstIdx: %d %d", bitIdx, ofstIdx);
                        if (ofstIdx >= maxOfstIdx) continue;
                        if (drawPdData(axis, pd, ofstIdx, len)) {
                            len++;
                        }
                    }
                }
            }
#endif
        }
    }
    testInputBuffer.ver_len = len;
    stIdx += len_line_data_y;

#ifdef FRONTEND_LINE_THRESHOLD
    if (stIdx != (PD_DATA_IDX2 + len_line_data_x + len_line_data_y)) {
        TRACE("ERROR! invalid stdIdx: %d, %d", stIdx, (len_line_data_x + len_line_data_y));
    }

    //X line-threshold
    int thCntX = 0, tIdxX = 0;
    for (pd = 0; pd < disp.nMaxPdX; pd++) {
        int bufIdx = stIdx + (pd * slope_byte_x);
        pRcv = &HidReceiveBuf[bufIdx];
        for (int i = 0; i < slope_byte_x; i++) {
            if (tIdxX < MAX_HOR_TOUCH_PD_WIN32) {
                hor_line_threshold_win32[tIdxX++] = pRcv[i];
                for (int j = 0; j < 8; j++) {
                    if ((pRcv[i] & (0x1 << j)) != 0) {
                        thCntX++;
                    }
                }
            }
            else {
                TRACE("ERROR! invalid tIdxX: %d", tIdxX);
            }
        }
    }
    if (len_line_data_x < MAX_HOR_TOUCH_PD_WIN32) {
    }
    else {
        TRACE("ERROR! invalid len_line_data_x: %d, %d", len_line_data_x, MAX_HOR_TOUCH_PD_WIN32);
    }
    stIdx += len_line_data_x;
    testInputBuffer.threshold_x_cnt = thCntX;

    //Y line-threshold
    int thCntY = 0, tIdxY = 0;
    for (pd = 0; pd < disp.nMaxPdY; pd++) {
        int bufIdx = stIdx + (pd * slope_byte_y);
        pRcv = &HidReceiveBuf[bufIdx];
        for (int i = 0; i < slope_byte_y; i++) {
            if (tIdxY < MAX_HOR_TOUCH_PD_WIN32) {
                ver_line_threshold_win32[tIdxY++] = pRcv[i];
                for (int j = 0; j < 8; j++) {
                    if ((pRcv[i] & (0x1 << j)) != 0) {
                        thCntY++;
                    }
                }
            }
            else {
                TRACE("ERROR! invalid tIdxX: %d", tIdxX);
            }
        }
    }
    stIdx += len_line_data_y;
    testInputBuffer.threshold_y_cnt = thCntY;
#if 0 //for test
    //TRACE("stIdx=%d", stIdx);
    if (thCntX > 0 || thCntY > 0) {
        TRACE("thCntX,thCntY=%d,%d", thCntX, thCntY);
    }
#endif //0
#endif
#else
    slope_mask_t slopeVal, slopeMask;
    // Draw X
    stIdx = PD_DATA_IDX2;
    len = 0;
    for (pd = 0; pd < disp.nMaxPdX; pd++) {
        get_slope_val(ENUM_HOR_X, stIdx, pd, pRcv, &slopeVal);
        if (slopeVal.upper || slopeVal.lower) {
            //TRACE("X,pd,slopeVal.upper,slopeVal.lower: %d, %016llx %016llx", stIdx + (pd * slope_byte), slopeVal.upper, slopeVal.lower);
            for (k = 0; k < offsetTblLenX; k++) {
                get_slope_mask(ENUM_HOR_X, k, &slopeMask);
                if (slopeVal.upper & slopeMask.upper || slopeVal.lower & slopeMask.lower) {
                    if (drawPdData(ENUM_HOR_X, pd, k, len)) {
                        len++;
                    }
                }
            }
        } //if (slopeVal.upper || slopeVal.lower)
    } //for (pd = 0; pd < disp.nMaxPdX; pd++) {
    testInputBuffer.hor_len = len;

    // Draw Y
    stIdx = PD_DATA_IDX2 + (disp.nMaxPdX * slope_byte_x);
    len = 0;
    for (pd = 0; pd < disp.nMaxPdY; pd++) {
        get_slope_val(ENUM_VER_Y, stIdx, pd, pRcv, &slopeVal);
        if (slopeVal.upper || slopeVal.lower) {
            //TRACE("Y,pd,slopeVal.upper,slopeVal.lower: %d, %016llx %016llx", stIdx + (pd * slope_byte), slopeVal.upper, slopeVal.lower);
            for (k = 0; k < BS_offsetTblLenY; k++) {
                get_slope_mask(ENUM_VER_Y, k, &slopeMask);
                if (slopeVal.upper & slopeMask.upper || slopeVal.lower & slopeMask.lower) {
                    if (drawPdData(ENUM_VER_Y, pd, k, len)) {
                        len++;
                    }
                }
            }
        } //if (slopeVal.upper || slopeVal.lower)
    } //for (pd = 0; pd < disp.nMaxPdX; pd++) {
    testInputBuffer.ver_len = len;
#endif

    return(true);
}

int MainWindow::RestoreDisplayPos(int x, int y, vec_t *pos)
{
    float tmpX = x / s_ratioXforP2L;
    float tmpY = y / s_ratioYforP2L;
    pos->x = (s_sensor_end_x - s_aarea_zero_x) - tmpX;
    pos->y = (s_sensor_end_y - s_aarea_zero_y) - tmpY;
    return 0; //no-error
}

QColor MainWindow::GetDisplayColor(unsigned long color)
{
#if 0 //move to mainwindow.h
    static QColor colorQt[MY_COLOR + 1] = {
        BGR_2_QRGB(0,0,0),        //0 //black
        BGR_2_QRGB(255,0,0),      //1 //blue
        BGR_2_QRGB(255,0,255),	//2 //magenta
        BGR_2_QRGB(192,192,192),	//3 //silver
        BGR_2_QRGB(128,128,128),	//4 //gray
        BGR_2_QRGB(0,0,128),      //5 //maroon
        BGR_2_QRGB(0,128,128),	//6 //oliver
        BGR_2_QRGB(0,128,0),      //7 //green
        BGR_2_QRGB(128,0,128),	//8 //purple
        BGR_2_QRGB(128,128,0),	//9 //teal
        BGR_2_QRGB(0,255,255),	//10 //yellow
        BGR_2_QRGB(255,255,0),	//11 //cyan
        BGR_2_QRGB(0,255,0),      //12 //lime
        BGR_2_QRGB(0,0,255)       //13 //red
    };
#endif
    if (color > MY_COLOR) {
        return qRgb(0,0,0); //0 //black
    }
    return colorQt[color];
}

void MainWindow::resetTouchStatus(void)
{
    for(int i = 0; i < (ALLOWABLE_TOUCH_BE*2); i++)
    {
        touchPoint[i].stat = 0;
        touchPointPre[i].stat = 0;
    }
    touchPointCnt = 0;
    touchPointPreCnt = 0;
}

void MainWindow::showDebugLabel(touch_point_t *tp)
{
    QString labelText[MAX_BG_DEBUG_MISC];
    int labelTextIdx = 0;

    //reset debug info
    for (int i = 0; i < MAX_BG_DEBUG_MISC; i++) {
        debugLabel[i]->setText("");
    }

#ifdef SHOW_DEBUG_SIZE
    labelText[0] = QString("+x:%1 -x:%2 x:%3").arg(s_debug_misc[0]).arg(s_debug_misc[1]).arg(s_debug_misc[2]);
    labelText[1] = QString("+y:%1 -y:%2 y:%3").arg(s_debug_misc[3]).arg(s_debug_misc[4]).arg(s_debug_misc[5]);
    //labelText[2] = QString("X:%1 Y:%2").arg(s_debug_misc[6]).arg(s_debug_misc[7]);
    //labelText[2] = QString("UsedMax:%1").arg(s_debug_misc[6]);
    labelText[2] = QString("(0) X:%1 Y:%2").arg(s_debug_misc[6]).arg(s_debug_misc[7]);
    labelText[3] = QString("(X) min:%1 max:%2").arg(s_debug_misc[8]).arg(s_debug_misc[9]);
    labelText[4] = QString("(Y) min:%1 max:%2").arg(s_debug_misc[10]).arg(s_debug_misc[11]);
    labelText[5] = QString("(X*Y) min:%1 max:%2").arg(s_debug_misc[12]).arg(s_debug_misc[13]);
    labelText[6] = QString("(X2) min:%1 max:%2").arg(s_debug_misc[14]).arg(s_debug_misc[15]);
    labelText[7] = QString("(Y2) min:%1 max:%2").arg(s_debug_misc[16]).arg(s_debug_misc[17]);
    labelText[8] = QString("total:%1 false:%2").arg(s_debug_misc[18]).arg(s_debug_misc[19]);
#endif
#ifdef SHOW_DEBUG_TRACKING
    labelText[0] = QString("framePeriod:%1").arg(s_debug_frameperiod);
    labelText[1] = QString("diffX:%1 diffY:%2").arg(s_debug_diff_x).arg(s_debug_diff_y);
#endif
#ifdef SHOW_DEBUG_SIZE_VAR
    labelText[0] = QString("Sq: %1 (%2, %3)").arg(s_debug_sizeSq).arg(s_debug_sizeX).arg(s_debug_sizeY);
    labelText[1] = QString("Type: %1 (%2, %3)").arg(s_debug_size_type).arg(touchPoint[0].xSize).arg(touchPoint[0].ySize);
    if (s_debug_curCnt) {
        labelText[2] = QString("SqCur: %1 (%2, %3)").arg(s_debug_sizeSqCur).arg(s_debug_sizeXcur).arg(s_debug_sizeYcur);
    }
    else {
        labelText[2] = QString("SqCur: ");
    }
    labelText[3] = QString("Tc: %1 (%2, %3)").arg(BG_touch_count).arg(BG_touch_size[0].xSize).arg(BG_touch_size[0].ySize);
#endif
#if 0 //def SHOW_DEBUG_SIZE_VAR
    labelText[0] = QString("Sq:%1  (%2)(%3)").arg(s_debug_sizeSq).arg(s_debug_sizeSqCur).arg(s_debug_sizeSqCur/s_debug_sizeSq);
    labelText[1] = QString("Dn:%1  Up:%2").arg(s_debug_down_skip_cnt).arg(s_debug_up_skip_cnt);
    labelText[2] = QString("Type:%1 (%2,%3)").arg(s_debug_size_type).arg(touchPoint[0].xSize).arg(touchPoint[0].ySize);
#endif
#if 0 //def SHOW_DEBUG_SIZE_DOWN
    labelText[0] = QString("Pos: A=%1 X=%2 Y=%3").arg(s_debug_insideArea).arg(s_debug_posXcur).arg(s_debug_posYcur);
    labelText[1] = QString("th50/10: X=%1/%3 Y=%2/%4").arg(s_debug_th50X).arg(s_debug_th50Y).arg(s_debug_th10X).arg(s_debug_th10Y);
    labelText[2] = QString("th50/10(U): X=%1/%3 Y=%2/%4").arg(s_debug_th50X_used).arg(s_debug_th50Y_used).arg(s_debug_th10X_used).arg(s_debug_th10Y_used);
    labelText[3] = QString("Sz50/10: X=%1/%3 Y=%2/%4").arg(s_debug_sizeXcur).arg(s_debug_sizeYcur).arg(s_debug_th10sizeXcur).arg(s_debug_th10sizeYcur);
    labelText[4] = QString("Sz50m/M: X=%1/%2 Y=%3/%4").arg(s_debug_sizeXMin).arg(s_debug_sizeXMax).arg(s_debug_sizeYMin).arg(s_debug_sizeYMax);
    labelText[5] = QString("Sz10m/M: X=%1/%2 Y=%3/%4").arg(s_debug_th10sizeXMin).arg(s_debug_th10sizeXMax).arg(s_debug_th10sizeYMin).arg(s_debug_th10sizeYMax);
    labelText[6] = QString("Type: %1 %2").arg(s_debug_size_type).arg(s_debug_curStat);
    labelText[7] = QString("SizeStart: %1 (%2)(%3)").arg(s_debug_sizeSq).arg(s_debug_sizeX).arg(s_debug_sizeY);
#endif
#if defined(SHOW_DEBUG_SIZE_DOWN) || defined(SHOW_DEBUG_SIZE_DOWN_0205)
        float th10Xratio = 0, th10Yratio = 0;
        if (s_debug_th50X_used > EPSILON) {
            th10Xratio = (float)s_debug_th10X_used / (float)s_debug_th50X_used;
        }
        if (s_debug_th50Y_used > EPSILON) {
            th10Yratio = (float)s_debug_th10Y_used / (float)s_debug_th50Y_used;
        }
        //labelText[0].sprintf("Pos: A=%d X=%0.1f Y=%0.1f",s_debug_insideArea,s_debug_posXcur,s_debug_posYcur);
        //labelText[labelTextIdx++].sprintf("Pos(hid): X=%d Y=%d", tp[0].x, tp[0].y);
        labelText[labelTextIdx++].sprintf("Pos: X=%0.1f Y=%0.1f",s_debug_posXcur,s_debug_posYcur);
        labelText[labelTextIdx++].sprintf("th50/10: X=%d/%d Y=%d/%d", s_debug_th50X, s_debug_th10X, s_debug_th50Y, s_debug_th10Y);
        labelText[labelTextIdx++].sprintf("th50_10(R): X=%0.2f Y=%0.2f", th10Xratio, th10Yratio);
        labelText[labelTextIdx++].sprintf("th50/10(U): X=%d/%d Y=%d/%d", s_debug_th50X_used, s_debug_th10X_used, s_debug_th50Y_used, s_debug_th10Y_used);
        labelText[labelTextIdx++].sprintf("Sz50/10: X=%0.1f/%0.1f Y=%0.1f/%0.1f", s_debug_sizeXcur, s_debug_th10sizeXcur, s_debug_sizeYcur, s_debug_th10sizeYcur);
        labelText[labelTextIdx++].sprintf("x10y50=%0.1f x50y10=%0.1f sum=%0.1f", s_debug_X10Y50, s_debug_X50Y10, (s_debug_X10Y50 + s_debug_X50Y10));
        //labelText[labelTextIdx++].sprintf("edge=%d sizeType=%d", s_debug_edge, s_debug_size_type);
        labelText[labelTextIdx++].sprintf("sizeType=%d", s_debug_size_type);
        labelText[labelTextIdx++].sprintf("lineThresholdValue=%d", lineThresholdValue);
        labelText[labelTextIdx++].sprintf("touch_count=%d", testOuputBuffer2.touch_count);
        labelText[labelTextIdx++].sprintf("testNextScan.x1=%d %d", (int)testNextScan.x1, (int)testNextScan.y1);
        labelText[labelTextIdx++].sprintf("touch_data_edge.x=%0.1f %0.1f", testOuputBuffer2.touch_data_edge.x, testOuputBuffer2.touch_data_edge.y);
        labelText[labelTextIdx++].sprintf("MinX=%0.2f->%0.2f,MaxX=%0.2f->%0.2f,MinY=%0.2f->%0.2f,MaxY=%0.2f->%0.2f", min_max_before.minX, min_max_after.minX, min_max_before.maxX, min_max_after.maxX, min_max_before.minY, min_max_after.minY, min_max_before.maxY, min_max_after.maxY);

        //labelText[labelTextIdx++].sprintf("Sz50m/M: X=%0.1f/%0.1f Y=%0.1f/%0.1f", s_debug_sizeXMin, s_debug_sizeXMax, s_debug_sizeYMin, s_debug_sizeYMax);
        //labelText[labelTextIdx++].sprintf("Sz10m/M: X=%0.1f/%0.1f Y=%0.1f/%0.1f", s_debug_th10sizeXMin, s_debug_th10sizeXMax, s_debug_th10sizeYMin, s_debug_th10sizeYMax);
        //labelText[labelTextIdx++].sprintf("Weight: %0.4f", s_debug_sizeWeight);
        //labelText[labelTextIdx++].sprintf("Type: %d %d", s_debug_size_type, s_debug_curStat);
        //labelText[labelTextIdx++].sprintf("SizeStart: %0.1f (%0.1f)(%0.1f)", s_debug_sizeSq, s_debug_sizeX, s_debug_sizeY);
#endif
#if 0 //def SHOW_DEBUG_SIZE_DOWN
    labelText[labelTextIdx++] = QString("Size: %1 (%2)(%3)").arg(s_debug_sizeSq).arg(s_debug_sizeX).arg(s_debug_sizeY);
    labelText[labelTextIdx++] = QString("Pos:  %1 %2").arg(s_debug_posXcur).arg(s_debug_posYcur);
    labelText[labelTextIdx++] = QString("Type: %1 %2").arg(s_debug_size_type).arg(s_debug_curStat);
    labelText[labelTextIdx++] = QString("Size: %1 (%2)(%3)").arg(s_debug_sizeSqCur).arg(s_debug_sizeXcur).arg(s_debug_sizeYcur);
    labelText[labelTextIdx++] = QString("MaxSq/MinSq: %1/%2").arg(s_debug_sizeSqMax).arg(s_debug_sizeSqMin);
    labelText[labelTextIdx++] = QString("Max/Min: (%3/%4)(%5/%6)").arg(s_debug_sizeXMax).arg(s_debug_sizeXMin).arg(s_debug_sizeYMax).arg(s_debug_sizeYMin);
    labelText[labelTextIdx++] = QString("Hor/Ver: %1 %2 (%3 %4)").arg(s_debug_th50X).arg(s_debug_th50Y).arg(s_debug_th10X).arg(s_debug_th10Y);
#endif
#if 0 //def SHOW_DEBUG_SIZE_DOWN
    labelText[0] = QString("Pos:  %1 %2").arg(s_debug_posXcur).arg(s_debug_posYcur);
    labelText[1] = QString("Size: %1 %2 (%3)").arg(s_debug_sizeXcur).arg(s_debug_sizeYcur).arg(s_debug_sizeSq);
    labelText[2] = QString("Type: %1 %2").arg(s_debug_size_type).arg(s_debug_curStat);
    labelText[3] = QString("Hor/Ver: %1 %2 (%3 %4)").arg(BS_inBuf->hor_len).arg(BS_inBuf->ver_len).arg(BS_inBuf->threshold_x_cnt).arg(BS_inBuf->threshold_y_cnt);
    labelText[4] = QString("Ratio: %1 %2").arg(BG_th50RatioX).arg(BG_th50RatioY);
    labelText[5] = QString("Size Mul: %1 (%2 %3)").arg(BG_sizeMulMin).arg(BG_sizeMin).arg(BG_th10CntMin);
    labelText[6] = QString("Size Mul Min/Max: %1 %2").arg(BG_sizeMulMin_min).arg(BG_sizeMulMin_max);
#endif
#if 0 //def DEBUG_INITIAL_SIZESQ
    labelText[7] = QString("%1 %2").arg(s_debug_sizeSq_initial_max).arg(s_debug_sizeSq_initial);
#endif
#if 0
    int idx_i, idx_j;
    float val_i, val_j;
    for (int i = 0; i < BS_touch_info_fine_cnt; i++)
    {
        debugSortIdx[i] = i;
    }
    for (int i = 0; i < (BS_touch_info_fine_cnt - 1); i++) {
        for (int j = (i+1); j < BS_touch_info_fine_cnt; j++) {
            idx_i = debugSortIdx[i];
            idx_j = debugSortIdx[j];
            val_i = BS_touch_info_fine[idx_i].grpXposX;
            val_j = BS_touch_info_fine[idx_j].grpXposX;
            if (val_i > val_j) {
                debugSortIdx[i] = idx_j;
                debugSortIdx[j] = idx_i;
            }
        }
    }
    for (int j = 0; j < BS_touch_info_fine_cnt && j < MAX_BG_DEBUG_MISC; j++) {
        int i = debugSortIdx[j];
        labelText[j].sprintf("%d(%d): %d W=%0.2f (w=%0.2f d=%0.2f) X=%0.2f Y=%0.2f",
                             j, i, BS_touch_info_fine[i].fineStat, BS_touch_info_fine[i].weightFinal,
                             BS_touch_info_fine[i].weight, BS_touch_info_fine[i].xyGrpDist,
                             BS_touch_info_fine[i].centerPos.x, BS_touch_info_fine[i].centerPos.y);
    }
#endif
#if 0 //for test, do not remove
    QString tmpBuf, tmpBuf2;
    int labelTextIdx = 0;
    axis_t axis, axis_2;
    int line, cxp, slotCnt, calMode;
    int line_2, cxp_2, slotCnt_2;
    for (int i = 0; i < BG_cal_slot_cur_cnt; i++) {
        axis = BG_cal_slot_cur[i].axis;
        line = BG_cal_slot_cur[i].line;
        cxp = BG_cal_slot_cur[i].cxp;
        slotCnt = BG_cal_slot_cur[i].slotCnt;
        calMode = BG_cal_slot_cur[i].calMode;
        if (calMode == FINE_CXP_CAL_INIT) {
            //tmpBuf = QString("*a=%1 l=%2 g=%3    ").arg(axis).arg(line).arg(cxp);
            continue;
        }
        else {
            //tmpBuf.sprintf("a=%02d l=%02d g=%02d ", axis, line, cxp);
            tmpBuf = QString("a=%1 l=%2 g=%3    ").arg(axis).arg(line).arg(cxp);
        }
        for (int j = 0; j < slotCnt; j++) {
            //tmpBuf2.sprintf("%02d ", BG_cal_slot_cur[i].slot[j]);
            tmpBuf2 = QString("%1 ").arg(BG_cal_slot_cur[i].slot[j]);
            tmpBuf.append(tmpBuf2);
        }
        if (labelTextIdx < MAX_BG_DEBUG_MISC) {
            labelText[labelTextIdx] = tmpBuf;
            labelTextIdx++;
        }
        if (calMode == FINE_CXP_CAL_NORM) {
            for (int k = 0; k < BG_cal_slot_org_cnt; k++) {
                axis_2 = BG_cal_slot_org[k].axis;
                line_2 = BG_cal_slot_org[k].line;
                cxp_2 = BG_cal_slot_org[k].cxp;
                slotCnt_2 = BG_cal_slot_org[k].slotCnt;
                if (axis == axis_2 && line == line_2 && cxp_2 == FINE_CXP_ORI) {
                    //tmpBuf.sprintf("A=%02d L=%02d G=%02d ", axis_2, line_2, cxp_2);
                    tmpBuf = QString("A=%1 L=%2 G=%3    ").arg(axis_2).arg(line_2).arg(cxp_2);
                    for (int j = 0; j < slotCnt_2; j++) {
                        //tmpBuf2.sprintf("%02d ", BG_cal_slot_cur[k].slot[j]);
                        tmpBuf2 = QString("%1 ").arg(BG_cal_slot_org[k].slot[j]);
                        tmpBuf.append(tmpBuf2);
                    }
                    if (labelTextIdx < MAX_BG_DEBUG_MISC) {
                        labelText[labelTextIdx] = tmpBuf;
                        labelTextIdx++;
                    }
                    break;
                }
            }
        }
    }
#endif
#if 0 //def FINE_TEST_RULE_1 //for test do not remove
    QString tmpBuf;
    axis_t axis;
    int labelTextIdx = 0;
    int line, cxp, slotCnt, calMode, diffSumMin, maxCnt, gridWidth;
    for (int i = 0; i < BG_cal_slot_org_cnt; i++) {
        axis = BG_cal_slot_org[i].axis;
        line = BG_cal_slot_org[i].line;
        cxp = BG_cal_slot_org[i].cxp;
        slotCnt = BG_cal_slot_org[i].slotCnt;
        calMode = BG_cal_slot_org[i].calMode;
        diffSumMin = BG_cal_slot_org[i].diffSumMin;
        maxCnt = BG_cal_slot_org[i].maxCnt;
        gridWidth = BG_cal_slot_org[i].gridWidth;
        if (calMode == FINE_CXP_CAL_INIT) {
            //tmpBuf = QString("a=%1 l=%2 g=%3 Min=%4 Cnt=%5 Width=%6  ").arg(axis).arg(line).arg(cxp).arg(diffSumMin).arg(maxCnt).arg(gridWidth);
            continue;
        }
        else {
            tmpBuf = QString("a=%1 l=%2 g=%3 Min=%4 Cnt=%5 Width=%6  ").arg(axis).arg(line).arg(cxp).arg(diffSumMin).arg(maxCnt).arg(gridWidth);
            //tmpBuf = QString("a=%1 l=%2 g=%3 Cnt=%5 Width=%6  ").arg(axis).arg(line).arg(cxp).arg(maxCnt).arg(gridWidth);
        }
        if (labelTextIdx < MAX_BG_DEBUG_MISC) {
            labelText[labelTextIdx] = tmpBuf;
            labelTextIdx++;
        }
    }
#endif
#if 0 //debug multi
        QString tmpBuf;
        int labelTextIdx = 0;
        for (int i = 0; i < BG_touch_count_multi; i++) {
            tmpBuf.sprintf("[%d] Sq=%0.1f (%0.1f %0.1f) (%0.1f %0.1f) (%0.1f %0.1f)",
                           i, BG_debug_touch_area[i],
                           BG_debug_touch_area_minmax[i].min, BG_debug_touch_area_minmax[i].max,
                           BG_debug_touch_size[i].xSize, BG_debug_touch_size[i].ySize,
                           BG_debug_touch_data[i].x, BG_debug_touch_data[i].y);
            if (labelTextIdx < MAX_BG_DEBUG_MISC) {
                labelText[labelTextIdx] = tmpBuf;
                labelTextIdx++;
            }
        }
#endif

    //show debug info
    for (int i = 0; i < labelTextIdx; i++) {
        debugLabel[i]->setText(labelText[i]);
    }
}

void MainWindow::showTouchLabel(void)
{
    int i;
    QString labelText[MAX_BG_DEBUG_MISC], tmpQstr;

    //reset debug info
    for (int i = 0; i < MAX_BG_DEBUG_MISC; i++) {
        touchLabel[i]->setText("");
    }

    //textTouch.replace(QString("\r\n"), QString(";"));
    QStringList textTouchList = textTouch.split(";");
    //int listCount = textTouch.split(";").length();
    i = 0;
    foreach(QString tmpQstr, textTouchList) {
        if (i < MAX_BG_DEBUG_MISC) {
            labelText[i++] = tmpQstr;
        }
        else {
            break;
        }
    }
#ifdef TEMP_FILE_WRITE //nsmoon@190419
    if (bWriteFile) {
       touchLabel[0]->setStyleSheet("QLabel { color : red; }");
    }
    else {
        touchLabel[0]->setStyleSheet("QLabel { color : blue; }");
    }
#endif
    //show debug info
    for (int i = 0; i < MAX_BG_DEBUG_MISC; i++) {
        touchLabel[i]->setText(labelText[i]);
    }
}

void MainWindow::drawTouchPoint(touch_point_t *tp)
{
#if 0
    static QColor color[20] = {
        qRgb(0, 0, 0),
        qRgb(255, 0, 0), qRgb(0, 255, 0), qRgb(0, 0, 255),
        qRgb(255, 255, 0), qRgb(0, 255, 255), qRgb(255, 0, 255),
        qRgb(255, 0, 0), qRgb(0, 255, 0), qRgb(0, 0, 255),
        qRgb(0, 128, 0),
        qRgb(255, 128, 0), qRgb(0, 255, 128), qRgb(0, 128, 255),
        qRgb(255, 128, 0), qRgb(0, 128, 255), qRgb(255, 128, 255),
        qRgb(255, 128, 0), qRgb(0, 255, 128), qRgb(0, 128, 255),
    };
#endif

    //static QGraphicsItem* textItem = NULL;
    //QString textTouch;
    char tmpStr[10];
    //QPointF pos, posPre;
    vec_t pos, posPre;
    float x0, x1, y0, y1;
    touch_point_t *tpPre = &touchPointPre[0];
    QPen pen;
    pen.setWidth(0); //2=>0
    uint8_t stat, statPre;
    int curIdx, prevIdx;

    if (BG_frame_no >= 28) {
        TRACE_NOP;
    }
    textTouch.clear();
    if (testNextScan.brushMode) {
        textTouch.append("{").append(QString::number(BG_frame_no)).append("}").append(";");
    }
    else {
        textTouch.append("[").append(QString::number(BG_frame_no)).append("] ").append(m_ReadFilePath).append(";");
    }

    for(int i = 0; i < touchPointCnt; i++)
    {
        curIdx = i;
        stat = tp[curIdx].stat & 0x03;
        switch (stat) {
        case 0x3:
            sprintf(tmpStr, "%s", "DOWN ");
            break;
        case 0x0:
            sprintf(tmpStr, "%s", "UP ");
            break;
        default:
            sprintf(tmpStr, "%s", "UNKNOWN ");
            break;
        }

        if (stat != 0x03) {
            RestoreDisplayPos(tp[curIdx].x, tp[curIdx].y, &pos);
            x1 = posX(pos.x);
            y1 = posY(pos.y);
            textTouch.append(tmpStr).append(QString::number(i)).append(" ").\
                    append("x:").append(QString::number(x1)).append(", ").\
                    append("y:").append(QString::number(y1)).\
                    append(";");
            continue;
        }

        statPre = 0;
        for (int j = 0; j < touchPointPreCnt; j++) {
            statPre = tpPre[j].stat & 0x03;
            if (statPre == 0x03 && tpPre[j].id == tp[curIdx].id) {
                prevIdx = j;
                break;
            }
            else {
                statPre = 0;
            }
        }

        //qDebug() << stat << tmpStr;
        if (stat == 0x03 && statPre == 0x03) {
            RestoreDisplayPos(tpPre[prevIdx].x, tpPre[prevIdx].y, &posPre);
            x0 = posX(posPre.x);
            y0 = posY(posPre.y);
            RestoreDisplayPos(tp[curIdx].x, tp[curIdx].y, &pos);
            x1 = posX(pos.x);
            y1 = posY(pos.y);
            if (touchDisplayColor == TOUCH_DISP_COLOR_SIZE) {
                if (tp[curIdx].xSize == 2) {
                    pen.setColor(colorQt[0]);
                }
                else if (tp[curIdx].xSize == 4/*6*/) {
                    pen.setColor(colorQt[2]);
                }
                else if (tp[curIdx].xSize == 8) {
                    pen.setColor(colorQt[1]);
                }
                else if (tp[curIdx].xSize == 1) {
                    pen.setColor(colorQt[6]);
                }
                else {
                    pen.setColor(colorQt[6]);
                }
            }
            else { //id color
                pen.setColor(colorQt[tp[curIdx].id]);
            }

            //touchLst.push_back(this->scene->addLine(posPre.x(), posPre.y(), pos.x(), pos.y(), pen));
            touchLst.push_back(scene->addLine(QLineF(x0, y0, x1, y1), pen));
            touchLst.last()->setVisible(true);
            //text.append(QString::number(i)).append(" ").append("x:").append(QString::number(pos.x())).append(", ").append("y:").append(QString::number(pos.y())).append("\r\n");
            textTouch.append(tmpStr).append(QString::number(i)).append(" ").\
                    append("x:").append(QString::number(tp[curIdx].x)).append(", ").\
                    append("y:").append(QString::number(tp[curIdx].y)).append(", ").\
                    append("size:").append(QString::number(tp[curIdx].xSize)).append("*").append(QString::number(tp[curIdx].ySize)).\
                    append(";");
        }
    } //for(int i = 0; i < touchPointCnt; i++)

    for(int i = 0; i < touchPointCnt; i++)
    {
        tpPre[i] = tp[i]; //save
    }
    touchPointPreCnt = touchPointCnt;

    //if(textItem != NULL)
    //{
    //    this->scene->removeItem(textItem);
    //}
    //textItem = this->scene->addText(text);

    showDebugLabel(tp); //show debug info
    showTouchLabel(); //show touch info

    bDrawTouchPoint = false;
    return;
}

#ifdef DEBUG_WRITE_INPUT_BUFF //nsmoon@200629
#ifdef DEBUG_WRITE_INPUT_BUFF_GNERATE //for generating test data
void MainWindow::debugWriteInputBuff(DEF_PD_INFO *inBuf)
{
    int debug_frame_no = BG_frame_no - DEBUG_FRAME_NO_START;
    if (debug_frame_no >= 0 && debug_frame_no < DEBUG_FRAME_COUNT) {
        s_debug_inBuf[debug_frame_no].hor_touch_pd = (DEF_PD *)&debug_hor_touch_pd[debug_frame_no][0];
        s_debug_inBuf[debug_frame_no].ver_touch_pd = (DEF_PD *)&debug_ver_touch_pd[debug_frame_no][0];
        s_debug_inBuf[debug_frame_no].threshold_x = (uint8_t *)&debug_threshold_x[debug_frame_no][0];
        s_debug_inBuf[debug_frame_no].threshold_y = (uint8_t *)&debug_threshold_y[debug_frame_no][0];
        s_debug_inBuf[debug_frame_no].hor_len = inBuf->hor_len;
        s_debug_inBuf[debug_frame_no].ver_len = inBuf->ver_len;
        s_debug_inBuf[debug_frame_no].threshold_x_cnt = inBuf->threshold_x_cnt;
        s_debug_inBuf[debug_frame_no].threshold_y_cnt = inBuf->threshold_y_cnt;

        if (inBuf->hor_len < DEBUG_MAX_TEST_DATA) {
            for (int j = 0; j < inBuf->hor_len; j++) {
                s_debug_inBuf[debug_frame_no].hor_touch_pd[j].pd = inBuf->hor_touch_pd[j].pd;
                s_debug_inBuf[debug_frame_no].hor_touch_pd[j].led = inBuf->hor_touch_pd[j].led;
            }        }
        else {
            TRACE("ERROR! invalid s_inBuf->hor_len %d", inBuf->hor_len);
        }

        if (inBuf->ver_len < DEBUG_MAX_TEST_DATA) {
            for (int j = 0; j < inBuf->ver_len; j++) {
                s_debug_inBuf[debug_frame_no].ver_touch_pd[j].pd = inBuf->ver_touch_pd[j].pd;
                s_debug_inBuf[debug_frame_no].ver_touch_pd[j].led = inBuf->ver_touch_pd[j].led;
            }        }
        else {
            TRACE("ERROR! invalid s_inBuf->ver_len %d", inBuf->ver_len);
        }

        memcpy(s_debug_inBuf[debug_frame_no].threshold_x, inBuf->threshold_x, DEBUG_MAX_THRESHOLD_X);
        memcpy(s_debug_inBuf[debug_frame_no].threshold_y, inBuf->threshold_y, DEBUG_MAX_THRESHOLD_Y);

        if (debug_frame_no == 0) {
            TRACE("debug_frame_no=,%d,%d,,%d,%d,,%d,%d,", BG_frame_no, debug_frame_no, inBuf->hor_len, inBuf->ver_len, inBuf->threshold_x_cnt, inBuf->threshold_y_cnt);
            for (int j = 0; j < inBuf->hor_len; j++) {
                TRACE("(%d, %d)", inBuf->hor_touch_pd[j].pd, inBuf->hor_touch_pd[j].led);
            }
            for (int j = 0; j < s_debug_inBuf[debug_frame_no].hor_len; j++) {
                TRACE("[%d, %d]", s_debug_inBuf[debug_frame_no].hor_touch_pd[j].pd, s_debug_inBuf[debug_frame_no].hor_touch_pd[j].led);
            }
            for (int j = 0; j < DEBUG_MAX_THRESHOLD_X; j++) {
                TRACE("(%d)", s_debug_inBuf[debug_frame_no].threshold_x[j]);
            }
            for (int j = 0; j < DEBUG_MAX_THRESHOLD_Y; j++) {
                TRACE("[%d]", s_debug_inBuf[debug_frame_no].threshold_y[j]);
            }
        }
    }
}

void MainWindow::debugPrintInputBuff(void)
{
    newFileWrite("#define DEBUG_FRAME_COUNT       %d", DEBUG_FRAME_COUNT);
    newFileWrite("#define DEBUG_MAX_THRESHOLD_X	%d", DEBUG_MAX_THRESHOLD_X);
    newFileWrite("#define DEBUG_MAX_THRESHOLD_Y	%d \n", DEBUG_MAX_THRESHOLD_Y);

    newFileWrite("DEF_PD_INFO const s_debug_inBuf[DEBUG_FRAME_COUNT] = {");
    for (int i = 0; i < DEBUG_FRAME_COUNT; i++)
    {
        newFileWrite(" { //%d", i);
        newFileWrite("  %d, %d, 0, 0, %d, %d, 0, 0", s_debug_inBuf[i].hor_len, s_debug_inBuf[i].ver_len, s_debug_inBuf[i].threshold_x_cnt, s_debug_inBuf[i].threshold_y_cnt);
        newFileWrite(" },");
    }
    newFileWrite("}; \n");

    newFileWrite("DEF_PD const debug_hor_touch_pd[DEBUG_FRAME_COUNT][DEBUG_MAX_TEST_DATA] = {");
    for (int i = 0; i < DEBUG_FRAME_COUNT; i++)
    {
        int len = s_debug_inBuf[i].hor_len;
        newFileWrite(" { //%d-%d", i, len);
        for (int j = 0; j < len; j++)
        {
            newFileWrite("  {%d, %d}, //%d", s_debug_inBuf[i].hor_touch_pd[j].pd, s_debug_inBuf[i].hor_touch_pd[j].led, j);
        }
        newFileWrite(" },");
    }
    newFileWrite("}; \n");

    newFileWrite("DEF_PD const debug_ver_touch_pd[DEBUG_FRAME_COUNT][DEBUG_MAX_TEST_DATA]= {");
    for (int i = 0; i < DEBUG_FRAME_COUNT; i++)
    {
        int len = s_debug_inBuf[i].ver_len;
        newFileWrite(" { //%d-%d", i, len);
        for (int j = 0; j < len; j++)
        {
            newFileWrite("  {%d, %d}, //%d", s_debug_inBuf[i].ver_touch_pd[j].pd, s_debug_inBuf[i].ver_touch_pd[j].led, j);
        }
        newFileWrite(" },");
    }
    newFileWrite("};\r\n");

    newFileWrite("uint8_t const debug_threshold_x[DEBUG_FRAME_COUNT][DEBUG_MAX_THRESHOLD_X]= {");
    for (int i = 0; i < DEBUG_FRAME_COUNT; i++)
    {
        newFileWrite(" { //%d", i);
        for (int j = 0; j < DEBUG_MAX_THRESHOLD_X; j++)
        {
            newFileWrite("  %d, //%d", (int)s_debug_inBuf[i].threshold_x[j], j);
        }
        newFileWrite(" },");
    }
    newFileWrite("}; \n");

    newFileWrite("uint8_t const debug_threshold_y[DEBUG_FRAME_COUNT][DEBUG_MAX_THRESHOLD_Y]= {");
    for (int i = 0; i < DEBUG_FRAME_COUNT; i++)
    {
        newFileWrite(" { //%d", i);
        for (int j = 0; j < DEBUG_MAX_THRESHOLD_Y; j++)
        {
            newFileWrite("  %d, //%d", (int)s_debug_inBuf[i].threshold_y[j], j);
        }
        newFileWrite(" },");
    }
    newFileWrite("}; \n");
    m_NewFile->close();
}
#endif

int MainWindow::debugReadInputBuff(DEF_PD_INFO *inBuf)
{
    if (debug_frame_no >= 0 && debug_frame_no < DEBUG_FRAME_COUNT) {
        inBuf->hor_len = s_debug_inBuf[debug_frame_no].hor_len;
        inBuf->ver_len = s_debug_inBuf[debug_frame_no].ver_len;
        inBuf->threshold_x_cnt = s_debug_inBuf[debug_frame_no].threshold_x_cnt;
        inBuf->threshold_y_cnt = s_debug_inBuf[debug_frame_no].threshold_y_cnt;

        inBuf->hor_touch_pd = (DEF_PD *)&debug_hor_touch_pd[debug_frame_no][0];
        inBuf->ver_touch_pd = (DEF_PD *)&debug_ver_touch_pd[debug_frame_no][0];
        inBuf->threshold_x = (uint8_t *)&debug_threshold_x[debug_frame_no][0];
        inBuf->threshold_y = (uint8_t *)&debug_threshold_y[debug_frame_no][0];
        debug_frame_no++;
        return 0;
    }
    TRACE_ERROR("ERROR! invalid debug_frame_no %d", debug_frame_no);
    return 1;
}
#endif

int MainWindow::backend_process_line_data(int nBufNumber)
{
    s_inBuf = (DEF_PD_INFO *)&testInputBuffer;
    DEF_DATA_INFO *OutBuf = &testOuputBuffer;
    DEF_DATA_INFO2 *OutBuf2 = &testOuputBuffer2;
    BACKEND_STATUS ret;

    START_STOP_POLY_TRACE(0xFD);

    OutBuf->len = 0;
    OutBuf->buf = &s_outBuf[0];
    OutBuf2->touch_count = 0;

    draw_pd_data(nBufNumber, FALSE); //FALSE: get testInputBuffer
    testNextScan.currScan = testNextScan.nextScan;
    //testNextScan.brushMode = 0; //for pen
    //testNextScan.brushMode = 1; //for brush

#ifdef DEBUG_WRITE_INPUT_BUFF //for generate test data //nsmoon@200629
#ifdef DEBUG_WRITE_INPUT_BUFF_GNERATE //for generating test data
    debugWriteInputBuff(s_inBuf);
#else
    if (debugReadInputBuff(s_inBuf)) {
        return 0; //no-error
    }
#endif
#endif

    testNextScan.numTouch = s_prevTouchCntPen;
    //DEBUG_TICK_COUNT(0);
    ret = BG_call_backend2(s_inBuf, (DEF_DATA_INFO2 *)OutBuf2, (next_scan_t *)&testNextScan);
    //if (OutBuf->len > 0) DEBUG_TICK_COUNT(1);
    if (ret != NO_BACKEND_ERROR) {
        START_STOP_POLY_TRACE(0xFE);
        rmFrameCnt += 4;
        TRACE_ERROR("ERROR! BG_call_backend2..%d", ret);
        return 1; //error
    }
#if 1 //nsmoon@211018
    TRACE_SAVELOG("[%d] [%d]", BG_frame_no, OutBuf2->touch_count);
    for (int i = 0; i < OutBuf2->touch_count; i++) {
        TRACE_SAVELOG(" %d %0.1f %0.1f %0.1f %0.1f", i, OutBuf2->touch_data[i].x, OutBuf2->touch_data[i].y, OutBuf2->touch_size[i].xSize, OutBuf2->touch_size[i].ySize);
    }
#endif
#if 0 //nsmoon@210430
    if ((testNextScan.x1 + testNextScan.y1) > MAX_REMAINED_LINE_FRAME) {
        if (++rmFrameCnt > MAX_REMAINED_FRAME_CNT) {
            TRACE_NOP; //force to AGC
        }
    }
    else {
        rmFrameCnt = 0;
    }
    TRACE("testNextScan.x1,y1= %d %d %d", testNextScan.x1, testNextScan.y1, rmFrameCnt);
#endif
#if 1 //def FRONTEND_FORCED_AGC
#define FRAME_REMAINED_LINE_MAX_X 5		//YJ@211102
#define FRAME_REMAINED_LINE_MAX_Y 20
    if((OutBuf2->touch_count == 0) && (OutBuf2->touch_data_edge.x == 0 )) {
        if ((testNextScan.x1 + testNextScan.y1) >= FRAME_REMAINED_LINE_MAX_X) {
            rmFrameCnt += 2;
        }
    }
    else {
        if(rmFrameCnt > 0) {
            rmFrameCnt--;
        }
    }
    //TRACE("testNextScan.x1,y1= %d %d %d %d %d", testNextScan.x1 , testNextScan.y1, OutBuf2->touch_count, (int)OutBuf2->touch_data_edge.x, rmFrameCnt);
#endif

    s_touch_count = OutBuf2->touch_count;
    s_touch_data = OutBuf2->touch_data;
    s_touch_size = OutBuf2->touch_size;
    s_touch_lineCnt = OutBuf2->touch_line_cnt;
    s_multi_fine = OutBuf2->multi_fine;
    s_touchReportCnt = 0;

    s_touch_point_tracking(); //nsmoon@201127
    START_STOP_POLY_TRACE(0xFE);

#if (UPDATE_TOUCH_STATUS_TEST_ENABLE == 1)
    s_smooth_filter_test(); //s_smooth_filter();
    s_coordinates_conversion(OutBuf);
#else
#if 1 //old s_touch_point_tracking
    //if(touchMode == APP_DRAW_PEN_MARKER)
    if (testNextScan.brushMode == 0)
    {
        s_smooth_filter();
        s_coordinates_conversion(OutBuf);
    }
#if (BRUSH_MODE_ENABLE == 1) //shj@20200226	for bursh
    else
    {
        s_smooth_filter_brush();
        s_coordinates_conversion_brush(OutBuf);
    }
#endif
#else
#if (BRUSH_MODE_ENABLE == 1) //shj@20200226	for bursh
    if (BS_brushMode) {
        //s_touch_point_tracking_brush();
        s_touch_point_tracking_new(update_touch_state_brush, 0);
    }
    else
#endif
    {
        //s_touch_point_tracking();
        s_touch_point_tracking_new(update_touch_state, init_pos_info);
    }
#endif
#endif

#if 0 //for test
#if defined(ONE_EURO_FILTER_ENABLE)
    s_smooth_euro_filter();
#elif defined(MOVING_AVG_FILTER_ENABLE)
    s_smooth_movavg_filter();
#elif defined(KALMAN_LINER_FILTER_ENABLE)
    s_smooth_kalman_liner_filter();
#elif defined(HOLTS_EXP_FILTER_ENABLE)
    s_smooth_holts_exp_filter();
#else
    s_smooth_filter();
#endif
    s_coordinates_conversion(OutBuf);
#endif //0

#ifdef HOR_EDGE_TOUCH_ENABLE
    //TRACE("touch_data_edge= %0.1f, %0.1f", OutBuf2->touch_data_edge.x, OutBuf2->touch_data_edge.y);
    s_wheel_delta = s_cal_hor_scroll(OutBuf2->touch_data_edge);
    //TRACE("s_wheel_delta= %d", s_wheel_delta);
#endif

#if 1 //save touch point
   for (int i = 0; i < OutBuf->len; i++) {
#if 0 //def QT_DEBUG
       TRACE("[%d]%d,%d", i, OutBuf->buf[i].status, OutBuf->buf[i].contactID);
       TRACE("(%d,%d)(%d,%d)", OutBuf->buf[i].xCord, OutBuf->buf[i].yCord, OutBuf->buf[i].xSize, OutBuf->buf[i].ySize);
#endif
       touchPoint[i].id = OutBuf->buf[i].contactID;
       touchPoint[i].stat = OutBuf->buf[i].status;
       touchPoint[i].x = OutBuf->buf[i].xCord;
       touchPoint[i].y = OutBuf->buf[i].yCord;
       touchPoint[i].xSize = OutBuf->buf[i].xSize;
       touchPoint[i].ySize = OutBuf->buf[i].ySize;
   }
   touchPointCnt = OutBuf->len;
#endif
#if 0 //not-use
    int cnt = 0;
    for (int i = 0; i < OUTPUT_BUFFER_SIZE - 1; i += 6) {
        touchPoint[cnt].stat = OutBuffer[i];
        if (OutBuffer[i]) { //if status is not 0
            touchPoint[cnt].x = (OutBuffer[i + 3] << 8) + OutBuffer[i + 2];
            touchPoint[cnt].y = (OutBuffer[i + 5] << 8) + OutBuffer[i + 4];
            //if (!i) TRACENR("backend_process_line_data()..");
            //TRACENR("[%d] %02x %x %04x %04x  ", i / 6, OutBuffer[i], OutBuffer[i + 1], touchPoint[cnt].x, touchPoint[cnt].y);
            cnt++;
        }
    }
#endif

    while (bDrawTouchPoint) Sleep(1); //nsmoon@211019
    bDrawTouchPoint = true;
    emit CallDrawTouchPoint(&touchPoint[0]);

#ifndef DRAW_THREAD
    if (is_myDrawPolygon()) {
        while (bDrawOutPoly) Sleep(1); //nsmoon@211019
        bDrawOutPoly = true;
        emit CallDrawOutPoly();
        TRACE_NOP;
    }
#endif
#if 1 //nsmoon@211019
    if (s_inBuf->hor_len == 0 && s_inBuf->ver_len == 0) {
        return 1; //no-input
    }
    return 0; //no-error
#endif
}

void MainWindow::set_debug_func(DEF_MIN_MAX *before, DEF_MIN_MAX *after)
{
    min_max_before.minX = before->minX;
    min_max_after.minX = after->minX;
    min_max_before.maxX = before->maxX;
    min_max_after.maxX = after->maxX;
    min_max_before.minY = before->minY;
    min_max_after.minY = after->minY;
    min_max_before.maxY = before->maxY;
    min_max_after.maxY = after->maxY;
}

#define TRACE_RETRY(...)    //TRACE(__VA_ARGS__)
void MainWindow::hidThrdUsb(const void* obj)
{
    MainWindow* pThis = (MainWindow*)obj;
    //unsigned short aUsbInIdx;
    int nResult;
    int nReceivedCount = 0, nPacketStart;
    int totalPacketSize;   //total packet size, bytes
    unsigned int packetNumber = 0;      //packet number
    unsigned char *InPtr;
    //bool bRequest = TRUE;  //true? packet?
    bool bFoundHeader = false;
    int tmpSize;
    DWORD error;
    unsigned char usbCmd;

    aUsbIn[0] = (unsigned char *)malloc(USB_PROCESS_BUFF);

    //unsigned char Command;
    //int touchCnt = 0;
    pThis->nBufNumber = 0;
    //bWaitSndMsg = FALSE;
    pThis->HidReceiveBuf = (unsigned char *)&aUsbIn[0][0];

    while (1)
    {
        if ((!bHidConnected) || (!AttachedState && !pThis->bFromFile)) {
            ofstDataCnt = 0; //reset
            //bFirstStart = true;
            Sleep(10);
            continue;
        }
        if (bPause) {
            if (bOneStep) {
                Sleep(10);
                continue;
            }
            bOneStep = true;
        }
        //if (bWaitSndMsg) {
        //    TRACE("nBufNumber,bWaitSndMsg: %d %d", nBufNumber, bWaitSndMsg);
        //    Sleep(10);
        //    continue;
        //}

#if defined(USE_USB_LIB)  //nsmoon@191113
        if (!pThis->bFromFile) {
            //TRACE("nDltNormal=%d", pThis->nDltNormal);
            if (pThis->nDltNormal == TARGET_NORMAL) {
                if (pThis->USB_SendRequest(USB_NORCMD_DLT)) {
                    Sleep(300);
                }
                else {
                    pThis->nDltNormal = TARGET_DLT_REQ;
                }
                continue;
            }
            else if (pThis->nDltNormal == TARGET_DLT_REQ) {
                uint8_t res = pThis->HID_RecvNorRes();
                if (res == TARGET_RES_OK) {
                    pThis->nDltNormal = TARGET_DLT_OK;
                }
                else {
                    pThis->nDltNormal = TARGET_NORMAL;
                }
                continue;
            }
        }
#endif //1

        if (bRequest)
        {
            if (!pThis->bFromFile) {
                //request packet
                if (bInitOfst)
                {
#ifdef FRONTEND_LINE_THRESHOLD
                    usbCmd = USB_CMD_LINE_THRESHOLD;
#else
                    usbCmd = USB_CMD_LINE_DATA;
#endif
                }
                else {
                    usbCmd = USB_CMD_OFST_DATA;
                }
                error = pThis->USB_SendRequest(usbCmd);
                if (error != ERROR_SUCCESS && error != ERROR_IO_PENDING) {
                    TRACE("ERROR! USB_SendRequest=%d (%d)", error, bInitOfst);
                    Sleep(1);
                    continue;
                }
            }
            bRequest = FALSE;  //do not request packet
            bFoundHeader = FALSE;
            nReceivedCount = 0;
            totalPacketSize = 0;
            packetNumber = 0;
            nPacketStart = 0;
            pThis->usb_time_out = 0;
            TRACE_RETRY("@");
            //Sleep(5);
        }
#if 0
        else {
            if (packetNumber < NUM_OF_PACKET) {
                packetNumber++;
            }
            else {
                TRACE_ERROR("ERROR! invalid packetNumber: %d (%d)", packetNumber, NUM_OF_PACKET);
                goto L10;
            }
        }
#endif

        if ((nReceivedCount + USB_PACKET_SIZE) < USB_PROCESS_BUFF) {
            InPtr = (unsigned char *)&aUsbIn[pThis->nBufNumber][nReceivedCount];
        }
        else {
            TRACE_ERROR("ERROR! invalid nReceivedCount %d", nReceivedCount);
            InPtr = (unsigned char *)&aUsbIn[pThis->nBufNumber][0];
        }

        //read packet
#ifdef HID_THRD_TEST
        if (bInitOfst) {
            nResult = TEST_DATA_BUFF_LEN;
        }
        else {
            nResult = TEST_OFFSET_BUFF_LEN;
        }
#else
        if (pThis->bFromFile) {
            nResult = pThis->m_file->read((char *)InPtr, USB_PACKET_SIZE);
        }
        else {
            pThis->USB_readData(InPtr, &nResult, 0); //0: sync read
        }
#endif
        if (nResult == 0) {
            if(pThis->bFromFile && pThis->m_file->isOpen()) {
                qDebug("Closed debug file~ Stop thread!");
                bHidConnected = false;
                pThis->bFromFile = false;
                pThis->m_file->close();
#ifdef DEBUG_WRITE_INPUT_BUFF //for generate test data //nsmoon@200629
#ifdef DEBUG_WRITE_INPUT_BUFF_GNERATE //for generating test data
                pThis->debugPrintInputBuff();
#endif
#endif
            }
            else if (pThis->usb_time_out++ > USB_RETRY_COUNT) {
                qDebug("ERROR! read_timeout..");
                bRequest = TRUE; //request re-transmit
            }
            else {
                TRACE_RETRY("+");
                //Sleep(10);
            }
            continue;
        }
        else if (nResult < 0) {
            TRACE("ERROR! invalid nResult");
            goto L10; //error
        }

        if (bFoundHeader == false) {
            if (bInitOfst) {
  #if defined(DPT_POLYGON_DATA)
                if (check_header_poly(InPtr, nReceivedCount, &totalPacketSize, &nPacketStart))
  #else
                if (pThis->check_header_line(InPtr, &totalPacketSize, &nPacketStart))
  #endif
                {
                    goto L10; //error
                }
            }
            else {
                if (pThis->check_header_ofst(InPtr, &totalPacketSize, &nPacketStart))
                {
                    continue;
                }
            }
            bFoundHeader = TRUE;
            if (pThis->bFromFile) {
                while (1) {
                    nReceivedCount += nResult;
                    InPtr = (unsigned char *)&aUsbIn[pThis->nBufNumber][nReceivedCount];
                    tmpSize = (totalPacketSize + 2) - nReceivedCount;
                    nResult = pThis->m_file->read((char *)InPtr, tmpSize);
                    if (tmpSize == nResult) break;
                }
                //TRACE("nReceivedCount,totalPacketSize: %x %x", (nReceivedCount + nResult), (totalPacketSize + 2));
            }
        }

        //increased packet count
        nReceivedCount += nResult;
        //TRACE("nReceivedCount,totalPacketSize: %d / %d", nReceivedCount, totalPacketSize);
        if (totalPacketSize <= 0 || totalPacketSize > USB_PROCESS_BUFF) {
            TRACE_ERROR("ERROR! invalid totalPacketSize %d", totalPacketSize);
            //goto L10; //error
        }

        if (nReceivedCount >= (totalPacketSize + 2)) //crc 2-bytes
        {
            //check crc
            uint16_t crc_cal = pThis->USB_CalculateCrc(&pThis->aUsbIn[nBufNumber][nPacketStart], totalPacketSize);
            uint16_t crc_get = pThis->aUsbIn[nBufNumber][totalPacketSize + 1];
            crc_get = (crc_get << 8) | pThis->aUsbIn[nBufNumber][totalPacketSize];
#ifndef FRONTEND_CRC_CHECK //nsmoon@210510
            crc_get = crc_cal;
#endif
            if (crc_cal == crc_get) {
                //TRACE("OK! crc %04x", crc_get);
                //change Usb buffer
                //TRACE("nBufNumber,nPacketStart: %d %d (%x)", nBufNumber, nPacketStart, pThis->HidReceiveBuf);
                pThis->HidReceiveBuf = (unsigned char *)&pThis->aUsbIn[pThis->nBufNumber][nPacketStart];
                pThis->HidReceiveLen = totalPacketSize;
                if (++pThis->nBufNumber >= MAX_USB_IN_BUFF) pThis->nBufNumber = 0;
#ifdef TEMP_FILE_WRITE
                if (!pThis->bFromFile) {
                    if (bInitOfst) {
                        if (bWriteFile) {
                            pThis->m_soureFile.write((const char*)pThis->HidReceiveBuf, totalPacketSize+2);
                        }
                    }
                    else {
                        //pThis->m_soureFile.write((const char*)pThis->HidReceiveBuf, totalPacketSize+2);
                        //save offset data
                        ofstDataCnt = totalPacketSize+2;
                        TRACE("ofstDataCnt= %d", ofstDataCnt);
                        memcpy(ofstData, pThis->HidReceiveBuf, ofstDataCnt);
                    }
                }
#endif
                //process received packet
                pThis->bRequest = TRUE; //request next packet
                //bWaitSndMsg = TRUE;

                if (pThis->bInitOfst == false) {
                    pThis->set_ofst_data(pThis->nBufNumber);
                    pThis->initDispParm(); //init prameter for display
                    pThis->draw_axis();
#ifdef DEBUG_WRITE_INPUT_BUFF //nsmoon@200629
                    debug_frame_no = 0;
#ifdef DEBUG_WRITE_INPUT_BUFF_GNERATE
                    pThis->m_CreateFilePath = QString("debug");
                    pThis->OpenNewFileName();
#endif
#endif
                }
                else {
#if 0 //nsmoon@211019 move to in backend_process_line_data()
                    while (pThis->bDrawOutPoly || pThis->bDrawTouchPoint) {
                        Sleep(10);
                    }
#endif
                    int ret = pThis->backend_process_line_data(pThis->nBufNumber);
                    if (bFirstStart && ret == 0) {
                        bFirstStart = false;
                        bPause = true;
                        bOneStep = true;
                    }
    #if defined(DEBUG_FRAME_NO)//for debugging
                    if (BG_frame_no >= DEBUG_FRAME_NO )
    #endif
                    {
    #if (DEBUG_FRAME_PAUSE == 1)
                        //if (BG_frame_no > 70 && touchPointCnt > 0 && touchPointCnt < 3) //f2p-1023d
                        bPause = true;
                        bOneStep = true;
    #else
                        TRACE_NOP;
    #endif
                    }
                    if (pThis->bChangeThreshold) {
                        pThis->bInitOfst = false;
                        pThis->bChangeThreshold = false;
                    }
                }
                if (pThis->bFromFile) {
                    Sleep(10);
                }
#if 0 //for timediff, nsmoon@201217
                else {
                    if (pThis->dateTimeCnt < 0) {
                        pThis->dateTimeCnt = 0;
                    }
                    else {
                        int curr = pThis->dateTimeCnt;
                        int prev = (curr) ? 0 : 1;
                        pThis->dateTime[curr] = QDateTime::currentDateTime();
                        qint64 msecDiff = pThis->dateTime[prev].msecsTo(pThis->dateTime[curr]);
                        TRACE_RELEASE("msecDiff= %lld (%d/%d)", msecDiff, curr, prev);
                        pThis->dateTimeCnt = (pThis->dateTimeCnt) ? 0 : 1;
                    }
                }
#endif
            }
            else {
                TRACE_ERROR("ERROR! invalid crc %04x %04x", crc_cal, crc_get);
            } //if (crc_cal == crc_get)
        }
        continue;

    L10:
        if (!pThis->bFromFile) {
            //flush read buffer
            TRACE("Flush USB read data...");
            int loopTmp = 0;
            while (loopTmp++ < 500)
            {
                error = pThis->USB_readData(InPtr, &nResult, 1); //1: async read
                if (error != ERROR_SUCCESS) {
                    TRACENR("loopTmp=%d", loopTmp);
                    break;
                }
            }
        }
        pThis->bRequest = TRUE; //request packet
    } //while
}
/*end of file*/
