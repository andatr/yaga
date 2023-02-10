#include "precompiled.h"
#include "color_space.h"

namespace yaga {
namespace player {
namespace {

enum class ColorSpace {
  BT709       = 1,
  Unspecified = 2,
  Reserved    = 3,
  FCC         = 4,
  BT470       = 5,
  BT601       = 6,
  SMPTE240M   = 7,
  YCgCo       = 8,
  BT2020      = 9,
  BT2020C     = 10
};

// -----------------------------------------------------------------------------------------------------------------------------
void inline getConstants(ColorSpace colorSpace, float& wr, float& wb, int& black, int& white, int& max)
{
  black = 16;
  white = 235;
  max = 255;
  switch (colorSpace) {
    case ColorSpace::FCC:
      wr = 0.30f;
      wb = 0.11f;
      break;
    case ColorSpace::BT470:
    case ColorSpace::BT601:
      wr = 0.2990f;
      wb = 0.1140f;
      break;
    case ColorSpace::SMPTE240M:
      wr = 0.212f;
      wb = 0.087f;
      break;
    case ColorSpace::BT2020:
    case ColorSpace::BT2020C:
      wr = 0.2627f;
      wb = 0.0593f;
      black = 64  << 6;
      white = 940 << 6;
      max = (1 << 16) - 1;
      break;
    case ColorSpace::BT709:
    default:
      wr = 0.2126f;
      wb = 0.0722f;
      break;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void setMatrixYuvToRgb(ColorSpace colorSpace)
{
  float wr  = 0.0f;
  float wb  = 0.0f;
  int black = 0;
  int white = 0;
  int max   = 0;
  getConstants(colorSpace, wr, wb, black, white, max);
  float matrix[3][3] = {
    1.0f, 0.0f, (1.0f - wr) / 0.5f,
    1.0f, -wb * (1.0f - wb) / 0.5f / (1 - wb - wr), -wr * (1 - wr) / 0.5f / (1 - wb - wr),
    1.0f, (1.0f - wb) / 0.5f, 0.0f
  };
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      matrix[i][j] = (float)(1.0 * max / (white - black) * matrix[i][j]);
    }
  }
  //cudaMemcpyToSymbol(matYuv2Rgb, mat, sizeof(mat));
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
ColorSpaceConverter::ColorSpaceConverter(CudaContext* cuda) :
  cuda_(cuda),
  module_{},
  yuvToRgba32_{}
{
  loadModule();
}

// -----------------------------------------------------------------------------------------------------------------------------
ColorSpaceConverter::~ColorSpaceConverter()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void ColorSpaceConverter::yuvToRgba32(uint8_t* yuv, int yuvPitch, uint8_t* rgb, int rgbPitch, int width, int height)
{
  CUdeviceptr rgbDev;
  CUDA_DRIVER_CALL(cuMemAlloc(&rgbDev, rgbPitch));
  void* args[] = { &yuv, &yuvPitch, &rgbDev, &rgbPitch, &width, &height };
  CUDA_DRIVER_CALL(cuLaunchKernel(yuvToRgba32_,
    (width  + 63) / 32 / 2,
    (height + 3)  / 2  / 2,
    1,
    32,
    2,
    1,
    0,
    nullptr,
    args,
    nullptr
  ));
}

// -----------------------------------------------------------------------------------------------------------------------------
void ColorSpaceConverter::loadModule()
{
  const char* fname = R"(C:\Projects\cpp\yaga\src\player\nvcc\ColorSpace2.ptx)";

  std::ifstream file(fname, std::ios::in | std::ios::binary);
  file.seekg(0, std::ios::end);
  size_t size = file.tellg();
  file.seekg(0);
  std::vector<char> data(size + 1);
  file.read(data.data(), size);

  CUDA_DRIVER_CALL(cuModuleLoadData(&module_, data.data()));
  CUDA_DRIVER_CALL(cuModuleGetFunction(&yuvToRgba32_, module_, "yuvToRgba32"));
}

} // !namespace player
} // !namespace yaga
