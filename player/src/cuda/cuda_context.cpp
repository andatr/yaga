#include "precompiled.h"
#include "cuda_context.h"

namespace yaga {
namespace player {

// -----------------------------------------------------------------------------------------------------------------------------
CudaContext::CudaContext(int deviceIndex) :
  context_{},
  locked_(false)
{
  CUDA_CALL(cuInit(0), "Could not init CUDA");
  int deviceCount = 0;
  CUDA_CALL(cuDeviceGetCount(&deviceCount), "Could not enumerate devices");
  if (deviceIndex < 0 || deviceIndex >= deviceCount) {
    THROW("Could not find CUDA device %1% out of %2%", deviceIndex, deviceCount);
  }
  CUdevice device{};
  CUDA_CALL(cuDeviceGet(&device, deviceIndex), "Could not get CUDA device");
  char deviceName[256];
  CUDA_CALL(cuDeviceGetName(deviceName, sizeof(deviceName), device), "Could not get CUDA device name");
  LOG(info) << "Decoding device: " << deviceName;
  CUDA_CALL(cuCtxCreate(&context_, 0, device), "Could not create CUDA context");
}

// -----------------------------------------------------------------------------------------------------------------------------
CudaContext::~CudaContext()
{
  cuCtxDestroy(context_);
}

// -----------------------------------------------------------------------------------------------------------------------------
ContextLock::ContextLock(CudaContext& cuda) :
  cuda_(&cuda)
{
  if (cuda.locked_ == 0) {
    CUDA_DRIVER_CALL(cuCtxPushCurrent(*cuda));
  }
  ++cuda.locked_;
}

// -----------------------------------------------------------------------------------------------------------------------------
ContextLock::ContextLock(CudaContext* cuda) :
  cuda_(cuda)
{
  if (!cuda->locked_) {
    CUDA_DRIVER_CALL(cuCtxPushCurrent(**cuda));
  }
  ++cuda->locked_;
}

// -----------------------------------------------------------------------------------------------------------------------------
ContextLock::~ContextLock()
{
  --cuda_->locked_;
  if (cuda_->locked_ == 0) {
    CUDA_DRIVER_CALL(cuCtxPopCurrent(NULL));
  }
}

} // !namespace player
} // !namespace yaga
