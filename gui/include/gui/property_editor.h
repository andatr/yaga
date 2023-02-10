#ifndef YAGA_GUI_PROPERTY_EDITOR
#define YAGA_GUI_PROPERTY_EDITOR

#include <memory>

namespace yaga {

class PropertyEditor
{
public:
  virtual void render() = 0;
  virtual ~PropertyEditor() {}
};

typedef std::unique_ptr<PropertyEditor> PropertyEditorPtr;

} // !namespace yaga

#endif // !YAGA_GUI_PROPERTY_EDITOR
