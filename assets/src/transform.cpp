#include "precompiled.h"
#include "assets/transform.h"
#include "assets/storage.h"
#include "binary_serializer_helper.h"
#include "binary_serializer_registry.h"
#include "friendly_serializer_helper.h"

namespace yaga {
namespace assets {
namespace {

constexpr const char* PARENT_PNAME      = "parent";
constexpr const char* TRANSLATION_PNAME = "translation";
constexpr const char* ROTATION_PNAME    = "rotation";
constexpr const char* SCALE_PNAME       = "scale";

} // !namespace

BINARY_SERIALIZER_REG(Transform)

// -----------------------------------------------------------------------------------------------------------------------------
Transform::Transform(const std::string& name) :
  Asset(name),
  translation_{},
  rotation_{},
  scale_(1.0f, 1.0f, 1.0f),
  parent_(nullptr)
{
  addProperty("Translation", &translation_);
  addProperty("Rotation",    &rotation_   );
  addProperty("Scale",       &scale_      );
  addProperty("Parent",      &parentName_ );
}

// -----------------------------------------------------------------------------------------------------------------------------
Transform::~Transform()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Transform* Transform::translation(const glm::vec3& value)
{
  translation_ = value;
  properties_[PropertyIndex::translation]->update(this);
  return this;
}

// -----------------------------------------------------------------------------------------------------------------------------
Transform* Transform::rotation(const glm::quat& value)
{
  rotation_ = value;
  properties_[PropertyIndex::rotation]->update(this);
  return this;
}

// -----------------------------------------------------------------------------------------------------------------------------
Transform* Transform::scale(const glm::vec3& value)
{
  scale_ = value;
  properties_[PropertyIndex::scale]->update(this);
  return this;
}

// -----------------------------------------------------------------------------------------------------------------------------
Transform* Transform::parent(TransformPtr value)
{
  parent_ = value;
  parentName_ = value ? value->name_ : "";
  properties_[PropertyIndex::parent]->update(this);
  return this;
}

// -----------------------------------------------------------------------------------------------------------------------------
TransformPtr Transform::deserializeBinary(std::istream& stream)
{
  std::string name;
  binser::read(stream, name);
  auto transform = std::make_unique<Transform>(name);
  binser::read(stream, transform->parentName_ );
  binser::read(stream, transform->translation_);
  binser::read(stream, transform->scale_      );
  binser::read(stream, transform->rotation_   );
  return transform;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Transform::serializeBinary(Asset* asset, std::ostream& stream)
{
  auto transform = assetCast<Transform>(asset);
  binser::write(stream, transform->name_       );
  binser::write(stream, transform->parentName_ ); 
  binser::write(stream, transform->translation_);
  binser::write(stream, transform->scale_      );
  binser::write(stream, transform->rotation_   );
}

// -----------------------------------------------------------------------------------------------------------------------------
TransformPtr Transform::deserializeFriendly(std::istream& stream)
{
  namespace pt = boost::property_tree;
  pt::ptree props;
  pt::read_json(stream, props);
  auto transform = frser::createAsset<Transform>(props);
  frser::read(props, PARENT_PNAME,      transform->parentName_ );
  frser::read(props, TRANSLATION_PNAME, transform->translation_);
  frser::read(props, ROTATION_PNAME,    transform->rotation_   );
  frser::read(props, SCALE_PNAME,       transform->scale_      );
  return transform;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Transform::serializeFriendly(Asset* asset, std::ostream& stream)
{
  namespace pt = boost::property_tree;
  auto transform = assetCast<Transform>(asset);
  pt::ptree props;
  frser::write(props, frser::NAME_PNAME, transform->name_       );
  frser::write(props, PARENT_PNAME,      transform->parentName_ );
  frser::write(props, TRANSLATION_PNAME, transform->translation_);
  frser::write(props, ROTATION_PNAME,    transform->rotation_   );
  frser::write(props, SCALE_PNAME,       transform->scale_      );
  pt::write_json(stream, props);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Transform::resolveRefs(Asset* asset, Storage* storage)
{
  auto transform = assetCast<Transform>(asset);
  if (!transform->parentName_.empty()) {
    transform->parent_ = storage->get<Transform>(transform->parentName_);
  }
}

} // !namespace assets
} // !namespace yaga
