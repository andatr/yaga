#include "precompiled.h"
#include "options.h"
#include "assets/application.h"
#include "assets/binary_serializer.h"
#include "assets/friendly_serializer.h"
#include "engine/application.h"
#include "engine/game.h"

namespace fs = boost::filesystem;

namespace yaga
{
namespace
{

struct AppDef
{
  std::string name;
  std::string path;
  AppDef() {}
  AppDef(const std::string& n, const std::string& p) : name(n), path(p) {}
};
typedef std::vector<AppDef> AppList;

constexpr size_t BAD_INDEX = std::numeric_limits<size_t>::max();

// -----------------------------------------------------------------------------------------------------------------------------
assets::SerializerPtr createSerializer(const fs::path& assetPath)
{
  if (fs::exists(assetPath)) {
    if (fs::is_directory(assetPath)) {
      return std::make_unique<assets::FriendlySerializer>(assetPath.string());
    }
    if (fs::is_regular(assetPath)) {
      return std::make_unique<assets::BinarySerializer>(assetPath.string());
    }
  }
  THROW("Bad asset path \"%1%\"", assetPath);
}

// -----------------------------------------------------------------------------------------------------------------------------
fs::path& makeAbsolutePath(fs::path& path, const fs::path& root)
{
  if (!path.is_absolute()) {
    path = root / path;
  }
  return path;
}

// -----------------------------------------------------------------------------------------------------------------------------
fs::path makeAbsolutePath(const std::string& path, const fs::path& root)
{
  auto fsPath = fs::path(path);
  makeAbsolutePath(fsPath, root);
  return fsPath;
}

// -----------------------------------------------------------------------------------------------------------------------------
void runApplication(fs::path assetPath, const Options& options)
{
  assetPath = makeAbsolutePath(assetPath, options.workingDir());
  auto storage = std::make_unique<assets::Storage>();
  auto serializer = createSerializer(assetPath);
  serializer->registerStandardAssets();
  auto appAsset = serializer->deserialize<assets::Application>("application", storage.get());
  //
  auto gameLibPath = makeAbsolutePath(appAsset->gameLibPath(), assetPath);
  auto createGameFunc = boost::dll::import_alias<CreateGameFunc>(gameLibPath, createGameFuncName,
    boost::dll::load_mode::append_decorations);
  auto game = createGameFunc(std::move(serializer), std::move(storage));
  //
  auto rendererLibPath = makeAbsolutePath(appAsset->rendererLibPath(), assetPath);
  auto createApplication = boost::dll::import_alias<CreateApplicationFunc>(rendererLibPath, createApplicationFuncName,
    boost::dll::load_mode::append_decorations);
  auto app = createApplication(std::move(game), appAsset);
  app->run();
}

// -----------------------------------------------------------------------------------------------------------------------------
AppList readAppList(const std::string& filename)
{
  static const std::string whitespace = " \t";
  AppList apps;
  std::ifstream file(filename, std::ios::in);
  std::string line;
  while (std::getline(file, line)) {
    auto pos = line.find_first_of(whitespace);
    if (pos != std::string::npos && line.size() > pos + 1) {
      apps.emplace_back(line.substr(0, pos), line.substr(pos + 1));
    }
  }
  return apps;
}

// -----------------------------------------------------------------------------------------------------------------------------
size_t displayAppList(const AppList& apps)
{
  std::cout << "Enter index of application to run (1-" << apps.size() << "):\n";
  for (size_t i = 0; i < apps.size(); ++i) {
    std::cout << (i + 1) << ": " << apps[i].name << "\n";
  }
  size_t appIndex = 0;
  std::cin >> appIndex;
  if (appIndex <= 0 || appIndex > apps.size()) return BAD_INDEX;
  return appIndex - 1;   
}

// -----------------------------------------------------------------------------------------------------------------------------
void main(const Options& options)
{
  log::init(options.logSeverity(), log::format::severity | log::format::time);
  auto appPath = fs::path(options.appPath());
  if (fs::is_regular(appPath) && appPath.filename().string() == "appList.txt") {
    auto apps = readAppList(appPath.string());
    if (apps.size() == 1) {
      runApplication(apps[0].path, options);
    }
    else if (apps.size() > 1) {
      auto appIndex = displayAppList(apps);
      if (appIndex == BAD_INDEX) return;
      runApplication(apps[appIndex].path, options);
    }
  }
  else {
    runApplication(appPath, options);
  }
}

} // !namespace
} // !namespace yaga

// -----------------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
#ifdef NDEBUG
  try
#endif // !NDEBUG
  {
    yaga::Options options(argc, argv);
    yaga::main(options);
  }
#ifdef NDEBUG
  catch (const yaga::Exception& exp) {
    LOG_E(fatal, exp); 
    return EXIT_FAILURE
  }
  catch (const std::exception& exp) {
    LOG(fatal) << exp.what();
    return EXIT_FAILURE
  }
  catch (...) {
    LOG(fatal) << "Unknown exception";
    return EXIT_FAILURE
  }
#endif // !NDEBUG
  return EXIT_SUCCESS;
}