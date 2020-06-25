#ifndef YAGA_ASSETS_MODEL
#define YAGA_ASSETS_MODEL

#include <memory>
#include <istream>

#include "assets/asset.h"
#include "assets/material.h"
#include "assets/mesh.h"
#include "assets/serializer.h"

namespace yaga
{
namespace assets
{

class Model;
typedef std::unique_ptr<Model> ModelPtr;

class Model : public Asset
{
public:
  explicit Model(const std::string& name);
  virtual ~Model();
  Material* material() const { return material_; }
  Mesh* mesh() const { return mesh_; }
public:
  static const SerializationInfo serializationInfo;
  static ModelPtr deserializeBinary(const std::string& name, std::istream& stream,
    size_t size, RefResolver& resolver);
  static ModelPtr deserializeFriendly(const std::string& name, const std::string& path, RefResolver& resolver);
private:
  Material* material_;
  Mesh* mesh_;
};

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_MODEL
