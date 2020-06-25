#ifndef YAGA_VULKAN_RENDERER_CAMERA
#define YAGA_VULKAN_RENDERER_CAMERA

#include <memory>
#include <vector>

#include "buffer.h"
#include "uniform.h"
#include "engine/camera.h"

namespace yaga
{
namespace vk
{

class Camera : public yaga::Camera
{
friend class CameraPool;
public:
  struct Frame
  {
    BufferPtr buffer;
    VkDescriptorSet descriptor;
    VkCommandBuffer command;
    bool dirty;
  };
  typedef yaga::Camera Parent;
public:
  explicit Camera(Object* obj, assets::Camera* asset, CameraPool* pool, uint32_t frames);
  virtual ~Camera();
  Frame& frame(uint32_t index) { return frames_[index]; }
private:
  void updateView() override;
  void onAssetUpdated(assets::CameraProperty prop) override;
private:
  CameraPool* pool_;
  UniformObject uniform_;
  std::vector<Frame> frames_;
};

typedef std::unique_ptr<Camera> CameraPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_CAMERA
