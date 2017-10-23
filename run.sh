#!/bin/bash
clear
echo "Checking for build...."
#make -f makefile
make -f ./makefile
echo "Running driver...."
./driver $1
