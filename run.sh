#!/bin/sh

# your retroarch cores path( .so files )
coresPath="/mnt/SDCARD/joo/retroarch/.retroarch/cores"

homedir=`dirname "$1"`
confPath="conf"
romPath=`dirname "$1"`
romDir=$(basename $(dirname "$1"))
romRealName="$1"
romFullName=`basename "$1"`
romName=${romFullName%.*}
romExtension=${romFullName##*.}


echo "romPath : $romPath" > log_run.txt
echo "romDir : $romDir" >> log_run.txt
echo "romRealName : $romRealName" >> log_run.txt
echo "romFullName : $romFullName" >> log_run.txt
echo "romName : $romName" >> log_run.txt

text=`cat "$confPath/$romDir/$romFullName"`
echo "target : $text" >> log_run.txt

runFile=`echo $text | cut -d ',' -f1`
coreName=`echo $text | cut -d ',' -f2`

echo "runFile : $runFile" >> log_run.txt
echo "coreName : $coreName" >> log_run.txt

# your retroarch or ra32.ss file path
runPath=/mnt/SDCARD/joo/retroarch
cd $runPath
$runPath/cpufreq.sh
HOME=$homedir $runPath/$runFile -L $coresPath/$coreName "$1"
