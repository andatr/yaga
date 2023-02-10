#ifndef YAGA_PLAYER_SRC_FFMPEG_FFMPEG
#define YAGA_PLAYER_SRC_FFMPEG_FFMPEG

#include "utility/compiler.h"

DISABLE_WARNINGS

extern "C" {

#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavcodec/avcodec.h>

} // !extern "C"

ENABLE_WARNINGS

#include "utility/exception.h"

#define FF_GUARD(code, msg) ffGuard(code, msg": error %1%", __FILE__, __LINE__);

// -----------------------------------------------------------------------------------------------------------------------------
inline int ffGuard(int code, const char* msg, const char* file, int line)
{ 
  if (code < 0) throw yaga::Exception(file, line, msg, code);  
  return code;
}

#endif // !YAGA_PLAYER_SRC_FFMPEG_FFMPEG
