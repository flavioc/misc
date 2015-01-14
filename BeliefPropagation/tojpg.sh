#!/bin/sh

FILE=$1
export RGBDEF=/usr/local/Cellar/netpbm/10.60.02/misc/rgb.txt

cat $FILE | pgmtoppm > $FILE.ppm
ppmtojpeg $FILE.ppm > $FILE.jpg
