#ifndef YAGA_VULKAN_PLAYER_SRC_NV_DECODER
#define YAGA_VULKAN_PLAYER_SRC_NV_DECODER

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "decoder/decoder_backend.h"
#include "cuda_context.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace nv_decoder_detail {

int CUDAAPI changeFormat     (void* userData, CUVIDEOFORMAT*           format            );
int CUDAAPI decodePicture    (void* userData, CUVIDPICPARAMS*          pictureParams     );
int CUDAAPI displayPicture   (void* userData, CUVIDPARSERDISPINFO*     displayInfo       );
int CUDAAPI getOperatingPoint(void* userData, CUVIDOPERATINGPOINTINFO* operatingPointInfo);

} // !namespace nv_decoder_detail

class NvDecoder : public DecoderBackend
{
friend int CUDAAPI nv_decoder_detail::changeFormat     (void* userData, CUVIDEOFORMAT*           format            );
friend int CUDAAPI nv_decoder_detail::decodePicture    (void* userData, CUVIDPICPARAMS*          pictureParams     );
friend int CUDAAPI nv_decoder_detail::displayPicture   (void* userData, CUVIDPARSERDISPINFO*     displayInfo       );
friend int CUDAAPI nv_decoder_detail::getOperatingPoint(void* userData, CUVIDOPERATINGPOINTINFO* operatingPointInfo);

public:
  NvDecoder(
    cudaVideoCodec codec, 
    unsigned       clockRate, 
    uint32_t       width, 
    uint32_t       height, 
    DecoderBackend::Handler handler);
  ~NvDecoder();
  void decode(const uint8_t* data, uint64_t size, int flags, int64_t timestamp) override;
  void resize(uint32_t width, uint32_t height) override;

private:
  typedef AutoDestructor<CUvideoparser>  Parser;
  typedef AutoDestructor<CUvideoctxlock> CuContextLock;
  typedef AutoDestructor<CUvideodecoder> CuDecoder;
  typedef cudaVideoSurfaceFormat         SurfaceFormat;
  typedef DecoderBackend::Handler        FrameHandler;

private:
  void createParser(cudaVideoCodec codec, unsigned clockRate);
  int  changeFormat      (CUVIDEOFORMAT*           format);
  int  getOperatingPoint (CUVIDOPERATINGPOINTINFO* opInfo);
  int  decodePicture     (CUVIDPICPARAMS*         picture);
  int  displayPicture    (CUVIDPARSERDISPINFO*    picture);
  int  reconfigureDecoder(CUVIDEOFORMAT*           format);
  int  createDecoder     (CUVIDEOFORMAT*           format);
  void calcFrameSize     (CUVIDEOFORMAT*           format);
  void copyLumaData  (CUstream stream, size_t pitch, CUdeviceptr from, char* to);
  void copyChromaData(CUstream stream, size_t pitch, CUdeviceptr from, char* to);

private:
  CudaContext    cuda_;
  unsigned       operatingPoint_;
  bool           displayAllLayers_;
  FrameHandler   handler_;
  Parser         parser_;
  bool           configured_;
  CUVIDEOFORMAT  format_;
  CuContextLock  contextLock_;
  CuDecoder      decoder_;
  SurfaceFormat  outputFormat_;
  uint32_t       dimWidth_;
  uint32_t       dimHeight_;
  uint32_t       maxWidth_;
  uint32_t       maxHeight_;
  FormatPtr      frameFormat_;
};

} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_NV_DECODER
