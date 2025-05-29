# Makefile for Generic STT Sample

.PHONY: all clean

SPLC = $(STREAMS_INSTALL)/bin/sc
SPLC_FLAGS = -a

all: standalone

standalone: StreamsNeMoTest.spl
	$(SPLC) $(SPLC_FLAGS) -T -M StreamsNeMoTest

clean:
	rm -rf output

# Run examples with different models
run-nemo:
	./output/bin/standalone \
		audioFile=../../test_data/audio/11-ibm-culture-2min-16k.wav \
		modelType=nemo \
		modelPath=../../models/nemo_cache_aware_conformer/models--nvidia--stt_en_fastconformer_hybrid_large_streaming_multi/snapshots/ae98143333690bd7ced4bc8ec16769bcb8918374/stt_en_fastconformer_hybrid_large_streaming_multi.nemo \
		outputFile=nemo_transcript.txt

run-onnx:
	./output/bin/standalone \
		audioFile=../../test_data/audio/11-ibm-culture-2min-16k.wav \
		modelType=onnx \
		modelPath=../../models/sherpa-onnx-streaming-zipformer-bilingual-zh-en-2023-02-20/encoder-epoch-99-avg-1.onnx \
		outputFile=onnx_transcript.txt

help:
	@echo "Usage:"
	@echo "  make all       - Build the application"
	@echo "  make run-nemo  - Run with NeMo model"
	@echo "  make run-onnx  - Run with ONNX model"
	@echo "  make clean     - Clean build artifacts"