#ifndef YAGA_ENGINE_MATERIAL
#define YAGA_ENGINE_MATERIAL

#include <memory>

#include <boost/core/noncopyable.hpp>

namespace yaga
{

class Material;
typedef std::unique_ptr<Material> MaterialPtr;

class Material : private boost::noncopyable
{
public:
  virtual ~Material() {}
};

} // !namespace yaga

#endif // !YAGA_ENGINE_MATERIAL
