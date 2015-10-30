#include "wavelet.h"
using namespace std;
using namespace Eigen;

WaveletCompressor::WaveletCompressor(int dim)
	: dimension(dim){
	xform.resize(dim, dim);
	inverse.resize(dim, dim);
	areaWeights.resize(dim, dim);

	// Construct normalized Haar wavelet transform (and its
	// inverse/transpose) in the specified dimension
	for (int i = 0; i < dimension; i++){
		for (int j = 0; j < dimension; j++){
			xform(i, j) = (i == j ? 1.0f : 0.0f);
		}
	}
	int iters = Log2(dim);
	for (int i = 0; i < iters; i++){
		Iterate(i, xform);
	}
	NormalizeOrthogonal(xform);

	if (!xform.isUnitary()){
		cout << "Haar wavelet transform is not properly normalized." << endl;
		exit(1);
	}
	inverse = xform.transpose();

	// Compute wavelet area weights
	Diagonal(0, 0, dim, dim, 1);
	// Normalize them
	areaWeights *= (1.0f / (dim*dim));
}

void WaveletCompressor::Compress(const MatrixXf& dataIn, MatrixXf& dataOut){
	dataOut = inverse * dataIn * xform;
}

void WaveletCompressor::Decompress(const MatrixXf& dataIn, MatrixXf& dataOut){
	dataOut = xform * dataIn * inverse;
}

void WaveletCompressor::Iterate(int iter, MatrixXf& mat){
	MatrixXf next(dimension, dimension);
	// Set up identity matrix
	for (int i = 0; i < dimension; i++){
		for (int j = 0; j < dimension; j++){
			next(i, j) = (i == j ? 1.0f : 0.0f);
		}
	}
	// Set up Haar transform step
	int partdim = (iter > 0 ? dimension / (2 << (iter - 1)) : dimension);
	int pdby2 = partdim / 2;
	// First, the averaging
	for (int i = 0; i < partdim; i++){
		for (int j = 0; j < pdby2; j++){
			if (i == 2 * j || i == 2 * j + 1)
				next(i, j) = 0.5f;
			else next(i, j) = 0.0f;
		}
	}
	// Then, the differencing
	for (int i = 0; i < partdim; i++){
		for (int j = 0; j < pdby2; j++){
			if (i == 2 * j)
				next(i, j + pdby2) = 0.5f;
			else if (i == 2 * j + 1)
				next(i, j + pdby2) = -0.5f;
			else next(i, j + pdby2) = 0.0f;
		}
	}
	// Then accumulate
	mat = mat * next;
}

void WaveletCompressor::Diagonal(int imin, int jmin, int imax, int jmax, float weight){
	// Weight the lower-right corner
	for (int i = (imax + imin) / 2; i < imax; i++){
		for (int j = (jmax + jmin) / 2; j < jmax; j++){
			areaWeights(i, j) = weight;
		}
	}

	// Base case
	if (imin == imax && jmin == jmax) return;

	// Recurse on the other corners
	Horizontal(imin, (jmax + jmin) / 2, (imax + imin) / 2, jmax, weight * 2);
	Vertical((imax + imin) / 2, jmin, imax, (jmax + jmin) / 2, weight * 2);
	Diagonal(imin, jmin, (imax + imin) / 2, (jmax + jmin) / 2, weight * 4);
}

void WaveletCompressor::Horizontal(int imin, int jmin, int imax, int jmax, float weight)
{
	// Weight the bottom
	for (int i = (imax + imin) / 2; i < imax; i++){
		for (int j = jmin; j < jmax; j++)
		{
			areaWeights(i, j) = weight;
		}
	}

	// Base case
	if (imin == imax) return;

	// Recurse
	Horizontal(imin, jmin, (imax + imin) / 2, jmax, weight * 2);
}

void WaveletCompressor::Vertical(int imin, int jmin, int imax, int jmax, float weight){
	// Weight the right side
	for (int i = imin; i < imax; i++){
		for (int j = (jmin + jmax) / 2; j < jmax; j++){
			areaWeights(i, j) = weight;
		}
	}

	// Base case
	if (jmin == jmax) return;

	// Recurse
	Vertical(imin, jmin, imax, (jmin + jmax) / 2, weight * 2);
}


inline void NormalizeOrthogonal(Eigen::MatrixXf& mat){
	for (int i = 0; i < mat.cols(); i++){
		mat.col(i).normalize();
	}
}

