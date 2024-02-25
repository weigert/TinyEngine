# TinyEngine/examples

# Iterate over Sub-Directories: make all
.PHONY: all
all:
	@for dir in * ; do \
    if [ -d $${dir} ]; then \
			cd $${dir}; \
			echo $${dir}; \
			rm ./main ||:; \
			$(MAKE) --no-print-directory all; \
			cd ..; \
		else \
			exit 0; \
		fi; \
 	done