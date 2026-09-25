# Export user variables for the Kbuild file and kernel Makefile
ifdef CROSS_COMPILE
    export CROSS_COMPILE
endif
ifdef ARCH
    export ARCH
endif


# When natively compiling, we can infer the kernel's source tree directory,
# if the user has not specified it.
ifndef CROSS_COMPILE
    KERNEL_VERSION = $(shell uname -r)
    KBUILD_DIR ?= /lib/modules/$(KERNEL_VERSION)/build/
endif

# Add include directory for dma-proxy.h
EXTRA_CFLAGS += -I$(PWD)/../Common

all:
	make -C $(KBUILD_DIR) M=`pwd` modules
 
clean:
	rm -rf *.o *.cmd *.order Module.symvers *.ko *.mod* ./.* .tmp_versions
 
obj-m += dma-proxy.o
