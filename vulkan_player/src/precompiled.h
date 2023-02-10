#ifndef YAGA_VULKAN_PLAYER_SRC_PRECOMPILED
#define YAGA_VULKAN_PLAYER_SRC_PRECOMPILED

#include "utility/compiler.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <functional>
#include <iostream>
#include <iomanip>
#include <limits>
#include <memory>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

DISABLE_WARNINGS

#include <vk_mem_alloc.h>
#include <GLFW/glfw3.h>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/signals2.hpp>

#include "imgui_wrapper.h"
#include <imgui_freetype.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>

ENABLE_WARNINGS

#include "utility/glm.h"
#include "utility/exception.h"
#include "utility/log.h"

#endif // !YAGA_VULKAN_PLAYER_SRC_PRECOMPILED
