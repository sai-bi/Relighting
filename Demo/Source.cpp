#include "cv.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include "demo.h"
#include "Utils\utils.h"

using namespace std;
using namespace cv;




int main(){
	string dir = "C:\\Users\\bisai\\Documents\\research\\prt\\scenes\\sh";
	for (int i = 0; i < 9; i++){
		SaveSHBasisImage(16, i, cv::Size(256, 128), (dir + "-" + to_string(i) + ".exr").c_str());
	}
	// dir = "C:\\Users\\bisai\\Documents\\research\\prt\\scenes\\sh";
	string mitsuba_path = "C:\\Users\\bisai\\Downloads\\Mitsuba";
	string scene_path = "C:\\Users\\bisai\\Documents\\research\\prt\\scenes\\matpreview.xml";
	string target_dir = "C:\\Users\\bisai\\Documents\\research\\prt\\sh_result";
	string xml_path = "C:\\Users\\bisai\\Documents\\research\\prt\\scenes\\temp_scene.xml";
	/*for (int i = 0; i < 9; i++){
		cout << i << endl;
		SaveSHBasisImage(16, i, cv::Size(256, 128), (dir + ".exr").c_str());
		string output_path = dir + "_out_" + to_string(i) + ".exr";
		SaveSHBasisImage(16, i, cv::Size(256, 128), (dir + ".exr").c_str());
		string command = mitsuba_path + "\\mitsuba.exe " + xml_path + " -q -o " + output_path;
		system(command.data());
	}*/


	vector<Mat> final_image;
	vector<Mat> final_image2;
	final_image.resize(3);
	final_image2.resize(3);
	for (int i = 0; i < 3; i++){
		final_image[i] = Mat::zeros(512, 512, CV_32FC1);
		final_image2[i] = Mat::zeros(128, 256, CV_32FC1);
	}


	string env_path = "C:\\Users\\bisai\\Documents\\research\\prt\\scenes\\grace_cross.exr";
	Mat env_img = imread(env_path, cv::IMREAD_ANYCOLOR | cv::IMREAD_ANYDEPTH);
	resize(env_img, env_img, cv::Size(256, 128));

	imwrite("C:\\Users\\bisai\\Documents\\research\\prt\\scenes\\grace_cross_new.exr", env_img);
	Mat_<float> coeff(3, 9);



	Mat final = Mat::zeros(128, 256, CV_32FC1);
	vector<Mat> channels;

	float rv[9] = { 0.79, 0.39, -0.34, -0.29, -0.11, -0.26, -0.16, 0.56, 0.21 };
	float gv[9] = { 0.44, 0.35, -0.18, -0.16, -0.05, -0.22, -0.09, 0.21, -0.05 };
	float bv[9] = { 0.54, 0.60, -0.27, 0.01, -0.12, -0.47, -0.15, 0.14, -0.30 };

	for (int i = 0; i < 9; i++){
		string image_name = dir  + "-" + to_string(i) + ".exr";
		Mat basis = imread(image_name, cv::IMREAD_ANYCOLOR | cv::IMREAD_ANYDEPTH);
		Scalar t1 = sum(basis.mul(basis));
		//float v = t1[0];// *(4.0* CV_PI) / (128.0 * 256.0);
		//v = sqrt(v);
		float v = 1.0;

		split(basis, channels);
		final_image2[0] += bv[i] * channels[0] ;
		final_image2[1] += gv[i] * channels[1] ;
		final_image2[2] += rv[i] * channels[2] ;


		Scalar t = sum(basis.mul(env_img));
		coeff(0, i) = t[0] / v;
		coeff(1, i) = t[1] / v;
		coeff(2, i) = t[2] / v;

		channels.clear();
	}

	coeff = coeff * (4.0* CV_PI) / (128.0 * 256.0);
	// cout << coeff << endl;
	Mat output;
	merge(final_image2, output);
	imwrite("C:\\Users\\bisai\\Documents\\research\\prt\\scenes\\sh_light.exr", output);







	for (int i = 0; i < 3; i++){
		final_image[i] = Mat::zeros(512, 512, CV_32FC1);
	}


	for (int i = 0; i < 9; i++){
		cout << i << endl;
		string output_path = dir + "_out_" + to_string(i) + ".exr";
		Mat img = imread(output_path, cv::IMREAD_ANYCOLOR | cv::IMREAD_ANYDEPTH);
		split(img, channels);
		final_image[0] += bv[i] * channels[0] * 8;
		final_image[1] += gv[i] * channels[1] * 8;
		final_image[2] += rv[i] * channels[2] * 8;
		/*final_image[0] += coeff(0,i) * channels[0];
		final_image[1] += coeff(1,i) * channels[1];
		final_image[2] += coeff(2,i) * channels[2];*/
		channels.clear();
	}

	;
	merge(final_image, output);
	imwrite("C:\\Users\\bisai\\Documents\\research\\prt\\scenes\\sh_result.exr", output);

	return 0;
}

