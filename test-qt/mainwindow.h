#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include "FlyCapture2.h"
//#include "ui_mainwindow.h"
//#include "thread.h"
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

namespace Ui {
class MainWindow;
}

using namespace std;
using namespace cv;
using namespace FlyCapture2;
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
	int selectObject, loopcount, first_press, trackObject, fauto_press, value, clicks, backgrnd_clicks, time_duration, mfeature, thresh, max_thresh, thresh_type, contour, pixdiffx, pixdiffy, filename_inc;
	Mat image, object, temp, lbutton_up, waypts_sans, tmplate, gimage, update_tmp, mask;
	IplImage * im, * src_img;
	bool mouseevent_valid, stop_capture, processing_status;
	double factorx, factory;
	Point2f waypoints[4];
	BusManager busMgr;
	PGRGuid guid;
	Image rawImage, convertedimage;
	FrameRate fps; VideoMode vd;
	Size img_stream;
	ImageMetadata metadata;
	Error error;
	FILE * fp;
	Camera cam;
	QImage * Qim;
	QMutex mutex;
	QSignalMapper * mapper;
	QGraphicsScene * scene, * scene_bckgrndsans, * scene_bckgrnd, * scene_template, *scene_mask;
	QVBoxLayout * scroll_layout;
	CvCapture * cap;
	Rect selection, boundrect, PrevRect, backgrndptsrect;
	string point, filename;
	CvFont font;
	double angleprev, angle;
	Moments mm;
	std::vector<std::vector<Point>> contours, backgrnd_contours, orgim_bckgrndcont;
	std::vector<int> backgrnd_contnos;
	//CvSeq *backgrnd_contours;
	CvSeq * backgrnd_approx;
	CvMemStorage * storage;
	std::vector<Vec4i> hierarchy;
	Point origin, pixelcheck, * backgrnd_pts;
	Point2f COM, COMprev;
	BYTE datasend[7], prevsent[7];
	HANDLE serialHandle, m_threadDoneEvent;
	stringstream ss;
	static void mouseHandler(int ,int , int ,int , void*);
	double calculate_angle(Point2f *,RotatedRect);
	void onMouse( int ,int , int ,int , void*);
	void mousePressEvent(QMouseEvent * ev);
	void mouseMoveEvent(QMouseEvent * ev);
	void mouseReleaseEvent(QMouseEvent * ev);
	void keyPressEvent(QKeyEvent * ev);
	void DelayMS(UINT);
	QImage * Mat2QImage(Mat const&, QImage *);
	Mat QImage2Mat(QImage const& );
	IplImage * ImageToIplImage(QPixmap * qPix);
	BYTE * determine_command(Point, Point *, int);
	bool send_serialportcommand(BYTE *);
    explicit MainWindow(QWidget *parent = 0);
	bool dataTransfer(HANDLE *, BYTE *);
	void * thread_control;
	void * wayptspath_thread;
    ~MainWindow();

//signals:
	//void manual_finished();

public slots:
	//typedef void (MainWindow::*Method)(QString);
	void on_Capturepb_pushed();
	void learnTemplate();
	void learn_backgroundfield();
	void on_trackbt_pushed();
	void on_Stopcapturepb_pushed();
	//void on_coilcontrol_manual_pushed(QString);
	void on_coilcontrol_automatic_pushed();
	void on_closeapplication_pushed();
	void on_learnwaypts_pb_pushed();
	void nextwaypoint_pb_pushed();
	//void on_autowayptspath_pb_pushed();
	void thread_started(QString);
	void retrieve_time();
	void get_coilcommand(QString);
	void on_getpixelval_cb_clicked();
	void on_backgrndpts_cb_clicked();
	void filesettings(QString);
	void open_helpwindow();

private:
	Ui::MainWindow *ui; 
};
//Q_DECLARE_METATYPE(MainWindow::Method);

#endif // MAINWINDOW_H 