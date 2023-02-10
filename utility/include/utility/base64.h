#ifndef YAGA_UTILITY_BASE64
#define YAGA_UTILITY_BASE64

#include <string>

namespace yaga {
namespace base64 {

std::string decode(const std::string& str);
std::string encode(const std::string& str);

} // !namespace base64
} // !namespace yaga

#endif // !YAGA_UTILITY_BASE64