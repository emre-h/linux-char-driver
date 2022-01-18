obj-m += emre.o

CFLAGS := $(CFLAGS) -std=gnu99 -D_GNU_SOURCE

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean