#include "thread_manualcontrol.h"
#include "mainwindow.h"
#include "waypointspath_control.h"
#include <QMainWindow>
#include <QtGui>
#include "FlyCapture2.h"
#include<conio.h>
#include<stdio.h>
#include<iostream>
#include <qthread.h>
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

Thread_manualcontrol::Thread_manualcontrol(QObject * parent = (QObject *)0, MainWindow * mw = NULL): QThread(parent){
	qDebug() << "Running";
	w = mw; //value = 0xaa; time_duration = 4000;
	//QObject::connect(w->mapper,SIGNAL(mapped(QString)), this, SLOT(start()), Qt::QueuedConnection);
	QObject::connect(this,SIGNAL(started()),this,SLOT(on_coilcontrol()), Qt::QueuedConnection);
	QObject::connect(this,SIGNAL(manual_finished()),this,SLOT(quit()));
}

void Thread_manualcontrol::TimeDelay(UINT delay)
{
	DWORD start, end;
	start = end = GetTickCount();

	// Wait for 'delay' milliseconds.
	while ((end - start) < delay)
	{
		end = GetTickCount();
	}
}

void Thread_manualcontrol::on_coilcontrol(){
	//QThread::exec();
	qDebug() << "Thread : "<<this->thread()->currentThreadId();
	qDebug() <<"Command Value :" <<w->value <<" Coil : " << coil;
	//time_duration = 2000;
	//if(!w->stop_capture)
		//w->on_Capturepb_pushed();
	//emit manual_finished();
	//return;
	//MainWindow * w;
	for(int i = 0; i < 7; i++)
	{w->datasend[i] = 0; w->prevsent[i] = 0x00;}
	bool success = false, check;
	//serialHandle = NULL;
	CString comport = "\\\\.\\COM4";
	//ui->camwn_label->setText(coil);
	//if(first_press)
		//CloseHandle(serialHandle);
	w->serialHandle = CreateFile(comport, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//if(serialHandle == INVALID_HANDLE_VALUE)
		//ui->camwn_label->setText("Error: Invalid Handle Value for Device 1."); 
	DCB serialParams = { 0 };
	COMMTIMEOUTS timeout = { 0 };
	serialParams.DCBlength = sizeof(serialParams);

	GetCommState(w->serialHandle, &serialParams);
	serialParams.BaudRate = 9600;
	serialParams.ByteSize = 8;
	serialParams.StopBits = ONESTOPBIT;
	serialParams.Parity = PARITY_NONE;
	GetCommState(w->serialHandle, &serialParams);

	timeout.ReadIntervalTimeout = 50;
	timeout.ReadTotalTimeoutConstant = 50;
	timeout.ReadTotalTimeoutMultiplier = 50;
	timeout.WriteTotalTimeoutConstant = 50;
	timeout.WriteTotalTimeoutMultiplier = 10;
	SetCommTimeouts(w->serialHandle, &timeout);
	DWORD toBeWritten = 7, toBeRead = 7;
	DWORD* written = (DWORD*)malloc(sizeof(DWORD));
	DWORD* read = (DWORD*)malloc(sizeof(DWORD));
	LPDWORD event = (DWORD *)malloc(sizeof(DWORD));int ev;
	SetCommMask(w->serialHandle, EV_TXEMPTY);
	//char coil_ch = coil.toStdString().c_str()[0];
	switch(coil){
	
	case 'U':
		w->datasend[3] = w->value;	
		//qDebug() << " Command Value : " << w->value;
		//w->camwn_label->setText(QString::number(datasend[0]));
		//SetCommMask(serialHandle, EV_TXEMPTY);
		//if(WaitCommEvent(serialHandle, event , NULL)){
		//check = send_serialportcommand(datasend);
		//if(WaitCommEvent(w->serialHandle,event,NULL))
		check = WriteFile(w->serialHandle, (LPVOID)w->datasend, toBeWritten, written, NULL);
		//GetCommMask(serialHandle,event);}
		//send_serialportcommand(datasend);
		TimeDelay(time_duration);
		//if(check)
		//check = send_serialportcommand(prevsent);
		//datasend[0] = 0x00;
		//if(WaitCommEvent(w->serialHandle,event,NULL))
		check = WriteFile(w->serialHandle, (LPVOID)w->prevsent, toBeWritten, written, NULL);
	//status = SI_Write(*hDev1, (LPVOID)dataSent, toBeWritten, written);
		if(!check)
		{
			//w->ui->camwn_label->setText("Surprise Removal on Device 1 Write!");
		}
		break;

	case 'D':
		w->datasend[2] = w->value;
		//send_serialportcommand(datasend);
		check = WriteFile(w->serialHandle, (LPVOID)w->datasend, toBeWritten, written, NULL);
		TimeDelay(time_duration);
		w->datasend[2] = 0x00;
		//send_serialportcommand(prevsent);
		if(WaitCommEvent(w->serialHandle,event,NULL))
		check = WriteFile(w->serialHandle, (LPVOID)w->datasend, toBeWritten, written, NULL);
	//status = SI_Write(*hDev1, (LPVOID)dataSent, toBeWritten, written);
		if(!check)
		{
			printf("Surprise Removal on Device 1 Write!");
		}break;

	case 'L':
		w->datasend[5] = int(w->value);
		check = WriteFile(w->serialHandle, (LPVOID)w->datasend, toBeWritten, written, NULL);
		qDebug() << " Check status : "<< check;
		TimeDelay(time_duration);
		check = WriteFile(w->serialHandle, (LPVOID)w->prevsent, toBeWritten, written, NULL);
		qDebug() << " Check status : "<< check;
	//status = SI_Write(*hDev1, (LPVOID)dataSent, toBeWritten, written);
		if(!check)
		{
			printf("Surprise Removal on Device 1 Write!");
		}break;
	
	case 'R':
		w->datasend[6] = w->value;
		check = WriteFile(w->serialHandle, (LPVOID)w->datasend, toBeWritten, written, NULL);
		TimeDelay(time_duration);
		check = WriteFile(w->serialHandle, (LPVOID)w->prevsent, toBeWritten, written, NULL);
	//status = SI_Write(*hDev1, (LPVOID)dataSent, toBeWritten, written);
		if(!check)
		{
			printf("Surprise Removal on Device 1 Write!");
		}
	break;

	case 'C':
		//w->camwn_label->setText("Checking terminate");
		w->datasend[0] = 0x00;
		check = WriteFile(w->serialHandle,(LPVOID)w->datasend,toBeWritten,written,NULL);
		break;
	}
	//first_press++;
	//if(first_press)
	TimeDelay(100);
	CloseHandle(w->serialHandle);
	emit manual_finished();
	//WriteFile(serialHandle,(LPVOID){0x00,0x00,0x00,0x00,0x00,0x00,0x00},toBeWritten,written, NULL);
}
