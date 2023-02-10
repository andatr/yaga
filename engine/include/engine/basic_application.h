#ifndef YAGA_ENGINE_BASIC_APPLICATION
#define YAGA_ENGINE_BASIC_APPLICATION

#include "utility/compiler.h"

#include <memory>

DISABLE_WARNINGS
#include <boost/noncopyable.hpp>
ENABLE_WARNINGS

#include "config.h"
#include "assets/serializer.h"
#include "engine/application.h"

namespace yaga {

class BasicApplication
  : private boost::noncopyable
  , public Application
{
public:
  explicit BasicApplication(ConfigPtr config);
  virtual ~BasicApplication();

protected:
  void init(Context* context, Input* input) override;
  void resize()      override;
  bool loop()        override;
  void stop()        override;
  void shutdown()    override;
  void gui()         override;
  ConfigPtr config() override { return config_; }

protected:
  ConfigPtr config_;
  bool running_;
  Context* context_;
  Input* input_;
};

typedef std::shared_ptr<BasicApplication> BasicApplicationPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_BASIC_APPLICATION
