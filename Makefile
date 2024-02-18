# TinyEngine
# Make Rules

# Version: 1.8
# Author: Nicholas McDonald
# Tested on GNU/Linux

# Build and Install TinyEngine
.PHONY: source
source:
	@echo "TinyEngine: Building and Installing from source..."
	@cd source; $(MAKE) --no-print-directory all
	@echo "TinyEngine: Done"

# Build all Examples
.PHONY: examples
examples:
	@echo "TinyEngine: Compiling examples..."
	@cd examples; $(MAKE) --no-print-directory all
	@echo "TinyEngine: Done"

# Build and install Python Module
.PHONY: python
python:
	@echo "TinyEngine: Building Python Module..."
	@cd python; $(MAKE) --no-print-directory all
	@echo "TinyEngine: Done"

# Build the Documentation
.PHONY: doc
doc:
	@echo "TinyEngine: Building Documentation w. Doxygen..."
	@doxygen ./doc/Doxyfile
	@echo "TinyEngine: Done"

# All of the above...
.PHONY: all
all: source examples python