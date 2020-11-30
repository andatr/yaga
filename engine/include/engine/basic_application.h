#ifndef YAGA_ENGINE_BASIC_APPLICATION
#define YAGA_ENGINE_BASIC_APPLICATION

#include <memory>
#include <boost/noncopyable.hpp>

#include "assets/storage.h"
#include "engine/application.h"

namespace yaga {

class BasicApplication
  : private boost::noncopyable
  , public Application
{
public:
  BasicApplication(assets::Serializer* serializer);
  virtual ~BasicApplication();

protected:
  void init(RenderingContext* renderer, Input* input) override;
  void resize() override;
  void loop(float delta) override;
  void shutdown() override;

protected:
  assets::StoragePtr persistentAssets_;
  assets::Serializer* serializer_;
  RenderingContext* renderer_;
  Input* input_;
};

typedef std::unique_ptr<BasicApplication> BasicApplicationPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_BASIC_APPLICATION