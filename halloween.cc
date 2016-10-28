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

void alphaBlend(const cv::Mat& spider, const cv::Mat& frame, cv::Mat& blend_dest) {
	blend_dest = frame.clone();
	std::cout << "blend cols" << blend_dest.cols << "  " << blend_dest.rows << std::endl;
	std::cout << "spider cols" << spider.cols << "  " << spider.rows << std::endl;
	std::cout << "frame cols" << frame.cols << "  " << frame.rows << std::endl;
	
	for (int i = 0; i < frame.cols; ++i) {
		for (int j = 0; j < frame.rows; ++j) {
			cv::Vec4b intensity1 = spider.at<cv::Vec4b>(i, j);
			uchar red1 = intensity1.val[0];
			uchar green1 = intensity1.val[1];
		  uchar blue1 = intensity1.val[2];
			uchar alpha1 = intensity1.val[3];
	
			cv::Vec4b intensity2 = frame.at<cv::Vec4b>(i, j);
			uchar red2 = intensity2.val[0];
			uchar green2 = intensity2.val[1];
			uchar blue2 = intensity2.val[2];
			uchar alpha2 = intensity2.val[3];

			float result_alpha = (alpha1 / 255.0);
	
			uchar result_blue = blue1*result_alpha + blue2*(1-result_alpha);
			uchar result_green = green1*result_alpha + green2*(1-result_alpha);
			uchar result_red = red1*result_alpha + red2*(1-result_alpha);

			blend_dest.at<cv::Vec4b>(i, j) = cv::Vec4b(result_red, result_green, result_blue, 255); 
		
		}
	}

}

void detectFaces(cv::Mat& frame, const cv::Mat& spider) {
	std::vector<cv::Rect> faces; // c++
	//Rect c_faces = (cv::Rect*) malloc( sizeof(cv::Rect) * ); 	
	cv::Mat grayscale_frame;
	cvtColor(frame, grayscale_frame, cv::COLOR_BGR2GRAY);
	equalizeHist(grayscale_frame, grayscale_frame);

	uchar* camData = new uchar[frame.total()*4];
	cv::Mat frameRGBA(frame.size(), CV_8UC4, camData);
	cv::cvtColor(frame, frameRGBA, CV_BGR2RGBA, 4);
	std::cout<< "rgbaframe type"<< type2str(frameRGBA.type()) <<std::endl;

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

		cv::Point center(face_x + faces[i].width/2, face_y + faces[i].height/2);

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

		alphaBlend(spider_crop, frame_crop, blend_dest);
		//addWeighted( spider_crop, 0.5, frame_crop, 0.5, 0.0, blend_dest );

		blend_dest.copyTo(frameRGBA(cv::Rect(spider_top_left.x, spider_top_left.y, spider_end_x - spider_x, spider_end_y - spider_y)));
		
		ellipse(frame, center, cv::Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, cvScalar(255, 0, 255), 4, 8, 0);
	}
	
	cv::imshow(window_name, frameRGBA);
	delete[] camData;
}


int main(void) 
{
	cv::VideoCapture cap(0);
	cv::Mat frame;
	cv::Mat spider = cv::imread("spider.png", -1);
		std::cout<< "frame type"<< type2str(frame.type()) <<std::endl;
		std::cout<< "png type"<< type2str(spider.type()) <<std::endl;
  
	face_cascade.load("haarcascade_frontalface_alt.xml");
	
	while(cap.read(frame)) {
		detectFaces(frame, spider);

		// Pause
		if( cv::waitKey(30) >= 0)
			break;
	}
	return 0;
}


