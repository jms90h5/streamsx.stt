# Makefile for NeMo CTC Speech-to-Text Samples
# Builds sample applications using the working NeMo CTC implementation

STREAMS_INSTALL ?= $(HOME)/teracloud/streams/7.2.0.0
TOOLKIT_PATH = ..

# Samples to build
SAMPLES = BasicNeMoDemo NeMoCTCRealtime NeMoFileTranscription

# Default target
all: $(SAMPLES)

# Ensure toolkit is built first
check-toolkit:
	@echo "Checking if toolkit is built..."
	@if [ ! -f "$(TOOLKIT_PATH)/impl/lib/libnemo_ctc_interface.so" ]; then \
		echo "Building toolkit first..."; \
		$(MAKE) -C $(TOOLKIT_PATH); \
	fi

# Build individual samples
BasicNeMoDemo: check-toolkit
	@echo "Building BasicNeMoDemo..."
	@mkdir -p output/BasicNeMoDemo
	sc -a -t $(TOOLKIT_PATH) -M BasicNeMoDemo --output-directory output/BasicNeMoDemo
	@echo "✅ BasicNeMoDemo built successfully"

NeMoCTCRealtime: check-toolkit
	@echo "Building NeMoCTCRealtime..."
	@mkdir -p output/NeMoCTCRealtime
	sc -a -t $(TOOLKIT_PATH) -M NeMoCTCRealtime --output-directory output/NeMoCTCRealtime
	@echo "✅ NeMoCTCRealtime built successfully"

NeMoFileTranscription: check-toolkit
	@echo "Building NeMoFileTranscription..."
	@mkdir -p output/NeMoFileTranscription
	sc -a -t $(TOOLKIT_PATH) -M NeMoFileTranscription --output-directory output/NeMoFileTranscription
	@echo "✅ NeMoFileTranscription built successfully"

# Clean build artifacts
clean:
	rm -rf output/
	@echo "✅ Cleaned build artifacts"

# Test samples (requires models and test data)
test: all
	@echo "Testing samples..."
	@echo ""
	@echo "=== Testing BasicNeMoDemo (unthrottled) ==="
	@if [ -f "$(TOOLKIT_PATH)/models/fastconformer_ctc_export/model.onnx" ]; then \
		cd output/BasicNeMoDemo && ./bin/standalone; \
		echo ""; \
		echo "=== Testing BasicNeMoDemo (realtime 1x) ==="; \
		streamtool submitjob BasicNeMoDemo.sab -P realtimePlayback=true; \
	else \
		echo "❌ Missing models. Run model export first."; \
	fi

# Test individual samples with different modes
test-basic:
	@echo "Testing BasicNeMoDemo..."
	@cd output/BasicNeMoDemo && ./bin/standalone
	@echo ""
	@echo "Testing BasicNeMoDemo with realtime playback..."
	@cd output/BasicNeMoDemo && streamtool submitjob BasicNeMoDemo.sab -P realtimePlayback=true

test-realtime:
	@echo "Testing NeMoCTCRealtime (fast mode)..."
	@cd output/NeMoCTCRealtime && ./bin/standalone
	@echo ""
	@echo "Testing NeMoCTCRealtime (1x playback, 256ms chunks)..."
	@cd output/NeMoCTCRealtime && streamtool submitjob NeMoCTCRealtime.sab \
		-P realtimePlayback=true -P chunkSizeMs=256

test-file:
	@echo "Testing NeMoFileTranscription (fast mode)..."
	@cd output/NeMoFileTranscription && ./bin/standalone
	@echo ""
	@echo "Testing NeMoFileTranscription (1x playback)..."
	@cd output/NeMoFileTranscription && streamtool submitjob NeMoFileTranscription.sab \
		-P realtimePlayback=true

# Setup required models and test data
setup-models:
	@echo "Checking models and test data..."
	@if [ ! -d "$(TOOLKIT_PATH)/models/fastconformer_ctc_export" ]; then \
		echo "❌ NeMo CTC model not found. Please run model export first:"; \
		echo "   cd $(TOOLKIT_PATH) && python export_proven_working_model.py"; \
		exit 1; \
	fi
	@if [ ! -f "$(TOOLKIT_PATH)/test_data/audio/librispeech-1995-1837-0001.wav" ]; then \
		echo "❌ Test audio files not found. Please ensure test data is available."; \
		exit 1; \
	fi
	@echo "✅ Models and test data are ready"

# Show sample status
status:
	@echo "=== NeMo CTC Samples Status ==="
	@echo "Toolkit path: $(TOOLKIT_PATH)"
	@echo "Streams install: $(STREAMS_INSTALL)"
	@echo ""
	@echo "Available samples:"
	@for sample in $(SAMPLES); do \
		if [ -f "output/$$sample/$$sample.sab" ]; then \
			echo "  ✅ $$sample (built)"; \
		else \
			echo "  ⏸️  $$sample (not built)"; \
		fi; \
	done
	@echo ""
	@echo "Throttling support:"
	@echo "  ✅ All samples support -P realtimePlayback=true"
	@echo "  ✅ Default mode: Process as fast as possible"
	@echo "  ✅ Realtime mode: Process at 1x audio speed"
	@echo ""
	@echo "Required models:"
	@if [ -f "$(TOOLKIT_PATH)/models/fastconformer_ctc_export/model.onnx" ]; then \
		echo "  ✅ NeMo CTC model ($(shell stat -c%s $(TOOLKIT_PATH)/models/fastconformer_ctc_export/model.onnx 2>/dev/null | numfmt --to=iec)B)"; \
	else \
		echo "  ❌ NeMo CTC model (missing)"; \
	fi
	@if [ -f "$(TOOLKIT_PATH)/models/fastconformer_ctc_export/tokens.txt" ]; then \
		echo "  ✅ Vocabulary file ($(shell wc -l < $(TOOLKIT_PATH)/models/fastconformer_ctc_export/tokens.txt 2>/dev/null) tokens)"; \
	else \
		echo "  ❌ Vocabulary file (missing)"; \
	fi

# Help target
help:
	@echo "NeMo CTC Speech-to-Text Samples with Realtime Playback Support"
	@echo "=============================================================="
	@echo ""
	@echo "Available targets:"
	@echo "  all              - Build all samples"
	@echo "  BasicNeMoDemo    - Build basic demo sample"
	@echo "  NeMoCTCRealtime  - Build real-time processing sample"
	@echo "  NeMoFileTranscription - Build file transcription sample"
	@echo "  clean            - Remove build artifacts"
	@echo "  test             - Test all samples"
	@echo "  test-basic       - Test BasicNeMoDemo in both modes"
	@echo "  test-realtime    - Test NeMoCTCRealtime with different settings"
	@echo "  test-file        - Test NeMoFileTranscription in both modes"
	@echo "  setup-models     - Check model and test data availability"
	@echo "  status           - Show build and dependency status"
	@echo "  help             - Show this help"
	@echo ""
	@echo "Realtime Playback Feature:"
	@echo "  All samples support optional 1x speed audio processing"
	@echo "  Usage: streamtool submitjob <sample>.sab -P realtimePlayback=true"
	@echo ""
	@echo "Prerequisites:"
	@echo "  1. Source Streams environment: source $(STREAMS_INSTALL)/bin/streamsprofile.sh"
	@echo "  2. Build toolkit: make -C $(TOOLKIT_PATH)"
	@echo "  3. Export NeMo model: cd $(TOOLKIT_PATH) && python export_proven_working_model.py"
	@echo ""
	@echo "Quick start:"
	@echo "  make status      # Check dependencies"
	@echo "  make all         # Build all samples"
	@echo "  make test        # Test samples in both modes"

.PHONY: all clean test test-basic test-realtime test-file setup-models status help check-toolkit $(SAMPLES)