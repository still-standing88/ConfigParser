// main.cpp
#include "ConfigParser.hpp"
#include <iostream>
#include <string>

void demoIniParser() {
    std::cout << "\n--- INI Parser Demo ---\n";
    ConfigParser::IniParser iniConfig;

    // Writing to INI
    iniConfig["app_name"] = "ConfigParserDemo";
    iniConfig["version"] = 1.0;
    iniConfig["debug_mode"] = true;
    iniConfig["max_connections"] = 100;

    iniConfig.save("demo.ini");
    std::cout << "INI file created.\n";

    // Reading from INI
    ConfigParser::IniParser readIni("demo.ini");
    if (readIni.getError() == ConfigParser::ConfigError::NO_ERROR) {
        std::cout << "Reading from INI file:\n";
        for (const auto& key : readIni) {
            std::cout << key << " = " << readIni[key] << std::endl;
        }
    } else {
        std::cout << "Error reading INI file.\n";
    }
}

void demoCfgParser() {
    std::cout << "\n--- CFG Parser Demo ---\n";
    ConfigParser::CfgParser cfgConfig;

    // Writing to CFG
    cfgConfig.addSection("AppInfo");
    cfgConfig["AppInfo"]["name"] = "ConfigParserDemo";
    cfgConfig["AppInfo"]["version"] = 1.0;

    cfgConfig.addSection("Settings");
    cfgConfig["Settings"]["debug_mode"] = true;
    cfgConfig["Settings"]["max_connections"] = 100;

    cfgConfig.save("demo.cfg");
    std::cout << "CFG file created.\n";

    // Reading from CFG
    ConfigParser::CfgParser readCfg("demo.cfg");
    if (readCfg.getError() == ConfigParser::ConfigError::NO_ERROR) {
        std::cout << "Reading from CFG file:\n";
        for (const auto& sectionName : readCfg.sections()) {
            std::cout << "[" << sectionName << "]\n";
            auto& section = readCfg.section(sectionName);
            for (const auto& key : section) {
                std::cout << key << " = " << section[key] << std::endl;
            }
            std::cout << std::endl;
        }
    } else {
        std::cout << "Error reading CFG file.\n";
    }
}

int main() {
    std::cout << "ConfigParser Library Demo\n";
    std::cout << "=========================\n";

    demoIniParser();
    demoCfgParser();

    return 0;
}