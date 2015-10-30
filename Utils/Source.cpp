#include "cv.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main(){
	Mat img = imread("C:\\Users\\bisai\\Documents\\temp\\gray.jpg");
	imshow("test", img);
	waitKey(0);
	return 0;
}