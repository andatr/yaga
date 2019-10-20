#ifndef YAGA_ASSET_SHADER
#define YAGA_ASSET_SHADER

#include <memory>
#include <istream>
#include <ostream>

#include "Asset.h"
#include "Array.h"
#include "Serializer.h"

namespace yaga { namespace asset
{
	class Shader;
	typedef std::unique_ptr<Shader> ShaderPtr;

	class Shader : public Asset
	{
	public:
		explicit Shader(const std::string& name);
		virtual ~Shader();
		const ByteArray& Code() const { return _code; }
		Shader& Code(const ByteArray& code) { _code = code; return *this; }
	public: // Serialization
		static const AssetId assetId;
		static size_t Serialize(Asset* asset, std::ostream& stream, bool binary);
		static ShaderPtr Deserialize(const std::string& name, std::istream& stream, size_t size, bool binary);
	private:
		ByteArray _code;
	};
}}

#endif // !YAGA_ASSET_SHADER
