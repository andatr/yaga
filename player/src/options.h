#ifndef YAGA_PLAYER_SRC_OPTIONS
#define YAGA_PLAYER_SRC_OPTIONS

#include "utility/compiler.h"

#include <string>

namespace yaga {
namespace player {

class ProgramOptions
{
public:
  explicit ProgramOptions(int argc, char* argv[]);
  const std::string& workingDir()  const { return workingDir_;  }
  const std::string& filename()    const { return filename_;    }
  const std::string& logSeverity() const { return logSeverity_; }

private:
  std::string workingDir_;
  std::string filename_;
  std::string logSeverity_;
};

} // !namespace player
} // !namespace yaga

#endif // !YAGA_PLAYER_SRC_OPTIONS
