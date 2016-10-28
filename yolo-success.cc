#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main()
{
//	VideoCapture stream1(0);

	//Mat cameraFrame;
	//stream1.read(cameraFrame);

	//imshow("Camera output",cameraFrame);
  Mat imageMe = imread("yolo.png", 100);

	namedWindow("y o l o", WINDOW_AUTOSIZE);	
	imshow("yolo", imageMe);

	waitKey(0); 
 
	return 0;
}
