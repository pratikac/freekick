#ifndef __RRT__
#define __RRT__

#include <cv.h>
#include <random.h>
#include <freekick.h>

#define EPSILON         (0.2)
#define MAX_NODES       (400)

#define THRESHOLD       (10)
#define EXTEND_DIST     (10)

#define GOAL_X          (500)
#define GOAL_Y          (150)

// Robot Parameters (pixels)
#define ROBOT_RADIUS    (35)

// Probabilities
#define GOAL_PROB       (0.4)

#define VERBOSE         (0)
typedef struct
{
    cvPoint point;
    int parentIndex;
}cvPointNode;

class Obstacle
{
    public:
        Point center;
        unsigned int radius;

        Obstacle(int xc = 0, int yc = 0, unsigned int r = 0)
        {
            center = Point(xc,yc);
            radius = r;
        }

};

unsigned int RRTPlan(cvPoint, cvPoint, int);
cvPoint Extend(cvPoint, cvPoint, int);
cvPointNode Nearest(cvPoint, unsigned int);
inline float dist(cvPoint, cvPoint);
cvPoint getCurrentTarget(cvPoint);
cvPoint RandomPoint(void);
bool isObstructed(cvPoint, int);
int lineHitsObstacles(cvPoint, cvPoint);

#endif
