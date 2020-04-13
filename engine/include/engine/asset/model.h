#ifndef YAGA_ENGINE_ASSET_MODEL
#define YAGA_ENGINE_ASSET_MODEL

#include <memory>
#include <istream>
#include <ostream>

#include "engine/asset/asset.h"
#include "engine/asset/material.h"
#include "engine/asset/mesh.h"
#include "engine/asset/serializer.h"
#include "utility/array.h"

namespace yaga
{
namespace asset
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
  static ModelPtr deserialize(const std::string& name, std::istream& stream, size_t size);
  static ModelPtr deserializeFriendly(const std::string& type, const std::string& name, std::istream& stream, size_t size);
protected:
  void resolveRefs(Database*) override;
private:
  Material* material_;
  Mesh* mesh_;
  std::string materialName_;
  std::string meshName_;
};

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ENGINE_ASSET_MODEL
