#!/bin/bash

taget_file=$1
type=$2
output_file_path=$3

glslang -V "$taget_file" -S "$type" -o "$output_file_path"
#glslang -V "$taget_file" -verti