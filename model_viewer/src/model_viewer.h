#ifndef YAGA_MODEL_VIEWER_SRC_MODEL_VIEWER
#define YAGA_MODEL_VIEWER_SRC_MODEL_VIEWER

#include <memory>
#include <boost/dll/alias.hpp>

#include "engine/basic_game.h"
#include "engine/camera.h"
#include "engine/game.h"
#include "engine/object.h"

namespace yaga {

class ModelViewer : public BasicGame
{
public:
  ModelViewer(assets::SerializerPtr serializer, assets::StoragePtr storage);
  virtual ~ModelViewer();

protected:
  void init(Application* app) override;
  void resize() override;
  void loop(float delta) override;
  void shutdown() override;

private:
  typedef BasicGame base;

private:
  std::vector<ObjectPtr> objects_;
  Camera* camera_;
  assets::Camera* cameraAsset_;
  Transform* cameraPosition_;
  Transform* objectPosition_;
};

typedef std::unique_ptr<ModelViewer> ModelViewerPtr;

GamePtr createGame(assets::SerializerPtr serializer, assets::StoragePtr storage);

} // !namespace yaga

BOOST_DLL_ALIAS(yaga::createGame, createGame)

#endif // !YAGA_MODEL_VIEWER_SRC_MODEL_VIEWER
