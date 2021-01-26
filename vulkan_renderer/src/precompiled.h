#ifndef YAGA_VULKAN_RENDERER_SRC_PRECOMPILED
#define YAGA_VULKAN_RENDERER_SRC_PRECOMPILED

#include <algorithm>
#include <array>
#include <chrono>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

#pragma warning(push, 0)

#include <vk_mem_alloc.h>
#include <GLFW/glfw3.h>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/signals2.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>

#pragma warning(pop)

#include "utility/exception.h"
#include "utility/log.h"

#endif // !YAGA_VULKAN_RENDERER_SRC_PRECOMPILED
