#!/bin/sh
date >> info.txt
echo "the shell is running" >> info.txt

while :
do
stillRunning=$(ps -ef |grep main |grep -v "grep")
if [ -z "$stillRunning" ]
then
date >> info.txt
echo "the screen was closed!!!!!!!!!!!!!!!!!" >> info.txt
/home/wang/NFS/HLS/hls
date >> info.txt
echo "the screen was started!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" >> info.txt
fi

sleep 10