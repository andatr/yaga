#ifndef YAGA_ASSET_MATERIAL
#define YAGA_ASSET_MATERIAL

#include <memory>

#include "Shader.h"
#include "Serializer.h"

namespace yaga { namespace asset
{
	class Material;
	typedef std::unique_ptr<Material> MaterialPtr;

	class Material : public Asset
	{
	public:
		explicit Material(const std::string& name);
		virtual ~Material();
		Shader* VertexShader() const { return _vertShader; }
		Shader* FragmentShader() const { return _fragShader; }
		Material& VertexShader(Shader* shader);
		Material& FragmentShader(Shader* shader);
	public: // Serialization
		static const AssetId assetId;
		static size_t Serialize(Asset* asset, std::ostream& stream, bool binary);
		static MaterialPtr Deserialize(const std::string& name, std::istream& stream, size_t size, bool binary);
	protected:
		void ResolveRefs(Database*) override;
	private:
		Shader* _vertShader;
		Shader* _fragShader;
		std::string _vertName;
		std::string _fragName;
	};
}}

#endif // !YAGA_ASSET_MATERIAL
