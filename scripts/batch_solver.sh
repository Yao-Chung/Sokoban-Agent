#!/bin/bash
for binary_file in `ls $1`; do
    cat $1/$binary_file | $PWD/build/exec/solver $2/$binary_file
done