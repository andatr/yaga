#include "precompiled.h"
#include "utility/property.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
IProperty::IProperty(const std::string& name, const std::type_index type, bool readOnly) :
  name_(name),
  type_(type),
  readOnly_(readOnly)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
SignalConnectionPtr IProperty::onUpdate(const SignalUpdate::slot_type& handler)
{
  return std::make_unique<SignalConnection>(sigUpdate_.connect(handler));
}

} // !namespace yaga
