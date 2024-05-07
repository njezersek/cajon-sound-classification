#pragma once

#define CLAMP(x, a, b) ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))


void print_wave(float* wave, int len, int height, float min_val = 1, float max_val = -1);