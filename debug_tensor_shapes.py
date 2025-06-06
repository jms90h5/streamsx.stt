#!/usr/bin/env python3
"""Debug tensor shape issues in NeMo cache-aware streaming"""

import numpy as np

def analyze_dimension_mismatch():
    """Analyze the 38 vs 89 dimension mismatch"""
    
    print("=== Dimension Analysis ===")
    
    # Configuration
    chunk_frames = 160  # Input frames
    downsample_factor = 4
    att_context_left = 70
    att_context_right = 0
    
    # Calculate downsampled size
    downsampled_frames = chunk_frames // downsample_factor
    print(f"Input chunk: {chunk_frames} frames")
    print(f"After 4x downsampling: {downsampled_frames} frames")
    
    # The error shows 38 vs 89
    # 38 might be coming from actual processing
    # Let's check different scenarios
    
    print("\n=== Possible Sources of 38 ===")
    
    # Scenario 1: Padding issues
    # If we pad to nearest multiple of something
    padded_40_to_nearest_4 = (downsampled_frames // 4) * 4
    print(f"1. Padded to nearest 4: {padded_40_to_nearest_4}")
    
    # Scenario 2: Convolutional downsampling
    # Some conv layers might reduce size
    after_conv = downsampled_frames - 2  # Common with kernel_size=3, stride=1, no padding
    print(f"2. After conv (k=3, s=1, p=0): {after_conv}")
    
    # Scenario 3: Subsampling in attention
    # The model might do additional subsampling
    print(f"3. 40 - 2 = {40 - 2} (matches 38!)")
    
    print("\n=== Attention Context Analysis ===")
    print(f"Left context: {att_context_left}")
    print(f"Expected attention size: {att_context_left + 19} = 89")
    print(f"Actual left side: 38")
    print(f"Difference: {89 - 38} = 51 frames")
    
    print("\n=== Possible Fix ===")
    print("The model seems to expect:")
    print("- Input gets reduced from 40 to 38 frames somewhere")
    print("- Attention needs to see 89 frames total")
    print("- We might need to adjust how we prepare the audio signal tensor")
    
    # Check if padding would help
    print("\n=== Padding Strategy ===")
    frames_needed = 89 - att_context_left  # How many frames from current chunk
    print(f"Frames needed from chunk: {frames_needed} (to reach 89 total)")
    print(f"But we only have: 38")
    print(f"Missing: {frames_needed - 38} = {19 - 38} (negative means we have too few)")

if __name__ == "__main__":
    analyze_dimension_mismatch()