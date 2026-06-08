all:
	make -C ~/focal M=`pwd` modules
 
clean:
	rm -rf *.o *.cmd *.order Module.symvers *.ko *.mod* ./.* .tmp_versions
 
obj-m +=  pl_reset.o
