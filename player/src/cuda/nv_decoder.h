#ifndef YAGA_PLAYER_SRC_NV_DECODER
#define YAGA_PLAYER_SRC_NV_DECODER

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "decoder.h"
#include "color_space.h"
#include "cuda_context.h"
#include "device_frame.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace player {
namespace nv_decoder_detail {

int CUDAAPI changeFormat     (void* userData, CUVIDEOFORMAT*           format            );
int CUDAAPI decodePicture    (void* userData, CUVIDPICPARAMS*          pictureParams     );
int CUDAAPI displayPicture   (void* userData, CUVIDPARSERDISPINFO*     displayInfo       );
int CUDAAPI getOperatingPoint(void* userData, CUVIDOPERATINGPOINTINFO* operatingPointInfo);

} // !namespace nv_decoder_detail

class NvDecoder : public IDecoder
{
friend int CUDAAPI nv_decoder_detail::changeFormat     (void* userData, CUVIDEOFORMAT*           format            );
friend int CUDAAPI nv_decoder_detail::decodePicture    (void* userData, CUVIDPICPARAMS*          pictureParams     );
friend int CUDAAPI nv_decoder_detail::displayPicture   (void* userData, CUVIDPARSERDISPINFO*     displayInfo       );
friend int CUDAAPI nv_decoder_detail::getOperatingPoint(void* userData, CUVIDOPERATINGPOINTINFO* operatingPointInfo);

public:
  NvDecoder(
    cudaVideoCodec codec, 
    unsigned clockRate, 
    uint32_t width, 
    uint32_t height, 
    VideoPlayerFrame** frames, 
    int frameCount);
  ~NvDecoder();
  void decode(const uint8_t* data, int size, int flags, int64_t timestamp) override;
  void getFrame(VideoPlayerFrameProcessor* proc) override;
  void resize(uint32_t width, uint32_t height) override;
  void stop() override;

private:
  struct Sync
  {
    int count;
    std::mutex mutex;
    std::condition_variable condition;
    Sync() : count(0) {}
  };

private:
  VideoPlayerFrame* popFrameWrite();
  VideoPlayerFrame* popFrameRead();
  void pushFrameWrite();
  void pushFrameRead();
  int changeFormat     (CUVIDEOFORMAT*           format);
  int getOperatingPoint(CUVIDOPERATINGPOINTINFO* opInfo);
  int decodePicture    (CUVIDPICPARAMS*         picture);
  int displayPicture   (CUVIDPARSERDISPINFO*    picture);

private:
  CudaContext cuda_;
  unsigned    operatingPoint_;
  bool        displayAllLayers_;
  DeviceFrame deviceFrame_;
  Sync        readSync_;
  Sync        writeSync_;
  std::vector<VideoPlayerFrame*> frames_;
  ColorSpaceConverter converter_;
  AutoDestructor<CUvideoparser> parser_;
  std::atomic_bool reconfig_;
};

} // !namespace player
} // !namespace yaga

#endif // !YAGA_PLAYER_SRC_NV_DECODER
