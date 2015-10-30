#ifndef UTILS_H
#define UTILS_H
#include "common.h"


void CreateCubemap(int face_index, int x, int y, int cubemap_length, std::vector<cv::Mat>& cubeface);
int subPixelF(cv::Vec3f& color, const cv::Mat& image, float x, float y);
int ConvertCubemapToLightProbe(cv::Mat& light_probe, const std::vector<cv::Mat>& cubemap, const cv::Size light_probe_size);
inline void UpdateProgressBar(int curr, int total, int step_size);



#endif