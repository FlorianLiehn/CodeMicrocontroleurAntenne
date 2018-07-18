/*
 * ComServer.c
 *
 *  Created on: 10 juil. 2018
 *      Author: liehnfl
 */

#include "../../CodesPC/ComServer/ComServer.h"

int setInterfaceAttribs (int fd, int speed, int parity, int should_block)
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
        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        								// no signaling chars, no echo,
                                        // no canonical processing

        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
        tty.c_iflag &= ~(ICRNL | INLCR | IGNCR); // not map \r to \n

        tty.c_oflag = 0;                // no remapping, no delays

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                fprintf(stderr, "error %d from tcsetattr", errno);
                return -1;
        }

        return 0;
}

void cancelHandler(int dummy,int*running) {
	*running = 0;
}

int main(){
	//stop procedure : Ctrl+C
	int running=1;
	void sigHandler(int dummy){cancelHandler(dummy,&running);}
	signal(SIGINT, sigHandler);

	//init CrcTable (lower CRC compute time)
	crcInit();
	//init USB connection
	int fd = open (MICROCONTROLER_PORT, MICROCONTROLER_COM_PARAM);
	if (fd < 0)
	{
		fprintf(stderr, "error %d opening %s: %s", errno,
			 MICROCONTROLER_PORT, strerror (errno));
		return EXIT_FAILURE;
	}
	setInterfaceAttribs(fd, B115200, 0,0);

	//init Thread Read Write
	pthread_t thRead,thWrite;
	if(pthread_create(&thRead , NULL,threadReaderLoger  , (void*)&fd) == -1 ||
	   pthread_create(&thWrite, NULL,threadServerEmitter, (void*)&fd) == -1) {
		perror("pthread_create");
		return EXIT_FAILURE;
	}
	//Threads running
	while(running){
		usleep(US_EXIT_LOOP);
	}
	//cancel & error
	pthread_cancel(thRead );
	pthread_cancel(thWrite);
	perror("threads finished");
}
