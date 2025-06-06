#!/bin/bash

# Quick fix for tensor dimension issue
# The encoder expects [batch, mel_bins, time] but we're providing [batch, time, mel_bins]

cd /homes/jsharpe/teracloud/com.teracloud.streamsx.stt

echo "Creating fixed version of ProvenNeMoSTT.cpp..."

# Create backup
cp impl/src/ProvenNeMoSTT.cpp impl/src/ProvenNeMoSTT.cpp.backup

# Fix the tensor shape in runEncoder function
sed -i 's/time_steps = features.size() \/ N_MELS/time_steps = features.size() \/ N_MELS/' impl/src/ProvenNeMoSTT.cpp
sed -i 's/static_cast<int64_t>(time_steps), static_cast<int64_t>(mel_bins)/static_cast<int64_t>(mel_bins), static_cast<int64_t>(time_steps)/' impl/src/ProvenNeMoSTT.cpp

echo "Rebuilding with corrected tensor dimensions..."

# Rebuild
./build_proven_debug.sh

echo "✅ Fixed tensor dimensions. Test again with:"
echo "export LD_LIBRARY_PATH=\$PWD/impl/lib:\$PWD/deps/onnxruntime/lib:\$LD_LIBRARY_PATH && ./test_proven_cpp_implementation"