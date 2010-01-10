#include <math.h>
#include "macros.h"
#include "cv.h"
#include "highgui.h"
#include "BlobResult.h"
#include <iostream>
using namespace std;

#include "thresholds.h"
#include "geometry.h"


#define IMG_WIDTH       (640)
#define IMG_HEIGHT		(480)

#define MAX_BLOBS       (50)
#define BLOB_SIZE_MIN   (200)
#define BALL_SIZE_MAX   (250)
#define BALL_COMPACTNESS (5)

IplImage *img = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), 8, 3);
IplImage *original_img = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), 8, 3);
IplImage *hsv = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), 8, 3);
IplImage *dst = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), 8, 1);

CBlob blobs[MAX_BLOBS];
CBlobResult blobRes;
int numOfBlobs = 0;

void imgTransform(uchar hueU, uchar hueL, uchar satU, uchar satL, uchar valU, uchar valL)
{
	unsigned int imgstep = 0, dststep = 0, channels = 0;
	int x = 0, y = 0;
	cvSmooth(original_img, img, CV_GAUSSIAN, 5, 5);
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
};

void extractBall()
{
    imgTransform(BALL_HUE_U, BALL_HUE_L, BALL_SAT_U, BALL_SAT_L, VAL_U, VAL_L);

	blobRes = CBlobResult(dst, NULL, 0);
	blobRes.Filter( blobRes, B_EXCLUDE, CBlobGetArea(), B_LESS, BLOB_SIZE_MIN );// keep blobs larger than BLOB_SIZE_MIN
	numOfBlobs = blobRes.GetNumBlobs(); cout << numOfBlobs << endl;
	blobRes.Filter( blobRes, B_EXCLUDE, CBlobGetArea(), B_GREATER, BALL_SIZE_MAX );// keep blobs smaller than BALL_SIZE_MAX
	numOfBlobs = blobRes.GetNumBlobs(); cout << numOfBlobs << endl;
	blobRes.Filter( blobRes, B_INCLUDE, CBlobGetCompactness(), B_GREATER, BALL_COMPACTNESS );// keep blobs smaller than BALL_COMPACTNESS
	numOfBlobs = blobRes.GetNumBlobs(); cout << numOfBlobs << endl;

	for(int i=0; i<numOfBlobs; i++)
		blobs[i] = blobRes.GetBlob(i);
};

void extractBots()
{
    //RED TEAM
    imgTransform(TEAM_R_HUE_U, TEAM_R_HUE_L, TEAM_R_SAT_U, TEAM_R_SAT_L, VAL_U, VAL_L);
	blobRes = CBlobResult(dst, NULL, 0);
	blobRes.Filter( blobRes, B_EXCLUDE, CBlobGetArea(), B_LESS, BLOB_SIZE_MIN );// keep blobs larger than BLOB_SIZE_MIN
	numOfBlobs = blobRes.GetNumBlobs(); cout << numOfBlobs << endl;
    if(numOfBlobs == 2)
    {
        for (int i=0; i<2; i++)
            blobRes.GetBlob(i)

	for(int i=0; i<numOfBlobs; i++)
		blobs[i] = blobRes.GetBlob(i);
};


void printBlobs()
{

	CBlobGetXCenter getXC;
	CBlobGetYCenter getYC;
    CBlobGetArea    getArea;
    CBlobGetCompactness getCompactness;


	printf("-----Printng Blobs------\n");
	for(int i=0; i<numOfBlobs; i++)
	{
		printf("%d\t(%3.2f,%3.2f),%3.2f %3.2f\n", i, getXC(blobs[i]), getYC(blobs[i]), getArea(blobs[i]), getCompactness(blobs[i]));		
	}
	printf("\n");

	cvNamedWindow("old", 1);
	cvNamedWindow("new", 1);
	cvMoveWindow("old", 0,0);
	cvMoveWindow("new", 0,400);

	cvShowImage("old", img);
	cvShowImage("new", dst);
	cvWaitKey();

};

int main(void)
{
	original_img = cvLoadImage("./arena.jpg");
    extractBots();
    //extractBall();
    cout<<"Extracted!";
    printBlobs();
    return 0;
};

