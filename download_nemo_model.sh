#!/bin/bash

# Download NVidia NeMo FastConformer streaming model for TeraCloud Streams STT
# This script downloads a cache-aware streaming Conformer model exported to ONNX format

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MODELS_DIR="$SCRIPT_DIR/models"
NEMO_MODEL_DIR="$MODELS_DIR/nemo_fastconformer_streaming"

echo "Downloading NVidia NeMo FastConformer streaming model..."
echo "Target directory: $NEMO_MODEL_DIR"

# Create models directory if it doesn't exist
mkdir -p "$NEMO_MODEL_DIR"

# NeMo FastConformer streaming model URL
# Note: This is a placeholder URL - you'll need to replace with actual model download
MODEL_URL="https://api.ngc.nvidia.com/v2/models/nvidia/nemo/stt_en_fastconformer_hybrid_large_streaming_multi/versions/1.20.0/files/stt_en_fastconformer_hybrid_large_streaming_multi.nemo"

# Alternative: HuggingFace model URL (if available)
HF_MODEL_URL="https://huggingface.co/nvidia/stt_en_fastconformer_hybrid_large_streaming_multi"

echo ""
echo "=== NeMo Model Download Instructions ==="
echo ""
echo "IMPORTANT: This script provides instructions for downloading NeMo models."
echo "You'll need to:"
echo ""
echo "1. Export your own NeMo model to ONNX format using:"
echo "   python -c \""
echo "   import nemo.collections.asr as nemo_asr"
echo "   model = nemo_asr.models.EncDecRNNTBPEModel.from_pretrained('nvidia/stt_en_fastconformer_hybrid_large_streaming_multi')"
echo "   model.set_export_config({'cache_support': True})"
echo "   model.export('$NEMO_MODEL_DIR/fastconformer_streaming.onnx')"
echo "   \""
echo ""
echo "2. OR download a pre-exported ONNX model from NGC:"
echo "   wget -O '$NEMO_MODEL_DIR/fastconformer_streaming.onnx' \\"
echo "        'https://catalog.ngc.nvidia.com/models/nvidia:nemo:stt_en_fastconformer_hybrid_large_streaming_multi'"
echo ""
echo "3. OR use the provided sample NeMo export script:"
echo "   python3 export_nemo_to_onnx.py"
echo ""

# Create a sample export script
cat > "$SCRIPT_DIR/export_nemo_to_onnx.py" << 'EOF'
#!/usr/bin/env python3
"""
Export NeMo FastConformer model to ONNX format with cache support
for use with TeraCloud Streams STT toolkit.

Requirements:
    pip install nemo_toolkit[all]
    pip install torch onnx onnxruntime
"""

import os
import sys
import argparse

def export_nemo_to_onnx(model_name, output_path, enable_cache=True):
    """Export NeMo model to ONNX format"""
    try:
        import nemo.collections.asr as nemo_asr
        print(f"Loading NeMo model: {model_name}")
        
        # Load the pre-trained model
        model = nemo_asr.models.EncDecRNNTBPEModel.from_pretrained(model_name)
        
        # Configure for cache-aware streaming if requested
        if enable_cache:
            print("Configuring model for cache-aware streaming...")
            model.set_export_config({'cache_support': True})
        
        # Export to ONNX
        print(f"Exporting to: {output_path}")
        model.export(output_path)
        
        print("Export completed successfully!")
        print(f"Model saved to: {output_path}")
        
        # Verify the exported model
        try:
            import onnx
            onnx_model = onnx.load(output_path)
            print(f"ONNX model verification: OK")
            print(f"Model inputs: {len(onnx_model.graph.input)}")
            print(f"Model outputs: {len(onnx_model.graph.output)}")
            
            for i, input_tensor in enumerate(onnx_model.graph.input):
                print(f"  Input {i}: {input_tensor.name}")
            for i, output_tensor in enumerate(onnx_model.graph.output):
                print(f"  Output {i}: {output_tensor.name}")
                
        except ImportError:
            print("Warning: onnx package not available for verification")
        
        return True
        
    except ImportError as e:
        print(f"Error: NeMo toolkit not available: {e}")
        print("Please install with: pip install nemo_toolkit[all]")
        return False
    except Exception as e:
        print(f"Error exporting model: {e}")
        return False

def main():
    parser = argparse.ArgumentParser(description="Export NeMo model to ONNX")
    parser.add_argument("--model", default="nvidia/stt_en_fastconformer_hybrid_large_streaming_multi",
                       help="NeMo model name")
    parser.add_argument("--output", default="models/nemo_fastconformer_streaming/fastconformer_streaming.onnx",
                       help="Output ONNX file path")
    parser.add_argument("--no-cache", action="store_true", 
                       help="Disable cache support (not recommended for streaming)")
    
    args = parser.parse_args()
    
    # Create output directory
    os.makedirs(os.path.dirname(args.output), exist_ok=True)
    
    # Export the model
    success = export_nemo_to_onnx(
        model_name=args.model,
        output_path=args.output,
        enable_cache=not args.no_cache
    )
    
    if success:
        print("\n=== Next Steps ===")
        print(f"1. Test the model with the STT toolkit:")
        print(f"   cd samples/CppONNX_OnnxSTT")
        print(f"   ./output/bin/standalone --nemo-model {args.output}")
        print(f"2. Integrate with SPL applications")
        sys.exit(0)
    else:
        sys.exit(1)

if __name__ == "__main__":
    main()
EOF

chmod +x "$SCRIPT_DIR/export_nemo_to_onnx.py"

echo "Created NeMo export script: export_nemo_to_onnx.py"
echo ""
echo "To export a NeMo model with cache support:"
echo "  python3 export_nemo_to_onnx.py"
echo ""
echo "To use a custom model:"
echo "  python3 export_nemo_to_onnx.py --model your_model_name --output path/to/output.onnx"
echo ""

# Create placeholder model structure for development
echo "Creating placeholder model structure for development..."
mkdir -p "$NEMO_MODEL_DIR"

cat > "$NEMO_MODEL_DIR/README.md" << 'EOF'
# NeMo FastConformer Streaming Model

This directory should contain your exported NeMo ONNX model.

## Required Files

- `fastconformer_streaming.onnx` - The main model file
- `config.yaml` (optional) - Model configuration
- `tokenizer.model` (optional) - Tokenizer for text processing

## Model Information

- **Architecture**: Cache-aware FastConformer-Hybrid
- **Training**: ~13,000 hours of English speech
- **Parameters**: 114M
- **Chunk Size**: 16 frames (160ms)
- **Latency Options**: 0ms, 80ms, 480ms, 1040ms

## Usage

```cpp
// C++ usage in STT pipeline
auto pipeline = createNeMoPipeline("models/nemo_fastconformer_streaming/fastconformer_streaming.onnx", true);
```

```bash
# Command line usage
./standalone --nemo-model models/nemo_fastconformer_streaming/fastconformer_streaming.onnx
```

## Export Instructions

1. Install NeMo toolkit: `pip install nemo_toolkit[all]`
2. Run the export script: `python3 ../export_nemo_to_onnx.py`
3. Test with the STT toolkit

EOF

echo "Created model directory structure: $NEMO_MODEL_DIR"
echo ""
echo "=== Download Complete ==="
echo ""
echo "Next steps:"
echo "1. Export a NeMo model to ONNX format (see instructions above)"
echo "2. Place the ONNX file in: $NEMO_MODEL_DIR/"
echo "3. Test with the STT toolkit samples"
echo "4. Integrate with your SPL applications"
echo ""