#include "precompiled.h"
#include "file_info_widget.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
FileInfoWidget::FileInfoWidget(GuiContext context, Player* player) :
  Widget(context),
  player_(player)
{
  StreamInfo info;
  info.title = "General";
  for (const auto& item : player_->description()) {
    info.description.emplace_back(item.first, item.second);
  }
  streams_.push_back(std::move(info));
  buildStreamInfo(player_->videoStreams(),    "Video "    );
  buildStreamInfo(player_->audioStreams(),    "Audio "    );
  buildStreamInfo(player_->subtitleStreams(), "Subtitles ");
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileInfoWidget::render()
{
  setPosition();
  bool open = true;
  if (ImGui::Begin("File Info##FileInfoDialog", &open, ImGuiWindowFlags_NoCollapse)) {
    if (ImGui::BeginTabBar("TabBar##FileInfoDialog", ImGuiTabBarFlags_None)) {
      for (const auto& stream : streams_) {
        if (ImGui::BeginTabItem(stream.title.c_str())) {
          for (const auto& desc : stream.description) {
            ImGui::Text(desc.first.c_str());
            ImGui::SameLine(150.0f);
            ImGui::Text(desc.second.c_str());
          }
          ImGui::EndTabItem();
        }
      }
      ImGui::EndTabBar();
    }
  }
  updatePosition();
  ImGui::End();
  if (open) enqueueRendering();
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileInfoWidget::buildStreamInfo(StreamFamily* family, const char* name)
{
  for (int i = 0; i < family->count(); ++i) {
    StreamInfo info;
    info.title = name + std::to_string(i + 1);
    for (const auto& item : family->stream(i)->description()) {
      info.description.emplace_back(item.first, item.second);
    }
    streams_.push_back(std::move(info));
  }
}

} // !namespace yaga
