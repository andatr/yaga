#ifndef YAGA_ORBIT_SIM_SRC_ORBIT_SIM
#define YAGA_ORBIT_SIM_SRC_ORBIT_SIM

#include <memory>
#include <boost/dll/alias.hpp>

#include "engine/basic_application.h"

namespace yaga {

class OrbitSim : public BasicApplication
{
public:
  OrbitSim(assets::Serializer* serializer);
  virtual ~OrbitSim();

private:
  void init(RenderingContext* renderer, Input* input) override;
  void loop(float delta) override;
  void shutdown() override;

private:
  typedef BasicApplication base;
};

typedef std::unique_ptr<OrbitSim> OrbitSimPtr;

ApplicationPtr createApplication(assets::Serializer*);

} // !namespace yaga

BOOST_DLL_ALIAS(yaga::createApplication, createApplication)

#endif // !YAGA_ORBIT_SIM_SRC_ORBIT_SIM
