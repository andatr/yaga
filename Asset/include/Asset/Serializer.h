#ifndef YAGA_ASSET_SERIALIZER
#define YAGA_ASSET_SERIALIZER

#include <map>
#include <string>
#include <functional>

#include "Database.h"

namespace yaga { namespace asset
{
	struct AssetId
	{
		std::string friendly;
		uint32_t binary;
	};

	class Serializer
	{
	public:
		typedef std::function<size_t(Asset* asset, std::ostream& stream, bool binary)> SerializeProc;
		typedef std::function<AssetPtr(const std::string& name, std::istream& stream, size_t size, bool binary)> DeserializeProc;
	public:
		static void Register(const AssetId& id, const SerializeProc& serializer, const DeserializeProc& deserializer);
		template<typename T>
		static void Register();
		static void RegisterStandard();
		static size_t Serialize(std::ostream& stream, const std::string& str);
		static std::string DeserializeString(std::istream& stream);
		static void Deserialize(const std::string& name, const std::string& path, Database* db);
		static void Deserialize(const std::string& dir, Database* db);
	private:
		static AssetPtr Deserialize(const std::string& type, const std::string& name, const std::string& file);
	private:
		class SInfo
		{
		public:
			AssetId id;
			SerializeProc serializer;
			DeserializeProc deserializer;
		};
		typedef decltype(AssetId::binary) SInfoId;
		typedef decltype(AssetId::friendly) SInfoIdFriendly;
	private:
		static std::map<SInfoId, SInfo> _serializers;
		static std::map<SInfoIdFriendly, SInfo> _serializersFriendly;
	};

	// -------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	void Serializer::Register()
	{
		Serializer::Register(T::assetId, T::Serialize, T::Deserialize);
	}
}}

#endif // !YAGA_ASSET_SERIALIZER
