#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <ctype.h>

#include "../Messages/messages.h"

int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                fprintf(stderr, "error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        								// no signaling chars, no echo,
                                        // no canonical processing


        tty.c_oflag = 0;                // no remapping, no delays

        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                fprintf(stderr, "error %d from tcsetattr", errno);
                return -1;
        }

        return 0;
}

void
set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                fprintf(stderr, "error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                fprintf(stderr, "error %d setting term attributes", errno);
}

int running=1;
void intHandler(int dummy) {
    running = 0;
}

void writeSerialMessage(int fd,int id, ARGS arguments){
	inline int PC_Serial_writer(uint8_t* buff,int n)
			{return write(fd,buff,n);}

	SimpleMessage message={.id=id,.arguments=arguments};
	int tot= write_message(PC_Serial_writer,
			(SerialPayload){.simple_message=message});
	printf("Message written! :%d\n",tot);
}

void write_test_message(int fd){
	ARGS arguments;

	arguments.traj_length.length[0]=1;
	arguments.traj_length.length[1]=0;
	writeSerialMessage(fd,ID_MSG_ORDER_TRAJ_SET_LENGTH,arguments);

	strcpy(arguments.message_antenne,
			ANTENNA_SURVIE);
	writeSerialMessage(fd,ID_MSG_ORDER_TRAJ_SET_NEW_POINT,
				(ARGS){.message_antenne=ANTENNA_SURVIE});


	writeSerialMessage(fd,ID_MSG_ORDER_TRAJ_CHECK_CORRECT,arguments);
}

void replaceInString(char*buff,char old,char new,int length){
	for(int i=0;i<length;i++)
		if(buff[i]==old)
			buff[i]=new;
}
int max(int a , int b){
	if(a>b)return a;
	return b;
}

void main(){
   signal(SIGINT, intHandler);
   crcInit();

	char *portname = "/dev/ttyACM1";

	int fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0)
	{
		     fprintf(stderr, "error %d opening %s: %s", errno, portname, strerror (errno));
		     return;
	}

	set_interface_attribs (fd, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
	set_blocking (fd, 0);                // set no blocking

	//create a intermediar reader function
	inline int PC_Serial_reader(uint8_t* buff,int n)
		{return read(fd,buff,n);}

	printf("C'est parti! %s\n",portname);
	int count=0;
	SerialPayload message;
	while(running){
		int state=read_message(PC_Serial_reader,message.buffer);

		if(state>=0){
			int args_length=max(state-3-4-1,1);
			char log_ascci[args_length];
			strncpy(log_ascci,message.stamp_message.arguments.message_antenne,
					args_length);
			replaceInString(log_ascci,'\r','\n',args_length);
			replaceInString(log_ascci,'\0','o',args_length);
			printf("Message id:%02d:",
				message.stamp_message.id%ID_MSG_LOG_REEMIT_OFFSET);
			for(int i=0;i<args_length;i++)printf("%c",log_ascci[i]);
			printf("\tTime:");
			uint32_t time=0;
			for(int i=0;i<4;i++)
				time+=message.stamp_message.timestamps[i]<<(8*i);
			printf("%ums",time);

			if(state==0)printf("\tERROR ON CRC");
			printf("\n");

			if(count++>=10){
				count=0;
				write_test_message(fd);
			}
		}

	}
    close(fd);
}
