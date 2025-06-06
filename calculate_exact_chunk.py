#!/usr/bin/env python3
"""Calculate exact chunk size for librispeech test file"""

def calculate_exact_chunk():
    print("=== Exact Chunk Size Calculation ===")
    
    # Data points from our tests:
    data_points = [
        (84, 29),   # 84 chunk frames → 29 output frames
        (55, 25),   # 55 chunk frames → 25 output frames
    ]
    
    # Calculate the relationship
    chunk1, output1 = data_points[0]
    chunk2, output2 = data_points[1]
    
    # Linear relationship: output = a * chunk + b
    a = (output2 - output1) / (chunk2 - chunk1)
    b = output1 - a * chunk1
    
    print(f"Relationship: output = {a:.4f} * chunk + {b:.4f}")
    
    # Target: 19 output frames
    target_output = 19
    target_chunk = (target_output - b) / a
    
    print(f"For {target_output} output frames: chunk = {target_chunk:.1f}")
    
    # Try nearby integer values
    candidates = [int(target_chunk), int(target_chunk) + 1, int(target_chunk) - 1]
    
    print(f"\nCandidates to try:")
    for candidate in candidates:
        predicted = a * candidate + b
        print(f"  {candidate} frames → {predicted:.1f} output")
    
    # Also check if we need to target a different output
    # The error shows "25 by 76", so maybe we need 76-25=51 more frames
    # Or maybe the target isn't 19
    print(f"\nAlternative analysis:")
    print(f"Current: 25 by 76 (difference: {76-25})")
    print(f"Maybe we need {76} total frames in attention?")
    
    if 76 > 25:
        # Total attention = context + current
        context_size = 76 - 25  # 51
        print(f"Implied context size: {context_size}")
        print(f"To get 76 total: need {76-context_size} = {25} current frames")
        print(f"We're already at 25, so this might be close!")
    
    return int(target_chunk)

if __name__ == "__main__":
    result = calculate_exact_chunk()
    print(f"\nRECOMMENDATION: Try chunk_frames = {result}")