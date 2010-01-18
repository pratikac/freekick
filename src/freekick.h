#ifndef __freekick__
#define __freekick__

#include <iostream>
#include <math.h>
#include <conio.h>
#include <vector>
#include <windows.h>
#include <time.h>
#include "macros.h"
#include <cv.h>
#include <highgui.h>
#include <BlobResult.h>
#include <videoInput.h>
using namespace std;

#include "random.h"
#include "thresholds.h"
#include "geometry.h"

#define BAUD_RATE       (38400)
#define TOT_BALLS       (4)

#define IMAGE_WIDTH     (640)
#define IMAGE_HEIGHT    (480)

#define MIN_GOAL_DIST                   (10)
#define CIRCLE_BOT_DIST                 (40)
#define BALL_DETECTION_FRAMES_TO_AVG    (1)
#define MIN_BALL_DIST                   (30)
#define OUR_INF                         (IMAGE_WIDTH*10)

#define EPSILON         (0.2)
#define MAX_NODES       (400)

#define THRESHOLD       (10)
#define EXTEND_DIST     (10)

#define GOAL_X          (500)
#define GOAL_Y          (150)

// Robot Parameters (pixels)
#define ROBOT_RADIUS    (45)

// Probabilities
#define GOAL_PROB       (0.4)

//#define VERBOSE         (0)

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

enum
{
    OBS_D_L=0,
    OBS_D_R,
    OBS_RED_W,
    OBS_RED_B,
    OBS_BLUE_W,
    OBS_BLUE_B
};

typedef struct Bot
{
    CvPoint center;
    float angle;
    CvPoint circleCenter;
    CvPoint rectCenter;
    int state;
    CvPoint current_dest;
}
Bot;

enum 
{
    IDLE,
    MOVE,
    CAPTURE,
    DRIBBLE,
    SHOOT,
};

typedef struct
{
    CvPoint point;
    int parentIndex;
}CvPointNode;

class Obstacle
{
    public:
        CvPoint center;
        unsigned int radius;

        Obstacle(int xc = 0, int yc = 0, unsigned int r = 0)
        {
            CvPoint temp;
            temp.x = xc;
            temp.y = yc;
            center = temp;
            radius = r;
        }

};

unsigned int RRTPlan(CvPoint, CvPoint, int);
CvPoint Extend(CvPoint, CvPoint, int);
CvPointNode Nearest(CvPoint, unsigned int);
inline float dist(CvPoint, CvPoint);
CvPoint getCurrentTarget(CvPoint);
CvPoint RandomPoint(void);
bool isObstructed(CvPoint, int);
int lineHitsObstacles(CvPoint, CvPoint);
void writeObstacles(void);
void smoothenRRTPath(int);
void drawPath(IplImage*, CvScalar);

void initBots(void);
void getGoals(void);
void getBots(void);
void getBalls(void);
void drawBlobs(CBlobResult blobs, IplImage* img, CvScalar color);
CBlobResult extractBlobs(IplImage *img, uchar hueL, uchar hueU, uchar satL, uchar satU, uchar valL = VAL_L, uchar valU = VAL_U);

inline CvPoint getCenter(CBlob blob);
inline float angleOfBot(Bot* bot);
void printBlobArea(CBlobResult blob);
void printBotParams();

// Serial Port functions
extern "C"
{
    BOOL serialportInit(HANDLE *handle, const char *port, UINT32 baudRate);
    void SerialPutC(HANDLE hCom, char txchar);
    unsigned int SerialTX(HANDLE hCom, char* buff, unsigned int count);
    char SerialGetC(HANDLE hCom);
}

#endif
