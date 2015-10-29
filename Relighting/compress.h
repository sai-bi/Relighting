#ifndef COMPRESS_H
#define COMPRESS_H
#include "common.h"

typedef Eigen::SparseMatrix<double> SpMat;

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


inline void UpdateProgressBar(int curr, int total){
	int divide = total / 25 + 2;
	curr = curr + 1;
	if (curr != total && curr % divide != 1){
		return;
	}
	double progress = curr / (double)(total - 1);
	int barWidth = 70;
	std::cout << "[";
	int pos = (int)(barWidth * progress);
	for (int i = 0; i < barWidth; ++i) {
		if (i < pos) std::cout << "=";
		else if (i == pos) std::cout << ">";
		else std::cout << " ";
	}
	if (curr != total){
		std::cout << "] " << int(progress * 100.0) << " %\r";
		std::cout.flush();
	}
	else{
		std::cout << "] " << int(progress * 100.0) << " %" << std::endl;
	}	
}

#endif