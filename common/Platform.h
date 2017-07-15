#pragma once

#include <string>

namespace Platform
{
	bool getBooleanPreference(const std::string& key);
	float getFloatPreference(const std::string& key);
	int getIntPreference(const std::string& key);
	long long getLongPreference(const std::string& key);
	std::string getStringPreference(const std::string& key);

	void putBooleanPreference(const std::string& key, bool value);
	void putFloatPreference(const std::string& key, float value);
	void putIntPreference(const std::string& key, int value);
	void putLongPreference(const std::string& key, long long value);
	void putStringPreference(const std::string& key, const std::string& value);
}
