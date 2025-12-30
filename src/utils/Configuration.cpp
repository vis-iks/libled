#include "utils/Configuration.h"
#include "utils/File.h"

// Constructor
Configuration::Configuration(const String& configFilename)
{
	// Load config file from the application directory
	String procdir = File::GetCurrentProcessDir();
	String fullConfigPath = File::CombinePath(procdir, configFilename);
	std::cout << "Loading configuration " << fullConfigPath << "..." << std::endl;
    if (File::FileExists(fullConfigPath)) {
        try {
	        toml.LoadFile(fullConfigPath);
        } catch (const std::exception& ex) {
            std::cerr << "Error loading configuration: " << ex.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown error loading configuration." << std::endl;
        }
    } else {
        std::cout << "Configuration file not found, using defaults." << std::endl;
    }

	// Read general settings
	datapath = toml.GetString("General.DataPath", procdir);
}
