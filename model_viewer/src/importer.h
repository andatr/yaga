#ifndef YAGA_MODEL_VIEWER_SRC_IMPORTER
#define YAGA_MODEL_VIEWER_SRC_IMPORTER

#include "assets/mesh.h"

namespace yaga {
namespace mview {

struct MeshMetadata
{
  glm::vec3 min;
  glm::vec3 max;
};

assets::MeshPtr importMesh(const std::string& path, MeshMetadata& meta);

} // !namespace mview
} // !namespace yaga

#endif // !YAGA_MODEL_VIEWER_SRC_IMPORTER
