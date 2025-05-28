#!/usr/bin/env python3
"""Convert WAV files to raw PCM by skipping the header"""

import sys
import os

def wav_to_raw(wav_file, raw_file):
    """Extract raw PCM data from WAV file (skip 44-byte header)"""
    with open(wav_file, 'rb') as f:
        # Skip WAV header (typically 44 bytes)
        f.seek(44)
        # Read the rest as raw PCM
        pcm_data = f.read()
    
    with open(raw_file, 'wb') as f:
        f.write(pcm_data)
    
    print(f"Converted {wav_file} to {raw_file}")
    print(f"Raw PCM size: {len(pcm_data)} bytes")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python wav_to_raw.py input.wav output.raw")
        sys.exit(1)
    
    wav_to_raw(sys.argv[1], sys.argv[2])