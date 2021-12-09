#!/bin/bash
rm -f $PWD/$2
for file in `ls $1`; do
    "$PWD/build/exec/extract" "$1/$file" "$PWD/$2" "$3"
done
