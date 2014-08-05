#ifndef THREAD_H
#define THREAD_H

#include <QMainWindow>
#include <QtGui>
#include "mainwindow.h"
#include <qthread.h>
#include "FlyCapture2.h"
#include<conio.h>
#include<stdio.h>
#include<iostream>
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

class Thread_manualcontrol : public QThread{
	Q_OBJECT
public :
	Thread_manualcontrol(QObject * parent, MainWindow *);
	MainWindow * w;
	int value, time_duration; char coil;
 signals:
	void manual_finished();

public :
	void TimeDelay(UINT);
public slots:
	//void on_coilcontrol_manual_pushed(QString, MainWindow *);
	void on_coilcontrol();
};

#endif // THREAD_H
	

	 
