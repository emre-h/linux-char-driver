obj-m += emre.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc ctl.c -o ctl.o

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

ctl:
	gcc ctl.c -o ctl.o