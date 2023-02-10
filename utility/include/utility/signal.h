#ifndef YAGA_UTILITY_SIGNAL
#define YAGA_UTILITY_SIGNAL

#include "utility/compiler.h"

#include <memory>
#include <vector>

DISABLE_WARNINGS
#include <boost/signals2.hpp>
ENABLE_WARNINGS

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
class SignalConnection
{
public:
  SignalConnection(boost::signals2::connection connection);
  ~SignalConnection();

private:
  boost::signals2::connection connection_;
};

typedef std::unique_ptr<SignalConnection> SignalConnectionPtr;
typedef std::vector<SignalConnectionPtr> SignalConnections;

} // !namespace yaga

#endif // !YAGA_UTILITY_SIGNAL
