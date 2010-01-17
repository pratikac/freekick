#include <rrt.h>

cvPoint Empty = Point(2*IMAGEWIDTH, 2*IMAGEHEIGHT);
cvPointNode     path[MAX_NODES];
int             numObstacles;
Obstacle        obs[2];
bool            getObstacles = true;

// 2 opponents + 2 Ds, add ownBot manually
Obstacle    obs[2];

unsigned int RRTPlan(cvPoint initial, cvPoint goal, int botColor)
{
    cvPointNode nearest;
    cvPoint extended, target;
    unsigned int i = 0;
    nearest.point = initial;
    // The first point in the path is the starting point
    path[0].point = initial;
    path[0].parentIndex = 0;
    i = 1;
    while( (dist(nearest.point, goal) > THRESHOLD) && (i < MAX_NODES) )
    {
        target = getCurrentTarget(goal);
#ifdef VERBOSE
        fprintf(stderr, "Got target: (%d, %d)\n", target.x, target.y);
#endif
        nearest = Nearest(target, i);
        extended = Extend(nearest.point, target, botColor);
        if (extended != Empty)
        {
            path[i].point = extended;
            path[i].parentIndex = nearest.parentIndex;
            i++;
        }
    }
#ifdef VERBOSE
    if( i == MAX_NODES)
        fprintf(stderr, "MAX_NODES REACHED!\n");
#endif
    return i;
}

cvPoint Extend(cvPoint nearest, cvPoint target, int botColor)
{
    cvPoint extended = Empty;
    unsigned int distance = (unsigned int) dist(nearest, target);
    if ( distance < EXTEND_DIST )
        extended = target;
    else
    {
        extended.x = (int)(nearest.x + (float)(target.x - nearest.x)*EXTEND_DIST/distance);
        extended.y = (int)(nearest.y + (float)(target.y - nearest.y)*EXTEND_DIST/distance);
#ifdef VERBOSE
        fprintf(stderr, "nearest: (%d, %d), target: (%d, %d), extended: (%d, %d)\n",
                nearest.x, nearest.y, target.x, target.y, extended.x, extended.y);
#endif
    }

    if ( isObstructed(extended, botColor) )
    {
#ifdef VERBOSE
        fprintf(stderr, "Collision: (%d, %d)\n", extended.x, extended.y);
#endif
        return Empty;
    }
    else
        return extended;
}

cvPointNode Nearest(cvPoint target, unsigned int num_nodes)
{
    cvPointNode nearest;
    unsigned int min_dist = UINT_MAX;
    unsigned int cur_dist = UINT_MAX;
    nearest.point = path[0].point;

    for (unsigned int i = 0; i < num_nodes; i++)
    {
        cur_dist = (unsigned int) dist(path[i].point, target);
        if (cur_dist < min_dist)
        {
            min_dist = cur_dist;
            nearest = path[i];
            nearest.parentIndex = i;
        }
    }
    return nearest;
}

bool isObstructed(cvPoint p, int botColor)
{
    int i;
    int retFlag = 0;
    for(i = 0; i < 2; i++)
    {
        if(ourColor == RED)
        {
            if(dist(blueBot[i].center, p) < (blueBot[i].radius + ROBOT_RADIUS) ) 
                retFlag  = 1;
        }
        else
        {
            if(dist(redBot[i].center, p) < (redBot[i].radius + ROBOT_RADIUS) ) 
                retFlag  = 1;
        }
    }
    if(botColor == WHITE)
    {
        if(dist(ourBot[BLACK]->center, p) < (ourBot[BLACK]->radius + ROBOT_RADIUS) ) 
            retFlag  = 1;
    }
    else if(botColor == BLACK)
    {
        if(dist(ourBot[WHITE]->center, p) < (ourBot[WHITE]->radius + ROBOT_RADIUS) ) 
            retFlag  = 1;
    }

    if( ( ABS(p.x) >= (ABS(GOAL_LEFT_NORTH_X) - ROBOT_RADIUS) ) && ( ABS(p.y) <= (ABS(GOAL_LEFT_NORTH_Y) + ROBOT_RADIUS) ) )
        retFlag = 1;

    return retFlag;
}

inline float dist(cvPoint i, cvPoint j)
{
    return sqrt( (float)((j.x - i.x)*(j.x - i.x) + (j.y - i.y)*(j.y - i.y)) );
}

cvPoint getCurrentTarget(cvPoint goal)
{
    float p;
    p = randfloat();
    if (p < GOAL_PROB)
        return goal;
    else
        return RandomcvPoint();
}

cvPoint RandomcvPoint(void)
{
    cvPoint random;
    random.x = randint(IMAGE_WIDTH);
    random.y = randint(IMAGE_HEIGHT);
    return random;
}

int lineHitsObstacles(cvPoint start, cvPoint end)
{
    /*
     * Given that p1 and p2 both lie outside the obstacle, the line segment between p1
     * and p2 hits an obstacle iff:
     * 1) The distance between the line joining p1, p2 is less than the obstacle radius.
     * 2) The projection of the center of the obstacle is lies between p1 and p2.
     */

    float xp, yp, dotProduct;
    int xo, yo; // Obstacle x, y co-ordinates
    float obsDist;

    float a = end.y - start.y;
    float b = start.x - end.x;
    float c = (end.x - start.x)*start.y - (end.y - start.y)*start.x;

#ifdef VERBOSE
    fprintf(stderr, "checking points: (%d, %d), (%d, %d)\n", start.x, start.y,
            end.x, end.y);
#endif

    for(int i = 0; i < 2; i++)
    {
        xo = obs[i].center.x;
        yo = obs[i].center.y;

        xp = xo - a*(a*xo + b*yo + c)/(a*a + b*b);
        yp = yo - b*(a*xo + b*yo + c)/(a*a + b*b);

        obsDist = sqrt((xp - xo)*(xp - xo) + (yp -yo)*(yp -yo));
#ifdef VERBOSE
        fprintf(stderr, "projected: (%f, %f), dist: %f\n", xp, yp, obsDist);
#endif
        if( obsDist + EPSILON <  (obs[i].radius + ROBOT_RADIUS))
        {
            dotProduct = (start.x - xp)*(end.x - xp)\
                         + (start.y - yp)*(end.y - yp);

            if(dotProduct < 0)
            {
#ifdef VERBOSE
                fprintf(stderr, "HIT\n");
#endif
                return 1;
            }
        }
    }

    return 0;
}
