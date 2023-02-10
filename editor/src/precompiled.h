#ifndef YAGA_EDITOR_SRC_PRECOMPILED
#define YAGA_EDITOR_SRC_PRECOMPILED

#include "utility/compiler.h"

#include <array>
#include <chrono>
#include <iomanip>
#include <limits>
#include <set>
#include <sstream>

DISABLE_WARNINGS

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <boost/asio.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>

#include "imgui_wrapper.h"

ENABLE_WARNINGS

#include "utility/exception.h"
#include "utility/glm.h"
#include "utility/log.h"

#endif // !YAGA_EDITOR_SRC_PRECOMPILED
