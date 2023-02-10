#ifndef YAGA_VULKAN_PLAYER_SRC_CUDA_CUDA_CONTEXT
#define YAGA_VULKAN_PLAYER_SRC_CUDA_CUDA_CONTEXT

#include <thread>

#include "cuda.h"

namespace yaga {

class CudaContext
{
friend class ContextLock;

public:
  explicit CudaContext(int deviceIndex = 0);
  ~CudaContext();
  CUcontext operator*() { return context_; }

private:
  CUcontext context_;
  int locked_;
};

class ContextLock
{
public:
  explicit ContextLock(CudaContext& cuda);
  explicit ContextLock(CudaContext* cuda);
  ~ContextLock();

private:
  CudaContext* cuda_;
};

} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_CUDA_CUDA_CONTEXT
