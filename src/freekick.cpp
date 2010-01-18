#include "freekick.h"

// Display variables for segmented image
CvScalar white = CV_RGB(255, 255, 255);
CvScalar green = CV_RGB(0, 255, 0);
CvScalar red = CV_RGB(255, 0, 0);
CvScalar blue = CV_RGB(0, 0, 255);
CvScalar orange = CV_RGB(255, 175, 0);
CvScalar pink = CV_RGB(255, 0, 100);
CvScalar yellow = CV_RGB(255, 255, 0);
CvScalar cyan = CV_RGB(0, 255, 255);
CvScalar black = CV_RGB(0, 0, 0);

CvPoint redGoalMaskTopLeft, redGoalMaskBottomRight;
CvPoint blueGoalMaskTopLeft, blueGoalMaskBottomRight;

// game variables
CvPoint origin;
// [0] = WHITE, [1] = BLACK for ourBot
Bot redBot[2], blueBot[2], *opponentBot[2], *ourBot[2];
int ourColor = RED;                         // initialize this too
CvPoint blueGoal, redGoal, *ourGoal, *opponentGoal;

CvPoint balls[TOT_BALLS];                   // stores info of all balls
CvPoint *pballs;                            // pointer to iterate
int nBallsPresent = TOT_BALLS;
bool ballCaught = FALSE;
int closestBallIndex = 0;

// RRT vars
CvPoint Empty;
CvPointNode     path[MAX_NODES];
int             numObstacles = 6;
bool            getObstacles = true;
vector<CvPoint> lines;

// 4 robots + 2 Ds
// check the enum in the header
Obstacle        obs[6];


// image variables
IplImage* image;
// hsv and dst needed for extractBlobs (globals to avoid initialization on every call)
IplImage* hsv = cvCreateImage(cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), 8, 3);
IplImage* dst = cvCreateImage(cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), 8, 1);
IplImage* blobsImg = cvCreateImage(cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), 8, 3);

// serial port handler
HANDLE serialPort;


int main()
{
    initrand();

    image = cvLoadImage("arena.jpg");
    
    ourColor = BLUE;
    ourBot[BLACK]->state = IDLE;
    ourBot[WHITE]->state = IDLE;
    ourBot[BLACK]->current_dest = OUR_INF;
    ourBot[WHITE]->current_dest = OUR_INF;

    Empty.x = 2*IMAGE_WIDTH;
    Empty.y = 2*IMAGE_HEIGHT;

    //getBalls();
    getBots();
    getGoals();

    cvNamedWindow("old", 1);
	cvNamedWindow("seg", 1);
	cvMoveWindow("old", 0,0);
	cvMoveWindow("seg", 0,400);


    
    CvPoint dest;
    dest.x = 550;
    dest.y = 240;
    int total_nodes;
    total_nodes = RRTPlan(ourBot[WHITE]->center, dest, WHITE);
    smoothenRRTPath(total_nodes);
    drawPath(blobsImg, green);

	cvShowImage("old", image);
	cvShowImage("seg", blobsImg);

    cvWaitKey();




    CvPoint closestBall;

    while(nBallsPresent)
    {
        switch(ourBot[WHITE]->state)
        {
            case (IDLE):
                getClosestBall(ourBot[WHITE]);
                break;
            case (MOVE):
                total_nodes = RRTPlan(ourBot[WHITE]->center, closestBall, WHITE);
                smoothenRRTPath(total_nodes);
                moveToPoint(ourBot[WHITE]);
                


        //Bot1
        
        
        
        //Bot2










    }



    /*
    cvNamedWindow("newImage", 0);
    IplImage* newImage = cvCreateImage(cvSize(IMAGE_WIDTH,IMAGE_HEIGHT), 8, 3);
    videoInput vin;
    vin.setupDevice(0, IMAGE_WIDTH, IMAGE_HEIGHT);
    CvVideoWriter *writer = 0;
    writer=cvCreateVideoWriter("out.avi", CV_FOURCC_DEFAULT, 5 ,cvSize(IMAGE_WIDTH,frameH), 1);
    for(int i=0; i<1000; i++)
    {
        vin.getPixels(0, (unsigned char*)newImage->imageData, 0, 1);
        cvShowImage("newImage", newImage);
        cvWriteFrame(writer, newImage);
        cvWaitKey(20);
    }
    cvReleaseVideoWriter(&writer);
    vin.stopDevice(0);
    */


    return 0;
}


void getBots(void)
{
    CBlobResult blobs;
    CBlobResult blobsTemp;
    CvPoint whiteTemp, blackTemp;
    bool bGotCircle[2] = {FALSE, FALSE};        //[0] = RED, [1] = BLUE
    bool bGotRect[2] = {FALSE, FALSE};
    int i,j;

    // Red Bot
    blobs = extractBlobs(image, TEAM_R_HUE_L, TEAM_R_HUE_U, TEAM_R_SAT_L, TEAM_R_SAT_U);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, BOT_AREA_MAX);
    //cout<<"numofBlobs1 : "<<blobs.GetNumBlobs()<<endl;
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, BOT_AREA_MIN);
    //cout<<"numofBlobs2 : "<<blobs.GetNumBlobs()<<endl;
    drawBlobs(blobs, blobsImg, red);
    
    for(i=0; i < blobs.GetNumBlobs(); i++)
    {
        redBot[i].center = getCenter(blobs.GetBlob(i));
        //printf("Red Bot (x,y):\t%0.2f\t%0.2f\n", (float)redBot[i].center.x, (float)redBot[i].center.y);
    }

    // Blue Bot
    blobs = extractBlobs(image, TEAM_B_HUE_L, TEAM_B_HUE_U, TEAM_B_SAT_L, TEAM_B_SAT_U);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, BOT_AREA_MAX);
    //cout<<"numofBlobs1 : "<<blobs.GetNumBlobs()<<endl;
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, BOT_AREA_MIN);
    //cout<<"numofBlobs1 : "<<blobs.GetNumBlobs()<<endl;
    drawBlobs(blobs, blobsImg, cyan);

    for(i=0; i < blobs.GetNumBlobs(); i++)
    {
        blueBot[i].center = getCenter(blobs.GetBlob(i));
        printf("Blue Bot (x,y):\t%0.2f\t%0.2f\n", (float)blueBot[i].center.x, (float)blueBot[i].center.y);
    }

    // depending upon the segmentation of tags, change the *ourBot pointers to point to redBot[0] or redBot[1]
    // Note : always access bots in the logic code using these pointers

    //---------------------------------------------------------------------------------------------------
    // White Tags
    blobs = extractBlobs(image, TAG_W_HUE_L, TAG_W_HUE_U, TAG_W_SAT_L, TAG_W_SAT_U, TAG_W_VAL_L, TAG_W_VAL_U);
    
    // Circles
    blobsTemp = blobs;
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, TAG_CIRCLE_AREA_MAX);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, TAG_CIRCLE_AREA_MIN);
    //blobs.Filter(blobs, B_INCLUDE, CBlobGetCompactness(), B_GREATER, TAG_CIRCLE_COMPACTNESS_MIN);
    drawBlobs(blobs, blobsImg, white);
 
    bGotCircle[RED]  = FALSE;
    bGotCircle[BLUE] = FALSE;
    for(i = 0; i < blobs.GetNumBlobs(); i++)
    {
        whiteTemp = getCenter(blobs.GetBlob(i));
        if (dist(redBot[0].center, whiteTemp) < CIRCLE_BOT_DIST)
        {
            redBot[0].circleCenter = whiteTemp;
            bGotCircle[RED] = TRUE;
            if(ourColor == RED)
            {
                ourBot[WHITE] = &redBot[0];
                ourBot[BLACK] = &redBot[1];
            }
            else
            {
                opponentBot[WHITE] = &redBot[0];
                opponentBot[BLACK] = &redBot[1];
            }
        }
        else if (dist(redBot[1].center, whiteTemp) < CIRCLE_BOT_DIST)
        {
            redBot[1].circleCenter = whiteTemp;
            bGotCircle[RED] = TRUE;
            if(ourColor == RED)
            {
                ourBot[WHITE] = &redBot[1];
                ourBot[BLACK] = &redBot[0];
            }
            else
            {
                opponentBot[WHITE] = &redBot[1];
                opponentBot[BLACK] = &redBot[0];
            }
        }
        else if (dist(blueBot[0].center, whiteTemp) < CIRCLE_BOT_DIST)
        {
            blueBot[0].circleCenter = whiteTemp;
            bGotCircle[BLUE] = TRUE;
            if(ourColor == BLUE)
            {
                ourBot[WHITE] = &blueBot[0];
                ourBot[BLACK] = &blueBot[1];
            }
            else
            {
                opponentBot[WHITE] = &blueBot[0];
                opponentBot[BLACK] = &blueBot[1];
            }
        }
        else if (dist(blueBot[1].center, whiteTemp) < CIRCLE_BOT_DIST)
        {
            blueBot[1].circleCenter = whiteTemp;
            bGotCircle[BLUE] = TRUE;
            if(ourColor == BLUE)
            {
                ourBot[WHITE] = &blueBot[1];
                ourBot[BLACK] = &blueBot[0];
            }
            else
            {
                opponentBot[WHITE] = &blueBot[1];
                opponentBot[BLACK] = &blueBot[0];
            }
        }

        if(bGotCircle[RED] && bGotCircle[BLUE])
            break;
    }
    
    // Rect
    blobs = blobsTemp;
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, TAG_RECT_AREA_MAX);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, TAG_RECT_AREA_MIN);
    //blobs.Filter(blobs, B_INCLUDE, CBlobGetCompactness(), B_LESS, TAG_RECT_COMPACTNESS_MAX);
    drawBlobs(blobs, blobsImg, white);
 
    bGotRect[RED]  = FALSE;
    bGotRect[BLUE] = FALSE;
    for(i = 0; i < blobs.GetNumBlobs(); i++)
    {
        whiteTemp = getCenter(blobs.GetBlob(i));
        if (dist(redBot[0].center, whiteTemp) < CIRCLE_BOT_DIST)
        {
            redBot[0].rectCenter = whiteTemp;
            bGotRect[RED] = TRUE;
        }
        else if (dist(redBot[1].center, whiteTemp) < CIRCLE_BOT_DIST)
        {
            redBot[1].rectCenter = whiteTemp;
            bGotRect[RED] = TRUE;
        }
        else if (dist(blueBot[0].center, whiteTemp) < CIRCLE_BOT_DIST)
        {
            blueBot[0].rectCenter = whiteTemp;
            bGotRect[BLUE] = TRUE;
        }
        else if (dist(blueBot[1].center, whiteTemp) < CIRCLE_BOT_DIST)
        {
            blueBot[1].rectCenter = whiteTemp;
            bGotRect[BLUE] = TRUE;
        }

        if(bGotRect[RED] && bGotRect[BLUE])
            break;
    }
    //-----------------------------------------------------------------------------------------

    // Black Tags
    blobs = extractBlobs(image, TAG_B_HUE_L, TAG_B_HUE_U, TAG_B_SAT_L, TAG_B_SAT_U, TAG_B_VAL_L, TAG_B_VAL_U);

    // Circles
    blobsTemp = blobs;
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, TAG_CIRCLE_AREA_MAX);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, TAG_CIRCLE_AREA_MIN);
    //blobs.Filter(blobs, B_INCLUDE, CBlobGetCompactness(), B_GREATER, TAG_CIRCLE_COMPACTNESS_MIN);
    drawBlobs(blobs, blobsImg, black);
 
    bGotCircle[RED]  = FALSE;
    bGotCircle[BLUE] = FALSE;

    for(i = 0; i < blobs.GetNumBlobs(); i++)
    {
        blackTemp = getCenter(blobs.GetBlob(i));
        if (dist(redBot[0].center, blackTemp) < CIRCLE_BOT_DIST)
        {
            redBot[0].circleCenter = blackTemp;
            bGotCircle[RED] = TRUE;
            if(ourColor == RED)
            {
                ourBot[BLACK] = &redBot[0];
                ourBot[WHITE] = &redBot[1];
            }
            else
            {
                opponentBot[BLACK] = &redBot[0];
                opponentBot[WHITE] = &redBot[1];
            }
        }
        else if (dist(redBot[1].center, blackTemp) < CIRCLE_BOT_DIST)
        {
            redBot[1].circleCenter = blackTemp;
            bGotCircle[RED] = TRUE;
            if(ourColor == RED)
            {
                ourBot[BLACK] = &redBot[1];
                ourBot[WHITE] = &redBot[0];
            }
            else
            {
                opponentBot[BLACK] = &redBot[1];
                opponentBot[WHITE] = &redBot[0];
            }
        }
        else if (dist(blueBot[0].center, blackTemp) < CIRCLE_BOT_DIST)
        {
            blueBot[0].circleCenter = blackTemp;
            bGotCircle[BLUE] = TRUE;
            if(ourColor == BLUE)
            {
                ourBot[BLACK] = &blueBot[0];
                ourBot[WHITE] = &blueBot[1];
            }
            else
            {
                opponentBot[BLACK] = &blueBot[0];
                opponentBot[WHITE] = &blueBot[1];
            }
        }
        else if (dist(blueBot[1].center, blackTemp) < CIRCLE_BOT_DIST)
        {
            blueBot[1].circleCenter = blackTemp;
            bGotCircle[BLUE] = TRUE;
            if(ourColor == BLUE)
            {
                ourBot[BLACK] = &blueBot[1];
                ourBot[WHITE] = &blueBot[0];
            }
            else
            {
                opponentBot[BLACK] = &blueBot[1];
                opponentBot[WHITE] = &blueBot[0];
            }
        }

        if(bGotCircle[RED] && bGotCircle[BLUE])
            break;
    }
    
    // Rect
    blobs = blobsTemp;
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, TAG_RECT_AREA_MAX);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, TAG_RECT_AREA_MIN);
    //blobs.Filter(blobs, B_INCLUDE, CBlobGetCompactness(), B_LESS, TAG_RECT_COMPACTNESS_MAX);
    drawBlobs(blobs, blobsImg, black);
 
    bGotRect[RED]  = FALSE;
    bGotRect[BLUE] = FALSE;
    for(i = 0; i < blobs.GetNumBlobs(); i++)
    {
        blackTemp = getCenter(blobs.GetBlob(i));
        if (dist(redBot[0].center, blackTemp) < CIRCLE_BOT_DIST)
        {
            redBot[0].rectCenter = blackTemp;
            bGotRect[RED] = TRUE;
        }
        else if (dist(redBot[1].center, blackTemp) < CIRCLE_BOT_DIST)
        {
            redBot[1].rectCenter = blackTemp;
            bGotRect[RED] = TRUE;
        }
        else if (dist(blueBot[0].center, blackTemp) < CIRCLE_BOT_DIST)
        {
            blueBot[0].rectCenter = blackTemp;
            bGotRect[BLUE] = TRUE;
        }
        else if (dist(blueBot[1].center, blackTemp) < CIRCLE_BOT_DIST)
        {
            blueBot[1].rectCenter = blackTemp;
            bGotRect[BLUE] = TRUE;
        }

        if(bGotRect[RED] && bGotRect[BLUE])
            break;
    }
    //--------------------------------------------------------------------------

    for(i=0; i<2; i++)
    {
        ourBot[i]->angle = angleOfBot(ourBot[i]);
        opponentBot[i]->angle = angleOfBot(opponentBot[i]);
    }
    printBotParams();

    writeObstacles();
 
    return;
}

void getBalls(void)
{
    CBlobResult blobs;
	
    blobs = extractBlobs(image, BALL_HUE_L, BALL_HUE_U, BALL_SAT_L, BALL_SAT_U);
    
    nBallsPresent = blobs.GetNumBlobs();
    
    //printBlobArea(blobs);

    blobs.Filter(blobs, B_INCLUDE, CBlobGetArea(), B_GREATER, BALL_AREA_MIN);
    nBallsPresent = blobs.GetNumBlobs();
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, BALL_AREA_MAX);
    //blobs.Filter(blobs, B_EXCLUDE, CBlobGetCompactness(), B_GREATER, BALL_COMPACTNESS_MIN);
    // blobs.Filter(blobs, B_EXCLUDE, CBlobGetMean(), B_LESS, 200);
    drawBlobs(blobs, blobsImg, orange);
    nBallsPresent = blobs.GetNumBlobs();
    
    for(int i=0; i<TOT_BALLS; i++)
    {
        if(i < nBallsPresent)
        {
            // Update the locations of only those balls present
            balls[i] = getCenter(blobs.GetBlob(i));
            //ballsPrev[i] = balls[i];
        }
        else
        {
            balls[i].x = OUR_INF;
            balls[i].y = OUR_INF;
        }
    }

    if((nBallsPresent == 0) && (ballCaught == FALSE) )
        printf("Balls Over\n");
    
    return;
}

void getGoals(void)
{
    CBlobResult blobs;
    CBlob blob;
    
    CBlobGetXCenter getXCenter;
    CBlobGetYCenter getYCenter;

    
    // Red Goal
    blobs = extractBlobs(image, GOAL_R_HUE_L, GOAL_R_HUE_U, GOAL_R_SAT_L, GOAL_R_SAT_U);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, GOAL_AREA_MAX);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, GOAL_AREA_MIN);
    //blobs.Filter(blobs, B_INCLUDE, CBlobGetCompactness(), B_LESS, GOAL_COMPACTNESS_MAX);
    if(blobs.GetNumBlobs() != 0)
    {
        drawBlobs(blobs, blobsImg, red);
        blob = blobs.GetBlob(0);
        redGoal = getCenter(blob);
        //printf("Red Goal max_x: %d\n", (int)blob.MaxX());
    }

    // Blue Goal
    blobs = extractBlobs(image, GOAL_B_HUE_L, GOAL_B_HUE_U, GOAL_B_SAT_L, GOAL_B_SAT_U);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, GOAL_AREA_MAX);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, GOAL_AREA_MIN);
    //blobs.Filter(blobs, B_INCLUDE, CBlobGetCompactness(), B_LESS, GOAL_COMPACTNESS_MAX);
    if(blobs.GetNumBlobs() != 0)
    {

        drawBlobs(blobs, blobsImg, blue);
        blob = blobs.GetBlob(0);
        blueGoal = getCenter(blob);
        //printf("Blue Goal max_x: %d\n", (int)blob.MaxX());
    } 
    return;
}

void drawBlobs(CBlobResult blobs, IplImage* img, CvScalar color)
{
    if(blobs.GetNumBlobs() == 0)
        return;

    CBlob blob;
    CvPoint point;
    
    CBlobGetXCenter getXCenter;
    CBlobGetYCenter getYCenter;

    for (int x = 0; x < blobs.GetNumBlobs(); x++)
    {
        blob = blobs.GetBlob(x);
        point.x = (int)(getXCenter(blob));
        point.y = (int)(getYCenter(blob));
        blob.FillBlob(img, color);
        cvCircle(img, point, 2, white, -1);
    }
}

void drawPath(IplImage* img, CvScalar color)
{

    if(lines.size() == 0)
    {
        return;
    }
    
    for (int i = 0; i < lines.size()-1; i++)
    {
        cvLine(img, lines[i], lines[i+1], color);
    }
}

CBlobResult extractBlobs(IplImage* img, uchar hueL, uchar hueU, uchar satL, uchar satU, uchar valL, uchar valU)
{
    unsigned int imgstep = 0, dststep = 0, channels = 0;
    int x = 0, y = 0;
    CBlobResult blobs;
    cvCvtColor(img, hsv, CV_BGR2HSV);
    cvZero(dst);
    unsigned char h, s, v;
    uchar* hsvData = (uchar *)hsv->imageData;
    uchar* dstData = (uchar *)dst->imageData;
    imgstep = img->widthStep / sizeof (uchar);
    dststep = dst->widthStep / sizeof (uchar);
    channels = img->nChannels;
    for (y = 0; y < (img->height); y++)
    {
        for (x = 0; x < (img->width); x++)
        {
            h = hsvData[y * imgstep + x * channels + 0];
            s = hsvData[y * imgstep + x * channels + 1];
            v = hsvData[y * imgstep + x * channels + 2];
            if (hueL > hueU)
            {
                if (((h <= hueU) || (h >= hueL)) && ((s >= satL) && (s <= satU)) && ((v >= valL) && (v <= valU)))
                {
                    dstData[y * dststep + x] = 255;
                }
            }
            else
            {
                if (((h >= hueL) && (h <= hueU)) && ((s >= satL) && (s <= satU)) && ((v >= valL) && (v <= valU)))
                {
                    dstData[y * dststep + x] = 255;
                }
            }
        }
    }
    blobs = CBlobResult(dst, NULL, 0);
    return blobs;
}

inline CvPoint getCenter(CBlob blob)
{
    CBlobGetXCenter getXCenter;
    CBlobGetYCenter getYCenter;
    CvPoint temp;
    temp.x = (int)(getXCenter(blob));
    temp.y = (int)(getYCenter(blob));

    return temp;
}

inline float angleOfBot(Bot* bot)
{
    return (float)(180 / (CV_PI) * atan2((float)(bot->center.y - bot->circleCenter.y), (float)(bot->circleCenter.x - bot->center.x)));
}

void printBlobArea(CBlobResult blob)
{
    CBlobGetXCenter getXC;
	CBlobGetYCenter getYC;
    CBlobGetArea    getArea;
    CBlob tempBlob;
	
    for(int i=0; i<blob.GetNumBlobs(); i++)
	{
        tempBlob = blob.GetBlob(i);
		printf("%d\t(%3.2f,%3.2f),%3.2f \n", i, getXC(tempBlob), getYC(tempBlob), getArea(tempBlob));		
	}
	printf("\n");
}

void printBotParams()
{
    for(int i=0; i<2; i++)
        cout<<"Red: "<<i<<" x: "<<redBot[i].center.x<<" y: "<<redBot[i].center.y<<" angle: "<<redBot[i].angle<<endl;
    for(int i=0; i<2; i++)
        cout<<"Blue: "<<i<<" x: "<<blueBot[i].center.x<<" y: "<<blueBot[i].center.y<<" angle: "<<blueBot[i].angle<<endl;
}

void writeObstacles(void)
{
    // write obs[] array
    obs[OBS_D_L].center.x = -IMAGE_WIDTH/2;
    obs[OBS_D_L].center.y = 0;
    obs[OBS_D_L].radius = 400/SCALE_FACTOR + ROBOT_RADIUS;
    
    obs[OBS_D_R].center.x = IMAGE_WIDTH/2;
    obs[OBS_D_R].center.y = 0;
    obs[OBS_D_R].radius = 400/SCALE_FACTOR + ROBOT_RADIUS;
    
    if(ourColor == RED)
    {
        obs[OBS_RED_W].center = ourBot[WHITE]->center;
        obs[OBS_RED_W].radius = ROBOT_RADIUS;
        obs[OBS_RED_B].center = ourBot[BLACK]->center;
        obs[OBS_RED_B].radius = ROBOT_RADIUS;

        obs[OBS_BLUE_W].center = opponentBot[WHITE]->center;
        obs[OBS_BLUE_W].radius = ROBOT_RADIUS;
        obs[OBS_BLUE_B].center = opponentBot[BLACK]->center;
        obs[OBS_BLUE_B].radius = ROBOT_RADIUS;
    }
    else if(ourColor == BLUE)
    {
        obs[OBS_RED_W].center = opponentBot[WHITE]->center;
        obs[OBS_RED_W].radius = ROBOT_RADIUS;
        obs[OBS_RED_B].center = opponentBot[BLACK]->center;
        obs[OBS_RED_B].radius = ROBOT_RADIUS;

        obs[OBS_BLUE_W].center = ourBot[WHITE]->center;
        obs[OBS_BLUE_W].radius = ROBOT_RADIUS;
        obs[OBS_BLUE_B].center = ourBot[BLACK]->center;
        obs[OBS_BLUE_B].radius = ROBOT_RADIUS;
    }
};

unsigned int RRTPlan(CvPoint initial, CvPoint goal, int botColor)
{
    CvPointNode nearest;
    CvPoint extended, target;
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
        if ( (extended.x != Empty.x) || (extended.y != Empty.y))
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

void smoothenRRTPath(int num_nodes)
{
    int i = num_nodes - 1;
    CvPoint prevPoint, startPoint, endPoint;
    startPoint = path[0].point;
    printf("%d, %d\n", startPoint.x, startPoint.y);
    lines.clear();
    lines.push_back(startPoint);
    bool done = false;
    while(!done)
    {
        endPoint = path[i].point;
        if(lineHitsObstacles(startPoint, endPoint))
        {
            i = path[i].parentIndex;
            endPoint = path[i].point;
#ifdef VERBOSE
            fprintf(stderr, "trying point %d/%d: %d, %d\n",
                    i, num_nodes-1, endPoint.x, endPoint.y);
#endif
        }
        else
        {
            startPoint = endPoint;
            printf("%d, %d\n", startPoint.x, startPoint.y);
            lines.push_back(startPoint);
            i = num_nodes - 1;
            if((startPoint.x == path[num_nodes - 1].point.x) && (startPoint.y == path[num_nodes - 1].point.y))
                done = true;
        }
    }
    printf("LinesDone\n");
}


CvPoint Extend(CvPoint nearest, CvPoint target, int botColor)
{
    CvPoint extended = Empty;
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

CvPointNode Nearest(CvPoint target, unsigned int num_nodes)
{
    CvPointNode nearest;
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

bool isObstructed(CvPoint p, int botColor)
{
    int i;
    int retFlag = 0;
    for(i = 0; i < 2; i++)
    {
        if(ourColor == RED)
        {
            if(dist(blueBot[i].center, p) < (2*ROBOT_RADIUS) ) 
            {
                retFlag  = 1;
            }
        }
        else
        {
            if(dist(redBot[i].center, p) < (2*ROBOT_RADIUS) ) 
            {
                retFlag  = 1;
            }
        }
    }
    if(botColor == WHITE)
    {
        if(dist(ourBot[BLACK]->center, p) < (2*ROBOT_RADIUS) ) 
        {
            retFlag  = 1;
        }
    }
    else if(botColor == BLACK)
    {
        if(dist(ourBot[WHITE]->center, p) < (2*ROBOT_RADIUS) ) 
        {
            retFlag  = 1;
        }
    }

    if(  ( p.x <= (DEE_LEFT_NORTH_X + ROBOT_RADIUS)  ))// || (p.x >= (DEE_RIGHT_NORTH_X - ROBOT_RADIUS)) ))
    {
        retFlag = 1;
    }

    return retFlag;
}

inline float dist(CvPoint i, CvPoint j)
{
    return sqrt( (float)((j.x - i.x)*(j.x - i.x) + (j.y - i.y)*(j.y - i.y)) );
}

CvPoint getCurrentTarget(CvPoint goal)
{
    float p;
    p = randfloat();
    if (p < GOAL_PROB)
        return goal;
    else
        return RandomPoint();
}

CvPoint RandomPoint(void)
{
    CvPoint random;
    random.x = randint(IMAGE_WIDTH);
    random.y = randint(IMAGE_HEIGHT);
    return random;
}

int lineHitsObstacles(CvPoint start, CvPoint end)
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

    for(int i = 2; i < 6; i++)
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
