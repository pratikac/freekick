#ifndef __freekick__
#define __freekick__

#include <iostream>
#include <math.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include "macros.h"
#include <cv.h>
#include <highgui.h>
#include <BlobResult.h>
#include <videoInput.h>
using namespace std;

#include "thresholds.h"
#include "geometry.h"

#define BAUD_RATE       (38400)
#define TOT_BALLS       (4)

#define IMAGE_WIDTH     (640)
#define IMAGE_HEIGHT    (480)

#define MIN_GOAL_DIST                   (10)
#define CIRCLE_BOT_DIST                 (40)
#define BALL_DETECTION_FRAMES_TO_AVG    (15)
#define MIN_BALL_DIST                   (30)
#define OUR_INF                         (IMAGE_WIDTH*10)

enum
{
    WHITE=0,
    BLACK
};

enum
{
    RED=0,
    BLUE
};
typedef struct Bot
{
    CvPoint center;
    float angle;
    CvPoint circleCenter;
    CvPoint rectCenter;
    unsigned char HUE_L;
    unsigned char HUE_U;
    unsigned char SAT_L;
    unsigned char SAT_U;
    unsigned int AREA_MIN;
    unsigned int AREA_MAX;
    unsigned int COMPACTNESS_MIN;
    unsigned int COMPACTNESS_MAX;
}
Bot;

void initBots(void);
void getGoals(void);
void getBots(void);
void getBalls(void);
void drawBlobs(CBlobResult blobs, IplImage* img, CvScalar color);
CBlobResult extractBlobs(IplImage *img, uchar hueL, uchar hueU, uchar satL, uchar satU, uchar valL = VAL_L, uchar valU = VAL_U);

inline CvPoint getCenter(CBlob blob);
inline float angleOfBot(Bot* bot);
inline float dist(CvPoint p1, CvPoint p2);

// Serial Port functions
extern "C"
{
    BOOL serialportInit(HANDLE *handle, const char *port, UINT32 baudRate);
    void SerialPutC(HANDLE hCom, char txchar);
    unsigned int SerialTX(HANDLE hCom, char* buff, unsigned int count);
    char SerialGetC(HANDLE hCom);
}

#endif
