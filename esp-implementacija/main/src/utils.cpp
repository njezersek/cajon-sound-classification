
#include "utils.hpp"

#include <math.h>
#include <stdio.h>

void print_wave(float* wave, int len, int height, float min_val, float max_val){
    for(int j=height; j>0; j--){
        printf("|");
        for(int i=0; i<len; i++){
            int val = (int)((wave[i] - min_val) / (max_val - min_val) * height);
            if(val >= j){
                printf("*");
            }
            else{
                printf(" ");
            }
        }
        printf("|\n");
    }
    
    // add column numbers
    for(int digit=2; digit>=0; digit--){
        printf(" ");
        for(int i=0; i<len; i++){
            int d = (i / (int)pow(10, digit)) % 10;
            printf("%d", d);
        }
        printf(" \n");
    }
}