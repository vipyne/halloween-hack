////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// a silly halloween hack at Recurse Center -- october 2016
// uses OpenCV face detection
//
// compile : $ g++ halloween.cc -o holla.exe `pkg-config --cflags --libs opencv` 
// usage   : $ ./holla.exe 
// example : $ ./holla.exe
//           ==> a window opens and streams from camera... plus fun stuff 
//
// authors: @vipyne and @alinen
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>


cv::CascadeClassifier face_cascade;
cv::String window_name = "~ ~ ~  s p i d e r   v i s i o n  ~ ~ ~";

namespace vp
{
	std::string type2str(int type);
	void alphaBlend(const cv::Mat& spider, cv::Mat& frame, cv::Mat& blend_dest);
	void detectFaces(cv::Mat& frame, const cv::Mat& spider);
}

// http://stackoverflow.com/a/17820615/3191800
std::string vp::type2str(int type) {
  std::string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }

  r += "C";
  r += (chans+'0');

  return r;
}

void vp::alphaBlend(const cv::Mat& spider, cv::Mat& frame, cv::Mat& blend_dest) {
	blend_dest = cv::Mat(frame.rows, frame.cols, CV_8UC4);
	
	for (int i = 0; i < frame.cols; ++i) {
		for (int j = 0; j < frame.rows; ++j) {
			cv::Vec4b intensity1 = spider.at<cv::Vec4b>(j, i);
			uchar red1 = intensity1.val[0];
			uchar green1 = intensity1.val[1];
		  uchar blue1 = intensity1.val[2];
			uchar alpha1 = intensity1.val[3];
	
			cv::Vec4b intensity2 = frame.at<cv::Vec4b>(j, i);
			uchar red2 = intensity2.val[0];
			uchar green2 = intensity2.val[1];
			uchar blue2 = intensity2.val[2];
			uchar alpha2 = intensity2.val[3];

			float result_alpha = (alpha1 / 255.0);
	
			uchar result_blue = blue1*result_alpha + blue2*(1-result_alpha);
			uchar result_green = green1*result_alpha + green2*(1-result_alpha);
			uchar result_red = red1*result_alpha + red2*(1-result_alpha);

			blend_dest.at<cv::Vec4b>(j, i) = cv::Vec4b(result_red, result_green, result_blue, 255);
		}
	}
}

void vp::detectFaces(cv::Mat& frame, const cv::Mat& input_spider) {
	std::vector<cv::Rect> faces; // c++
	//Rect c_faces = (cv::Rect*) malloc( sizeof(cv::Rect) * ); // someday set this up in c too... just for kicks
	cv::Mat grayscale_frame;
	cvtColor(frame, grayscale_frame, cv::COLOR_BGR2GRAY);
	equalizeHist(grayscale_frame, grayscale_frame);

	// add alpha channel to camera frames
	uchar* camData = new uchar[frame.total()*4];
	cv::Mat frameRGBA(frame.size(), CV_8UC4, camData);
	cv::cvtColor(frame, frameRGBA, CV_RGB2RGBA, 4);

	// void CascadeClassifier::detectMultiScale( const Mat& image, 
	// vector<Rect>& objects, 
	// double scaleFactor=1.1, 
	// int minNeighbors=3, 
	// int flags=0, 
	// Size minSize=Size(), 
	// Size maxSize=Size())
	face_cascade.detectMultiScale( grayscale_frame, 
		 faces, 
		 1.1, 
		 3, 
		 0|cv::CASCADE_SCALE_IMAGE, 
		 cv::Size(30, 30)); 

	for(size_t i = 0; i < faces.size(); ++i) {
		cv::Mat spider; 
		//	void resize(InputArray src, OutputArray dst, Size dsize, double fx=0, double fy=0, int interpolation=INTER_LINEAR )
		cv::resize(input_spider, spider, cv::Size(faces[i].width/3, faces[i].height/3), 0, 0);

		// place spider on forehead above left eye	
		cv::Point center(faces[i].x + faces[i].width/1.5, faces[i].y + faces[i].height/6);
		
		int spider_end_x = spider.cols;
		int spider_end_y = spider.rows;

		cv::Point spider_top_left(center.x - spider_end_x/2, center.y - spider_end_y/2); 

		int spider_x = 0;
		int spider_y = 0;

		if (spider_top_left.x < 0) {
			spider_x = abs(spider_top_left.x);
			spider_top_left.x = 0;
		}
		if (spider_top_left.y < 0) {
			spider_y = abs(spider_top_left.y);
			spider_top_left.y = 0;
		}

		if (spider_top_left.x + spider_end_x - spider_x > frame.cols) {
			spider_end_x = frame.cols - spider_top_left.x - 1;
		}		
		if (spider_top_left.y + spider_end_y - spider_y > frame.rows) {
			spider_end_y = frame.rows - spider_top_left.y - 1;
		}

		cv::Mat spider_crop =	spider( cv::Range(spider_y, spider_end_y), cv::Range(spider_x, spider_end_x) );
		cv::Mat frame_crop = frameRGBA(cv::Range(spider_top_left.y, spider_top_left.y + spider_end_y - spider_y), cv::Range(spider_top_left.x, spider_top_left.x + spider_end_x - spider_x));
		
		cv::Mat blend_dest;
		vp::alphaBlend(spider_crop, frame_crop, blend_dest);

		blend_dest.copyTo(frameRGBA(cv::Rect(spider_top_left.x, spider_top_left.y, spider_end_x - spider_x, spider_end_y - spider_y)));
	}
	
	cv::imshow(window_name, frameRGBA);
	delete[] camData;
}


int main(void) 
{
	cv::VideoCapture cap(0);
	cv::Mat frame;
	cv::Mat input_spider = cv::imread("spider.png", -1);
  
	face_cascade.load("haarcascade_frontalface_alt.xml");
	
	std::cout << "" << std::endl;
	std::cout << "   it looks like there is something above your eye... look in the camera to see" << std::endl;
	std::cout << "" << std::endl;

	while(cap.read(frame)) {
		vp::detectFaces(frame, input_spider);

		// Pause
		if( cv::waitKey(30) == 'q')	
			break;

	}
	std::cout << "   happy halloween!" << std::endl;
	std::cout << "" << std::endl;

	return 0;
}


