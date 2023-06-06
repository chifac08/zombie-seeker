KBUILD_CFLAGS += -g -Wall
obj-m += zombie_seeker.o

module:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

test:
	gcc -o tests/zombie_process tests/zombie_process.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f tests/zombie_process
