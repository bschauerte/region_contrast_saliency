#!/bin/bash

TEST_FOLDER="test_package"
FILES="ext examples build.m README.md region_saliency.cpp region_saliency.h region_saliency_mex.cpp stdafx.cpp stdafx.h test_region_saliency_mex.m visualize_region_contrast_distance_bias.m visualize_theoretical_distance_bias.m"

# copy the files and try to compile, if it failes, then the pacakge is broken!
rm -rf $TEST_FOLDER
mkdir $TEST_FOLDER
for file in $FILES; do
	cp -rf $file $TEST_FOLDER"/"
done
cd $TEST_FOLDER
zip -r LDRC.zip .
