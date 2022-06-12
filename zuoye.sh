#!/bin/bash
#filename zuoye


retry_count=3
count=0

if [ -e ~/dazuoye/drivertest.ko ]
then
	make clean
fi
make 2>err.log
echo -e "make success"

while true 
do
	sudo insmod drivertest.ko 2>error.log
	if [ $? != 0 ]
	then
		count=`expr $count + 1`
	else
		echo -e "insmod success"
		break
	fi
	if [ $count -eq 5 ]
	then
		echo -e "insmod fail"
		count=0;
		exit 1
		break
	fi
done

while true 
do
        sudo mknod /dev/xkp c 237 0 2>error.log
        if [ $? != 0 ]
        then
                count=`expr $count + 1`
        else
		echo -e "mknod success"
                break
        fi
        if [ $count -eq 5 ]
        then
		echo -e "mknod fail"
                count=0;
                exit 1
                break
        fi
done

while true 
do
        sudo chmod 0666 /dev/xkp 2>error.log
        if [ $? != 0 ]
        then
                count=`expr $count + 1`
        else
                echo -e "chmod success"
                break
        fi
        if [ $count -eq 5 ]
        then
                echo -e "chmod fail"
                count=0;
                exit 1
                break
        fi
done



