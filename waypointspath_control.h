#ifndef WAYPOINTSPATH_CONTROL_H
#define WAYPOINTSPATH_CONTROL_H

#include <QMainWindow>
#include <QtGui>
#include "FlyCapture2.h"
#include "mainwindow.h"
#include <qthread.h>
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

class waypointspath_control : public QThread{
	Q_OBJECT

public :
	waypointspath_control(QObject *, MainWindow *);
	int tolerance, channel, prevcount, commcount, coilchange_track, wayptno, jiggle_track, cycles_no;
	double conv_factor[2];
	double prevdist, prevangle, xdist, ydist;
	bool juggle, ycontrol, xcontrol;
	char motiondir[2];
	Point COMprev;
	BYTE data_command[7], start_command, command, command_x, command_y;
	MainWindow * w;
	DWORD toBeWritten;
	DWORD * written;
	LPDWORD event;
	void determine_command(int);
	double calc_distance(Point);
	void TimeDelay(UINT);

public slots:
	void waypoints_autopath();

signals:
	void autopath_finished();
};

#endif //WAYPOINTSPATH_CONTROL