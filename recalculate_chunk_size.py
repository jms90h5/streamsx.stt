#!/usr/bin/env python3
"""Recalculate optimal chunk size based on new evidence"""

def recalculate_optimal_size():
    print("=== Recalculation Based on 29 by 80 Error ===")
    
    # Current: 84 chunk frames → 29 output frames
    # Target: ? chunk frames → 19 output frames
    
    current_chunk = 84
    current_output = 29
    target_output = 19
    
    # Calculate scaling factor
    scale_factor = target_output / current_output
    optimal_chunk = int(current_chunk * scale_factor)
    
    print(f"Current chunk: {current_chunk} frames → {current_output} output frames")
    print(f"Scale factor: {target_output}/{current_output} = {scale_factor:.3f}")
    print(f"Optimal chunk: {optimal_chunk} frames")
    
    # Verify this makes sense
    reduction_ratio = current_output / current_chunk
    predicted_output = optimal_chunk * reduction_ratio
    
    print(f"\nVerification:")
    print(f"Reduction ratio: {reduction_ratio:.3f}")
    print(f"Predicted output: {optimal_chunk} × {reduction_ratio:.3f} = {predicted_output:.1f}")
    
    # Also note the attention dimension changed from 89 to 80
    print(f"\nAttention dimension changed:")
    print(f"Previous: 38 by 89 (context=70)")
    print(f"Current:  29 by 80 (context=?)")
    print(f"The context size might have changed too")
    
    return optimal_chunk

if __name__ == "__main__":
    optimal = recalculate_optimal_size()
    print(f"\nRECOMMENDATION: Try chunk_frames = {optimal}")