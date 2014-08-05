#include <QMainWindow>
#include <QtGui>
#include "FlyCapture2.h"
//#include "thread.h"
#include<conio.h>
#include "track_robot.h"
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

track_robot::track_robot(QObject * parent = (QObject *)0) : QObject(parent){
	key  = 0;
	loopcount = 0;
	//object = cvLoadImage("Template_video3.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	//filename = ui->filename_le->text().toStdString() + ".txt";
	fp = fopen(filename.c_str(),"w+");
	if(fp == NULL)
		return;
	filename = "Position of COM       Orientation(degrees)        Frame No.\n";
	fputs(filename.c_str(),fp);
	object.copyTo(obj_or);
	//namedWindow("Check");
    //Detect the keypoints using SURF Detector
    //int minHessian = ui->featthresh_dd->currentText().toInt();
	GaussianBlur( object, object, Size(3,3), 0, 0, BORDER_DEFAULT );
	detector = new SurfFeatureDetector(minHessian);
	bgsub.nmixtures = 3;
	//bgsub.set("initializationFrames",20);
	//bgsub.set("decisionThreshold",0.7);
	//cv::Ptr<SurfFeatureDetector> detector = cv::FeatureDetector::create("SURF");
    //SurfFeatureDetector detector( minHessian );

	//QString extract_thresh = ui->extractthresh_dd->currentText();
	if(extract_thresh == "Default")
		extractor = new SurfDescriptorExtractor() ;
	else
		extractor = new SurfDescriptorExtractor(extract_thresh.toInt()) ;
	namedWindow("Test");
	//if(ui->stream_filemenu->currentIndex() > 1)
		//cap = cvCaptureFromAVI(ui->stream_filemenu->currentText().toLocal8Bit().data());	
	//int Key = 0;
	//while(Key != 27 && !object.data && im != NULL){
	//	cvShowImage("Test",im);		im = cvQueryFrame(cap);
	//	Key = cvWaitKey(30);}
	//if(im == NULL)
	//	return;
	robot_area = 0.0;
    //Get the corners from the object
    //obj_corners[0] = cvPoint(0,0);
    //obj_corners[1] = cvPoint( object.cols, 0 );
    //obj_corners[2] = cvPoint( object.cols, object.rows );
    //obj_corners[3] = cvPoint( 0, object.rows );
	//cv::Mat mask(object.rows,object.cols,object.depth());
	cam_mot = 0;
	mfeature = 0, thresh_type = 0, thresh = 50;
	IplImage  *frame_small, *object_small = cvCreateImage(cvSize((int)(object.cols / 5),(int)(object.rows / 5)),IPL_DEPTH_8U,object.channels());
    framecount = 0; 
	std::vector<Point> approx;
	RotatedRect rott_rect;Point2f rotrect[4];
	time_t start,end;Mat boundcn;
	string pos,position[] = {"Position is (",",",")"},angle_t[] = {"Angle",":","degrees"},angle_s;
	CvFont font;
	double angleprev, angle;
	CvPoint org;
	start = time(NULL);
	clock_t st,en;
	st = clock();
	/*if(cam.IsConnected()){
		rawImage.ReleaseBuffer();
		cam.StopCapture();
		if(error != PGRERROR_OK)
			error.PrintErrorTrace();
		Sleep(25);
		SetEvent(m_threadDoneEvent);
		WaitForSingleObject(m_threadDoneEvent,500);
		cam.Disconnect();
		cam.Connect(&guid);}*/
//	cam.Connect(&guid);
	//cam.StartCapture();
}