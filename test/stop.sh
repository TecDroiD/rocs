#!/bin/bash

for i in `ps -aeo pid,command | grep rocsmq | awk '{print $1}' `
do
 name=`ps -aeo pid,command | grep $i | awk '{print $2}' `
 echo "Killing $name"
 kill $i
 echo "killed."
done
