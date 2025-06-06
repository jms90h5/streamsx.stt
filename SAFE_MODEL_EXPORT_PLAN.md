# Safe FastConformer Decoder Model Export Plan

**Date**: June 1, 2025  
**Issue**: Decoder model corrupted (38KB file) due to previous export crash  
**Goal**: Export working decoder model safely without memory exhaustion  

## Analysis of Previous Export Attempts

### What Worked (From Success Documentation)
1. **Encoder export succeeded**: 436MB encoder model exists and loads correctly
2. **Cache-aware export succeeded**: Encoder has proper cache tensors with `cache_support=True`
3. **Dual model architecture**: FastConformer-Hybrid exports encoder and decoder separately

### What Failed
1. **Decoder export crashed**: Only 38KB corrupted file remained
2. **Memory exhaustion**: Likely crashed during decoder ONNX conversion
3. **Protobuf parsing error**: Current decoder file is unreadable

### Scripts Analysis (Best to Worst)

#### ✅ SAFEST APPROACH: `final_working_export.py`
- **Memory efficient**: Loads model once, exports to separate files
- **Proven approach**: Uses pytorch_lightning fix that worked before
- **Explicit cache support**: `model.set_export_config({'cache_support': 'True'})`
- **Separate encoder/decoder export**: `model.export(encoder_path, decoder_path)`

#### ⚠️ RISKY: `download_and_export_fresh.py`  
- **Memory intensive**: Downloads 439MB model fresh (memory doubling)
- **May crash**: Large memory allocation during download + export

#### ❌ AVOID: Multiple individual scripts
- **Complex dependency fixes**: Many scripts try different pytorch_lightning hacks
- **Inconsistent results**: Different cache_support configurations

## Recommended Safe Export Command

Based on the successful approach from `final_working_export.py`, here's the safest command:

### **COMMAND TO RUN**:
```bash
cd /homes/jsharpe/teracloud/com.teracloud.streamsx.stt
python final_working_export.py
```

## Why This Approach is Safe

### 1. **Memory Efficient**
- Uses existing downloaded model (`models/nemo_fresh_export/fresh_fastconformer.nemo`)
- No additional model downloads
- Single model load, dual export

### 2. **Proven Fix**
- Contains the pytorch_lightning.utilities.combined_loader fix that worked
- Same approach that successfully exported the encoder

### 3. **Explicit Cache Support**
```python
model.set_export_config({'cache_support': 'True'})
```

### 4. **Dual Export Pattern**
```python
# Export both encoder and decoder
encoder_path = "models/nemo_fresh_export/encoder-fastconformer_cache_final.onnx"
decoder_path = "models/nemo_fresh_export/decoder_joint-fastconformer_cache_final.onnx"
model.export(encoder_path, decoder_path)
```

## Expected Results

### ✅ **Success Indicators**:
- Decoder file size: ~20-40MB (reasonable for hybrid decoder)
- No "Protobuf parsing failed" errors
- Both encoder and decoder load in C++ test
- Cache tensors present in both models

### ❌ **Failure Indicators**:
- File size < 1MB or > 100MB 
- Memory allocation errors
- pytorch_lightning import errors
- ONNX Runtime protobuf errors

## Backup Plan (If Safe Export Fails)

### Alternative 1: Use Existing Working Model
If the export still fails, we can:
1. **Use wav2vec2 model**: Known working alternative in `models/wav2vec2_base.onnx`
2. **Demonstrate tensor fix**: Show that broadcasting error is resolved
3. **Complete Streams application**: Use working model for final deliverable

### Alternative 2: Minimal Decoder Export
```bash
python export_nemo_cache_minimal.py
```
- Smaller memory footprint
- CTC-only decoder (no RNN-T complexity)

## What NOT to Run

### ❌ **AVOID THESE SCRIPTS** (High crash risk):
- `download_and_export_fresh.py` - Downloads 439MB + exports (memory doubling)
- `comprehensive_nemo_fix.py` - Complex multi-step process
- `export_nemo_dynamic.py` - Experimental approach
- Any script with "fix_dependencies" - Complex dependency manipulation

## Monitoring During Export

### Watch for these warning signs:
1. **Memory usage spike** - Monitor with `top` or `htop`
2. **Long silence** - Export should show progress messages
3. **Python process hanging** - Kill if no output for >5 minutes
4. **Disk space issues** - Ensure >2GB free space

## Recovery if Export Crashes

### If it crashes again:
1. **Stop immediately** - Don't retry the same approach
2. **Use Alternative 1** - Fall back to wav2vec2 model
3. **Document the issue** - Add to crash recovery documentation
4. **Focus on Streams app** - Complete the deliverable with working model

---

**RECOMMENDATION**: Run `python final_working_export.py` - this is the safest approach based on previous successful patterns.