# Streams Application Implementation TODO

**Date**: June 1, 2025  
**Goal**: Complete working Streams application with C++ toolkit operators  
**Status**: Need to complete implementation and create SPL application  

## Current Situation

### What We Have ✅
1. **C++ Implementation**: NeMoCacheAwareConformer class with tensor fix
2. **ONNX Models**: Encoder (435.7MB) and Decoder (20.3MB) exported correctly
3. **Build System**: Clean compilation with ONNX Runtime integration
4. **Partial Success**: Validated tensor dimension fix approach

### What We Need 🔧
1. **Fix chunk alignment**: Resolve "20 by 89" error (configuration issue)
2. **Complete C++ test**: Get accurate transcription from librispeech audio
3. **SPL Operator**: Create/update NeMoSTT operator wrapper
4. **Streams Application**: Build complete SPL application
5. **Verified Output**: Accurate transcription of test audio

## Implementation Plan

### Step 1: Fix Chunk Alignment Issue
The model expects chunks that produce exactly 19 frames after downsampling:
- Current: 158 input frames → 20 output frames
- Needed: 152 input frames → 19 output frames
- Fix: Adjust chunk_frames = 152

### Step 2: Ensure Cache Accumulation
- Debug why cache isn't updating between chunks
- Verify cache tensors are properly sized and copied
- Ensure current_cache_frames increments correctly

### Step 3: Complete C++ Standalone Test
- Get working transcription with test_nemo_cache_aware_dual
- Verify output matches expected librispeech content
- Confirm no "sh sh sh" repetition or empty output

### Step 4: Create/Update SPL Operator
Location: `com.teracloud.streamsx.stt/NeMoSTT/`
- Update operator XML definition
- Ensure C++ code generation templates work
- Integrate with toolkit build system

### Step 5: Build Streams Application
Create complete SPL application that:
1. Reads audio file (FileAudioSource)
2. Processes through NeMoSTT operator
3. Outputs transcription results
4. Handles streaming chunks properly

### Step 6: Test and Validate
- Run application with librispeech audio
- Verify accurate transcription output
- Test with multiple audio files
- Ensure no memory leaks or crashes

## Technical Requirements

### C++ Operator Requirements
- Must implement SPL operator interface
- Handle tuple processing for streaming
- Manage cache state across tuples
- Output transcription results as tuples

### SPL Application Structure
```spl
composite NeMoTranscriptionApp {
    graph
        stream<blob audio> AudioStream = FileSource() {
            param
                file: "test_data/audio/librispeech-1995-1837-0001.wav";
        }
        
        stream<rstring text> Transcription = NeMoSTT(AudioStream) {
            param
                modelPath: getThisToolkitDir() + "/models/nemo_fresh_export";
                chunkSize: 152;  // Calibrated for 19 frames
                enableCache: true;
        }
        
        () as Sink = FileSink(Transcription) {
            param
                file: "transcription_output.txt";
        }
}
```

## Expected Librispeech Transcription
The test file should produce something like:
"HE HOPED THERE WOULD BE STEW FOR DINNER TURNIPS AND CARROTS AND BRUISED POTATOES AND FAT MUTTON PIECES TO BE LADLED OUT IN THICK PEPPERED FLOUR FATTENED SAUCE"

## Success Criteria
1. ✅ No tensor broadcasting errors
2. ✅ Cache properly accumulates across chunks
3. ✅ Accurate transcription output
4. ✅ Clean SPL application build
5. ✅ Runs as Streams job successfully

## Current Blockers
1. Chunk size alignment (19 vs 20 frames)
2. Cache not accumulating between chunks
3. SPL operator integration not complete
4. No working end-to-end demonstration

## Priority Actions
1. **HIGH**: Fix chunk alignment in C++ test
2. **HIGH**: Debug cache accumulation issue
3. **MEDIUM**: Create SPL operator wrapper
4. **MEDIUM**: Build complete Streams application
5. **LOW**: Optimize performance and add error handling