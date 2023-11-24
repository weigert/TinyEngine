# TinyEngine/examples

# Iterate over Sub-Directories: make all
.PHONY: all
all:
	@for dir in * ; do \
    if [ -d $${dir} ]; then \
			cd $${dir}; \
			rm ./main ||:; \
			$(MAKE) all; \
			cd ..; \
		else \
			exit 0; \
		fi; \
 	done