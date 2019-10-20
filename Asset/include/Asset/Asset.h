#ifndef YAGA_ASSET_ASSET
#define YAGA_ASSET_ASSET

#include <memory>
#include <string>
#include <vector>

namespace yaga { namespace asset
{
	class Asset
	{
	public:
		explicit Asset(const std::string& name);
		virtual ~Asset() {}
		const std::string& Name() const { return _name; }
	protected:
		friend class Database;
		virtual void ResolveRefs(Database* db);
	protected:
		const std::string _name;
	};

	typedef std::unique_ptr<Asset> AssetPtr;
}}

#endif // !YAGA_ASSET_ASSET

