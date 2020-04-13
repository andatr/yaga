#ifndef YAGA_UTILITY_ID
#define YAGA_UTILITY_ID

#include <cstdint>

namespace yaga
{

class Id
{
public:
  typedef uint32_t type;
public:
  explicit Id(type id) { id_ = id; }
  type id() const { return id_; }
protected:
  type id_;
};

} // !namespace yaga

#endif // !YAGA_UTILITY_ID
