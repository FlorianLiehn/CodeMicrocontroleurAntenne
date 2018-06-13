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

void write_test_message(int fd){

	ARGS log_test;
	strcpy(log_test.message_antenne,"MESSAGE PCuC");

	log_message test={
		.order=ORDER_ANTENNA,
		.logs =log_test,
	};
	Payload_message payload={.message=test};
	uint8_t emit_buffer[serialMessageLength];
	encodePayload(payload.buffer,emit_buffer);
	write(fd,emit_buffer,serialMessageLength);
	printf("Message writted!\n");


}

int wait_message(int fd,char* message){
	uint8_t buf [serialMessageLength];
    memset (message, 0, Payload_message_lenght);

	int n=read (fd,buf,1);//n = number of read bytes
	if(buf[0]!=HEADER_BYTE || n!=1)
		return -1;
	while(n==1)
		n+=read (fd,&(buf[n]),1);//read nb
	int tot=(int)(buf[1]);
	while(n<tot+3){
		n+=read (fd,&(buf[n]),tot+3-n);
	}
	strncpy(message,&(buf[2]),tot);

	uint8_t crc=ComputeCRC(message,tot);
	printf("taille:%d CRC:%d|%d\n",tot, buf[tot+3-1],crc);
	if(crc!=buf[tot+3-1]){
		printf("ERREUR CRC\n");
		return 0;
	}
	return 1;
}



void read_message(char message[]){

	Payload_message msg;
	strcpy(msg.buffer,message);
	log_message log_msg=msg.message;
	printf("New message %d :",log_msg.order);
	switch(log_msg.order){
	case(ORDER_GOTO):
	default:
		printf("%s\n",log_msg.logs.message_antenne);
		break;
	}

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

	char buf [Payload_message_lenght];
	printf("C'est parti! %s\n",portname);

	int count=0;
	while(running){
		if(wait_message(fd,buf)>=0){
			printf("Message:%s\n",buf);

			if(count++>=10){
				count=0;
				write_test_message(fd);
			}
		}

	}
    close(fd);
}
