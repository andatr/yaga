#include "precompiled.h"
#include "gui.h"
#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Gui::Gui()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Gui::~Gui()
{
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::Render();
  ImDrawData* draw_data = ImGui::GetDrawData();


  ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);
}

} // !namespace vk
} // !namespace yaga