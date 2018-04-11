#!/bin/bash


#This script simulates network traffic, creating files which have size and access date changed (deemed: "volatile") and files which are deleted and created (deemed: "deletable")
#NOTE: it may take a few minutes for simulator to reach equilibrium for deletable files, in which it will create and delete a deletable file every loop or so rather consistently.

cd ..
mkdir testbed
cd testbed

# This script creates 100 files named "volatile-x-bytes.txt"
i="0"
while [ $i -lt 100 ]
do
    touch volatile-$i-bytes.txt
    
    #clear file if exists
    truncate -s 0 volatile-$i-bytes.txt
    
    #make file x bytes large
    truncate -s $i volatile-$i-bytes.txt
    
    i=$(($i+1))
done

#every 5 seconds, a file will be changed, a file will be deleted, and a file will be created
while true
do
	#change file's size and access date
	#(changes a file of type named "volatile-x-bytes.txt".)
	#NOTE: DOES NOT RENAME FILE ONCE CHANGED
		#select random file to change
		sleep 5
		selectedFileNum=$((RANDOM % 100))
		selectedFileNum=$selectedFileNum

		#changes size to random number between 0 and 99 bytes
		truncate -s 0 volatile-$selectedFileNum-bytes.txt
		randomSize=$((RANDOM % 100))
		
		truncate -s $randomSize volatile-$selectedFileNum-bytes.txt

		echo Changed size of volatile-$selectedFileNum-bytes.txt to $randomSize bytes

		randomDaysNum=$((1 + RANDOM % 100))
		touch -d "$randomDaysNum days ago" volatile-$selectedFileNum-bytes.txt
		echo Changed access "date" of volatile-$selectedFileNum-bytes.txt to $randomDaysNum days ago

	
	#create file
	#(creates a file named "deletable-x-bytes.txt")
		randomSizeNewFile=$((RANDOM % 100))
		touch deletable-$randomSizeNewFile-bytes.txt

		#clear file if already exists
		truncate -s 0 deletable-$randomSizeNewFile-bytes.txt
	
		#fill file with $randomSizeNewFile bytes of random data
		truncate -s $randomSizeNewFile deletable-$randomSizeNewFile-bytes.txt
	    
	    
		echo Created deletable-$randomSizeNewFile-bytes.txt

	#delete file 
	#(deletes a file of type named named "deletable-x-bytes.txt")
		randomSizeDeleteFile=$((RANDOM % 100))
		rm deletable-$randomSizeDeleteFile-bytes.txt
		echo Deleted deletable-$randomSizeDeleteFile-bytes.txt " (if it existed)"
		echo
done
