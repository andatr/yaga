#ifndef YAGA_EDITOR_SRC_EDITOR_GUI_ASSET_MANAGER_GUI
#define YAGA_EDITOR_SRC_EDITOR_GUI_ASSET_MANAGER_GUI

#include <map>
#include <memory>
#include <string>

#include <boost/iterator/iterator_facade.hpp>

#include "assets/asset.h"
#include "utility/compiler.h"
#include "utility/prop_info.h"

namespace yaga {
namespace editor {

// -----------------------------------------------------------------------------------------------------------------------------
class AssetMetadataGui
{
public:
  virtual ~AssetMetadataGui() {}
  virtual assets::AssetPtr load() = 0;
  virtual void rename(const std::string& name) = 0;
  virtual void remove() = 0;
};

typedef std::unique_ptr<AssetMetadataGui> AssetMetadataGuiPtr;

// -----------------------------------------------------------------------------------------------------------------------------
class AssetTypeGui
{
public:
  virtual ~AssetTypeGui() {}
  virtual const std::string& name() const = 0;
  virtual bool external() const = 0;
  
};

typedef std::unique_ptr<AssetTypeGui> AssetTypeGuiPtr;

template <class Value>
class node_iter : public boost::iterator_facade<node_iter<Value>, Value, boost::forward_traversal_tag>
{
public:
  node_iter() : m_node(0) {}
  explicit node_iter(Value* p) : m_node(p) {}

private:
  friend class boost::iterator_core_access;

private:
  bool equal(node_iter<Value> const& other) const { return this->m_node == other.m_node; }
  void increment() { m_node = m_node->next(); }
  Value& dereference() const { return *m_node; }

private:
  Value* m_node;
};

typedef node_iter<node_base> node_iterator;
typedef node_iter<node_base const> node_const_iterator;

// -----------------------------------------------------------------------------------------------------------------------------
class AssetManagerGui : public PropInfo
{
public:
  typedef std::map<std::string, AssetMetadataGuiPtr> AssetMap;
  typedef std::map<AssetTypeGui*, AssetMap> Assets;
  struct PropertyIndex
  {
    static const int assetTypes = 0;
  };

public:
  virtual ~AssetManagerGui() {}
  virtual const Assets& assets() const = 0;
  virtual AssetMetadataGui* createAsset(const std::string& name, const std::string& filename, std::string& reason) = 0;


};

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_GUI_ASSET_MANAGER_GUI
