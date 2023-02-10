#include "precompiled.h"
#include "utility/base64.h"
#include "utility/exception.h"

// base64 alphabet contains 64 characters
// that means base64 character can be represented by 6 bits ( 2 ^ 6 = 64 )
// ascii character takes 8 bits
// so the easies way to convert ascii to base64 is to take 3 ascii characters ( 3 * 8 = 24 )
// and reinterpret them as 4 base64 characters ( 6 * 4 = 24 )

// ascii bits:  aaaa aaaa bbbb bbbb cccc cccc
// base64 bits: 1111 1122 2222 3333 3344 4444

// base64 ch1:  1111 1100 0000 0000 0000 0000 = ((ch1 & 0xFC) >> 2)
// base64 ch2:  0000 0011 1111 0000 0000 0000 = ((ch2 & 0xF0) >> 4) + ((ch1 & 0x03) << 4) 
// base64 ch3:  0000 0000 0000 1111 1100 0000 = ((ch3 & 0xC0) >> 6) + ((ch2 & 0x0F) << 2)
// base64 ch4:  0000 0000 0000 0000 0011 1111 =   ch3 & 0x3F

namespace yaga   {
namespace base64 {
namespace {

const std::array<char, 128> reverseAlphabet();

const char BAD_CHAR = 255;
const char PAD_CHAR = '=';
const char* alphabet = { "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" };
const std::array<char, 128> alphabetR = reverseAlphabet();

// -----------------------------------------------------------------------------------------------------------------------------
const std::array<char, 128> reverseAlphabet()
{
  std::array<char, 128> reversed {};
  memset(reversed.data(), BAD_CHAR, 128);
  char i = 0;
  while (alphabet[i]) {
    const char ch = alphabet[i];
    reversed[ch] = i;
    ++i;
  }
  return reversed;
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
std::string decode(const std::string& str)
{
#define bad(inc) (str[i + inc] >= alphabetR.size() || alphabetR[str[i + inc]] == BAD_CHAR)
#define getCharL(inc, flag, shift) ((alphabetR[str[i + inc]]) << shift)
#define getCharR(inc, flag, shift) ((alphabetR[str[i + inc]]) >> shift)

  if (str.empty()) return std::string();
  if (str.size() % 4 != 0) throw 1;
  std::string decoded;
  decoded.reserve(str.size() / 4 * 3);
  size_t i = 0;
  const size_t size = str.size() < 4 ? str.size() : str.size() - 4;
  try {
    for (; i < size; i += 4) {
      if (bad(0) || bad(1) || bad(2) || bad(3)) throw 1;
      decoded.push_back(getCharL(0, 0xFF, 2) + getCharR(1, 0x30, 4));
      decoded.push_back(getCharL(1, 0x0F, 4) + getCharR(2, 0x3C, 2));
      decoded.push_back(getCharL(2, 0x03, 6) + getCharR(3, 0xFF, 0));
    }
    if (i < str.size()) {
      if (bad(0) || bad(1)) throw 1;
      decoded.push_back(getCharL(0, 0xFF, 2) + getCharR(1, 0x30, 4));
      if (str[i + 2] != PAD_CHAR) {
        if (bad(2)) throw 1;
        decoded.push_back(getCharL(1, 0x0F, 4) + getCharR(2, 0x3C, 2));
        if (str[i + 3] != PAD_CHAR) {
          if (bad(3)) throw 1;
          decoded.push_back(getCharL(2, 0x03, 6) + getCharR(3, 0xFF, 0));
        }
      }
    }
  }
  catch (...) {
    THROW("Bad base64 string %1%", str);
  }
  return decoded;

#undef getCharR
#undef getCharL
#undef bad
}

// -----------------------------------------------------------------------------------------------------------------------------
std::string encode(const std::string& str)
{
  if (str.empty()) return std::string();
  const size_t encodedSize = (str.size() + 2) / 3 * 4;
  std::string encoded;
  encoded.reserve(encodedSize);
  size_t i = 0;
  const size_t size = str.size() / 3 * 3;
  for (; i < size; i += 3) {
    const char ch1 = str[i];
    const char ch2 = str[i + 1];
    const char ch3 = str[i + 2];
    encoded.push_back(alphabet[ (ch1 & 0xFC) >> 2]);
    encoded.push_back(alphabet[((ch1 & 0x03) << 4) + ((ch2 & 0xF0) >> 4)]);
    encoded.push_back(alphabet[((ch2 & 0x0F) << 2) + ((ch3 & 0xC0) >> 6)]);
    encoded.push_back(alphabet[  ch3 & 0x3F]); 
  }
  if (i < str.size()) {
    const char ch1 = str[i];
    encoded.push_back(alphabet[(ch1 & 0xFC) >> 2]);
    if (i + 1 < str.size()) {
      const char ch2 = str[i + 1];
      encoded.push_back(alphabet[((ch1 & 0x03) << 4) + ((ch2 & 0xF0) >> 4)]);
      if (i + 2 < str.size()) {
        const char ch3 = str[i + 2];
        encoded.push_back(alphabet[((ch2 & 0x0F) << 2) + ((ch3 & 0xC0) >> 6)]);
        encoded.push_back(alphabet[  ch3 & 0x3F]);
      }
      else {
        encoded.push_back(alphabet[(ch2 & 0x0F) << 2]);
        encoded.push_back(PAD_CHAR);
      }
    }
    else {
      encoded.push_back(alphabet[(ch1 & 0x03) << 4]);
      encoded.push_back(PAD_CHAR);
      encoded.push_back(PAD_CHAR);
    }
  }
  return encoded;
}

} // !namespace base64
} // !namespace yaga
