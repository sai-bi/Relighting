#include "compress.h"
#include "windows.h"
using namespace std;
using namespace cv;
using namespace Eigen;

int main(int argc, char* argv[]){
	string cubeface_dir = "C:\\Users\\bisai\\Documents\\research\\prt\\data1";
	int cubemap_length = 2;
	int image_length = 512;

	CompressPerLightImages(cubeface_dir, cubemap_length, image_length);
	// DecompressPerLightImages(cubeface_dir, cubemap_length, image_length);
	return 0;
}