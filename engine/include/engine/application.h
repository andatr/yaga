#ifndef YAGA_ENGINE_APPLICATION
#define YAGA_ENGINE_APPLICATION

#include <memory>

#include "engine/input.h"
#include "engine/context.h"

namespace yaga {

class Application
{
public:
  virtual ~Application() {}
  virtual void init(Context* context, Input* input) = 0;
  virtual void resize()   = 0;
  virtual bool loop()     = 0;
  virtual void shutdown() = 0;
  virtual void gui()      = 0;
};

typedef std::unique_ptr< Application> ApplicationPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_APPLICATION
