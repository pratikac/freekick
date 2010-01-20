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

#define BAUD_RATE               (38400)
#define TOT_BALLS               (4)

#define IMAGE_WIDTH             (640)
#define IMAGE_HEIGHT            (480)

#define CIRCLE_BOT_DIST         (40)

#define BALL_CAPTURE_DIST       (50)
#define DRIBBLER_START_DIST     (150)
#define OUR_INF                 (IMAGE_WIDTH*10)

#define RRT_OBSTACLE_CLEARANCE  (0.2)
#define RRT_MAX_NODES           (400)
#define RRT_THRESHOLD           (10)
#define RRT_EXTEND_DIST         (10)


// Robot Parameters (pixels)
#define ROBOT_RADIUS            (45)

// Probabilities
#define GOAL_PROB               (0.4)

//#define VERBOSE               (0)

// Speeds of Bot
#define TURN_SPEED              (128)
#define STRAIGHT_SPEED          (200)
#define WITH_BALL_FACTOR        (0.5)
#define MAX_SPEED               (250)

// Angle and Distance tolerances
#define ANGLE_TOLERANCE         (5)
#define NEXT_NODE_DIST          (20)
#define SHOOT_DIST              (80)

//Message Tokens
#define WHITE_TAG               (0x01111110)
#define BLACK_TAG               (0x10100101)

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

enum
{
    DRIBBLER_OFF = 0,
    DRIBBLER_IN,
    DRIBBLER_OUT
};

typedef struct Bot
{
    CvPoint center;
    float angle;
    CvPoint circleCenter;
    CvPoint rectCenter;
    int state;
    vector<CvPoint> currentPath;
    unsigned int currentNodeIndex;	//stores index of the current node in currentPath
    CvPoint currentDest;
    float angleDest;
    int ballIndex;
    int dribblerState;
    bool inTurn;

    unsigned char lMotorPWM, rMotorPWM;
    unsigned char lMotorDIR, rMotorDIR;
}
Bot;

// OUT OF SHEER FRUSTRATION
void set(CvPoint* dest, float x, float y)
{
	dest->x = (int)x;
	dest->y = (int)y;
}

enum 
{
    IDLE=0,
    MOVE_TO_BALL,
    CAPTURED,
    MOVE_TO_GOAL,
    SHOOT
};

enum
{
    FORWARD = 0,
    BACKWARD
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

unsigned int RRTPlan(Bot*);
CvPoint Extend(CvPoint, CvPoint, Bot*);
CvPointNode Nearest(CvPoint, unsigned int);
inline float dist(CvPoint, CvPoint);
CvPoint getCurrentTarget(CvPoint);
CvPoint RandomPoint(void);
bool isObstructed(CvPoint, Bot*);
int lineHitsObstacles(CvPoint, CvPoint);
void writeObstacles(void);
void drawPath(Bot* ,IplImage*, CvScalar);

void initBots(void);
void getGoals(void);
void getBots(void);
void getBalls(void);
void drawBlobs(CBlobResult blobs, IplImage* img, CvScalar color);
CBlobResult extractBlobs(IplImage *img, uchar hueL, uchar hueU, uchar satL, uchar satU, uchar valL = VAL_L, uchar valU = VAL_U);
void processPicture();

inline CvPoint getCenter(CBlob blob);
inline float angleOfBot(Bot* bot);
void printBlobArea(CBlobResult blob);
void printBotParams();

void getClosestBall(Bot* bot);
void getBotMove(Bot* bot);
void sendBotMove(Bot* bot);
// Serial Port functions
extern "C"
{
    BOOL serialportInit(HANDLE *handle, const char *port, UINT32 baudRate);
    void SerialPutC(HANDLE hCom, char txchar);
    unsigned int SerialTX(HANDLE hCom, char* buff, unsigned int count);
    char SerialGetC(HANDLE hCom);
}

#endif
