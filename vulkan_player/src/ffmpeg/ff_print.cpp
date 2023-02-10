#include "precompiled.h"
#include "ff_demuxer.h"

namespace yaga {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
void clear(std::stringstream& buffer)
{
  buffer.str(std::string());
  buffer.clear();
}

// -----------------------------------------------------------------------------------------------------------------------------
std::string toUpper(const char* ptr, std::stringstream& buffer)
{
  clear(buffer);
  while (*ptr != 0) {
    buffer << static_cast<char>(std::toupper(*ptr));
    ++ptr;
  }
  return buffer.str();
}

// -----------------------------------------------------------------------------------------------------------------------------
std::string toUpper(const char* str)
{
  std::stringstream buffer;
  return toUpper(str, buffer);
}

// -----------------------------------------------------------------------------------------------------------------------------
void printTimePart(int64_t part, std::stringstream& buffer)
{
  if (part < 10) buffer << "0";
  buffer << part;
}

// -----------------------------------------------------------------------------------------------------------------------------
std::string printDuration(int64_t duration, std::stringstream& buffer)
{
  clear(buffer);
  duration /= AV_TIME_BASE;
  int64_t minutes = duration / 60;
  int64_t seconds = duration - minutes * 60;
  int64_t hours = minutes / 60;
  minutes -= hours * 60;
  printTimePart(hours,   buffer);
  buffer << ":";
  printTimePart(minutes, buffer);
  buffer << ":";
  printTimePart(seconds, buffer);
  return buffer.str();
}

// -----------------------------------------------------------------------------------------------------------------------------
std::string printCodecTag(uint32_t value, std::stringstream& buffer)
{
  clear(buffer);
  while (value > 0) {
    const char ch = value & 0xFF;
    buffer << ch;
    value = value >> 8;
  }
  return buffer.str();
}

// -----------------------------------------------------------------------------------------------------------------------------
std::string printBitRate(int64_t bitRate, std::stringstream& buffer, bool clr = true)
{
  int mult = 0;
  double value = static_cast<double>(bitRate);
  while (value > 1000.0) {
    value /= 1000.0;
    ++mult;
  }
  if (clr) clear(buffer);
  buffer << std::fixed << std::setprecision(1) << value;
  switch (mult) {
  case 0:
    buffer << " bit/s";
    break;
  case 1:
    buffer << " kbit/s";
    break;
  case 2:
    buffer << " Mbit/s";
    break;
  case 3:
    buffer << " Gbit/s";
    break;
  case 4:
    buffer << " Tbit/s";
    break;
  case 5:
    buffer << " Pbit/s";
    break;
  case 6:
    buffer << " Ebit/s";
    break;
  case 7:
    buffer << " Zbit/s";
    break;
  case 8:
    buffer << " Ybit/s";
    break;
  default:
    buffer << " ?/s";
    break;
  }
  return clr ? buffer.str() : std::string();
}

// -----------------------------------------------------------------------------------------------------------------------------
std::string printSize(int64_t size, std::stringstream& buffer)
{
  int mult = 0;
  double value = static_cast<double>(size);
  while (value > 1024.0) {
    value /= 1024.0;
    ++mult;
  }
  clear(buffer);
  buffer << std::fixed << std::setprecision(1) << value;
  switch (mult) {
  case 0:
    buffer << " bytes";
    break;
  case 1:
    buffer << " KiB";
    break;
  case 2:
    buffer << " MiB";
    break;
  case 3:
    buffer << " GiB";
    break;
  case 4:
    buffer << " TiB";
    break;
  case 5:
    buffer << " PiB";
    break;
  case 6:
    buffer << " EiB";
    break;
  case 7:
    buffer << " ZiB";
    break;
  case 8:
    buffer << " YiB";
    break;
  default:
    buffer << " ?";
    break;
  }
  return buffer.str();
}

// -----------------------------------------------------------------------------------------------------------------------------
std::string printFrameRate(const AVRational& value, std::stringstream& buffer)
{
  clear(buffer);
  const double val = static_cast<double>(value.num) / value.den;
  buffer << std::fixed << std::setprecision(3) << val << " (" << value.num << "/" << value.den << ")";
  return buffer.str();
}

// -----------------------------------------------------------------------------------------------------------------------------
std::string printFrameRate(int64_t frames, int64_t duration, std::stringstream& buffer)
{
  clear(buffer);
  const auto fr = static_cast<double>(frames) * AV_TIME_BASE / duration;
  buffer << std::fixed << std::setprecision(3) << fr;
  return buffer.str();
}

// -----------------------------------------------------------------------------------------------------------------------------
std::string printFieldOrder(AVFieldOrder value)
{
  switch (value) {
  case AV_FIELD_PROGRESSIVE:
    return "Progressive";
  case AV_FIELD_TT:
    return "TT";
  case AV_FIELD_BB:
    return "BB";
  case AV_FIELD_TB:
    return "TB";
  case AV_FIELD_BT:
    return "BT";
  default:
    return "Unknown";
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
const char* printPixelFormatName(AVPixelFormat value)
{
  switch (value) {
  case AV_PIX_FMT_YUV420P:
    return "YUV420P";
  case AV_PIX_FMT_YUYV422:
    return "YUYV422";
  case AV_PIX_FMT_YVYU422:
    return "YVYU422";
  case AV_PIX_FMT_UYVY422:
    return "UYVY422";
  case AV_PIX_FMT_RGB24:
    return "RGB24";
  case AV_PIX_FMT_BGR24:
    return "BGR24";
  case AV_PIX_FMT_YUV422P:
    return "YUV422P";
  case AV_PIX_FMT_YUVJ422P:
    return "YUVJ422P";
  case AV_PIX_FMT_YUV410P:
    return "YUV410P";
  case AV_PIX_FMT_YUV411P:
    return "YUV411P";
  case AV_PIX_FMT_YUVJ411P:
    return "YUVJ411P";
  case AV_PIX_FMT_GRAY8:
    return "GRAY8";
  case AV_PIX_FMT_MONOWHITE:
    return "MONOWHITE";
    break;  
  case AV_PIX_FMT_MONOBLACK:
    return "MONOBLACK";
  case AV_PIX_FMT_PAL8:
    return "PAL8";
  case AV_PIX_FMT_YUVJ420P:
    return "YUVJ420P";
  case AV_PIX_FMT_NV12:
    return "NV12";
  case AV_PIX_FMT_UYYVYY411:
    return "UYYVYY411";
  case AV_PIX_FMT_BGR8:
    return "BGR8";
  case AV_PIX_FMT_RGB8:
    return "RGB8";
  case AV_PIX_FMT_BGR4:
    return "BGR4";
  case AV_PIX_FMT_RGB4:
    return "RGB4";
  case AV_PIX_FMT_BGR4_BYTE:
    return "BGR4 BYTE";
  case AV_PIX_FMT_RGB4_BYTE:
    return "RGB4 BYTE";
  case AV_PIX_FMT_NV21:
    return "NV21";
  case AV_PIX_FMT_ARGB:
    return "ARGB";
  case AV_PIX_FMT_RGBA:
    return "RGBA";
  case AV_PIX_FMT_ABGR:
    return "ABGR";
  case AV_PIX_FMT_BGRA:
    return "BGRA";
  case AV_PIX_FMT_GRAY16BE:
    return "GRAY16BE";
  case AV_PIX_FMT_GRAY16LE:
    return "GRAY16LE";
  case AV_PIX_FMT_YUV440P:
    return "YUV440P";
  case AV_PIX_FMT_YUVJ440P:
    return "YUVJ440P";
  case AV_PIX_FMT_YUVA420P:
    return "YUVA420P";
  case AV_PIX_FMT_RGB48BE:
    return "RGB48BE";
  case AV_PIX_FMT_RGB48LE:
    return "RGB48LE";
  case AV_PIX_FMT_BGR48BE:
    return "BGR48BE";
  case AV_PIX_FMT_BGR48LE:
    return "BGR48LE";
  case AV_PIX_FMT_RGB565BE:
    return "RGB565BE";
  case AV_PIX_FMT_RGB565LE:
    return "RGB565LE";
  case AV_PIX_FMT_RGB555BE:
    return "RGB555BE";
  case AV_PIX_FMT_RGB555LE:
    return "RGB555LE";
  case AV_PIX_FMT_BGR565BE:
    return "BGR565BE";
  case AV_PIX_FMT_BGR565LE:
    return "BGR565LE";
  case AV_PIX_FMT_BGR555BE:
    return "BGR555BE";
  case AV_PIX_FMT_BGR555LE:
    return "BGR555LE";
  case AV_PIX_FMT_YUV420P16LE:
    return "YUV420P16LE";
  case AV_PIX_FMT_YUV420P16BE:
    return "YUV420P16BE";
  case AV_PIX_FMT_YUV422P16LE:
    return "YUV422P16LE";
  case AV_PIX_FMT_YUV422P16BE:
    return "YUV422P16BE";
  case AV_PIX_FMT_YUV444P16LE:
    return "YUV444P16LE";
  case AV_PIX_FMT_YUV444P16BE:
    return "YUV444P16BE";
  case AV_PIX_FMT_RGB444LE:
    return "RGB444LE";
  case AV_PIX_FMT_RGB444BE:
    return "RGB444BE";
  case AV_PIX_FMT_BGR444LE:
    return "BGR444LE";
  case AV_PIX_FMT_BGR444BE:
    return "BGR444BE";
  case AV_PIX_FMT_YA8:
    return "YA8";
  case AV_PIX_FMT_YUV420P9BE:
    return "YUV420P9BE";
  case AV_PIX_FMT_YUV420P9LE:
    return "YUV420P9LE";
  case AV_PIX_FMT_YUV420P10BE:
    return "YUV420P10BE";
  case AV_PIX_FMT_YUV420P10LE:
    return "YUV420P10LE";
  case AV_PIX_FMT_YUV422P10BE:
    return "YUV422P10BE";
  case AV_PIX_FMT_YUV422P10LE:
    return "YUV422P10LE";
  case AV_PIX_FMT_YUV444P9BE:
    return "YUV444P9BE";
  case AV_PIX_FMT_YUV444P9LE:
    return "YUV444P9LE";
  case AV_PIX_FMT_YUV444P10BE:
    return "YUV444P10BE";
  case AV_PIX_FMT_YUV444P10LE:
    return "YUV444P10LE";
  case AV_PIX_FMT_YUV422P9BE:
    return "YUV422P9BE";
  case AV_PIX_FMT_YUV422P9LE:
    return "YUV422P9LE";
  case AV_PIX_FMT_GBRP:
    return "GBRP";
  case AV_PIX_FMT_GBRP9BE:
    return "GBRP9BE";
  case AV_PIX_FMT_GBRP9LE:
    return "GBRP9LE";
  case AV_PIX_FMT_GBRP10BE:
    return "GBRP10BE";
  case AV_PIX_FMT_GBRP10LE:
    return "GBRP10LE";
  case AV_PIX_FMT_GBRP16BE:
    return "GBRP16BE";
  case AV_PIX_FMT_GBRP16LE:
    return "GBRP16LE";
  case AV_PIX_FMT_YUVA444P:
    return "YUVA444P";
  case AV_PIX_FMT_YUVA420P9BE:
    return "YUVA420P9BE";
  case AV_PIX_FMT_YUVA420P9LE:
    return "YUVA420P9LE";
  case AV_PIX_FMT_YUVA422P9BE:
    return "YUVA422P9BE";
  case AV_PIX_FMT_YUVA422P9LE:
    return "YUVA422P9LE";
  case AV_PIX_FMT_YUVA420P10BE:
    return "YUVA420P10BE";
  case AV_PIX_FMT_YUVA420P10LE:
    return "YUVA420P10LE";
  case AV_PIX_FMT_YUVA422P10BE:
    return "YUVA422P10BE";
  case AV_PIX_FMT_YUVA422P10LE:
    return "YUVA422P10LE";
  case AV_PIX_FMT_YUVA444P10BE:
    return "YUVA444P10BE";
  case AV_PIX_FMT_YUVA444P10LE:
    return "YUVA444P10LE";
  case AV_PIX_FMT_YUVA420P16BE:
    return "YUVA420P16BE";
  case AV_PIX_FMT_YUVA420P16LE:
    return "YUVA420P16LE";
  case AV_PIX_FMT_YUVA422P16BE:
    return "YUVA422P16BE";
  case AV_PIX_FMT_YUVA422P16LE:
    return "YUVA422P16LE";
  case AV_PIX_FMT_YUVA444P16BE:
    return "YUVA444P16BE";
  case AV_PIX_FMT_YUVA444P16LE:
    return "YUVA444P16LE";
  case AV_PIX_FMT_XYZ12LE:
    return "XYZ12LE";
  case AV_PIX_FMT_XYZ12BE:
    return "XYZ12BE";
  case AV_PIX_FMT_NV16:
    return "NV16";
  case AV_PIX_FMT_RGBA64BE:
    return "RGBA64BE";
  case AV_PIX_FMT_RGBA64LE:
    return "RGBA64LE";
  case AV_PIX_FMT_BGRA64BE:
    return "BGRA64BE";
  case AV_PIX_FMT_BGRA64LE:
    return "BGRA64LE";
  case AV_PIX_FMT_YA16BE:
    return "YA16BE";
  case AV_PIX_FMT_YA16LE:
    return "YA16LE";
  case AV_PIX_FMT_GBRAP:
    return "GBRAP";
  case AV_PIX_FMT_GBRAP16BE:
    return "GBRAP16BE";
  case AV_PIX_FMT_GBRAP16LE:
    return "GBRAP16LE";
  case AV_PIX_FMT_0RGB:
    return "0RGB";
  case AV_PIX_FMT_RGB0:
    return "RGB0";
  case AV_PIX_FMT_0BGR:
    return "0BGR";
  case AV_PIX_FMT_BGR0:
    return "BGR0";
  case AV_PIX_FMT_YUV420P12BE:
    return "YUV420P12BE";
  case AV_PIX_FMT_YUV420P12LE:
    return "YUV420P12LE";
  case AV_PIX_FMT_YUV420P14BE:
    return "YUV420P14BE";
  case AV_PIX_FMT_YUV420P14LE:
    return "YUV420P14LE";
  case AV_PIX_FMT_YUV422P12BE:
    return "YUV422P12BE";
  case AV_PIX_FMT_YUV422P12LE:
    return "YUV422P12LE";
  case AV_PIX_FMT_YUVA422P:
    return "YUVA422P";
  case AV_PIX_FMT_YUVA422P12BE:
    return "YUVA422P12BE";
  case AV_PIX_FMT_YUVA422P12LE:
    return "YUVA422P12LE";
  case AV_PIX_FMT_YUV422P14BE:
    return "YUV422P14BE";
  case AV_PIX_FMT_YUV422P14LE:
    return "YUV422P14LE";
  case AV_PIX_FMT_YUV444P12BE:
    return "YUV444P12BE";
  case AV_PIX_FMT_YUV444P12LE:
    return "YUV444P12LE";
  case AV_PIX_FMT_YUVA444P9BE:
    return "YUVA444P9BE";
  case AV_PIX_FMT_YUVA444P12BE:
    return "YUVA444P12BE";
  case AV_PIX_FMT_YUVA444P12LE:
    return "YUVA444P12LE";
  case AV_PIX_FMT_YUVA444P9LE:
    return "YUVA444P9LE";
  case AV_PIX_FMT_YUV444P14BE:
    return "YUV444P14BE";
  case AV_PIX_FMT_YUV444P14LE:
    return "YUV444P14LE";
  case AV_PIX_FMT_GBRP12BE:
    return "GBRP12BE";
  case AV_PIX_FMT_GBRP12LE:
    return "GBRP12LE";
  case AV_PIX_FMT_GBRP14BE:
    return "GBRP14BE";
  case AV_PIX_FMT_GBRP14LE:
    return "GBRP14LE";
  case AV_PIX_FMT_YUV440P10LE:
    return "YUV440P10LE";
  case AV_PIX_FMT_YUV440P10BE:
    return "YUV440P10BE";
  case AV_PIX_FMT_YUV440P12LE:
    return "YUV440P12LE";
  case AV_PIX_FMT_YUV440P12BE:
    return "YUV440P12BE";
  case AV_PIX_FMT_AYUV64LE:
    return "AYUV64LE";
  case AV_PIX_FMT_AYUV64BE:
    return "AYUV64BE";
  case AV_PIX_FMT_P010LE:
    return "P010LE";
  case AV_PIX_FMT_P010BE:
    return "P010BE";
  case AV_PIX_FMT_GBRAP12BE:
    return "GBRAP12BE";
  case AV_PIX_FMT_GBRAP12LE:
    return "GBRAP12LE";
  case AV_PIX_FMT_GBRAP10BE:
    return "GBRAP10BE";
  case AV_PIX_FMT_GBRAP10LE:
    return "GBRAP10LE";
  case AV_PIX_FMT_GRAY12BE:
    return "GRAY12BE";
  case AV_PIX_FMT_GRAY12LE:
    return "GRAY12LE";
  case AV_PIX_FMT_GRAY10BE:
    return "GRAY10BE";
  case AV_PIX_FMT_GRAY10LE:
    return "GRAY10LE";
  case AV_PIX_FMT_P016LE:
    return "P016LE";
  case AV_PIX_FMT_P016BE:
    return "P016BE";
  case AV_PIX_FMT_GRAY9BE:
    return "GRAY9BE";
  case AV_PIX_FMT_GRAY9LE:
    return "GRAY9LE";
  case AV_PIX_FMT_GBRPF32BE:
    return "GBRPF32BE";
  case AV_PIX_FMT_GBRPF32LE:
    return "GBRPF32LE";
  case AV_PIX_FMT_GBRAPF32BE:
    return "GBRAPF32BE";
  case AV_PIX_FMT_GBRAPF32LE:
    return "GBRAPF32LE";
  case AV_PIX_FMT_GRAY14BE:
    return "GRAY14BE";
  case AV_PIX_FMT_GRAY14LE:
    return "GRAY14LE";
  case AV_PIX_FMT_GRAYF32BE:
    return "GRAYF32BE";
  case AV_PIX_FMT_GRAYF32LE:
    return "GRAYF32LE";
  case AV_PIX_FMT_NV24:
    return "NV24";
  case AV_PIX_FMT_NV42:
    return "NV42";
  case AV_PIX_FMT_YUV444P:
    return "YUV444P";
  case AV_PIX_FMT_YUVJ444P:
    return "YUVJ444P";
  case AV_PIX_FMT_Y210BE:
    return "Y210BE";
  case AV_PIX_FMT_Y210LE:
    return "Y210LE";
  case AV_PIX_FMT_X2RGB10LE:
    return "X2RGB10LE";
  case AV_PIX_FMT_X2RGB10BE:
    return "X2RGB10BE";
  case AV_PIX_FMT_X2BGR10LE:
    return "X2BGR10LE";
  case AV_PIX_FMT_X2BGR10BE:
    return "X2BGR10BE";
  case AV_PIX_FMT_NV20BE:
    return "NV20BE";
  case AV_PIX_FMT_NV20LE:
    return "NV20LE";
  case AV_PIX_FMT_P210BE:
    return "P210BE";
  case AV_PIX_FMT_P210LE:
    return "P210LE";
  case AV_PIX_FMT_P410BE:
    return "P410BE";
  case AV_PIX_FMT_P410LE:
    return "P410LE";
  case AV_PIX_FMT_P216BE:
    return "P216BE";
  case AV_PIX_FMT_P216LE:
    return "P216LE";
  case AV_PIX_FMT_P416BE:
    return "P416BE";
  case AV_PIX_FMT_P416LE:
    return "P416LE";
  default:
    break;
  }
  return "";
}

// -----------------------------------------------------------------------------------------------------------------------------
void printPixelFormat(AVPixelFormat value, Description& desc)
{
  char const* color    = nullptr;
  char const* sampling = nullptr;
  char const* depth    = nullptr;
  switch (value) {
  case AV_PIX_FMT_YUV420P:
    color = "planar YUV";
    sampling = "4:2:0";
    depth = "12";
    break;
  case AV_PIX_FMT_YUYV422:
  case AV_PIX_FMT_YVYU422:
  case AV_PIX_FMT_UYVY422:
    color = "packed YUV";
    sampling = "4:2:2";
    depth = "16";
    break;
  case AV_PIX_FMT_RGB24:
    color = "packed RGB";
    sampling = "8:8:8";
    depth = "24";
    break;
  case AV_PIX_FMT_BGR24:
    color = "packed BGR";
    sampling = "8:8:8";
    depth = "24";
    break;
  case AV_PIX_FMT_YUV422P:
  case AV_PIX_FMT_YUVJ422P:
    color = "planar YUV";
    sampling = "4:2:2";
    depth = "16";
    break; 
  case AV_PIX_FMT_YUV410P:
    color = "planar YUV";
    sampling = "4:1:0";
    depth = "9";
    break;
  case AV_PIX_FMT_YUV411P:
  case AV_PIX_FMT_YUVJ411P:
    color = "planar YUV";
    sampling = "4:1:1";
    depth = "12";
    break;
  case AV_PIX_FMT_GRAY8:
    color = "gray";
    depth = "8";
    break;
  case AV_PIX_FMT_MONOWHITE:
    color = "monowhite";
    depth = "1";
    break;  
  case AV_PIX_FMT_MONOBLACK:
    color = "monoblack";
    depth = "1";
    break;
  case AV_PIX_FMT_PAL8:
    color = "PAL8";
    break;
  case AV_PIX_FMT_YUVJ420P:
  case AV_PIX_FMT_NV12:
    color = "planar YUV";
    sampling = "4:2:0";
    depth = "12";
    break; 
  case AV_PIX_FMT_UYYVYY411:
    color = "packed YUV";
    sampling = "4:1:1";
    depth = "12";
    break;
  case AV_PIX_FMT_BGR8:
  case AV_PIX_FMT_RGB8:
    color = "packed RGB";
    sampling = "3:3:2";
    depth = "8";
    break;
  case AV_PIX_FMT_BGR4:
  case AV_PIX_FMT_RGB4:
    color = "packed RGB";
    sampling = "1:2:1";
    depth = "4";
    break;
  case AV_PIX_FMT_BGR4_BYTE:
  case AV_PIX_FMT_RGB4_BYTE:
  case AV_PIX_FMT_NV21:
    color = "packed RGB";
    sampling = "1:2:1";
    depth = "8";
    break;
  case AV_PIX_FMT_ARGB:
    color = "packed ARGB";
    sampling = "8:8:8:8";
    depth = "32";
    break;
  case AV_PIX_FMT_RGBA:
    color = "packed RGBA";
    sampling = "8:8:8:8";
    depth = "32";
    break;
  case AV_PIX_FMT_ABGR:
    color = "packed ABGR";
    sampling = "8:8:8:8";
    depth = "32";
    break;
  case AV_PIX_FMT_BGRA:
    color = "packed BGRA";
    sampling = "8:8:8:8";
    depth = "32";
    break;
  case AV_PIX_FMT_GRAY16BE:
  case AV_PIX_FMT_GRAY16LE:
    color = "gray";
    depth = "16";
    break;
  case AV_PIX_FMT_YUV440P:
  case AV_PIX_FMT_YUVJ440P:
    color = "planar YUV";
    sampling = "4:4:0";
    break;
  case AV_PIX_FMT_YUVA420P:
    color = "planar YUV";
    sampling = "4:2:0";
    depth = "20";
    break;
  case AV_PIX_FMT_RGB48BE:
  case AV_PIX_FMT_RGB48LE:
  case AV_PIX_FMT_BGR48BE:
  case AV_PIX_FMT_BGR48LE:
    color = "packed RGB";
    sampling = "16:16:16";
    depth = "48";
    break;
  case AV_PIX_FMT_RGB565BE:
  case AV_PIX_FMT_RGB565LE:
    color = "packed RGB";
    sampling = "5:6:5";
    depth = "16";
    break;
  case AV_PIX_FMT_RGB555BE:
  case AV_PIX_FMT_RGB555LE:
    color = "packed RGB";
    sampling = "5:5:5";
    depth = "16";
    break;
  case AV_PIX_FMT_BGR565BE:
  case AV_PIX_FMT_BGR565LE:
    color = "packed BGR";
    sampling = "5:6:5";
    depth = "16";
    break;
  case AV_PIX_FMT_BGR555BE:
  case AV_PIX_FMT_BGR555LE:
    color = "packed BGR";
    sampling = "5:5:5";
    depth = "16";
    break;
  case AV_PIX_FMT_YUV420P16LE:
  case AV_PIX_FMT_YUV420P16BE:
    color = "planar YUV";
    sampling = "4:2:0";
    depth = "24";
    break;
  case AV_PIX_FMT_YUV422P16LE:
  case AV_PIX_FMT_YUV422P16BE:
    color = "planar YUV";
    sampling = "4:2:2";
    depth = "32";
    break;
  case AV_PIX_FMT_YUV444P16LE:
  case AV_PIX_FMT_YUV444P16BE:
    color = "planar YUV";
    sampling = "4:4:4";
    depth = "48";
    break;
  case AV_PIX_FMT_RGB444LE:
  case AV_PIX_FMT_RGB444BE:
    color = "packed RGB";
    sampling = "4:4:4";
    depth = "16";
    break;
  case AV_PIX_FMT_BGR444LE:
  case AV_PIX_FMT_BGR444BE:
    color = "packed BGR";
    sampling = "4:4:4";
    depth = "16";
    break;
  case AV_PIX_FMT_YA8:
    color = "8 bits gray, 8 bits alpha";
    break;
  case AV_PIX_FMT_YUV420P9BE:
  case AV_PIX_FMT_YUV420P9LE:
    color = "planar YUV";
    sampling = "4:2:0";
    depth = "13.5";
    break;
  case AV_PIX_FMT_YUV420P10BE:
  case AV_PIX_FMT_YUV420P10LE:
    color = "planar YUV";
    sampling = "4:2:0";
    depth = "15";
    break;
  case AV_PIX_FMT_YUV422P10BE:
  case AV_PIX_FMT_YUV422P10LE:
    color = "planar YUV";
    sampling = "4:2:2";
    depth = "20";
    break;
  case AV_PIX_FMT_YUV444P9BE:
  case AV_PIX_FMT_YUV444P9LE:
    color = "planar YUV";
    sampling = "4:4:4";
    depth = "27";
    break;
  case AV_PIX_FMT_YUV444P10BE:
  case AV_PIX_FMT_YUV444P10LE:
    color = "planar YUV";
    sampling = "4:4:4";
    depth = "30";
    break;
  case AV_PIX_FMT_YUV422P9BE:
  case AV_PIX_FMT_YUV422P9LE:
    color = "planar YUV";
    sampling = "4:2:2";
    depth = "18";
    break;
  case AV_PIX_FMT_GBRP:
    color = "planar GBR";
    sampling = "4:4:4";
    depth = "24";
    break;
  case AV_PIX_FMT_GBRP9BE:
  case AV_PIX_FMT_GBRP9LE:
    color = "planar GBR";
    sampling = "4:4:4";
    depth = "27";
    break;
  case AV_PIX_FMT_GBRP10BE:
  case AV_PIX_FMT_GBRP10LE:
    color = "planar GBR";
    sampling = "4:4:4";
    depth = "30";
    break;
  case AV_PIX_FMT_GBRP16BE:
  case AV_PIX_FMT_GBRP16LE:
    color = "planar GBR";
    sampling = "4:4:4";
    depth = "48";
    break;
  case AV_PIX_FMT_YUVA444P:
    color = "planar YUV";
    sampling = "4:4:4";
    depth = "32";
    break;
  case AV_PIX_FMT_YUVA420P9BE:
  case AV_PIX_FMT_YUVA420P9LE:
    color = "planar YUV";
    sampling = "4:2:0";
    depth = "22.5";
    break;
  case AV_PIX_FMT_YUVA422P9BE:
    color = "planar YUV";
    sampling = "4:2:2";
    depth = "27";
    break;
  case AV_PIX_FMT_YUVA422P9LE:
    color = "planar YUV";
    sampling = "4:2:2";
    depth = "27";
    break;
  case AV_PIX_FMT_YUVA420P10BE:
  case AV_PIX_FMT_YUVA420P10LE:
    color = "planar YUV";
    sampling = "4:2:0";
    depth = "25";
    break;
  case AV_PIX_FMT_YUVA422P10BE:
  case AV_PIX_FMT_YUVA422P10LE:
    color = "planar YUV";
    sampling = "4:2:2";
    depth = "30";
    break;
  case AV_PIX_FMT_YUVA444P10BE:
  case AV_PIX_FMT_YUVA444P10LE:
    color = "planar YUV";
    sampling = "4:4:4";
    depth = "40";
    break;
  case AV_PIX_FMT_YUVA420P16BE:
  case AV_PIX_FMT_YUVA420P16LE:
    color = "planar YUV";
    sampling = "4:2:0";
    depth = "40";
    break;
  case AV_PIX_FMT_YUVA422P16BE:
  case AV_PIX_FMT_YUVA422P16LE:
    color = "planar YUV";
    sampling = "4:2:2";
    depth = "48";
    break;
  case AV_PIX_FMT_YUVA444P16BE:
  case AV_PIX_FMT_YUVA444P16LE:
    color = "planar YUV";
    sampling = "4:4:4";
    depth = "64";
    break;
  case AV_PIX_FMT_XYZ12LE:
  case AV_PIX_FMT_XYZ12BE:
    color = "packed XYZ";
    sampling = "4:4:4";
    depth = "36";
    break;
  case AV_PIX_FMT_NV16:
    color = "interleaved chroma YUV";
    sampling = "4:2:2";
    depth = "16";
    break;
  case AV_PIX_FMT_RGBA64BE:
  case AV_PIX_FMT_RGBA64LE:
  case AV_PIX_FMT_BGRA64BE:
  case AV_PIX_FMT_BGRA64LE:
    color = "packed RGBA";
    sampling = "16:16:16:16";
    depth = "64";
    break;
  case AV_PIX_FMT_YA16BE:
  case AV_PIX_FMT_YA16LE:
    color = "gray";
    depth = "16";
    break;
  case AV_PIX_FMT_GBRAP:
    color = "planar GBRA";
    sampling = "4:4:4:4";
    depth = "32";
    break;
  case AV_PIX_FMT_GBRAP16BE:
  case AV_PIX_FMT_GBRAP16LE:
    color = "planar GBRA";
    sampling = "4:4:4:4";
    depth = "64";
    break;
  case AV_PIX_FMT_0RGB:
  case AV_PIX_FMT_RGB0:
    color = "packed RGB";
    sampling = "8:8:8";
    depth = "32";
    break;
  case AV_PIX_FMT_0BGR:
  case AV_PIX_FMT_BGR0:
    color = "packed BGR";
    sampling = "8:8:8";
    depth = "32";
    break;
  case AV_PIX_FMT_YUV420P12BE:
  case AV_PIX_FMT_YUV420P12LE:
    color = "planar YUV";
    sampling = "4:2:0";
    depth = "18";
    break;
  case AV_PIX_FMT_YUV420P14BE:
  case AV_PIX_FMT_YUV420P14LE:
    color = "planar YUV";
    sampling = "4:2:0";
    depth = "21";
    break;
  case AV_PIX_FMT_YUV422P12BE:
  case AV_PIX_FMT_YUV422P12LE:
  case AV_PIX_FMT_YUVA422P:
  case AV_PIX_FMT_YUVA422P12BE:
  case AV_PIX_FMT_YUVA422P12LE:
    color = "planar YUV";
    sampling = "4:2:2";
    depth = "24";
    break;
  case AV_PIX_FMT_YUV422P14BE:
  case AV_PIX_FMT_YUV422P14LE:
    color = "planar YUV";
    sampling = "4:2:2";
    depth = "28";
    break;
  case AV_PIX_FMT_YUV444P12BE:
  case AV_PIX_FMT_YUV444P12LE:
  case AV_PIX_FMT_YUVA444P9BE:
  case AV_PIX_FMT_YUVA444P12BE:
  case AV_PIX_FMT_YUVA444P12LE:
  case AV_PIX_FMT_YUVA444P9LE:
    color = "planar YUV";
    sampling = "4:4:4";
    depth = "36";
    break;
  case AV_PIX_FMT_YUV444P14BE:
  case AV_PIX_FMT_YUV444P14LE:
    color = "planar YUV";
    sampling = "4:4:4";
    depth = "42";
    break;
  case AV_PIX_FMT_GBRP12BE:
  case AV_PIX_FMT_GBRP12LE:
    color = "planar GBR";
    sampling = "4:4:4";
    depth = "36";
    break;
  case AV_PIX_FMT_GBRP14BE:
  case AV_PIX_FMT_GBRP14LE:
    color = "planar GBR";
    sampling = "4:4:4";
    depth = "42";
    break;
  case AV_PIX_FMT_YUV440P10LE:
  case AV_PIX_FMT_YUV440P10BE:
    color = "planar YUV";
    sampling = "4:4:0";
    depth = "20";
    break;
  case AV_PIX_FMT_YUV440P12LE:
  case AV_PIX_FMT_YUV440P12BE:
    color = "planar YUV";
    sampling = "4:4:0";
    depth = "24";
    break;
  case AV_PIX_FMT_AYUV64LE:
  case AV_PIX_FMT_AYUV64BE:
    color = "packed AYUV";
    sampling = "4:4:4";
    depth = "64";
    break;
  case AV_PIX_FMT_P010LE:
  case AV_PIX_FMT_P010BE:
    color = "NV12";
    depth = "10";
    break;
  case AV_PIX_FMT_GBRAP12BE:
  case AV_PIX_FMT_GBRAP12LE:
    color = "planar GBR";
    sampling = "4:4:4:4";
    depth = "48";
    break;
  case AV_PIX_FMT_GBRAP10BE:
  case AV_PIX_FMT_GBRAP10LE:
    color = "planar GBR";
    sampling = "4:4:4:4";
    depth = "40";
    break;
  case AV_PIX_FMT_GRAY12BE:
  case AV_PIX_FMT_GRAY12LE:
    color = "gray";
    depth = "12";
    break;
  case AV_PIX_FMT_GRAY10BE:
  case AV_PIX_FMT_GRAY10LE:
    color = "gray";
    depth = "10";
    break;
  case AV_PIX_FMT_P016LE:
  case AV_PIX_FMT_P016BE:
    color = "NV12";
    depth = "16";
    break;
  case AV_PIX_FMT_GRAY9BE:
  case AV_PIX_FMT_GRAY9LE:
    color = "gray";
    depth = "9";
    break;
  case AV_PIX_FMT_GBRPF32BE:
  case AV_PIX_FMT_GBRPF32LE:
    color = "IEEE-754 single precision planar GBR";
    sampling = "4:4:4";
    depth = "96";
    break;
  case AV_PIX_FMT_GBRAPF32BE:
  case AV_PIX_FMT_GBRAPF32LE:
    color = "IEEE-754 single precision planar GBRA";
    sampling = "4:4:4:4";
    depth = "128";
    break;
  case AV_PIX_FMT_GRAY14BE:
  case AV_PIX_FMT_GRAY14LE:
    color = "gray";
    depth = "14";
    break;
  case AV_PIX_FMT_GRAYF32BE:
  case AV_PIX_FMT_GRAYF32LE:
    color = "IEEE-754 single precision Y";
    depth = "32";
    break;
  case AV_PIX_FMT_NV24:
  case AV_PIX_FMT_NV42:
  case AV_PIX_FMT_YUV444P:
  case AV_PIX_FMT_YUVJ444P:
    color = "planar YUV";
    sampling = "4:4:4";
    depth = "24";
    break;
  case AV_PIX_FMT_Y210BE:
  case AV_PIX_FMT_Y210LE:
    color = "packed YUV";
    sampling = "4:2:2";
    depth = "20";
    break;
  case AV_PIX_FMT_X2RGB10LE:
  case AV_PIX_FMT_X2RGB10BE:
    color = "packed RGB";
    sampling = "10:10:10";
    depth = "30";
    break;
  case AV_PIX_FMT_X2BGR10LE:
  case AV_PIX_FMT_X2BGR10BE:
    color = "packed BGR";
    sampling = "10:10:10";
    depth = "30";
    break;
  case AV_PIX_FMT_NV20BE:
  case AV_PIX_FMT_NV20LE:
  case AV_PIX_FMT_P210BE:
  case AV_PIX_FMT_P210LE:
    color = "interleaved chroma YUV";
    sampling = "4:2:2";
    depth = "20";
    break;
  case AV_PIX_FMT_P410BE:
  case AV_PIX_FMT_P410LE:
    color = "interleaved chroma YUV";
    sampling = "4:4:4";
    depth = "30";
    break;
  case AV_PIX_FMT_P216BE:
  case AV_PIX_FMT_P216LE:
    color = "interleaved chroma YUV";
    sampling = "4:2:2";
    depth = "32";
    break;
  case AV_PIX_FMT_P416BE:
  case AV_PIX_FMT_P416LE:
    color = "interleaved chroma YUV";
    sampling = "4:4:4";
    depth = "48";
    break;
  default:
    break;
  }
  if (color)    desc.emplace_back("Color",     color   );
  if (sampling) desc.emplace_back("Sampling",  sampling);
  if (depth)    desc.emplace_back("Bit depth", depth   );
}

// -----------------------------------------------------------------------------------------------------------------------------
std::string printChannelLayout(uint64_t layout, std::stringstream& buffer)
{
  clear(buffer);
  int index  = 1;
  bool first = true;
  while (layout > 0) {
    if (layout & 1) {
      if (first) first = false;
      else buffer << " ";
      switch (index) {
      case 1:
        buffer << "L";
        break;
      case 2:
        buffer << "R";
        break;
      case 3:
        buffer << "C";
        break;
      case 4:
        buffer << "LFE";
        break;
      case 10:
        buffer << "Ls";
        break;
      case 11:
        buffer << "Rs";
        break;
      default:
        buffer << index;
        break;
      }
    }
    ++index;
    layout = layout >> 1;
  }
  return buffer.str();
}

// -----------------------------------------------------------------------------------------------------------------------------
std::string printSamplingRate(int value, std::stringstream& buffer, bool clr = true)
{
  int mult = 0;
  double v = static_cast<double>(value);
  while (v > 1000.0) {
    v /= 1000.0;
    ++mult;
  }
  if (clr) clear(buffer);
  buffer << static_cast<int>(v);
  switch (mult) {
  case 0:
    buffer << " Hz";
    break;
  case 1:
    buffer << " kHz";
    break;
  case 2:
    buffer << " mHz";
    break;
  case 3:
    buffer << " gHz";
    break;
  case 4:
    buffer << " tHz";
    break;
  case 5:
    buffer << " pHz";
    break;
  case 6:
    buffer << " eHz";
    break;
  case 7:
    buffer << " zHz";
    break;
  case 8:
    buffer << " yHz";
    break;
  default:
    buffer << " ?Hz";
    break;
  }
  return clr ? buffer.str() : std::string();
}

// -----------------------------------------------------------------------------------------------------------------------------
void printSampleFormat(AVSampleFormat format, Description& desc)
{
  char const* layout = nullptr;
  char const* scan   = nullptr;
  switch (format) {
  case AV_SAMPLE_FMT_U8:
    layout = "Unsigned 8 bits";
    scan   = "Progressive";
  case AV_SAMPLE_FMT_S16:
    layout = "Unsigned 8 bits";
    scan   = "Progressive";
  case AV_SAMPLE_FMT_S32:
    layout = "Unsigned 8 bits";
    scan   = "Progressive";
  case AV_SAMPLE_FMT_FLT:
    layout = "Float";
    scan   = "Progressive";
  case AV_SAMPLE_FMT_DBL:
    layout = "Double";
    scan   = "Progressive";
  case AV_SAMPLE_FMT_U8P:
    layout = "Unsigned 8 bits";
    scan   = "Planar";
  case AV_SAMPLE_FMT_S16P:
    layout = "Signed 16 bits";
    scan   = "Planar";
  case AV_SAMPLE_FMT_S32P:
    layout = "Signed 32 bits";
    scan   = "Planar";
  case AV_SAMPLE_FMT_FLTP:
    layout = "Float";
    scan   = "Planar";
  case AV_SAMPLE_FMT_DBLP:
    layout = "Double";
    scan   = "Planar";
  case AV_SAMPLE_FMT_S64:
    layout = "Signed 64 bits";
    scan   = "Progressive";
  case AV_SAMPLE_FMT_S64P:
    layout = "Signed 64 bits";
    scan   = "Planar";
  default:
    break;
  }
  if (layout) desc.emplace_back("Bit layout", layout);
  if (scan)   desc.emplace_back("Scan type",  scan);
}

// -----------------------------------------------------------------------------------------------------------------------------
int64_t parseDuration(AVDictionary* dict)
{
  auto duration = av_dict_get(dict, "DURATION", nullptr, 0);
  if (!duration) return -1;
  const char* value = duration->value;
  int hours   = 0;
  int minutes = 0;
  int seconds = 0;
  if (strlen(value) < 8) return -1;
  if (!sscanf_s(value,     "%d", &hours  )) return -1;
  if (!sscanf_s(value + 3, "%d", &minutes)) return -1;
  if (!sscanf_s(value + 6, "%d", &seconds)) return -1;
  int64_t result = static_cast<int64_t>(seconds) + static_cast<int64_t>(minutes) * 60 + static_cast<int64_t>(hours) * 3600;
  return result * AV_TIME_BASE; 
}

// -----------------------------------------------------------------------------------------------------------------------------
int64_t getDuration(AVStream* stream)
{
  if (stream->duration > 0) {
    const double result = static_cast<double>(stream->duration) * stream->time_base.num / stream->time_base.den;
    return static_cast<int64_t>(result) * AV_TIME_BASE;
  }
  return parseDuration(stream->metadata);
}

// -----------------------------------------------------------------------------------------------------------------------------
int64_t parseBitRate(AVDictionary* dict)
{
  auto bitRate = av_dict_get(dict, "BPS", nullptr, 0);
  if (!bitRate) return -1;
  int64_t value = 0;
  if (strlen(bitRate->value) < 1) return -1;
  if (!sscanf_s(bitRate->value, "%lld", &value)) return -1;
  return value;
}

// -----------------------------------------------------------------------------------------------------------------------------
int64_t getBitRate(AVStream* stream)
{
  if (stream->codecpar->bit_rate > 0) return stream->codecpar->bit_rate;
  return parseBitRate(stream->metadata);
}

// -----------------------------------------------------------------------------------------------------------------------------
int64_t getBitRate(AVFormatContext* context)
{
  if (context->bit_rate > 0) return context->bit_rate;
  return parseBitRate(context->metadata);
}

// -----------------------------------------------------------------------------------------------------------------------------
int64_t getFrameCount(AVStream* stream)
{
  if (stream->nb_frames > 0) return stream->nb_frames;
  auto count = av_dict_get(stream->metadata, "NUMBER_OF_FRAMES", nullptr, 0);
  if (!count) return -1;
  int64_t value = 0;
  if (strlen(count->value) < 1) return -1;
  if (!sscanf_s(count->value, "%lld", &value)) return -1;
  return value;
}

// -----------------------------------------------------------------------------------------------------------------------------
std::string makeVideoStreamName(AVStream* stream)
{
  std::stringstream out;
  const auto param   = stream->codecpar;
  const auto format  = static_cast<AVPixelFormat>(param->format);
  const auto bitRate = getBitRate(stream);
  out << toUpper(avcodec_get_name(param->codec_id));
  if (param->profile > 0) out << ", " << avcodec_profile_name(param->codec_id, param->profile);
  out << ", " << printPixelFormatName(format);
  out << ", " << param->width << "x" << param->height;
  if (bitRate > 0) { 
    out << ", ";
    printBitRate(bitRate, out, false);
  }
  return out.str();
}

// -----------------------------------------------------------------------------------------------------------------------------
std::string makeAudioStreamName(AVStream* stream)
{
  std::stringstream out;
  const auto param    = stream->codecpar;
  const auto language = av_dict_get(stream->metadata, "language", nullptr, 0);
  const auto bitRate  = getBitRate(stream);
  if (language) out << language->value << ", ";
  out << toUpper(avcodec_get_name(param->codec_id));
  if (param->profile > 0)  out << ", " << avcodec_profile_name(param->codec_id, param->profile);
  out << ", ";
  printSamplingRate(param->sample_rate, out, false);
  out << ", " << param->channels <<" channels";
  if (bitRate > 0) {
    out << ", ";
    printBitRate(bitRate, out, false);
  }
  return out.str();
}

// -----------------------------------------------------------------------------------------------------------------------------
std::string makeSubtitleStreamName(AVStream* stream)
{
  std::stringstream out;
  const auto param    = stream->codecpar;
  const auto title    = av_dict_get(stream->metadata, "title",    nullptr, 0);
  const auto language = av_dict_get(stream->metadata, "language", nullptr, 0);
  if (title)    out << title->value << " ";
  if (language) out << "[" << language->value << "] ";
  out << toUpper(avcodec_get_name(param->codec_id));
  return out.str();
}

// -----------------------------------------------------------------------------------------------------------------------------
Description makeVideoStreamDescription(AVStream* stream)
{
  Description desc;
  std::stringstream buffer;
  const auto param = stream->codecpar;
  const auto duration = getDuration  (stream);
  const auto bitRate  = getBitRate   (stream);
  const auto format   = static_cast<AVPixelFormat>(param->format);
                                  desc.emplace_back("Format",       toUpper(avcodec_get_name(param->codec_id), buffer));
  if (param->profile > 0)         desc.emplace_back("Profile",      avcodec_profile_name(param->codec_id, param->profile));
  if (param->codec_tag > 0)       desc.emplace_back("Codec",        printCodecTag(param->codec_tag, buffer));
  if (duration > 0)               desc.emplace_back("Duration",     printDuration(duration,         buffer));
  if (bitRate  > 0)               desc.emplace_back("Bit rate",     printBitRate (bitRate,          buffer));
                                  desc.emplace_back("Width",        std::to_string(param->width));
                                  desc.emplace_back("Height",       std::to_string(param->height));
                                  desc.emplace_back("Frame rate",   printFrameRate(stream->avg_frame_rate, buffer));
                                  desc.emplace_back("Pixel format", printPixelFormatName(format));
                                  printPixelFormat(format, desc);
                                  desc.emplace_back("Scan type",    printFieldOrder(param->field_order));
  return desc;
}

// -----------------------------------------------------------------------------------------------------------------------------
Description makeAudioStreamDescription(AVStream* stream)
{
  Description desc;
  std::stringstream buffer;
  const auto param = stream->codecpar;
  const auto duration = getDuration  (stream);
  const auto bitRate  = getBitRate   (stream);
  const auto frames   = getFrameCount(stream);
  const auto format   = static_cast<AVSampleFormat>(stream->codecpar->format);
  const auto language = av_dict_get(stream->metadata, "language", nullptr, 0);
                                  desc.emplace_back("Format",     toUpper(avcodec_get_name(param->codec_id), buffer));
  if (param->profile > 0)         desc.emplace_back("Profile",    avcodec_profile_name(param->codec_id, param->profile));
  if (param->codec_tag > 0)       desc.emplace_back("Codec",      printCodecTag(param->codec_tag, buffer));
  if (duration > 0)               desc.emplace_back("Duration",   printDuration(duration,         buffer));
  if (bitRate  > 0)               desc.emplace_back("Bit rate",   printBitRate (bitRate,          buffer));
                                  desc.emplace_back("Channels",   std::to_string    (param->channels              ));
                                  desc.emplace_back("Layout",     printChannelLayout(param->channel_layout, buffer));
                                  desc.emplace_back("Sampling",   printSamplingRate (param->sample_rate,    buffer));
  if (frames > 0 && duration > 0) desc.emplace_back("Frame rate", printFrameRate    (frames, duration,      buffer));
                                  printSampleFormat(format, desc);
  if (language > 0)               desc.emplace_back("Language",   language->value);
  return desc;
}

// -----------------------------------------------------------------------------------------------------------------------------
Description makeSubtitleStreamDescription(AVStream* stream)
{
  Description desc;
  std::stringstream buffer;
  const auto param    = stream->codecpar;
  const auto duration = getDuration  (stream);
  const auto bitRate  = getBitRate   (stream);
  const auto frames   = getFrameCount(stream);
  const auto language = av_dict_get(stream->metadata, "language", nullptr, 0);
  const auto title    = av_dict_get(stream->metadata, "title",    nullptr, 0);
                            desc.emplace_back("Format",   toUpper(avcodec_get_name(param->codec_id), buffer));
  if (param->profile > 0)   desc.emplace_back("Profile",  avcodec_profile_name(param->codec_id, param->profile));
  if (param->codec_tag > 0) desc.emplace_back("Codec",    printCodecTag(param->codec_tag, buffer));
  if (duration > 0)         desc.emplace_back("Duration", printDuration(duration,         buffer));
  if (bitRate  > 0)         desc.emplace_back("Bit rate", printBitRate (bitRate,          buffer));
  if (frames   > 0)         desc.emplace_back("Elements", std::to_string(frames));
  if (language > 0)         desc.emplace_back("Language", language->value);
  if (title    > 0)         desc.emplace_back("Title",    title->value);
  return desc;
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
int64_t getDuration(AVFormatContext* context)
{
  if (context->duration > 0) return context->duration;
  return parseDuration(context->metadata);
}

// -----------------------------------------------------------------------------------------------------------------------------
Description makeFormatDescription(AVFormatContext* context)
{
  Description desc;
  std::stringstream buffer;
  const auto bitRate  = getBitRate(context);
  const auto duration = getDuration(context);
  const auto encoder  = av_dict_get(context->metadata, "encoder", nullptr, 0);
  const auto title    = av_dict_get(context->metadata, "title",   nullptr, 0);
  desc.emplace_back("File name", context->url);
  desc.emplace_back("File size", printSize(static_cast<int64_t>(boost::filesystem::file_size(context->url)), buffer));
  desc.emplace_back("Format",    std::string(context->iformat->long_name) + " (" + context->iformat->name + ")");
  if (bitRate  > 0) desc.emplace_back("Bit rate", printBitRate (bitRate,  buffer));
  if (duration > 0) desc.emplace_back("Duration", printDuration(duration, buffer));
  if (encoder)      desc.emplace_back("Encoder",  encoder->value);
  if (title)        desc.emplace_back("Title",    title->value);
  return desc;
}

// -----------------------------------------------------------------------------------------------------------------------------
std::string makeStreamName(AVStream* stream)
{
  switch (stream->codecpar->codec_type) {
  case AVMEDIA_TYPE_VIDEO:
    return makeVideoStreamName(stream);
  case AVMEDIA_TYPE_AUDIO:
    return makeAudioStreamName(stream);
  case AVMEDIA_TYPE_SUBTITLE:
    return makeSubtitleStreamName(stream);
  default:
    break;
  }
  return std::string();
}

// -----------------------------------------------------------------------------------------------------------------------------
Description makeStreamDescription(AVStream* stream)
{
  switch (stream->codecpar->codec_type) {
  case AVMEDIA_TYPE_VIDEO:
    return makeVideoStreamDescription(stream);
  case AVMEDIA_TYPE_AUDIO:
    return makeAudioStreamDescription(stream);
  case AVMEDIA_TYPE_SUBTITLE:
    return makeSubtitleStreamDescription(stream);
  default:
    break;
  }
  return {};
}

// -----------------------------------------------------------------------------------------------------------------------------
std::string getTitle(AVFormatContext* context)
{
  auto title = av_dict_get(context->metadata, "title", nullptr, 0);
  if (title) {
    return title->value;
  }
  else {
    size_t i = 0;
    size_t p = 0;
    const char* str = context->url;
    while (str[i] != 0) {
      if (str[i] == '\\' || str[i] == '/') p = i;
      ++i;
    }
    if (p < i) return str + p + 1;
  }
  return std::string();
}

} // !namespace yaga
