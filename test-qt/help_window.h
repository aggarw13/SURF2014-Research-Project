#ifndef HELP_WINDOW_H
#define HELP_WINDOW_H

#include <QMainWindow>
#include <QtGui>
#include "ui_help_window.h"
#include<conio.h>
#include <qdialog.h>
#include<stdio.h>
#include <math.h>
#include <time.h>
#include<windows.h>
#include<string.h>
#include<atlstr.h>

using namespace std;
namespace Ui_help{
	class Help_window;}

class Help_window : public QDialog
{
	Q_OBJECT
	public:
	Help_window(QWidget *);

	private slots:
	void on_closewn_pushed();

	private:
		Ui_help::Help_window * ui;
};

#endif //HELP_WINDOW_H