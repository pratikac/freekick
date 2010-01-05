#include "macros.h"
#include "cv.h"
#include "highgui.h"
#include "BlobResult.h"
#include <math.h>

#define IMG_WIDTH       (640)
#define IMG_HEIGHT		(480)

//segmentation #defs
// hs = [ (0,180), (0,256), (0,256) ]
#define MAX_BLOBS       (5)
#define BLOB_SIZE_MIN   (400)

#define BALL_HUE_U		(50)
#define BALL_HUE_L		(50)
#define BALL_SAT_U		(50)
#define BALL_SAT_L		(50)

#define GOAL_HUE_U		(108)
#define GOAL_HUE_L		(103)
#define GOAL_SAT_U		(210)
#define GOAL_SAT_L		(195)

#define VAL_U			(260)
#define VAL_L			(230)

IplImage *img = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), 8, 3);
IplImage *hsv = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), 8, 3);
IplImage *dst = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), 8, 1);

CBlob blobs[MAX_BLOBS];
CBlobResult blobRes;
int numOfBlobs = 0;

void imgTransform(uchar hueU, uchar hueL, uchar satU, uchar satL, uchar valU, uchar valL)
{
	unsigned int imgstep = 0, dststep = 0, channels = 0;
	int x = 0, y = 0;
	cvSmooth(img, img, CV_GAUSSIAN, 5, 5);
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

void extractBlobs()
{
	blobRes = CBlobResult(dst, NULL, 0);
	blobRes.Filter( blobRes, B_INCLUDE, CBlobGetArea(), B_GREATER, BLOB_SIZE_MIN );

	numOfBlobs = blobRes.GetNumBlobs();

	for(int i=0; i<numOfBlobs; i++)
		blobs[i] = blobRes.GetBlob(i);
};

void printBlobs()
{
	cvNamedWindow("old", 1);
	cvNamedWindow("new", 1);
	cvMoveWindow("old", 0,0);
	cvMoveWindow("new", 0,400);

	cvShowImage("old", img);
	cvShowImage("new", dst);
	cvWaitKey();

	CBlobGetXCenter getXC;
	CBlobGetYCenter getYC;

	printf("-----Printng Blobs------\n");
	for(int i=0; i<numOfBlobs; i++)
	{
		printf("%d\t(%3.2f,%3.2f)\n", i, getXC(blobs[i]), getYC(blobs[i]));		
	}
	printf("\n");
};

int main(void)
{
	img = cvLoadImage("./test.jpg");
    imgTransform(GOAL_HUE_U, GOAL_HUE_L, GOAL_SAT_U, GOAL_SAT_L, VAL_U, VAL_L);
    extractBlobs();
    printBlobs();
    return 0;
};

