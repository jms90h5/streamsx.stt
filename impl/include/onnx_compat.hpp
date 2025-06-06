#pragma once

// Compatibility wrapper for ONNX Runtime headers
// Force C++17 mode and handle macro conflicts

#ifndef __cplusplus
#error "This header requires C++"
#endif

#if __cplusplus < 201703L
#error "ONNX Runtime requires C++17 or later"
#endif

// Include ONNX Runtime headers with all warnings disabled
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <onnxruntime_cxx_api.h>

#pragma GCC diagnostic pop