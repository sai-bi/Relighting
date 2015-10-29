#include "compress.h"
#include "wavelet.h"
using namespace std;
using namespace Eigen;
using namespace cv;

/// <summary>
///		Initialize a 2D dynamic array of type T
/// </summary>
/// <param name="rows">The rows.</param>
/// <param name="cols">The cols.</param>
/// <returns>2D dynamic array of type T.</returns>
template<class T>
T** AllocPointerArray(int rows, int cols){
	T** mat = new T*[rows];
	for (int i = 0; i < rows; i++){
		mat[i] = new T[cols];
	}
	return mat;
}

/// <summary>
///		Frees dynamic 2D array.
/// </summary>
/// <param name="rows">The rows.</param>
/// <param name="cols">The cols.</param>
/// <param name="mat">The 2d matrix to free.</param>
template<class T>
void FreePointerArray(int rows, T** mat){
	for (int i = 0; i < rows; i++){
		delete[] mat[i];
	}
	delete[] mat;
}

// Load the scenes rendered by each cubemap pixels.
// Memory must be allocated before calling this function.
// 
void LoadPerLightImages(int cubemap_length, int cubeface_index,
	std::string cubeface_dir, std::string postfix,
	cv::Mat_<Vec3f>* per_light_images){
	string face_image_dir = cubeface_dir + "\\face_" + to_string(cubeface_index);

	int count = 0;
	int image_num = cubemap_length * cubemap_length;
	for (int i = 0; i < cubemap_length; i++){
		for (int j = 0; j < cubemap_length; j++){
			string image_path = face_image_dir + "\\" + to_string(count) + postfix + ".exr";
			per_light_images[count] = imread(image_path, cv::IMREAD_ANYCOLOR | cv::IMREAD_ANYDEPTH);

			UpdateProgressBar(count, image_num);
			count++;
		}
	}
}

/// <summary>
///		Extracts the per pixel images from per light images.
/// </summary>
/// <param name="per_light_images">
///		The scene images rendered by each cubemap pixel.
/// </param>
/// <param name="cubemap_length">
///		Side length of cubemap.
///	</param>
/// <param name="per_pixel_images">
///		Output: the cubemap from the view of each image pixel. Memory must be allocated before calling.
///		It is N * 3 array, where N is the number of pixels in rendered images. 3 corresponds to 3 channels.
/// </param>
/// <param name="image_length">
///		The length of rendered images.
///	</param>
void ExtractPerPixelImages(cv::Mat_<Vec3f>* per_light_images, int cubemap_length,
	Eigen::MatrixXf** per_pixel_images, int image_length){
	int pixel_number = image_length * image_length;
	for (int i = 0; i < pixel_number; i++){
		per_pixel_images[i][0].resize(cubemap_length, cubemap_length);
		per_pixel_images[i][1].resize(cubemap_length, cubemap_length);
		per_pixel_images[i][2].resize(cubemap_length, cubemap_length);
	
		int count = 0;
		// iterate row by row
		for (int r = 0; r < cubemap_length; r++){
			for (int c = 0; c < cubemap_length; c++){
				per_pixel_images[i][0](r, c) = per_light_images[count](i)[0];
				per_pixel_images[i][1](r, c) = per_light_images[count](i)[1];
				per_pixel_images[i][2](r, c) = per_light_images[count](i)[2];
				count++;
			}
		}
	}
}


/// <summary>
///		Extracts the per light images from per pixel images.
/// </summary>
/// <param name="per_pixel_images">
///		Cubemap images from the view of rendered image pixels.
/// </param>
/// <param name="image_length">
///		The rendered image length.
/// </param>
/// <param name="per_light_images">
///		The scene images rendered by each cubemap pixel.
///	</param>
/// <param name="cubemap_length"> 
///		The cubemap_length.
/// </param>
void ExtractPerLightImages(Eigen::MatrixXf** per_pixel_images, int image_length,
	cv::Mat_<Vec3f>* per_light_images, int cubemap_length){
	int count = 0;
	int pixel_number = image_length * image_length;
	for (int r = 0; r < cubemap_length; r++){
		for (int c = 0; c < cubemap_length; c++){
			per_light_images[count].create(image_length, image_length);
			for (int k = 0; k < pixel_number; k++){
				per_light_images[count](k)[0] = per_pixel_images[k][0](r, c);
				per_light_images[count](k)[1] = per_pixel_images[k][1](r, c);
				per_light_images[count](k)[2] = per_pixel_images[k][2](r, c);
			}
			count++;
		}
	}
}


/// <summary>
/// Save the wavelet-tranformed images rendered by each cubemap pixel. In 'exr' file format.
/// </summary>
/// <param name="per_light_images">The per_light_images.</param>
/// <param name="cubemap_length">The cubemap_length.</param>
/// <param name="target_dir">The target_dir.</param>
void SaveCompressedImages(cv::Mat_<cv::Vec3f>* per_light_images, int cubemap_length,
	std::string target_dir, std::string postfix){
	int total = cubemap_length * cubemap_length;
	ofstream fout;
	for (int i = 0; i < total; i++){
		string output_path = target_dir + "\\" + to_string(i) + postfix + ".exr";
		cv::imwrite(output_path, per_light_images[i]);
		UpdateProgressBar(i, total);
	} 
}


/// <summary>
/// Apply wavelet transform to images rendered by cubemap.
/// </summary>
/// <param name="cubeface_dir">The directory that stores the rendering result of 6 faces.</param>
/// <param name="cubemap_length">The side length of cubemap.</param>
/// <param name="image_length">The side length of rendered images.</param>
void CompressPerLightImages(std::string cubeface_dir, int cubemap_length, int image_length){
	int cubemap_pixel_num = cubemap_length * cubemap_length;
	int image_pixel_num = image_length * image_length;
	WaveletCompressor comp(cubemap_length);
	for (int i = 0; i < 6; i++){
		printf("Compressing face %d...\n", i);
		Mat_<Vec3f>* per_light_images = new Mat_<Vec3f>[image_pixel_num];
		printf("Load per light images...\n");
		LoadPerLightImages(cubemap_length, i, cubeface_dir, "", per_light_images);
		
		MatrixXf** per_pixel_images = AllocPointerArray<MatrixXf>(image_pixel_num, 3);
		printf("Extract per pixel images...\n");
		ExtractPerPixelImages(per_light_images, cubemap_length, per_pixel_images, image_length);
		printf("Compress per pixel images...\n");
		for (int j = 0; j < image_pixel_num; j++){
			comp.Compress(per_pixel_images[j][0], per_pixel_images[j][0]);
			comp.Compress(per_pixel_images[j][1], per_pixel_images[j][1]);
			comp.Compress(per_pixel_images[j][2], per_pixel_images[j][2]);
		}

		printf("Extract per light images...\n");
		Mat_<Vec3f>* compressed_per_light_images = new Mat_<Vec3f>[cubemap_pixel_num];
		ExtractPerLightImages(per_pixel_images, image_length, compressed_per_light_images, cubemap_length);

		printf("Save compressed per pixel images...\n");
		string target_dir = cubeface_dir + "\\face_" + to_string(i);
		SaveCompressedImages(compressed_per_light_images, cubemap_length, target_dir, "-comp");

		delete[] per_light_images;
		FreePointerArray(image_pixel_num, per_pixel_images);
		delete[] compressed_per_light_images;
	}
}

// Used to validate the compress function
void DecompressPerLightImages(std::string cubeface_dir, int cubemap_length, int image_length){
	int cubemap_pixel_num = cubemap_length * cubemap_length;
	int image_pixel_num = image_length * image_length;
	WaveletCompressor comp(cubemap_length);
	for (int i = 0; i < 6; i++){
		printf("Compressing face %d...\n", i);
		Mat_<Vec3f>* per_light_images = new Mat_<Vec3f>[image_pixel_num];
		printf("Load per light images...\n");
		LoadPerLightImages(cubemap_length, i, cubeface_dir, "-comp", per_light_images);

		MatrixXf** per_pixel_images = AllocPointerArray<MatrixXf>(image_pixel_num, 3);
		printf("Extract per pixel images...\n");
		ExtractPerPixelImages(per_light_images, cubemap_length, per_pixel_images, image_length);
		printf("Decompress per pixel images...\n");
		for (int j = 0; j < image_pixel_num; j++){
			comp.Decompress(per_pixel_images[j][0], per_pixel_images[j][0]);
			comp.Decompress(per_pixel_images[j][1], per_pixel_images[j][1]);
			comp.Decompress(per_pixel_images[j][2], per_pixel_images[j][2]);
		}

		printf("Extract per light images...\n");
		Mat_<Vec3f>* compressed_per_light_images = new Mat_<Vec3f>[cubemap_pixel_num];
		ExtractPerLightImages(per_pixel_images, image_length, compressed_per_light_images, cubemap_length);

		printf("Save compressed per pixel images...\n");
		string target_dir = cubeface_dir + "\\face_" + to_string(i);
		SaveCompressedImages(compressed_per_light_images, cubemap_length, target_dir, "-valid");

		delete[] per_light_images;
		FreePointerArray(image_pixel_num, per_pixel_images);
		delete[] compressed_per_light_images;
	}
}




