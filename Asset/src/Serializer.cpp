#include "Pch.h"
#include "Array.h"
#include "Serializer.h"
#include "Application.h"
#include "Material.h"
#include "Shader.h"

namespace yaga { namespace asset
{
	namespace fs = boost::filesystem;
	namespace endian = boost::endian;

namespace
{
	// -------------------------------------------------------------------------------------------------------------------------
	std::string TypeFromPath(const fs::path& path)
	{
		auto type = path.extension().string();
		if (type.size() > 0) {
			type = type.substr(1);
			boost::algorithm::to_lower(type);
		}
		return type;
	}

} // !namespace

	std::map<Serializer::SInfoId, Serializer::SInfo> Serializer::_serializers;
	std::map<Serializer::SInfoIdFriendly, Serializer::SInfo> Serializer::_serializersFriendly;

	// -------------------------------------------------------------------------------------------------------------------------
	void Serializer::Register(const AssetId& id, const SerializeProc& serializer, const DeserializeProc& deserializer)
	{
		SInfo info = { id, serializer, deserializer };
		_serializers[id.binary]  = info;
		_serializersFriendly[id.friendly] = info;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Serializer::RegisterStandard()
	{
		Register<asset::Application>();
		Register<asset::Shader>();
		Register<asset::Material>();
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Serializer::Deserialize(const std::string& dir, Database* db)
	{
		auto parent = fs::path(dir);
		fs::recursive_directory_iterator root(dir);
		for (auto&& entry : root) {
			if (!fs::is_regular_file(entry)) continue;
			auto type = TypeFromPath(entry.path());
			auto name = fs::relative(entry, parent).stem().string();
			auto asset = Deserialize(type, name, entry.path().string());
			if (!asset) {
				LOG(warning) << "Could not load asset of type " << type;
			}
			else {
				db->Put(std::move(asset), false);
			}
		}
		db->ResolveRefs();
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Serializer::Deserialize(const std::string& name, const std::string& path, Database* db)
	{
		auto type = TypeFromPath(path);
		auto asset = Deserialize(type, name, path);
		if (!asset) {
			LOG(warning) << "Could not load asset of type " << type;
		}
		else {
			db->Put(std::move(asset));
		}
	}

	// -------------------------------------------------------------------------------------------------------------------------
	AssetPtr Serializer::Deserialize(const std::string& type, const std::string& name, const std::string& file)
	{
		try {
			auto it = _serializersFriendly.find(type);
			if (it != _serializersFriendly.end()) {
				std::ifstream stream(file, std::ios::ate | std::ios::binary);
				size_t size = static_cast<size_t>(stream.tellg());
				stream.seekg(0);
				return it->second.deserializer(name, stream, size, false);
			}
		}
		catch (...) {
			LOG(warning) << "Could not load asset " << file;
		}
		return nullptr;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	size_t Serializer::Serialize(std::ostream& stream, const std::string& str)
	{
		size_t size = str.size();
		endian::native_to_little_inplace(size);
		stream.write((char*)&size, sizeof(size_t));
		stream.write(str.data(), str.size());
		return str.size() + sizeof(size_t);
	}

	// -------------------------------------------------------------------------------------------------------------------------
	std::string Serializer::DeserializeString(std::istream& stream)
	{
		const constexpr size_t BSIZE = 64;
		size_t size;
		stream.read((char*)&size, sizeof(size_t));
		endian::little_to_native_inplace(size);
		if (size <= BSIZE) {
			char buffer[BSIZE];
			stream.read(buffer, size);
			return std::string(buffer, size);
		}
		ByteArray buffer(size);
		stream.read(buffer.Data(), size);
		return std::string(buffer.Data(), size);
	}
}}

