#!/bin/bash

# Debug script to build the proven implementation

cd /homes/jsharpe/teracloud/com.teracloud.streamsx.stt

echo "Current directory: $(pwd)"
echo "Checking ONNX Runtime paths..."

ONNX_LOCAL="deps/onnxruntime"
echo "ONNX_LOCAL path: $ONNX_LOCAL"
echo "Full lib path: $ONNX_LOCAL/lib/libonnxruntime.so"

if [ -f "$ONNX_LOCAL/lib/libonnxruntime.so" ]; then
    echo "✅ ONNX Runtime found at: $ONNX_LOCAL"
    ONNXRUNTIME_ROOT="$ONNX_LOCAL"
elif [ -f "/usr/local/lib/libonnxruntime.so" ]; then
    echo "✅ ONNX Runtime found at: /usr/local"
    ONNXRUNTIME_ROOT="/usr/local"
elif [ -f "/usr/lib/libonnxruntime.so" ]; then
    echo "✅ ONNX Runtime found at: /usr"
    ONNXRUNTIME_ROOT="/usr"
else
    echo "❌ ONNX Runtime not found"
    exit 1
fi

echo "Using ONNXRUNTIME_ROOT: $ONNXRUNTIME_ROOT"

# Build manually with correct paths
CXX=g++
CXXFLAGS="-O3 -std=c++14 -fPIC -Wall -Wextra"
CXXFLAGS="$CXXFLAGS -Iimpl/include"
CXXFLAGS="$CXXFLAGS -I$ONNXRUNTIME_ROOT/include"

LDFLAGS="-L$ONNXRUNTIME_ROOT/lib"
LDFLAGS="$LDFLAGS -lonnxruntime"
LDFLAGS="$LDFLAGS -lsndfile"
LDFLAGS="$LDFLAGS -ldl"

echo "Creating build directory..."
mkdir -p impl/build impl/lib

echo "Compiling ProvenFeatureExtractor.cpp..."
$CXX $CXXFLAGS -c -o impl/build/ProvenFeatureExtractor.o impl/src/ProvenFeatureExtractor.cpp

echo "Compiling ProvenNeMoSTT.cpp..."
$CXX $CXXFLAGS -c -o impl/build/ProvenNeMoSTT.o impl/src/ProvenNeMoSTT.cpp

echo "Creating shared library..."
$CXX -shared $LDFLAGS -o impl/lib/libproven_nemo_stt.so impl/build/ProvenFeatureExtractor.o impl/build/ProvenNeMoSTT.o

echo "Compiling test executable..."
$CXX $CXXFLAGS -o test_proven_cpp_implementation test_proven_cpp_implementation.cpp -L./impl/lib -lproven_nemo_stt $LDFLAGS

echo "✅ Build complete!"
echo "Run with: ./test_proven_cpp_implementation"