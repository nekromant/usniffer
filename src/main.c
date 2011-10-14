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
#include <sys/epoll.h>
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#include <errno.h>
#include <time.h>
#include "uart.h"


static char* prev_tag;
//copypaste from an OpenGL app, actually
static struct timeval		tv, tv0;
static float			time_counter, last_frame_time_counter, dt, prev_time, fps, elapsed;
void frametimer_init() {
 gettimeofday(&tv0, NULL);
 //frame=1;
 //frames_per_fps = 30; 
}

float frametimer_since(float offset)
{
  return elapsed-offset;
}

void frametimer_update() {
 last_frame_time_counter = time_counter;
 gettimeofday(&tv, NULL);
 time_counter = (float)(tv.tv_sec-tv0.tv_sec) + 0.000001*((float)(tv.tv_usec-tv0.tv_usec));
 dt = time_counter - last_frame_time_counter; 
 elapsed+=dt;
}

void process_event(struct epoll_event* ev)
{
  struct uart_settings_t* us = ev->data.ptr;
  char buf[8];
  int count,i,l;
  l=0;
  count=0; 
  if (ev->events & EPOLLIN)
  {
   if (us->tag != prev_tag) {
   printf("\n");
   frametimer_update();
   printf("[%f] %s:\t",frametimer_since(0), us->tag); 
   prev_tag=us->tag;
   }
   while (count!=-1)
     {
       count = read(us->fd, buf, 8);
       for (i=0;i<count;i++){
	printf(" 0x%hhx",buf[i]);
       }
       fflush(stdout);
     }
            
       if ((errno==EAGAIN) || errno==EWOULDBLOCK) {
       return;
       }else
       {
	 perror("\nread: ");
       }
  }
  
}

int main(int argc, char* argv[])
{
 if (argc<2)
 {
   printf("Usage %s port1 port2 port3...\n", argv[0]);
   printf("Port statements must be in this form:\n");
   printf("tag:/dev/ttyUSB0:115200:8:n:1\n");
   exit(0);
 }
 int i;
 struct uart_settings_t* us;
 int efd = epoll_create(argc);
 struct epoll_event ev;
 
 for (i=1; i<argc; i++)
 {
   printf("Parsing: %s \n", argv[i]);
   us = str_to_uart_settings(argv[i]); 
   us->fd = uart_init(us);
   fprintf(stderr, "fd is %d\n", us->fd);
   if (us->fd<0) 
   {
     fprintf(stderr, "Failed to open port %s\n", us->port);
     exit(1);
   }
   ev.data.ptr = (void*) us;
   ev.events=EPOLLIN | EPOLLET;
   if (epoll_ctl(efd, EPOLL_CTL_ADD, us->fd, &ev)!=0)
   {
     fprintf(stderr, "Failed to add port %s to epoll(%d)\n", us->port, us->fd);
     perror("epoll:");
     exit(1);
   }
 }
 //Init the frametimer
 frametimer_init();
 while (1)
 {
   int c = epoll_wait(efd, &ev, 1, 0);
   if (c) process_event(&ev);
 }
 
}