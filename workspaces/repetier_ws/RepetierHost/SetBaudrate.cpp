// Set serial port to any baudrate if possible
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> 
#include <errno.h> 
#include <time.h>  
#include <string.h>
#include <sys/ioctl.h>
//#undef TCGETS2
#ifdef TCGETS2
#warning TCGETS2
#include <asm-generic/termbits.h>
#else
#warning TERMIOS
#include <termios.h> 
#endif
#include <linux/serial.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

int open_port(const char *port)
{
	int fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
	
	if(fd == -1) {
		printf("error: Unable to open %s. \n",port);
		exit(-2);
	}
	else
	{
		fcntl(fd, F_SETFL, 0);
	}
	
	return(fd);
} //open_port

#ifdef TCGETS2
#warning TCGETS2
int set_baudrate(int handle, int baud) {
  struct termios2 ios;
      
  fcntl(handle, F_SETFL, 0);
  if(ioctl(handle, TCGETS2, &ios)) {
    cerr << "error: TCGETS2 failed" << endl;
    exit(-2);
  }
  ios.c_ispeed = baud;
  ios.c_ospeed = baud;
  ios.c_cflag &= ~CBAUD;
  ios.c_cflag |= BOTHER;
  //cfmakeraw(&ios);
  ios.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
  ios.c_oflag &= ~OPOST;
  ios.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  ios.c_cflag &= ~(CSIZE | PARENB);
  ios.c_cflag |= CS8;
  //\cfmakeraw
  ios.c_cflag |= (CLOCAL | CREAD);
  ios.c_cflag &= ~CRTSCTS;
  if(ioctl(handle, TCSETS2, &ios) != 0) {
    cerr << "error: TCSETS2 failed" << endl;
    exit(-2);
  }
  if(ioctl(handle, TCGETS2, &ios)) {
    cerr << "error: reget TCGETS2 failed" << endl;
    exit(-2);
  }
  cerr << "actual baud: " << ios.c_ispeed << endl;
  if(abs(baud - ios.c_ispeed) * 100 / baud >= 5) {
    cerr << "error: couldn't set desired baud rate " << baud << " got " << ios.c_ispeed << endl;
    exit(-2);
  }
}
#else
#warning TERMIOS
static int rate_to_constant(int baudrate) {
#define B(x) case x: return B##x
	switch(baudrate) {
		B(50);     B(75);     B(110);    B(134);    B(150);
		B(200);    B(300);    B(600);    B(1200);   B(1800);
		B(2400);   B(4800);   B(9600);   B(19200);  B(38400);
		B(57600);  B(115200); B(230400); B(460800); B(500000); 
		B(576000); B(921600); B(1000000);B(1152000);B(1500000); 
	default: return 0;
	}
#undef B
} 

int set_baudrate(int handle, int baud) {
  termios ios;
  int speed = rate_to_constant(baud);
      
  if(speed == 0) {
    struct serial_struct ss;
    ss.reserved_char[0] = 0;
    if(ioctl(handle, TIOCGSERIAL, &ss) < 0) {
       cerr << "error: TIOCGSERIAL" << endl;
       exit(-2);
    }
    ss.flags = (ss.flags & ~ASYNC_SPD_MASK) | ASYNC_SPD_CUST;
    ss.custom_divisor = (ss.baud_base + (baud / 2)) / baud;
    ss.custom_divisor = (ss.baud_base + (baud / 2)) / baud;
    if(ss.custom_divisor == 0) {
      cerr << "error: baud_base (" << ss.baud_base << ") does not allow setting custom baud rate" << endl;  
      exit(-2);
    }
    long closestSpeed = ss.baud_base / ss.custom_divisor;
    // cout << " Closest speed " << closestSpeed << endl;
    ss.reserved_char[0] = 0;
    if (closestSpeed < baud * 98 / 100 || closestSpeed > baud * 102 / 100) {
       cerr << "error: couldn't set desired baud rate " << baud << endl;
       exit(-2);
    }
            
    if(ioctl(handle, TIOCSSERIAL, &ss) < 0) {
        cerr << "error: Setting baudrate " << baud << " failed" << endl;
		  	exit(-2);
    }
  }
            
  fcntl(handle, F_SETFL, 0);
	tcgetattr(handle, &ios);
  ::cfsetispeed(&ios, speed ? speed : B38400);
  ::cfsetospeed(&ios, speed ? speed : B38400);
	cfmakeraw(&ios);
	ios.c_cflag |= (CLOCAL | CREAD);
	ios.c_cflag &= ~CRTSCTS;
	if (tcsetattr(handle, TCSANOW, &ios) != 0) {
	  cerr << "error: tcsetattr failed" << endl;
		exit(-2);
	}
}
#endif

int main(int argc, const char* argv[])
{ 
	if(argc != 3) {
	   cerr << "Syntax: SetBaudrate serialDevice baudrate" << endl;
	   exit(-1); 
	}
	int fd = open_port(argv[1]);
	if(fd<0) {
			cerr << "error: could not open device " << argv[1] << endl;
			exit(-2);
	}
	int baud = atoi(argv[2]);
	if(baud<=0) {
		cerr << "error: Positive baud rate needed, but I got " << argv[2] << endl;
		exit(-2);
	}
	set_baudrate(fd, baud);
	return(0);	
}