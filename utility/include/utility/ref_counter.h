#ifndef YAGA_UTILITY_REF_COUNTER
#define YAGA_UTILITY_REF_COUNTER

#include <cstdint>

namespace yaga
{

class RefCounter
{
public:
  explicit RefCounter() : counter_(0) {}
  uint32_t refCounter() const { return counter_; }
  uint32_t incrementCounter() { return ++counter_; }
  uint32_t decrementCounter() { return counter_ > 0 ? --counter_ : 0; }
protected:
  uint32_t counter_;
};

} // !namespace yaga

#endif // !YAGA_UTILITY_REF_COUNTER
