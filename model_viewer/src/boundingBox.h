#ifndef YAGA_MODEL_VIEWER_SRC_FRAME
#define YAGA_MODEL_VIEWER_SRC_FRAME

#include "importer.h"
#include "assets/mesh.h"

namespace yaga {
namespace mview {

assets::MeshPtr makeBoundingBox(assets::Mesh* mesh, const MeshMetadata& meta);

} // !namespace mview
} // !namespace yaga

#endif // !YAGA_MODEL_VIEWER_SRC_FRAME
