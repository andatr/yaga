#ifndef YAGA_MODEL_VIEWER_SRC_MODEL_VIEWER
#define YAGA_MODEL_VIEWER_SRC_MODEL_VIEWER

#include <memory>
#include <boost/dll/alias.hpp>

#include "assets/mesh.h"
#include "engine/basic_application.h"
#include "engine/camera.h"
#include "engine/object.h"

namespace yaga {

  struct MeshMetadata;

class ModelViewer : public BasicApplication
{
public:
  ModelViewer(assets::Serializer* serializer);
  virtual ~ModelViewer();

protected:
  void init(Context* context, Input* input) override;
  void resize() override;
  bool loop() override;
  void shutdown() override;

private:
  typedef BasicApplication base;

private:
  assets::Mesh* importModel(const std::string& path, const std::string& path2, MeshMetadata& meta);
  void setupCamera();
  void createBoundingBox(assets::Mesh* modelMesh, const MeshMetadata& meta);

private:
  std::vector<ObjectPtr> objects_;
  Camera* camera_;
  Transform* cameraPosition_;
  Transform* objectPosition_;
};

typedef std::unique_ptr<ModelViewer> ModelViewerPtr;

ApplicationPtr createApplication(assets::Serializer* serializer);

} // !namespace yaga

BOOST_DLL_ALIAS(yaga::createApplication, createApplication)

#endif // !YAGA_MODEL_VIEWER_SRC_MODEL_VIEWER
