#ifndef YAGA_ENGINE_BASIC_APPLICATION
#define YAGA_ENGINE_BASIC_APPLICATION

#include <memory>
#include <boost/noncopyable.hpp>
#include <boost/property_tree/ptree_fwd.hpp>

#include "assets/storage.h"
#include "assets/serializer.h"
#include "engine/application.h"

namespace yaga {

class BasicApplication
  : private boost::noncopyable
  , public Application
{
public:
  BasicApplication(const boost::property_tree::ptree& options);
  virtual ~BasicApplication();

protected:
  void init(Context* context, Input* input) override;
  void resize()   override;
  bool loop()     override;
  void stop()     override;
  void shutdown() override;
  void gui()      override;

protected:
  bool running_;
  assets::StoragePtr assets_;
  assets::SerializerPtr serializer_;
  Context* context_;
  Input* input_;
};

typedef std::unique_ptr<BasicApplication> BasicApplicationPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_BASIC_APPLICATION
