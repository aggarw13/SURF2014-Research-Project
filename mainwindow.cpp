#include "mainwindow.h"
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
    ui(new Ui::MainWindow), QObject(parent)
{
    ui->setupUi(this);
	selectObject = 0;
	first_press = 0, loopcount = 0, fauto_press = 0,clicks = 0;
	mouseevent_valid = false;
	scene  = new QGraphicsScene(this);
	scene->setSceneRect(0,0,ui->Video_wn->width(),ui->Video_wn->height());
	ui->Video_wn->setScene(scene);
	Qim = NULL;
	m_threadDoneEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	//ui->scrollArea->addScrollBarWidget(ui->scrollArea,Qt::Alignment
	//ui->Video_wn->setMouseTracking(true);
	QObject::connect(ui->capture_pb,SIGNAL(clicked()),this,SLOT(on_Capturepb_pushed()));
	QObject::connect(ui->stopccapture_pb,SIGNAL(clicked()),this,SLOT(on_Stopcapturepb_pushed()));
	QStringList streamlist = QStringList() <<QString("Camera") << QString("MinOil.wmv") << QString("Video_demo.wmv") << QString("Tumbling_video.wmv");
	QObject::connect(ui->track_cb,SIGNAL(clicked()), this, SLOT(on_trackbt_pushed()));
	QObject::connect(ui->template_pb,SIGNAL(clicked()),this,SLOT(learnTemplate()));
	QObject::connect(ui->autogo_pb,SIGNAL(clicked()),this,SLOT(on_coilcontrol_automatic_pushed()));
	QObject::connect(ui->closewn_pb,SIGNAL(clicked()),this,SLOT(on_closeapplication_pushed()));
	QObject::connect(ui->setwaypts_cb,SIGNAL(clicked()),this,SLOT(on_learnwaypts_pb_pushed()));
	QObject::connect(ui->autowaypts_pb,SIGNAL(clicked()),this,SLOT(on_autowayptspath_pb_pushed()));
	QSignalMapper * mapper = new QSignalMapper(this);
	mapper->setMapping(ui->leftc_pb,QString("L"));
	mapper->setMapping(ui->rightc_pb,QString("R"));
	mapper->setMapping(ui->bottomc_pb,QString("B"));
	mapper->setMapping(ui->topc_pb,QString("T"));
	mapper->setMapping(ui->upc_pb,QString("U"));
	mapper->setMapping(ui->downc_pb, QString("D"));
	mapper->setMapping(ui->termcurrent_pb,QString("C"));
	QObject::connect(ui->leftc_pb, SIGNAL(clicked()),mapper, SLOT(map()));
	QObject::connect(ui->topc_pb,SIGNAL(clikced()),mapper,SLOT(map()));
	QObject::connect(ui->rightc_pb,SIGNAL(clicked()),mapper,SLOT(map()));
	QObject::connect(ui->bottomc_pb,SIGNAL(clicked()),mapper,SLOT(map()));
	QObject::connect(ui->upc_pb,SIGNAL(clicked()),mapper,SLOT(map()));
	QObject::connect(ui->downc_pb,SIGNAL(clicked()),mapper,SLOT(map()));
	QObject::connect(ui->termcurrent_pb,SIGNAL(clicked()),mapper,SLOT(map()));
	QObject::connect(mapper,SIGNAL(mapped(QString)),this,SLOT(on_coilcontrol_manual_pushed(QString)));
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
	streamlist = QStringList() << QString("100") << QString("300") << QString("500");
	ui->featthresh_dd->addItems(streamlist); streamlist.clear();
	streamlist = QStringList() << QString("600") << QString("Default");
	ui->extractthresh_dd->addItems(streamlist);
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
void MainWindow::on_Capturepb_pushed(){
    IplImage * src_img;
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
	while (waitKey(30) != 27 && loopcount <= 150) {
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
		cvWriteFrame(output_video,src_img);
		cv::resize(cv::Mat(src_img),temp,Size(ui->Video_wn->width(),ui->Video_wn->height()));
		//output_video.write(cv::Mat(src_img));
		temp.copyTo(image);
		if(clicks && clicks <= 4 && ui->setwaypts_cb->isChecked()){
			cvInitFont(&font,CV_FONT_HERSHEY_PLAIN,1.0,1.0);
			for(int k = 0; k < clicks; k++)
			{
				cvDrawCircle(&IplImage(image),cvPoint(waypoints[k].x,waypoints[k].y),3,Scalar::all(255),1);
				string point = string("Waypoint") + std::to_string(long double(k + 1));
				cvPutText(&IplImage(image),point.c_str(),Point(waypoints[k].x + 3, waypoints[k].y + 3),&font,Scalar::all(255));
			}
		}
		if(selection.x)
			rectangle(image,selection,Scalar(255,0,0),1);
		}
		if(ui->stream_filemenu->currentIndex() > 1){
			cv::resize(cv::Mat(image),temp,Size(ui->Video_wn->width(),ui->Video_wn->height()));
			temp.copyTo(image);}
		//QImage Qim((const uchar *)image.data,image.cols, image.rows, image.step, QImage::Format_RGB32);
		if(ui->track_cb->isChecked())
		{cvCopy(&IplImage(image),im); }//goto track_robot;}
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
		if(loopcount == 150)
			ui->camwn_label->setText("5 seconds");
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
    cvWaitKey(0);
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
	double angle;
	if(points[0].x < points[3].x)
		angle = atan(std::abs(points[3].y - points[0].y) / std::abs(points[0].x - points[3].x)) * (180 * 7 / 22);
	else
		angle = 180.00 - atan((points[0].y - points[3].y) / (points[0].x - points[3].x)) * (180 * 7 / 22);
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
	if(ev->button() == Qt::LeftButton){
		origin = Point(ev->x() - ui->Video_wn->x() - 34 ,ev->y() - ui->Video_wn->y() - 14);
        selection = Rect(ev->x() - ui->Video_wn->x() - 34 ,ev->y() - ui->Video_wn->y() - 14,0,0);
		ui->camwn_label->setText(QString::number(ev->globalX() - ui->Video_wn->x() - 34)+QString(",")+QString::number(ev->globalY() - ui->Video_wn->y() - 34));
		//cvDrawCircle(&IplImage(image),Point(ui->Video_wn->x() - 34 ,ui->Video_wn->y() - 14),3,Scalar::all(255),1);
		Qim = Mat2QImage(image, Qim);
		scene->addPixmap(QPixmap::fromImage(*Qim));
		image.copyTo(temp);
		selectObject = true;}
}
}
void MainWindow::mouseMoveEvent(QMouseEvent * ev){
	if(!mouseevent_valid)
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
	if(image.data){
	cv::resize(cv::Mat(image),temp,Size(ui->Video_wn->width(),ui->Video_wn->height()));
	temp.copyTo(image);}
	if(ui->setwaypts_cb->isChecked())
	{   
		if(clicks == 0)
			image.copyTo(waypts_sans);
		clicks++; string point; 
		if(clicks <= 4){
			waypoints[clicks - 1] = Point(ev->x() - ui->Video_wn->x() - 34 ,ev->y() - ui->Video_wn->y() - 14);
			//for(int k = 0; k < clicks; k++)
			//{
				//ui->camwn_label->setText(QString::number(clicks));
				cvDrawCircle(&IplImage(image),cvPoint(waypoints[clicks - 1].x,waypoints[clicks - 1].y),3,Scalar::all(255),1);
				point = string("Waypoint") + std::to_string(long double(clicks));
				cvPutText(&IplImage(image),point.c_str(),Point(waypoints[clicks - 1].x + 3, waypoints[clicks - 1].y + 3),&font,Scalar::all(255));
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
	clicks = 0;
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
	cv::BackgroundSubtractorMOG2 bgsub;
	object = cvLoadImage("Template_video3.jpg",CV_LOAD_IMAGE_GRAYSCALE);
track_robot:
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
	if(cap == NULL)
		return ;
	else{im = cvQueryFrame(cap);
	imshow("Test",cv::Mat(im));}
	int Key = 0;
	//while(Key != 27 && !object.data && im != NULL){
	//	cvShowImage("Test",im);		im = cvQueryFrame(cap);
	//	Key = cvWaitKey(30);}
	//if(im == NULL)
	//	return;
	double robot_area = 0.0;
	Point2f COM, COMprev;Moments mt;
	Rect PrevRect;
   // detector -> detect( object, kp_object );
	 std::vector<KeyPoint> kp_image;
     std::vector<vector<DMatch > > matches;
     std::vector<DMatch > good_matches;
     std::vector<Point2f> obj;
	 std::vector<Point2f> scene;
     std::vector<Point2f> scene_corners(4);
    //Calculate descriptors (feature vectors)
     Mat des_image, img_matches;

   // extractor.compute( object, kp_object, des_object );
 

	//cv::VideoCapture cap;
	//cap.open("Video_demo.wmv");
    //namedWindow("Good Matches");
	 Mat H;
    std::vector<Point2f> obj_corners(4);

    //Get the corners from the object
    obj_corners[0] = cvPoint(0,0);
    obj_corners[1] = cvPoint( object.cols, 0 );
    obj_corners[2] = cvPoint( object.cols, object.rows );
    obj_corners[3] = cvPoint( 0, object.rows );
	Mat temp,background,mask;
	//cv::Mat mask(object.rows,object.cols,object.depth());
	cv::Rect rectLine;
	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierarchy;
	Mat prevframe;
	double maxC_area;
	int contour, cam_mot = 0,contor; 
    char key = 'a';
	Mat frame;
	int mfeature = 0, thresh_type = 0, thresh = 60;
	IplImage * frameprev, * frame1, *frame_small, *object_small = cvCreateImage(cvSize((int)(object.cols / 5),(int)(object.rows / 5)),IPL_DEPTH_8U,object.channels());
    int framecount = 0; int corner_count, or_size;
	std::vector<Point> approx;
	Rect boundrect; RotatedRect rott_rect;Point2f rotrect[4];
	time_t start,end;Mat boundcn;
	string pos,position[] = {"Position is (",",",")"},angle_t[] = {"Angle",":","degrees"},angle_s;
	CvFont font;
	CvPoint org;
	//QGraphicsScene * gscene  = new QGraphicsScene(this);
	//gscene->setSceneRect(ui->Video_wn->x(),ui->Video_wn->y(),ui->Video_wn->width(),ui->Video_wn->height());
	//ui->Video_wn->setScene(gscene);
	 while (key != 27)
	 {
		//Mat Lpl_obj;
		//Laplacian(object, object, CV_16S, 3, 1, 0, BORDER_DEFAULT );
		//convertScaleAbs(object, object);
		//cap >> frame;
		corner_count = 500;
		if (framecount < 10)
        {
            framecount++;
			if(framecount == 4){
				//cap >> cv::Mat(im);
				im = cvQueryFrame(cap);}
				//im = &IplImage(frame);}
				//cornerSubPix(cv::Mat(im),
            continue;
        }
		if(loopcount == 0){
			start = time(NULL);
			frameprev = cvCreateImage(cvGetSize(im),im->depth,im->nChannels);
			if(loopcount == 0)
				cvConvertImage(im,frameprev);
				//cvCopy(im,frameprev);}
			frame1 = cvCreateImage(cvGetSize(im),IPL_DEPTH_8U,1);}
		im = cvQueryFrame(cap);
		if(im == NULL)
			break;
		cvCvtColor(im,frame1,CV_BGR2GRAY);
		//im = &IplImage(frame);
		frame = cv::Mat(frame1);
		GaussianBlur( frame, frame, Size(3,3), 0, 0, BORDER_DEFAULT );
		IplImage * image = cvCreateImage(cvGetSize(im), im->depth, 1);
		if(loopcount >= 0){
			//Laplacian(frame,frame,frame.depth(),3);
			//convertScaleAbs(frame,fmask);
			//Canny(frame,frame,100,255);
			bgsub(frame,fgmask);
			goto next;
		}
		if(loopcount == -1 || mfeature == -1){
		feature_track:
		//cvResize(&IplImage(object),object_small,1);
		imshow("Background",object);
		detector -> detect( object, kp_object );
		extractor->compute( object, kp_object, des_object );
		obj_corners[0] = cvPoint(0,0);
		obj_corners[1] = cvPoint( object.cols, 0 );
		obj_corners[2] = cvPoint( object.cols, object.rows );
		obj_corners[3] = cvPoint( 0, object.rows );
		printf("Keypoints of the object: %d\n",kp_object.size());
		//cv::Mat mask_roi = Mat::zeros(im->height,im->width,CV_8U);
		printf("Index check : %f %f",kp_object[0].pt.x,kp_object[1].pt.y);

		//Laplacian(segm,segm,CV_8U,1,2);
		//convertScaleAbs(segm, fgmask);
		//Canny(fgmask,fgmask,100,255,3);
		printf("Frame Image type : %d",im-> depth);
		cvCvtColor(im, image, CV_BGR2GRAY);
		//cvtColor(cv::Mat(im),frame,CV_RGB2GRAY);
		Mat Lpl;
		//cvResize(&(IplImage)frame,image);
		Lpl = cv::Mat(image);	
		//Laplacian( frame, frame, CV_16S, 3, 1, 0, BORDER_DEFAULT );
		//convertScaleAbs( frame, Lpl );
		//image = &(IplImage)Lpl;
		//image = &IplImage(frame);
		if(loopcount > 0){
			//cvSetImageROI(image,rectLine);
			//cv::Mat roi(mask_roi, rectLine);
			//roi = cv::Scalar(255,255,255);
			//roi.copyTo(mask_roi);
			//detector->detect( cv::Mat(image), kp_image, mask_roi);
			//cvNamedWindow("Masking");
			//imshow("Masking",roi);
		}
		//else
		if(loopcount > 0)
		{matches.clear(); good_matches.clear();}
		detector->detect( image, kp_image);
		printf("Frame copied Image type : %d",image-> depth);
		extractor->compute( image, kp_image, des_image );
		matcher.knnMatch(des_object, des_image, matches, 2);
		printf("No. of matches : %d\n", matches.size());
       for(int i = 0; i < min(des_image.rows-1,(int) matches.size()); i++) //THIS LOOP IS SENSITIVE TO SEGFAULTS
        {
            if((matches[i][0].distance < 0.6*(matches[i][1].distance)) && ((int) matches[i].size()<=2 && (int) matches[i].size()>0))
            {
                good_matches.push_back(matches[i][0]);
            }
        }
			printf("No. of good matches : %d\n", good_matches.size());
        //Draw only "good" matches
		//drawMatches( object, kp_object, image, kp_image, good_matches, img_matches, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
			ui->camwn_label->setText(QString::number(good_matches.size()));
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
		else 
			object = Lpl(PrevRect);
		rectLine = boundingRect(cv::Mat(scene_corners));
		IplImage * filterHSV = cvCreateImage(cvGetSize(im),IPL_DEPTH_8U,3);
		IplImage * filtergray = cvCreateImage(cvGetSize(im),8,3);
		img_matches.convertTo(img_matches,CV_8U);
		cvCopy(im,filterHSV);
		//cvConvertScale(im,filterHSV,1.0/256);
		//temp = cv::Mat(filterHSV);
		//Laplacian( cv::Mat(image), frame, CV_8U, 3, 1, 0, BORDER_DEFAULT );
		cv::Mat update_tmp; 
		//Canny(cv::Mat(image),temp,100,255);
		//convertScaleAbs( frame, temp );
		printf("\nTemporary image type : %d\n",temp.channels());
		//cvInRangeS(im,cv::Scalar(60,100,100),cv::Scalar(60,255,255),im);
		GaussianBlur(temp,temp,Size(3,3),0,0,BORDER_DEFAULT);
		cv::threshold(cv::Mat(image),temp,90,255,CV_THRESH_BINARY_INV);
		//cv::erode(temp,temp,cv::Mat());
		//cv::dilate(temp,temp,cv::Mat());
		morphologyEx(temp,temp,cv::MORPH_CLOSE,cv::Mat());
		//imshow("Good Matches",cv::Mat(im));loopcount++;
		//continue;
		update_tmp = temp(rectLine);
		Canny(update_tmp,update_tmp,170,255);
		//cvCvtColor(filterHSV,filterHSV,CV_HSV2RGB);
		//cvCvtColor(filterHSV,filtergray,CV_RGB2GRAY);
		printf("\nGray channels :%d",filtergray -> nChannels);
		if(loopcount > 0){
			boundcn = cv::Mat::zeros(frame.size(),CV_8U);
		rectangle(boundcn,rectLine,Scalar::all(255),1);}	
		findContours(update_tmp, contours, hierarchy, CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);		
		maxC_area = 0;
		contour = 0;
		printf("\nNumber of contours : %d",contours.size());
		for (int i = 0; i < contours.size(); i++)
		{
			Moments mm = cv::moments(contours[i], false);
			//if(loopcount == 0) {
			if(mm.m00 > maxC_area){
				maxC_area  = mm.m00;contour = i;}//}
		}
		//std::vector<std::vector<CvPoint>> Contour;
		//std::vector<CvPoint> con;
		//Contour.push_back(contours[contour]);
		//drawContours(update_tmp,contours,contour,Scalar(255),2);
		printf("\nContour no. : %d",contour);
		//cvShowImage("Good Matches",im);
		//cvSetImageROI(image,contours[contour]);
		//cv::Rect rect(boundingRect(contours[contour]));
		//cvResetImageROI(image);
		//mask(object.rows,object.cols,object.depth());
		//boundingRect(cv::Mat(contours[contour]))
		robot_area = contourArea(contours[contour]);
		printf("\nARea of robot : %f\n",robot_area);
		namedWindow("Check",WINDOW_AUTOSIZE);
		background = cv::Mat(im);
		mask = Mat::zeros(im->height,im->width,im->depth);
		//mask = frame(rectLine);
		//cvSetImageROI(&IplImage(mask),rectLine);
		cv::Mat(im).copyTo(background);
		//cvCopy(im,&IplImage(background));
		rectangle(mask,rectLine,Scalar::all(255),1);
		//frame.copyTo(background,mask);
		boundcn = Mat::zeros(frame.size(),CV_8U);
		drawContours(boundcn,contours,contour,Scalar::all(255),1);	
		//imshow("Check",boundcn);
		PrevRect = rectLine;
		boundrect = rectLine;
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
		rectangle(cv::Mat(im),rectLine,Scalar::all(255),2);
		goto printpos;
}
	if(loopcount == 0)
		goto next;
		//bgsub.getBackgroundImage(segm);
		//imshow("Background",segm);
		//PrevRect = boundingRect(cv::Mat(scene_corners));
		prevframe = image;
		//cv::erode(Lpl,Lpl,cv::Mat());
		//cv::dilate(Lpl,Lpl,cv::Mat());
		if(loopcount > 0){
			frame = cv::Mat(frame1);
			GaussianBlur(fgmask,fgmask,Size(3,3),0,0);
			/*if(loopcount > 1){
				if(thresh_type == 0){
					boundrect.x = boundrect.x - 15;
					boundrect.y = boundrect.y - 15;}
				else{
					boundrect.x = boundrect.x - 20;
					boundrect.y = boundrect.y - 20;}
				boundrect.height = boundrect.height + 40;
				boundrect.width = boundrect.width + 40;}
			COMprev = COM;
			PrevRect  = boundrect;*/
			cv::Mat(im).copyTo(segm);
		//segm = cv::Mat(frameprev);
			subtract(segm, cv::Mat(frameprev),segm);

		//Laplacian(segm,segm,CV_16S,3,1,0);
		//buff = cv::Mat(frame1);
		//convertScaleAbs(segm,temp);
			cvtColor(segm,buff,CV_RGB2GRAY);
			GaussianBlur(buff,buff,Size(3,3),0);
			morphologyEx(buff,buff,MORPH_CLOSE,cv::Mat());
			cv::threshold(buff,buff,20,255,CV_THRESH_BINARY);
			//cvSetImageROI(&IplImage(buff),boundrect);
			//buff = buff(boundrect);
			findContours(buff, contours, hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
			or_size = contours.size();
			printf("Subtracted image type : %d\n",contours.size());
			//printf("\nArea of boundrect : %d\n",boundrect.area());}
			/*if(contours.size() >= 50){
			goto track;
			rectangle(cv::Mat(im),boundrect,Scalar::all(255),2);
			//im = cvQueryFrame(cap);
			goto printpos;
			bgsub(frame,fgmask);
			}*/
		//else{
			//boundrect = Rect(boundrect.x ,boundrect.y - 50, boundrect.width + 20,boundrect.height + 50);
			//printf("COM : (%f,%f)\n",(COM.x),COM.y);
			//boundrect -= Point(0,60);
			cv::threshold(fgmask,fgmask,50,255,CV_THRESH_BINARY);
			GaussianBlur(fgmask,fgmask,Size(3,3),0,0); 
			//adaptiveThreshold(fgmask,fgmask,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C,CV_THRESH_BINARY,11,2);
			medianBlur(fgmask,fgmask,3);
			//fgmask.copyTo(fmask);
			//}	
			Laplacian(fgmask,fgmask,fmask.depth(),3);convertScaleAbs(fgmask,fmask);	
			morphologyEx(fgmask,fmask,MORPH_CLOSE, cv::Mat());
			Canny(fmask,fmask,50,255);
			goto next;
		//cv::erode(fmask,fmask,cv::Mat());
		//cv::dilate(fmask,fmask, cv::Mat())
			//imshow("Check",fmask);
			//goto printpos;
			//if(mfeature == 1){
				//imshow("Good Matches",fmask);
			IplImage * fmas = &IplImage(fmask);
				cvSetImageROI(fmas,boundrect);
		boundcn = Mat::zeros(frame.size(),CV_8U);
		findContours(cv::Mat(fmas), contours, hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		maxC_area = 0;
		contour = 0;
		for(int i = 0; i < contours.size(); i++)
		{
			mt = cv::moments(contours[i],false);
			if(maxC_area < mt.m00){
				maxC_area = mt.m00;contour = i;}
		}
		//fmask.copyTo(boundcn);
		printf("Background sub area : %f\n",maxC_area);
		//approxPolyDP(contours[contour],approx,3,true);
		drawContours(fmask,contours,contour,cv::Scalar(255),1);
		//if(maxC_area == 3772.5)
		//	imshow("Check",fmask);
		if(maxC_area < robot_area / 1.5 || maxC_area > robot_area * 1.5){
			track:
			boundcn = Mat::zeros(frame.size(),CV_8U);
			int check = 0;
			recalc:
			frame = cv::Mat(frame1);
			frame.copyTo(temp);	
			medianBlur(temp,temp,3);
			GaussianBlur(temp,temp,Size(3,3),0,0,BORDER_DEFAULT);
			//if(maxC_area == 99.5){adaptiveThreshold(temp,temp,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C ,CV_THRESH_BINARY_INV,11,2);
			//Canny(temp,temp,200,255);}
			//else
			if(or_size == 120){
				thresh = 40; thresh_type = 1;}
			cv::threshold(temp,temp,thresh,255,thresh_type);
			//if(or_size == 120)
				//imshow("Check",temp);
			//adaptiveThreshold(temp,temp,255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,11,2);
		//cv::erode(temp,temp,cv::Mat());
		//cv::dilate(temp,temp,cv::Mat());
			//std::vector<vector<Point>> contors;
			morphologyEx(temp,temp,cv::MORPH_CLOSE,cv::Mat());
			//imshow("Check",temp);
			//buff = cv::Mat::zeros(buff.size(),buff.depth());
			buff = temp(boundrect);
			temp.copyTo(boundcn);
			//rectangle(boundcn,boundrect,Scalar::all(255),1);
			//imshow("Check",boundcn);
			//if(or_size == 88)
			//{rectangle(temp,boundrect,Scalar::all(255),1);}
			//cv::threshold(temp,temp,200,255,CV_THRESH_BINARY);
			//IplImage * bound = &IplImage(temp);
			//cvSetImageROI(bound,boundrect);
			//buff.copyTo(cv::Mat(bound));
			//Canny(buff,buff,100,255);		
			//temp.copyTo(boundcn);
			contours.clear();
			findContours(buff, contours, hierarchy, CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
			double maxCarea = 0;
			contor = 0; 
			for (int i = 0; i < contours.size(); i++)
			{
				mt = cv::moments(contours[i], false);
			if(mt.m00 > maxCarea){
				maxCarea  = mt.m00;contor = i;}
			}
			mt = cv::moments(contours[contor]);
			if((mt.m00 < robot_area / 1.5 || robot_area * 1.5 < mt.m00) && check < 1){
				printf("Re-cal check : %d\n", check);
				if(mt.m00 < robot_area / 1.5){
				boundrect.x = boundrect.x - 20;
				boundrect.y = boundrect.y - 20;
				boundrect.width = boundrect.width + 40;
				boundrect.height = boundrect.height + 20;}
				if(mt.m00 > robot_area * 1.5){
					//boundrect.x = boundrect.x + 5;
					//boundrect.y = boundrect.y + 5;
					boundrect.width = boundrect.width - 40;
					boundrect.height = boundrect.height - 20;}
				check++;  PrevRect = boundrect; goto recalc;}check = 0;
			drawContours(boundcn,contours,contor,Scalar::all(255),1);
			boundrect = boundingRect(cv::Mat(contours[contor]));
			rectangle(boundcn,boundrect,Scalar::all(255),1);
			//if(maxC_area == 631.5)
			//	imshow("Check",boundcn);
			rott_rect = minAreaRect(contours[contor]); 
			printf("Contour size in alternative : %f\n",cv::moments(contours[contor]).m00);}
		else{
		boundrect = boundingRect(cv::Mat(contours[contour]));
		mt = cv::moments(contours[contor]);
		drawContours(boundcn,contours,contour,Scalar::all(255),1);
		rott_rect = minAreaRect(contours[contour]);}
		rott_rect.points(rotrect);
		/*for(int i = 0; i < 3 ; i++)
			line(boundcn,rotrect[i],rotrect[i+1],Scalar::all(255),1);
		line(boundcn,rotrect[3],rotrect[0],Scalar::all(255),1); */
		cvDrawCircle(&IplImage(boundcn),rotrect[3],5,Scalar::all(255),1);
		//imshow("Check",boundcn);
		printf("Angle: %f\n",calculate_angle(rotrect,rott_rect));
		COM.x = mt.m10/mt.m00; COM.y = mt.m01/mt.m00;
		COM.x += PrevRect.x; COM.y += PrevRect.y;
		boundrect.x += PrevRect.x;
		boundrect.y += PrevRect.y;
		//if(maxC_area == 3772.5)
			//thresh_type;
		//printf("Pixel Values : %d\n",int(frame.at<uchar>(COM)));
		//rectangle(temp,boundrect,Scalar::all(255),1); cvDrawCircle(&IplImage(boundcn),Point(boundrect.x,boundrect.y),5,Scalar::all(255),1);
		//if(loopcount == 2){
		//imshow("Check",cv::Mat(fmas));
		cvResetImageROI(fmas);
		//rectangle(cv::Mat(im),boundrect,Scalar::all(255),2);
		printf("Check: %d\n",im -> width);
		frame = cv::Mat(frame1);
		temp = frame(boundrect);
		//cvtColor(temp,temp,CV_RGB2GRAY);
		obj_or.copyTo(object);
		//cvResetImageROI(&IplImage(fmask));
		}

		printpos:
		pos = position[0] + std::to_string(long double(COM.x)) + position[1] + std::to_string(long double(COM.y))+position[2];
		font;
		ui->angle->setText(QString::number(calculate_angle(rotrect,rott_rect)));
		ui->pos_COMx->setText(QString::number(COM.x));
		ui->pos_COMy->setText(QString::number(COM.y));
		angle_s = angle_t[0]+angle_t[1]+std::to_string(long double(calculate_angle(rotrect,rott_rect)))+angle_t[2];
		cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX,1.0,1.0,0);
		org;org.x = COM.x; org.y = COM.y;
		cvPutText(im,angle_s.c_str(),cvPoint(208,40),&font,Scalar(0,255,255));
		cvPutText(im,pos.c_str(),org,&font, cv::Scalar(255,255,255));

		next:
		//String info = pos.substr(12);
		//info +=  std::string(int(filename.find("tation") - info.find(")")),' ') + std::to_string(long double(calculate_angle(rotrect,rott_rect)));
		//info += std::string(int(filename.find("me No.")) - info.size() ,' ') + std::to_string(long double(loopcount + 1)) + "\n";
		//fwrite(info.c_str(),sizeof(char), info.size(),fp); 
		//QImage Qim = Mat2QImage(im);
		//ui->Video_wn->resize(QSize(gscene->width(),gscene->height()));
		//QGraphicsPixmapItem *item = gscene->addPixmap((QPixmap::fromImage(Qim)).scaled(QSize((int)gscene->width(), (int)gscene->height()),Qt::KeepAspectRatio, Qt::SmoothTransformation));
		cv::resize(cv::Mat(im),temp,Size(ui->Video_wn->width(),ui->Video_wn->height()));
		Qim = Mat2QImage(cv::Mat(im), Qim);
		this->scene->addPixmap(QPixmap::fromImage(*Qim));
		imshow("Test",fgmask);
		ui->Video_wn->fitInView(QRectF(0, 0,ui->Video_wn->width(), ui->Video_wn->height()),Qt::KeepAspectRatio);
		cvCopy(im,frameprev);
		key = waitKey(30);
		/*if(mfeature == 1){
			//PrevRect.x = PrevRect.x + 10;
			//PrevRect.y = PrevRect.y + 5;
			//PrevRect.width = PrevRect.width - 10;
			//PrevRect.height = PrevRect.height - 10;
			PrevRect += Point(20,20);
			PrevRect -= Size(40,40);
			boundrect = PrevRect;}
		else if(mfeature == 2){
			PrevRect += Point(40,40);
			PrevRect -= Size(50,50);
			boundrect = PrevRect;}
		else
			PrevRect = boundrect;*/
		mfeature = 0;
		loopcount++;
		if(loopcount == 100){
			end = time(NULL);
			printf("Frame processing rate : %f",difftime(end,start));
		}
}
	fclose(fp);
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
	if(ui->stream_filemenu->currentIndex() == 1){
		error = cam.StopCapture();
		if(error != PGRERROR_OK)
			error.PrintErrorTrace();
		Sleep(25);
		SetEvent(m_threadDoneEvent);
		WaitForSingleObject(m_threadDoneEvent,500);
		error = cam.Disconnect();
		if(error != PGRERROR_OK)
			error.PrintErrorTrace();
	}
	QGraphicsScene * scene  = new QGraphicsScene(this);
	ui->Video_wn->setScene(scene);
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
	serialParams.StopBits = 1;
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


void MainWindow::on_coilcontrol_manual_pushed(QString coil)
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
	serialParams.StopBits = 1;
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
		BYTE val;
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
	//WriteFile(serialHandle,(LPVOID){0x00,0x00,0x00,0x00,0x00,0x00,0x00},toBeWritten,written, NULL);
}

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
	serialParams.StopBits = 1;
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
	if(ui->setwaypts_cb->isChecked())
		ui->autowaypts_pb->setEnabled(true);
	else
		ui->autowaypts_pb->setEnabled(false);
	return;
}

void MainWindow::on_autowayptspath_pb_pushed(){
	if(ui->track_cb->isChecked())
	{
		COM.x = ui->pos_COMx->text().toInt();
		COM.y = ui->pos_COMy->text().toInt();
		if(clicks == 4)
		{
			int tolerance = 10;
			BYTE * data_command;
			BYTE data_comm[7];
			for(int l = 0; l < 4; l++){
				while(sqrt(pow(std::abs(COM.x - waypoints[l].x),2.0) + pow(std::abs(COM.y - waypoints[l].y),2.0)) > tolerance)
					{
						//data_command = determine_command(COM,waypoints,l);
						send_serialportcommand(determine_command(COM,waypoints,l));
				}		
			}}}
	}

BYTE *  MainWindow::determine_command(Point COM, Point * waypts, int waypt_no){
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
}


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