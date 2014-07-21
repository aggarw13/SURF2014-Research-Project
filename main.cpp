#include "mainwindow.h"
#include "thread.h"
#include <QApplication>
#include <QMainWindow>
#include <QtGui>
#include "FlyCapture2.h"
#include<conio.h>
#include <qthread.h>
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

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
	/*Thread * thread = new Thread(0);
	//w.mapper = new QSignalMapper(thread);
	//&(w.on_coilcontrol_manual_pushed(QString))->moveToThread(thread);
	/*w.mapper->setMapping(&wleftc_pb,QString("L"));
	w.mapper->setMapping(w.rightc_pb,QString("R"));
	w.mapper->setMapping(w.bottomc_pb,QString("B"));
	w.mapper->setMapping(w.topc_pb,QString("T"));
	w.mapper->setMapping(w.upc_pb,QString("U"));
	w.mapper->setMapping(w.downc_pb, QString("D"));
	w.mapper->setMapping(w.termcurrent_pb,QString("C"));
	QObject::connect(w.leftc_pb, SIGNAL(clicked()),w.mapper, SLOT(map()));
	QObject::connect(w.topc_pb,SIGNAL(clikced()),w.mapper,SLOT(map()));
	QObject::connect(w.rightc_pb,SIGNAL(clicked()),w.mapper,SLOT(map()));
	QObject::connect(w.bottomc_pb,SIGNAL(clicked()),w.mapper,SLOT(map()));
	QObject::connect(w.upc_pb,SIGNAL(clicked()),w.mapper,SLOT(map()));
	QObject::connect(w.downc_pb,SIGNAL(clicked()),w.mapper,SLOT(map()));
	QObject::connect(w.termcurrent_pb,SIGNAL(clicked()),w.mapper,SLOT(map()));*/
    w.show();

    return a.exec();
}
