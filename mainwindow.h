#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
	int selectObject, loopcount, first_press, trackObject, fauto_press, value, clicks, time_duration;
	Mat image, object, temp, lbutton_up, waypts_sans, tmplate;
	IplImage * im;
	bool mouseevent_valid;
	Point waypoints[4];
	BusManager busMgr;
	PGRGuid guid;
	Image rawImage, convertedimage;
	FrameRate fps; VideoMode vd;
	ImageMetadata metadata;
	Error error;
	Camera cam;
	QImage * Qim;
	QSignalMapper * mapper;
	QGraphicsScene * scene;
	QVBoxLayout * scroll_layout;
	CvCapture * cap;
	Rect selection, boundrect, PrevRect;
	CvFont font;
	Point origin;
	CvPoint COM;
	BYTE datasend[7], prevsent[7];
	HANDLE serialHandle, m_threadDoneEvent;
	Ui::MainWindow *ui; 
	static void mouseHandler(int ,int , int ,int , void*);
	double calculate_angle(Point2f *,RotatedRect);
	void onMouse( int ,int , int ,int , void*);
	void mousePressEvent(QMouseEvent * ev);
	void mouseMoveEvent(QMouseEvent * ev);
	void mouseReleaseEvent(QMouseEvent * ev);
	void keyPressEvent(QKeyEvent * ev);
	void DelayMS(UINT);
	void on_Stopcapturepb_pushed();
	QImage * Mat2QImage(Mat const&, QImage *);
	Mat QImage2Mat(QImage const& );
	IplImage * ImageToIplImage(QPixmap * qPix);
	BYTE * determine_command(Point, Point *, int);
	bool send_serialportcommand(BYTE *);
    explicit MainWindow(QWidget *parent = 0);
	bool dataTransfer(HANDLE *, BYTE *);
    ~MainWindow();

//signals:
	//void manual_finished();

public slots:
	//typedef void (MainWindow::*Method)(QString);
	void on_Capturepb_pushed();
	void learnTemplate();
	void on_trackbt_pushed();
	//void on_coilcontrol_manual_pushed(QString);
	void on_coilcontrol_automatic_pushed();
	void on_closeapplication_pushed();
	void on_learnwaypts_pb_pushed();
	void on_autowayptspath_pb_pushed();
	void retrieve_time();
	void get_coilcommand(QString);

};
//Q_DECLARE_METATYPE(MainWindow::Method);

#endif // MAINWINDOW_H 