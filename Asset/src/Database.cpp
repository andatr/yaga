#include "Pch.h"
#include "Database.h"

namespace yaga { namespace asset
{
	// -------------------------------------------------------------------------------------------------------------------------
	Asset* Database::Get(const std::string& name)
	{
		auto it = _assets.find(name);
		if (it == _assets.end())
			THROW("Could not find asset \"%1%\"", name);
		return it->second.get();
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Database::Put(AssetPtr asset, bool resolveRefs)
	{
		// auto it = _assets.find(asset->Name());
		// if (it != _assets.end())
		//	THROW("Asset with name \"%1%\" already exists", asset->Name());
		if (resolveRefs) {
			asset->ResolveRefs(this);
		}
		_assets[asset->Name()] = std::move(asset);
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Database::ResolveRefs()
	{
		for (const auto& asset: _assets) {
			asset.second->ResolveRefs(this);
		}
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Database::Delete(AssetPtr asset)
	{
		_assets.erase(asset->Name());
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Database::Delete(const std::string& name)
	{
		_assets.erase(name);
	}
}}

