#include "precompiled.h"
#include "friendly_serializer_helper.h"

namespace pt = boost::property_tree;

namespace yaga   {
namespace assets {
namespace frser  {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, int size>
void readArray(pt::ptree& ptree, const std::string& name, T& value)
{
  int i = 0;
  for (auto node : ptree.get_child(name)) {
    value[i++] = node.second.get_value<float>();
    if (i >= size) break;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, int size>
void writeArray(pt::ptree& ptree, const std::string& name, const T& value)
{
  pt::ptree arrayNode;
  for (int i = 0; i < size; ++i) {
    pt::ptree elementNode;
    elementNode.put("", value[i]);
    arrayNode.push_back(std::make_pair("", elementNode));
  }
  ptree.add_child(name, arrayNode);
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
void read(pt::ptree& ptree, const std::string& name, glm::vec3& value)
{
  readArray<glm::vec3, 3>(ptree, name, value);
}

// -----------------------------------------------------------------------------------------------------------------------------
void write(pt::ptree& ptree, const std::string& name, const glm::vec3& value)
{
  writeArray<glm::vec3, 3>(ptree, name, value);
}

// -----------------------------------------------------------------------------------------------------------------------------
void read(pt::ptree& ptree, const std::string& name, glm::quat& value)
{
  readArray<glm::quat, 4>(ptree, name, value);
}

// -----------------------------------------------------------------------------------------------------------------------------
void write(pt::ptree& ptree, const std::string& name, const glm::quat& value)
{
  writeArray<glm::quat, 4>(ptree, name, value);
}

} // !namespace frser
} // !namespace assets
} // !namespace yaga