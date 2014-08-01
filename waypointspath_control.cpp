#include <QMainWindow>
#include <QtGui>
#include "FlyCapture2.h"
//#include "mainwindow.h"
#include <qthread.h>
#include "waypointspath_control.h"
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

waypointspath_control::waypointspath_control(QObject * parent = (QObject *)0, MainWindow * mw = NULL): QThread(parent){
	tolerance = 10;
	w = mw;
	prevdist = prevangle = 0.0;
	juggle = ycontrol = false; xcontrol = true;
	toBeWritten = 7; commcount = prevcount = coilchange_track = jiggle_track = 0;
	written = (DWORD*)malloc(sizeof(DWORD));
	event = (DWORD *)malloc(sizeof(DWORD));
	for(int i = 0; i < 7; i++)
		data_command[i] = 0x00;
	//start_command = 0x00;
	QObject::connect(this,SIGNAL(started()), this, SLOT(waypoints_autopath()));
	QObject::connect(this,SIGNAL(autopath_finished()), this , SLOT(quit()));

}

void waypointspath_control::TimeDelay(UINT delay)
{
	DWORD start, end;
	start = end = GetTickCount();

	// Wait for 'delay' milliseconds.
	while ((end - start) < delay)
	{
		end = GetTickCount();
	}
}

double waypointspath_control::calc_distance(Point waypt){
	return sqrt(pow(std::abs(w->COM.x - waypt.x * conv_factor[0]),2) + pow(std::abs(w->COM.y - waypt.y * conv_factor[1]),2));}



void waypointspath_control::waypoints_autopath(){
	qDebug() << "Waypoints control thread running";
	qDebug() << " Command entered : " << command;
	command = start_command;
	command_x = command_y = start_command;
	TimeDelay(1000);
	if(w->clicks)
		{
		CString comport = "\\\\.\\COM4";
		w->serialHandle = CreateFile(comport, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if(w->serialHandle == INVALID_HANDLE_VALUE)
			emit autopath_finished();
		DCB serialParams = { 0 };
		serialParams.DCBlength = sizeof(serialParams);
		GetCommState(w->serialHandle, &serialParams);
		serialParams.BaudRate = 9600;
		serialParams.ByteSize = 8;
		serialParams.StopBits = ONESTOPBIT;
		serialParams.Parity = PARITY_NONE;
		GetCommState(w->serialHandle, &serialParams);

		// Set timeouts
		COMMTIMEOUTS timeout = { 0 };
		timeout.ReadIntervalTimeout = 50;
		timeout.ReadTotalTimeoutConstant = 50;
		timeout.ReadTotalTimeoutMultiplier = 50;
		timeout.WriteTotalTimeoutConstant = 50;
		timeout.WriteTotalTimeoutMultiplier = 10;
		SetCommTimeouts(w->serialHandle, &timeout);
		for(int l = 1; l <= cycles_no; l++){
		for(wayptno = 0; wayptno < w->clicks; wayptno++){
			prevdist = calc_distance(w->waypoints[wayptno]);
			xdist = std::abs(w->COM.x - w->waypoints[wayptno].x);
			ydist = std::abs(w->COM.y - w->waypoints[wayptno].y);
			prevangle = w->angle;
			COMprev = w->COM;
			while(wayptno < w->clicks && sqrt(pow(std::abs(w->COM.x - w->waypoints[wayptno].x),2) + pow(std::abs(w->COM.y - w->waypoints[wayptno].y),2)) > tolerance)
				{
					//data_command = determine_command(COM,waypoints,l);
					commcount++;
					determine_command(wayptno);
					//WriteFile(w->serialHandle,(LPCVOID)data_command, toBeWritten, written,NULL);
					qDebug() << "Check state waypoint control : "<< WriteFile(w->serialHandle,(LPVOID)data_command, toBeWritten, written,NULL);
					TimeDelay(1000);
					if(!WriteFile(w->serialHandle,(LPCVOID)w->prevsent,toBeWritten, written, NULL));
						emit autopath_finished();
					TimeDelay(500);
					//for(int i = 0; i < 7; i++)
						//data_command[i] = 0x00;
				}		
		}
		}
	}
	CloseHandle(w->serialHandle);
	emit autopath_finished();
}

void waypointspath_control::determine_command(int waypt_no){
	// w->prevsent[i] = 0x00;}
	data_command[2] = data_command[3] = data_command[5] = data_command[6] = 0x00;
	qDebug()<<" COM : ("<< w->COM.x <<"," << w->COM.y <<")"<< " Waypoint "<<waypt_no <<" : ("<< w->waypoints[waypt_no].x * conv_factor[0] <<"," << w->waypoints[waypt_no].y * conv_factor[1]<<")";
	qDebug() << " Conversion factor x : " << conv_factor[0] << " Conversion fact y : "<< conv_factor[1];
	if(commcount - jiggle_track == 9)
	{juggle = true; xcontrol = true; ycontrol = false;}
	if(!juggle){
		if(prevdist - calc_distance(w->waypoints[waypt_no]) < 4 && (commcount - prevcount) >= 3 && std::abs(prevangle - w->angle) < 5){
			//prevcount = commcount;
			if(command_x != 0xee && command_y != 0xee)
				{command_x += 17;
			command_y += 0x11;}
			else goto changecoil;
		}
		if(commcount - coilchange_track == 5){
			changecoil:
			if(xcontrol)
			{xcontrol = false; ycontrol = true; }
			else if(ycontrol)
			{xcontrol = true; ycontrol = false;}
			coilchange_track = commcount;}
		else if(prevdist - calc_distance(w->waypoints[waypt_no]) > 5 || std::abs(prevangle - w->angle) > 5){
			prevdist = calc_distance(w->waypoints[waypt_no]);
			command = start_command;
			xdist = std::abs(w->COM.x - w->waypoints[waypt_no].x * conv_factor[0]);
			ydist = std::abs(w->COM.y - w->waypoints[waypt_no].y * conv_factor[1]);
			prevangle = w->angle;
			COMprev = w->COM; prevcount = jiggle_track = commcount;}
		if(std::abs(w->COM.x - w->waypoints[waypt_no].x * conv_factor[0]) < 10){
			if(command_x <= start_command && command_x != 0x00)
				command_x -= 0x22;
			else
				command_x = start_command;}
		if(std::abs(w->COM.y - w->waypoints[waypt_no].y * conv_factor[1]) < 10){
			if(command_y <= start_command && command_y != 0x00)
				command_y -= 0x22;
			else
				command_y = start_command;}
		if(std::abs(w->COMprev.x - w->COM.x) > 10)
			command_x = 0x44;
		if(std::abs(w->COMprev.y - w->COM.y) > 10)
			command_y = 0x44;
		if(w->COM.x - w->waypoints[waypt_no].x * conv_factor[0] > 5){
			if(!ycontrol && xcontrol)
			data_command[5] = command_x;
			motiondir[0] = 'L'; }
		else if(w->waypoints[waypt_no].x * conv_factor[0] - w->COM.x > 5){
			if(!ycontrol && xcontrol)
			data_command[6] = command_x;
			motiondir[0] = 'R'; }
		//data_command[0]=data_command[1]=data_command[4] = 0x00;
		if(w->COM.y - w->waypoints[waypt_no].y * conv_factor[1] > 5){
			if(ycontrol && !xcontrol)
			data_command[3] = command_y;
			motiondir[1] = 'U'; }
		else if(w->waypoints[waypt_no].y * conv_factor[1] - w->COM.y > 5){
			if(ycontrol && !xcontrol)
			data_command[2] = command_y;
			motiondir[1] = 'D'; }}
	if(juggle){
		if(std::abs(xdist - std::abs(w->COM.x - w->waypoints[waypt_no].x)) < 5 && xcontrol){
		if(motiondir[1] == 'U')
			data_command[2] = start_command;
		if(motiondir[1] == 'D')
			data_command[3] = start_command;
		if(commcount - jiggle_track == 11){
			ycontrol = true; xcontrol = false;}
		}
		if(std::abs(ydist - std::abs(w->COM.y - w->waypoints[waypt_no].y)) < 5 && ycontrol){
		if(motiondir[0] == 'L')
			data_command[6] = start_command;
		if(motiondir[0] == 'R')
			data_command[5] = start_command;}
		if(commcount - jiggle_track == 13)
			juggle = false;
	}
			ydist = std::abs(w->COM.y - w->waypoints[waypt_no].y);
	qDebug() <<" Direction of command, x : " << xcontrol << " y : "<< ycontrol << " Command " << command;
	qDebug() << " Direction of commands : "<< motiondir[0] << motiondir[1] << " Distance : "<<calc_distance(w->waypoints[waypt_no]);
	qDebug() << " Commcount - coiltrack : " <<commcount - coilchange_track;
	data_command[0]=data_command[1]=data_command[4] = 0x00;
	return ;
}