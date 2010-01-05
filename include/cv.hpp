/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#ifndef _CV_HPP_
#define _CV_HPP_

#ifdef __cplusplus

namespace cv
{

enum { BORDER_REPLICATE=IPL_BORDER_REPLICATE, BORDER_CONSTANT=IPL_BORDER_CONSTANT,
       BORDER_REFLECT=IPL_BORDER_REFLECT, BORDER_REFLECT_101=IPL_BORDER_REFLECT_101,
       BORDER_REFLECT101=BORDER_REFLECT_101, BORDER_WRAP=IPL_BORDER_WRAP,
       BORDER_TRANSPARENT, BORDER_DEFAULT=BORDER_REFLECT_101, BORDER_ISOLATED=16 };

CV_EXPORTS int borderInterpolate( int p, int len, int borderType );

class CV_EXPORTS BaseRowFilter
{
public:
    BaseRowFilter();
    virtual ~BaseRowFilter();
    virtual void operator()(const uchar* src, uchar* dst,
                            int width, int cn) = 0;
    int ksize, anchor;
};


class CV_EXPORTS BaseColumnFilter
{
public:
    BaseColumnFilter();
    virtual ~BaseColumnFilter();
    virtual void operator()(const uchar** src, uchar* dst, int dststep,
                            int dstcount, int width) = 0;
    virtual void reset();
    int ksize, anchor;
};


class CV_EXPORTS BaseFilter
{
public:
    BaseFilter();
    virtual ~BaseFilter();
    virtual void operator()(const uchar** src, uchar* dst, int dststep,
                            int dstcount, int width, int cn) = 0;
    virtual void reset();
    Size ksize;
    Point anchor;
};


class CV_EXPORTS FilterEngine
{
public:
    FilterEngine();
    FilterEngine(const Ptr<BaseFilter>& _filter2D,
                 const Ptr<BaseRowFilter>& _rowFilter,
                 const Ptr<BaseColumnFilter>& _columnFilter,
                 int srcType, int dstType, int bufType,
                 int _rowBorderType=BORDER_REPLICATE,
                 int _columnBorderType=-1,
                 const Scalar& _borderValue=Scalar());
    virtual ~FilterEngine();
    void init(const Ptr<BaseFilter>& _filter2D,
              const Ptr<BaseRowFilter>& _rowFilter,
              const Ptr<BaseColumnFilter>& _columnFilter,
              int srcType, int dstType, int bufType,
              int _rowBorderType=BORDER_REPLICATE, int _columnBorderType=-1,
              const Scalar& _borderValue=Scalar());
    virtual int start(Size wholeSize, Rect roi, int maxBufRows=-1);
    virtual int start(const Mat& src, const Rect& srcRoi=Rect(0,0,-1,-1),
                      bool isolated=false, int maxBufRows=-1);
    virtual int proceed(const uchar* src, int srcStep, int srcCount,
                        uchar* dst, int dstStep);
    virtual void apply( const Mat& src, Mat& dst,
                        const Rect& srcRoi=Rect(0,0,-1,-1),
                        Point dstOfs=Point(0,0),
                        bool isolated=false);
    bool isSeparable() const { return (const BaseFilter*)filter2D == 0; }
    int remainingInputRows() const;
    int remainingOutputRows() const;
    
    int srcType, dstType, bufType;
    Size ksize;
    Point anchor;
    int maxWidth;
    Size wholeSize;
    Rect roi;
    int dx1, dx2;
    int rowBorderType, columnBorderType;
    Vector<int> borderTab;
    int borderElemSize;
    Vector<uchar> ringBuf;
    Vector<uchar> srcRow;
    Vector<uchar> constBorderValue;
    Vector<uchar> constBorderRow;
    int bufStep, startY, startY0, endY, rowCount, dstY;
    Vector<uchar*> rows;
    
    Ptr<BaseFilter> filter2D;
    Ptr<BaseRowFilter> rowFilter;
    Ptr<BaseColumnFilter> columnFilter;
};

enum { KERNEL_GENERAL=0, KERNEL_SYMMETRICAL=1, KERNEL_ASYMMETRICAL=2,
       KERNEL_SMOOTH=4, KERNEL_INTEGER=8 };

CV_EXPORTS int getKernelType(const Mat& kernel, Point anchor);

CV_EXPORTS Ptr<BaseRowFilter> getLinearRowFilter(int srcType, int bufType,
                                            const Mat& kernel, int anchor,
                                            int symmetryType);

CV_EXPORTS Ptr<BaseColumnFilter> getLinearColumnFilter(int bufType, int dstType,
                                            const Mat& kernel, int anchor,
                                            int symmetryType, double delta=0,
                                            int bits=0);

CV_EXPORTS Ptr<BaseFilter> getLinearFilter(int srcType, int dstType,
                                           const Mat& kernel,
                                           Point anchor=Point(-1,-1),
                                           double delta=0, int bits=0);

CV_EXPORTS Ptr<FilterEngine> createSeparableLinearFilter(int srcType, int dstType,
                          const Mat& rowKernel, const Mat& columnKernel,
                          Point _anchor=Point(-1,-1), double delta=0,
                          int _rowBorderType=BORDER_DEFAULT,
                          int _columnBorderType=-1,
                          const Scalar& _borderValue=Scalar());

CV_EXPORTS Ptr<FilterEngine> createLinearFilter(int srcType, int dstType,
                 const Mat& kernel, Point _anchor=Point(-1,-1),
                 double delta=0, int _rowBorderType=BORDER_DEFAULT,
                 int _columnBorderType=-1, const Scalar& _borderValue=Scalar());

CV_EXPORTS Mat getGaussianKernel( int ksize, double sigma, int ktype=CV_64F );

CV_EXPORTS Ptr<FilterEngine> createGaussianFilter( int type, Size ksize,
                                    double sigma1, double sigma2=0,
                                    int borderType=BORDER_DEFAULT);

CV_EXPORTS void getDerivKernels( Mat& kx, Mat& ky, int dx, int dy, int ksize,
                                 bool normalize=false, int ktype=CV_32F );

CV_EXPORTS Ptr<FilterEngine> createDerivFilter( int srcType, int dstType,
                                        int dx, int dy, int ksize,
                                        int borderType=BORDER_DEFAULT );

CV_EXPORTS Ptr<BaseRowFilter> getRowSumFilter(int srcType, int sumType,
                                                 int ksize, int anchor=-1);
CV_EXPORTS Ptr<BaseColumnFilter> getColumnSumFilter(int sumType, int dstType,
                                                       int ksize, int anchor=-1,
                                                       double scale=1);
CV_EXPORTS Ptr<FilterEngine> createBoxFilter( int srcType, int dstType, Size ksize,
                                                 Point anchor=Point(-1,-1),
                                                 bool normalize=true,
                                                 int borderType=BORDER_DEFAULT);

enum { MORPH_ERODE=0, MORPH_DILATE=1, MORPH_OPEN=2, MORPH_CLOSE=3,
       MORPH_GRADIENT=4, MORPH_TOPHAT=5, MORPH_BLACKHAT=6 };

CV_EXPORTS Ptr<BaseRowFilter> getMorphologyRowFilter(int op, int type, int ksize, int anchor=-1);
CV_EXPORTS Ptr<BaseColumnFilter> getMorphologyColumnFilter(int op, int type, int ksize, int anchor=-1);
CV_EXPORTS Ptr<BaseFilter> getMorphologyFilter(int op, int type, const Mat& kernel,
                                               Point anchor=Point(-1,-1));

static inline Scalar morphologyDefaultBorderValue() { return Scalar::all(DBL_MAX); }

CV_EXPORTS Ptr<FilterEngine> createMorphologyFilter(int op, int type, const Mat& kernel,
                    Point anchor=Point(-1,-1), int _rowBorderType=BORDER_CONSTANT,
                    int _columnBorderType=-1,
                    const Scalar& _borderValue=morphologyDefaultBorderValue());

enum { MORPH_RECT=0, MORPH_CROSS=1, MORPH_ELLIPSE=2 };
CV_EXPORTS Mat getStructuringElement(int shape, Size ksize, Point anchor=Point(-1,-1));

CV_EXPORTS void copyMakeBorder( const Mat& src, Mat& dst,
                                int top, int bottom, int left, int right,
                                int borderType, const Scalar& value=Scalar() );

CV_EXPORTS void medianBlur( const Mat& src, Mat& dst, int ksize );
CV_EXPORTS void GaussianBlur( const Mat& src, Mat& dst, Size ksize,
                              double sigma1, double sigma2=0,
                              int borderType=BORDER_DEFAULT );
CV_EXPORTS void bilateralFilter( const Mat& src, Mat& dst, int d,
                                 double sigmaColor, double sigmaSpace,
                                 int borderType=BORDER_DEFAULT );
CV_EXPORTS void boxFilter( const Mat& src, Mat& dst, int ddepth,
                           Size ksize, Point anchor=Point(-1,-1),
                           bool normalize=true,
                           int borderType=BORDER_DEFAULT );
static inline void blur( const Mat& src, Mat& dst,
                         Size ksize, Point anchor=Point(-1,-1),
                         int borderType=BORDER_DEFAULT )
{
    boxFilter( src, dst, -1, ksize, anchor, true, borderType );
}

CV_EXPORTS void filter2D( const Mat& src, Mat& dst, int ddepth,
                          const Mat& kernel, Point anchor=Point(-1,-1),
                          double delta=0, int borderType=BORDER_DEFAULT );

CV_EXPORTS void sepFilter2D( const Mat& src, Mat& dst, int ddepth,
                             const Mat& kernelX, const Mat& kernelY,
                             Point anchor=Point(-1,-1),
                             double delta=0, int borderType=BORDER_DEFAULT );

CV_EXPORTS void Sobel( const Mat& src, Mat& dst, int ddepth,
                       int dx, int dy, int ksize=3,
                       double scale=1, double delta=0,
                       int borderType=BORDER_DEFAULT );

CV_EXPORTS void Scharr( const Mat& src, Mat& dst, int ddepth,
                        int dx, int dy, double scale=1, double delta=0,
                        int borderType=BORDER_DEFAULT );

CV_EXPORTS void Laplacian( const Mat& src, Mat& dst, int ddepth,
                           int ksize=1, double scale=1, double delta=0,
                           int borderType=BORDER_DEFAULT );

CV_EXPORTS void Canny( const Mat& image, Mat& edges,
                       double threshold1, double threshold2,
                       int apertureSize=3, bool L2gradient=false );

CV_EXPORTS void cornerMinEigenVal( const Mat& src, Mat& dst,
                                   int blockSize, int ksize=3,
                                   int borderType=BORDER_DEFAULT );

CV_EXPORTS void cornerHarris( const Mat& src, Mat& dst, int blockSize,
                              int ksize, double k,
                              int borderType=BORDER_DEFAULT );

CV_EXPORTS void cornerEigenValsAndVecs( const Mat& src, Mat& dst,
                                        int blockSize, int ksize,
                                        int borderType=BORDER_DEFAULT );

CV_EXPORTS void preCornerDetect( const Mat& src, Mat& dst, int ksize,
                                 int borderType=BORDER_DEFAULT );

CV_EXPORTS void cornerSubPix( const Mat& image, Vector<Point2f>& corners,
                              Size winSize, Size zeroZone,
                              TermCriteria criteria );

CV_EXPORTS void goodFeaturesToTrack( const Mat& image, Vector<Point2f>& corners,
                                     int maxCorners, double qualityLevel, double minDistance,
                                     const Mat& mask=Mat(), int blockSize=3,
                                     bool useHarrisDetector=false, double k=0.04 );

CV_EXPORTS void HoughLines( Mat& image, Vector<Vec2f>& lines,
                            double rho, double theta, int threshold,
                            double srn=0, double stn=0 );

CV_EXPORTS void HoughLinesP( Mat& image, Vector<Vec4i>& lines,
                             double rho, double theta, int threshold,
                             double minLineLength=0, double maxLineGap=0 );

CV_EXPORTS void HoughCircles( Mat& image, Vector<Vec3f>& circles,
                              int method, double dp, double minDist,
                              double param1=100, double param2=100,
                              int minRadius=0, int maxRadius=0 );

CV_EXPORTS void erode( const Mat& src, Mat& dst, const Mat& kernel,
                       Point anchor=Point(-1,-1), int iterations=1,
                       int borderType=BORDER_CONSTANT,
                       const Scalar& borderValue=morphologyDefaultBorderValue() );
CV_EXPORTS void dilate( const Mat& src, Mat& dst, const Mat& kernel,
                        Point anchor=Point(-1,-1), int iterations=1,
                        int borderType=BORDER_CONSTANT,
                        const Scalar& borderValue=morphologyDefaultBorderValue() );
CV_EXPORTS void morphologyEx( const Mat& src, Mat& dst, int op, const Mat& kernel,
                              Point anchor=Point(-1,-1), int iterations=1,
                              int borderType=BORDER_CONSTANT,
                              const Scalar& borderValue=morphologyDefaultBorderValue() );

enum { INTER_NEAREST=0, INTER_LINEAR=1, INTER_CUBIC=2, INTER_AREA=3,
       INTER_LANCZOS4=4, INTER_MAX=7, WARP_INVERSE_MAP=16 };

CV_EXPORTS void resize( const Mat& src, Mat& dst,
                        Size dsize=Size(), double fx=0, double fy=0,
                        int interpolation=INTER_LINEAR );

CV_EXPORTS void warpAffine( const Mat& src, Mat& dst,
                            const Mat& M, Size dsize,
                            int flags=INTER_LINEAR,
                            int borderMode=BORDER_CONSTANT,
                            const Scalar& borderValue=Scalar());
CV_EXPORTS void warpPerspective( const Mat& src, Mat& dst,
                                 const Mat& M, Size dsize,
                                 int flags=INTER_LINEAR,
                                 int borderMode=BORDER_CONSTANT,
                                 const Scalar& borderValue=Scalar());

CV_EXPORTS void remap( const Mat& src, Mat& dst, const Mat& map1, const Mat& map2,
                       int interpolation, int borderMode=BORDER_CONSTANT,
                       const Scalar& borderValue=Scalar());

CV_EXPORTS void convertMaps( const Mat& map1, const Mat& map2, Mat& dstmap1, Mat& dstmap2,
                             int dstmap1type, bool nninterpolation=false );

CV_EXPORTS Mat getRotationMatrix2D( Point2f center, double angle, double scale );
CV_EXPORTS Mat getPerspectiveTransform( const Point2f src[], const Point2f dst[] );
CV_EXPORTS Mat getAffineTransform( const Point2f src[], const Point2f dst[] );
CV_EXPORTS void invertAffineTransform(const Mat& M, Mat& iM);

CV_EXPORTS void getRectSubPix( const Mat& image, Size patchSize,
                               Point2f center, Mat& patch, int patchType=-1 );

CV_EXPORTS void integral( const Mat& src, Mat& sum, int sdepth=-1 );
CV_EXPORTS void integral( const Mat& src, Mat& sum, Mat& sqsum, int sdepth=-1 );
CV_EXPORTS void integral( const Mat& src, Mat& sum, Mat& sqsum, Mat& tilted, int sdepth=-1 );

CV_EXPORTS void accumulate( const Mat& src, Mat& dst, const Mat& mask=Mat() );
CV_EXPORTS void accumulateSquare( const Mat& src, Mat& dst, const Mat& mask=Mat() );
CV_EXPORTS void accumulateProduct( const Mat& src1, const Mat& src2,
                                   Mat& dst, const Mat& mask=Mat() );
CV_EXPORTS void accumulateWeighted( const Mat& src, Mat& dst,
                                    double alpha, const Mat& mask=Mat() );

enum { THRESH_BINARY=0, THRESH_BINARY_INV=1, THRESH_TRUNC=2, THRESH_TOZERO=3,
       THRESH_TOZERO_INV=4, THRESH_MASK=7, THRESH_OTSU=8 };

CV_EXPORTS double threshold( const Mat& src, Mat& dst, double thresh, double maxval, int type );

enum { ADAPTIVE_THRESH_MEAN_C=0, ADAPTIVE_THRESH_GAUSSIAN_C=1 };

CV_EXPORTS void adaptiveThreshold( const Mat& src, Mat& dst, double maxValue,
                                   int adaptiveMethod, int thresholdType,
                                   int blockSize, double C );

CV_EXPORTS void pyrDown( const Mat& src, Mat& dst, const Size& dstsize=Size());
CV_EXPORTS void pyrUp( const Mat& src, Mat& dst, const Size& dstsize=Size());
CV_EXPORTS void buildPyramid( const Mat& src, Vector<Mat>& dst, int maxlevel );


CV_EXPORTS void undistort( const Mat& src, Mat& dst, const Mat& cameraMatrix,
                           const Mat& distCoeffs, const Mat& newCameraMatrix=Mat() );
CV_EXPORTS void initUndistortRectifyMap( const Mat& cameraMatrix, const Mat& distCoeffs,
                           const Mat& R, const Mat& newCameraMatrix,
                           Size size, int m1type, Mat& map1, Mat& map2 );
CV_EXPORTS Mat getDefaultNewCameraMatrix( const Mat& cameraMatrix, Size imgsize=Size(),
                                          bool centerPrincipalPoint=false );

enum { OPTFLOW_USE_INITIAL_FLOW=4, OPTFLOW_FARNEBACK_GAUSSIAN=256 };

CV_EXPORTS void calcOpticalFlowPyrLK( const Mat& prevImg, const Mat& nextImg,
                           const Vector<Point2f>& prevPts,
                           Vector<Point2f>& nextPts,
                           Vector<bool>& status, Vector<float>& err,
                           Size winSize=Size(15,15), int maxLevel=3,
                           TermCriteria criteria=TermCriteria(
                            TermCriteria::COUNT+TermCriteria::EPS,
                            30, 0.01),
                           double derivLambda=0.5,
                           int flags=0 );

CV_EXPORTS void calcOpticalFlowFarneback( const Mat& prev0, const Mat& next0,
                               Mat& flow0, double pyr_scale, int levels, int winsize,
                               int iterations, int poly_n, double poly_sigma, int flags );
    
    
CV_EXPORTS void calcHist( const Vector<Mat>& images, const Vector<int>& channels,
                          const Mat& mask, MatND& hist, const Vector<int>& histSize,
                          const Vector<Vector<float> >& ranges,
                          bool uniform=true, bool accumulate=false );

CV_EXPORTS void calcHist( const Vector<Mat>& images, const Vector<int>& channels,
                          const Mat& mask, SparseMat& hist, const Vector<int>& histSize,
                          const Vector<Vector<float> >& ranges,
                          bool uniform=true, bool accumulate=false );
    
CV_EXPORTS void calcBackProject( const Vector<Mat>& images, const Vector<int>& channels,
                                 const MatND& hist, Mat& backProject,
                                 const Vector<Vector<float> >& ranges,
                                 double scale=1, bool uniform=true );
    
CV_EXPORTS void calcBackProject( const Vector<Mat>& images, const Vector<int>& channels,
                                 const SparseMat& hist, Mat& backProject,
                                 const Vector<Vector<float> >& ranges,
                                 double scale=1, bool uniform=true );

CV_EXPORTS double compareHist( const MatND& H1, const MatND& H2, int method );

CV_EXPORTS double compareHist( const SparseMat& H1, const SparseMat& H2, int method );

CV_EXPORTS void equalizeHist( const Mat& src, Mat& dst );

CV_EXPORTS void watershed( const Mat& image, Mat& markers );

enum { INPAINT_NS=CV_INPAINT_NS, INPAINT_TELEA=CV_INPAINT_TELEA };

CV_EXPORTS void inpaint( const Mat& src, const Mat& inpaintMask,
                         Mat& dst, double inpaintRange, int flags );

CV_EXPORTS void distanceTransform( const Mat& src, Mat& dst, Mat& labels,
                                   int distanceType, int maskSize );

CV_EXPORTS void distanceTransform( const Mat& src, Mat& dst,
                                   int distanceType, int maskSize );

enum { FLOODFILL_FIXED_RANGE = 1 << 16,
       FLOODFILL_MASK_ONLY = 1 << 17 };

CV_EXPORTS int floodFill( Mat& image,
                          Point seedPoint, Scalar newVal, Rect* rect=0,
                          Scalar loDiff=Scalar(), Scalar upDiff=Scalar(),
                          int flags=4 );

CV_EXPORTS int floodFill( Mat& image, Mat& mask,
                          Point seedPoint, Scalar newVal, Rect* rect=0,
                          Scalar loDiff=Scalar(), Scalar upDiff=Scalar(),
                          int flags=4 );

CV_EXPORTS void cvtColor( const Mat& src, Mat& dst, int code, int dstCn=0 );

class CV_EXPORTS Moments
{
public:
    Moments();
    Moments(double m00, double m10, double m01, double m20, double m11,
            double m02, double m30, double m21, double m12, double m03 );
    Moments( const CvMoments& moments );
    operator CvMoments() const;
    
    double  m00, m10, m01, m20, m11, m02, m30, m21, m12, m03; // spatial moments
    double  mu20, mu11, mu02, mu30, mu21, mu12, mu03; // central moments
    double  nu20, nu11, nu02, nu30, nu21, nu12, nu03; // central normalized moments
};

CV_EXPORTS Moments moments( const Mat& image, bool binaryImage=false );

CV_EXPORTS void HuMoments( const Moments& moments, double hu[7] );

enum { TM_SQDIFF=CV_TM_SQDIFF, TM_SQDIFF_NORMED=CV_TM_SQDIFF_NORMED,
       TM_CCORR=CV_TM_CCORR, TM_CCORR_NORMED=CV_TM_CCORR_NORMED,
       TM_CCOEFF=CV_TM_CCOEFF, TM_CCOEFF_NORMED=CV_TM_CCOEFF_NORMED };

CV_EXPORTS void matchTemplate( const Mat& image, const Mat& templ, Mat& result, int method );

enum { RETR_EXTERNAL=CV_RETR_EXTERNAL, RETR_LIST=CV_RETR_LIST,
       RETR_CCOMP=CV_RETR_CCOMP, RETR_TREE=CV_RETR_TREE };

enum { CHAIN_APPROX_NONE=CV_CHAIN_APPROX_NONE,
       CHAIN_APPROX_SIMPLE=CV_CHAIN_APPROX_SIMPLE,
       CHAIN_APPROX_TC89_L1=CV_CHAIN_APPROX_TC89_L1,
       CHAIN_APPROX_TC89_KCOS=CV_CHAIN_APPROX_TC89_KCOS };

CV_EXPORTS Vector<Vector<Point> >
    findContours( const Mat& image, Vector<Vec4i>& hierarchy,
                  int mode, int method, Point offset=Point());

CV_EXPORTS Vector<Vector<Point> >
    findContours( const Mat& image, int mode, int method, Point offset=Point());

CV_EXPORTS void
    drawContours( Mat& image, const Vector<Vector<Point> >& contours,
                  const Scalar& color, int thickness=1,
                  int lineType=8, const Vector<Vec4i>& hierarchy=Vector<Vec4i>(),
                  int maxLevel=1, Point offset=Point() );

CV_EXPORTS void approxPolyDP( const Vector<Point>& curve,
                              Vector<Point>& approxCurve,
                              double epsilon, bool closed );
CV_EXPORTS void approxPolyDP( const Vector<Point2f>& curve,
                              Vector<Point2f>& approxCurve,
                              double epsilon, bool closed );

CV_EXPORTS double arcLength( const Vector<Point>& curve, bool closed );
CV_EXPORTS double arcLength( const Vector<Point2f>& curve, bool closed );

CV_EXPORTS Rect boundingRect( const Vector<Point>& points );
CV_EXPORTS Rect boundingRect( const Vector<Point2f>& points );

CV_EXPORTS double contourArea( const Vector<Point>& contour );
CV_EXPORTS double contourArea( const Vector<Point2f>& contour );

CV_EXPORTS RotatedRect minAreaRect( const Vector<Point>& points );
CV_EXPORTS RotatedRect minAreaRect( const Vector<Point2f>& points );

CV_EXPORTS void minEnclosingCircle( const Vector<Point>& points,
                                    Point2f center, float& radius );
CV_EXPORTS void minEnclosingCircle( const Vector<Point2f>& points,
                                    Point2f center, float& radius );

CV_EXPORTS Moments moments( const Vector<Point>& points );
CV_EXPORTS Moments moments( const Vector<Point2f>& points );

CV_EXPORTS double matchShapes( const Vector<Point2f>& contour1,
                               const Vector<Point2f>& contour2,
                               int method, double parameter );
CV_EXPORTS double matchShapes( const Vector<Point>& contour1,
                               const Vector<Point>& contour2,
                               int method, double parameter );

CV_EXPORTS void convexHull( const Vector<Point>& points,
                            Vector<int>& hull, bool clockwise=false );
CV_EXPORTS void convexHull( const Vector<Point>& points,
                            Vector<Point>& hull, bool clockwise=false );
CV_EXPORTS void convexHull( const Vector<Point2f>& points,
                            Vector<int>& hull, bool clockwise=false );
CV_EXPORTS void convexHull( const Vector<Point2f>& points,
                            Vector<Point2f>& hull, bool clockwise=false );

CV_EXPORTS bool isContourConvex( const Vector<Point>& contour );
CV_EXPORTS bool isContourConvex( const Vector<Point2f>& contour );

CV_EXPORTS RotatedRect fitEllipse( const Vector<Point>& points );
CV_EXPORTS RotatedRect fitEllipse( const Vector<Point2f>& points );

CV_EXPORTS Vec4f fitLine( const Vector<Point> points, int distType,
                          double param, double reps, double aeps );
CV_EXPORTS Vec4f fitLine( const Vector<Point2f> points, int distType,
                          double param, double reps, double aeps );
CV_EXPORTS Vec6f fitLine( const Vector<Point3f> points, int distType,
                          double param, double reps, double aeps );

CV_EXPORTS double pointPolygonTest( const Vector<Point>& contour,
                                    Point2f pt, bool measureDist );
CV_EXPORTS double pointPolygonTest( const Vector<Point2f>& contour,
                                    Point2f pt, bool measureDist );

CV_EXPORTS Mat estimateRigidTransform( const Vector<Point2f>& A,
                                       const Vector<Point2f>& B,
                                       bool fullAffine );

CV_EXPORTS void updateMotionHistory( const Mat& silhouette, Mat& mhi,
                                     double timestamp, double duration );

CV_EXPORTS void calcMotionGradient( const Mat& mhi, Mat& mask,
                                    Mat& orientation,
                                    double delta1, double delta2,
                                    int apertureSize=3 );

CV_EXPORTS double calcGlobalOrientation( const Mat& orientation, const Mat& mask,
                                         const Mat& mhi, double timestamp,
                                         double duration );
// TODO: need good API for cvSegmentMotion

CV_EXPORTS RotatedRect CAMShift( const Mat& probImage, Rect& window,
                                 TermCriteria criteria );

CV_EXPORTS int meanShift( const Mat& probImage, Rect& window,
                          TermCriteria criteria );

class CV_EXPORTS KalmanFilter
{
public:
    KalmanFilter();
    KalmanFilter(int dynamParams, int measureParams, int controlParams=0);
    void init(int dynamParams, int measureParams, int controlParams=0);

    const Mat& predict(const Mat& control=Mat());
    const Mat& correct(const Mat& measurement);

    Mat statePre;           // predicted state (x'(k)):
                            //    x(k)=A*x(k-1)+B*u(k)
    Mat statePost;          // corrected state (x(k)):
                            //    x(k)=x'(k)+K(k)*(z(k)-H*x'(k))
    Mat transitionMatrix;   // state transition matrix (A)
    Mat controlMatrix;      // control matrix (B)
                            //   (it is not used if there is no control)
    Mat measurementMatrix;  // measurement matrix (H)
    Mat processNoiseCov;    // process noise covariance matrix (Q)
    Mat measurementNoiseCov;// measurement noise covariance matrix (R)
    Mat errorCovPre;        // priori error estimate covariance matrix (P'(k)):
                            //    P'(k)=A*P(k-1)*At + Q)*/
    Mat gain;               // Kalman gain matrix (K(k)):
                            //    K(k)=P'(k)*Ht*inv(H*P'(k)*Ht+R)
    Mat errorCovPost;       // posteriori error estimate covariance matrix (P(k)):
                            //    P(k)=(I-K(k)*H)*P'(k)
    Mat temp1;              // temporary matrices
    Mat temp2;
    Mat temp3;
    Mat temp4;
    Mat temp5;
};


///////////////////////////// Object Detection ////////////////////////////

template<> inline void Ptr<CvHaarClassifierCascade>::delete_obj()
{ cvReleaseHaarClassifierCascade(&obj); }

class CV_EXPORTS HaarClassifierCascade
{
public:
    enum { DO_CANNY_PRUNING = CV_HAAR_DO_CANNY_PRUNING,
           SCALE_IMAGE = CV_HAAR_SCALE_IMAGE,
           FIND_BIGGEST_OBJECT = CV_HAAR_FIND_BIGGEST_OBJECT,
           DO_ROUGH_SEARCH = CV_HAAR_DO_ROUGH_SEARCH };
    
    HaarClassifierCascade();
    HaarClassifierCascade(const String& filename);
    bool load(const String& filename);

    void detectMultiScale( const Mat& image,
                           Vector<Rect>& objects,
                           double scaleFactor=1.1,
                           int minNeighbors=3, int flags=0,
                           Size minSize=Size());

    int runAt(Point pt, int startStage=0, int nstages=0) const;

    void setImages( const Mat& sum, const Mat& sqsum,
                    const Mat& tiltedSum, double scale );
    
    Ptr<CvHaarClassifierCascade> cascade;
};

CV_EXPORTS void undistortPoints( const Vector<Point2f>& src, Vector<Point2f>& dst,
                                 const Mat& cameraMatrix, const Mat& distCoeffs,
                                 const Mat& R=Mat(), const Mat& P=Mat());

CV_EXPORTS Mat Rodrigues(const Mat& src);
CV_EXPORTS Mat Rodrigues(const Mat& src, Mat& jacobian);

enum { LMEDS=4, RANSAC=8 };

CV_EXPORTS Mat findHomography( const Vector<Point2f>& srcPoints,
                               const Vector<Point2f>& dstPoints,
                               Vector<bool>& mask, int method=0,
                               double ransacReprojThreshold=0 );

CV_EXPORTS Mat findHomography( const Vector<Point2f>& srcPoints,
                               const Vector<Point2f>& dstPoints,
                               int method=0, double ransacReprojThreshold=0 );

/* Computes RQ decomposition for 3x3 matrices */
CV_EXPORTS void RQDecomp3x3( const Mat& M, Mat& R, Mat& Q );
CV_EXPORTS Vec3d RQDecomp3x3( const Mat& M, Mat& R, Mat& Q,
                              Mat& Qx, Mat& Qy, Mat& Qz );

CV_EXPORTS void decomposeProjectionMatrix( const Mat& projMatrix, Mat& cameraMatrix,
                                           Mat& rotMatrix, Mat& transVect );
CV_EXPORTS void decomposeProjectionMatrix( const Mat& projMatrix, Mat& cameraMatrix,
                                           Mat& rotMatrix, Mat& transVect,
                                           Mat& rotMatrixX, Mat& rotMatrixY,
                                           Mat& rotMatrixZ, Vec3d& eulerAngles );

CV_EXPORTS void matMulDeriv( const Mat& A, const Mat& B, Mat& dABdA, Mat& dABdB );

CV_EXPORTS void composeRT( const Mat& rvec1, const Mat& tvec1,
                           const Mat& rvec2, const Mat& tvec2,
                           Mat& rvec3, Mat& tvec3 );

CV_EXPORTS void composeRT( const Mat& rvec1, const Mat& tvec1,
                           const Mat& rvec2, const Mat& tvec2,
                           Mat& rvec3, Mat& tvec3,
                           Mat& dr3dr1, Mat& dr3dt1,
                           Mat& dr3dr2, Mat& dr3dt2,
                           Mat& dt3dr1, Mat& dt3dt1,
                           Mat& dt3dr2, Mat& dt3dt2 );

CV_EXPORTS void projectPoints( const Vector<Point3f>& objectPoints,
                               const Mat& rvec, const Mat& tvec,
                               const Mat& cameraMatrix,
                               const Mat& distCoeffs,
                               Vector<Point2f>& imagePoints );

CV_EXPORTS void projectPoints( const Vector<Point3f>& objectPoints,
                               const Mat& rvec, const Mat& tvec,
                               const Mat& cameraMatrix,
                               const Mat& distCoeffs,
                               Vector<Point2f>& imagePoints,
                               Mat& dpdrot, Mat& dpdt, Mat& dpdf,
                               Mat& dpdc, Mat& dpddist,
                               double aspectRatio=0 );

CV_EXPORTS void solvePnP( const Vector<Point3f>& objectPoints,
                          const Vector<Point2f>& imagePoints,
                          const Mat& cameraMatrix,
                          const Mat& distCoeffs,
                          Mat& rvec, Mat& tvec,
                          bool useExtrinsicGuess=false );

CV_EXPORTS Mat initCameraMatrix2D( const Vector<Vector<Point3f> >& objectPoints,
                                   const Vector<Vector<Point2f> >& imagePoints,
                                   Size imageSize, double aspectRatio=1. );

enum { CALIB_CB_ADAPTIVE_THRESH = CV_CALIB_CB_ADAPTIVE_THRESH,
       CALIB_CB_NORMALIZE_IMAGE = CV_CALIB_CB_NORMALIZE_IMAGE,
       CALIB_CB_FILTER_QUADS = CV_CALIB_CB_FILTER_QUADS };

CV_EXPORTS bool findChessboardCorners( const Mat& image, Size patternSize,
                                       Vector<Point2f>& corners,
                                       int flags=CV_CALIB_CB_ADAPTIVE_THRESH+
                                            CV_CALIB_CB_NORMALIZE_IMAGE );

CV_EXPORTS void drawChessboardCorners( Mat& image, Size patternSize,
                                       const Vector<Point2f>& corners,
                                       bool patternWasFound );

enum
{
    CALIB_USE_INTRINSIC_GUESS = CV_CALIB_USE_INTRINSIC_GUESS,
    CALIB_FIX_ASPECT_RATIO = CV_CALIB_FIX_ASPECT_RATIO,
    CALIB_FIX_PRINCIPAL_POINT = CV_CALIB_FIX_PRINCIPAL_POINT,
    CALIB_ZERO_TANGENT_DIST = CV_CALIB_ZERO_TANGENT_DIST,
    CALIB_FIX_FOCAL_LENGTH = CV_CALIB_FIX_FOCAL_LENGTH,
    CALIB_FIX_K1 = CV_CALIB_FIX_K1,
    CALIB_FIX_K2 = CV_CALIB_FIX_K2,
    CALIB_FIX_K3 = CV_CALIB_FIX_K3,
    // only for stereo
    CALIB_FIX_INTRINSIC = CV_CALIB_FIX_INTRINSIC,
    CALIB_SAME_FOCAL_LENGTH = CV_CALIB_SAME_FOCAL_LENGTH,
    // for stereo rectification
    CALIB_ZERO_DISPARITY = CV_CALIB_ZERO_DISPARITY
};

CV_EXPORTS void calibrateCamera( const Vector<Vector<Point3f> >& objectPoints,
                                 const Vector<Vector<Point2f> >& imagePoints,
                                 Size imageSize,
                                 Mat& cameraMatrix, Mat& distCoeffs,
                                 Vector<Mat>& rvecs, Vector<Mat>& tvecs,
                                 int flags=0 );

CV_EXPORTS void calibrationMatrixValues( const Mat& cameraMatrix,
                                Size imageSize,
                                double apertureWidth,
                                double apertureHeight,
                                double& fovx,
                                double& fovy,
                                double& focalLength,
                                Point2d& principalPoint,
                                double& aspectRatio );

CV_EXPORTS void stereoCalibrate( const Vector<Vector<Point3f> >& objectPoints,
                                 const Vector<Vector<Point2f> >& imagePoints1,
                                 const Vector<Vector<Point2f> >& imagePoints2,
                                 Mat& cameraMatrix1, Mat& distCoeffs1,
                                 Mat& cameraMatrix2, Mat& distCoeffs2,
                                 Size imageSize, Mat& R, Mat& T,
                                 Mat& E, Mat& F,
                                 TermCriteria criteria = TermCriteria(TermCriteria::COUNT+
                                    TermCriteria::EPS, 30, 1e-6),
                                 int flags=CALIB_FIX_INTRINSIC );

CV_EXPORTS void stereoRectify( const Mat& cameraMatrix1, const Mat& distCoeffs1,
                               const Mat& cameraMatrix2, const Mat& distCoeffs2,
                               Size imageSize, const Mat& R, const Mat& T,
                               Mat& R1, Mat& R2, Mat& P1, Mat& P2, Mat& Q,
                               int flags=CALIB_ZERO_DISPARITY );

CV_EXPORTS bool stereoRectifyUncalibrated( const Vector<Point2f>& points1,
                                           const Vector<Point2f>& points2,
                                           const Mat& F, Size imgSize,
                                           Mat& H1, Mat& H2,
                                           double threshold=5 );

CV_EXPORTS void convertPointsHomogeneous( const Vector<Point2f>& src,
                                          Vector<Point3f>& dst );
CV_EXPORTS void convertPointsHomogeneous( const Vector<Point3f>& src,
                                          Vector<Point2f>& dst );

enum
{ 
    FM_7POINT = CV_FM_7POINT,
    FM_8POINT = CV_FM_8POINT,
    FM_LMEDS = CV_FM_LMEDS,
    FM_RANSAC = CV_FM_RANSAC
};

CV_EXPORTS Mat findFundamentalMat( const Vector<Point2f>& points1,
                                   const Vector<Point2f>& points2,
                                   Vector<bool>& mask,
                                   int method=FM_RANSAC,
                                   double param1=3., double param2=0.99 );

CV_EXPORTS Mat findFundamentalMat( const Vector<Point2f>& points1,
                                   const Vector<Point2f>& points2,
                                   int method=FM_RANSAC,
                                   double param1=3., double param2=0.99 );

CV_EXPORTS void computeCorrespondEpilines( const Vector<Point2f>& points1,
                                           int whichImage, const Mat& F,
                                           Vector<Vec3f>& lines );

template<> inline void Ptr<CvStereoBMState>::delete_obj()
{ cvReleaseStereoBMState(&obj); }

// Block matching stereo correspondence algorithm
class CV_EXPORTS StereoBM
{
    enum { NORMALIZED_RESPONSE = CV_STEREO_BM_NORMALIZED_RESPONSE,
        BASIC_PRESET=CV_STEREO_BM_BASIC,
        FISH_EYE_PRESET=CV_STEREO_BM_FISH_EYE,
        NARROW_PRESET=CV_STEREO_BM_NARROW };
    
    StereoBM();
    StereoBM(int preset, int ndisparities=0, int SADWindowSize=21);
    void init(int preset, int ndisparities=0, int SADWindowSize=21);
    void operator()( const Mat& left, const Mat& right, Mat& disparity );

    Ptr<CvStereoBMState> state;
};

CV_EXPORTS void reprojectImageTo3D( const Mat& disparity,
                                    Mat& _3dImage, const Mat& Q,
                                    bool handleMissingValues=false );

class CV_EXPORTS KeyPoint
{
public:    
    KeyPoint() : pt(0,0), size(0), angle(-1), response(0), octave(0) {}
    KeyPoint(Point2f _pt, float _size, float _angle=-1, float _response=0, int _octave=0)
    : pt(_pt), size(_size), angle(_angle), response(_response), octave(_octave) {}
    KeyPoint(float x, float y, float _size, float _angle=-1, float _response=0, int _octave=0)
    : pt(x, y), size(_size), angle(_angle), response(_response), octave(_octave) {}
    
    Point2f pt;
    float size;
    float angle;
    float response;
    int octave;
};

CV_EXPORTS void write(FileStorage& fs, const String& name, const Vector<KeyPoint>& keypoints);
CV_EXPORTS void read(const FileNode& node, Vector<KeyPoint>& keypoints);    

class CV_EXPORTS SURF : public CvSURFParams
{
public:
    SURF();
    SURF(double _hessianThreshold, bool _extended=false);

    int descriptorSize() const;
    void operator()(const Mat& img, const Mat& mask,
                    Vector<KeyPoint>& keypoints) const;
    void operator()(const Mat& img, const Mat& mask,
                    Vector<KeyPoint>& keypoints,
                    Vector<float>& descriptors,
                    bool useProvidedKeypoints=false) const;
};


class CV_EXPORTS MSER : public CvMSERParams
{
public:
    MSER();
    MSER( int _delta, int _min_area, int _max_area,
          float _max_variation, float _min_diversity,
          int _max_evolution, double _area_threshold,
          double _min_margin, int _edge_blur_size );
    Vector<Vector<Point> > operator()(Mat& image, const Mat& mask) const;
};


class CV_EXPORTS StarDetector : CvStarDetectorParams
{
public:
    StarDetector();
    StarDetector(int _maxSize, int _responseThreshold,
                 int _lineThresholdProjected,
                 int _lineThresholdBinarized,
                 int _suppressNonmaxSize);

    void operator()(const Mat& image, Vector<KeyPoint>& keypoints) const;
};
    
}

//////////////////////////////////////////////////////////////////////////////////////////

class CV_EXPORTS CvLevMarq
{
public:
    CvLevMarq();
    CvLevMarq( int nparams, int nerrs, CvTermCriteria criteria=
        cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,30,DBL_EPSILON),
        bool completeSymmFlag=false );
    ~CvLevMarq();
    void init( int nparams, int nerrs, CvTermCriteria criteria=
        cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,30,DBL_EPSILON),
        bool completeSymmFlag=false );
    bool update( const CvMat*& param, CvMat*& J, CvMat*& err );
    bool updateAlt( const CvMat*& param, CvMat*& JtJ, CvMat*& JtErr, double*& errNorm );

    void clear();
    void step();
    enum { DONE=0, STARTED=1, CALC_J=2, CHECK_ERR=3 };

    CvMat* mask;
    CvMat* prevParam;
    CvMat* param;
    CvMat* J;
    CvMat* err;
    CvMat* JtJ;
    CvMat* JtJN;
    CvMat* JtErr;
    CvMat* JtJV;
    CvMat* JtJW;
    double prevErrNorm, errNorm;
    int lambdaLg10;
    CvTermCriteria criteria;
    int state;
    int iters;
    bool completeSymmFlag;
};


// 2009-01-12, Xavier Delacour <xavier.delacour@gmail.com>

struct lsh_hash {
  int h1, h2;
};

struct CvLSHOperations
{
  virtual ~CvLSHOperations() {}

  virtual int vector_add(const void* data) = 0;
  virtual void vector_remove(int i) = 0;
  virtual const void* vector_lookup(int i) = 0;
  virtual void vector_reserve(int n) = 0;
  virtual unsigned int vector_count() = 0;

  virtual void hash_insert(lsh_hash h, int l, int i) = 0;
  virtual void hash_remove(lsh_hash h, int l, int i) = 0;
  virtual int hash_lookup(lsh_hash h, int l, int* ret_i, int ret_i_max) = 0;
};


#endif /* __cplusplus */

#endif /* _CV_HPP_ */

/* End of file. */