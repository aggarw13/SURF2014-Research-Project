#include "mainwindow.h"
#include "thread_manualcontrol.h"
#include "waypointspath_control.h"
#include <ui_mainwindow.h>
#include <qgraphicsview>
#include <qsignalmapper.h>
#include <qgraphicsscene.h>
#include <qevent.h>
#include<qobject.h>
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
#include<fstream>
#include "SiUSBXp.h"
using namespace cv;
using namespace FlyCapture2;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	selectObject = 0;
	first_press = 0, loopcount = 0, fauto_press = 0,clicks = 0, backgrnd_clicks = 0;
	mouseevent_valid = false;
	scene  = new QGraphicsScene(this);
	scene->setSceneRect(0,0,ui->Video_wn->width(),ui->Video_wn->height());
	ui->Video_wn->setScene(scene);
	backgrnd_pts = (Point *)malloc(sizeof(Point) * ui->backgrndclicks_label->text().toInt());
	Qim = NULL;
	factorx = 3; factory = 2.75;
	m_threadDoneEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	//ui->scrollArea->addScrollBarWidget(ui->scrollArea,Qt::Alignment
	//ui->Video_wn->setMouseTracking(true);
	QObject::connect(ui->capture_pb,SIGNAL(clicked()),this,SLOT(on_Capturepb_pushed()));
	QObject::connect(ui->stopcapture_pb,SIGNAL(clicked()),this,SLOT(on_Stopcapturepb_pushed()));
	QStringList streamlist = QStringList() <<QString("Camera") << QString("MinOil.wmv") << QString("Video_demo.wmv") << QString("Tumbling_video.wmv");
	//QObject::connect(this,SIGNAL(manual_finished()), this, SLOT(on_Stopcapturepb_pushed()));
	QObject::connect(ui->track_cb,SIGNAL(clicked()),this,SLOT(on_trackbt_pushed()));
	QObject::connect(ui->template_pb,SIGNAL(clicked()),this,SLOT(learnTemplate()));
	QObject::connect(ui->autogo_pb,SIGNAL(clicked()),this,SLOT(on_coilcontrol_automatic_pushed()));
	QObject::connect(ui->closewn_pb,SIGNAL(clicked()),this,SLOT(on_closeapplication_pushed()));
	QObject::connect(ui->setwaypts_cb,SIGNAL(clicked()),this,SLOT(on_learnwaypts_pb_pushed()));
	QObject::connect(ui->pixelval_cb,SIGNAL(clicked()),this,SLOT(on_getpixelval_cb_clicked()));
	QObject::connect(ui->learnbackgnd_pb,SIGNAL(clicked()), this, SLOT(learn_backgroundfield()));
	QObject::connect(ui->nextwaypt_pb,SIGNAL(clicked()),this,SLOT(nextwaypoint_pb_pushed()));
	//QObject::connect(ui->autowaypts_pb,SIGNAL(clicked()),this,SLOT(on_autowayptspath_pb_pushed()));
	//QThread * thread = new QThread();
	Thread_manualcontrol *thread = new Thread_manualcontrol(0, this);
	thread_control = (void *)thread;
	waypointspath_control * waypoints_thread = new waypointspath_control(0, this);
	wayptspath_thread = (void *)waypoints_thread;
	//thread->moveToThread(thread);
	mapper = new QSignalMapper();
	QSignalMapper * threadmap = new QSignalMapper();
	//mapper->moveToThread(thread);
	//ui->leftc_pb->moveToThread(thread);
	//QObject::connect(ui->leftc_pb,SIGNAL(clicked()), thread, SLOT(start()));
	mapper->setMapping(ui->leftc_pb,QString("L"));
	mapper->setMapping(ui->rightc_pb,QString("R"));
	mapper->setMapping(ui->bottomc_pb,QString("B"));
	mapper->setMapping(ui->topc_pb,QString("T"));
	mapper->setMapping(ui->upc_pb,QString("U"));
	mapper->setMapping(ui->downc_pb, QString("D"));
	mapper->setMapping(ui->termcurrent_pb,QString("C"));
	mapper->setMapping(ui->autowaypts_pb,QString("W"));
	QObject::connect(ui->leftc_pb, SIGNAL(clicked()),mapper, SLOT(map()));
	QObject::connect(ui->topc_pb,SIGNAL(clicked()),mapper,SLOT(map()));
	QObject::connect(ui->rightc_pb,SIGNAL(clicked()),mapper,SLOT(map()));
	QObject::connect(ui->bottomc_pb,SIGNAL(clicked()),mapper,SLOT(map()));
	QObject::connect(ui->upc_pb,SIGNAL(clicked()),mapper,SLOT(map()));
	QObject::connect(ui->downc_pb,SIGNAL(clicked()),mapper,SLOT(map()));
	QObject::connect(ui->autowaypts_pb,SIGNAL(clicked()),mapper,SLOT(map()));
	//QObject::connect(ui->autowaypts_pb,SIGNAL(clicked()), this, SLOT(on_trackbt_pushed()));
	/*QObject::connect(ui->leftc_pb, SIGNAL(clicked()),thread,SLOT(start()));
	QObject::connect(ui->topc_pb,SIGNAL(clikced()),thread,SLOT(start()));
	QObject::connect(ui->rightc_pb,SIGNAL(clicked()),thread,SLOT(start()));
	QObject::connect(ui->bottomc_pb,SIGNAL(clicked()),thread,SLOT(start()));
	QObject::connect(ui->upc_pb,SIGNAL(clicked()),thread,SLOT(start()));
	QObject::connect(ui->downc_pb,SIGNAL(clicked()),thread,SLOT(start()));
	QObject::connect(ui->termcurrent_pb,SIGNAL(clicked()),mapper,SLOT(map()));*/
	//QObject::connect(mapper,SIGNAL(mapped(QString)),this,SLOT(retrieve_time()));
	QObject::connect(mapper,SIGNAL(mapped(QString)),this,SLOT(get_coilcommand(QString)));
	//QObject::connect(mapper,SIGNAL(mapped(QString)), thread, SLOT(start()), Qt::QueuedConnection);
	QObject::connect(mapper,SIGNAL(mapped(QString)),this,SLOT(thread_started(QString)));
	//QObject::connect(mapper,SIGNAL(mapped(QString)),thread,SLOT(on_coilcontrol_manual_pushed(QString)), Qt::QueuedConnection);
	//QObject::connect(mapper,SIGNAL(mapped(QString)),thread,SLOT(on_coilcontrol()), Qt::QueuedConnection);
	//QObject::connect(thread,SIGNAL(manual_finished()),ui->camwn_label, SLOT(setText("Threading check")), Qt::QueuedConnection);
	//QObject::connect(mapper,SIGNAL(mapped(QString)),this,SLOT(on_coilcontrol_manual_pushed(QString)));
	//QObject::connect(thread,SIGNAL(manual_finished()),thread,SLOT(quit()));
	//thread->start();
	//ui->camwn_label->setText("Thread Status : " + QString::number(thread->isRunning()));
	//thread->quit();
	thread->wait();
	//QObject::connect(ui->rightc_pb,SIGNAL(clicked()),this,SLOT(on_coilcontrol_manual_pushed('R')));
	//QObject::connect(ui->upc_pb,SIGNAL(clicked()),this,SLOT(on_coilcontrol_manual_pushed('U')));
	//QObject::connect(ui->downc_pb,SIGNAL(clicked()),this,SLOT(on_coilcontrol_manual_pushed('P')));
	//QObject::connect(ui->termcurrent_pb,SIGNAL(clicked()),this,SLOT(on_coilcontrol_manual_pushed('T')));
	ui->stream_filemenu->addItems(streamlist);
	streamlist.clear();
	streamlist = QStringList() << QString("640x480Y8") <<QString("640x480Y16") << QString("640x480RGB") << QString("1280x960Y8") << QString("1280x960Y16") << QString("1280x960RGB");
	ui->Videomode_dd->addItems(streamlist);
	streamlist.clear();streamlist = QStringList() << QString("7.50 Hz") << QString("15.00 Hz") << QString("30.00 Hz");
	ui->stream_fpdd->addItems(streamlist); streamlist.clear();
	streamlist = QStringList() << QString("100") << QString("200") << QString("300") << QString("400") << QString("500") ;
	ui->featthresh_dd->addItems(streamlist); streamlist.clear();
	streamlist = QStringList() <<QString("100") << QString("400") << QString("500") << QString("600") << QString("700") << QString("Default");
	ui->extractthresh_dd->addItems(streamlist);
	streamlist.clear();
	streamlist = QStringList() << QString("0") << QString("1");
	ui->bckgrdthreshtype_dd->addItems(streamlist);streamlist.clear();
	streamlist = QStringList() << "Enclosing" << "Non-Enclosing";
	ui->backgrndtype_dd->addItems(streamlist);
	/*streamlist = QStringList() << QString("30") << QString("40") << QString("90") << QString("100") << QString("125") << QString("160") << QString("200");
	ui->threshlimit_le->addItems(streamlist);
	streamlist.clear();
	streamlist = QStringList() << QString("50") << QString("90") << QString("125") << QString("150") << QString("200") <<QString("255");
	ui->threshmax_le->addItems(streamlist);
	streamlist.clear();*/
	streamlist = QStringList() << QString("0") << QString("1");
	ui->threshtype_dd->addItems(streamlist);
	cvSetMouseCallback("Test", mouseHandler , this );
	for(int i = 0; i < 7; i++)
	{datasend[i] = 0x00; prevsent[i] = 0x00;}
	CString comport = "\\\\.\\COM4";
	//serialHandle = CreateFile(comport, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if(serialHandle == INVALID_HANDLE_VALUE)
		ui->camwn_label->setText("Error: Invalid Handle Value for Device 1."); 
// Do some basic settings
	//serialParams = {0};
	//on_capture_pb_pushed();
}

void MainWindow::thread_started(QString coil){
	if(coil != "W"){
		Thread_manualcontrol * mthread = (Thread_manualcontrol *)thread_control;
		mthread->moveToThread(mthread);
		mthread->time_duration = ui->td_le->text().toInt();
		mthread->coil = coil.toStdString().c_str()[0];
		mthread->start();
	}
	else{
		waypointspath_control * wthread = (waypointspath_control *)wayptspath_thread;
		wthread->moveToThread(wthread);
		ss <<  ui->startcommand_le->text().toStdString();
		ss >> std::hex >> value;
		wthread->start_command = value;
		wthread->conv_factor[0] = double(img_stream.width) / image.cols;
		wthread->conv_factor[1] = double(img_stream.height) / image.rows;
		if(ui->waypts_cycles->text().toInt() != 0)
			wthread->cycles_no = ui->waypts_cycles->text().toInt();
		else
			wthread->cycles_no = 1;
		qDebug() << "Conversion factor : " << wthread->conv_factor[0] << " Window width : " << image.cols;
		wthread->start();
		ui->track_cb->setChecked(true);
		on_trackbt_pushed();
		//emit ui->track_cb->clicked(true);
		//ui->track_cb->clicked(true);
	}
	qDebug("GUI thread running status : %d",this->thread()->isRunning());
}


void MainWindow::on_Capturepb_pushed(){
	//if(action.toStdString().c_str()[0] == 'E')
		//return;
   stop_capture = false;
	//cv::VideoWriter output_video; 
	CvVideoWriter * output_video;
	if(ui->stream_filemenu->currentIndex() > 1){
	cap = cvCaptureFromAVI(ui->stream_filemenu->currentText().toLocal8Bit().data());
	image = cvQueryFrame(cap);}
	else{
		if(!cam.IsConnected()){
	busMgr.GetCameraFromIndex(0, &guid);
	cam.Connect(&guid);}
	switch(ui->stream_fpdd->currentIndex()){
	case 1: fps = FRAMERATE_7_5; break;
	case 2: fps = FRAMERATE_15; break;
	case 3: fps = FRAMERATE_30; break;
	default : fps = FRAMERATE_15;
	}
	/*switch(ui->stream_fpdd->currentIndex()){
	case 1: vd = VIDEOMODE_640x480Y8;  break;
	case 2: vd = VIDEOMODE_640x480Y16; break;
	case 4: vd = VIDEOMODE_1280x960Y8; break;
	case 5: vd = VIDEOMODE_1280x960Y16; break;
	case 6: vd = VIDEOMODE_1280x960RGB; break;
	default : vd = VIDEOMODE_1280x960RGB; 
	}*/
	switch(ui->Videomode_dd->currentIndex()){
	case 1: vd = VIDEOMODE_640x480Y8; src_img = cvCreateImage(cvSize(480,640),IPL_DEPTH_8U,1); break;
	case 2: vd = VIDEOMODE_640x480Y16; src_img = cvCreateImage(cvSize(480,640),IPL_DEPTH_16U,1); break;
	case 4: vd = VIDEOMODE_1280x960Y8; src_img = cvCreateImage(cvSize(960,1280),IPL_DEPTH_8U,1); break;
	case 5: vd = VIDEOMODE_1280x960Y16; src_img = cvCreateImage(cvSize(960,1280),IPL_DEPTH_16U,1); break;
	case 6: vd = VIDEOMODE_1280x960RGB; src_img = cvCreateImage(cvSize(960,1280),IPL_DEPTH_8U,3); break;
	default : vd = VIDEOMODE_1280x960RGB; src_img = cvCreateImage(cvSize(960,1280),IPL_DEPTH_8U,3); 
	}
	cam.SetVideoModeAndFrameRate(static_cast<VideoMode>(vd),fps);
	output_video = cvCreateVideoWriter("Video_output.wmv",CV_FOURCC('W','M','V','2'),30,Size(960,1280),1);
	//output_video = VideoWriter("Video_output.avi",CV_FOURCC('W','M','V','3'),30, Size(960,1280),true);
	/*(cv::VideoWriter output_cap("Video_output.wmv",1,cap.get(CV_CAP_PROP_FPS),
               cv::Size(cap.get(CV_CAP_PROP_FRAME_WIDTH),
               cap.get(CV_CAP_PROP_FRAME_HEIGHT)));*/
	/*Format7ImageSettings form7sett;
	form7sett.width   = 960; 
	form7sett.height  = 1280;  
	form7sett.mode    = MODE_3;  
	form7sett.offsetX = 0;  
	form7sett.offsetY = 0;  
	form7sett.pixelFormat = PIXEL_FORMAT_RGB;
	bool valid;
	Format7PacketInfo fmt7PacketInfo;
	cam.ValidateFormat7Settings(&form7sett, &valid, &fmt7PacketInfo );
	cam.SetFormat7Configuration(&form7sett,fmt7PacketInfo.recommendedBytesPerPacket);
	// Set Format 7 (partial image mode) settings	*/	
	cam.StartCapture();
	}
	//cv::VideoCapture cap("Video_demo.wmv");
	//unsigned int rowBytes = (double).GetReceivedDataSize()/(double)cf2Img.GetRows();
//cv::Mat opencvImg = cv::Mat( cf2Img.GetRows(), cf2Img.GetCols(), CV_8UC3, cf2Img.GetData(),
//rowBytes );
	namedWindow("Test",WINDOW_AUTOSIZE);
	while (waitKey(30) != 27 && !stop_capture) {
		//qDebug() << "Thread Id :"<<QThread::currentThreadId();
		//ui->camwn_label->setText("Loopcount : "+QString::number(loopcount));
		if(ui->stream_filemenu->currentIndex() == 1){
		error = cam.RetrieveBuffer( &rawImage );
		
		if (error != FlyCapture2::PGRERROR_OK){
			continue;
		}
        //metadata = rawImage.GetMetadata();
       // cout << "framecount: " << metadata.embeddedFrameCounter << endl;
		rawImage.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &convertedimage);
		//src_img = Mat(convertedimage.GetRows(),convertedimage.GetCols(),CV_8UC1);
		src_img = cvCreateImage(cvSize(convertedimage.GetCols(),convertedimage.GetRows()),IPL_DEPTH_8U,3);
		memcpy(src_img->imageData, static_cast<uchar *>(convertedimage.GetData()), convertedimage.GetDataSize());
		if(loopcount == 0)
			img_stream = Size(src_img->width,src_img->height);
		cvWriteFrame(output_video,src_img);
		cv::resize(cv::Mat(src_img),temp,Size(ui->Video_wn->width(),ui->Video_wn->height()));
		//output_video.write(cv::Mat(src_img));
		temp.copyTo(image);
		if(clicks && clicks <= ui->wayptsno_le->text().toInt() && ui->setwaypts_cb->isChecked()){
			cvInitFont(&font,CV_FONT_HERSHEY_PLAIN,1.0,1.0);
			for(int k = 0; k < clicks; k++)
			{
				cvDrawCircle(&IplImage(image),cvPoint(waypoints[k].x,waypoints[k].y),3,Scalar::all(255),1);
				string point = string("Waypoint") + std::to_string(long double(k + 1));
				cvPutText(&IplImage(image),point.c_str(),Point(waypoints[k].x + 3, waypoints[k].y + 3),&font,Scalar::all(255));
			}
		}
		if(pixelcheck.x && (ui->pixelval_cb->isChecked() || ui->backgrndpts_cb->isChecked()))
			cvDrawCircle(&IplImage(image),pixelcheck,3,Scalar::all(255));
		if(selection.x)
			rectangle(image,selection,Scalar(255,0,0),1);
		}
		if(ui->stream_filemenu->currentIndex() > 1){
			cv::resize(cv::Mat(image),temp,Size(ui->Video_wn->width(),ui->Video_wn->height()));
			temp.copyTo(image);}
		//QImage Qim((const uchar *)image.data,image.cols, image.rows, image.step, QImage::Format_RGB32);
		if(ui->track_cb->isChecked())
		{break; }//goto track_robot;}
		//if(SIGNAL(manual_finished()))
			//break;
		
		Qim = Mat2QImage(image, Qim);
		//QImage Qim(convertedimage.GetData(),convertedimage.GetCols(),convertedimage.GetRows(),QImage::Format_RGB888);
		if(Qim != NULL){
			scene->clear();
			scene->addPixmap(QPixmap::fromImage(*Qim));}
		//ui->Video_wn->resize(QSize(scene->width(),scene->height()));
		//QGraphicsPixmapItem *item = scene->addPixmap((QPixmap::fromImage(Qim)).scaled(QSize((int)scene->width(), (int)scene->height()),Qt::KeepAspectRatio, Qt::SmoothTransformation));
		//ui->Video_wn->fitInView(QRectF(0, 0,ui->Video_wn->width(), ui->Video_wn->height()),Qt::KeepAspectRatio);
		//image = QImage2Mat(Qim);
		//scene->addPixmap(QPixmap::fromImage(Qim));
		//if(image.data)
		//output_cap.write(image);
		//imshow("Test",image);
		if(ui->stream_filemenu->currentIndex() > 1){
			src_img = cvQueryFrame(cap);
			image = cv::Mat(src_img);
			if(src_img == NULL)
				break;
		}
		loopcount++;
		//output_video.write(image);
		//cam.Disconnect();}
		//if(src_img->imageData != NULL)
		//	free(src_img->imageData);
		//if(src_img->imageDataOrigin != NULL)
			//free(src_img->imageDataOrigin);
		//if(src_img->imageId != NULL)
			//free(src_img->imageId);
		free(src_img->roi);
		free(src_img->tileInfo);
		cvReleaseData(src_img);
		cvReleaseImage(&src_img);
	}
	//error = cam.Disconnect();
	cvReleaseVideoWriter(&output_video);
	if(ui->stream_filemenu->currentIndex() == 1){
	if(error != PGRERROR_OK)
		ui->camwn_label->setText(QString::fromStdString(error.GetDescription()));}
	//on_Stopcapturepb_pushed();
}

void MainWindow::onMouse( int event, int x, int y, int flags, void* param)
	{
	ui->camwn_label->setText("Checking callback");
    if( selectObject )
    {
        selection.x = MIN(x, origin.x);
        selection.y = MIN(y, origin.y);
        selection.width = std::abs(x - origin.x);
        selection.height = std::abs(y - origin.y);
		Mat tmplate = image(selection);
		cvtColor(tmplate,object,CV_RGB2GRAY);
    }
	
    if(event ==  CV_EVENT_LBUTTONDOWN){
        origin = Point(x,y);
        selection = Rect(x,y,0,0);
		selectObject = true;}

	if((event == CV_EVENT_MOUSEMOVE) && selectObject){
		line(image,origin,Point(x,origin.y),Scalar(255,0,0),1);
		line(image,Point(x,y),Point(x,origin.y),Scalar(255,0,0),1);
		line(image,Point(x,y),Point(origin.x,y),Scalar(255,0,0),1);
		line(image,Point(origin.x,y),origin,Scalar(255,0,0),1);
		imshow("Test",image);
		image.copyTo(lbutton_up);
		temp.copyTo(image);}

    if(event ==  CV_EVENT_LBUTTONUP){
        selectObject = false;
		imshow("Test",lbutton_up);
        if( selection.width > 0 && selection.height > 0 )
            trackObject = -1;
		}
}

void MainWindow::mouseHandler(int event, int x, int y, int flags, void * param){
	static_cast<MainWindow*> (param)->image.copyTo(static_cast<MainWindow*> (param)->temp);
	static_cast<MainWindow*> (param)->onMouse(event,x,y,flags, param);
}

double MainWindow::calculate_angle(Point2f* points,RotatedRect rotrect)
{
	double angle; Point2f p1,p2;
	if(sqrt(pow(std::abs(points[1].x - points[2].x),2.0f) + pow(std::abs(points[1].y - points[2].y),2.0f)) > sqrt(pow(std::abs(points[1].x - points[0].x),2.0f) + pow(std::abs(points[1].y - points[0].y),2.0f)))
		{p1 = points[1]; p2 = points[2];}
	else
		{p1 = points[0]; p2 = points[1];}
	if(p1.x < p2.x)
		angle = atan(std::abs(p1.y - p2.y) / std::abs(p1.x - p2.x)) * (180 * 7 / 22);
	else
		angle = 180.00 - atan(std::abs(p1.y - p2.y) / std::abs(p1.x - p2.x)) * (180 * 7 / 22);
	if(angleprev == 0 && angle == 180)
		angle = 0;
	return angle;}

void MainWindow::mousePressEvent(QMouseEvent * ev){
	mouseevent_valid = false;
	if(ev->x() < ui->Video_wn->x() - 1|| ev->x() > ui->Video_wn->x() - 34 + ui->Video_wn->width())
		return;
	if(ev->y() < ui->Video_wn->y() - 14 || ev->y() > ui->Video_wn->y() - 14 + ui->Video_wn->height())
		return;
	if(!ui->stream_filemenu->currentIndex() > 0)
		return;
	mouseevent_valid = true;
	grabMouse();
	if(image.data){
	cv::resize(cv::Mat(image),temp,Size(ui->Video_wn->width(),ui->Video_wn->height()));
	temp.copyTo(image);
	image.copyTo(lbutton_up);
	//if(ev->flags() == MOUSEEVENTF_LEFTDOWN
	if(ev->button() == Qt::LeftButton && !ui->setwaypts_cb->isChecked() && !ui->backgrndpts_cb->isChecked()){
		origin = Point(ev->x() - ui->Video_wn->x() - 34 ,ev->y() - ui->Video_wn->y() - 14);
        selection = Rect(ev->x() - ui->Video_wn->x() - 34 ,ev->y() - ui->Video_wn->y() - 14,0,0);
		//ui->camwn_label->setText(QString::number(ev->globalX() - ui->Video_wn->x() - 34)+QString(",")+QString::number(ev->globalY() - ui->Video_wn->y() - 34));
		//cvDrawCircle(&IplImage(image),Point(ui->Video_wn->x() - 34 ,ui->Video_wn->y() - 14),3,Scalar::all(255),1);
		Qim = Mat2QImage(image, Qim);
		scene->addPixmap(QPixmap::fromImage(*Qim));
		image.copyTo(temp);
		selectObject = true;}
}
}
void MainWindow::mouseMoveEvent(QMouseEvent * ev){
	if(!mouseevent_valid || ui->backgrndpts_cb->isChecked())
		return;
	if(image.data){
	cv::resize(cv::Mat(image),temp,Size(ui->Video_wn->width(),ui->Video_wn->height()));
	temp.copyTo(image);
	image.copyTo(temp);
	if(selectObject){
		line(image,origin,Point(ev->x()-ui->Video_wn->x() - 34 ,origin.y),Scalar(255,0,0),1);
		line(image,Point(ev->x()-ui->Video_wn->x() - 34 ,ev->y()-ui->Video_wn->y() - 14),Point(ev->x()-ui->Video_wn->x() - 34 ,origin.y),Scalar(255,0,0),1);
		line(image,Point(ev->x()-ui->Video_wn->x() - 34 ,ev->y()-ui->Video_wn->y() - 14),Point(origin.x,ev->y()-ui->Video_wn->y() - 14),Scalar(255,0,0),1);
		line(image,Point(origin.x,ev->y()-ui->Video_wn->y() - 14),origin,Scalar(255,0,0),1);
		selection.x = MIN(ev->x()-ui->Video_wn->x() - 34 , origin.x);
        selection.y = MIN(ev->y()-ui->Video_wn->y() - 14, origin.y);
        selection.width = std::abs(ev->x()-ui->Video_wn->x() - 34- origin.x);
        selection.height = std::abs(ev->y()-ui->Video_wn->y() - 14 - origin.y);
		Qim = Mat2QImage(image, Qim);
		scene->addPixmap(QPixmap::fromImage(*Qim));
		image.copyTo(lbutton_up);
		temp.copyTo(image);
	}
}
}
void MainWindow::mouseReleaseEvent(QMouseEvent *ev){
	if(!mouseevent_valid)
		return;
	if(ev->x() < ui->Video_wn->x() - 1|| ev->x() > ui->Video_wn->x() - 34 + ui->Video_wn->width())
		return;
	if(ev->y() < ui->Video_wn->y() - 14 || ev->y() > ui->Video_wn->y() - 14 + ui->Video_wn->height())
		return;
	if(image.data){
	cv::resize(cv::Mat(image),temp,Size(ui->Video_wn->width(),ui->Video_wn->height()));
	temp.copyTo(image);}
	if(ui->backgrndpts_cb->isChecked()){
		backgrnd_clicks++;
		if(backgrnd_clicks <= ui->backgrndno_le->text().toInt()){
			pixelcheck = Point(ev->x() - ui->Video_wn->x() - 34 ,ev->y() - ui->Video_wn->y() - 14);
			backgrnd_pts[backgrnd_clicks - 1] = Point(ev->x() - ui->Video_wn->x() - 34 ,ev->y() - ui->Video_wn->y() - 14);}
		else backgrnd_clicks = 0;
		ui->backgrndclicks_label->setText(QString::number(backgrnd_clicks));
		releaseMouse();
		return;
	}
	if(ui->pixelval_cb->isChecked() && image.data){
		pixelcheck = Point(ev->x() - ui->Video_wn->x() - 34 ,ev->y() - ui->Video_wn->y() - 14);
		cvDrawCircle(&IplImage(image),Point(ev->x() - ui->Video_wn->x() - 34 ,ev->y() - ui->Video_wn->y() - 14),3,Scalar::all(255));
		cvtColor(image,gimage,CV_BGR2GRAY);
		ui->pixelvalue->setText(QString::number(gimage.at<uchar>(Point(pixelcheck))));
	}
	if(ui->setwaypts_cb->isChecked())
	{   
		if(clicks == 0)
			image.copyTo(waypts_sans);
		clicks++; string point; 
		if(clicks <= ui->wayptsno_le->text().toInt()){
			waypoints[clicks - 1] = Point2f(ev->x() - ui->Video_wn->x() - 34 ,ev->y() - ui->Video_wn->y() - 14);
			//for(int k = 0; k < clicks; k++)
			//{
				//ui->camwn_label->setText(QString::number(clicks));
				/*cvDrawCircle(&IplImage(image),cvPoint(waypoints[clicks - 1].x,waypoints[clicks - 1].y),3,Scalar::all(255),1);
				point = string("Waypoint") + std::to_string(long double(clicks));
				cvPutText(&IplImage(image),point.c_str(),Point(waypoints[clicks - 1].x + 3, waypoints[clicks - 1].y + 3),&font,Scalar::all(255));*/
			//}
			//cvDrawCircle(&IplImage(image),cvPoint(ev->x(),ev->y()),3,Scalar(0,255,0),1);
			//point = string("Destination") + std::to_string(long double(clicks));
			//cvPutText(&image,point.c_str(),Point(ev->x() + 3, ev->y()),&font,Scalar::all(255));
		}
		else{
			clicks = 0; if(ui->stream_filemenu->currentIndex() > 1) waypts_sans.copyTo(image); }
		Qim = Mat2QImage(image, Qim);
		scene->addPixmap(QPixmap::fromImage(*Qim));
		releaseMouse();
		return;
	}
	//clicks = 0;
	if(image.data){
	if(ev->button() == Qt::LeftButton && selectObject)
	{
		selection.x = MIN(ev->x()-ui->Video_wn->x() - 34 , origin.x);
        selection.y = MIN(ev->y()-ui->Video_wn->y() - 14, origin.y);
        selection.width = std::abs(ev->x()-ui->Video_wn->x() - 34 - origin.x);
        selection.height = std::abs(ev->y()-ui->Video_wn->y() - 14 - origin.y);
		Qim = Mat2QImage(lbutton_up, Qim);
		scene->addPixmap(QPixmap::fromImage(*Qim));
		selectObject = false;
		releaseMouse();
	}
	}}

void MainWindow::on_getpixelval_cb_clicked(){
	if(ui->pixelval_cb->isChecked())
		ui->pixelvalue->setEnabled(true);
	else
		ui->pixelvalue->setEnabled(false);
}

void MainWindow::on_backgrndpts_cb_clicked(){
	if(!ui->backgrndpts_cb->isChecked()){
		ui->backgrndclicks_label->setEnabled(false);
		ui->learnbackgnd_pb->setEnabled(false);}
	else{ui->learnbackgnd_pb->setEnabled(true);
	ui->backgrndclicks_label->setEnabled(true);}}

void MainWindow::keyPressEvent(QKeyEvent * ev){
	//ui->camwn_label->setText(QString("Key event : ")+QString::number(ev->key()));
	if(ev->key() == 16777216){
		ui->track_cb->setChecked(false);
		waypointspath_control * wayptsthread = (waypointspath_control *)wayptspath_thread;
		if(wayptsthread->isRunning()){
			//QMutexLocker locker(&mutex);
			mutex.lock();
			wayptsthread->wayptno = clicks;
			//CloseHandle(serialHandle);
			wayptsthread->quit(); wayptsthread->wait();
			mutex.unlock();
		}
	}
}

void MainWindow::nextwaypoint_pb_pushed(){
	waypointspath_control * wthread = (waypointspath_control *)wayptspath_thread;
	if(wthread->isRunning() && wthread->wayptno < ui->autowaypts_pb->text().toInt()){
		mutex.lock();
		wthread->wayptno++;
		mutex.unlock();}
}

void MainWindow::learnTemplate()
{
	time_t start, end;
	start=clock();
	//ui->camwn_label->setText(QString::number(selectObject));
		/*while(clock() - start != 1000)
		continue;*/
	tmplate = image(selection);
	cvtColor(tmplate,object,CV_RGB2GRAY);
	namedWindow("ROI");
	//if(!selectObject){
	cvSetMouseCallback("Test", mouseHandler , this );//selectObject = 1;}
	if(object.data) {
		imshow("ROI",object);}
	loopcount = 0;
	//if(!selectObject)
		//on_trackbt_pushed();
}

void MainWindow::learn_backgroundfield(){
	if(cam.IsConnected() && image.data)
	{
		cvtColor(image,update_tmp,CV_BGR2GRAY);
		GaussianBlur(update_tmp,update_tmp,Size(3,3),0,0,BORDER_DEFAULT);
		cv::threshold(update_tmp,update_tmp, ui->bckgrdthreshlimit_le->text().toInt(),ui->bckgrdthreshmax_le->text().toInt(),ui->bckgrdthreshtype_dd->currentText().toInt());
		//imshow("Background Field",update_tmp);
		findContours(update_tmp, backgrnd_contours, hierarchy, CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);	
		storage = cvCreateMemStorage(0);
		//cvFindContours(&IplImage(update_tmp), storage, &backgrnd_contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
		temp = Mat::zeros(image.size(), CV_8UC1);
		//im = cvCreateImage(Size(image.cols,image.rows),CV_8U,1);
		contour = 0;
		//if(backgrnd_contours.size())
			//backgrnd_contours.clear();
		//backgrnd_contours = std::vector<vector<Point>>(contours.size());
		if(backgrnd_contnos.size())
			backgrnd_contnos.clear();
		//for(int i = 0; i < contours.size(); i++)
			//approxPolyDP(contours[i],backgrnd_contours[i],3,1);
		//backgrnd_approx = cvApproxPoly(backgrnd_contours,sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(backgrnd_contours)*0.02,0);
		//qDebug() << " approx Poly Contours Size :" << backgrnd_contours.size()<< " CvSeq  Contours Size : " << contours.size();
		//CvSeq * current = backgrnd_contours;
		//backgrnd_contours.resize(backgrnd_contours,
		//for (current = backgrnd_contours; current != NULL; current = current -> h_next)
		for(int i = 0; i < backgrnd_contours.size(); i++)
		{
			//mm = cv::moments(contours[i], false);
			for(int j = 0; j < ui->backgrndno_le->text().toInt(); j++){
				//drawContours(temp,backgrnd_contours,i, Scalar::all(255),1);
			//	ui->camwn_label->setText(QString::number(cvPointPolygonTest(&Mat(contours[i]),backgrnd_pts[j],1) < 15));
				if(std::abs(pointPolygonTest(Mat(backgrnd_contours[i]),backgrnd_pts[j],1)) < 5){
				//if(boundingRect(Mat(contours[i])).contains(backgrnd_pts[j]))
					//backgrnd_approx = current; 
						if(std::find(backgrnd_contnos.begin(),backgrnd_contnos.end(),i) == backgrnd_contnos.end())
							backgrnd_contnos.push_back(i);}
					qDebug() << " Point check on contour Polygon "<<i<<" approx : "<<pointPolygonTest(Mat(backgrnd_contours[i]),backgrnd_pts[j],1);
				}
		}
		//temp = cv::Mat(image.size(),CV_8UC1);
		qDebug() << " approx Poly Contours no :" << contour << " CvSeq  Contours Size : " << contours.size();
		if(backgrnd_contours.size()){
			for(int i = 0; i < backgrnd_contnos.size(); i++){
			for(int j = 0; j < backgrnd_contours[backgrnd_contnos[i]].size(); j++){
				backgrnd_contours[backgrnd_contnos[i]][j].x = backgrnd_contours[backgrnd_contnos[i]][j].x * 300 / image.cols;
				backgrnd_contours[backgrnd_contnos[i]][j].y = backgrnd_contours[backgrnd_contnos[i]][j].y * 200 / image.rows;
			}
			drawContours(temp,backgrnd_contours,backgrnd_contnos[i],Scalar::all(255));
			}
		}
		//cvDrawContours(&IplImage(temp),backgrnd_contours->h_next->h_next,Scalar::all(255),Scalar::all(255),100,1);
		im = &IplImage(temp);
		imshow("Background Field",cv::Mat(im));
	}}

void MainWindow::on_trackbt_pushed(){
	/*else{
	if(ui->stream_filemenu->currentIndex() == 2)
		object = cvLoadImage("Template_video3.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	else if(ui->stream_filemenu->currentIndex() == 3)
		object = cvLoadImage("Template_mr.jpg",CV_LOAD_IMAGE_GRAYSCALE);}*/
	//namedWindow("Template",CV_WINDOW_AUTOSIZE);
	//imshow("Template",object);
   /* if( !object.data)
    {
        std::cout<< "Error reading object " << std::endl;
        return;
    }*/
	int key  = 0;
	loopcount = 0;
	cv::BackgroundSubtractorMOG2 bgsub;
	//object = cvLoadImage("Template_video3.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	string filename = ui->filename_le->text().toStdString() + ".txt";
	FILE * fp = fopen(filename.c_str(),"w+");
	if(fp == NULL)
		return;
	filename = "Position of COM       Orientation(degrees)        Frame No.\n";
	fputs(filename.c_str(),fp);
	Mat fgmask, segm, roi, buff, fmask, obj_or;object.copyTo(obj_or);
	//namedWindow("Check");
    //Detect the keypoints using SURF Detector
    int minHessian = ui->featthresh_dd->currentText().toInt();
	GaussianBlur( object, object, Size(3,3), 0, 0, BORDER_DEFAULT );
	FeatureDetector * detector = new SurfFeatureDetector(minHessian);
	bgsub.nmixtures = 3;
	//bgsub.set("initializationFrames",20);
	//bgsub.set("decisionThreshold",0.7);
	//cv::Ptr<SurfFeatureDetector> detector = cv::FeatureDetector::create("SURF");
    //SurfFeatureDetector detector( minHessian );
    std::vector<KeyPoint> kp_object;
	std::vector<KeyPoint> kp_img;
	QString extract_thresh = ui->extractthresh_dd->currentText();
	DescriptorExtractor * extractor;
	if(extract_thresh == "Default")
		extractor = new SurfDescriptorExtractor() ;
	else
		extractor = new SurfDescriptorExtractor(extract_thresh.toInt()) ;
    Mat des_object, des_img;
	FlannBasedMatcher matcher;
	namedWindow("Test");
	if(ui->stream_filemenu->currentIndex() > 1)
		cap = cvCaptureFromAVI(ui->stream_filemenu->currentText().toLocal8Bit().data());	
	int Key = 0;
	//while(Key != 27 && !object.data && im != NULL){
	//	cvShowImage("Test",im);		im = cvQueryFrame(cap);
	//	Key = cvWaitKey(30);}
	//if(im == NULL)
	//	return;
	double robot_area = 0.0;
	Moments mt;
   // detector -> detect( object, kp_object );
	 std::vector<KeyPoint> kp_image;
     std::vector<vector<DMatch > > matches;
     std::vector<DMatch > good_matches;
     std::vector<Point2f> obj;
	 std::vector<Point2f> scene;
     std::vector<Point2f> scene_corners(4);
    //Calculate descriptors (feature vectors)
     Mat des_image, img_matches;
	 Mat H;
    std::vector<Point2f> obj_corners(4);
    //Get the corners from the object
    obj_corners[0] = cvPoint(0,0);
    obj_corners[1] = cvPoint( object.cols, 0 );
    obj_corners[2] = cvPoint( object.cols, object.rows );
    obj_corners[3] = cvPoint( 0, object.rows );
	Mat background,mask;
	//cv::Mat mask(object.rows,object.cols,object.depth());
	cv::Rect rectLine;
	std::vector<Vec4i> hierarchy;
	Mat prevframe;
	double maxC_area, maxCarea;
	int contour, cam_mot = 0,contor; 
	Mat frame;
	//Point2f COM;
	mfeature = 0, thresh_type = 0, thresh = 50, max_thresh = 255;
	IplImage * frameprev, * frame1, *frame_small, *object_small = cvCreateImage(cvSize((int)(object.cols / 5),(int)(object.rows / 5)),IPL_DEPTH_8U,object.channels());
    int framecount = 0; int corner_count, or_size;
	std::vector<Point> approx;
	RotatedRect rott_rect;Point2f rotrect[4];
	time_t start,end;Mat boundcn;
	string pos,position[] = {"Position is (",",",")"},angle_t[] = {"Angle",":","degrees"},angle_s;
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
	cam.Connect(&guid);
	cam.StartCapture();
	while (key != 27 && ui->track_cb->isChecked() )
    {
		//cap >> frame;
		if(!cam.IsConnected() || !ui->track_cb->isChecked())
			return;
		cam.RetrieveBuffer(&rawImage);
		rawImage.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &convertedimage);
		im = cvCreateImage(cvSize(convertedimage.GetCols(),convertedimage.GetRows()),IPL_DEPTH_8U,3);
		memcpy(im->imageData, static_cast<uchar *>(convertedimage.GetData()), convertedimage.GetDataSize());
		if(loopcount == 0){
			start = time(NULL);
			frameprev = cvCreateImage(cvGetSize(im),im->depth,im->nChannels);
			cvConvertImage(im,frameprev);
			frame1 = cvCreateImage(cvGetSize(im),IPL_DEPTH_8U,1);
			boundrect.x = selection.x * im->width / image.cols;
			boundrect.y = selection.y * im->height / image.rows;
			boundrect.height = selection.height *  im->height / ui->Video_wn->height();
			boundrect.width = selection.width * im->width / ui->Video_wn->width();
		}
		if(im->imageData == NULL)
			return;
		cvCvtColor(im,frame1,CV_BGR2GRAY);
		//im = &IplImage(frame);
		frame = cv::Mat(frame1);
		GaussianBlur( frame, frame, Size(3,3), 0, 0, BORDER_DEFAULT );
		//IplImage * image = cvCreateImage(cvGetSize(im), im->depth, 1);
		bgsub(frame,fgmask);
		if(loopcount == 0){
			feature_track:
			detector -> detect( object, kp_object );
		extractor->compute( object, kp_object, des_object );
		obj_corners[0] = cvPoint(0,0);
		obj_corners[1] = cvPoint( object.cols, 0 );
		obj_corners[2] = cvPoint( object.cols, object.rows );
		obj_corners[3] = cvPoint( 0, object.rows );
		//printf("Keypoints of the object: %d\n",kp_object.size());
		//cv::Mat mask_roi = Mat::zeros(im->height,im->width,CV_8U);
		//printf("Index check : %f %f",kp_object[0].pt.x,kp_object[1].pt.y);

		//Laplacian(segm,segm,CV_8U,1,2);
		//convertScaleAbs(segm, fgmask);
		//Canny(fgmask,fgmask,100,255,3);
		
		printf("Frame Image type : %d",im-> depth);
		//if(loopcount > 0)
		//{matches.clear(); good_matches.clear();}
		detector->detect( frame1, kp_image);
		//printf("Frame copied Image type : %d",image-> depth);
		extractor->compute( frame1, kp_image, des_image );
		matcher.knnMatch(des_object, des_image, matches, 2);
		printf("No. of matches : %d\n", matches.size());
        for(int i = 0; i < min(des_image.rows-1,(int) matches.size()); i++) //THIS LOOP IS SENSITIVE TO SEGFAULTS
        {
            if((matches[i][0].distance < 0.6*(matches[i][1].distance)) && ((int) matches[i].size()<=2 && (int) matches[i].size()>0))
            {
                good_matches.push_back(matches[i][0]);
            }
		}
        //Draw only "good" matches
		//drawMatches( object, kp_object, image, kp_image, good_matches, img_matches, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
		qDebug()<<"Good Matches : "<<QString::number(good_matches.size());
		if (good_matches.size() >= 4)
        {

            for( int i = 0; i < good_matches.size(); i++ )
            {
                //Get the keypoints from the good matches
                obj.push_back( kp_object[ good_matches[i].queryIdx ].pt );
                scene.push_back( kp_image[ good_matches[i].trainIdx ].pt );
            }

            H = findHomography( obj, scene, CV_RANSAC );

            perspectiveTransform( obj_corners, scene_corners, H);
			cvCopy(im,frameprev);
            //Draw lines between the corners (the mapped object in the scene image )
			line(cv::Mat(im), scene_corners[0], scene_corners[1], Scalar(0, 255, 0), 4 );
			line(cv::Mat(im), scene_corners[1], scene_corners[2], Scalar( 0, 255, 0), 4 );
			line(cv::Mat(im), scene_corners[2], scene_corners[3], Scalar( 0, 255, 0), 4 );
			line(cv::Mat(im), scene_corners[3], scene_corners[0] , Scalar( 0, 255, 0), 4 );
		}
		PrevRect = boundrect;//rectangle(cv::Mat(im),boundrect,Scalar::all(255),1);goto next;}
		//rectLine = boundingRect(cv::Mat(scene_corners));
		//img_matches.convertTo(img_matches,CV_8U);
		//Canny(cv::Mat(image),temp,100,255);
		//convertScaleAbs( frame, temp );
		//printf("\nTemporary image type : %d\n",temp.channels());
		//cvInRangeS(im,cv::Scalar(60,100,100),cv::Scalar(60,255,255),im);
		//GaussianBlur(temp,temp,Size(3,3),0,0,BORDER_DEFAULT);
		mask = Mat::zeros(frame.size(),CV_8UC1);
		for(int i = 0; i < backgrnd_contnos.size(); i++)
			drawContours(mask,backgrnd_contours, backgrnd_contnos[i],Scalar(255),CV_FILLED);
		bitwise_not(mask,mask);
		if(backgrnd_contnos.size())
			frame.copyTo(temp, mask);
		else
			frame.copyTo(temp);
		update_tmp = temp(boundrect);
		cv::threshold(update_tmp,update_tmp,ui->threshlimit_le->text().toInt(),ui->threshmax_le->text().toInt(),ui->threshtype_dd->currentText().toInt());	
		//cv::threshold(cv::Mat(frame1),temp,210,255,0);
		update_tmp.copyTo(boundcn);
		imshow("Check",boundcn);
		//Canny(update_tmp,update_tmp,170,255);
		findContours(update_tmp, contours, hierarchy, CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);		
		maxC_area = 0;
		contour = 0;
		qDebug("\nNumber of contours : %d",contours.size());
		for (int i = 0; i < contours.size(); i++)
		{
			mm = cv::moments(contours[i], false);
			//if(loopcount == 0) {
			if(mm.m00 > maxC_area){
				maxC_area  = mm.m00;contour = i;}//}
		}
		printf("\nContour no. : %d",contour);
		robot_area = contourArea(contours[contour]);
		robot_area  = (robot_area * 200 * 300 )/ (im ->width * im->height);
		printf("\nARea of robot : %f\n",robot_area);
		namedWindow("Check",WINDOW_AUTOSIZE);
		background = cv::Mat(im);
		//mask = frame(rectLine);
		//cvSetImageROI(&IplImage(mask),rectLine);
		//cv::Mat(im).copyTo(background);
		//cvCopy(im,&IplImage(background));
		//rectangle(mask,rectLine,Scalar::all(255),1);
		//frame.copyTo(background,mask);
		boundcn = Mat::zeros(frame.size(),CV_8U);
		drawContours(boundcn,contours,contour,Scalar::all(255),1);	
		//imshow("Check",boundcn);
		roi = cv::Mat(im) - background;
		//roi.copyTo(mask);
		//if(loopcount == 1)
		//robot_area = maxC_area;
		mt = cv::moments(contours[contour]);
		COM.x = mt.m10/mt.m00 + boundrect.x; COM.y = mt.m01/mt.m00 + boundrect.y; COMprev = COM;
		rott_rect = minAreaRect(contours[contour]);
		rott_rect.points(rotrect);
		rectLine = boundingRect(cv::Mat(contours[contour]));
		rectLine.x += PrevRect.x;
		rectLine.y += PrevRect.y;
		object = frame(rectLine);
		if(int(frame.at<uchar>(COM)) < int(frame.at<uchar>(Point(rectLine.x,rectLine.y))))
			thresh_type = 1;
		rott_rect = minAreaRect(contours[contour]);
		rott_rect.points(rotrect);
		printf("Thresh Type : %d\n",thresh_type);
		boundrect = rectLine;
		rectangle(cv::Mat(im),rectLine,Scalar::all(255),2);
		goto printpos;
}	
		prevframe = image;
		//cv::erode(Lpl,Lpl,cv::Mat());
		//cv::dilate(Lpl,Lpl,cv::Mat());
		if(loopcount > 0){
			frame = cv::Mat(frame1);
			cv::resize(frame,temp,Size(300,200));  temp.copyTo(frame); 
			cv::resize(fgmask,temp,Size(300,200)); temp.copyTo(fgmask);
			GaussianBlur(fgmask,fgmask,Size(3,3),0,0);
			//rectangle(cv::Mat(im),boundrect,Scalar::all(255),2);
			if(loopcount > 0){
			boundrect.y = int(boundrect.y * 200/ frame1->height);
			boundrect.x = int(boundrect.x * 300/ frame1->width);
			boundrect.width = boundrect.width * 300 / frame1 -> width;
			boundrect.height = boundrect.height * 200 / frame1 -> height;
			COM.x = COM.x * 300 / im -> width;
			COM.y = COM.y * 200 / im->height;
			}

			//rectangle(cv::Mat(im),boundrect,Scalar::all(255),1);
			if(loopcount > 0){
				printf("Checking entry\n");
				if(thresh_type == 0){
					boundrect.x = boundrect.x - 12 * 300/ frame1 -> width;
					boundrect.y = boundrect.y - 12 * 200/ frame1 -> height;}
				else{
					boundrect.x = boundrect.x - 20 * 300/ frame1 -> width;
					boundrect.y = boundrect.y - 20 * 200/ frame1 -> height;}
				boundrect.height = boundrect.height + int((33 * 300)/ frame1 -> width);
				boundrect.width = boundrect.width + int((33 * 200) / frame1 -> height);}	
			
			PrevRect  = boundrect;
			//cvSetImageROI(&IplImage(fmask),boundrect);
			//printf("Mean Value : %f",mean_matches.val[0]);
			cv::Mat(im).copyTo(segm);
		//segm = cv::Mat(frameprev);
			subtract(segm, cv::Mat(frameprev),segm);
			int check = 0;
		//Laplacian(segm,segm,CV_16S,3,1,0);
		//buff = cv::Mat(frame1);
		//convertScaleAbs(segm,temp);
			cvtColor(segm,buff,CV_RGB2GRAY);
			GaussianBlur(buff,buff,Size(3,3),0);
			morphologyEx(buff,buff,MORPH_CLOSE,cv::Mat());
			cv::threshold(buff,buff,20,255,CV_THRESH_BINARY);

			findContours(buff, contours, hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
			or_size = contours.size();
			printf("Subtracted image type : %d\n",contours.size());
			if(contours.size() >= 50){
			goto track;
			revert:
			mfeature++;
			printf("Mfeature : %d\n",mfeature);
			if((angle < angleprev / 1.5 || angle > angleprev * 1.5)){
				if(mt.m00 < robot_area * 1.3 && mt.m00 > robot_area / 1.3){COM.x = COM.x * im->width / 300; COM.y = COM.y * im -> height / 200;
				goto printpos;}
			}
			else
				if((angle < angleprev / 2 || angle > angleprev * 2))
					boundrect = PrevRect;
			//if(mt.m00 > robot_area * 1.3){
			if(mt.m00 < robot_area / 1.3){
			boundrect.x = boundrect.x - 5 * 300 / frame1 -> width;
			boundrect.y = boundrect.y - 5 * 200 / frame1 -> height;
			boundrect.width = boundrect.width + 15 * 300 / frame1 -> width ;
			boundrect.height = boundrect.height + 15 * 200 / frame1 -> height;}
			if(mt.m00 > robot_area * 1.3 && mfeature < 2){
				boundrect.x = boundrect.x = boundrect.x + 10 * 300 / frame1 -> height;
				boundrect.y = boundrect.y + 10 * 200 / frame1 -> width;
				boundrect.width = boundrect.width - 30 * 300 / frame1 -> width;
				boundrect.height= boundrect.height  - 200 * 30 / frame1 -> height;}
			PrevRect = boundrect;
				//PrevRect = boundrect;
				//mfeature = 1;
				//cvCopy(im,frameprev);
			//if(mfeature == 2)
			//if(or_size == 245){
				//rectangle(fmask,boundrect,Scalar::all(255),1);
				//}
			cvCopy(im,frameprev);
			goto printpos;
			bgsub(frame,fgmask);
			}
		//else{
			fmask = fgmask(boundrect);
			//qDebug("COM : (%f,%f)\n",(COM.x),COM.y);
			//Laplacian(fgmask,fgmask,fgmask.depth(),3);convertScaleAbs(fgmask,fmask);
			GaussianBlur(fmask,fmask,Size(3,3),0,0); 
			mask = Mat(frame.size(),CV_8UC1,Scalar(255));
			for(int k = 0; k < backgrnd_contnos.size(); k++){
				drawContours(mask, backgrnd_contours,backgrnd_contnos[k],Scalar(0), CV_FILLED);}
				//drawContours(fmask,backgrnd_contours,backgrnd_contnos[k],Scalar(0));
			fmask.copyTo(temp);
			goto track;
			//bitwise_not(mask,mask);
			if(backgrnd_contnos.size()){
				fmask = Mat(frame.size(),CV_8UC1);
				temp.copyTo(fmask,mask);}
			cv::threshold(fmask,fmask,thresh,max_thresh,CV_THRESH_BINARY);
			//adaptiveThreshold(fgmask,fgmask,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C,CV_THRESH_BINARY,11,2);
			//medianBlur(fmask,fmask,3);
			//Laplacian(fmask,fmask,fmask.depth(),3);convertScaleAbs(fgmask,fmask);	
			morphologyEx(fmask,fmask,MORPH_CLOSE, cv::Mat());
			//IplImage * fmas = &IplImage(fmask);
			//	cvSetImageROI(fmas,boundrect);

		findContours(cv::Mat(fmask), contours, hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		maxC_area = 0;
		contour = 0;
		for(int i = 0; i < contours.size(); i++)
		{
			mt = cv::moments(contours[i],false);
			if(maxC_area < mt.m00){
				maxC_area = mt.m00;contour = i;}
		}
		//fmask.copyTo(boundcn);
		rectangle(boundcn,boundrect,Scalar::all(255),1);
		printf("Background sub area : %f %f\n",maxC_area,robot_area);
		//approxPolyDP(contours[contour],approx,3,true);
		drawContours(fmask,contours,contour,cv::Scalar(255),1);
		//if(maxC_area == 223.00)
		//{
			fgmask.copyTo(boundcn);
			//drawContours(boundcn,contours,contour,Scalar::all(255),1);
			//rectangle(boundcn,boundrect,Scalar::all(255),1); 
			//imshow("Check", boundcn);
		//}
		boundcn = Mat::zeros(frame.size(),CV_8U);
		if(maxC_area < robot_area / 1.15 || maxC_area > robot_area * 1.15){
			track: 
			boundcn = Mat::zeros(frame.size(),CV_8U);
			recalc:
			//frame.copyTo(temp);	
			//temp.copyTo(boundcn);
			buff = cv::Mat::zeros(frame.size(), CV_8U);
			//medianBlur(buff,buff,3);
			GaussianBlur(buff,buff,Size(3,3),0,0,BORDER_DEFAULT);
			//if(maxC_area == 99.5){adaptiveThreshold(temp,temp,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C ,CV_THRESH_BINARY_INV,11,2);
			//Canny(temp,temp,200,255);}
			//else
			//if(maxC_area == 307.5){
			mask = Mat(frame.size(),CV_8UC1,Scalar::all(255));
			for(int k = 0; k < backgrnd_contnos.size(); k++)
			{
				//drawContours(temp,backgrnd_contours,backgrnd_contnos[k],Scalar(0));
				drawContours(mask,backgrnd_contours,backgrnd_contnos[k],Scalar(0),CV_FILLED);
				for(int j = 0; j < backgrnd_contours[backgrnd_contnos[k]].size(); j++){
					if(boundrect.contains(backgrnd_contours[backgrnd_contnos[k]][j])){
						//if(pointPolygonTest(backgrnd_contours[backgrnd_contnos[k]],Point(boundrect.x,boundrect.y),1) > pointPolygonTest(backgrnd_contours[backgrnd_contnos[k]],Point(boundrect.x+boundrect.width,boundrect.y),1)
						qDebug("Boundary Reached");
						//boundrect = rott_rect.boundingRect();
					}}}
			if(backgrnd_contnos.size()){
			//bitwise_not(mask,mask);
			frame.copyTo(temp,mask);}
			else
				frame.copyTo(temp);
			buff = temp(boundrect);
		//	if(or_size == 120){
				//thresh = 40; thresh_type = 1;}
			imshow("Background Boundary Removal",temp);
			imshow("Check mask",mask);
			max_thresh = 255;
			if(!check && (cv::Mat(frame).at<uchar>(Point(COM)) < cv::Mat(frame).at<uchar>(Point(boundrect.x + boundrect.width, boundrect.y + boundrect.height / 2)) && cv::Mat(frame).at<uchar>(Point(COM)) < cv::Mat(frame).at<uchar>(Point(boundrect.x - boundrect.width / 4, boundrect.y + boundrect.height / 2))))
			{thresh = (cv::Mat(frame).at<uchar>(Point(COM)) + cv::Mat(frame).at<uchar>(Point(boundrect.x + boundrect.width + 30, boundrect.y + boundrect.height / 2))) / 2;thresh_type = 1;}
			//if(!thresh_type && (cv::Mat(frame).at<uchar>(Point((int)COM.x,(int)COM.y)) < 50))
			if(!check && cv::Mat(frame).at<uchar>(Point(COM)) > cv::Mat(frame).at<uchar>(Point(boundrect.x + boundrect.width, boundrect.y + boundrect.height / 2)) && cv::Mat(frame).at<uchar>(Point(COM)) > cv::Mat(frame).at<uchar>(Point(boundrect.x - boundrect.width /4, boundrect.y + boundrect.height / 2)))
			{max_thresh = ui->threshmax_le->text().toInt(); thresh_type = 0; thresh = cv::Mat(frame).at<uchar>(Point(COM)) - cv::Mat(frame).at<uchar>(Point(boundrect.x + boundrect.width, boundrect.y + boundrect.height / 2))/ 2;
			if(angleprev < 30 || angleprev > 150)
				factorx = 3.75;
			else if(angleprev < 45 || angleprev > 135)
				factorx = 6.5;
			else if(angleprev > 45 && angleprev < 135)
				factory = 5.5;
			else if (angleprev > 60 && angleprev < 120)
				factory = 3;
				pixdiffx = cv::Mat(frame).at<uchar>(Point(COM)) - cv::Mat(frame).at<uchar>(Point(COM.x - sqrt(robot_area) / factorx, COM.y));
				pixdiffy = cv::Mat(frame).at<uchar>(Point(COM)) - cv::Mat(frame).at<uchar>(Point(COM.x, COM.y - sqrt(robot_area) / factory));
			//if((cv::Mat(frame).at<uchar>(Point(COM)) - cv::Mat(frame).at<uchar>(Point(COM.x - sqrt(robot_area) / factorx, COM.y)) < thresh / 2 && (angleprev < 45 || angleprev > 135)) || (cv::Mat(frame).at<uchar>(Point(COM)) - cv::Mat(frame).at<uchar>(Point(COM.x, COM.y - sqrt(robot_area) / factory)) < thresh / 2 && (angleprev >= 45 && angleprev <=135)))
				thresh = (0.8* cv::Mat(frame).at<uchar>(Point(COM)) + 0.2 * cv::Mat(frame).at<uchar>(Point(boundrect.x + boundrect.width, boundrect.y + boundrect.height / 2))) / 2;
			//else if(pixdiffx > thresh / 2 && (angleprev < 45 || angleprev > 135))
				//thresh = (cv::Mat(frame).at<uchar>(Point(COM.x - sqrt(robot_area) / factorx, COM.y)) + cv::Mat(frame).at<uchar>(Point(boundrect.x + boundrect.width, boundrect.y + boundrect.height / 2))) / 2;
			//else if(pixdiffy > thresh / 2 && angleprev > 45 && angleprev < 135)
				//thresh = (cv::Mat(frame).at<uchar>(Point(COM.x, COM.y - sqrt(robot_area) / factory)) + cv::Mat(frame).at<uchar>(Point(boundrect.x + boundrect.width, boundrect.y + boundrect.height / 2))) / 2;
			}
			qDebug()<<"COM Pixel "<<cv::Mat(frame).at<uchar>(Point(COM)) << " Difference : "<< pixdiffx << " " << pixdiffy<<" Outside Pixel value : "<<cv::Mat(frame).at<uchar>(Point(boundrect.x + boundrect.width, boundrect.y + boundrect.height / 2));
			cvDrawCircle(im,Point(boundrect.x + boundrect.width, boundrect.y + boundrect.height / 2),3,Scalar::all(255),1);
			 //thresh = cv::Mat(frame1).at<uchar>(Point(COM))- 5;
			if(check == 1 && !thresh_type){
				if(maxCarea < robot_area / 1.35)
					thresh = int(0.8 * thresh) ;
				else
					thresh = int(1.3 * thresh);
			}
			qDebug()<<"Thresh "<<thresh<<" Max Thresh : "<< max_thresh << " Check :"<<check;
			cv::threshold(buff,buff,thresh,max_thresh,thresh_type);
			temp.copyTo(boundcn);
			//ui->camwn_label->setText(QString::number(thresh_type));
			//cv::threshold(boundcn,boundcn,thresh,max_thresh,thresh_type);6
			rectangle(boundcn,boundrect,Scalar::all(255),1);
			//if(cv::Mat(frame).at<uchar>(Point(COM)) < 200)
			imshow("Check",boundcn);
			//if(or_size == 79)
			//adaptiveThreshold(temp,temp,255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,11,2);
		//cv::erode(temp,temp,cv::Mat());
		//cv::dilate(temp,temp,cv::Mat());
			//std::vector<vector<Point>> contors;
			morphologyEx(buff,buff,cv::MORPH_CLOSE,cv::Mat());
			//buff = cv::Mat::zeros(buff.size(),buff.depth());
			//rectangle(temp,boundrect,Scalar::all(255),1);
			//cv::threshold(temp,temp,200,255,CV_THRESH_BINARY);
			//IplImage * bound = &IplImage(temp);
			//cvSetImageROI(bound,boundrect);
			//buff.copyTo(cv::Mat(bound));
			//Canny(buff,buff,80,255);		
			//temp.copyTo(boundcn)'
			//printf("Pixel brightness : %d\n",cv::Mat(frame1).at<uchar>(Point(COM)));
			contours.clear();
			findContours(buff, contours, hierarchy, CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
			maxCarea = 0;
			for (int i = 0; i < contours.size(); i++)
			{
				mt = cv::moments(contours[i], false);
			if(mt.m00 > maxCarea){
				maxCarea  = mt.m00;contor = i;}
			}
		/*	if(std::abs(sqrt(pow((mt.m10 / mt.m00 *  frame1 -> width / 300) - COM.x , 2) + pow((mt.m01 / mt.m00 *  frame1 -> height / 200) - COM.y , 2)) - sqrt(pow(COM.x - COMprev.x , 2) + pow(COM.y - COMprev.y , 2))) > 10  && check && loopcount > 1 ) 
			{
				boundrect = rott_rect.boundingRect();
				PrevRect = boundrect;
			}*/
			//ui->camwn_label->setText("Area : "+QString::number(maxCarea));//rectangle(boundcn,boundrect,Scalar::all(255),1);
				drawContours(boundcn,contours,contor,Scalar::all(255),1);//imshow("Check",boundcn);
			mt = cv::moments(contours[contor]);
			if((mt.m00 < robot_area / 1.2 || robot_area * 1.2 < mt.m00) && check < 1){
				//goto revert;
				printf("Re-cal check : %d\n", check);
				if(mt.m00 < robot_area / 1.2){
				boundrect.x = boundrect.x + 3 * 300 / frame1 -> width;
				boundrect.y = boundrect.y + 3 * 200 / frame1 -> height;
				boundrect.width = boundrect.width + 25 * 300 / frame1 -> width;
				boundrect.height = boundrect.height + 25 * 200 / frame1 -> height;}
				if(mt.m00 > robot_area * 1.2){
					boundrect.x = boundrect.x + 5 * 300/ frame1 -> width;
					boundrect.y = boundrect.y + 5 * 200/ frame1 -> height;
					printf("Width subtraction : %d\n", int((300 * 300) / im -> width));
					boundrect.width = boundrect.width - int((20 * 300) / im -> width);
					boundrect.height = boundrect.height - int(20 *  200/ frame1 -> height);}
				check++;  	PrevRect = boundrect; goto recalc;	}check = 0;
				boundcn = Mat::zeros(frame.size(),CV_8U);
				drawContours(boundcn,contours,contor,Scalar::all(255),1);
				//if(loopcount <= 1)
					//ui->camwn_label->setText(QString::number(maxCarea));
				//if(loopcount == 1){ui->camwn_label->setText("Area : "+QString::number(maxCarea));//rectangle(boundcn,boundrect,Scalar::all(255),1);
			//	imshow("Check",boundcn);}
			if((maxCarea < robot_area / 1.3 || maxCarea > robot_area * 1.3) && mfeature <= 3)
					goto revert;
			boundcn = Mat::zeros(frame.size(),CV_8U);
			drawContours(boundcn,contours,contor,Scalar::all(255),1);
			boundrect = boundingRect(cv::Mat(contours[contor]));
			rectangle(boundcn,boundrect,Scalar::all(255),1);
			rott_rect = minAreaRect(contours[contor]); 
			}
		else{
		boundrect = boundingRect(cv::Mat(contours[contour]));
		mt = cv::moments(contours[contour]);
		rectangle(boundcn,boundrect,Scalar::all(255),1);
		drawContours(boundcn,contours,contour,Scalar::all(255),1);
		rott_rect = minAreaRect(contours[contour]);}
		rott_rect.points(rotrect);
		//if(loopcount == 1)
			//robot_area = mt.m00;
		for(int i = 0; i < 3 ; i++)
			line(boundcn,rotrect[i],rotrect[i+1],Scalar::all(255),1);
		line(boundcn,rotrect[3],rotrect[0],Scalar::all(255),1); 
		cvDrawCircle(&IplImage(boundcn),Point(rotrect[2]),3,Scalar::all(255),1);
		angle = calculate_angle(rotrect,rott_rect);
		boundrect.x += PrevRect.x;
		boundrect.y += PrevRect.y;
		if(!((std::abs(angleprev - angle) > 170 && std::abs(angleprev - angle) <= 180)) && (angle < angleprev / 1.5 || angle > angleprev * 1.5) && mfeature <= 3){printf("Angle curr Angle prev : %f %f\n",angle,angleprev);
			goto revert;}
		printf("Angle: %f\n",calculate_angle(rotrect,rott_rect));
		COM.x = mt.m10/mt.m00; COM.y = mt.m01/mt.m00;
		COM.x += PrevRect.x; COM.y += PrevRect.y;
		//boundcn = Mat::zeros(frame.size(),CV_8U);
		//cvDrawCircle(&IplImage(boundcn),Point(COM.x,COM.y),5,Scalar::all(255),1);	
		COM.x = (COM.x *  frame1 -> width) / 300;
		COM.y = (COM.y * frame1 -> height) / 200;
		COMprev = COM;
		mfeature = 0;
		PrevRect = boundrect;
		//if(maxC_area == 3772.5)
			//thresh_type;
		//printf("Pixel Values : %d\n",int(frame.at<uchar>(COM)));
		int x = COM.x ;int y= COM.y;
		printf("COM : %d %d \n", x,y);
		//rectangle(temp,boundrect,Scalar::all(255),1); cvDrawCircle(&IplImage(boundcn),Point(boundrect.x,boundrect.y),5,Scalar::all(255),1);
		//if(loopcount == 2){

		//cvResetImageROI(fmas);
		cvCopy(im,frameprev);
		//boundcn = Mat::zeros(cv::Mat( im).size(), cv::Mat(im).depth());
		printf("Check: %d\n",im -> width);
		//temp = cv::Mat(frame1);
		//temp = temp(boundrect);
		//cvtColor(temp,temp,CV_RGB2GRAY);
		obj_or.copyTo(object);
		//cvResetImageROI(&IplImage(fmask));
		}

printpos : 
		if(loopcount > 0){
		boundrect.y = boundrect.y * frame1 -> height / 200;
		boundrect.x = boundrect.x * frame1 -> width / 300 ;
		boundrect.width = (boundrect.width * frame1 -> width) / 300;
		boundrect.height = (boundrect.height * frame1 -> height) / 200;
		rectangle(cv::Mat(im),boundrect,Scalar::all(255),2);}
		printf("Height initial : %d\n", boundrect.height);
		printf("Height scaled up : %d\n",boundrect.height);
		//if(loopcount == 0)
		//{ 
			//COM.x = boundrect.x + boundrect.width / 2; COM.y= boundrect.y + rectLine.height / 2;}
		pos = position[0] + std::to_string(long double(COM.x)) + position[1] + std::to_string(long double(COM.y))+position[2];
		if(loopcount == 0)
			angleprev = angle = calculate_angle(rotrect,rott_rect);
		if((!mfeature) && (angle > angleprev / 1.5 && angle < angleprev * 1.5))
		{angleprev = calculate_angle(rotrect,rott_rect);printf("Angle prev update \n"); framecount = loopcount; }
		if((loopcount - framecount > 3) && !mfeature)
			angleprev = calculate_angle(rotrect,rott_rect);
		ui->angle->setText(QString::number(angleprev));
		ui->pos_COMx->setText(QString::number(COM.x));
		ui->pos_COMy->setText(QString::number(COM.y));
		angle_s = angle_t[0]+angle_t[1]+std::to_string(long double(angleprev))+angle_t[2];
		cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX,1.0,1.0,0);
		CvPoint org;org.x = COM.x; org.y = COM.y;
		cvPutText(im,angle_s.c_str(),cvPoint(20,40),&font,Scalar(0,255,255));
		//cvPutText(im,pos.c_str(),org,&font, cv::Scalar(255,255,255));

next:
		//ui->tracksett_gb->setText("Robot Area : "+QString::number(robot_area));
		//ui->camwn_label->setText(QString("Thresh Type : ")+QString::number(thresh_type));
		//thresh_type = 0;
		String info = pos.substr(12);
		info += std::string(int(filename.find("tation") - info.find(")")),' ') + std::to_string(long double(calculate_angle(rotrect,rott_rect)));
		info += std::string(int(filename.find("me No.")) - info.size() ,' ') + std::to_string(long double(loopcount + 1)) + "\n";
		fwrite(info.c_str(),sizeof(char), info.size(),fp); 
		cvDrawCircle(im,Point(COM),5,Scalar(255,0,255),1);
		cvDrawCircle(im,Point(boundrect.x + boundrect.width + 20,boundrect.y + boundrect.height / 2),4,Scalar::all(255),1);
		if(loopcount == 5)
		imshow("Good Matches", cv::Mat(im));//break;
		key = waitKey(10);
		cv::resize(cv::Mat(im),temp,Size(ui->Video_wn->width(),ui->Video_wn->height()));
		if(clicks && clicks <= 4 && ui->setwaypts_cb->isChecked()){
			cvInitFont(&font,CV_FONT_HERSHEY_PLAIN,1.0,1.0);
			for(int k = 0; k < clicks; k++)
			{
				cvDrawCircle(&IplImage(temp),waypoints[k],3,Scalar(180,180,0),1);
				point = string("Waypoint") + std::to_string(long double(k + 1));
				cvPutText(&IplImage(temp),point.c_str(),Point(waypoints[k].x + 3, waypoints[k].y + 3),&font,Scalar::all(180));
			}
		}
		Qim = Mat2QImage(temp, Qim);
		this->scene->addPixmap(QPixmap::fromImage(*Qim));
	//	delete &temp;
	//	ui->Video_wn->fitInView(QRectF(0, 0,ui->Video_wn->width(), ui->Video_wn->height()),Qt::KeepAspectRatio);
		loopcount++;
		//imshow("Background",object);
		if(loopcount == 100){
			end = time(NULL);
			ui->camwn_label->setText("Frame processing rate"+QString::number(difftime(end,start)));}
	cvReleaseImage(&im); //cvReleaseImage(&frameprev); cvReleaseImage(&frame1);
	}
	cvReleaseImage(&frameprev);
	cvReleaseImage(&frame1);
	fclose(fp);
	delete detector;
    return;
}

void MainWindow::DelayMS(UINT delay)
{
	DWORD start, end;
	start = end = GetTickCount();

	// Wait for 'delay' milliseconds.
	while ((end - start) < delay)
	{
		end = GetTickCount();
	}
}

void MainWindow::on_Stopcapturepb_pushed(){
	//ui->camwn_label->setText("Stop Capture Slot entered"); 
	stop_capture = true;
	/*if(ui->stream_filemenu->currentIndex() == 1){
		//cam.RetrieveBuffer(&rawImage);
		stop_capture = true;
		error = cam.StopCapture();
		if(error != PGRERROR_OK)
			error.PrintErrorTrace();
		Sleep(25);
		SetEvent(m_threadDoneEvent);
		WaitForSingleObject(m_threadDoneEvent,500);
		error = cam.Disconnect();
		if(error != PGRERROR_OK)
			error.PrintErrorTrace();
	}*/
	if(scene == NULL){
		scene  = new QGraphicsScene(this);
		ui->Video_wn->setScene(scene);}
	if(image.data)
		scene->addPixmap(QPixmap::fromImage(*Qim));
	//QCoreApplication::instance()->exit();
}

bool MainWindow::send_serialportcommand(BYTE * data_command){
	/*DCB serialParams = {0};
	COMMTIMEOUTS timeout = {0};
	serialParams.DCBlength = sizeof(serialParams);

	GetCommState(serialHandle, &serialParams);
	serialParams.BaudRate = 9600;
	serialParams.ByteSize = 8;
	serialParams.StopBits = ONESTOPBIT;
	serialParams.Parity = 0;
	GetCommState(serialHandle, &serialParams);

	timeout.ReadIntervalTimeout = 50;
	timeout.ReadTotalTimeoutConstant = 50;
	timeout.ReadTotalTimeoutMultiplier = 50;
	timeout.WriteTotalTimeoutConstant = 50;
	timeout.WriteTotalTimeoutMultiplier = 10;
	SetCommTimeouts(serialHandle, &timeout);*/
	DWORD toBeWritten = 7;
	DWORD* written = (DWORD*)malloc(sizeof(DWORD));
	LPDWORD event;
	//LPOVERLAPPED ov;
	//memset(&ov,0,sizeof(ov));
	SetCommMask(serialHandle, EV_TXEMPTY);
	if(WaitCommEvent(serialHandle, event,NULL) && event)
		return WriteFile(serialHandle, (LPVOID)data_command, toBeWritten, written, NULL);
}

void MainWindow::retrieve_time(){
	ui->camwn_label->setText("Checking slot execution");
	time_duration = ui->td_le->text().toInt();}

void MainWindow::get_coilcommand(QString coil){
	stringstream ss;
	switch(coil.toStdString().c_str()[0])
	{
	case 'U' : ss << ui->upc_in->text().toStdString();
		ss >> std::hex >> value; break;

	case 'D':
		ss << ui->downc_in->text().toStdString();
		ss >> std::hex >> value; break;

	case 'L':
		ss << ui->leftc_in->text().toStdString();
		ss >> std::hex >> value; break;

	case 'R':
		ss << ui->rightc_in->text().toStdString();
		ss >> std::hex >> value; break;
	}
}


/*void MainWindow::on_coilcontrol_manual_pushed(QString coil)
{
	for(int i = 0; i < 7; i++)
	{datasend[i] = 0; prevsent[i] = 0x00;}
	bool success = false, check;
	//serialHandle = NULL;
	stringstream ss; 
	CString comport = "\\\\.\\COM4";
	//ui->camwn_label->setText(coil);
	//if(first_press)
		//CloseHandle(serialHandle);
	serialHandle = CreateFile(comport, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//if(serialHandle == INVALID_HANDLE_VALUE)
		//ui->camwn_label->setText("Error: Invalid Handle Value for Device 1."); 
	DCB serialParams = {0};
	COMMTIMEOUTS timeout = {0};
	serialParams.DCBlength = sizeof(serialParams);

	GetCommState(serialHandle, &serialParams);
	serialParams.BaudRate = 9600;
	serialParams.ByteSize = 8;
	serialParams.StopBits = ONESTOPBIT;
	serialParams.Parity = 0;
	GetCommState(serialHandle, &serialParams);

	timeout.ReadIntervalTimeout = 50;
	timeout.ReadTotalTimeoutConstant = 50;
	timeout.ReadTotalTimeoutMultiplier = 50;
	timeout.WriteTotalTimeoutConstant = 50;
	timeout.WriteTotalTimeoutMultiplier = 10;
	SetCommTimeouts(serialHandle, &timeout);
	DWORD toBeWritten = 7, toBeRead = 7;
	DWORD* written = (DWORD*)malloc(sizeof(DWORD));
	DWORD* read = (DWORD*)malloc(sizeof(DWORD));
	LPDWORD event = (DWORD *)malloc(sizeof(DWORD));int ev;
	char coil_ch = coil.toStdString().c_str()[0];
	switch(coil_ch){
	
	case 'U':
		ss << ui->upc_in->text().toStdString();
		ss >> std::hex >> value;
		datasend[3] = value;	
		//ui->camwn_label->setText(QString::number(datasend[0]));
		//SetCommMask(serialHandle, EV_TXEMPTY);
		//if(WaitCommEvent(serialHandle, event , NULL)){
		//check = send_serialportcommand(datasend);
		check = WriteFile(serialHandle, (LPVOID)datasend, toBeWritten, written, NULL);
		//GetCommMask(serialHandle,event);}
		//send_serialportcommand(datasend);
		DelayMS(ui->td_le->text().toInt());
		if(check)
		//check = send_serialportcommand(prevsent);
		//datasend[0] = 0x00;
		check = WriteFile(serialHandle, (LPVOID)prevsent, toBeWritten, written, NULL);
	//status = SI_Write(*hDev1, (LPVOID)dataSent, toBeWritten, written);
		if(!check)
		{
			ui->camwn_label->setText("Surprise Removal on Device 1 Write!");
		}
		break;

	case 'D':
		ss << ui->downc_in->text().toStdString();
		ss >> std::hex >> value;
		datasend[2] = value;
		//send_serialportcommand(datasend);
		check = WriteFile(serialHandle, (LPVOID)datasend, toBeWritten, written, NULL);
		DelayMS(ui->td_le->text().toInt());
		//send_serialportcommand(prevsent);
		check = WriteFile(serialHandle, (LPVOID)prevsent, toBeWritten, written, NULL);
	//status = SI_Write(*hDev1, (LPVOID)dataSent, toBeWritten, written);
		if(!check)
		{
			printf("Surprise Removal on Device 1 Write!");
		}break;

	case 'L':
		QCoreApplication::processEvents();
		ss << ui->leftc_in->text().toStdString();
		ss >> std::hex >> value;
		//ui->camwn_label->setText(QString::number(value));
		datasend[5] = int(value);
		check = WriteFile(serialHandle, (LPVOID)datasend, toBeWritten, written, NULL);
		DelayMS(ui->td_le->text().toInt());
		check = WriteFile(serialHandle, (LPVOID)prevsent, toBeWritten, written, NULL);
	//status = SI_Write(*hDev1, (LPVOID)dataSent, toBeWritten, written);
		if(!check)
		{
			printf("Surprise Removal on Device 1 Write!");
		}break;
	
	case 'R':
		ss << ui->rightc_in->text().toStdString();
		ss >> std::hex >> value;
		datasend[6] = value;
		check = WriteFile(serialHandle, (LPVOID)datasend, toBeWritten, written, NULL);
		DelayMS(ui->td_le->text().toInt());
		check = WriteFile(serialHandle, (LPVOID)prevsent, toBeWritten, written, NULL);
	//status = SI_Write(*hDev1, (LPVOID)dataSent, toBeWritten, written);
		if(!check)
		{
			printf("Surprise Removal on Device 1 Write!");
		}
	break;

	case 'C':
		ui->camwn_label->setText("Checking terminate");
		datasend[0] = 0x00;
		check = WriteFile(serialHandle,(LPVOID)datasend,toBeWritten,written,NULL);
		break;
	}
	//first_press++;
	//if(first_press)
	DelayMS(100);
	CloseHandle(serialHandle);
	emit manual_finished();
	//WriteFile(serialHandle,(LPVOID){0x00,0x00,0x00,0x00,0x00,0x00,0x00},toBeWritten,written, NULL);
}*/

void MainWindow::on_coilcontrol_automatic_pushed(){
	for(int i = 0; i < 7; i++)
	{datasend[i] = 0x00; prevsent[i] = 0x00;}
	bool success = false, check;
	//serialHandle = NULL;
	//ui->camwn_label->setText(coil);
	//if(fauto_press)
		//CloseHandle(serialHandle);
	CString comport = "\\\\.\\COM4";
	serialHandle = CreateFile(comport, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if(serialHandle == INVALID_HANDLE_VALUE)
		printf("Error: Invalid Handle Value for Device 1."); 
	// Do some basic settings
	DCB serialParams = { 0 };
	serialParams.DCBlength = sizeof(serialParams);

	GetCommState(serialHandle, &serialParams);
	serialParams.BaudRate = 9600;
	serialParams.ByteSize = 8;
	serialParams.StopBits = ONESTOPBIT;
	serialParams.Parity = 0;
	GetCommState(serialHandle, &serialParams);

	// Set timeouts
	COMMTIMEOUTS timeout = { 0 };
	timeout.ReadIntervalTimeout = 50;
	timeout.ReadTotalTimeoutConstant = 50;
	timeout.ReadTotalTimeoutMultiplier = 50;
	timeout.WriteTotalTimeoutConstant = 50;
	timeout.WriteTotalTimeoutMultiplier = 10;
	SetCommTimeouts(serialHandle, &timeout);
	DWORD toBeWritten = 7, toBeRead = 7;
	DWORD* written = (DWORD*)malloc(sizeof(DWORD));
	DWORD* read = (DWORD*)malloc(sizeof(DWORD)); 
	LPDWORD event = (DWORD *)malloc(sizeof(DWORD));
	stringstream ss; 
	QString autocontrol_coils[] = {"upc_auto%1","downc_auto%1","topc_auto%1","bottomc_auto%1","leftc_auto%1","rightc_auto%1", "freq%1"};
	//ui->camwn_label->setText(ui->scrollArea->findChild<QLineEdit *>(autocontrol_coils[0].arg(2))->text());
	for(int i = 1; i < 3; i++)
	{
		for(int j = 0; j < 7; j++)
			{
				//ui->camwn_label->setText("Check");
				if(j){
					ss.clear();
					stringstream ss;}
				if(j != 4)
					ss << ui-> scrollArea ->findChild<QLineEdit *>(autocontrol_coils[j].arg(i))->text().toStdString();
				else{
					ss << ui->scrollArea->findChild<QLineEdit *>(QString("leftc_auto")+QString::number(i))->text().toStdString();}
				//ui->camwn_label->setText(ui->scrollArea->findChild<QLineEdit *>(autocontrol_coils[j].arg(1))->objectName());
				ss >> std::hex >> value; datasend[j] = value;}
		SetCommMask(serialHandle,EV_RXCHAR);
		WriteFile(serialHandle,(LPVOID)datasend, toBeWritten, written, NULL);
	/*	if(WaitCommEvent(serialHandle, event, NULL))
			ReadFile(serialHandle,(LPVOID)datasend,toBeRead,read,NULL);
		ui->camwn_label->setText(QString::number(datasend[1]));*/
		DelayMS(2000);
		//WriteFile(serialHandle,(LPVOID)prevsent,toBeWritten,written,NULL);
		//ui->camwn_label->setText(ui->scrollArea->findChild<QLineEdit *>(autocontrol_coils[1].arg(1))->text());
		//ui->camwn_label->setText(ui->scrollArea->findChild<QLineEdit *>(autocontrol_coils[1].arg(1))->objectName());
	}
	WriteFile(serialHandle,(LPVOID)prevsent,toBeWritten,written,NULL);
	fauto_press++;
	//if(fauto_press)
	DelayMS(3000);
	CloseHandle(serialHandle);
	return;
}

void MainWindow::on_learnwaypts_pb_pushed(){
	if(ui->setwaypts_cb->isChecked()){
		ui->autowaypts_pb->setEnabled(true);
		ui->nextwaypt_pb->setEnabled(true);}
	else{
		ui->nextwaypt_pb->setEnabled(false);
		ui->autowaypts_pb->setEnabled(false);}
	return;
}

/*void MainWindow::on_autowayptspath_pb_pushed(){
	if(ui->track_cb->isChecked())
	{
		if(clicks == 4)
		{
			int tolerance = 10;
			BYTE * data_command;
			BYTE data_comm[7];
			for(int l = 0; l < 4; l++){
				while(sqrt(pow(std::abs(COM.x - waypoints[l].x),2) + pow(std::abs(COM.y - waypoints[l].y),2)) > tolerance)
					{
						//data_command = determine_command(COM,waypoints,l);
						send_serialportcommand(determine_command(COM,waypoints,l));
				}		
			}}}
	}

BYTE *  MainWindow::determine_command(Point COM, Point2f * waypts, int waypt_no){
	BYTE * command = (BYTE *)malloc(sizeof(BYTE) * 4);
	if(COM.x - waypts[waypt_no].x > 5)
		command[5] = 0x22;
	else if(waypts[waypt_no].x - COM.x > 5)
		command[6] = 0x22;
	if(COM.y - waypts[waypt_no].y > 5)
		command[1] = 0x22;
	else if(waypts[waypt_no].y - COM.y > 5)
		command[0] = 0x22;
	command[2]=command[3]=command[6] = 0x00;
	return command;
}*/


void MainWindow::on_closeapplication_pushed()
{
	QCoreApplication::instance()->exit();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QImage * MainWindow::Mat2QImage(Mat const& src, QImage * dest){
	 temp; // make the same cv::Mat
     cvtColor(src, temp,CV_BGR2RGB); // cvtColor Makes a copt, that what i need
	 if(dest != NULL)
		 delete dest;
	dest = new QImage((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
	(*dest).bits(); // enforce deep copy, see documentation 
    return dest;
	// of QImage::QImage ( const uchar * data, int width, int height, Format format )
}

Mat MainWindow::QImage2Mat(QImage const& src)
{
     cv::Mat tmp(src.height(),src.width(),CV_8UC3,(uchar*)src.bits(),src.bytesPerLine());
     cv::Mat result; // deep copy just in case (my lack of knowledge with open cv)
     cvtColor(tmp, result,CV_BGR2RGB);
     return result;
}

IplImage * MainWindow::ImageToIplImage(QPixmap * qPix){

    int width = (qPix->toImage()).width();
    int height =(qPix->toImage()).height();

    // Creates a iplImage with 3 channels

    IplImage *img = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);

    for(int y=0;y<height;y++)
    {
        for(int x=0;x<width;x++ )
        {
            QRgb color = qPix->toImage().pixel(x,y);
            cvSet2D(img,y,x,cvScalar(qBlue(color),qGreen(color),qRed(color),1));
        }
    }
    return img; }