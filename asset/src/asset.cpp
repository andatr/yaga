#include "precompiled.h"
#include "asset.h"
#include "database.h"

namespace yaga
{
namespace asset
{

// -------------------------------------------------------------------------------------------------------------------------
Asset::Asset(const std::string& name) :
  name_(name)
{
}

// -------------------------------------------------------------------------------------------------------------------------
void Asset::ResolveRefs(Database*)
{
}

} // !namespace asset
} // !namespace yaga

