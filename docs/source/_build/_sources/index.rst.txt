ConfigParser Documentation
==========================

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   examples
   library_notes
   api

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

ConfigParser
============

ConfigParser is a C++ header only library for managing configuration files, it supports both INI and CFG format types. 

Features
--------

* Simple and intuitive API, simple and straightforward usage.
* A map-like structure which makes inserting and removing values easier.
* Stores all 6 basic data types with their original values, without the need for explicit conversion (int, float, double, string, bool, char)
* Supports both INI and CFG file formats.

Installation
------------

Copy the contents of the Src folder over to where your project resides and include ConfigParser.hpp. The library does not depend on anything other than Strutil library for string manipulation.

Usage
-----

Simple example of writing some arbitrary data into an INI file:

.. code-block:: cpp

   #include <iostream>
   #include "ConfigParser.hpp"

   using namespace std;
   using namespace ConfigParser;
   int main() {
       IniParser config;
       // Insert keys and values like you would a map or a python dictionary.
       config["nickname"] = "player";
       config["server-address"] = "server.host.com";
       config["server-port"] = 2500;
       config["voice-chat-volume"] = 0.5f;
       config["play-logo-start"] = false;

       config.save("settings.ini");
       return 0;
   }

Reading files:

.. code-block:: cpp

   #include <iostream>
   #include "ConfigParser.hpp"

   using namespace std;
   using namespace ConfigParser;
   int main() {
       IniParser config("settings.ini");
       if (config.getError() == ConfigError::NO_ERROR) {
           // iterate over keys in config and print their values
           for (auto& key : config) {
               cout << key << " = " << config[key] << endl;
           }
       }
       else {
           cout << "error opening file" << endl;
       }

       return 0;
   }

See the examples page for more detailed usage.

Library Notes
-------------

* The library handles and preserves empty lines and comments and generally keeps the original structure of a configuration file.
* The library uses std::string to store value data, then does the conversion when values are cast to other types.
* Any values that were inserted can be retrieved in their original types or in string format.

Credits
-------

String manipulation - Strutil
`Github Page <https://github.com/tgalaj/strutil>`_