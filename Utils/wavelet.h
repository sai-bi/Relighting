#ifndef WAVELET_H
#define WAVELET_H
#include "common.h"

class WaveletCompressor
{
public:
	WaveletCompressor(int dim = 0);
	void Compress(const Eigen::MatrixXf& dataIn, Eigen::MatrixXf& dataOut);
	void Decompress(const Eigen::MatrixXf& dataIn, Eigen::MatrixXf& dataOut);

	Eigen::MatrixXf areaWeights;

private:
	int dimension;
	Eigen::MatrixXf xform;
	Eigen::MatrixXf inverse;

	// Computing the wavelet transform
	void Iterate(int iter, Eigen::MatrixXf& mat);

	// Computing wavelet area weights
	void Diagonal(int imin, int jmin, int imax, int jmax, float weight);
	void Horizontal(int imin, int jmin, int imax, int jmax, float weight);
	void Vertical(int imin, int jmin, int imax, int jmax, float weight);
};


inline void NormalizeOrthogonal(Eigen::MatrixXf& mat);

inline int Log2(int num)
{
	int count = 0;
	for (; num != 1; count++, num >>= 1);
	return count;
}

#endif