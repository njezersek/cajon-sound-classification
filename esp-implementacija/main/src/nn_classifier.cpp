#include "nn_classifier.hpp"

#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "esp_dsp.h"

#include "model_data.h"



const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;

float* nn_classifier::input_buffer_debug = nullptr;

constexpr int kTensorArenaSize = 1024*4;
uint8_t tensor_arena[kTensorArenaSize];

void nn_classifier::init() {
	input_buffer_debug = (float *)malloc(NN_INPUT_SIZE * sizeof(float));

	model = tflite::GetModel(model_tflite);
	if (model->version() != TFLITE_SCHEMA_VERSION) {
		MicroPrintf("Model provided is schema version %d not equal to supported "
                "version %d.", model->version(), TFLITE_SCHEMA_VERSION);
		return;
	}

	// Specify the operations needed to run the model
	static tflite::MicroMutableOpResolver<11> resolver;
	resolver.AddFullyConnected();
	resolver.AddMul();
	resolver.AddAdd();
	resolver.AddRelu();
	resolver.AddLogistic();
	resolver.AddSoftmax();
	resolver.AddReshape();
	resolver.AddQuantize();
	resolver.AddDequantize();
	resolver.AddConv2D();
	resolver.AddMaxPool2D();

	// Build an interpreter to run the model with
	static tflite::MicroInterpreter static_interpreter(
		model, resolver, tensor_arena, kTensorArenaSize
	);
	interpreter = &static_interpreter;

	if(interpreter->AllocateTensors() != kTfLiteOk) {
		MicroPrintf("AllocateTensors() failed");
		return;
	}

	// Get pointers for the model's input and output tensors
	input = interpreter->input(0);
	output = interpreter->output(0);

	MicroPrintf("input: scale = %f, zero_point = %d\n", input->params.scale, input->params.zero_point);
	MicroPrintf("output: scale = %f, zero_point = %d\n", output->params.scale, output->params.zero_point);
}

/*
 * Preprocess the input data (in place). This function is called by the `transform` function.
 * 
 * @param x: pointer to the input data (result of FFT with window size 256)
 * @param x_size: size of the input data
*/
void nn_classifier::preprocess(float *x, int x_size){
	// normalize input data
	float max = -1;
	for(int i=0; i<x_size; i++){
		if(abs(x[i]) > max){
			max = abs(x[i]);
		}
	}
	for(int i=0; i<x_size; i++){
		x[i] /= max;
	}
}


/*
 * Run the neural network on the input data
 * 
 * @param x: pointer to the input data (result of FFT with window size 256)
 * @param x_size: size of the input data
 * @return the output of the neural network
*/
int nn_classifier::transform(float *x, int x_size){
	preprocess(x, x_size);

	// copy input data into input tensor
	for(int i=0; i<x_size; i++){
		input->data.f[i] = x[i];
		// input->data.int8[i] = x[i] * input->params.scale + input->params.zero_point;

		// input_buffer_debug[i] = input->data.int8[i];
		input_buffer_debug[i] = x[i];
	}

	unsigned int start = dsp_get_cpu_cycle_count();
	if(interpreter->Invoke() != kTfLiteOk) {
		MicroPrintf("Invoke() failed");
		return 0;
	}
	unsigned int end = dsp_get_cpu_cycle_count();

	float y = output->data.f[0];
	// float y = (output->data.int8[0] - output->params.zero_point) * output->params.scale;
	int out = y > 0.5 ? 1 : 0;

	// MicroPrintf("y = %f, out: %d, cycles: %i", y, out, end-start);

	return out;
}