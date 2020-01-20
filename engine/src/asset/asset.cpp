#include "precompiled.h"
#include "asset/asset.h"
#include "asset/database.h"

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
void Asset::resolveRefs(Database*)
{
}

} // !namespace asset
} // !namespace yaga

