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

#include "uart.h"


int main(int argc, char* argv[])
{
 if (argc<2)
 {
   printf("Usage %s port1 port2 port3...\n");
   printf("Port statements must be in this form:\n");
   printf("tag:/dev/ttyUSB0:115200:8:n:1\n");
 }
 int i;
  struct uart_settings* us;
 for (i=1; i<argc; i++)
 {
   printf("Parsing: %s \n", argv[i]);
  str_to_uart_settings(argv[i]); 
 }
 
}