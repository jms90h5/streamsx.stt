#!/usr/bin/env python3
"""
Decode the token IDs from the transcript to actual words using the vocabulary
"""

import re

# Load vocabulary - each line is a token, line number is the token ID
vocab = {}
with open('models/nemo_real/vocab.txt', 'r') as f:
    for idx, line in enumerate(f):
        token = line.strip()
        vocab[idx] = token

print(f"Loaded vocabulary with {len(vocab)} tokens")
print(f"Token 95 = '{vocab.get(95, 'UNKNOWN')}'")
print()

# Parse the transcript file and decode token IDs
with open('ibm_culture_transcript.txt', 'r') as f:
    content = f.read()

# Find all NeMo CTC outputs
pattern = r'\[NeMo CTC: ([\d\s]+)\]'
matches = re.findall(pattern, content)

print(f"Found {len(matches)} transcription outputs")
print()

# Decode the tokens
decoded_words = []
for match in matches:
    token_ids = [int(x) for x in match.strip().split()]
    words = [vocab.get(tid, f'[UNK:{tid}]') for tid in token_ids]
    decoded_words.extend(words)

# Remove duplicates and create final transcript
final_transcript = []
prev_word = None
for word in decoded_words:
    if word != prev_word:
        final_transcript.append(word)
        prev_word = word

print("Decoded transcript:")
print(" ".join(final_transcript))