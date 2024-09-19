/*A memory editor that does magical things to your games. In order to develop and apply real-time cheats use JGecko U.

		Special thanks to:
		Chadderz, Marionumber1 - Original TCP Gecko Installer
		dimok - Homebrew Launcher
		kinnay - Diibugger
		pwsincd - Icon and XML
		CosmoCortney - Original Cheat code handler
		Mewtality - New Cheat code handler
- from meta.xml
*/

#include <wups.h>

#include "arama/arama.h"
#include "arama/logger.h"
#include "arama/menu.h"
#include "arama/config.h"
#include "arama/notify.h"
#include "tcp/server.h"
#include "gecko/gecko.h"
#include "gecko/codeHandler.h"

//Metadata
WUPS_PLUGIN_NAME(ARAMA_PLUGIN_NAME);
WUPS_PLUGIN_DESCRIPTION("RAM multi-tool for Aroma");

WUPS_PLUGIN_VERSION("active-dev, v0.2");
WUPS_PLUGIN_AUTHOR("29thDay, c08oprkiua, TCPGecko contributors");
WUPS_PLUGIN_LICENSE("GPLv3");

WUPS_USE_WUT_DEVOPTAB();
WUPS_USE_STORAGE(ARAMA_PLUGIN_NAME);

INITIALIZE_PLUGIN()
{
	Logger::init(Logger::UDP);
	Logger::printf("\ninit plugin");

	PluginMenu::Init();

	Logger::deinit();
}

DEINITIALIZE_PLUGIN()
{
	TCP::stop();
}

ON_APPLICATION_START()
{
	Logger::init(Logger::UDP);
	Notifications::init();
	Logger::print("application start");

	if (Config::options[Config::aRAMa] && isRunningAllowedTitleID())
	{
		// Logger::print("Install code handler");
		// CodeHandler::Install();

		Notifications::show("Gecko starting...");
		TCP::start(runGecko);
	}
}

ON_APPLICATION_REQUESTS_EXIT()
{
	Logger::print("application request end");

	TCP::stop();
}

ON_APPLICATION_ENDS()
{
	Notifications::deinit();
	Logger::deinit();
}
