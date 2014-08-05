
#include <QMainWindow>
#include <QtGui>
#include "help_window.h"
#include<conio.h>
#include<stdio.h>
#include<iostream>
#include <math.h>
#include <time.h>
#include<windows.h>
#include<string.h>
#include<atlstr.h>
#include "SiUSBXp.h"


Help_window::Help_window(QWidget * parent = (QWidget *)0) : QDialog(parent){
	ui->setupUi(this);
}

void Help_window::on_closewn_pushed(){
	QCoreApplication::instance()->exit();
}