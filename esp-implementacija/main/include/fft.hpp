#pragma once


#define FFT_WINDOW_SIZE 64 // Amount of real input samples

namespace fft{
	extern float x[FFT_WINDOW_SIZE];

	void init();
	void transform(bool apply_hann_window = true);
	void print_last_result();
	void demo();
}