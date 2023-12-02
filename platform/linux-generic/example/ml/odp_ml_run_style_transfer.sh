#!/bin/bash
#
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2023 Nokia
#
set -e

wget https://github.com/microsoft/Windows-Machine-Learning/raw/master/Samples/FNSCandyStyleTransfer/UWP/cs/Assets/candy.onnx

./style_transfer ODP-Logo-HQ.png ODP-Logo-HQ-style.png

if [ ! -e ODP-Logo-HQ-style.png ]; then
	echo "style_transfer failed"
	rm candy.onnx
	exit 1
fi

rm candy.onnx
rm ODP-Logo-HQ-style.png
