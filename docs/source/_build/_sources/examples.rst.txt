Examples
========

This page demonstrates various features of the ConfigParser library, showcasing both INI and CFG file handling.

INI File Handling
-----------------

Basic Usage
^^^^^^^^^^^

Here's a simple example of creating and saving an INI file:

.. code-block:: cpp

   #include "ConfigParser.hpp"
   #include <iostream>

   int main() {
       ConfigParser::IniParser config;
       
       // Adding key-value pairs
       config["app_name"] = "MyApp";
       config["version"] = 1.2;
       config["debug_mode"] = true;
       
       // Saving to file
       config.save("config.ini");
       
       return 0;
   }

Reading and Modifying
^^^^^^^^^^^^^^^^^^^^^

Reading from an existing INI file and modifying its contents:

.. code-block:: cpp

   #include "ConfigParser.hpp"
   #include <iostream>

   int main() {
       ConfigParser::IniParser config("config.ini");
       
       if (config.getError() == ConfigParser::ConfigError::NO_ERROR) {
           // Reading values
           std::cout << "App Name: " << config["app_name"] << std::endl;
           std::cout << "Version: " << static_cast<double>(config["version"]) << std::endl;
           
           // Modifying a value
           config["debug_mode"] = false;
           
           // Adding a new key-value pair
           config["max_users"] = 100;
           
           // Saving changes
           config.save();
       } else {
           std::cout << "Error loading config file" << std::endl;
       }
       
       return 0;
   }

Iterating Over Keys
^^^^^^^^^^^^^^^^^^^

You can easily iterate over all keys in an INI file:

.. code-block:: cpp

   for (const auto& key : config) {
       std::cout << key << " = " << config[key] << std::endl;
   }

CFG File Handling
-----------------

Creating Sections
^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include "ConfigParser.hpp"
   #include <iostream>

   int main() {
       ConfigParser::CfgParser config;
       
       // Adding sections and key-value pairs
       config.addSection("AppInfo");
       config["AppInfo"]["name"] = "MyApp";
       config["AppInfo"]["version"] = 2.0;
       
       config.addSection("UserSettings");
       config["UserSettings"]["theme"] = "dark";
       config["UserSettings"]["font_size"] = 12;
       
       // Saving to file
       config.save("settings.cfg");
       
       return 0;
   }

Reading and Modifying Sections
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include "ConfigParser.hpp"
   #include <iostream>

   int main() {
       ConfigParser::CfgParser config("settings.cfg");
       
       if (config.getError() == ConfigParser::ConfigError::NO_ERROR) {
           // Reading from a specific section
           std::cout << "App Name: " << config["AppInfo"]["name"] << std::endl;
           
           // Modifying a value in a section
           config["UserSettings"]["font_size"] = 14;
           
           // Adding a new section
           config.addSection("Performance");
           config["Performance"]["max_fps"] = 60;
           
           // Saving changes
           config.save();
       } else {
           std::cout << "Error loading config file" << std::endl;
       }
       
       return 0;
   }

Iterating Over Sections and Keys
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   for (const auto& sectionName : config.sections()) {
       std::cout << "[" << sectionName << "]" << std::endl;
       const auto& section = config.section(sectionName);
       for (const auto& key : section) {
           std::cout << key << " = " << section[key] << std::endl;
       }
       std::cout << std::endl;
   }

Error Handling
--------------

The library provides error checking capabilities:

.. code-block:: cpp

   ConfigParser::IniParser config("non_existent.ini");
   if (config.getError() == ConfigParser::ConfigError::FILE_NOT_FOUND) {
       std::cout << "Config file not found. Creating a new one." << std::endl;
       // Proceed to create a new config file
   }

Type Conversion
---------------

The library handles automatic type conversion for basic types:

.. code-block:: cpp

   config["integer_value"] = 42;
   config["float_value"] = 3.14f;
   config["boolean_value"] = true;

   int intValue = config["integer_value"];
   float floatValue = config["float_value"];
   bool boolValue = config["boolean_value"];