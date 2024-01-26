/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2023 Nokia
 */

#include <odp_api.h>
#include <png.h> /*Must be at least v1.6.0*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "model_read.h"

/**
 * About model candy.onnx used in this example.
 *
 * The model stylizes of the input image.
 *
 * Model info:
 * Inputs: name: inputImage, type: float32, shape: [None, 3, 720, 720]
 * Outputs: name: outputImage, type: float32, shape: [None, 3, 720, 720]
 *
 * The input image must be in png format of size 720 * 720.
 *
 * The model can be downloaded from
 * https://github.com/microsoft/Windows-Machine-Learning/tree/master/Samples/
 * FNSCandyStyleTransfer/UWP/cs/Assets
 *
 * Refer https://github.com/jcjohnson/fast-neural-style for more information
 * about the model.
 */

#define WIDTH 720
#define HEIGHT 720
#define NUM_CHANNELS 3
#define NUM_PIXELS (720 * 720 * 3)
#define BATCH_SIZE 1
#define MAX_BATCH_SIZE 4
#define NUM_INPUT_SEGMENTS 3
#define MAX_MODEL_SIZE 2500000

static inline void hwc_to_chw(const png_byte *input, float *output)
{
	size_t stride = WIDTH * HEIGHT;

	for (size_t i = 0; i != stride; ++i) {
		for (size_t c = 0; c != NUM_CHANNELS; ++c)
			output[c * stride + i] = input[i * NUM_CHANNELS + c];
	}
}

static void chw_to_hwc(const float *input, png_bytep output)
{
	size_t t;
	float pixel;
	size_t stride = WIDTH * HEIGHT;

	for (int c = 0; c != NUM_CHANNELS; ++c) {
		t = c * stride;
		for (size_t i = 0; i != stride; ++i) {
			pixel = input[t + i];
			if (pixel < 0.f || pixel > 255.0f)
				pixel = 0;
			output[i * NUM_CHANNELS + c] = (png_byte)pixel;
		}
	}
}

static int read_png_file(const char *input_file, float *model_input)
{
	png_image image;
	png_bytep hwc_buffer;

	memset(&image, 0, sizeof(image));
	image.version = PNG_IMAGE_VERSION;
	if (png_image_begin_read_from_file(&image, input_file) == 0) {
		printf("png_image_begin_read_from_file %s failed\n", input_file);
		return -1;
	}

	/* Make sure that the image is of size 720 * 720 * 3 */
	image.format = PNG_FORMAT_BGR;
	if (PNG_IMAGE_SIZE(image) != NUM_PIXELS) {
		printf("Wrong image size:%d which should be %d\n",
		       PNG_IMAGE_SIZE(image), NUM_PIXELS);
		return -1;
	}

	if (image.height != HEIGHT || image.width != WIDTH) {
		printf("Wrong image height or width (both should be 720)\n");
		return -1;
	}

	/* Read the image data */
	hwc_buffer = (png_bytep)malloc(NUM_PIXELS);
	if (!hwc_buffer) {
		printf("Failed to allocate memory for hwc_buffer\n");
		return -1;
	}
	memset(hwc_buffer, 0, NUM_PIXELS);

	/* png images are typically read as channel last order */
	if (png_image_finish_read(&image, NULL, hwc_buffer, 0, NULL) == 0) {
		printf("Read from '%s' failed\n", input_file);
		free(hwc_buffer);
		return -1;
	}

	/* Channel last to channel first format */
	hwc_to_chw(hwc_buffer, model_input);

	free(hwc_buffer);
	return 0;
}

static int write_to_png(const char *output_file, float *model_output)
{
	png_bytep hwc_buffer;
	png_image image;
	int ret = 0;

	memset(&image, 0, sizeof(image));
	image.version = PNG_IMAGE_VERSION;
	image.format = PNG_FORMAT_BGR;
	image.height = HEIGHT;
	image.width = WIDTH;

	hwc_buffer = (png_bytep)malloc(NUM_PIXELS);
	if (!hwc_buffer) {
		printf("Allocating memory for hwc_buffer failed\n");
		return -1;
	}

	/* Channel first to channel last format */
	chw_to_hwc(model_output, hwc_buffer);

	if (!png_image_write_to_file(&image, output_file, 0, hwc_buffer, 0, NULL)) {
		printf("Write to '%s' failed: %s\n", output_file, image.message);
		ret = -1;
	}

	free(hwc_buffer);
	return ret;
}

/* It is not mandatory to use segments here, we use it in this example to demonstrate
 * how multiple segments for one input is used. Following code would work without any
 * problem:
 *	data->input_seg[0].addr = model_input;
 *	data->num_input_seg = 1;
 * In this case, only one input segment is used.
 */
static int prepare_infer_param(const char *image_file, odp_ml_data_t *data,
			       odp_ml_run_param_t *run_param)
{
	uint32_t image_size = NUM_PIXELS * sizeof(float);
	uint32_t seg_size = image_size / NUM_INPUT_SEGMENTS;
	float *model_input = malloc(image_size);

	if (!model_input) {
		printf("Failed to allocate memory for model_input\n");
		return -1;
	}

	if (read_png_file(image_file, model_input) != 0) {
		free(model_input);
		return -1;
	}

	odp_ml_run_param_init(run_param);
	run_param->batch_size = BATCH_SIZE;

	data->num_input_seg = NUM_INPUT_SEGMENTS;
	for (int i = 0; i < NUM_INPUT_SEGMENTS; i++) {
		data->input_seg[i].size = seg_size;
		data->input_seg[i].addr = malloc(seg_size);
		if (!data->input_seg[i].addr) {
			printf("Failed to allocate memory for input[%d].addr\n", i);
			free(model_input);
			return -1;
		}

		/* model_input is a pointer to float, so adding one for example move
		 * the pointer 4 bytes forward, but memcpy works with byte unit, so
		 * need to convert it to pointer to char first. */
		memcpy(data->input_seg[i].addr, (char *)(model_input) + i * seg_size, seg_size);
	}
	free(model_input);

	data->num_output_seg = 1;
	data->output_seg[0].size = image_size;
	data->output_seg[0].addr = malloc(image_size);
	if (!data->output_seg[0].addr) {
		printf("Failed to allocate memory for data->output_seg[0].addr\n");
		return -1;
	}
	memset(data->output_seg[0].addr, 0, image_size);

	return 0;
}

int main(int argc, char *argv[])
{
	odp_ml_data_t data;
	odp_instance_t inst;
	const char *input_file;
	const char *output_file;
	odp_ml_model_t ml_model;
	odp_ml_data_seg_t output;
	odp_ml_capability_t capa;
	odp_ml_config_t ml_config;
	odp_ml_run_param_t run_param;
	odp_ml_model_param_t model_param;
	odp_ml_data_seg_t input[NUM_INPUT_SEGMENTS];
	int ret = 0;

	if (argc != 3) {
		printf("Please specify both input image and output image name\n"
		       "\nUsage:\n"
		       "  %s input output\n"
		       "\nThis example runs inference on style transfer model\n\n",
		       argv[0]);
		return -1;
	}

	input_file = argv[1];
	output_file = argv[2];

	if (odp_init_global(&inst, NULL, NULL)) {
		printf("Global init failed.\n");
		return -1;
	}

	if (odp_init_local(inst, ODP_THREAD_CONTROL)) {
		printf("Local init failed.\n");
		return -1;
	}

	if (odp_ml_capability(&capa)) {
		printf("odp_ml_capability() failed\n");
		ret = -1;
		goto odp_term;
	}

	if (MAX_MODEL_SIZE > capa.max_model_size) {
		printf("Configured max model size %d exceeds max mode size %" PRIu64 " in capa\n",
		       MAX_MODEL_SIZE, capa.max_model_size);
		ret = -1;
		goto odp_term;
	}

	odp_ml_config_init(&ml_config);
	ml_config.max_model_size = MAX_MODEL_SIZE;
	ml_config.load_mode_mask = ODP_ML_COMPL_MODE_SYNC;
	ml_config.run_mode_mask = ODP_ML_COMPL_MODE_SYNC;

	if (odp_ml_config(&ml_config)) {
		printf("odp_ml_config() failed.\n");
		ret = -1;
		goto odp_term;
	}

	odp_ml_model_param_init(&model_param);
	if (read_model_from_file("candy.onnx", &model_param)) {
		ret = -1;
		goto odp_term;
	}

	odp_ml_data_format_t input_format = {
		.data_type = ODP_ML_DATA_TYPE_FP32,
		.data_type_size = 4,
		.shape.type = ODP_ML_SHAPE_BATCH,
		.shape.num_dim = 4,
		.shape.dim_name = {"N", "C", "H", "W"},
		.shape.dim = {ODP_ML_DIM_DYNAMIC, NUM_CHANNELS, HEIGHT, WIDTH},
		.shape.dim_max = {MAX_BATCH_SIZE, NUM_CHANNELS, HEIGHT, WIDTH}
	};

	model_param.extra_info.num_inputs = 1;
	model_param.extra_info.input_format = &input_format;

	ml_model = odp_ml_model_create("style-transfer", &model_param);
	free(model_param.model);
	if (!ml_model) {
		printf("odp_ml_model_create failed.\n");
		ret = -1;
		goto odp_term;
	}

	odp_ml_model_print(ml_model);

	if (odp_ml_model_load(ml_model, NULL)) {
		printf("odp_ml_model_load() failed\n");
		ret = -1;
		goto destroy_model;
	}

	data.input_seg = input;
	data.output_seg = &output;
	if (prepare_infer_param(input_file, &data, &run_param)) {
		printf("Prepare infer params failed.\n");
		ret = -1;
		goto unload;
	}

	/* The dynamic dimension must match BATCH_SIZE */
	if (odp_ml_run(ml_model, &data, &run_param) != 1) {
		printf("odp_ml_model_run() failed.\n");
		ret = -1;
	} else { /* Inference succeeded */
		if (write_to_png(output_file, (float *)output.addr))
			ret = -1;
	}

	free(output.addr);
	for (int i = 0; i < NUM_INPUT_SEGMENTS; i++)
		free(input[i].addr);

unload:
	if (odp_ml_model_unload(ml_model, NULL)) {
		printf("odp_ml_model_unload() failed\n");
		ret = -1;
		goto odp_term;
	}

destroy_model:
	if (odp_ml_model_destroy(ml_model)) {
		printf("odp_ml_model_destroy() failed\n");
		ret = -1;
	}

odp_term:
	if (odp_term_local()) {
		printf("Local term failed.\n");
		return -1;
	}

	if (odp_term_global(inst)) {
		printf("Global term failed.\n");
		return -1;
	}

	return ret;
}
