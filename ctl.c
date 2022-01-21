#include <assert.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
  char kernel_val[20];
  struct pollfd pfd;
  int fd, ret, n;
  
  fd = open("/dev/emre", O_RDWR | O_NONBLOCK);
    
  if(fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }
    
  pfd.fd = fd;
  pfd.events = POLLIN | POLLRDNORM | POLLOUT | POLLWRNORM;
    
  while(1) {
    ret = poll(&pfd, (unsigned long) 1, 5000);
    
    if(ret < 0) {
      perror("poll");
      assert(0);
    }
    
    if((pfd.revents & POLLIN ) == POLLIN) {
      read(pfd.fd, &kernel_val, sizeof(kernel_val));
      printf("POLLIN", kernel_val);
    }
    
    if((pfd.revents & POLLOUT ) == POLLOUT) {
      strcpy(kernel_val, "User Space");
      write(pfd.fd, &kernel_val, strlen(kernel_val));
      printf("POLLOUT", kernel_val);
    }
  }
}