#include "utils.h"
using namespace std;
using namespace cv;

void CreateCubemap(int face_index, int x, int y, int cubemap_length, vector<Mat>& cubeface){
	cubeface.resize(6);
	for (int i = 0; i < 6; i++){
		cubeface[i].create(cubemap_length, cubemap_length, CV_32FC3);
	}
	cubeface[face_index].at<Vec3f>(y, x) = Vec3f(1, 1, 1);
}


int convertToLLMap(cv::Mat& llEnv, const std::vector<cv::Mat>& cvEnvs, const cv::Size targetSize)
{
	if (cvEnvs.size() != 6)
	{
		printf("incorrect cube map\n");
		return -1;
	}
	int cubSize = cvEnvs[0].cols;

	if (targetSize == cv::Size(0, 0))
		llEnv = cv::Mat::zeros(4 * cubSize, 8 * cubSize, CV_32FC3);
	else
		llEnv = cv::Mat::zeros(targetSize, CV_32FC3);

	for (int x = 0; x < llEnv.cols; x++)
	{
		for (int y = 0; y < llEnv.rows; y++)
		{

			float u = ((float)x + 0.5f) / (float)llEnv.cols;
			float v = ((float)y + 0.5f) / (float)llEnv.rows;

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
			if (phi >= pi_4 && phi < pi_4 * 3)
			{

				face = 1;
				direction /= direction[0];
			}
			else if (phi >= pi_4 * 3 && phi < pi_4 * 5)
			{
				face = 4;
				direction /= direction[2];
			}
			else if (phi >= pi_4 * 5 && phi < pi_4 * 7)
			{
				face = 0;
				direction /= -direction[0];
			}
			else
			{
				face = 5;
				direction /= -direction[2];
			}

			if (direction[1] > 1.0)
			{
				direction /= direction[1];
				face = 2;
			}
			else if (direction[1] < -1.0)
			{
				direction /= -direction[1];
				face = 3;
			}


			if (face == 0)
			{
				//direction /= -direction[0];
				uc = -direction[2];
				vc = -direction[1];
			}
			else if (face == 1)
			{
				//direction /= direction[0];
				uc = direction[2];
				vc = -direction[1];
			}
			else if (face == 2)
			{
				direction /= direction[1];
				uc = -direction[0];
				vc = direction[2];
			}
			else if (face == 3)
			{
				direction /= -direction[1];
				uc = -direction[0];
				vc = -direction[2];
			}
			else if (face == 4)
			{
				direction /= direction[2];
				uc = -direction[0];
				vc = -direction[1];
			}
			else if (face == 5)
			{
				direction /= -direction[2];
				uc = direction[0];
				vc = -direction[1];
			}
			else
			{
				printf("wrong face\n");
				return -1;
			}

			uc = (uc + 1.f) / 2.f;
			vc = (vc + 1.f) / 2.f;

			float xc = uc * (cubSize - 1);
			float yc = vc * (cubSize - 1);


			cv::Vec3f c0;

			subPixelF(c0, cvEnvs[face], xc, yc);

			llEnv.at<cv::Vec3f>(y, x) = c0;
		}
	}
	return 0;
}


int subPixelF(cv::Vec3f& color, const cv::Mat& image, float x, float y)
{
	if (x < 0 || y < 0 || x >= image.cols || y >= image.rows)
		return -1;

	int ix = (int)x;
	int iy = (int)y;

	int ix_1 = ix + 1;
	int iy_1 = iy + 1;
	if (ix == image.cols - 1)
		ix_1 = ix;
	if (iy == image.rows - 1)
		iy_1 = iy;

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