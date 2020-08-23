#ifndef YAGA_MODEL_VIEWER_SRC_MODEL_VIEWER
#define YAGA_MODEL_VIEWER_SRC_MODEL_VIEWER

#include <memory>
#include <boost/dll/alias.hpp>

#include "engine/basic_application.h"
#include "engine/camera.h"
#include "engine/object.h"

namespace yaga {

class ModelViewer : public BasicApplication
{
public:
  ModelViewer(assets::Serializer* serializer);
  virtual ~ModelViewer();

protected:
  void init(RenderingContext* renderer, Input* input) override;
  void resize() override;
  void loop(float delta) override;
  void shutdown() override;

private:
  typedef BasicApplication base;

private:
  std::vector<ObjectPtr> objects_;
  Camera* camera_;
  assets::Camera* cameraAsset_;
  Transform* cameraPosition_;
  Transform* objectPosition_;
};

typedef std::unique_ptr<ModelViewer> ModelViewerPtr;

ApplicationPtr createApplication(assets::Serializer* serializer);

} // !namespace yaga

BOOST_DLL_ALIAS(yaga::createApplication, createApplication)

#endif // !YAGA_MODEL_VIEWER_SRC_MODEL_VIEWER
