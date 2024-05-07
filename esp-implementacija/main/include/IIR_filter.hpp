#pragma once

class IIR_filter{
private:

public:
	IIR_filter(int order, float *a, float *b);
	~IIR_filter();
};