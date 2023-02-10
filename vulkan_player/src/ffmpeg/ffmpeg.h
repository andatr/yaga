#ifndef YAGA_VULKAN_PLAYER_SRC_FFMPEG_FFMPEG
#define YAGA_VULKAN_PLAYER_SRC_FFMPEG_FFMPEG

#include <string>

#include "utility/compiler.h"

DISABLE_WARNINGS

extern "C" {

#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavcodec/avcodec.h>
#include <libavcodec/bsf.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>

} // !extern "C"

ENABLE_WARNINGS

#include "utility/exception.h"

#define FF_GUARD(code, msg, ...)    ffGuard   (code, "FFmpeg: "msg": error %1%", __FILE__, __LINE__, ##__VA_ARGS__);
#define FF_GUARD_PTR(ptr, msg, ...) ffGuardPtr(ptr,  "FFmpeg: "msg,              __FILE__, __LINE__, ##__VA_ARGS__);
#define FF_GUARD_AV(code, msg, ...) ffGuardAV (code, "FFmpeg: "msg": error %1%", __FILE__, __LINE__, ##__VA_ARGS__);

namespace yaga {

class Packet
{
public:
  Packet();
  ~Packet();
  AVPacket* operator*();

private:
  AVPacket* packet_;
};

// -----------------------------------------------------------------------------------------------------------------------------
inline void destroyPacket(AVPacket* packet)
{
  av_packet_free(&packet);
  LOG(trace) << "ffmpeg packet destroyed";
}

// -----------------------------------------------------------------------------------------------------------------------------
inline std::string avErr2Str(int error)
{
  char buffer[AV_ERROR_MAX_STRING_SIZE] {};
  return av_make_error_string(buffer, AV_ERROR_MAX_STRING_SIZE, error);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename... Args>
inline int ffGuard(int code, const char* msg, const char* file, int line, Args... args)
{ 
  if (code < 0) throw Exception(file, line, msg, code, args...);
  return code;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <>
inline int ffGuard(int code, const char* msg, const char* file, int line, AVMediaType type)
{ 
  if (code < 0) throw Exception(file, line, msg, code, av_get_media_type_string(type));
  return code;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, typename... Args>
inline T ffGuardPtr(T ptr, const char* msg, const char* file, int line, Args... args)
{ 
  if (!result) throw Exception(file, line, msg, args...);
  return result;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
inline T ffGuardPtr(T ptr, const char* msg, const char* file, int line, AVMediaType type)
{ 
  if (!ptr) throw Exception(file, line, msg, av_get_media_type_string(type));
  return ptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename... Args>
inline int ffGuardAV(int code, const char* msg, const char* file, int line, Args... args)
{ 
  if (code < 0) throw yaga::Exception(file, line, msg, avErr2Str(code), args...);
  return code;
}

} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_FFMPEG_FFMPEG
