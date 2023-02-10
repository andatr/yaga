#include "precompiled.h"
#include "host_frame.h"

namespace yaga {
namespace player {

// -----------------------------------------------------------------------------------------------------------------------------
HostFrame::HostFrame(CudaContext* cuda, size_t size) :
  cuda_(cuda),
  size_(size),
  data_{},
  timestamp_(0)
{ 
  if (size > 0) {
    ContextLock lock(cuda_);
    CUDA_DRIVER_CALL(cuMemAlloc(&data_, size));
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
HostFrame::~HostFrame()
{
  if (data_) {
    ContextLock lock(cuda_);
    cuMemFree(data_);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void HostFrame::resize(size_t size)
{
  if (size_ == size) return;
  size_ = size;
  ContextLock cxlock(cuda_);
  if (data_) {
    //char* d = (char*)data_;
    //delete[] d;
    cuMemFree(data_);
  }
  if (size > 0) {
    //data_ = (CUdeviceptr)new char[size];
    CUDA_DRIVER_CALL(cuMemAlloc(&data_, size));
  }
}

} // !namespace player
} // !namespace yaga
