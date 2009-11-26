#
# Makefile for Linux samplefs
#
obj-m += samplefs.o
samplefs-objs := super.o inode.o

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build/ M=$(shell pwd)

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build/ M=$(shell pwd) clean

