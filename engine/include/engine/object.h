#ifndef YAGA_ENGINE_OBJECT
#define YAGA_ENGINE_OBJECT

#include <memory>
#include <vector>
#include <unordered_set>

#include <boost/core/noncopyable.hpp>
#include <glm/glm.hpp>

namespace yaga
{

class Object;
typedef std::unique_ptr<Object> ObjectPtr;
typedef std::unordered_set<Object*> ObjectSet;

class Object : private boost::noncopyable
{
public:
  explicit Object();
  virtual ~Object() {}
  bool active() const { return active_; }
  Object& active(bool a) { active_ = a; return *this; }
  glm::mat4& transoform() { return transform_; }
  Object& transoform(const glm::mat4& t) { transform_ = t; return *this; }
  Object* parent() const { return parent_; }
  Object& parent(Object* p) { parent_ = p; return *this; }
  ObjectSet& children() { return children_; }
  virtual void update();
protected:
  friend class Scene;
protected:
  bool active_;
  Object* parent_;
  glm::mat4 transform_;
  ObjectSet children_;
};

} // !namespace yaga

#endif // !YAGA_ENGINE_OBJECT
