#ifndef YAGA_ENGINE_POST_PROCESSOR
#define YAGA_ENGINE_POST_PROCESSOR

#include <functional>
#include <memory>

namespace yaga {

class PostProcessor
{
friend class Context;

public:
  typedef std::function<void(void* data, size_t size)> Proc;

public:
  virtual ~PostProcessor() {}

protected:
  explicit PostProcessor(int order, bool hostMemory, Proc proc);

protected:
  int order_;
  bool host_;
  Proc proc_;
};

typedef std::unique_ptr<PostProcessor> PostProcessorPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_POST_PROCESSOR
