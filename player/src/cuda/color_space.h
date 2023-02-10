#ifndef YAGA_PLAYER_SRC_CUDA_COLOR_SPACE
#define YAGA_PLAYER_SRC_CUDA_COLOR_SPACE

#include "cuda_context.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace player {

class ColorSpaceConverter
{
public:
  ColorSpaceConverter(CudaContext* cuda);
  ~ColorSpaceConverter();
  void yuvToRgba32(uint8_t* yuv, int yuvPitch, uint8_t* rgb, int rgbPitch, int width, int height);

private:
  void loadModule();

private:
  CudaContext* cuda_;
  CUmodule     module_;
  CUfunction   yuvToRgba32_;
};

} // !namespace player
} // !namespace yaga

#endif // !YAGA_PLAYER_SRC_CUDA_COLOR_SPACE
