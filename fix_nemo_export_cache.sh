#!/bin/bash
# Fix NeMo export by using the correct cache-aware export method

echo "=== Fixing NeMo ONNX Export with Cache Support ==="
echo ""

# Check if model exists
MODEL_PATH="models/nemo_fastconformer_direct/stt_en_fastconformer_hybrid_large_streaming_multi.nemo"
if [ ! -f "$MODEL_PATH" ]; then
    echo "Error: Model not found at $MODEL_PATH"
    exit 1
fi

# Create output directory
OUTPUT_DIR="models/nemo_cache_aware_fixed"
mkdir -p "$OUTPUT_DIR"

# Copy the working export script from checkpoint
echo "Using the export script that worked before..."
cp checkpoint3_extracted/com.teracloud.streamsx.stt/export_nemo_cache_aware_onnx.py export_nemo_fixed.py

# Update paths in the script
sed -i 's|models/nemo_cache_aware_conformer/models--nvidia.*\.nemo|models/nemo_fastconformer_direct/stt_en_fastconformer_hybrid_large_streaming_multi.nemo|g' export_nemo_fixed.py
sed -i 's|models/nemo_cache_aware_onnx|models/nemo_cache_aware_fixed|g' export_nemo_fixed.py

echo "Running export with cache support..."
echo ""

# Run with limited memory to avoid crashes
export PYTORCH_CUDA_ALLOC_CONF=max_split_size_mb:512
export OMP_NUM_THREADS=1

python3 export_nemo_fixed.py

# Check if export succeeded
if [ -f "$OUTPUT_DIR/fastconformer_encoder_cache_aware.onnx" ]; then
    echo ""
    echo "✅ Export successful!"
    echo ""
    echo "Checking model structure..."
    python3 -c "
import onnx
model = onnx.load('$OUTPUT_DIR/fastconformer_encoder_cache_aware.onnx')
print(f'Inputs: {len(model.graph.input)}')
for inp in model.graph.input:
    print(f'  - {inp.name}')
print(f'\\nOutputs: {len(model.graph.output)}')
for out in model.graph.output:
    print(f'  - {out.name}')
"
else
    echo "❌ Export failed. Trying alternative method..."
    
    # Alternative: Use the checkpoint3 script directly
    echo ""
    echo "Running original working script..."
    cd checkpoint3_extracted/com.teracloud.streamsx.stt
    python3 export_nemo_cache_aware_onnx.py
fi