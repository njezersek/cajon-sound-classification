#pragma once

#include <cstdint>

#define NN_INPUT_SIZE 64

namespace nn_classifier
{
	extern float *input_buffer_debug;

	void init();
	void preprocess(float *x, int x_size);
	int transform(float *x, int x_size);
}