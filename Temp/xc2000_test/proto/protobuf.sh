#!/bin/sh

protofiles=$( ls | grep .proto )
echo $protofiles

for file in $protofiles; do
	protoc --cpp_out=./ $file;
done

#移动.h文件 
mv ./*.h ../code/inc/proto
#cp ./*.cc ../src/

cfile=$( ls | grep .cc)
for f in $cfile ;do
	#移动.cpp文件 
	mv $f ../code/src/proto/$( echo $f | awk -F. '{print $1"."$2".c"}')
done

