#ifndef YAGA_ASSETS_FRIENDLY_SERIALIZER_HELPER
#define YAGA_ASSETS_FRIENDLY_SERIALIZER_HELPER

#include "utility/compiler.h"

#include <memory>

DISABLE_WARNINGS
#include <boost/property_tree/ptree.hpp>
ENABLE_WARNINGS

#include "utility/glm.h"

namespace yaga   {
namespace assets {
namespace frser  {

constexpr const char* NAME_PNAME = "name";

void write(boost::property_tree::ptree& ptree, const std::string& name, const glm::vec3&   value);
void read (boost::property_tree::ptree& ptree, const std::string& name,       glm::vec3&   value);
void read (boost::property_tree::ptree& ptree, const std::string& name,       glm::quat&   value);
void write(boost::property_tree::ptree& ptree, const std::string& name, const glm::quat&   value);
template<typename T>
void read(boost::property_tree::ptree& ptree, const std::string& name, T& value);
template<typename T>
void write(boost::property_tree::ptree& ptree, const std::string& name, const T& value);
template<typename T>
void read(boost::property_tree::ptree& ptree, const std::string& name, std::vector<T>& value);
template<typename T>
void write(boost::property_tree::ptree& ptree, const std::string& name, const std::vector<T>& values);
template<typename T>
std::shared_ptr<T> createAsset(boost::property_tree::ptree& ptree);

// -----------------------------------------------------------------------------------------------------------------------------
template<typename T>
void read(boost::property_tree::ptree& ptree, const std::string& name, T& value)
{
  value = ptree.get<T>(name, T());
}

// -----------------------------------------------------------------------------------------------------------------------------
template<typename T>
void write(boost::property_tree::ptree& ptree, const std::string& name, const T& value)
{
  ptree.put(name, value);
}

// -----------------------------------------------------------------------------------------------------------------------------
template<typename T>
void read(boost::property_tree::ptree& ptree, const std::string& name, std::vector<T>& value)
{
  auto children = ptree.get_child(name);
  int i = 0;
  value.resize(children.count(""));
  for (auto& node : children) {
    read(node.second, "", value[i++]);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
template<typename T>
void write(boost::property_tree::ptree& ptree, const std::string& name, const std::vector<T>& values)
{
  boost::property_tree::ptree arrayNode;
  for (int i = 0; i < values.size(); ++i) {
    write(arrayNode, "", values[i]);
  }
  ptree.add_child(name, arrayNode);
}

// -----------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::shared_ptr<T> createAsset(boost::property_tree::ptree& ptree)
{
  const auto name = ptree.get<std::string>("name", "");
  return std::make_shared<T>(name);
}

} // !frser
} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_FRIENDLY_SERIALIZER_HELPER