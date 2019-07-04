#!/bin/bash

full_filename=$1
extension="${full_filename##*.}"
filename="${full_filename%.*}"

cd ../../cmake-build-debug
mkdir ../training/${filename}
./Armor_Sample -E $2 -O ../training/${filename} -P ${filename} ../test_clips/armors/${full_filename} $3
