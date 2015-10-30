#ifndef COMPRESS_H
#define COMPRESS_H
#include "Utils\common.h"
#include "Utils\wavelet.h"
#include "Utils\utils.h"
// typedef Eigen::SparseMatrix<double> SpMat;

void LoadPerLightImages(int cubemap_length, int cubeface_index, 
	std::string cubeface_dir, std::string postfix,
	cv::Mat_<cv::Vec3f>* per_light_images);

void ExtractPerPixelImages(cv::Mat_<cv::Vec3f>* per_light_images, int cubemap_length,
	Eigen::MatrixXf** per_pixel_images, int image_length);

void ExtractPerLightImages(Eigen::MatrixXf** per_pixel_images, int image_length,
	cv::Mat_<cv::Vec3f>* per_light_images, int cubemap_length);

void SaveCompressedImages(cv::Mat_<cv::Vec3f>* per_light_images, int cubemap_length, std::string target_dir, std::string postfix);

void CompressPerLightImages(std::string cubeface_dir, int cubemap_length, int image_length);

void DecompressPerLightImages(std::string cubeface_dir, int cubemap_length, int image_length);

template<class T> 
T** AllocPointerArray(int rows, int cols);

template<class T>
void FreePointerArray(int rows, T** mat);

#endif