// ***********************************************************************
// Assembly         : 
// Author           : bisai
// Created          : 10-27-2015
//
// Last Modified By : bisai
// Last Modified On : 10-29-2015
// ***********************************************************************
// <copyright file="utils.cpp" company="UCSD">
//     Copyright (c) . All rights reserved.
// </copyright>
// <summary>Some util functions.</summary>
// ***********************************************************************
#include "utils.h"
#include "pugixml\pugixml.cpp"

using namespace std;
using namespace cv;



/// <summary>
/// Given six faces of cubemap, save them as a horizontal-cross layout cubemap.
/// </summary>
/// <param name="filename">Target file name.</param>
/// <param name="cvEnvs">A vector that stores six faces of cubemaps.</param>
/// <returns>0</returns>
int saveAsCrossHorEnv(const std::string& filename, const std::vector<cv::Mat>& cvEnvs){
	if (cvEnvs.size() != 6){
		return 0;
	}
	for (int i = 0; i < 6; i++){
		if (cvEnvs[i].rows != cvEnvs[i].cols || i >= 1 && cvEnvs[i].rows != cvEnvs[i - 1].cols){
			return 0;
		}
	}
	int cubeSize = cvEnvs[0].rows;
	cv::Mat image = cv::Mat::zeros(cubeSize * 3, cubeSize * 4, cvEnvs[0].type());

	std::vector<cv::Mat> tempCvEnvs(6);
	tempCvEnvs[0] = cv::Mat(image, cv::Rect(cubeSize * 2, cubeSize * 1, cubeSize, cubeSize));
	tempCvEnvs[1] = cv::Mat(image, cv::Rect(cubeSize * 0, cubeSize * 1, cubeSize, cubeSize));
	tempCvEnvs[2] = cv::Mat(image, cv::Rect(cubeSize * 1, cubeSize * 0, cubeSize, cubeSize));
	tempCvEnvs[3] = cv::Mat(image, cv::Rect(cubeSize * 1, cubeSize * 2, cubeSize, cubeSize));
	tempCvEnvs[4] = cv::Mat(image, cv::Rect(cubeSize * 1, cubeSize * 1, cubeSize, cubeSize));
	tempCvEnvs[5] = cv::Mat(image, cv::Rect(cubeSize * 3, cubeSize * 1, cubeSize, cubeSize));

	for (int i = 0; i < 6; i++){
		cvEnvs[i].copyTo(tempCvEnvs[i]);
	}
	cv::imwrite(filename, image);
	return 0;
}


/// <summary>
/// Creates a cubemap with a single pixel has color (1, 1, 1) and other pixels black.
/// </summary>
/// <param name="face_index">The index of the face of the pixel.</param>
/// <param name="x">x coordinate of pixel</param>
/// <param name="y">y coordinate of pixel</param>
/// <param name="cubemap_length">The side length of cubemaps</param>
/// <param name="cubeface">The cubemap generated.</param>
void CreateCubemap(int face_index, int x, int y, int cubemap_length, vector<Mat>& cubeface){
	cubeface.resize(6);
	for (int i = 0; i < 6; i++){
		cubeface[i] = Mat(cubemap_length, cubemap_length, CV_32FC3, Vec3f(0,0,0));
	}
	cubeface[face_index].at<Vec3f>(y, x) = Vec3f(1, 1, 1);
}


/// <summary>
/// Converts a cubemap to a light probe environment map.
/// </summary>
/// <param name="llEnv">Output: the generated light probe map.</param>
/// <param name="cvEnvs">6 faces of cubemap.</param>
/// <param name="targetSize">Size of the target light probe image.</param>
/// <returns>status</returns>
int ConvertCubemapToLightProbe(cv::Mat& light_probe, const std::vector<cv::Mat>& cubemap, const cv::Size light_probe_size)
{
	if (cubemap.size() != 6){
		printf("incorrect cube map\n");
		return -1;
	}
	int cubSize = cubemap[0].cols;

	if (light_probe_size == cv::Size(0, 0)){
		light_probe = cv::Mat::zeros(4 * cubSize, 8 * cubSize, CV_32FC3);
	}
	else{
		light_probe = cv::Mat::zeros(light_probe_size, CV_32FC3);
	}

	for (int x = 0; x < light_probe.cols; x++)
	{
		for (int y = 0; y < light_probe.rows; y++)
		{

			float u = ((float)x + 0.5f) / (float)light_probe.cols;
			float v = ((float)y + 0.5f) / (float)light_probe.rows;

			float theta = v * CV_PI;
			float phi = u * 2.0 * CV_PI;
			float pi_4 = CV_PI / 4.0f;
			float pi_2 = CV_PI / 2.0f;

			cv::Vec3f direction;
			direction[1] = cos(theta);
			direction[0] = sin(phi) * sin(theta);
			direction[2] = -cos(phi) * sin(theta);
			int face = -1;
			float uc, vc;
			if (phi >= pi_4 && phi < pi_4 * 3){

				face = 1;
				direction /= direction[0];
			}
			else if (phi >= pi_4 * 3 && phi < pi_4 * 5){
				face = 4;
				direction /= direction[2];
			}
			else if (phi >= pi_4 * 5 && phi < pi_4 * 7){
				face = 0;
				direction /= -direction[0];
			}
			else{
				face = 5;
				direction /= -direction[2];
			}

			if (direction[1] > 1.0){
				direction /= direction[1];
				face = 2;
			}
			else if (direction[1] < -1.0){
				direction /= -direction[1];
				face = 3;
			}


			if (face == 0){
				uc = -direction[2];
				vc = -direction[1];
			}
			else if (face == 1){
				uc = direction[2];
				vc = -direction[1];
			}
			else if (face == 2){
				direction /= direction[1];
				uc = -direction[0];
				vc = direction[2];
			}
			else if (face == 3){
				direction /= -direction[1];
				uc = -direction[0];
				vc = -direction[2];
			}
			else if (face == 4){
				direction /= direction[2];
				uc = -direction[0];
				vc = -direction[1];
			}
			else if (face == 5){
				direction /= -direction[2];
				uc = direction[0];
				vc = -direction[1];
			}
			else{
				printf("wrong face\n");
				return -1;
			}

			uc = (uc + 1.f) / 2.f;
			vc = (vc + 1.f) / 2.f;

			float xc = uc * (cubSize - 1);
			float yc = vc * (cubSize - 1);

			cv::Vec3f c0;
			subPixelF(c0, cubemap[face], xc, yc);
			
			light_probe.at<cv::Vec3f>(y, x) = c0;
		}
	}
	return 0;
}


/// <summary>
/// Subsample the pixel f.
/// </summary>
/// <param name="color">Output: the subsampled color.</param>
/// <param name="image">Input image</param>
/// <param name="x">The x coord of pixel.</param>
/// <param name="y">The y coord of pixel.</param>
/// <returns>status</returns>
int subPixelF(cv::Vec3f& color, const cv::Mat& image, float x, float y)
{
	if (x < 0 || y < 0 || x >= image.cols || y >= image.rows){
		return -1;
	}

	int ix = (int)x;
	int iy = (int)y;

	int ix_1 = ix + 1;
	int iy_1 = iy + 1;
	if (ix == image.cols - 1){
		ix_1 = ix;
	}
	if (iy == image.rows - 1){
		iy_1 = iy;
	}

	float rx = x - (float)ix;
	float ry = y - (float)iy;

	const cv::Vec3f& cxy = image.at<cv::Vec3f>(iy, ix);
	const cv::Vec3f& cx_1y = image.at<cv::Vec3f>(iy, ix_1);
	const cv::Vec3f& cx_1y_1 = image.at<cv::Vec3f>(iy_1, ix_1);
	const cv::Vec3f& cxy_1 = image.at<cv::Vec3f>(iy_1, ix);

	cv::Vec3f cx = (1.0f - ry) * cxy + ry * cxy_1;
	cv::Vec3f cx_1 = (1.0f - ry) * cx_1y + ry * cx_1y_1;

	color = (1.0f - rx) * cx + rx * cx_1;

	return 0;
}


float GetSHValue(const Vec3f& dir, int sh_index){
	float value;
	switch (sh_index){
	case 0: value = 0.282095; break;
	case 1: value = 0.488603 * dir[1]; break;
	case 2: value = 0.488603 * dir[2]; break;
	case 3: value = 0.488603 * dir[0]; break;
	case 4: value = 1.092548 * dir[0] * dir[1]; break;
	case 5: value = 1.092548 * dir[1] * dir[2]; break;
	case 6: value = 0.315392 * (3 * dir[2] * dir[2] - 1); break;
	case 7: value = 1.092548 * dir[0] * dir[2]; break;
	case 8: value = 0.546274 * (dir[0] * dir[0] - dir[1] * dir[1]); break;
	default: exit(-1);  break;
	}
	return value;
}

void CreateSHCubemap(int cubemap_length, int sh_index, vector<Mat>& cubeface){
	cubeface.resize(6);
	for (int i = 0; i < 6; i++){
		cubeface[i] = Mat(cubemap_length, cubemap_length, CV_32FC3, Vec3f(0, 0, 0));
	}

	for (int i = 0; i < 6; i++){
		for (int r = 0; r < cubemap_length; r++){
			for (int c = 0; c < cubemap_length; c++){
				Vec3f dir = CubeLightDirection((c + 0.5) / (float)cubemap_length, (r + 0.5) / (float)cubemap_length, i);
				float sh_value = GetSHValue(dir, sh_index);
				cubeface[i].at<Vec3f>(r, c) = Vec3f(sh_value, sh_value, sh_value);
			}
		}
	}
}


// Map cubemap coordinates to 3D coordinates
// Note: mitsuba's coordinate system is quite special, with x to the left, y to to the top, and z to the inside
cv::Vec3d CubeLightDirection(double u, double v, int face_index){
	double a = 1;
	double b = 2 * u - 1;
	double c = 2 * v - 1;
	Vec3d direction(0, 0, 0);
	switch (face_index){
	case 0: direction = Vec3d(-a, -c, -b); break;
	case 1: direction = Vec3d(a, -c, b); break;
	case 2: direction = Vec3d(-b, a, c); break;
	case 3: direction = Vec3d(-b, -a, -c); break;
	case 4: direction = Vec3d(-b, -c, a); break;
	case 5: direction = Vec3d(b, -c, -a); break;
	default:exit(-1);
	}
	direction = normalize(direction);
	// return -1.0 * direction;
	return direction;
}

void SaveSHBasisImage(int cubemap_length, int sh_index, const cv::Size& probe_size, const char* file_name){
	vector<Mat> cube_faces;
	CreateSHCubemap(cubemap_length, sh_index, cube_faces);
	Mat light_probe;
	ConvertCubemapToLightProbe(light_probe, cube_faces, probe_size);

	imwrite(file_name, light_probe);
	
	/*int width = probe_size.width;
	int height = probe_size.height;
	Mat probe_light(probe_size.height, probe_size.width, CV_32FC3);

	for (int i = 0; i < height; i++){
		for (int j = 0; j < width; j++){
			float theta = (i + 0.5) / (float)height * CV_PI;
			float phi = (j  + 0.5) / (float)width * 2 * CV_PI;
			Vec3f dir;
			dir[0] = sinf(theta) * cosf(phi);
			dir[1] = sinf(theta) * sinf(phi);
			dir[2] = cosf(theta);
			float value = GetSHValue(dir, sh_index);
			probe_light.at<Vec3f>(i, j) = Vec3f(value, value, value);
		}
	}
	imwrite(file_name, probe_light);*/

}