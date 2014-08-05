#ifndef TRACK_ROBOT_H
#define TRACK_ROBOT_H

#include <QMainWindow>
#include <QtGui>
#include "FlyCapture2.h"
#include<conio.h>
#include<stdio.h>
#include<iostream>
#include <qlayout.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/flann/flann.hpp>
#include <opencv2/flann/flann_base.hpp>
#include <opencv2/calib3d/calib3d.hpp> // for homography
#include <opencv2/nonfree/nonfree.hpp>
#include <math.h>
#include <time.h>
#include<windows.h>
#include<string.h>
#include<atlstr.h>
#include "SiUSBXp.h"

using namespace cv;

class track_robot : public QThread{
	Q_OBJECT

public:
	track_robot(QObject *);
	Mat object,temp,frame,prevframe, fgmask, segm, roi, buff, fmask, obj_or, des_object, des_img;
	IplImage * frameprev, * frame1, * im;
	Rect boundrect, PrevRect;
	CvFont font;
	Point origin;
	int loopcount, key, minHessian, contour, contor, cam_mot, mfeature, thresh_type, thresh, framecount, corner_count, or_size;
	double robot_area, maxC_area, maxCarea;
	BackgroundSubtractorMOG2 bgsub;
	FeatureDetector * detector;
	DescriptorExtractor * extractor;
	FlannBasedMatcher matcher;
	string filename;
	FILE * fp;
	std::vector<KeyPoint> kp_object;
	std::vector<KeyPoint> kp_img;
	QString extract_thresh;
	Point2f COM, COMprev;
	Moments mt;
	std::vector<KeyPoint> kp_image;
    std::vector<vector<DMatch > > matches;
    std::vector<DMatch > good_matches;
    std::vector<Point2f> obj;
	std::vector<Point2f> scene;
   // std::vector<Point2f> scene_corners(4);
    //Calculate descriptors (feature vectors)
    Mat des_image, img_matches;
	Mat H, background , mask;
    //std::vector<Point2f> obj_corners(4);
	cv::Rect rectLine;
	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierarchy;

public slots:
	void trackrobot();

signals:
	void terminate_tracking();
};

#endif //TRACK_ROBOT
