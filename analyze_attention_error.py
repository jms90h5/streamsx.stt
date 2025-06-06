#!/usr/bin/env python3
"""Analyze the actual attention mechanism error instead of guessing"""

def analyze_where_node_error():
    print("=== Understanding the 'Where' Node Error ===")
    print()
    
    print("ERROR: 'Attempting to broadcast an axis by a dimension other than 1. 20 by 70'")
    print()
    
    print("What this means:")
    print("1. The 'Where' node is trying to broadcast two tensors")
    print("2. One tensor has dimension 20, another has dimension 70")
    print("3. Broadcasting fails because neither is size 1")
    print()
    
    print("The Where node in attention mechanisms typically does:")
    print("- Masking: Where(condition, value_if_true, value_if_false)")
    print("- Common use: attention masking to prevent looking at future tokens")
    print()
    
    print("In cache-aware streaming, this is likely:")
    print("- Attention mask: [current_frames + cache_frames] = total attention window")
    print("- The mask defines which positions can attend to which")
    print()
    
    print("Key insight:")
    print("The issue is NOT chunk size - it's that our tensors have mismatched shapes")
    print("going INTO the attention mechanism, not the chunk processing.")
    print()
    
    print("Root cause analysis:")
    print("1. We have 20 frames from current chunk")
    print("2. We have some cache that should make total = 70")
    print("3. But the cache and current chunk shapes don't align properly")
    print("4. This suggests the cache concatenation/preparation is wrong")
    print()
    
    print("What to investigate:")
    print("1. How are we concatenating cache + current chunk?")
    print("2. Are the cache tensors the right shape?")
    print("3. Are we preparing the attention inputs correctly?")
    print("4. Look at the prepareEncoderInputs() function specifically")

def suggest_debugging_approach():
    print("\n=== Proper Debugging Approach ===")
    print()
    
    print("Instead of guessing parameters, we should:")
    print()
    
    print("1. Add debug prints to see ACTUAL tensor shapes going into encoder")
    print("2. Check what the cache tensors look like before/after concatenation") 
    print("3. Compare with working NeMo examples to see expected tensor shapes")
    print("4. Look at the ONNX model graph to understand what the Where node expects")
    print()
    
    print("The error happens in '/layers.0/self_attn/Where' which means:")
    print("- It's in the first transformer layer")
    print("- It's in the self-attention mechanism")
    print("- It's specifically in a Where operation (masking)")
    print()
    
    print("This suggests the issue is in how we prepare the input tensors")
    print("for the encoder, not in chunk sizes or cache sizes.")

if __name__ == "__main__":
    analyze_where_node_error()
    suggest_debugging_approach()