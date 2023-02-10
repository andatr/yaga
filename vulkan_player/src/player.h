#ifndef YAGA_VULKAN_PLAYER_SRC_PLAYER
#define YAGA_VULKAN_PLAYER_SRC_PLAYER

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace yaga {

typedef std::vector<std::pair<std::string, std::string>> Description;

class Stream
{
public:
  virtual const std::string& name()        = 0;
  virtual const Description& description() = 0;
};

class StreamFamily
{
public:
  virtual ~StreamFamily() {}
  virtual int count()                    = 0;
  virtual int current()                  = 0;
  virtual Stream* stream(int index)      = 0;
  virtual void activateStream(int index) = 0;
};

struct Chapter
{
  int64_t timestamp;
  std::string name;
  Chapter() : timestamp(0) {}
};

class Player
{
public:
  virtual ~Player() {}
  virtual void start(const std::string& filename) = 0;
  virtual void stop() = 0;
  virtual void pause() = 0;
  virtual void resume() = 0;
  virtual void seek(int64_t timestamp) = 0;
  virtual int64_t duration()               = 0;
  virtual int64_t timestamp()              = 0;
  virtual const Description& description() = 0;
  virtual const std::string& title()       = 0;
  virtual const std::vector<Chapter>& chapters() = 0;
  virtual StreamFamily* videoStreams()    = 0;
  virtual StreamFamily* audioStreams()    = 0;
  virtual StreamFamily* subtitleStreams() = 0;
};

typedef std::shared_ptr<Player> PlayerPtr;

} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_PLAYER
