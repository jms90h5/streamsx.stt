# Main Makefile for TeraCloud Streams Speech-to-Text Toolkit

.PHONY: all clean

# Build SPL toolkit index
all:
	@echo "Building SPL toolkit index..."
	$(STREAMS_INSTALL)/bin/spl-make-toolkit -i .

clean:
	@echo "Cleaning toolkit..."
	rm -f toolkit.xml

# Build everything (implementation + toolkit)
build-all: 
	@echo "Building implementation library..."
	$(MAKE) -C impl
	@echo "Building SPL toolkit..."
	$(MAKE) all

# Clean everything
clean-all:
	@echo "Cleaning implementation..."
	$(MAKE) -C impl clean
	@echo "Cleaning toolkit..."
	$(MAKE) clean