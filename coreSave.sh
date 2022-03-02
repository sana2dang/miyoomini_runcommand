#!/bin/sh

confPath="conf"

runFile="$1"
coreName="$2"
romPath=`dirname "$3"`
romDir=$(basename $(dirname "$3"))
romRealName="$3"
romFullName=`basename "$3"`
romName=${romFullName%.*}
romExtension=${romFullName##*.}

echo "coreName : $coreName" > log_coreSave.txt
echo "romPath : $romPath" >> log_coreSave.txt
echo "romDir : $romDir" >> log_coreSave.txt
echo "romRealName : $romRealName" >> log_coreSave.txt
echo "romFullName : $romFullName" >> log_coreSave.txt
echo "romName : $romName" >> log_coreSave.txt
echo "romExtension : $romExtension" >> log_coreSave.txt

if [ ! -d $confPath ]; then
	mkdir conf
fi

cd conf

if [ ! -d $romDir ]; then
	mkdir $romDir
fi

cd $romDir

echo "$runFile,$coreName" > $romFullName

