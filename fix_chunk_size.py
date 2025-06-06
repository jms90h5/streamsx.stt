#!/usr/bin/env python3
"""Calculate correct chunk size for NeMo cache-aware streaming"""

def calculate_optimal_chunk_size():
    print("=== Optimal Chunk Size Calculation ===")
    
    # From NeMo documentation and examples
    # "chunk_size of 100 would be 100*4*10=4000ms for a model with 4x downsampling and 10ms shift"
    
    # We want 19 frames after all processing
    target_output_frames = 19
    
    # Working backwards:
    # 1. Add back conv layer reduction (typically 2 frames)
    after_conv = target_output_frames + 2  # 21 frames
    print(f"1. After conv adjustment: {after_conv} frames")
    
    # 2. Account for 4x downsampling 
    before_subsampling = after_conv * 4  # 84 frames
    print(f"2. Before subsampling (4x): {before_subsampling} frames")
    
    # 3. This should be our input chunk size
    optimal_chunk_frames = before_subsampling
    optimal_chunk_ms = optimal_chunk_frames * 10  # 10ms per frame
    
    print(f"3. Optimal input chunk: {optimal_chunk_frames} frames ({optimal_chunk_ms}ms)")
    
    # Verify with NeMo's example
    print(f"\n=== Verification with NeMo Example ===")
    # If chunk_size=100 gives 4000ms with 4x downsampling
    # Then chunk_size=21 should give us what we need
    nemo_equivalent = target_output_frames + 2  # ~21
    nemo_ms = nemo_equivalent * 4 * 10  # 840ms
    print(f"NeMo equivalent chunk_size: {nemo_equivalent} ({nemo_ms}ms)")
    
    print(f"\n=== Recommendation ===")
    print(f"Change chunk_frames from 160 to {optimal_chunk_frames}")
    print(f"This should give us {target_output_frames} frames after processing")
    print(f"Combined with 70 context frames = 89 total (matching model expectation)")
    
    return optimal_chunk_frames

if __name__ == "__main__":
    optimal_size = calculate_optimal_chunk_size()
    print(f"\nSUMMARY: Use chunk_frames = {optimal_size} instead of 160")