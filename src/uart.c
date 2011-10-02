#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#include <errno.h>

static struct termios oldtio,newtio;
int uart_init(char* port, int speed)
{
    int fd = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd <0) {
        return -EIO;
    }
    tcgetattr(fd,&oldtio); /* save current port settings */
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag =  CS8 | CLOCAL | CREAD;
    switch(speed)
    {
      case 115200:
      newtio.c_cflag|=B115200;
      break;
      case 57600:
      newtio.c_cflag|=B57600;
      break;
      case 19200:
      newtio.c_cflag|=B19200;
      break;
      default:
      printf("Unsupported baudrate - guess you'll need to add it to the source, since I'm lazy\n");
      break;
    }
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 0;   /* We're non-blocking */
    cfmakeraw(&newtio);
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);
    return fd;
}
