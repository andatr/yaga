#include "Pch.h"
#include "Material.h"
#include "Serializer.h"

namespace yaga { namespace asset
{
	const AssetId Material::assetId = { "met", 3 };

	// -------------------------------------------------------------------------------------------------------------------------
	Material::Material(const std::string& name) :
		Asset(name), _vertShader(nullptr), _fragShader(nullptr)
	{
	}
	
	// -------------------------------------------------------------------------------------------------------------------------
	Material::~Material()
	{
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Material::ResolveRefs(Database* db)
	{
		_vertShader = db->Get<Shader>(_vertName);
		_fragShader = db->Get<Shader>(_fragName);
	}

	// -------------------------------------------------------------------------------------------------------------------------
	Material& Material::VertexShader(Shader* shader)
	{
		_vertShader = shader;
		_vertName = _vertShader->Name();
		return *this;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	Material& Material::FragmentShader(Shader* shader)
	{
		_fragShader = shader;
		_fragName = _fragShader->Name();
		return *this;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	size_t Material::Serialize(Asset* asset, std::ostream& stream, bool)
	{
		auto material = dynamic_cast<Material*>(asset);
		if (!material)
			THROW("Material serializer was given wrong asset");
		auto size = Serializer::Serialize(stream, material->_vertName);
		return size + Serializer::Serialize(stream, material->_fragName);
	}

	// -------------------------------------------------------------------------------------------------------------------------
	MaterialPtr Material::Deserialize(const std::string& name, std::istream& stream, size_t, bool binary)
	{
		auto material = std::make_unique<Material>(name);
		if (binary) {
			material->_vertName = Serializer::DeserializeString(stream);
			material->_fragName = Serializer::DeserializeString(stream);
		}
		else {
			std::getline(stream, material->_vertName);
			std::getline(stream, material->_fragName);
			boost::erase_all(material->_vertName, "\r");
			boost::erase_all(material->_fragName, "\r");
		}
		return material;
	}
}}

