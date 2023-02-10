#ifndef YAGA_VULKAN_PLAYER_SRC_DECODER_FRAME
#define YAGA_VULKAN_PLAYER_SRC_DECODER_FRAME

#include <cstdint>
#include <memory>

namespace yaga {

struct Format
{
  enum class Sampling
  {
    YUV444,
    YUV422,
    YUV420,
    Invalid
  };

  struct Channel
  {
    size_t  width;
    size_t  height;
    size_t  size;
    uint8_t bytes;

    Channel() :
      width(0),
      height(0),
      size(0),
      bytes(0)
    {}
  };

  Channel  luma;
  Channel  chroma;
  size_t   frameSize;
  Sampling sampling;
  float    colors[9];

  Format() :   
    frameSize(0),
    sampling(Sampling::Invalid),
    colors{} 
  {}
};

typedef std::shared_ptr<Format> FormatPtr;

class Frame
{
public:
  virtual ~Frame() {}
  virtual char* data() = 0;
  virtual void timestamp(int64_t value) = 0;
  virtual void format(const FormatPtr& value) = 0;
  virtual const FormatPtr& format() const = 0;
};

} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_FRAME
