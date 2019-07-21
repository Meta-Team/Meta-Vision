#!/bin/sh

cd /home/meta/Desktop/Meta-Vision/build

log_filename=`date +%Y-%m-%d_%H-%M-%S`

while

    echo "---------------------- CMAKE -------------------------"
    cmake ..

    echo
    echo "----------------------- MAKE -------------------------"
    make Vision -j4

    echo
    echo "------------------------ RUN -------------------------"
    ./Vision ../jetson.yml | tee "../log/${log_filename}.txt"

    echo
    echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
    echo "                        RESTART                        "
    echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
    echo

do :; done