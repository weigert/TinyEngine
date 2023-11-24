# TinyEngine
# Make Rules

# Version: 1.8
# Author: Nicholas McDonald
# Tested on GNU/Linux

# Build and Install TinyEngine
.PHONY: source
source:
	@echo "TinyEngine: Building and Installing from source..."
	@cd source; $(MAKE) all

# Build all Examples
.PHONY: examples
examples:
	@echo "TinyEngine: Compiling examples..."
	@cd examples; $(MAKE) all

# Build and install Python Module
.PHONY: python
python:
	@echo "TinyEngine: Building Python Module..."
	@cd python; $(MAKE) all

# All of the above...
.PHONY: all
all: source examples python