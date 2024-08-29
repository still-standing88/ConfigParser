/**
 ******************************************************************************
* ConfigParser.
* @file ConfigParser.hpp
* @brief C++ header only library to manage configuration files, provides classes for different configuration types.
* @Copyright (C) 2024 Oussama Ben Gatrane
* @license
* This product is licensed under the MIT License. See the license file for the full license text.
*
* @see https://github.com/still-standing88/ConfigParser
* Version: 1.1.0
*  Author: [Oussama AKA Still standing]
* Description: A simple C++ header only library to manage configuration files.
*
  ******************************************************************************
**/
#pragma once
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <utility>
#include <string>
#include <sstream>
#include <fstream>
#include <type_traits>
#include <typeinfo>
#include <map>
#include <unordered_map>
#include <vector>
#include "strutil.h"


/**
* @brief ConfigParser namespace
*
*/
namespace ConfigParser {
	// Forword declorations, typedefs user through out namespace
	class ConfigValue;
	class ConfigSection;
	struct ConfigLine;
	typedef std::unordered_map<std::string, ConfigSection> SectionMap;
	typedef std::vector<ConfigLine> LineVector;
	typedef std::map<std::string, ConfigValue> ValueMap;
	typedef std::vector<std::string> StringVector;
	using KeysIter = typename StringVector::iterator;
	using ConstKeysIter = typename StringVector::const_iterator;
	using namespace strutil;

	template<typename element_t>
	static inline int find(const std::vector<element_t>& vectorContainer, const element_t& value) {
		auto iter = std::find(vectorContainer.begin(), vectorContainer.end(), value);
		return !(iter == vectorContainer.end()) ? static_cast<int> (std::distance(vectorContainer.begin(), iter)) : -1;
	}

	template<typename element_t>
	static inline void removeElement(std::vector<element_t>& vectorContainer, const element_t& value) {
		int index = find<element_t>(vectorContainer, value);
		if (index != -1) {
			vectorContainer.erase(vectorContainer.begin() + index);
		}
	}

	/**
* @enum ConfigError Enum class
	* @brief Defined error types used for file error checking.
	*/
	enum class ConfigError {
		FILE_NOT_FOUND,
		FILE_OPEN_ERROR,
		FILE_READ_ERROR,
		NO_ERROR
	};

	/**
* @enum ConfigType enum
	* @brief Config data types.
	*/
	enum class ConfigType {
		CONFIG_EMPTY_LINE,
		CONFIG_COMMENT,
		CONFIG_SECTION,
		CONFIG_VALUE
	};

	/**
	* @struct ConfigLine struct
	* @brief Stores config data for reading/writing config files.
	*/
	struct ConfigLine {
		ConfigType type;
		std::string content;
	};

	/**
	* @class ConfigValue
	* @brief Base class for managing value data, handles different DataTypes and uses std::string to store them.
	*/
	class ConfigValue {
	private:
		std::string data;

	public:
		ConfigValue(): 
			data("") {}
		template<typename value_type>
		ConfigValue(value_type _data = "") {
			setData(_data);
		}
		~ConfigValue() {}

		template<typename value_type>
		ConfigValue& operator=(value_type value) {
			setData(value);
			return *this;
		}

		operator std::string() const { return data; }

		template<typename value_type>
		operator value_type() {
			return getStringValue<value_type>(data);
		}

		friend std::ostream& operator<<(std::ostream& os, const ConfigValue& cv) {
			os << cv.data;
			return os;
		}

		template<typename value_type>
		void setData(value_type value) {
			data.clear();
			if constexpr (std::is_same<value_type, int>::value ||
				std::is_same<value_type, float>::value ||
				std::is_same<value_type, double>::value) {
				data = to_string(value);
			}
			else if constexpr (std::is_same<value_type, char>::value ||
				std::is_same<value_type, std::string>::value) {
				data = value;
			}
			else if constexpr (std::is_same<value_type, bool>::value) {
				data = (value == true) ? "true" : "false";
			}
			else if constexpr (std::is_same<value_type, const char*>::value) {
				data = std::string(value);
			}
			else {
				static_assert("non supported type");
			}
		}
	private:
		template <typename type>
		static inline std::string getType(type variable) {
			return typeid(std::decay_t<decltype(variable)>).name();
		}

		static inline std::string typeErrorMsg(const std::string& type) { return "String value is non convertible to type " + type; }
		template<typename value_t>
		static inline value_t convertString(const std::string& str);

		template<>
		static inline int convertString(const std::string& str) {
			int value;
			try {
				value = std::stoi(str);
				return value;
			}
			catch (...) {
				throw std::invalid_argument(typeErrorMsg("int"));
			}
		}

		template<>
		static inline float convertString(const std::string& str) {
			float value;
			try {
				value = std::stof(str);
				return value;
			}
			catch (...) {
				throw std::invalid_argument(typeErrorMsg("float"));
			}
		}

		template<>
		static inline double convertString(const std::string& str) {
			double value;
			try {
				value = std::stod(str);
				return value;
			}
			catch (...) {
				throw std::invalid_argument(typeErrorMsg("double"));
			}
		}

		template<>
		static inline bool convertString(const std::string& str) {
			if (str == "true" || str == "false") {
				return (str == "true") ? true : false;
			}
			else {
				throw std::invalid_argument(typeErrorMsg("bool"));
			}
		}

		template<>
		static inline char convertString(const std::string& str) {
			if (str.length() == 1) {
				return str[0];
			}
			else {
				throw std::invalid_argument(typeErrorMsg("char"));
			}
		}

		template<>
		static inline std::string convertString(const std::string& str) {
			if (strutil::contains(getType(str), "string")) {
				return str;
			}
			else {
				throw std::invalid_argument(typeErrorMsg("string"));
			}
		}

		template<typename value_t>
		static value_t getStringValue(const std::string& str) {
			if constexpr (std::is_same<value_t, int>::value) {
				return convertString<int>(str);
			}
			else if constexpr (std::is_same<value_t, float>::value) {
				return convertString<float>(str);
			}
			else if constexpr (std::is_same<value_t, double>::value) {
				return convertString<double>(str);
			}
			else if constexpr (std::is_same<value_t, char>::value) {
				return convertString<char>(str);
			}
			else if constexpr (std::is_same<value_t, bool>::value) {
				return convertString<bool>(str);
			}
			else if constexpr (std::is_same<value_t, std::string>::value) {
				return convertString<std::string>(str);
			}
			else {
				static_assert("unsupported conversion type");
			}
		}


	};

	/**
	* @class Parser class
	* @brief Base class for existing parsers. Contains methods which must be overwriten to implement functionality.
	*/
	class Parser {
	public:
		Parser(std::string _path = "") :
			path(_path), errorCode(ConfigError::NO_ERROR) {

		}
		virtual ~Parser() {} //< Implement a custom destructor.

		/**
		* @brief Get the current path to the config file.
		* @return REturns a constant std string for file path.
		*/
		const std::string& getPath() const { return this->path; }

		/**
		* @brief Retrieves currently imitted error code.
		* @return Returns a Config error enum state.
		*/
		const ConfigError getError() const { return errorCode; }

		/**
		* @brief Clears current eror state.
		*/
		void flush() { errorCode = ConfigError::NO_ERROR; }

		/**
		* @brief Loads a config file.
		* @param String, file path.
		*/
		virtual void load(std::string _path) {

			flush();
			erase();
			path = _path;
			readFile();
		}

		/**
		* @brief Reloads an opened config file.
		*/
		virtual void reload() {
			erase();
			readFile();
		}

		/**
		* @brief Writes config data to the file path.
		* @param String, path file, where file must be saved.
		*/
		virtual void save(std::string _path = "") {
			if (!_path.empty()) {
				path = _path;
			}
			write();
		}

	protected:
		static inline bool fileExists(const std::string& filePath) { return std::filesystem::exists(filePath); } //< Checks if a file exists.
		inline bool isComment(std::string str) { return starts_with(trim_copy(str), "#"); } //< Checks if string is a comment.
		inline bool isEmptyLine(std::string str) { return trim_copy(str).empty(); } //< Check if string is an empty line.
		inline bool isValue(const std::string& str) { return contains(str, "="); } //< Check if a string is a value.

		/**
		* @brief Check if string is a section name.
		*/
		inline bool isSection(std::string str) {
			auto _str = trim_copy(str);
			return (starts_with(_str, "[") && ends_with(_str, "]"));
		}

		StringVector extractValue(const std::string& str) { return split(trim_copy(str), '='); } // Extracts the value from  the string.
		/**
		* @brief Extracts a section name from string.
		*/
		std::string extractSection(std::string str) {
			trim(str);
			str.pop_back();
			str.erase(0, 1);
			return str;
		}

		void appendLine(ConfigType type, std::string content) { lines.emplace_back(ConfigLine(type, std::move(content))); } //< Appends a line to the parser (used for parsing data to and from the file).

		/**
		* @brief Removes a specified line using it's content.
		*/
		void removeLine(const std::string& handle) {
			for (int index = 0; index < lines.size(); index++) {
				auto line = lines[index];
				if (line.content == handle) {
					lines.erase(lines.begin() + index);
					break;
				}
			}
		}

		/**
		* @brief helper function to read file.
		*/
		virtual void readFile() {
			
				if (!path.empty()) {

					this->read();
				}
		}

		virtual void read() = 0; //< Override for implementation. (reads data from file)
		virtual void write() = 0;//< Override for implementation. (writes data to file)

		/**
		* @brief Takes cair of clearing lines. (can be overwriten for implementation)
		*/
		virtual void erase() {
			lines.clear();
			lines.shrink_to_fit();
		}

		ConfigError errorCode;
		std::string path;
		std::fstream file;
		LineVector lines;
	};

	/**
	* @class ConfigSection class
 * @brief Represents a configuration section with key-value pairs.
 */
	class ConfigSection {
	public:
		ConfigSection() {}
		~ConfigSection() { clear(); }

		/**
		 * @brief Inserts a key-value pair if the key doesn't exist.
		 * @param key The key to insert.
		 * @param value The value to associate with the key.
		 */
template<typename value_type>
		void insert(std::string key, value_type value) {
			if (!dict.contains(key)) {
				std::string& _key = key;
				keys.push_back(_key);
				dict.emplace(_key,ConfigValue(value));
			}
		}

		/**
		 * @brief Removes and returns the value associated with the key.
		 * @param key The key to remove.
		 * @return The value associated with the key.
		 */
		virtual ConfigValue pop(const std::string& key) {
			auto node = dict.extract(key);
			ConfigValue value = node.mapped();
			removeElement(keys, key);
			node._Release();
			return value;
		}

		/**
		 * @brief Removes a key-value pair.
		 * @param key The key to remove.
		 */
		virtual void remove(const std::string& key) {
			if (dict.contains(key)) {
				removeElement(keys, key);
				dict.erase(key);
			}
		}

		/**
		 * @brief Updates the value of an existing key.
		 * @param key The key to update.
		 * @param value The new value.
		 */
		template<typename value_type>
		void update(const std::string& key, value_type value) {
			if (dict.contains(key)) {
				dict[key] = value;
			}
		}

		/**
		 * @brief Checks if a key exists.
		 * @param key The key to check.
		 * @return True if the key exists, false otherwise.
		 */
		bool exists(const std::string& key) { return dict.contains(key); }

		/**
		 * @brief Clears all key-value pairs.
		 */
		virtual void clear() {
			dict.clear();
			keys.clear();
		}

		/**
		 * @brief Gets the value associated with a key.
		 * @param key The key to look up.
		 * @return The value associated with the key.
		 * @throw std::out_of_range if the key doesn't exist.
		 */
		ConfigValue& get(const std::string& key) {
			if (dict.contains(key)) {
				return dict[key];
			}
			else {
				throw std::out_of_range("Non existent key: " + key);
			}
		}

		virtual ConfigValue& operator[](std::string key) {
			if (dict.contains(key)) {
				return dict[key];
			}
			else {
				dict.emplace(key,ConfigValue(""));
				keys.push_back(key);
				return dict[key];
			}
		}


		KeysIter begin() { return keys.begin(); }
		KeysIter end() { return keys.end(); }
		ConstKeysIter begin() const { return keys.cbegin(); }
		ConstKeysIter end() const { return keys.cend(); }

	protected:
		ValueMap dict;
		StringVector keys;
	};

	/**
	* IniParser class
	* @brief Ini config file type parser, inherits from both ConfigSection and Parser classes.
	*/
	class IniParser : public Parser, public ConfigSection {
	public:
		IniParser(std::string _path = "") :
			Parser(_path) {
			readFile();
		}
		~IniParser() { this->erase(); }

		/**
		* @brief Function override from the ConfigSection class to handle line addition.
		*/
		template<typename value_type>
		void insert(std::string key, value_type value) {
			if (!dict.contains(key)) {
				appendLine(ConfigType::CONFIG_VALUE, key);
				ConfigSection::insert(key,value);
			}
		}

		/**
* @brief Function override from the ConfigSection class to handle line removal.
*/
		virtual ConfigValue pop(const std::string& key) override {
			if (dict.contains(key)) {
				removeLine(key);
			}
			return ConfigSection::pop(key);
		}

		/**
* @brief Function override from the ConfigSection class to handle line removal.
*/
		virtual void remove(const std::string& key) override {
			if (dict.contains(key)) {
				removeLine(key);
				ConfigSection::remove(key);
			}
		}

		virtual ConfigValue& operator[](std::string key) override {
			if (!dict.contains(key)) {
				appendLine(ConfigType::CONFIG_VALUE, key);
			}
			return ConfigSection::operator[](key);
		}
		/**
		* @brief erases all keys and values.
		*/
		virtual void clear() override {
			ConfigSection::clear();
			Parser::erase();
		}

	protected:
		//< Read function implementation
		virtual void read() override {
			if (fileExists(path)) {
				file.open(path, std::ios::in);
				if (!file.is_open()) {
					errorCode = ConfigError::FILE_OPEN_ERROR;
					return;
				}
				std::string line;
				while (!file.eof()) {
					std::getline(file, line);
					if (isComment(line)) {
						appendLine(ConfigType::CONFIG_COMMENT, trim_copy(line));
					}
					else if (isEmptyLine(line)) {
						appendLine(ConfigType::CONFIG_EMPTY_LINE, trim_copy(line));
					}
					else if (isValue(line)) {
						StringVector pare = extractValue(line);
						if (static_cast<int> (pare.size()) >= 2) {
							appendLine(ConfigType::CONFIG_VALUE, pare[0]);
							insert(pare[0], pare[1]);
						}
					}
					line.clear();
				}
				file.close();
				file.clear();
			}
			else {
				errorCode = ConfigError::FILE_NOT_FOUND;
			}
		}

		//< Write function implementation
		virtual void write() override {
			if (!path.empty()) {
				file.open(path, std::ios::out | std::ios::trunc);
				if (file.is_open()) {
					for (auto& line : lines) {
						if (line.type == ConfigType::CONFIG_EMPTY_LINE || line.type == ConfigType::CONFIG_COMMENT) {
							file << line.content << std::endl;
						}
						else if (line.type == ConfigType::CONFIG_VALUE) {
							file << line.content << " = " << dict[line.content] << std::endl;
						}
					}
					file.close();
				}
				else {
					errorCode = ConfigError::FILE_OPEN_ERROR;
				}
			}
		}

		//< Erase function implementation
		virtual void erase() override {
			clear();
		}
	};


	/**
	* @class CfgParser class
 * @brief CFG file type  Parser, inherits from Parser class.
 * Provides functionality for  and removing sections. Each section is a ConfigSection class which provides acces to it's values.
 * It is also possibel to loop through class sections as with values.
 */
	class CfgParser : public Parser {
	private:
		StringVector keys;
		SectionMap _sections;

	public:
		/**
		 * @brief Constructor.
		 * @param _path Path to the configuration file.
		 */
		CfgParser(std::string _path = "") : Parser(_path) {
			readFile();
		}

		/**
		 * @brief Destructor.
		 */
		~CfgParser() {
			clear();
			erase();
		}

		/**
		 * @brief Adds a new section.
		 * @param sectionName Name of the section to add.
		 */
		void addSection(std::string sectionName) {
			if (!_sections.contains(sectionName)) {
				keys.push_back(sectionName);
				appendLine(ConfigType::CONFIG_SECTION, sectionName);
				_sections[sectionName] = ConfigSection();
			}
		}

		/**
		 * @brief Removes a section.
		 * @param sectionName Name of the section to remove.
		 */
		void removeSection(const std::string& sectionName) {
			if (_sections.contains(sectionName)) {
				removeElement<std::string>(keys, sectionName);
				removeLine(sectionName);
				_sections.erase(sectionName);
			}
		}

		/**
		 * @brief Retrieves a section.
		 * @param sectionName Name of the section to retrieve.
		 * @return Reference to the ConfigSection.
		 * @throw std::out_of_range if section not found.
		 */
		ConfigSection& section(const std::string& sectionName) {
			if (_sections.contains(sectionName)) {
				return _sections[sectionName];
			}
			else {
				throw std::out_of_range("Section not found: " + sectionName);
			}
		}

		/**
		 * @brief Gets all section names.
		 * @return Vector of section names.
		 */
		const StringVector& sections() { return keys; }

		/**
		 * @brief Clears all sections and parser data.
		 */
		void clear() {
			keys.clear();
			_sections.clear();
			Parser::erase();
		}

		ConfigSection& operator[](const std::string& sectionName) { return section(sectionName); }

		KeysIter begin() { return keys.begin(); }
		KeysIter end() { return keys.end(); }
		ConstKeysIter cbegin() { return keys.cbegin(); }
		ConstKeysIter cend() { return keys.cend(); }

	protected:
		/**
		 * @brief Reads the configuration file.
		 */
		virtual void read() override {
			if (fileExists(path)) {
				file.open(path, std::ios::in);
				if (!file.is_open()) {
					errorCode = ConfigError::FILE_OPEN_ERROR;
					return;
				}
				std::string line;
				while (!file.eof()) {
					std::getline(file, line);
					if (isComment(line)) {
						appendLine(ConfigType::CONFIG_COMMENT, trim_copy(line));
					}
					else if (isEmptyLine(line)) {
						appendLine(ConfigType::CONFIG_EMPTY_LINE, trim_copy(line));
					}
					else if (isSection(line)) {
						std::string section_name = extractSection(line);
						appendLine(ConfigType::CONFIG_SECTION, line);
						addSection(section_name);
						bool section_end = false;
						while (!section_end && !file.eof()) {
							line.clear();
							std::getline(file, line);
							if (isEmptyLine(line)) {
								section_end = true;
							}
							else if (isValue(line)) {
								auto value = extractValue(line);
								if (static_cast<int>(value.size()) >= 2) {
									_sections[section_name][value[0]] = value[1];
								}
							}
						}
					}
					line.clear();
				}
				file.close();
				file.clear();
			}
			else {
				errorCode = ConfigError::FILE_NOT_FOUND;
			}
		}

		/**
		 * @brief Writes the configuration to file.
		 */
		virtual void write() override {
			if (!path.empty()) {
				file.open(path, std::ios::out | std::ios::trunc);
				if (file.is_open()) {
					for (auto& line : lines) {
						if (line.type == ConfigType::CONFIG_EMPTY_LINE || line.type == ConfigType::CONFIG_COMMENT) {
							file << line.content << std::endl;
						}
						else if (line.type == ConfigType::CONFIG_SECTION) {
							file << "[" << line.content << "]" << std::endl;
							ConfigSection& section_ = (*this)[line.content];
							for (auto& value : section_) {
								file << value << " = " << section_[value] << std::endl;
							}
							file << "\n" << std::endl;
						}
					}
					file.close();
					file.clear();
				}
				else {
					errorCode = ConfigError::FILE_OPEN_ERROR;
				}
			}
		}

		/**
		 * @brief Erases all data.
		 */
		virtual void erase() override {
			clear();
		}
	};
} //Namespace ConfigParser 