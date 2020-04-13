#ifndef YAGA_ENGINE_CAMERA
#define YAGA_ENGINE_CAMERA

#include <memory>

#include "engine/object.h"

namespace yaga
{

class Camera;
typedef std::unique_ptr<Camera> CameraPtr;

class Camera : public Object
{
public:
  explicit Camera();
  virtual ~Camera() {}
public:
  glm::mat4& matrix() { return matrix_; }
  Camera& matrix(const glm::mat4& m) { matrix_ = m; return *this; }
protected:
  glm::mat4 matrix_;
};

} // !namespace yaga

#endif // !YAGA_ENGINE_CAMERA
