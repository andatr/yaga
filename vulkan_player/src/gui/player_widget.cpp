#include "precompiled.h"
#include "player_widget.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
  PlayerWidget::PlayerWidget(GuiContext context, Player* player) :
  Widget(context),
  player_(player)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void PlayerWidget::render()
{
  /*const auto& style = ImGui::GetStyle();
  const auto& screen = context_.screen();

  setPosition();
  bool open = true;
  if (player_ && ImGui::Begin("##PlayerControls", &open, ImGuiWindowFlags_NoCollapse)) {
   
    const ImVec2 p = ImGui::GetCursorScreenPos();
    const auto start1 = ImVec2(p.x, p.y);

    auto t = player_->timestamp() * 1000000;
    auto d = player_->duration();

    float progress = (screen.x - 2.0f * start1.x) * t / d;
    const auto end1  = ImVec2(start1.x + progress, start1.y + 14.0f);

    const auto start2 = ImVec2(start1.x + progress, p.y);
    const auto end2  = ImVec2(screen.x - start1.x, start1.y + 14.0f);

    ImDrawList* drawList = ImGui::GetWindowDrawList();




    
    float x = p.x + 4.0f;
    float y = p.y + 4.0f;
    static float sz = 36.0f;
    static float thickness = 3.0f;
    static ImVec4 colf = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    const ImU32 col = ImColor(colf);
    drawList->AddRectFilled(start1, end1, col); 

    static ImVec4 colf2 = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    const ImU32 col2 = ImColor(colf2);
    drawList->AddRectFilled(start2, end2, col2); 


    //drawList->AddRectFilled(start, size, col); 
    ImGui::SetCursorScreenPos(ImVec2(8.0f, 300.0f));
    ImGui::Text("qweqweqwe");

    // Horizontal line (faster than AddLine, but only handle integer thickness)
    updatePosition();
    ImGui::End();
  }
  if (open) enqueueRendering();*/
}

} // !namespace yaga
