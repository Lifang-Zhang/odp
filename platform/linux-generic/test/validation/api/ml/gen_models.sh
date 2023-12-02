#!/bin/bash

set -e

# cd to the directory where this script is in
cd "$( dirname "${BASH_SOURCE[0]}" )"

python3 simple_linear_gen.py

python3 batch_add_gen.py
