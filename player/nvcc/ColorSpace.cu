union RGBA32 {
  uint32_t data;
  struct {
    uint8_t r;
	uint8_t g;
    uint8_t b;
    uint8_t a;
  } color;
};

__constant__ float matrixYuvToRgb[3][3];

// -----------------------------------------------------------------------------------------------------------------------------
template<class T>
__device__ inline T clamp(T x, T lower, T upper)
{
  return x < lower ? lower : (x > upper ? upper : x);
}

// -----------------------------------------------------------------------------------------------------------------------------
template<class Rgb, class YuvUnit>
__device__ inline Rgb yuvToRgbPixel(YuvUnit y, YuvUnit u, YuvUnit v)
{
  const int low = 1 << (sizeof(YuvUnit) * 8 - 4);
  const int mid = 1 << (sizeof(YuvUnit) * 8 - 1);
  float fy = (int)y - low;
  float fu = (int)u - mid;
  float fv = (int)v - mid;
  const float maxf = (1 << sizeof(YuvUnit) * 8) - 1.0f;
  YuvUnit r = (YuvUnit)clamp(matrixYuvToRgb[0][0] * fy + matrixYuvToRgb[0][1] * fu + matrixYuvToRgb[0][2] * fv, 0.0f, maxf);
  YuvUnit g = (YuvUnit)clamp(matrixYuvToRgb[1][0] * fy + matrixYuvToRgb[1][1] * fu + matrixYuvToRgb[1][2] * fv, 0.0f, maxf);
  YuvUnit b = (YuvUnit)clamp(matrixYuvToRgb[2][0] * fy + matrixYuvToRgb[2][1] * fu + matrixYuvToRgb[2][2] * fv, 0.0f, maxf);
  Rgb rgb {};
  const int shift = abs((int)sizeof(YuvUnit) - (int)sizeof(rgb.color.r)) * 8;
  if (sizeof(YuvUnit) >= sizeof(rgb.color.r)) {
    rgb.color.r = r >> shift;
    rgb.color.g = g >> shift;
    rgb.color.b = b >> shift;
   }
  else {
    rgb.color.r = r << shift;
    rgb.color.g = g << shift;
    rgb.color.b = b << shift;
  }
  return rgb;
}

// -----------------------------------------------------------------------------------------------------------------------------
extern "C" __global__ void yuvToRgba32(uint8_t* yuv, int yuvPitch, uint8_t* rgb, int rgbPitch, int width, int height)
{
  int x = (threadIdx.x + blockIdx.x * blockDim.x) * 2;
  int y = (threadIdx.y + blockIdx.y * blockDim.y) * 2;
  if (x + 1 >= width || y + 1 >= height) return;
  uint8_t* src = yuv + x * sizeof(ushort2) / 2 + y * yuvPitch;
  uint8_t* dst = rgb + x * sizeof(RGBA32)      + y * rgbPitch;
  ushort2 l0 = *(ushort2*)src;
  ushort2 l1 = *(ushort2*)(src + yuvPitch);
  ushort2 ch = *(ushort2*)(src + (height - y / 2) * yuvPitch);
  *(uint2*)dst = uint2 {
    yuvToRgbPixel<RGBA32>(l0.x, ch.x, ch.y).data,
    yuvToRgbPixel<RGBA32>(l0.y, ch.x, ch.y).data
  };
  *(uint2*)(dst + rgbPitch) = uint2 {
    yuvToRgbPixel<RGBA32>(l1.x, ch.x, ch.y).data, 
    yuvToRgbPixel<RGBA32>(l1.y, ch.x, ch.y).data
  };
}
