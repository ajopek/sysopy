#!/bin/bash

if [ $# -eq 0 ]
  then
    echo "Too few arguments"
    exit 1
fi

./master $1 &
./slave $1 3 &
./slave $1 14 &
./slave $1 15 &
./slave $1 9 &
./slave $1 26 &
./slave $1 5 &
