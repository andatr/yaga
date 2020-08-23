#ifndef YAGA_TUBE_RACE_SRC_RACE_GAME
#define YAGA_TUBE_RACE_SRC_RACE_GAME

#include <memory>
#include <boost/dll/alias.hpp>

#include "engine/basic_application.h"

namespace yaga {

class TubeRace : public BasicApplication
{
public:
  TubeRace(assets::Serializer* serializer);
  virtual ~TubeRace();

protected:
  void init(RenderingContext* renderer, Input* input) override;
  void loop(float delta) override;
  void shutdown() override;

private:
  typedef BasicApplication base;
};

typedef std::unique_ptr<TubeRace> TubeRacePtr;

ApplicationPtr createApplication(assets::Serializer*);

} // !namespace yaga

BOOST_DLL_ALIAS(yaga::createApplication, createApplication)

#endif // !YAGA_TUBE_RACE_SRC_RACE_GAME
