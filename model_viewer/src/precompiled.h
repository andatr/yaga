#ifndef YAGA_MODEL_VIEWER_SRC_PRECOMPILED
#define YAGA_MODEL_VIEWER_SRC_PRECOMPILED

#include <array>
#include <chrono>
#include <iomanip>
#include <limits>
#include <sstream>

#pragma warning(push, 0)

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <boost/asio.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>

#pragma warning(pop)

#include "utility/exception.h"
#include "utility/log.h"

#endif // !YAGA_MODEL_VIEWER_SRC_PRECOMPILED
