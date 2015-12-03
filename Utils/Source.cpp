#include "cv.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "utils.h"
using namespace std;
using namespace cv;

int main(){
	for (int i = 0; i < 1; i++){
		vector<Mat> cubemap;
		cubemap.resize(6);
		for (int j = 0; j < 6; j++){
			cubemap[j].create(16, 16, CV_32FC3);
		}
		cubemap[0] = Mat::ones(16, 16, CV_32FC3);
		Mat envmap;
		ConvertCubemapToLightProbe(envmap, cubemap, Size(256, 128));
		imwrite("my_env.exr", envmap);
	}
	return 0;
	return 0;
}