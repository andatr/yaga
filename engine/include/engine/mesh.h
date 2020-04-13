#ifndef YAGA_ENGINE_MESH
#define YAGA_ENGINE_MESH

#include <memory>

#include <boost/core/noncopyable.hpp>

#include "engine/vertex.h"

namespace yaga
{

class Mesh;
typedef std::unique_ptr<Mesh> MeshPtr;

class Mesh : private boost::noncopyable
{
public:
  virtual ~Mesh() {}
};

} // !namespace yaga

#endif // !YAGA_ENGINE_MESH
