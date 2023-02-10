#include "precompiled.h"
#include "gui/file_dialog.h"
#include "gui/widget.h"

#define ID(str) "##FileDialog_"##str
#define LABEL(str) str##"##FileDialog"

namespace fs = boost::filesystem;

namespace yaga {
namespace file_dialog {
namespace {

const ImVec2 defaultSize(500.0f, 300.0f);
const ImVec4 folderColor(1.0f, 0.8f, 0.6f, 1.0f);
const ImVec4 errorColor (1.0f, 0.0f, 0.0f, 1.0f);
constexpr float colWidth[] = { 230.0f, 80.0f, 80.0f };
const char* OPEN_ID = LABEL("Open File");
const char* SAVE_ID = LABEL("Save File");
constexpr size_t kb = 1024;
constexpr size_t mb = kb * 1024;
constexpr size_t gb = mb * 1024;
constexpr size_t tb = gb * 1024;

std::string defaultDir = "";
bool show = false;

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
  ImVec2 size;
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

// -----------------------------------------------------------------------------------------------------------------------------
class State
{
public:
  State();
  void changeDir(const std::string& path);
  void selectEntry(size_t index);
  void changeDrive(size_t index);
  void sort();
  void sort(Column column, SortOrder order);
  void openFile();
  void saveFile();
  void getDrives();

protected:
  Column sortColumn_;
  SortOrder sortOrder_;
  std::string directory_;
  std::vector<Entry> entries_;
  size_t currentEntry_;
  std::vector<std::string> drives_;
  size_t currentDrive_;
  std::vector<char> filename_;
  bool error_;
  bool completed_;
};

typedef std::shared_ptr<State> StatePtr;
typedef std::function<void(StatePtr)> Action;

// -----------------------------------------------------------------------------------------------------------------------------
class FileDialog;
typedef std::shared_ptr<FileDialog> FileDialogPtr;

class FileDialog :
  public State,
  public Widget
{
public:
  FileDialog(GuiContext context, Mode mode, Handle handle);
  ~FileDialog();
  void init(const std::string& path);
  void render() override;

private:
  void renderFileName();
  void renderDrives();
  void renderTable();
  void renderTableRow(size_t index);
  void renderColumn(Column column, const char* label);
  void post(Action action);
  void enqueueAction();
  void sort(Column column, SortOrder order);
  void selectEntry(size_t index);
  void selectDrive(size_t index);
  void process();

private:
  Mode mode_;
  Layout layout_;
  Handle handle_;
  StatePtr backState_;
  std::queue<Action> actions_;
};

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
Layout::Layout() :
  init(false),
  error(false),
  size{},
  comboSize{},
  buttonSize{}
{
}

// -----------------------------------------------------------------------------------------------------------------------------
State::State() :
  sortColumn_(Column::Name),
  sortOrder_(SortOrder::Up),
  currentEntry_(0),
  currentDrive_(0),
  error_(false),
  completed_(false)
{
  filename_.resize(512);
}

// -----------------------------------------------------------------------------------------------------------------------------
void State::changeDir(const std::string& path)
{
  fs::path dir = path;
  if (!fs::is_directory(dir)) {
    memset(filename_.data(), 0, filename_.size());
    memcpy(filename_.data(), path.c_str(), std::min(filename_.size(), path.size()));
    dir = dir.parent_path();
  }
  error_ = false;
  directory_ = path;
  entries_.clear();
  entries_.push_back(getDoubleDotEntry(dir));
  try {
    for (auto& p : fs::directory_iterator(dir)) {
      try {
        entries_.push_back(getEntry(fs::is_directory(p), p));
      }
      catch (...) {}
    }
  }
  catch (...) {}
  if (!drives_.empty() && !dir.empty()) {
    auto it = std::find(drives_.begin(), drives_.end(), dir.root_path().string());
    if (it != drives_.end()) {
      currentDrive_ = it - drives_.begin();
    }
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void State::selectEntry(size_t index)
{
  if (index >= entries_.size()) return;
  const auto& entry = entries_[index];
  if (entry.dir) {
    changeDir(entry.path);
    sort();
  }
  else {
    memset(filename_.data(), 0, filename_.size());
    memcpy(filename_.data(), entry.path.c_str(), std::min(filename_.size(), entry.path.size()));
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void State::changeDrive(size_t index) 
{
  if (index >= drives_.size()) return;
  currentDrive_ = index;
  changeDir(drives_[index]);
  sort();
}

// -----------------------------------------------------------------------------------------------------------------------------
void State::sort()
{
  switch (sortColumn_) {
  case Column::Name:
    std::sort(++entries_.begin(), entries_.end(), [order = sortOrder_](const Entry& a, const Entry& b) {
      if (a.dir && !b.dir) return true;
      if (!a.dir && b.dir) return false;
      return order == SortOrder::Down
        ? a.name > b.name
        : a.name < b.name;
    });
    break;
  case Column::Type:
    std::sort(++entries_.begin(), entries_.end(), [order = sortOrder_](const Entry& a, const Entry& b) {
      if (a.dir && !b.dir) return true;
      if (!a.dir && b.dir) return false;
      return order == SortOrder::Down
        ? a.type > b.type
        : a.type < b.type;
    });
    break;
  case Column::Size:
    std::sort(++entries_.begin(), entries_.end(), [order = sortOrder_](const Entry& a, const Entry& b) {
      if (a.dir && !b.dir) return true;
      if (!a.dir && b.dir) return false;
      return order == SortOrder::Down
        ? a.size > b.size
        : a.size < b.size;
    });
    break;
  case Column::Time:
    std::sort(++entries_.begin(), entries_.end(), [order = sortOrder_](const Entry& a, const Entry& b) {
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
void State::sort(Column column, SortOrder order)
{
  this->sortColumn_ = column;
  this->sortOrder_ = order;
  sort();
}

// -----------------------------------------------------------------------------------------------------------------------------
void State::openFile()
{
  error_ = false;
  try {
    fs::path path = filename_.data();
    if (!fs::exists(path) || !path.is_absolute()) {
      error_ = true;
    }
    else if (fs::is_directory(path)) {
      changeDir(path.string());
      sort();
    }
    else {
      completed_ = true;
    }
  }
  catch (...) {
    error_ = true;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void State::saveFile()
{
  error_ = false;
  try {
    fs::path path = filename_.data();
    if (!path.is_absolute()) {
      error_ = true;
    }
    else if (fs::is_directory(path)) {
      changeDir(path.string());
      sort();
    }
    else {
      completed_ = true;
    }
  }
  catch (...) {
    error_ = true;
  }
}

#ifdef _WIN32
#ifndef _INC_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // !_INC_WINDOWS

// -----------------------------------------------------------------------------------------------------------------------------
void State::getDrives()
{
  auto mask = GetLogicalDrives();
  for (int i = 0; i < 26; ++i) {
    if (!(mask & (1 << i))) continue;
    char drive[4] = { static_cast<char>('A' + i), ':', '\\', '\0' };
    auto type = GetDriveTypeA(drive);
    if (type == DRIVE_REMOVABLE || type == DRIVE_FIXED) {
      drives_.push_back(drive);
    } 
  }
}
#else // _WIN32

// -----------------------------------------------------------------------------------------------------------------------------
void State::getDrives()
{
}

#endif // !_WIN32

// -----------------------------------------------------------------------------------------------------------------------------
FileDialog::FileDialog(GuiContext context, Mode mode, Handle handle) :
  Widget(context),
  mode_(mode),
  handle_(handle)
{
  show = true;
  backState_ = std::make_shared<State>(*this);
}

// -----------------------------------------------------------------------------------------------------------------------------
FileDialog::~FileDialog()
{
  show = false;
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileDialog::init(const std::string& path)
{
  post([path](StatePtr state) {
    state->getDrives();
    state->changeDir(path);
    state->sort();
  });
  enqueueRendering();
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileDialog::render()
{
  if (completed_) {
    context_.postSeq(std::bind(handle_, std::string(filename_.begin(), filename_.end())));
    return;
  }
  ImGui::SetNextWindowSize(defaultSize, ImGuiCond_FirstUseEver);
  setPosition();
  const char* title = mode_ == Mode::Open ? OPEN_ID : SAVE_ID;
  bool open = true;
  if (ImGui::Begin(title, &open, ImGuiWindowFlags_NoCollapse)) {
    renderTable();
    renderDrives();
    renderFileName();
  }
  updatePosition();
  ImGui::End();
  if (open) {
    enqueueRendering();
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileDialog::renderColumn(Column column, const char* label)
{
  ImGui::AlignTextToFramePadding();
  if (ImGui::Selectable(label)) {
    sort(column, column == sortColumn_
      ? sortOrder_ == SortOrder::Down ? SortOrder::Up : SortOrder::Down
      : SortOrder::Up);
  }
  if (sortColumn_ == column) {
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::ArrowButton("", sortOrder_ == SortOrder::Up ? ImGuiDir_Up : ImGuiDir_Down);
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
  }
  ImGui::NextColumn();
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileDialog::renderTableRow(size_t index)
{
  const auto& style = ImGui::GetStyle();
  auto& entry = entries_[index];
  if (entry.dir) {
    ImGui::PushStyleColor(ImGuiCol_Text, folderColor);
  }
  // column name
  if (ImGui::Selectable(entry.name.c_str(),
    index == currentEntry_,
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
void FileDialog::renderTable()
{
  const auto& style = ImGui::GetStyle();
  const auto size = ImGui::GetContentRegionAvail();
  const auto tableSize = size - ImVec2(0.0f, style.ItemSpacing.y + layout_.buttonSize.y);
  if (ImGui::BeginChild(ID("FileList"), tableSize, true, ImGuiWindowFlags_HorizontalScrollbar)) {
    ImGui::Text("%s", directory_.c_str());
    ImGui::Separator();
    ImGui::Columns(4, ID("Columns"));
    if (!layout_.init) {
      ImGui::SetColumnWidth(0, colWidth[0]);
      ImGui::SetColumnWidth(1, colWidth[1]);
      ImGui::SetColumnWidth(2, colWidth[2]);
      layout_.init = true;
    }
    renderColumn(Column::Name, LABEL("Name"));
    renderColumn(Column::Type, LABEL("Type"));
    renderColumn(Column::Size, LABEL("Size"));
    renderColumn(Column::Time, LABEL("Time"));
    ImGui::Separator();
    for (size_t i = 0; i < entries_.size(); ++i) {
      renderTableRow(i);
    }
  }
  ImGui::EndChild();
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileDialog::renderDrives()
{
  if (drives_.empty()) return;
  const auto& style = ImGui::GetStyle();
  ImGui::PushItemWidth(style.FramePadding.x * 2.0f + ImGui::GetFontSize() * 3.0f);
  const auto& drive = drives_[currentDrive_];
  if (ImGui::BeginCombo(ID("Drive"), drive.c_str())) {
    for (size_t i = 0; i < drives_.size(); ++i) {
      if (ImGui::Selectable(drives_[i].c_str(), currentDrive_ == i)) {
        selectDrive(i);
      }
    }
    ImGui::EndCombo();
  }
  layout_.comboSize = ImGui::GetItemRectSize();
  ImGui::PopItemWidth();
  ImGui::SameLine();
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileDialog::renderFileName()
{
  const auto& style = ImGui::GetStyle();
  const auto driveSize = drives_.empty() ? 0.0f : layout_.comboSize.x + style.ItemSpacing.x;
  ImGui::PushItemWidth(size_.x - layout_.buttonSize.x - driveSize - style.ItemSpacing.x - layout_.buttonSize.y);
  if (layout_.error) {
    ImGui::PushStyleColor(ImGuiCol_Text, errorColor);
  }
  if (ImGui::InputText(ID("FileName"), filename_.data(), filename_.size(), ImGuiInputTextFlags_EnterReturnsTrue)) {
    process();
  }
  if (layout_.error) {
    ImGui::PopStyleColor();
  }
  ImGui::PopItemWidth();
  ImGui::SameLine();
  const char* label = mode_ == Mode::Open ? LABEL("Open") : LABEL("Save");
  if (ImGui::Button(label)) {
    process();
  }
  layout_.buttonSize = ImGui::GetItemRectSize();
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileDialog::post(Action action)
{
  actions_.push(action);
  if (actions_.size() == 1) {
    enqueueAction();
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileDialog::enqueueAction()
{
  auto action = actions_.front();
  actions_.pop();
  *backState_ = *static_cast<State*>(this);
  context_.postCon(std::bind(action, backState_), [w = shared_from_this()]() {
    auto fd = reinterpret_cast<FileDialog*>(w.get());
    if (fd->actions_.empty()) {
      State* state = fd;
      *state = *fd->backState_;
    }
    else {
      fd->enqueueAction();
    }
  });
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileDialog::sort(Column column, SortOrder order)
{
  post([column, order](StatePtr state) {
    state->sort(column, order);
  });
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileDialog::selectEntry(size_t index)
{
  post([index](StatePtr state) {
    state->selectEntry(index);
  });
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileDialog::selectDrive(size_t index)
{
  post([index](StatePtr state) {
    state->changeDrive(index);
  });
}

// -----------------------------------------------------------------------------------------------------------------------------
void FileDialog::process()
{
  if (mode_ == Mode::Open) {
    post([](StatePtr state) {
      state->openFile();
    });
  }
  else if (mode_ == Mode::Save) {
    post([](StatePtr state) {
      state->saveFile();
    });
  }
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
void render(GuiContext context, Mode mode, const std::string& path, Handle handle)
{
  if (show) return;
  auto dialog = std::make_shared<FileDialog>(context, mode, handle);
  dialog->init(path);
}

// -----------------------------------------------------------------------------------------------------------------------------
void render(GuiContext context, Mode mode, Handle handle)
{
  if (show) return;
  auto dialog = std::make_shared<FileDialog>(context, mode, handle);
  dialog->init(defaultDir);
}

// -----------------------------------------------------------------------------------------------------------------------------
void setDefaultDir(const std::string& path)
{
  defaultDir = path;
}

} // !namespace file_dialog
} // !namespace yaga
