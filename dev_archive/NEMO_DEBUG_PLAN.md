# NeMo Model Debugging Plan

## Problem Statement
The NeMo model outputs token 95 ("that") with probability -2.91337 for EVERY time step, regardless of:
- Audio content (IBM culture, test files, silence)
- Audio format (8kHz, 16kHz) 
- Audio quality/length

This indicates a fundamental model integration issue.

## Potential Root Causes

### 1. ONNX Model Export Issues
- Model might be exported incorrectly from NeMo
- Dynamic shapes not handled properly
- Model architecture mismatch

### 2. Feature Preprocessing Mismatch
- Model expects different feature normalization
- Wrong mel-filterbank parameters
- CMVN normalization incorrect
- Feature scaling/offset wrong

### 3. Model Input Format Issues
- Wrong tensor shapes/order
- Missing model inputs (cache, lengths, etc.)
- Incorrect data types (float32 vs float16)

### 4. Model State Issues
- Model stuck in initialization state
- Cache not properly reset between chunks
- Model requires warm-up sequences

## Investigation Steps

### Step 1: Verify Model Architecture
```bash
# Check ONNX model details
python3 -c "
import onnx
model = onnx.load('models/nemo_fastconformer_streaming/conformer_ctc_dynamic.onnx')
print('Model inputs:')
for input in model.graph.input:
    print(f'  {input.name}: {input.type}')
print('Model outputs:')
for output in model.graph.output:
    print(f'  {output.name}: {output.type}')
"
```

### Step 2: Test with Known-Good NeMo Model
- Download official NeMo checkpoint
- Export to ONNX using official NeMo tools
- Compare outputs

### Step 3: Feature Analysis
- Compare input features with working Python NeMo inference
- Test with zero/constant input to isolate model behavior
- Verify feature statistics match training data

### Step 4: Model Input Debugging
- Test with different input shapes
- Try batch_size=1 vs dynamic
- Test with fixed-length sequences

## Immediate Actions (Next 2 Hours)

### Action 1: Get Working NeMo Model (30 min)
Download and test official NeMo model:
```bash
# Download from NGC or GitHub
wget https://api.ngc.nvidia.com/v2/models/nvidia/nemo/stt_en_conformer_ctc_small/versions/1.15.0/files/stt_en_conformer_ctc_small.nemo
```

### Action 2: Test with Python NeMo (30 min)
Create reference implementation:
```python
import nemo.collections.asr as nemo_asr

# Load model
model = nemo_asr.models.EncDecCTCModel.from_pretrained("stt_en_conformer_ctc_small")

# Test with IBM audio
transcription = model.transcribe(["test_data/11-ibm-culture-2min.wav"])
print(f"Reference transcription: {transcription}")
```

### Action 3: Fix Current Model (60 min)
Based on findings from Actions 1-2:
- Fix feature preprocessing
- Correct model inputs
- Implement proper ONNX inference