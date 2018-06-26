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
	SerialPayload seriMessage={.simple_message=message};
	int tot= write_message(PC_Serial_writer,&seriMessage);
	printf("Message written! :%d\n",tot);
}

void write_test_message(int fd){

	writeSerialMessage(fd,ID_MSG_ORDER_SURVIE,
				(ARGS){.message_antenne=ANTENNA_SURVIE});

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

void print_time(uint32_t millis){
	int ms=millis%1000;
	millis/=1000;

	int s=millis%60;
	millis/=60;

	int m=millis%60;
	millis/=60;

	int h= millis;

	printf("\t%2d:%2d:%2d.%3d\n",h,m,s,ms);
}

void print_log_message(StampedMessage message){
	int args_length=GetPayloadLength(message.id);
	args_length-=8;

	char log_ascci[args_length];
	strncpy(log_ascci,message.arguments.message_antenne,
			args_length);
	replaceInString(log_ascci,'\r','\n',args_length);
	replaceInString(log_ascci,'\0','o',args_length);

	printf("Message id:%02d:",message.id%ID_MSG_LOG_REEMIT_OFFSET);
	for(int i=0;i<args_length;i++)printf("%c",log_ascci[i]);

	uint32_t millis=0;
	for(int i=0;i<4;i++)millis+=message.date.millis[i]<<(8*i);
	uint16_t year=0;
	for(int i=0;i<2;i++)year+=message.date.year[i]<<(8*i);
	printf("\tTime:y:%d m:%2d d:%2d",
		year,message.date.month,message.date.day);
	print_time(millis);

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

		if(state >= 0){

			print_log_message(message.stamp_message);

			if(state==0)printf("\tERROR ON CRC\n");

			if(count++ >= 15){
				count=0;
				//write_test_message(fd);
			}
		}

	}
    close(fd);
}
