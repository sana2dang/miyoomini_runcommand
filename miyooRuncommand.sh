#!/bin/sh

# your retroarch cores path( .so files )
coresPath="/mnt/SDCARD/joo/retroarch/.retroarch/cores"

cd /mnt/SDCARD/App/runcommand/
./miyooRuncommand "$coresPath" "$1"

