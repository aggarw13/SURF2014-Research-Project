#include <stdio.h>

// OpenCV stuff
#include "stdafx.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp> // for homography

/*void showUsage()
{
        printf("\n");
        printf("Usage :\n");
        printf("  ./example object.png scene.png\n");
        exit(1);
}

int main(int argc, char * argv[])
{
		cv::initModule_nonfree(); 
        //Load as grayscale
		cv::namedWindow("Template");
		cv::namedWindow("Search_frame");
		
		cv::Mat objectImg = cvLoadImage("Template_mr.jpg",CV_LOAD_IMAGE_GRAYSCALE);
        cv::Mat sceneImg = cvLoadImage("Snapshot_microrobot1.jpg", CV_LOAD_IMAGE_GRAYSCALE);

        if(!objectImg.empty() || !sceneImg.empty())
        {
                std::vector<cv::KeyPoint> objectKeypoints;
                std::vector<cv::KeyPoint> sceneKeypoints;
                cv::Mat objectDescriptors;
                cv::Mat sceneDescriptors;

                ////////////////////////////
                // EXTRACT KEYPOINTS
                ////////////////////////////
                // The detector can be any of (see OpenCV features2d.hpp):
                // cv::FeatureDetector * detector = new cv::DenseFeatureDetector();
                // cv::FeatureDetector * detector = new cv::FastFeatureDetector();
                // cv::FeatureDetector * detector = new cv::GFTTDetector();
                // cv::FeatureDetector * detector = new cv::MSER();
                // cv::FeatureDetector * detector = new cv::ORB();
                cv::FeatureDetector * detector[2];
				detector[0] = new cv::SIFT();
				detector[1] = new cv::SIFT();
                // cv::FeatureDetector * detector = new cv::StarFeatureDetector();
                // cv::FeatureDetector * detector = new cv::SURF(600.0);
                // cv::FeatureDetector * detector = new cv::BRISK();
                detector[0]->detect(objectImg, objectKeypoints);
               // printf("Object: %d keypoints detected in %d ms\n", (int)objectKeypoints.size(), time.restart());
               // detector[1]->detect(sceneImg, sceneKeypoints);
                //printf("Scene: %d keypoints detected in %d ms\n", (int)sceneKeypoints.size(), time.restart());

                ////////////////////////////
                // EXTRACT DESCRIPTORS
                ////////////////////////////
                // The extractor can be any of (see OpenCV features2d.hpp):
                // cv::DescriptorExtractor * extractor = new cv::BriefDescriptorExtractor();
                // cv::DescriptorExtractor * extractor = new cv::ORB();
				   cv::DescriptorExtractor * extractor = new cv::SIFT();
                // cv::DescriptorExtractor * extractor = new cv::SURF(600.0);
                // cv::DescriptorExtractor * extractor = new cv::BRISK();
                // cv::DescriptorExtractor * extractor = new cv::FREAK();
                extractor->compute(objectImg, objectKeypoints, objectDescriptors);
//                printf("Object: %d descriptors extracted in %d ms\n", objectDescriptors.rows, time.restart());
                //extractor->compute(sceneImg, sceneKeypoints, sceneDescriptors);
             //   printf("Scene: %d descriptors extracted in %d ms\n", sceneDescriptors.rows, time.restart());

                ////////////////////////////
                // NEAREST NEIGHBOR MATCHING USING FLANN LIBRARY (included in OpenCV)
                ////////////////////////////
                cv::Mat results;
                cv::Mat dists;
                std::vector<std::vector<cv::DMatch> > matches;
                int k=2; // find the 2 nearest neighbors
                bool useBFMatcher = false; // SET TO TRUE TO USE BRUTE FORCE MATCHER
                if(objectDescriptors.type()==CV_8U)
                {
                        // Binary descriptors detected (from ORB, Brief, BRISK, FREAK)
                        printf("Binary descriptors detected...\n");
                        if(useBFMatcher)
                        {
                                cv::BFMatcher matcher(cv::NORM_HAMMING); // use cv::NORM_HAMMING2 for ORB descriptor with WTA_K == 3 or 4 (see ORB constructor)
                                matcher.knnMatch(objectDescriptors, sceneDescriptors, matches, k);
                        }
                        else
                        {
                                // Create Flann LSH index
                                cv::flann::Index flannIndex(sceneDescriptors, cv::flann::LshIndexParams(12, 20, 2), cvflann::FLANN_DIST_HAMMING);
//                                printf("Time creating FLANN LSH index = %d ms\n", time.restart());

                                // search (nearest neighbor)
                                flannIndex.knnSearch(objectDescriptors, results, dists, k, cv::flann::SearchParams() );
                        }
                }
                else
                {
                        // assume it is CV_32F
                        printf("Float descriptors detected...\n");
                        if(useBFMatcher)
                        {
                                cv::BFMatcher matcher(cv::NORM_L2);
                                matcher.knnMatch(objectDescriptors, sceneDescriptors, matches, k);
                        }
                        else
                        {
                                // Create Flann KDTree index
                                cv::flann::Index flannIndex(sceneDescriptors, cv::flann::KDTreeIndexParams(), cvflann::FLANN_DIST_EUCLIDEAN);
//                                printf("Time creating FLANN KDTree index = %d ms\n", time.restart());

                                // search (nearest neighbor)
                                flannIndex.knnSearch(objectDescriptors, results, dists, k, cv::flann::SearchParams() );
                        }
                }
//                printf("Time nearest neighbor search = %d ms\n", time.restart());

                // Conversion to CV_32F if needed
                if(dists.type() == CV_32S)
                {
                        cv::Mat temp;
                        dists.convertTo(temp, CV_32F);
                        dists = temp;
                }


                ////////////////////////////
                // PROCESS NEAREST NEIGHBOR RESULTS
                ////////////////////////////
                // Set gui data

                // Find correspondences by NNDR (Nearest Neighbor Distance Ratio)
                float nndrRatio = 0.8;
                std::vector<cv::Point2f> mpts_1, mpts_2; // Used for homography
                std::vector<int> indexes_1, indexes_2; // Used for homography
                std::vector<uchar> outlier_mask;  // Used for homography
                // Check if this descriptor matches with those of the objects
                if(!useBFMatcher)
                {
                        for(int i=0; i<objectDescriptors.rows; ++i)
                        {
                                // Apply NNDR
                                //printf("q=%d dist1=%f dist2=%f\n", i, dists.at<float>(i,0), dists.at<float>(i,1));
                                if(results.at<int>(i,0) >= 0 && results.at<int>(i,1) >= 0 &&
                                   dists.at<float>(i,0) <= nndrRatio * dists.at<float>(i,1))
                                {
                                        mpts_1.push_back(objectKeypoints.at(i).pt);
                                        indexes_1.push_back(i);

                                        mpts_2.push_back(sceneKeypoints.at(results.at<int>(i,0)).pt);
                                        indexes_2.push_back(results.at<int>(i,0));
                                }
                        }
                }
                else
                {
                        for(unsigned int i=0; i<matches.size(); ++i)
                        {
                                // Apply NNDR
                                //printf("q=%d dist1=%f dist2=%f\n", matches.at(i).at(0).queryIdx, matches.at(i).at(0).distance, matches.at(i).at(1).distance);
                                if(matches.at(i).size() == 2 &&
                                   matches.at(i).at(0).distance <= nndrRatio * matches.at(i).at(1).distance)
                                {
                                        mpts_1.push_back(objectKeypoints.at(matches.at(i).at(0).queryIdx).pt);
                                        indexes_1.push_back(matches.at(i).at(0).queryIdx);

                                        mpts_2.push_back(sceneKeypoints.at(matches.at(i).at(0).trainIdx).pt);
                                        indexes_2.push_back(matches.at(i).at(0).trainIdx);
                                }
                        }
                }

                // FIND HOMOGRAPHY
                unsigned int minInliers = 8;
                if(mpts_1.size() >= minInliers)
                {
//                        time.start();
                        cv::Mat H = findHomography(mpts_1, mpts_2,cv::RANSAC, 1.0, outlier_mask);
//                        printf("Time finding homography = %d ms\n", time.restart());
                        int inliers=0, outliers=0;
                        for(unsigned int k=0; k<mpts_1.size();++k)
                        {
                                if(outlier_mask.at(k))
                                {
                                        ++inliers;
                                }
                                else
                                {
                                        ++outliers;
                                }
                        }
				}

                ////////////////////////////
                //Cleanup
                ////////////////////////////
                delete detector;
                delete extractor;
				cv::imshow("Template",objectImg);
				cv::imshow("Scene_frame",sceneImg)l;
        }
        else
        {
                printf("Images are not valid!\n");
                showUsage();
        }
        return 1;
}*/

#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <time.h>

using namespace cv;
Rect selection;
int selectObject = 0;
int trackObject= 0;
void bg_temporaldifference(Mat currframe, Mat prevframe, Mat backgorund)
{
}
/*static void onMouse( int event, int x, int y, int, void* )
	{
    if( selectObject )
    {
        selection.x = MIN(x, origin.x);
        selection.y = MIN(y, origin.y);
        selection.width = std::abs(x - origin.x);
        selection.height = std::abs(y - origin.y);

        selection &= Rect(0, 0, image.cols, image.rows);
    }

    switch( event )
    {
    case CV_EVENT_LBUTTONDOWN:
        origin = Point(x,y);
        selection = Rect(x,y,0,0);
        selectObject = true;
        break;
    case CV_EVENT_LBUTTONUP:
        selectObject = false;
        if( selection.width > 0 && selection.height > 0 )
            trackObject = -1;
        break;
    }*/

#include <math.h>
double calculate_angle(Point2f* points,RotatedRect rotrect)
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
	return angle;}



int main()
{
	cv::initModule_nonfree();
	Mat object = cvLoadImage("Template_mr.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	//namedWindow("Template",CV_WINDOW_AUTOSIZE);
	//imshow("Template",object);
    if( !object.data)
    {
        std::cout<< "Error reading object " << std::endl;
        return -1;
    }

	cv::BackgroundSubtractorMOG2 bgsub;
	Mat fgmask, segm, roi, buff, fmask, obj_or;object.copyTo(obj_or);
	namedWindow("Check");
    //Detect the keypoints using SURF Detector
    int minHessian = 500;
	GaussianBlur( object, object, Size(3,3), 0, 0, BORDER_DEFAULT );
	FeatureDetector * detector = new SurfFeatureDetector(500);
	bgsub.nmixtures = 3;
	//bgsub.set("initializationFrames",20);
	//bgsub.set("decisionThreshold",0.7);
	//cv::Ptr<SurfFeatureDetector> detector = cv::FeatureDetector::create("SURF");
    //SurfFeatureDetector detector( minHessian );
    std::vector<KeyPoint> kp_object;
	std::vector<KeyPoint> kp_img;
	DescriptorExtractor * extractor = new SurfDescriptorExtractor();
    Mat des_object, des_img;
	FlannBasedMatcher matcher;
	CvCapture * cap = cvCaptureFromAVI("Video_demo.wmv");
	/*printf("Size of frame : %d %d\n",im->width,im->height);
	imshow("Good Matches", cv::Mat(im));
	continue;*/
	if(cap == NULL)
		return -1;
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
	double maxC_area,maxCarea;
	int contour, cam_mot = 0,contor = 0; 
    char key = 'a';
	IplImage * im = cvQueryFrame(cap);
	Mat frame;
	int mfeature = 0, thresh = 50;
	IplImage * frameprev, * frame1, *frame_small, *object_small = cvCreateImage(cvSize((int)(object.cols / 5),(int)(object.rows / 5)),IPL_DEPTH_8U,object.channels());
    int framecount = 0, loopcount = 0;int corner_count, or_size;
	double angleprev, angle;
	std::vector<Point> approx;
	Rect boundrect; RotatedRect rott_rect;Point2f rotrect[4];
	time_t start,end;Mat boundcn;
	int thresh_type = 0;
	start = time(NULL);
	clock_t st,en;
	st = clock();
	/*while(key != 27){
		cvtColor(cv::Mat(im),frame,CV_RGB2GRAY);
		printf("\nImage type : %d",frame.channels());
		im = cvQueryFrame(cap);
		cvShowImage("Check",im);loopcount++;
		if(loopcount == 100){ end = time(NULL); en = clock();
		printf("Frame Processing rate : %f", difftime(end,start)) ;}
		key = cvWaitKey(30);}*/

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
		/*else if(loopcount == 1){
			delete detector;
			detector = new FastFeatureDetector(2000);}*/
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
			bgsub(frame,fgmask);}
		if(loopcount == 0 || mfeature == -1){
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
			/*line(img_matches, scene_corners[0] + Point2f( object.cols, 0), scene_corners[1] + Point2f( object.cols, 0), Scalar(0, 255, 0), 10 );
			line(cv::Mat(image), scene_corners[1] + Point2f( object.cols, 0), scene_corners[2] + Point2f( object.cols, 0), Scalar( 0, 255, 0), 10 );
			line(cv::Mat(image), scene_corners[2] + Point2f( object.cols, 0), scene_corners[3] + Point2f( object.cols, 0), Scalar( 0, 255, 0), 10 );
			line(cv::Mat(image), scene_corners[3] + Point2f( object.cols, 0), scene_corners[0] + Point2f( object.cols, 0), Scalar( 0, 255, 0), 10 );
			//double line1,line2,line3,line4;*/
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
			/*else{
				if((mm.m00 >= robot_area - 1000) && (mm.m00 <= robot_area + 1000)){
					maxC_area = mm.m00; contour = i;}}*/
		}
		//std::vector<std::vector<CvPoint>> Contour;
		//std::vector<CvPoint> con;
		//Contour.push_back(contours[contour]);
		//drawContours(update_tmp,contours,contour,Scalar(255),2);
		//imshow("Check",update_tmp);
		//imshow("Check",update_tmp);
		printf("\nContour no. : %d",contour);
		robot_area = contourArea(contours[contour]);
		robot_area  = (robot_area * 200 * 300 )/ (im ->width * im->height);
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
		printf("Thresh Type : %d\n",thresh_type);
		rectangle(cv::Mat(im),rectLine,Scalar::all(255),2);
		goto printpos;
}
		//bgsub.getBackgroundImage(segm);
		//imshow("Background",segm);
		//PrevRect = boundingRect(cv::Mat(scene_corners));
		prevframe = image;
		//cv::erode(Lpl,Lpl,cv::Mat());
		//cv::dilate(Lpl,Lpl,cv::Mat());
		if(loopcount > 0){
			frame = cv::Mat(frame1);
			cv::resize(frame,temp,Size(300,200));  temp.copyTo(frame); 
			cv::resize(fgmask,temp,Size(300,200)); temp.copyTo(fgmask);
			GaussianBlur(fgmask,fgmask,Size(3,3),0,0);
			/*detector -> detect(frameprev, kp_object);
			detector -> detect(im, kp_img);
			//printf("Keypoints prev: %d\n",kp_object.size());
			printf("Keypoints curr: %d\n",kp_img.size());
			extractor->compute(frameprev,kp_object,des_object);
			std::vector<std::vector<DMatch>> matches;
			std::vector<double> distance;
			extractor->compute(frame,kp_img,des_img);
			matcher.knnMatch(des_img,des_object,matches,2);
			std::vector<Point2f> prevfr, curfr;
			std::vector<DMatch> good_matches;
			for(int i = 0; i < min(des_img.rows-1,(int) matches.size()); i++) //THIS LOOP IS SENSITIVE TO SEGFAULTS
		   {
			    if((matches[i][0].distance < 0.6*(matches[i][1].distance)) && ((int) matches[i].size()<=2 && (int) matches[i].size()>0))
				{
					good_matches.push_back(matches[i][0]);
				}
			}
			for(int i = 0; i < matches.size(); i++)
			{
				distance.push_back(matches[i][0].distance);
				int index = int(matches[i][0].trainIdx);
				//printf("Index : %d %d %d\n",good_matches.size(),good_matches[11].queryIdx,good_matches[11].trainIdx);
				if(matches[i][0].trainIdx < kp_img.size() && matches[i][0].queryIdx < kp_object.size()){
					prevfr.push_back( kp_object[matches[i][0].queryIdx].pt );  
					curfr.push_back( kp_img[matches[i][0].trainIdx].pt);
				}}
			
			cv::Scalar mean_matches= mean(distance);*/
			//Laplacian(frame,frame,frame.depth(),1,0,0);
			//convertScaleAbs(frame,fgmask);
			if(loopcount > 0){
			boundrect.y = int(boundrect.y * 200/ frame1->height);
			boundrect.x = int(boundrect.x * 300/ frame1->width);
			 boundrect.width = boundrect.width * 300 / frame1 -> width;
			 boundrect.height = boundrect.height * 200 / frame1 -> height;}
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
			/*else
			{
			boundrect.x = boundrect.x + 3 * (COMprev.x - COM.x);
			boundrect.y = boundrect.y + 3 * (COMprev.y - COM.y);
			boundrect.height = boundrect.height + 5 * (COM.x - COMprev.x);
			boundrect.width = boundrect.width + 5 * (COM.y - COMprev.y);
			PrevRect  = boundrect;
			COMprev = COM;}	*/		
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
			//imshow("Check",buff);
			//cvSetImageROI(&IplImage(buff),boundrect);
			//buff = buff(boundrect);
			findContours(buff, contours, hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
			or_size = contours.size();
			printf("Subtracted image type : %d\n",contours.size());
			//printf("\nArea of boundrect : %d\n",boundrect.area());}
			//if(contours.size() == 6){
			//Mat mak = Mat::zeros(frame.size(),frame.depth());
			//rectangle(mak,boundrect,Scalar::all(255),1);}
			if(contours.size() >= 50){
			//Mat Transform;
			//Mat Transform_avg=Mat::eye(frame1 -> width,frame1 -> height,CV_8U);
			//if(mean_matches.val[0] > 0.15 || mfeature)
			//{
			//Mat boundcn = Mat::zeros(frame.size(),CV_8U);
			//boundcn.copyTo(fmask);
			//drawContours(fmask,contours[contour],-1,cv::Scalar(255),1);
			//frame = cvQueryFrame(cap);
			/*if(good_matches.size() >= 4){
			namedWindow("Homography", CV_WINDOW_AUTOSIZE);
			printf("points2.vector(0 : %d %d",curfr.size(),prevfr.size());
			temp = findHomography(prevfr,curfr,CV_RANSAC);
			warpPerspective(frame,frame,te
			mp,frame.size(), INTER_LINEAR | WARP_INVERSE_MAP, BORDER_TRANSPARENT);}
			/*frame = cv::Mat(frame);
			cv::cvtColor(frame,temp,cv::COLOR_BGR2GRAY);
			CvSize img_sz = cvGetSize(frameprev);
			cvtColor(cv::Mat(frameprev),prevframe,CV_BGR2GRAY);
			vector<Point2f>cornerPrev;      
			cornerPrev.reserve(100);

			vector<Point2f>cornerCur;
			cornerCur.reserve(100);

			goodFeaturesToTrack(prevframe,cornerPrev,100,0.05,5.0,Mat(),3,false,0.04);
			goodFeaturesToTrack(temp,cornerCur,100,0.05,5.0,noArray(),3,false,0.04);
			double disp = 0;
			for(int i = 0; i < min(cornerPrev.size(),cornerCur.size());i++)
			{
				disp += sqrt(pow(cornerPrev[i].x - cornerCur[i].x,2) + pow(cornerPrev[i].y - cornerCur[i].y,2));
				printf("Average displaecement : %f\n", disp);
			}
			printf("Average displaecement : %f\n", disp /= 100);

    //cornerSubPix(prevImg,cornerPrev,Size(winSize,winSize),Size(-1,1),TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS,20,0.03));
    //cornerSubPix(currImg,cornerCur,Size(winSize,winSize),Size(-1,1),TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS,20,0.03));

    //Call Lucas Kanade algorithm
			CvSize pyr_sz = Size (img_sz.width+8, img_sz.height/3);

			vector<uchar>featureFound;
			featureFound.reserve(100);

			vector<float>featureErrors;
			featureErrors.reserve(100);    */

		//Only work on gray-scale image
			//calcOpticalFlowPyrLK(prevframe,temp,cornerPrev,cornerCur,featureFound,featureErrors,Size(15,15),3, cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.3),0,0.0001);

			//Mat transformMatrix = findHomography(cornerPrev,cornerCur,CV_RANSAC,0);



    //Computes an optimal affine transformation between two 2D point sets.
    //Mat trans = estimateRigidTransform(currImg,prevImg,false);
    //warpAffine(prevImg,outImg,transformMatrix,Size(640,480),INTER_NEAREST|WARP_INVERSE_MAP);

			//warpPerspective(temp,temp,transformMatrix,img_sz,INTER_NEAREST|WARP_INVERSE_MAP,BORDER_CONSTANT,0);
			//cv::equalizeHist(temp,temp);
			//cv::Canny(temp, temp,64,64)
           // Transform=estimateRigidTransform(temp,prevframe,0);
            //Transform(Range(0,2),Range(0,2))=Mat::eye(2,2,CV_64FC1);
            //Transform_avg+=(Transform-Transform_avg)/2.0;
			//warpAffine(temp,temp,Transform_avg,Size( frame1 -> width, frame1 -> height));
			//cvSmooth(&IplImage(fgmask),&IplImage(fmask));
			//im = cvQueryFrame(cap);
			//if(mfeature == 0)
			//cvCopy(im,frameprev);
			//fmask = frame(boundrect);
			//fmask.copyTo(object);
			goto track;
			revert:
			mfeature++;
			printf("Mfeature : %d\n",mfeature);
			if((angle < angleprev / 1.5 || angle > angleprev * 1.5)){
				if(mt.m00 < robot_area * 1.3 && mt.m00 > robot_area / 1.3)
					goto printpos;
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
			if(or_size == 245){
				rectangle(fmask,boundrect,Scalar::all(255),1);
				imshow("Check",fgmask);}
			//if(mfeature > 1)
			//	goto track;
			//rectangle(cv::Mat(im),boundrect,Scalar::all(255),2);
			//im = cvQueryFrame(cap);
			cvCopy(im,frameprev);
			goto printpos;
			bgsub(frame,fgmask);
			//imshow("Homography",temp);

			//imshow("Good Matches",fgmask);
			//cvtColor(fgmask,fmask,CV_RGB2GRAY);
			//rectangle(cv::Mat(im),boundrect,cv::Scalar::all(255),2);
			//temp = frame(boundrect);
			//imshow("Good Matches",cv::Mat(im));
			//cvResetImageROI(&IplImage(fmask));
		}
		//else{
			fmask = fgmask(boundrect);
			printf("COM : (%f,%f)\n",(COM.x),COM.y);
			//Laplacian(fgmask,fgmask,fgmask.depth(),3);convertScaleAbs(fgmask,fmask);
			GaussianBlur(fmask,fmask,Size(3,3),0,0); 
			//Canny(fmask,fmask,50,255);
			cv::threshold(fmask,fmask,50,255,CV_THRESH_BINARY);
			//adaptiveThreshold(fgmask,fgmask,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C,CV_THRESH_BINARY,11,2);
			//medianBlur(fmask,fmask,3);
			//Laplacian(fmask,fmask,fmask.depth(),3);convertScaleAbs(fgmask,fmask);	
			morphologyEx(fmask,fmask,MORPH_CLOSE, cv::Mat());
		//cv::erode(fmask,fmask,cv::Mat());
		//cv::dilate(fmask,fmask, cv::Mat())
			//imshow("Check",fmask);
			//goto printpos;
			//if(mfeature == 1){
				//imshow("Good Matches",fmask);
			IplImage * fmas = &IplImage(fmask);
			//	cvSetImageROI(fmas,boundrect);
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
		rectangle(boundcn,boundrect,Scalar::all(255),1);
		printf("Background sub area : %f %f\n",maxC_area,robot_area);
		//approxPolyDP(contours[contour],approx,3,true);
		drawContours(fmask,contours,contour,cv::Scalar(255),1);
		/*if(maxC_area == 223.00)
		{
			fgmask.copyTo(boundcn);
			drawContours(boundcn,contours,contour,Scalar::all(255),1);
			rectangle(boundcn,boundrect,Scalar::all(255),1); 
			imshow("Check", boundcn);
		}*/
		//if(maxC_area == 3772.5)
		//	imshow("Check",fmask);
		if(maxC_area < robot_area / 1.15 || maxC_area > robot_area * 1.15){
			track: 
			boundcn = Mat::zeros(frame.size(),CV_8U);
			recalc:
			frame.copyTo(temp);	
			temp.copyTo(boundcn);
			buff = cv::Mat::zeros(frame.size(), CV_8U);
			buff = temp(boundrect);
			//medianBlur(buff,buff,3);
			GaussianBlur(buff,buff,Size(3,3),0,0,BORDER_DEFAULT);
			//if(maxC_area == 99.5){adaptiveThreshold(temp,temp,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C ,CV_THRESH_BINARY_INV,11,2);
			//Canny(temp,temp,200,255);}
			//else
			//if(maxC_area == 307.5){

		//	if(or_size == 120){
				//thresh = 40; thresh_type = 1;}
			int max_thresh = 255;
			if(!thresh_type && (cv::Mat(frame1).at<uchar>(Point((int)COM.x,(int)COM.y)) < 50))
				{max_thresh = 90; thresh = 30; }
			else if(thresh_type && (cv::Mat(frame1).at<uchar>(Point(COM.x,COM.y)) > 50)){
				if(cv::Mat(frame1).at<uchar>(Point(COM)) > cv::Mat(frame1).at<uchar>(Point(boundrect.x + 50, boundrect.y + boundrect.height / 2)))
				{thresh = 50; thresh_type = 0;}}
			printf("Thresh Type : %d\n",thresh_type);
			cv::threshold(buff,buff,thresh,max_thresh,thresh_type);
			temp.copyTo(boundcn);
			//cv::threshold(boundcn,boundcn,thresh,max_thresh,thresh_type);
			rectangle(boundcn,boundrect,Scalar::all(255),1);
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
			printf("Pixel brightness : %d\n",cv::Mat(frame1).at<uchar>(Point(COM)));
			contours.clear();
			findContours(buff, contours, hierarchy, CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
			maxCarea = 0;
			for (int i = 0; i < contours.size(); i++)
			{
				mt = cv::moments(contours[i], false);
			if(mt.m00 > maxCarea){
				maxCarea  = mt.m00;contor = i;}
			}
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
				check++;  PrevRect = boundrect; goto recalc;}check = 0;
				if((maxCarea < robot_area / 1.3 || maxCarea > robot_area * 1.3) && mfeature <= 3)
					goto revert;
			boundcn = Mat::zeros(frame.size(),CV_8U);
			drawContours(boundcn,contours,contor,Scalar::all(255),1);
			boundrect = boundingRect(cv::Mat(contours[contor]));
			rectangle(boundcn,boundrect,Scalar::all(255),1);
			//Mat check_thresh;
			//cv::resize(boundcn,check_thresh,Size(im->width,im->height)); check_thresh.copyTo(boundcn);
			//rectangle(boundcn,boundrect,Scalar::all(255),1);
			//if(loopcount == 1)
			//imshow("Check",boundcn);
			//if(maxC_area == 631.5)
			//	imshow("Check",boundcn);
			rott_rect = minAreaRect(contours[contor]); 
			printf("Contour size in alternative : %f\n",cv::moments(contours[contor]).m00);}
		else{
		boundrect = boundingRect(cv::Mat(contours[contour]));
		mt = cv::moments(contours[contour]);
		drawContours(boundcn,contours,contour,Scalar::all(255),1);
		rott_rect = minAreaRect(contours[contour]);}
		rott_rect.points(rotrect);
		for(int i = 0; i < 3 ; i++)
			line(boundcn,rotrect[i],rotrect[i+1],Scalar::all(255),1);
		line(boundcn,rotrect[3],rotrect[0],Scalar::all(255),1); 
		cvDrawCircle(&IplImage(boundcn),Point(rotrect[2]),3,Scalar::all(255),1);
		angle = calculate_angle(rotrect,rott_rect);
		boundrect.x += PrevRect.x;
		boundrect.y += PrevRect.y;
		if((std::abs(angleprev - angle) != 180) && (angle < angleprev / 1.5 || angle > angleprev * 1.5) && mfeature <= 3){printf("Angle curr Angle prev : %f %f\n",angle,angleprev);
			goto revert;}
		printf("Angle: %f\n",calculate_angle(rotrect,rott_rect));
		COM.x = mt.m10/mt.m00; COM.y = mt.m01/mt.m00;
		COM.x += PrevRect.x; COM.y += PrevRect.y;
		//boundcn = Mat::zeros(frame.size(),CV_8U);
		//rectangle(boundcn,boundrect,Scalar::all(255),1);
		//cvDrawCircle(&IplImage(boundcn),Point(COM.x,COM.y),5,Scalar::all(255),1);	
		PrevRect = boundrect;
		mfeature = 0;
		COM.x = (COM.x *  frame1 -> width) / 300;
		COM.y = (COM.y * frame1 -> height) / 200;
		//if(maxC_area == 3772.5)
			//thresh_type;
		//printf("Pixel Values : %d\n",int(frame.at<uchar>(COM)));
		int x = COM.x ;int y= COM.y;
		printf("COM : %d %d \n", x,y);
		/*throw 1;
		}
		catch(int n){}*/
		//rectangle(temp,boundrect,Scalar::all(255),1); cvDrawCircle(&IplImage(boundcn),Point(boundrect.x,boundrect.y),5,Scalar::all(255),1);
		//if(loopcount == 2){
		//imshow("Check",cv::Mat(fmas));
		//cvResetImageROI(fmas);
		cvCopy(im,frameprev);
		//boundcn = Mat::zeros(cv::Mat(im).size(), cv::Mat(im).depth());
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
		string position[] = {"Position is (",",",")"};
		string pos = position[0] + std::to_string(long double(COM.x)) + position[1] + std::to_string(long double(COM.y))+position[2];
		CvFont font;
		if(loopcount == 0)
			angleprev = angle = calculate_angle(rotrect,rott_rect);
		if((!mfeature) && (angle > angleprev / 1.5 && angle < angleprev * 1.5))
		{angleprev = calculate_angle(rotrect,rott_rect);printf("Angle prev update \n"); framecount = loopcount; }
		if((loopcount - framecount > 3) && !mfeature)
			angleprev = calculate_angle(rotrect,rott_rect);
		string angle[] = {"Angle",":","degrees"};
		string angle_s = angle[0]+angle[1]+std::to_string(long double(angleprev))+angle[2];
		cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX,1.0,1.0,0);
		CvPoint org;org.x = COM.x; org.y = COM.y;
		cvPutText(im,angle_s.c_str(),cvPoint(20,40),&font,Scalar(0,255,255));
		cvPutText(im,pos.c_str(),org,&font, cv::Scalar(255,255,255));

	next:
		//if(or_size == 3772.5){
		cvDrawCircle(im,Point(COM),5,Scalar::all(255),1);
		cvDrawCircle(im,Point(boundrect.x + boundrect.width + 40,boundrect.y + boundrect.height / 2),10,Scalar::all(255),1);
		imshow("Good Matches", cv::Mat(im));//break;
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
		loopcount++;
		//imshow("Background",object);
		if(loopcount == 100){
			imshow("Good Matches",boundcn);
			end = time(NULL);
			printf("Frame processing rate : %f",difftime(end,start));}

	}
    return 0;
}

#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"

using namespace cv;


/* @function main */
/*int main(int argc, char** argv)
{
		cv::initModule_nonfree(); // to load SURF/SIFT etc.
        IplImage * tmp = cvLoadImage("Template_mr.jpg",CV_LOAD_IMAGE_GRAYSCALE);
		Mat img_1= cv::Mat(tmp);
		IplImage * searchim = cvLoadImage("Snapshot_microrobot1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
        Mat img_2 = cv::Mat(searchim);
		cvNamedWindow("Template");
		namedWindow("Search Image");

        if (!img_1.data || !img_2.data)
        {
                std::cout << " --(!) Error reading images " << std::endl; return 0;
        }
        //-- Step 1: Detect the keypoints using SURF Detector
        int minHessian = 150;
		SurfFeatureDetector * detector = new SurfFeatureDetector(minHessian);
		//cv::Ptr<SurfFeatureDetector> detector = cv::FeatureDetector::create("SURF");
		//SurfFeatureDetector detector(minHessian);

        std::vector<KeyPoint> keypoints_1, keypoints_2;
		imshow("Template",img_1);
		imshow("Search Image",img_2);
		printf("Type of image : %s depth : %d channels : %d",tmp -> colorModel, tmp -> depth, tmp -> nChannels);
		detector->detect(img_1, keypoints_1);
		//detector.detect(Mat(searchim), keypoints_2);
		
        //-- Step 2: Calculate descriptors (feature vectors)
        SurfDescriptorExtractor extractor;

        Mat descriptors_1, descriptors_2;

        extractor.compute(img_1, keypoints_1, descriptors_1);
        extractor.compute(img_2, keypoints_2, descriptors_2);

        //-- Step 3: Matching descriptor vectors using FLANN matcher
        FlannBasedMatcher matcher;
        std::vector<vector< DMatch >>matches;
		matcher.knnMatch(descriptors_1, descriptors_2, matches,2);

        double max_dist = 0; double min_dist = 100;

        //-- Quick calculation of max and min distances between keypoints
        for (int i = 0; i < descriptors_1.rows; i++)
        {
                double dist = matches[i][0].distance;
                if (dist < min_dist) min_dist = dist;
                if (dist > max_dist) max_dist = dist;
        }

        printf("-- Max dist : %f \n", max_dist);
        printf("-- Min dist : %f \n", min_dist);

        //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist )
        //-- PS.- radiusMatch can also be used here.
        std::vector< DMatch > good_matches;

        for (int i = 0; i < descriptors_1.rows; i++)
        {
                if (matches[i][0].distance < 2 * min_dist)
                {
                        good_matches.push_back(matches[i][0]);
                }
        }

        //-- Draw only "good" matches
        Mat img_matches;
        drawMatches(img_1, keypoints_1, img_2, keypoints_2,
                good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

        //-- Show detected matches
        imshow("Good Matches", img_matches);

        for (int i = 0; i < good_matches.size(); i++)
        {
                printf("-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx);
        }

        waitKey(0);

        return 0;
}*/