#include "Pch.h"
#include "Asset.h"
#include "Database.h"

namespace yaga { namespace asset
{
	// -------------------------------------------------------------------------------------------------------------------------
	Asset::Asset(const std::string& name) :
		_name(name)
	{
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Asset::ResolveRefs(Database*)
	{
	}
}}

