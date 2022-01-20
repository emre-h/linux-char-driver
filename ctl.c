#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#define DEVICE "/dev/emre"

int main() {
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    int fd = open(DEVICE, O_RDWR | O_NONBLOCK) | mode;
  
    write(fd, "data", 4);
}