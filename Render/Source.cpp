#include "render.h"
using namespace std;
using namespace cv;
using namespace pugi;

int main(int argc, char* argv[]){
	/*for (int i = 0; i < 1; i++){
		vector<Mat> cubemap;
		cubemap.resize(6);
		for (int j = 0; j < 6; j++){
			cubemap[j].create(16, 16, CV_32FC3);
		}

		cubemap[0] = Mat::Mat(Size(16, 16), CV_32FC3, Vec3f(1, 1, 1));

		Mat envmap;
		ConvertCubemapToLightProbe(envmap, cubemap, Size(256, 128));
		imwrite("my_env.exr", envmap);
	}
	return 0;*/

	string mitsuba_path = string(argv[1]);
	string scene_path = string(argv[2]);
	string target_dir = string(argv[3]);
	int cubemap_length = atoi(argv[4]);

	RenderCubemap(mitsuba_path, scene_path, target_dir, cubemap_length);
	//StartRendering();
	return 0;
}