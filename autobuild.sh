#!/bin/bash

set -e # "Exit immediately if a simple command exits with a non-zero status."

rm -rf `pwd`/build/* # `pwd` : `command` 

cd `pwd`/build &&
	cmake .. &&
	make

cd ..