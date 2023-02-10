#ifndef YAGA_PLAYER_SRC_CUDA_DEVICE_FRAME
#define YAGA_PLAYER_SRC_CUDA_DEVICE_FRAME

#include "cuda_context.h"
#include "engine/video_player.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace player {

class DeviceFrame
{
public:
  DeviceFrame(CudaContext* cuda, uint32_t width, uint32_t height);
  ~DeviceFrame();
  int changeFormat(CUVIDEOFORMAT* format);
  void map(CUVIDPARSERDISPINFO* picture, VideoPlayerFrame* frame);
  int decode(CUVIDPICPARAMS* picture);
  void resize(uint32_t width, uint32_t height);

private:
  int reconfigureDecoder(CUVIDEOFORMAT* format);
  int createDecoder(CUVIDEOFORMAT* format);
  void calcFrameSize();
  void copyLumaData  (CUDA_MEMCPY2D& params, CUstream stream, CUdeviceptr from, char* to);
  void copyChromaData(CUDA_MEMCPY2D& params, CUstream stream, CUdeviceptr from, char* to);

private:
  bool configured_;
  CudaContext*   cuda_;
  CUVIDEOFORMAT  format_;
  AutoDestructor<CUvideoctxlock> contextLock_;
  AutoDestructor<CUvideodecoder> decoder_;
  cudaVideoSurfaceFormat outputFormat_;
  uint32_t dimWidth_;
  uint32_t dimHeight_;
  uint32_t frameWidth_;
  uint32_t lumaHeight_;
  uint32_t chromaHeight_;
  uint32_t maxWidth_;
  uint32_t maxHeight_;
  size_t   frameSize_;
  int      bytesPerPixel_;
  int      chromaPlaneCount_;
};

} // !namespace player
} // !namespace yaga

#endif // !YAGA_PLAYER_SRC_CUDA_DEVICE_FRAME
