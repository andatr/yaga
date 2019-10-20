#include "Pch.h"
#include "Application.h"

namespace yaga { namespace asset
{
	const AssetId Application::assetId = { "cfg", 1 };

	// -------------------------------------------------------------------------------------------------------------------------
	Application::Application(const std::string& name) : 
		Asset(name), _fullscreen(false), _width(0), _height(0)
	{
	}
	
	// -------------------------------------------------------------------------------------------------------------------------
	Application::~Application()
	{
	}

	// -------------------------------------------------------------------------------------------------------------------------
	size_t Application::Serialize(Asset* asset, std::ostream& stream, bool)
	{
		auto app = dynamic_cast<Application*>(asset);
		if (!app)
			THROW("Application serializer was given wrong asset");

		namespace pt = boost::property_tree;
		
		std::stringstream ss;
		pt::ptree window;
		window.put<bool>("fullscreen", app->_fullscreen);
		window.put<uint32_t>("width", app->_width);
		window.put<uint32_t>("height", app->_height);
		window.put<std::string>("title", app->_title);
		pt::ptree props;
		props.add_child("window", window);
		pt::write_json(ss, props);

		stream << ss.rdbuf();
		return ss.tellp();
	}

	// -------------------------------------------------------------------------------------------------------------------------
	ApplicationPtr Application::Deserialize(const std::string& name, std::istream& stream, size_t, bool)
	{
		auto app = std::make_unique<Application>(name);

		namespace pt = boost::property_tree;
		pt::ptree props;
		pt::read_json(stream, props);
		auto window = props.get_child("window");
		app->_fullscreen = window.get<bool>("fullscreen", false);
		app->_width = window.get<uint32_t>("width", 640);
		app->_height = window.get<uint32_t>("height", 480);
		app->_title = window.get<std::string>("title", "YAGA Game");
				
		return std::move(app);
	}
}}

