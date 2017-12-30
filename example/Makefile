SUBDIRS = $(shell ls -d */)

all:
	@for dir in ${SUBDIRS} ; do \
		if [ -f "$$dir/Makefile" ]; then \
			make -C "$$dir" ; \
		fi \
	done

clean:
	@for dir in ${SUBDIRS} ; do \
		if [ -f "$$dir/Makefile" ]; then \
			make clean -C "$$dir" ; \
		fi \
	done
