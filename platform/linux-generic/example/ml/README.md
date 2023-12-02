# ML examples

Machie Learning API examples demonstrate how to use ODP ML API in different tasks:
simple linear computation, re-styling images and predicting a handwritten digit in
a given image.

Before going to specific examples, we need to install dependencies first:

```bash
sudo apt-get install libpng-dev
```

## Simple Linear

This example runs on a very simple model of form y = 3 * x + 4 where x is given
as the second argument.

### Generate model

```bash
python3 <odp_directory>/platform/linux-generic/test/validation/api/ml/simple_linear_gen.py
```

### Run simple linear

```bash
$ ./simple_linear 3
.
.
.
y = 3 * 3 + 4: 13
.
```

Or run the program with multiple threads, each thread inferences on one x given in
the input. Thus, the number of threads is the number of numbers in the second argument.

```bash
$ ./simple_linear [2,4,5]
.
.
.
y = 3 * 2 + 4: 10
../../../platform/linux-generic/odp_ml.c:2174:odp_ml_model_run():input_tensor[0]: 0x7fde8c4372f0
../../../platform/linux-generic/odp_ml.c:2174:odp_ml_model_run():input_tensor[0]: 0x7fde844372f0
y = 3 * 5 + 4: 19
y = 3 * 4 + 4: 16
.
```

## Style Transfer

This example applies artistic style to a given image.

### Get model and prepare data

Download the model:

```bash
wget https://raw.githubusercontent.com/microsoft/Windows-Machine-Learning/master/Samples/FNSCandyStyleTransfer/UWP/cs/Assets/candy.onnx
```

Prepare an image of size 720 * 720 in png format.

### Run style transfer

```bash
./style_transfer <input.png> <output.png>
```

Note: this example use multiple segments for one input in order to demonstrate how
input segmentation works. It is heavily referred from [fast-neural-style](https://github.com/jcjohnson/fast-neural-style). Functions `hwc_to_chw, chw_to_hwc, read_png_file and write_to_png` are
referred from there. Need to double check if there would be any license issue.

## MNIST

This example predicts a handwritten digit in a given image.

### Dowload MNIST model

```bash
wget https://media.githubusercontent.com/media/onnx/models/main/vision/classification/mnist/model/mnist-8.onnx
```

Or manually from [here](https://github.com/onnx/models/tree/main/vision/classification/mnist)

### Prepare input data

The input image is stored in a csv file, which contains in a row both the digit lable
(a number from 0 to 9) and the 784 pixel values (a number from 0 to 255).
The csv digit file can be created by copying any row other than the first from file
[mnist_test.csv](https://www.kaggle.com/oddrationale/mnist-in-csv).

```bash
$ cat digit7.csv
7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,84,185,159,151,60,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,222,254,254,254,254,241,198,198,198,198,198,198,198,198,170,52,0,0,0,0,0,0,0,0,0,0,0,0,67,114,72,114,163,227,254,225,254,254,254,250,229,254,254,140,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,17,66,14,67,67,67,59,21,236,254,106,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,83,253,209,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,233,255,83,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,129,254,238,44,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,59,249,254,62,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,133,254,187,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,205,248,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,126,254,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,75,251,240,57,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,221,254,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,203,254,219,35,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,254,254,77,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,224,254,115,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,133,254,254,52,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,61,242,254,254,52,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,121,254,254,219,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,121,254,207,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
```

### Run mnist

```bash
$ ./mnist mnist-12.onnx digit7.csv
.
.
.
predicted_digit: 7, expected_digit: 7
.
```

## Model Explorer

The example prints basic model information.

### Run model_explorer

```bash
$ ./model_explorer simple_linear.onnx
.
.
.
Model info
----------
  Model handle: 0x7fe8426ce1d8
  Name: model-explorer
  Model version: 1
  Model interface version: 0
  Index: 0
  Number of inputs: 1
    Input[0]: Name: x, Data_type: int32, Shape: static [1], Size: 4
  Number of outputs: 1
    Output[0]: Name: y, Data_type: int32, Shape: static [1], Size: 4
.
.
.
```
