#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>


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
	std::vector<cv::Rect> faces; // c++
	//Rect c_faces = (cv::Rect*) malloc( sizeof(cv::Rect) * ); 	
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
		int spider_end_x = spider.cols;
		int spider_end_y = spider.rows;	
		std::cout<< "face_x" <<face_x  <<std::endl;
		std::cout<< "face_y"<<face_y  <<std::endl;
		cv::Point center(face_x + faces[i].width/2 - 2, face_y + faces[i].height/2);

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
		std::cout<< "spider_end_x"<<spider_end_x  <<std::endl;
		std::cout<< "spider_end_y"<<spider_end_y  <<std::endl;
	
		std::cout<< "spider_x"<<spider_x  <<std::endl;
		std::cout<< "spider_y"<<spider_y  <<std::endl;

		std::cout<< "spider_top_left ? "<< (spider_top_left.x + spider_end_x - spider_x) <<std::endl;
		std::cout<< "frame width   " << frame.cols << std::endl;
		std::cout<< "spider_top_x"<<spider_top_left.x  <<std::endl;
		std::cout<< "spider_top_y"<<spider_top_left.y  <<std::endl;
		std::cout<< "spider_wdth"<<(spider_end_x - spider_x)  <<std::endl;
		std::cout<< "spider_with_y"<<(spider_end_y - spider_y) <<std::endl;
		cv::Mat spider_crop =	spider(cv::Range(spider_x, spider_end_x - 1), cv::Range(spider_y, spider_end_y - 1));

		spider_crop.copyTo(grayscale_frame(cv::Rect(spider_top_left.x, spider_top_left.y, spider_end_x - spider_x, spider_end_y - spider_y)));
		ellipse(frame, center, cv::Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, cvScalar(255, 0, 255), 4, 8, 0);
	}

	cv::imshow(window_name, grayscale_frame);
}


int main(void) 
{
	cv::VideoCapture cap(0);
	cv::Mat frame;
	cv::Mat spider = cv::imread("spider.png", CV_LOAD_IMAGE_GRAYSCALE);
  
	face_cascade.load("haarcascade_frontalface_alt.xml");
	
	while(cap.read(frame)) {
		detectFaces(frame, spider);

		// Pause
		if( cv::waitKey(30) >= 0)
			break;
	}
	return 0;
}


