obj-m += doorlock.o
doorlock-y := module.o gn.o usb.o

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build SUBDIRS=$(shell pwd) modules

clean:
	rm -f doorlock.ko  doorlock.mod.c  Module.markers  modules.order  Module.symvers
	rm -Rf .tmp_versions
	find . -iname \*~ -exec rm -f {} \;
	find . -iname \*.o -exec rm -f {} \;
	find . -iname .\*.cmd -exec rm -f {} \;
	
load: doorlock.ko
	sudo insmod ./doorlock.ko

unload:
	sudo rmmod doorlock
