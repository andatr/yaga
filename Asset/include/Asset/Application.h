#ifndef YAGA_ASSET_APPLICATION
#define YAGA_ASSET_APPLICATION

#include <memory>
#include <istream>
#include <ostream>

#include "Asset.h"
#include "Array.h"
#include "Serializer.h"

namespace yaga { namespace asset
{
	class Application;
	typedef std::unique_ptr<Application> ApplicationPtr;

	class Application : public Asset
	{
	public:
		explicit Application(const std::string& name);
		virtual ~Application();
		bool Fullscreen() const { return _fullscreen; }
		uint32_t Width() const { return _width; }
		uint32_t Height() const { return _height; }
		std::string Title() const { return _title; }
	public: // Serialization
		static const AssetId assetId;
		static size_t Serialize(Asset* asset, std::ostream& stream, bool binary);
		static ApplicationPtr Deserialize(const std::string& name, std::istream& stream, size_t size, bool binary);
	private:
		bool _fullscreen;
		uint32_t _width;
		uint32_t _height;
		std::string _title;
	};
}}

#endif // !YAGA_ASSET_APPLICATION
