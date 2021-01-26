#include "precompiled.h"
#include "gui/file_dialog.h"

#define ID(str) "##FileDialog_"##str
#define LABEL(str) str##"##FileDialog"

namespace fs = boost::filesystem;

namespace yaga {
namespace {

const ImVec2 windowSizeDefault = { 540.0f, 400.0f };
const ImVec4 folderColor(1.0f, 0.8f, 0.6f, 1.0f);
const ImVec4 errorColor(1.0f, 0.0f, 0.0f, 1.0f);
constexpr float colWidth[] = { 230.0f, 80.0f, 80.0f };
const char* FILE_DIALOG_OPEN_ID = LABEL("Open File");
const char* FILE_DIALOG_SAVE_ID = LABEL("Save File");
constexpr size_t BAD_INDEX = std::numeric_limits<size_t>::max();
constexpr size_t kb = 1024;
constexpr size_t mb = kb * 1024;
constexpr size_t gb = mb * 1024;
constexpr size_t tb = gb * 1024;

enum class SortOrder
{
  None,
  Up,
  Down
};

enum class Column
{
  None,
  Name,
  Type,
  Size,
  Time
};

struct Layout
{
  bool init;
  bool error;
  ImVec2 windowSize;
  ImVec2 comboSize;
  ImVec2 buttonSize;

  Layout();
};

struct Entry
{
  bool dir;
  std::string name;
  std::string path;
  std::string type;
  std::string sizeStr;
  std::string timeStr;
  size_t size;
  time_t time;
  float width;
};

struct State
{
  size_t version;
  Column sortColumn;
  SortOrder sortOrder;
  std::string directory;
  std::vector<Entry> entries;
  size_t currentEntry;
  std::vector<std::string> drives;
  size_t currentDrive;
  std::vector<char> filename;
  bool completed;
  Layout layout;

  State();
};

typedef std::shared_ptr<State> StatePtr;

// -----------------------------------------------------------------------------------------------------------------------------
void getFileTime(const fs::path& path, Entry& entry)
{
  entry.time = fs::last_write_time(path);
  std::tm* mt = std::localtime(&entry.time);
  std::stringstream ss;
  ss << std::put_time(mt, "%F %R");
  entry.timeStr = ss.str();
}

// -----------------------------------------------------------------------------------------------------------------------------
void getFileSize(const fs::path& path, Entry& entry)
{
  char buffer[64]{};
  entry.size = fs::file_size(path);
  if (entry.size >= tb) {
    sprintf(buffer, "%zu TB", entry.size / tb);
  }
  else if (entry.size >= gb) {
    sprintf(buffer, "%zu GB", entry.size / gb);
  }
  else if (entry.size >= mb) {
    sprintf(buffer, "%zu MB", entry.size / mb);
  }
  else if (entry.size >= kb) {
    sprintf(buffer, "%zu KB", entry.size / kb);
  }
  else {
    sprintf(buffer, "%zu  B", entry.size);
  }
  entry.sizeStr = buffer;
}

// -----------------------------------------------------------------------------------------------------------------------------
Entry getDoubleDotEntry(const fs::path& path)
{
  Entry entry{};
  entry.width = colWidth[1];
  entry.dir = true;
  entry.name = "..";
  const auto parent = path.parent_path();
  if (parent.empty() || !parent.is_absolute())
    entry.path = path.string();
  else
    entry.path = parent.string();
  return entry;
}

// -----------------------------------------------------------------------------------------------------------------------------
Entry getEntry(bool dir, const fs::path& path)
{
  Entry entry{};
  entry.width = colWidth[1];
  entry.dir = dir;
  entry.name = path.filename().string();
  entry.path = path.string();
  if (!dir) {
    getFileSize(path, entry);
    entry.type = path.extension().string();
  }
  getFileTime(path, entry);
  return entry;
}

// -----------------------------------------------------------------------------------------------------------------------------
void changeDir(StatePtr state, const std::string& path)
{
  fs::path dir = path;
  if (!fs::is_directory(dir)) {
    memset(state->filename.data(), 0, state->filename.size());
    memcpy(state->filename.data(), path.c_str(), std::min(state->filename.size(), path.size()));
    dir = dir.parent_path();
  }
  state->layout.error = false;
  state->directory = path;
  state->entries.clear();
  state->entries.push_back(getDoubleDotEntry(dir));
  try {
    for (auto& p : fs::directory_iterator(dir)) {
      try {
        state->entries.push_back(getEntry(fs::is_directory(p), p));
      }
      catch (...) {}
    }
  }
  catch (...) {}
  auto& drives = state->drives;
  if (!drives.empty() && !dir.empty()) {
    auto it = std::find(drives.begin(), drives.end(), dir.root_path().string());
    if (it != drives.end()) {
      state->currentDrive = it - drives.begin();
    }
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void sort(StatePtr state)
{
  auto& entries = state->entries;
  const auto column = state->sortColumn;
  const auto order = state->sortOrder;
  switch (column) {
  case Column::Name:
    std::sort(++entries.begin(), entries.end(), [order](const Entry& a, const Entry& b) {
      if (a.dir && !b.dir) return true;
      if (!a.dir && b.dir) return false;
      return order == SortOrder::Down
        ? a.name > b.name
        : a.name < b.name;
    });
    break;
  case Column::Type:
    std::sort(++entries.begin(), entries.end(), [order](const Entry& a, const Entry& b) {
      if (a.dir && !b.dir) return true;
      if (!a.dir && b.dir) return false;
      return order == SortOrder::Down
        ? a.type > b.type
        : a.type < b.type;
    });
    break;
  case Column::Size:
    std::sort(++entries.begin(), entries.end(), [order](const Entry& a, const Entry& b) {
      if (a.dir && !b.dir) return true;
      if (!a.dir && b.dir) return false;
      return order == SortOrder::Down
        ? a.size > b.size
        : a.size < b.size;
    });
    break;
  case Column::Time:
    std::sort(++entries.begin(), entries.end(), [order](const Entry& a, const Entry& b) {
      if (a.dir && !b.dir) return true;
      if (!a.dir && b.dir) return false;
      return order == SortOrder::Down
        ? a.time > b.time
        : a.time < b.time;
    });
    break;
  default:
    break;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void openFile(StatePtr state)
{
  state->layout.error = false;
  try {
    fs::path path = state->filename.data();
    if (!fs::exists(path) || !path.is_absolute()) {
      state->layout.error = true;
    }
    else if (fs::is_directory(path)) {
      changeDir(state, path.string());
      sort(state);
    }
    else {
      state->completed = true;
    }
  }
  catch (...) {
    state->layout.error = true;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void saveFile(StatePtr state)
{
  state->layout.error = false;
  try {
    fs::path path = state->filename.data();
    if (!path.is_absolute()) {
      state->layout.error = true;
    }
    else if (fs::is_directory(path)) {
      changeDir(state, path.string());
      sort(state);
    }
    else {
      state->completed = true;
    }
  }
  catch (...) {
    state->layout.error = true;
  }
}

#ifdef _WIN32
#ifndef _INC_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // !_INC_WINDOWS

// -----------------------------------------------------------------------------------------------------------------------------
void getDrives(StatePtr state)
{
  auto mask = GetLogicalDrives();
  for (int i = 0; i < 26; ++i) {
    if (!(mask & (1 << i))) continue;
    char drive[4] = { static_cast<char>('A' + i), ':', '\\', '\0' };
    auto type = GetDriveTypeA(drive);
    if (type == DRIVE_REMOVABLE || type == DRIVE_FIXED) {
      state->drives.push_back(drive);
    } 
  }
}
#else // _WIN32

// -----------------------------------------------------------------------------------------------------------------------------
void getDrives(StatePtr)
{
}

#endif // !_WIN32

// -----------------------------------------------------------------------------------------------------------------------------
Layout::Layout() :
  init(false),
  error(false),
  windowSize{},
  comboSize{},
  buttonSize{}
{
}

// -----------------------------------------------------------------------------------------------------------------------------
State::State() :
  version(0),
  sortColumn(Column::Name),
  sortOrder(SortOrder::Up),
  currentEntry(0),
  currentDrive(0),
  completed(false)
{
  filename.resize(512);
}

} // !namespace

class FileDialogImpl
{
public:
  FileDialogImpl(GuiContext context, FileDialogMode mode, const std::string& path, FileDialogHandle handle);
  void operator()();

private:
  void renderFileName();
  void renderDrives();
  void renderTable();
  void renderTableRow(size_t index);
  void renderColumn(Column column, const char* label);
  template<typename Action>
  void post(Action action);
  void sort(Column column, SortOrder order);
  void selectEntry(size_t index);
  void selectDrive(size_t index);
  void process();

private:
  GuiContext context_;
  FileDialogMode mode_;
  StatePtr state_;
  FileDialogHandle handle_;
};

// -----------------------------------------------------------------------------------------------------------------------------
void renderFileDialog(GuiContext context, FileDialogMode mode, const std::string& path, FileDialogHandle handle)
{
  context.render(FileDialogImpl(context, mode, path, handle));
}

// -----------------------------------------------------------------------------------------------------------------------------
FileDialogImpl::FileDialogImpl(GuiContext context, FileDialogMode mode, const std::string& path, FileDialogHandle handle) :
  context_(context),
  mode_(mode),
  handle_(handle)
{
  state_ = std::make_shared<State>();
  post([path](StatePtr newState) {
    changeDir(newState, path);
    yaga::sort(newState);
    getDrives(newState);
  });
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileDialogImpl::operator()()
{
  if (state_->completed) {
    context_.render(std::bind(handle_, std::string(state_->filename.begin(), state_->filename.end())));
    return;
  }
  ImGui::SetNextWindowPos((context_.screen() - state_->layout.windowSize) / 2.0f);
  ImGui::SetNextWindowSize(windowSizeDefault , ImGuiCond_FirstUseEver);
  bool open = true;
  const char* title = mode_ == FileDialogMode::Open ? FILE_DIALOG_OPEN_ID : FILE_DIALOG_SAVE_ID;
  if (ImGui::Begin(title, &open, ImGuiWindowFlags_NoCollapse)) {
    state_->layout.windowSize = ImGui::GetContentRegionAvail();
    renderTable();
    renderDrives();
    renderFileName();
  }
  ImGui::End();
  if (open)
    context_.render(*this);
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileDialogImpl::renderColumn(Column column, const char* label)
{
  ImGui::AlignTextToFramePadding();
  if (ImGui::Selectable(label)) {
    sort(column, column == state_->sortColumn
      ? state_->sortOrder == SortOrder::Down ? SortOrder::Up : SortOrder::Down
      : SortOrder::Up);
  }
  if (state_->sortColumn == column) {
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::ArrowButton("", state_->sortOrder == SortOrder::Up ? ImGuiDir_Up : ImGuiDir_Down);
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
  }
  ImGui::NextColumn();
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileDialogImpl::renderTableRow(size_t index)
{
  const auto& style = ImGui::GetStyle();
  auto& entry = state_->entries[index];
  if (entry.dir) {
    ImGui::PushStyleColor(ImGuiCol_Text, folderColor);
  }
  // column name
  if (ImGui::Selectable(entry.name.c_str(),
    index == state_->currentEntry,
    ImGuiSelectableFlags_AllowDoubleClick,
    ImVec2(ImGui::GetWindowContentRegionWidth(), 0)))
  {
    selectEntry(index);
  }
  ImGui::NextColumn();
  // column type
  ImGui::TextUnformatted(entry.type.c_str());
  ImGui::NextColumn();
  // column size: align right
  {
    auto width = ImGui::GetColumnWidth();
    auto pos = ImGui::GetCursorPosX();
    ImGui::SetCursorPosX(pos + width - entry.width);
    ImGui::TextUnformatted(entry.sizeStr.c_str());
    entry.width = ImGui::GetItemRectSize().x + 2.0f * style.ItemSpacing.x;
    ImGui::NextColumn();
  }
  // column time
  ImGui::TextUnformatted(entry.timeStr.c_str());
  ImGui::NextColumn();
  if (entry.dir) {
    ImGui::PopStyleColor();
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileDialogImpl::renderTable()
{
  const auto& style = ImGui::GetStyle();
  const auto tableSize = state_->layout.windowSize - ImVec2(0.0f, style.ItemSpacing.y + state_->layout.buttonSize.y);
  if (ImGui::BeginChild(ID("FileList"), tableSize, true, ImGuiWindowFlags_HorizontalScrollbar)) {
    ImGui::Text("%s", state_->directory.c_str());
    ImGui::Separator();
    ImGui::Columns(4, ID("Columns"));
    if (!state_->layout.init) {
      ImGui::SetColumnWidth(0, colWidth[0]);
      ImGui::SetColumnWidth(1, colWidth[1]);
      ImGui::SetColumnWidth(2, colWidth[2]);
      state_->layout.init = true;
    }
    renderColumn(Column::Name, LABEL("Name"));
    renderColumn(Column::Type, LABEL("Type"));
    renderColumn(Column::Size, LABEL("Size"));
    renderColumn(Column::Time, LABEL("Time"));
    ImGui::Separator();
    for (size_t i = 0; i < state_->entries.size(); ++i) {
      renderTableRow(i);
    }
  }
  ImGui::EndChild();
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileDialogImpl::renderDrives()
{
  if (state_->drives.empty()) return;
  const auto& style = ImGui::GetStyle();
  ImGui::PushItemWidth(style.FramePadding.x * 2.0f + ImGui::GetFontSize() * 3.0f);
  const auto& drive = state_->drives[state_->currentDrive];
  if (ImGui::BeginCombo(ID("Drive"), drive.c_str())) {
    for (size_t i = 0; i < state_->drives.size(); ++i) {
      if (ImGui::Selectable(state_->drives[i].c_str(), state_->currentDrive == i)) {
        selectDrive(i);
      }
    }
    ImGui::EndCombo();
  }
  state_->layout.comboSize = ImGui::GetItemRectSize();
  ImGui::PopItemWidth();
  ImGui::SameLine();
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileDialogImpl::renderFileName()
{
  const auto& style = ImGui::GetStyle();
  const auto driveSize = state_->drives.empty() ? 0.0f : state_->layout.comboSize.x + style.ItemSpacing.x;
  ImGui::PushItemWidth(state_->layout.windowSize.x - state_->layout.buttonSize.x -
    driveSize - style.ItemSpacing.x - state_->layout.buttonSize.y);
  if (state_->layout.error) {
    ImGui::PushStyleColor(ImGuiCol_Text, errorColor);
  }
  if (ImGui::InputText(ID("FileName"), state_->filename.data(), state_->filename.size(), 
    ImGuiInputTextFlags_EnterReturnsTrue))
  {
    process();
  }
  if (state_->layout.error) {
    ImGui::PopStyleColor();
  }
  ImGui::PopItemWidth();
  ImGui::SameLine();
  const char* label = mode_ == FileDialogMode::Open ? LABEL("Open") : LABEL("Save");
  if (ImGui::Button(label)) {
    process();
  }
  state_->layout.buttonSize = ImGui::GetItemRectSize();
}

// -----------------------------------------------------------------------------------------------------------------------------
template<typename Action>
void FileDialogImpl::post(Action action)
{
  auto newState = std::make_shared<State>(*state_);
  context_.post(std::bind(action, newState), [newState, state = state_]() {
    if (newState->version < state->version) return;
    ++newState->version;
    *state = *newState;
  });
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileDialogImpl::sort(Column column, SortOrder order)
{
  post([column, order](StatePtr newState) {
    newState->sortColumn = column;
    newState->sortOrder = order;
    yaga::sort(newState);
  });
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileDialogImpl::selectEntry(size_t index)
{
  post([index](StatePtr newState) {
    if (index >= newState->entries.size()) return;
    const auto& entry = newState->entries[index];
    if (entry.dir) {
      changeDir(newState, entry.path);
      yaga::sort(newState);
    }
    else {
      memset(newState->filename.data(), 0, newState->filename.size());
      memcpy(newState->filename.data(), entry.path.c_str(), std::min(newState->filename.size(), entry.path.size()));
    }
  });
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileDialogImpl::selectDrive(size_t index)
{
  post([index](StatePtr newState) {
    if (index >= newState->drives.size()) return;
    newState->currentDrive = index;
    changeDir(newState, newState->drives[index]);
    yaga::sort(newState);
  });
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileDialogImpl::process()
{
  if (mode_ == FileDialogMode::Open) {
    post(openFile);
  }
  else if (mode_ == FileDialogMode::Save) {
    post(saveFile);
  }
}

} // !namespace yaga
