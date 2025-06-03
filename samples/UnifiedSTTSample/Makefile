# Unified STT Sample Makefile
# 
# This Makefile allows easy switching between different STT operators:
#   make onnx    - Test FastConformer ONNX model
#   make nemo    - Test NeMo .nemo model
#   make clean   - Clean all outputs

# Configuration
STREAMS_INSTALL ?= $(shell echo $$STREAMS_INSTALL)
SPLC = $(STREAMS_INSTALL)/bin/sc
TOOLKIT_PATH = ../../..
SPLC_FLAGS = -a -t $(TOOLKIT_PATH)

# Sample selection (can be overridden)
SAMPLE_TYPE ?= onnx
OUTPUT_DIR = output_$(SAMPLE_TYPE)

# Composite names for each sample type
ifeq ($(SAMPLE_TYPE),onnx)
    SPL_MAIN_COMPOSITE = UnifiedSTTSample::OnnxSTTDemo
    MODEL_DESC = FastConformer ONNX
else ifeq ($(SAMPLE_TYPE),nemo)
    SPL_MAIN_COMPOSITE = UnifiedSTTSample::NeMoSTTDemo
    MODEL_DESC = NeMo FastConformer
else
    $(error Invalid SAMPLE_TYPE: $(SAMPLE_TYPE). Use 'onnx' or 'nemo')
endif

# Default target
.PHONY: all onnx nemo clean run-onnx run-nemo test compare help

all: help

help:
	@echo "Unified STT Sample - Build and Test Options"
	@echo "==========================================="
	@echo ""
	@echo "Build targets:"
	@echo "  make onnx     - Build ONNX FastConformer sample"
	@echo "  make nemo     - Build NeMo native model sample"  
	@echo "  make clean    - Clean all build outputs"
	@echo ""
	@echo "Run targets:"
	@echo "  make run-onnx - Build and run ONNX sample"
	@echo "  make run-nemo - Build and run NeMo sample"
	@echo ""
	@echo "Test targets:"
	@echo "  make test     - Run both samples and compare"
	@echo "  make compare  - Compare previous outputs"
	@echo ""
	@echo "Configuration:"
	@echo "  AUDIO_FILE    - Audio file to process (default: IBM culture)"
	@echo "  PROVIDER      - ONNX provider: CPU, CUDA, TensorRT"
	@echo "  THREADS       - Number of threads for ONNX"
	@echo ""

# Build targets
onnx:
	@echo "Building ONNX FastConformer sample..."
	$(MAKE) build SAMPLE_TYPE=onnx

nemo:
	@echo "Building NeMo FastConformer sample..."
	$(MAKE) build SAMPLE_TYPE=nemo

build:
	@echo "Building $(MODEL_DESC) sample..."
	@echo "Composite: $(SPL_MAIN_COMPOSITE)"
	@echo "Output: $(OUTPUT_DIR)"
	$(SPLC) $(SPLC_FLAGS) -M $(SPL_MAIN_COMPOSITE) --output-directory $(OUTPUT_DIR)

# Run targets  
run-onnx: onnx
	@echo "Running ONNX FastConformer sample..."
	@echo "========================================"
	./$(OUTPUT_DIR)/bin/standalone

run-nemo: nemo
	@echo "Running NeMo FastConformer sample..."
	@echo "====================================="
	./$(OUTPUT_DIR)/bin/standalone

# Test and comparison
test: clean
	@echo "Running comprehensive STT comparison..."
	@echo "======================================="
	@echo ""
	@echo "1. Testing ONNX FastConformer..."
	$(MAKE) run-onnx > test_results_onnx.txt 2>&1 || true
	@echo ""
	@echo "2. Testing NeMo FastConformer..."
	$(MAKE) run-nemo > test_results_nemo.txt 2>&1 || true
	@echo ""
	@echo "3. Generating comparison report..."
	$(MAKE) compare

compare:
	@echo "STT Sample Comparison Report"
	@echo "============================"
	@echo ""
	@if [ -f test_results_onnx.txt ]; then \
		echo "ONNX Results:"; \
		echo "-------------"; \
		grep -E "(Model Type:|Total processing time:|Average confidence:|FULL TRANSCRIPT:)" test_results_onnx.txt || echo "No results found"; \
		echo ""; \
	fi
	@if [ -f test_results_nemo.txt ]; then \
		echo "NeMo Results:"; \
		echo "-------------"; \
		grep -E "(Model Type:|Total processing time:|Average confidence:|FULL TRANSCRIPT:)" test_results_nemo.txt || echo "No results found"; \
		echo ""; \
	fi
	@echo "Detailed logs: test_results_onnx.txt, test_results_nemo.txt"

# Advanced configuration targets
config-fast:
	@echo "Configuring for fast testing (small chunks)..."
	$(MAKE) run-onnx BLOCK_SIZE=1600 CHUNK_SIZE=50

config-accurate:
	@echo "Configuring for accurate transcription (larger chunks)..."
	$(MAKE) run-onnx BLOCK_SIZE=6400 CHUNK_SIZE=200

config-gpu:
	@echo "Configuring for GPU acceleration..."
	$(MAKE) run-onnx PROVIDER=CUDA THREADS=1

# Clean targets
clean:
	@echo "Cleaning all outputs..."
	$(SPLC) $(SPLC_FLAGS) -C -M UnifiedSTTSample::OnnxSTTDemo 2>/dev/null || true
	$(SPLC) $(SPLC_FLAGS) -C -M UnifiedSTTSample::NeMoSTTDemo 2>/dev/null || true
	rm -rf output_*
	rm -f test_results_*.txt

# Development helpers
check-models:
	@echo "Checking model availability..."
	@echo "ONNX Models:"
	@ls -la ../../models/nemo_fastconformer_streaming/ 2>/dev/null || echo "  ONNX models not found"
	@echo ""
	@echo "NeMo Models:"  
	@ls -la ../../models/nemo_fastconformer_direct/ 2>/dev/null || echo "  NeMo models not found"
	@echo ""
	@echo "Test Audio:"
	@ls -la ../../test_data/audio/11-ibm-culture-2min-16k.wav 2>/dev/null || echo "  Test audio not found"

check-toolkit:
	@echo "Checking toolkit status..."
	@if [ -f $(TOOLKIT_PATH)/toolkit.xml ]; then \
		echo "✓ Toolkit indexed"; \
		grep -c "primitiveOp.*name=" $(TOOLKIT_PATH)/toolkit.xml | sed 's/^/  Operators: /'; \
	else \
		echo "✗ Toolkit not indexed - run: cd $(TOOLKIT_PATH) && spl-make-toolkit -i . --no-mixed-mode -m"; \
	fi

# Parameter passing for runtime configuration
ifdef AUDIO_FILE
    RUNTIME_PARAMS += -P audioFile=$(AUDIO_FILE)
endif
ifdef PROVIDER  
    RUNTIME_PARAMS += -P provider=$(PROVIDER)
endif
ifdef THREADS
    RUNTIME_PARAMS += -P numThreads=$(THREADS)
endif
ifdef BLOCK_SIZE
    RUNTIME_PARAMS += -P blockSize=$(BLOCK_SIZE)
endif
ifdef CHUNK_SIZE
    RUNTIME_PARAMS += -P chunkSizeMs=$(CHUNK_SIZE)
endif

ifneq ($(RUNTIME_PARAMS),)
    run-onnx run-nemo: STANDALONE_CMD = ./$(OUTPUT_DIR)/bin/standalone $(RUNTIME_PARAMS)
else
    run-onnx run-nemo: STANDALONE_CMD = ./$(OUTPUT_DIR)/bin/standalone
endif