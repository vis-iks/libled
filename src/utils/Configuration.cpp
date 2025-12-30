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
    String rawDataPath = toml.GetString("General.DataPath", procdir);

    if (File::IsPathRelative(rawDataPath.c_str()))
    {
        // If the config has a relative path (e.g. "../assets"), combine it 
        // with the process directory to create an absolute path.
        datapath = File::CombinePath(procdir.c_str(), rawDataPath.c_str());
        
        // Recommended: Clean up the path (resolves "/bin/../assets" to "/assets")
        // You will need to add the GetCanonicalPath function below to use this.
        datapath = File::GetCanonicalPath(datapath); 
    }
    else
    {
        // It is already absolute, use it as is.
        datapath = rawDataPath;
    }
}
