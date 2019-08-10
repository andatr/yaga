#include "Pch.h"
#include "Version.h"
#include "Platform/include/IApplication.h"
#include "Common/include/Exception.h"
#include "Common/include/Log.h"

const int WIDTH = 800;
const int HEIGHT = 600;
const char* TITLE = "YAGA";

// -----------------------------------------------------------------------------------------------------------------------------
int main()
{
	using namespace yaga;

#ifdef NDEBUG
	try
#endif
	{
		log::Init(log::Severity::trace);
		auto app = CreateApplication(WIDTH, HEIGHT, TITLE);
		app->Run();
	}
#ifdef NDEBUG
	catch (const Exception& exp) {
		Log(exp);
		return EXIT_FAILURE;
	}
	catch (const std::exception& exp) {
		Log(log::Severity::fatal, exp.what());
		return EXIT_FAILURE;
	}
	catch (...) {
		Log(log::Severity::fatal, "Unknown exception");
		return EXIT_FAILURE;
	}
#endif
	return EXIT_SUCCESS;
}