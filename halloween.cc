#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>

//using namespace std;
//using namespace cv;

cv::CascadeClassifier face_cascade;
cv::String window_name = "~ ~ ~  s p i d e r   v i s i o n  ~ ~ ~";


std::string type2str(int type) {
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


void detectFaces(cv::Mat& frame, const cv::Mat& spider) {
	
	std::cout<< "frame size" << frame.rows << ", " << frame.cols << std::endl;

	std::vector<cv::Rect> faces; // c++
	//Rect c_faces = (cv::Rect*) malloc( sizeof(cv::Rect) * ); 	
std::cout<<"here at the detect faces"<<std::endl;
	cv::Mat grayscale_frame;

	cvtColor(frame, grayscale_frame, cv::COLOR_BGR2GRAY);

	equalizeHist(grayscale_frame, grayscale_frame);

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
	
		int face_x = faces[i].x;
		int face_y = faces[i].y;
		int spider_cols = spider.cols;
		int spider_rows = spider.rows;	
std::cout<< "face_x" <<face_x  <<std::endl;
std::cout<< "face_y"<<face_y  <<std::endl;
		cv::Point center(face_x + faces[i].width/2, face_y + faces[i].height/2); 
//		if (face_x + spider.cols > frame.cols) {
//			spider_cols = frame.cols - face_x - 1;
//		}
		
//		if (face_y + spider.rows > frame.rows) {
//			spider_rows = frame.rows - face_y - 1;
//		}
std::cout<< "spider_cols"<<spider_cols  <<std::endl;
std::cout<< "spider_rows"<<spider_rows  <<std::endl;


		spider.copyTo(grayscale_frame(cv::Rect(0, 0, spider_cols, spider_rows)));

		ellipse(frame, center, cv::Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, cvScalar(255, 0, 255), 4, 8, 0);
	}

std::cout<< "grayscale typeype"<<type2str(grayscale_frame.type())  <<std::endl;
	cv::imshow(window_name, grayscale_frame);
}


int main(void) 
{
	cv::VideoCapture cap(0);

	cv::Mat frame;
	//cv::Mat spider;
	cv::Mat spider = cv::imread("spider.png", CV_LOAD_IMAGE_GRAYSCALE);
  
	face_cascade.load("haarcascade_frontalface_alt.xml");

std::cout<< "frame type"<<type2str(frame.type())  <<std::endl;
std::cout<< "spider type"<<type2str(spider.type())  <<std::endl;
	
	while(cap.read(frame)) {
		detectFaces(frame, spider);
std::cout<< "while loop frame type"<<type2str(frame.type())  <<std::endl;
std::cout<< "while spider type"<<type2str(spider.type())  <<std::endl;

		// Pause
		if( cv::waitKey(30) >= 0)
			break;
	}
	return 0;
}


