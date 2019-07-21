#!/bin/bash

cd /home/meta/Desktop/Meta-Vision/build

mkdir log

log_file=`date +%Y-%m-%d_%H-%M-%S.log`
cmake .. && make Vision -j4 && sudo ./Vision ../jetson.yml | tee log_file