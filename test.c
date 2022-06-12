#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <semaphore.h>

#define CHAR_DEVICE_NODE "/dev/xkp"
#define BUF_SIZE 256
#define COMMAND_START '1'
#define COMMAND_PAUSE '2'
#define COMMAND_STOP '3'
#define COMMAND_RESTART '4'
#define COMMAND_START_1 '5'
#define COMMAND_START_2 '6'
#define COMMAND_PAUSE_1 '7'
#define COMMAND_PAUSE_2 '8'
#define COMMAND_STOP_1 '9'
#define COMMAND_STOP_2 'a'
#define COMMAND_CONTINUE_1 'b'
#define COMMAND_CONTINUE_2 'c'




int main(void)
{
	int fd;

	printf("*********************************\n");
	printf("********  全部开始请按   1*******\n");
	printf("********  全部暂停请按   2*******\n");
	printf("********  全部停止请按   3*******\n");
	printf("********全部重新开始请按 4*******\n");
	printf("********开始第一线程请按 5*******\n");
	printf("********开始第二线程请按 6*******\n");
	printf("********暂停第一线程请按 7*******\n");
	printf("********暂停第二线程请按 8*******\n");
	printf("********停止第一线程请按 9*******\n");
	printf("********停止第二线程请按 a*******\n");
	printf("********继续第一进程请按 b*******\n");
	printf("********继续第二进程请按 c*******\n");


	int retval = 0;
	char data_buf[BUF_SIZE] = {0};

	fd = open(CHAR_DEVICE_NODE,O_RDWR);
	if(fd<0){
		printf("open %s failed!\n",CHAR_DEVICE_NODE);
		return -1;
	}
	printf("open %s successfully!\n",CHAR_DEVICE_NODE);
	while(1)
	{
		printf("num:");
		char cmd = -1;
		sync();
		cmd = getchar();
		getchar();
		printf("common:");
		if(cmd =='1'){
			printf("ALL START\n");
			cmd = COMMAND_START;
		}
		else if(cmd == '2'){
			printf("ALL PAUSE\n");
			cmd = COMMAND_PAUSE;
		}
        	else if(cmd == '3'){
			printf("ALL STOP\n");
        	        cmd = COMMAND_STOP;
        	}
        	else if(cmd == '4'){
			printf("ALL RESTASRT\n");
        	        cmd = COMMAND_RESTART;
        	}
        	else if(cmd == '5'){
			printf("START Kthread_1\n");
                	cmd = COMMAND_START_1;
        	}
        	else if(cmd == '6'){
			printf("START Kthread_2\n");
        	        cmd = COMMAND_START_2;
        	}
        	else if(cmd == '7'){
			printf("PAUSE Kthread_1\n");
        	        cmd = COMMAND_PAUSE_1;
        	}
        	else if(cmd == '8'){
			printf("PAUSE Kthread_2\n");
        	        cmd = COMMAND_PAUSE_2;
        	}
                else if(cmd == '9'){
			printf("STOP Kthread_1\n");
                        cmd = COMMAND_STOP_1;
                }
                else if(cmd == 'a'){
			printf("STOP Kthread_2\n");
                        cmd = COMMAND_STOP_2;
                }
                else if(cmd == 'b'){
			printf("CONTINUE Kthread_1\n");
                        cmd = COMMAND_CONTINUE_1;
                }
                else if(cmd == 'c'){
			printf("CONTINUE Kthread_2\n");
                        cmd = COMMAND_CONTINUE_2;
                }

		retval = ioctl(fd,cmd,0);
		//retval = ioctl(fd,COMMAND_A,0);
		if(retval<0){
			printf("ioctl failed!\n");
			return -1;
		}
	
		printf("iocal %s successfully!\n",CHAR_DEVICE_NODE);
	}
	sync();
	close(fd);
	return 0;
}
