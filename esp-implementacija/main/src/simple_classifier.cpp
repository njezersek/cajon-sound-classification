#include "simple_classifier.hpp"


int simple_classifier::high_low_border = 7;
float simple_classifier::threshold = 0.05685306094966863;

float simple_classifier::transform(float *x, int x_size){
	float low_sum = 0;
	float high_sum = 0;
	for(int i = 0; i < x_size; i++){
		if(i < high_low_border){
			low_sum += x[i];
		}
		else{
			high_sum += x[i];
		}
	}

	return high_sum/low_sum - threshold;
}