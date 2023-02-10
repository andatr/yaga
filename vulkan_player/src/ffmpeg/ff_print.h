#ifndef YAGA_VULKAN_PLAYER_SRC_FFMPEG_PRINT
#define YAGA_VULKAN_PLAYER_SRC_FFMPEG_PRINT

#include <string>

#include "ffmpeg.h"
#include "player.h"

namespace yaga {

Description makeFormatDescription(AVFormatContext* context);
std::string makeStreamName       (AVStream* stream);
Description makeStreamDescription(AVStream* stream);
int64_t     getDuration          (AVFormatContext* context);
std::string getTitle             (AVFormatContext* context);

} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_FFMPEG_PRINT
