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
CvPoint ballsPrev[TOT_BALLS] = {cvPoint(0,0)};

// image variables
IplImage* image;
// hsv and dst needed for extractBlobs (globals to avoid initialization on every call)
IplImage* hsv = cvCreateImage(cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), 8, 3);
IplImage* dst = cvCreateImage(cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), 8, 1);
IplImage* blobsImg = cvCreateImage(cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), 8, 3);

// serial port handler
HANDLE serialPort;


void init_capture(CvCapture* frame)
{
    frame = cvCaptureFromCAM(0);
}

void getNextFrame(CvCapture* frame, IplImage* img)
{
    if(!cvGrabFrame(frame))
    {
        printf("Could not grab a frame\n\7");
        return;
    }
    cout<<11<<endl;
    img = cvRetrieveFrame(frame);
}
    


int main()
{
    image = cvLoadImage("arena.jpg");
    /*
    getBalls();
    getBots();
    getGoals();

    cvNamedWindow("old", 1);
	cvNamedWindow("seg", 1);
	cvMoveWindow("old", 0,0);
	cvMoveWindow("seg", 0,400);

	cvShowImage("old", image);
	cvShowImage("seg", blobsImg);
    */

    cvNamedWindow("newImage", 1);
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

    /*
    CvCapture* capture = cvCreateCameraCapture(0);
    //CvCapture* capture = cvCaptureFromCAM(0); // capture from video device #0
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 640); 
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 480); 
    //cvSetCaptureProperty(capture, CV_CAP_PROP_FPS, 5); 
    
    cvQueryFrame(capture); 
    int fps=(int) cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
    int frameH    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
    int frameW    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
    cout<< frameH << " "<<frameW<<" "<<fps<<endl;
    
    
    //CvVideoWriter *writer = 0;
    //writer=cvCreateVideoWriter("out.avi", CV_FOURCC_DEFAULT, 5 ,cvSize(frameW,frameH), 1);
    //CV_FOURCC('U','2','6','3'),
    IplImage* newImage = 0; 
    for(int i = 0; i< 10; i++)
    {
        if(!cvGrabFrame(capture))
        {
            printf("Could not grab a frame\n\7");
            exit(0);
        }
        newImage=cvRetrieveFrame(capture);           // retrieve the captured frame
        //cvWriteFrame(writer, newImage);
        cvShowImage("newImage", newImage);
        cvWaitKey(1);
    }
    //cvReleaseVideoWriter(&writer);
    cvReleaseCapture(&capture);
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
        //printf("Blue Bot (x,y):\t%0.2f\t%0.2f\n", (float)blueBot[i].center.x, (float)blueBot[i].center.y);
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

    // Cirlces
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
            balls[i] = cvPoint(OUR_INF, OUR_INF);
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
        point = cvPoint((int)(getXCenter(blob)), (int)(getYCenter(blob)));
        blob.FillBlob(img, color);
        cvCircle(img, point, 2, white, -1);
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
    return cvPoint((int)(getXCenter(blob)), (int)(getYCenter(blob)));
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
