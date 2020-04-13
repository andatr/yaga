#ifndef YAGA_ENGINE_MODEL
#define YAGA_ENGINE_MODEL

#include <memory>
#include <string>

#include "engine/object.h"

namespace yaga
{

class Model;
typedef std::unique_ptr<Model> ModelPtr;

class Model : public Object
{
public:
  virtual ~Model() {}
};

} // !namespace yaga

#endif // !YAGA_ENGINE_MODEL
