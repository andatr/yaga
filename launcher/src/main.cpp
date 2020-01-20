#include "precompiled.h"
#include "options.h"
#include "version.h"
#include "engine/asset/serializer.h"
#include "renderer/application.h"

namespace fs = boost::filesystem;

namespace yaga
{
namespace
{

// -----------------------------------------------------------------------------------------------------------------------------
void runApplication(fs::path assetPath)
{
  auto app = createApplication();
  if (fs::is_directory(assetPath)) {
    asset::Serializer::deserializeFriendly(assetPath.string(), app->assets());
  }
  else {
    const auto appFilename = assetPath.filename();
    if (appFilename.extension().string() == "data") {
      asset::Serializer::deserializeBin(assetPath.parent_path().string(), app->assets());
    }
    else {
      asset::Serializer::deserializeFriendly(assetPath.parent_path().string(), app->assets());
    }
  }
  app->run();
}

typedef std::vector<std::pair<std::string, std::string>> AppList;

// -----------------------------------------------------------------------------------------------------------------------------
AppList readAppList(const std::string& filename)
{
  static const std::string whitespace = " \t";
  std::vector<std::pair<std::string, std::string>> apps;
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
void displayAppList(const AppList& apps)
{
  while (true) {
    std::cout << "Enter index of application to run (1-" << apps.size() << "):\n";
    for (int i = 0; i < apps.size(); ++i) {
      std::cout << (i + 1) << ": " << apps[i].first << "\n";
    }
    int appIndex = -1;
    std::cin >> appIndex;
    if (appIndex <= 0 || appIndex > apps.size()) return;
    runApplication(apps[appIndex - 1].second);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void main(const Options& options)
{
  log::init(options.logSeverity(), log::format::Severity | log::format::Time);
  asset::Serializer::registerStandardAssets();
  auto appPath = fs::path(options.appPath());
  if (fs::is_regular(appPath) && appPath.filename().string() == "appList.txt") {
    auto apps = readAppList(appPath.string());
    if (apps.size() == 1) {
      runApplication(apps[0].second);
    }
    else if (apps.size() > 1) {
      displayAppList(apps);
    }
  }
  else {
    runApplication(appPath);
  }
}

} // !namespace
} // !namespace yaga

// -----------------------------------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
#ifdef NDEBUG
  bool error = false;
  try
#endif // !NDEBUG
  {
    yaga::Options options(argc, argv);
    yaga::main(options);
  }
#ifdef NDEBUG
  catch (const yaga::Exception& exp) {
    error = true;
    LOG_E(fatal, exp); 
  }
  catch (const std::exception& exp) {
    error = true;
    LOG(fatal, exp.what());
  }
  catch (...) {
    error = true;
    LOG(fatal, "Unknown exception");
  }
  if (error) {
    std::cout << "Press Enter to exit\n";
    std::cin.get();
  }
#endif // !NDEBUG
  return EXIT_SUCCESS;
}