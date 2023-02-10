#ifndef YAGA_PLAYER_SRC_CUDA_CUDA
#define YAGA_PLAYER_SRC_CUDA_CUDA

#include "utility/compiler.h"

DISABLE_WARNINGS

#include <cuda.h>
#include "nvcuvid.h"
#include "cuviddec.h"

ENABLE_WARNINGS

#include "utility/exception.h"

#define CUDA_CALL(code, msg)   cudaCall      (code, msg": error %1%", __FILE__, __LINE__);
#define CUDA_DRIVER_CALL(code) cudaDriverCall(code, #code,            __FILE__, __LINE__);
#define NV_DECODER_CALL(code)  nvDecoderCall (code, #code,            __FILE__, __LINE__);

// -----------------------------------------------------------------------------------------------------------------------------
inline int cudaCall(int code, const char* msg, const char* file, int line)
{ 
  if (code) throw yaga::Exception(file, line, msg, code);
  return code;
}

// -----------------------------------------------------------------------------------------------------------------------------
inline void cudaDriverCall(CUresult code, const char* funcName, const char* file, int line)
{ 
  if (code == CUDA_SUCCESS) return;
  const char* message = nullptr;
  cuGetErrorName(code, &message);
  throw yaga::Exception(file, line, "CUDA error in %1%: %2%", funcName, message);         
}

// -----------------------------------------------------------------------------------------------------------------------------
inline void nvDecoderCall(CUresult code, const char* funcName, const char* file, int line)
{ 
  if (code == CUDA_SUCCESS) return;
  throw yaga::Exception(file, line, "CUDA error in %1%: %2%", funcName, code);         
}

#endif // !YAGA_PLAYER_SRC_CUDA_CUDA
