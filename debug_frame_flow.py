#!/usr/bin/env python3
"""Debug frame flow to understand 38 vs 89 dimension mismatch"""

def trace_frame_processing():
    print("=== Frame Processing Analysis ===")
    
    # Our current configuration
    chunk_frames = 160  # Input audio frames
    frame_shift_ms = 10  # 10ms per frame
    sample_rate = 16000
    downsample_factor = 4  # Conformer typical
    
    print(f"1. Input audio chunk: {chunk_frames} frames ({chunk_frames * frame_shift_ms}ms)")
    
    # Convert to samples
    chunk_samples = chunk_frames * (sample_rate * frame_shift_ms // 1000)
    print(f"2. Audio samples: {chunk_samples} samples")
    
    # Feature extraction (mel spectrograms)
    # Our ImprovedFbank config: frame_length=25ms, frame_shift=10ms
    frame_length_samples = int(25 * sample_rate / 1000)  # 400 samples
    frame_shift_samples = int(10 * sample_rate / 1000)   # 160 samples
    
    # Calculate number of feature frames
    feature_frames = (chunk_samples - frame_length_samples) // frame_shift_samples + 1
    print(f"3. Feature frames after mel extraction: {feature_frames}")
    
    # Conformer subsampling (usually 4x)
    after_subsampling = feature_frames // downsample_factor
    print(f"4. After {downsample_factor}x subsampling: {after_subsampling}")
    
    # Check for additional processing
    # Some models do conv layers that reduce size
    possible_conv_reduction = after_subsampling - 2  # Common with conv layers
    print(f"5. After possible conv layers: {possible_conv_reduction}")
    
    print(f"\n=== Key Finding ===")
    print(f"We get {possible_conv_reduction} frames, which matches the error '38 by 89'!")
    
    # What should we get?
    print(f"\n=== Expected Attention Window ===")
    att_context_left = 70
    expected_total = 89
    expected_current = expected_total - att_context_left
    print(f"Expected current chunk frames: {expected_current}")
    print(f"Actual current chunk frames: {possible_conv_reduction}")
    print(f"Difference: {expected_current - possible_conv_reduction}")
    
    print(f"\n=== Potential Solutions ===")
    print("1. Adjust input chunk size to compensate for reductions")
    print("2. Add padding to reach expected dimensions")
    print("3. Check if we're using the right frame configuration")
    
    # Calculate required input to get 19 output frames
    required_after_subsampling = 19 + 2  # Add back conv reduction
    required_feature_frames = required_after_subsampling * downsample_factor
    required_chunk_frames = required_feature_frames  # Approximately
    
    print(f"\nTo get 19 output frames:")
    print(f"- Need ~{required_chunk_frames} input feature frames")
    print(f"- Current: {chunk_frames} frames")
    print(f"- Adjustment needed: {required_chunk_frames - chunk_frames} frames")

if __name__ == "__main__":
    trace_frame_processing()