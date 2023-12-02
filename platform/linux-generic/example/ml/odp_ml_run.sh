#!/bin/bash
#
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2023 Nokia
#
set -e

./model_explorer${EXEEXT} simple_linear.onnx

./simple_linear${EXEEXT} [2,4,5]

wget https://github.com/onnx/models/raw/main/validated/vision/classification/mnist/model/mnist-12.onnx
./mnist mnist-12.onnx example_digit.csv

rm mnist-12.onnx
