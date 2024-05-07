#include "sample_player.hpp"
#include "samples.h"


uint32_t sample_player::pos = 0;
int sample_player::sample_type = 0;

void sample_player::play(int type){
	sample_type = type;
	pos = 0;
}

int32_t sample_player::update(){
	if(pos <= assets_sample_lengths[sample_type]){
		pos++;
		return assets_samples[sample_type][pos];
	}
	else{
		return 0;
	}
}