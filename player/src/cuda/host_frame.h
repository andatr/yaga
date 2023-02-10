#ifndef YAGA_PLAYER_SRC_CUDA_HOST_FRAME
#define YAGA_PLAYER_SRC_CUDA_HOST_FRAME

#include "cuda_context.h"

namespace yaga {
namespace player {

class HostFrame
{
public:
  HostFrame(CudaContext* cuda, size_t size);
  ~HostFrame();
  uint8_t* data()      const { return (uint8_t*)data_; }
  int64_t  timestamp() const { return timestamp_;      }
  size_t   size()      const { return size_;           }
  void timestamp(int64_t value) { timestamp_ = value; }
  void resize(size_t size);

private:
  CudaContext* cuda_;
  size_t       size_;
  CUdeviceptr  data_;
  int64_t timestamp_;
};

} // !namespace player
} // !namespace yaga

#endif // !YAGA_PLAYER_SRC_CUDA_HOST_FRAME
