#ifndef UTILS_H
#define UTILS_H
#include "common.h"


void CreateCubemap(int face_index, int x, int y, int cubemap_length, std::vector<cv::Mat>& cubeface);
int subPixelF(cv::Vec3f& color, const cv::Mat& image, float x, float y);
int convertToLLMap(cv::Mat& llEnv, const std::vector<cv::Mat>& cvEnvs, const cv::Size targetSize);

#endif