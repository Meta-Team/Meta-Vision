#!/bin/sh

cd /home/meta/Desktop/Meta-Vision/build

log_filename=`date +%Y-%m-%d_%H-%M-%S`

while
    cmake .. && make Vision -j4 && ./Vision ../test.yml | tee "../log/${log_filename}.txt"
do :; done