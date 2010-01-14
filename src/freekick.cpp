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
CvPoint ballsPrev[TOT_BALLS] = {cvPoint(0,0)};

// image variables
IplImage* img;
IplImage* blobsImg;
// hsv and dst needed for extractBlobs (globals to avoid initialization on every call)
IplImage* hsv = cvCreateImage(cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), 8, 3);
IplImage* dst = cvCreateImage(cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), 8, 1);

// serial port handler
HANDLE serialPort;

void getBots(void)
{
    CBlobResult blobs;
    CBlobResult blobsTemp;
    CvPoint whiteTemp, blackTemp;
    bool bGotCircle[2] = {FALSE, FALSE};        //[0] = RED, [1] = BLUE
    bool bGotRect[2] = {FALSE, FALSE};
    int i,j;

    // Red Bot
    blobs = extractBlobs(img, redBot[0].HUE_L, redBot[0].HUE_U, redBot[0].SAT_L, redBot[0].SAT_U);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, redBot[0].AREA_MAX);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, redBot[0].AREA_MIN);
    drawBlobs(blobs, blobsImg, red);
    
    i = 0;      // blob iter
    j = 0;      // bot iter
    while( (i < blobs.GetNumBlobs()) && (j < 2))
    {
        if (dist(redGoal, redBot[j].center) > MIN_GOAL_DIST)
        {
            redBot[j].center = getCenter(blobs.GetBlob(i));
            printf("Red Bot (x,y):\t%0.2f\t%0.2f\n", (float)redBot[j].center.x, (float)redBot[j].center.y);
            j++;
        }
        i++;
    }

    // Blue Bot
    blobs = extractBlobs(img, blueBot[0].HUE_L, blueBot[0].HUE_U, blueBot[0].SAT_L, blueBot[0].SAT_U);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, blueBot[0].AREA_MAX);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, blueBot[0].AREA_MIN);
    drawBlobs(blobs, blobsImg, blue);
    
    i = 0;      // blob iter
    j = 0;      // bot iter
    while( (i < blobs.GetNumBlobs()) && (j < 2))
    {
        if (dist(blueGoal, blueBot[j].center) > MIN_GOAL_DIST)
        {
            blueBot[j].center = getCenter(blobs.GetBlob(i));
            printf("Blue Bot (x,y):\t%0.2f\t%0.2f\n", (float)blueBot[j].center.x, (float)blueBot[j].center.y);
            j++;
        }
        i++;
    }

    // depending upon the segmentation of tags, change the *ourBot pointers to point to redBot[0] or redBot[1]
    // Note : always access bots in the logic code using these pointers

    //---------------------------------------------------------------------------------------------------
    // White Tags
    blobs = extractBlobs(img, 0, 255, TAG_W_SAT_L, TAG_W_SAT_U, TAG_W_VAL_L, TAG_W_VAL_U);

    // Cirlces
    blobsTemp = blobs;
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, TAG_CIRCLE_AREA_MAX);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, TAG_CIRCLE_AREA_MIN);
    blobs.Filter(blobs, B_INCLUDE, CBlobGetCompactness(), B_GREATER, TAG_CIRCLE_COMPACTNESS_MIN);
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
                ourBot[WHITE] = &redBot[0];
            else
                opponentBot[WHITE] = &redBot[0];
        }
        else if (dist(redBot[1].center, whiteTemp) < CIRCLE_BOT_DIST)
        {
            redBot[1].circleCenter = whiteTemp;
            bGotCircle[RED] = TRUE;
            if(ourColor == RED)
                ourBot[WHITE] = &redBot[1];
            else
                opponentBot[WHITE] = &redBot[1];
        }
        else if (dist(blueBot[0].center, whiteTemp) < CIRCLE_BOT_DIST)
        {
            blueBot[0].circleCenter = whiteTemp;
            bGotCircle[BLUE] = TRUE;
            if(ourColor == BLUE)
                ourBot[WHITE] = &blueBot[0];
            else
                opponentBot[WHITE] = &blueBot[0];
        }
        else if (dist(blueBot[1].center, whiteTemp) < CIRCLE_BOT_DIST)
        {
            blueBot[1].circleCenter = whiteTemp;
            bGotCircle[BLUE] = TRUE;
            if(ourColor == BLUE)
                ourBot[WHITE] = &blueBot[1];
            else
                opponentBot[WHITE] = &blueBot[1];
        }

        if(bGotCircle[RED] && bGotCircle[BLUE])
            break;
    }
  
    // Rect
    blobs = blobsTemp;
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, TAG_RECT_AREA_MAX);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, TAG_RECT_AREA_MIN);
    blobs.Filter(blobs, B_INCLUDE, CBlobGetCompactness(), B_LESS, TAG_RECT_COMPACTNESS_MAX);
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
    blobs = extractBlobs(img, 0, 255, TAG_B_SAT_L, TAG_B_SAT_U, TAG_B_VAL_L, TAG_B_VAL_U);

    // Cirlces
    blobsTemp = blobs;
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, TAG_CIRCLE_AREA_MAX);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, TAG_CIRCLE_AREA_MIN);
    blobs.Filter(blobs, B_INCLUDE, CBlobGetCompactness(), B_GREATER, TAG_CIRCLE_COMPACTNESS_MIN);
    drawBlobs(blobs, blobsImg, white);
 
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
                ourBot[BLACK] = &redBot[0];
            else
                opponentBot[BLACK] = &redBot[0];
        }
        else if (dist(redBot[1].center, blackTemp) < CIRCLE_BOT_DIST)
        {
            redBot[1].circleCenter = blackTemp;
            bGotCircle[RED] = TRUE;
            if(ourColor == RED)
                ourBot[BLACK] = &redBot[1];
            else
                opponentBot[BLACK] = &redBot[1];
        }
        else if (dist(blueBot[0].center, blackTemp) < CIRCLE_BOT_DIST)
        {
            blueBot[0].circleCenter = blackTemp;
            bGotCircle[BLUE] = TRUE;
            if(ourColor == BLUE)
                ourBot[BLACK] = &blueBot[0];
            else
                opponentBot[BLACK] = &blueBot[0];
        }
        else if (dist(blueBot[1].center, blackTemp) < CIRCLE_BOT_DIST)
        {
            blueBot[1].circleCenter = blackTemp;
            bGotCircle[BLUE] = TRUE;
            if(ourColor == BLUE)
                ourBot[BLACK] = &blueBot[1];
            else
                opponentBot[BLACK] = &blueBot[1];
        }

        if(bGotCircle[RED] && bGotCircle[BLUE])
            break;
    }
  
    // Rect
    blobs = blobsTemp;
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, TAG_RECT_AREA_MAX);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, TAG_RECT_AREA_MIN);
    blobs.Filter(blobs, B_INCLUDE, CBlobGetCompactness(), B_LESS, TAG_RECT_COMPACTNESS_MAX);
    drawBlobs(blobs, blobsImg, white);
 
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
    return;
}

void getBalls(void)
{
    CBlobResult blobs;
    static unsigned int frameCount = 0;
    static float nBallsDetectedAvg = 0;

    int i=0;

    if(frameCount == BALL_DETECTION_FRAMES_TO_AVG)
    {
        nBallsPresent = ceil(nBallsDetectedAvg / BALL_DETECTION_FRAMES_TO_AVG);
        nBallsDetectedAvg = 0;
        frameCount = 0;
    }

    blobs = extractBlobs(img, BALL_HUE_L, BALL_HUE_U, BALL_SAT_L, BALL_SAT_U);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, BALL_AREA_MAX);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, BALL_AREA_MIN);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetCompactness(), B_GREATER, BALL_COMPACTNESS_MAX);
    // blobs.Filter(blobs, B_EXCLUDE, CBlobGetMean(), B_LESS, 200);
    drawBlobs(blobs, blobsImg, orange);
    nBallsDetectedAvg += blobs.GetNumBlobs();
    
    for(i=0; i<TOT_BALLS; i++)
    {
        if(i < nBallsPresent)
        {
            // Update the locations of only those balls present
            if( i < blobs.GetNumBlobs() )
            {
                balls[i] = getCenter(blobs.GetBlob(i));
                ballsPrev[i] = balls[i];
                //fprintf(locations, "Yellow Ball No.%d\t%d\t%d\n", i+1,yellowBalls[i].x,yellowBalls[i].y);
            }
            else
                balls[i] = ballsPrev[i];
        }
        else
            balls[i] = cvPoint(OUR_INF, OUR_INF);
    }

    frameCount++;
    if((nBallsPresent == 0) && (ballCaught == FALSE) )
        printf("Balls Over\n");
    return;
}

void getGoals(void)
{
    CBlobResult blobs;
    CBlob blob;
    CBlobGetMean mean;
    
    // Red Goal
    blobs = extractBlobs(img, GOAL_R_HUE_L, GOAL_R_HUE_U, GOAL_R_SAT_L, GOAL_R_SAT_U);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, GOAL_AREA_MAX);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, GOAL_AREA_MIN);
    blobs.Filter(blobs, B_INCLUDE, CBlobGetCompactness(), B_LESS, GOAL_COMPACTNESS_MAX);
    
    // if the bot color also comes up due to bad thresholds, we need to choose the right most blob
    if((blobs.GetNumBlobs() == 1) || (blobs.GetNumBlobs() == 2) )
    {
        drawBlobs(blobs, blobsImg, red);
        if (blobs.GetNumBlobs() == 1)
        {
            blob = blobs.GetBlob(0);
        }
        else if(blobs.GetNumBlobs() == 2)
        {
            if( mean(blobs.GetBlob(0)) > mean(blobs.GetBlob(1)) ) // whichever is rightmost
                blob = blobs.GetBlob(0);
            else
                blob = blobs.GetBlob(1);
        }
        redGoal = getCenter(blob);
        redGoalMaskTopLeft = cvPoint((int)blob.MinX(), (int)blob.MinY());
        redGoalMaskBottomRight = cvPoint((int)blob.MaxX(), (int)blob.MaxY());
        printf("Red Goal max_x: %d\n", (int)blob.MaxX());
        //fprintf(locations, "Red Goal(x,y):\t%0.2f\t%0.2f\n", (float)redGoal.x, (float)redGoal.y);
    }
    
    // Blue Goal
    blobs = extractBlobs(img, GOAL_B_HUE_L, GOAL_B_HUE_U, GOAL_B_SAT_L, GOAL_B_SAT_U);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, GOAL_AREA_MAX);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, GOAL_AREA_MIN);
    blobs.Filter(blobs, B_INCLUDE, CBlobGetCompactness(), B_LESS, GOAL_COMPACTNESS_MAX);
    
    // if the bot color also comes up due to bad thresholds, we need to choose the right most blob
    if((blobs.GetNumBlobs() == 1) || (blobs.GetNumBlobs() == 2) )
    {
        drawBlobs(blobs, blobsImg, blue);
        if (blobs.GetNumBlobs() == 1)
        {
            blob = blobs.GetBlob(0);
        }
        else if(blobs.GetNumBlobs() == 2)
        {
            if(blobs.GetBlob(0)->SumX() < blobs.GetBlob(1)->SumX()) // Whichever is leftmost
                blob = blobs.GetBlob(0);
            else
                blob = blobs.GetBlob(1);
        }
        blueGoal = getCenter(blob);
        blueGoalMaskTopLeft = cvPoint((int)blob.MinX(), (int)blob.MinY());
        blueGoalMaskBottomRight = cvPoint((int)blob.MaxX(), (int)blob.MaxY());
        printf("Blue Goal max_x: %d\n", (int)blob.MaxX());
        //fprintf(locations, "Blue Goal(x,y):\t%0.2f\t%0.2f\n", (float)blueGoal.x, (float)blueGoal.y);
    }
    return;
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
    blobs = CBlobResult(dst, NULL, 100);
    return blobs;
}

inline CvPoint getCenter(CBlob blob)
{
    return cvPoint((int)(blob.SumX()), (int)(blob.SumY()));
}

inline float angleOfBot(Bot* bot)
{
    return (float)(180 / (CV_PI) * atan2((float)(bot->circleCenter.y - bot->rectCenter.y), (float)(bot->circleCenter.x - bot->rectCenter.x)));
}

inline float dist(CvPoint p1, CvPoint p2)
{
    return sqrt((float)((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y)));
}
