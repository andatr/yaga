#include "precompiled.h"
#include "utility/signal.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
SignalConnection::SignalConnection(boost::signals2::connection connection) :
  connection_(connection)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
SignalConnection::~SignalConnection()
{
  connection_.disconnect();
}

} // !namespace yaga
