#include "fft.hpp"

#include "esp_check.h"
#include "esp_system.h"
#include "esp_dsp.h"
#include "esp_err.h"


#include "utils.hpp"
#include <math.h>
#include "nn_classifier.hpp"

// Input test array
__attribute__((aligned(16)))
float fft::x[FFT_WINDOW_SIZE];

// Window coefficients
__attribute__((aligned(16)))
float wind[FFT_WINDOW_SIZE];

esp_err_t ret;


void fft::init(){
	ret = dsps_fft2r_init_fc32(NULL, FFT_WINDOW_SIZE/2);
	if (ret  != ESP_OK){
		ESP_LOGE("FFT", "Not possible to initialize FFT2R. Error = %i", ret);
		return;
	}
	ret = dsps_fft4r_init_fc32(NULL, FFT_WINDOW_SIZE/2);
	if (ret  != ESP_OK){
		ESP_LOGE("FFT", "Not possible to initialize FFT4R. Error = %i", ret);
		return;
	}

	dsps_wind_hann_f32(wind, FFT_WINDOW_SIZE);
}

/*
	Debug function to print the last result of the FFT stored in `x`
*/
void fft::print_last_result(){
	print_wave(x, FFT_WINDOW_SIZE/2, 20, 0, 1);
	printf("\n[");
	for(int i=0; i<FFT_WINDOW_SIZE/2; i++){
		printf("%f, ", x[i]);
	}
	printf("]\n");
}

/*
	Compute the FFT of the input data stored in `x`

	@param apply_hann_window: whether to apply a Hann window to the input data
*/
void fft::transform(bool apply_hann_window){
	if(apply_hann_window){
		dsps_mul_f32(x, wind, x, FFT_WINDOW_SIZE, 1, 1, 1);
	}
	// FFT Radix-2
	dsps_fft2r_fc32(x, FFT_WINDOW_SIZE/2);
	// Bit reverse 
	dsps_bit_rev2r_fc32(x, FFT_WINDOW_SIZE/2);
	// Convert one complex vector with length N/2 to one real spectrum vector with length N/2
	dsps_cplx2real_fc32(x, FFT_WINDOW_SIZE/2);

	for (int i = 0 ; i < FFT_WINDOW_SIZE/2 ; i++) {
		x[i] = sqrt(x[i * 2 + 0] * x[i * 2 + 0] + x[i * 2 + 1] * x[i * 2 + 1]) / FFT_WINDOW_SIZE * 2;
	}
}