#include "../common/Platform.h"

#include <map>

namespace Platform
{
	std::map<std::string, bool> booleanPreferences;
	std::map<std::string, float> floatPreferences;
	std::map<std::string, int> intPreferences;
	std::map<std::string, long long> longPreferences;
	std::map<std::string, std::string> stringPreferences;

	bool getBooleanPreference(const std::string& key)
	{
		auto it = booleanPreferences.find(key);
		if(it != booleanPreferences.end())
		{
			return it->second;
		}
		return false;
	}

	float getFloatPreference(const std::string& key)
	{
		auto it = floatPreferences.find(key);
		if(it != floatPreferences.end())
		{
			return it->second;
		}
		return 0.f;
	}

	int getIntPreference(const std::string& key)
	{
		auto it = intPreferences.find(key);
		if(it != intPreferences.end())
		{
			return it->second;
		}
		return 0;
	}

	long long getLongPreference(const std::string& key)
	{
		auto it = longPreferences.find(key);
		if(it != longPreferences.end())
		{
			return it->second;
		}
		return 0;
	}

	std::string getStringPreference(const std::string& key)
	{
		auto it = stringPreferences.find(key);
		if(it != stringPreferences.end())
		{
			return it->second;
		}
		return "";
	}

	void putBooleanPreference(const std::string& key, bool value)
	{
		booleanPreferences[key] = value;
	}

	void putFloatPreference(const std::string& key, float value)
	{
		floatPreferences[key] = value;
	}

	void putIntPreference(const std::string& key, int value)
	{
		intPreferences[key] = value;
	}

	void putLongPreference(const std::string& key, long long value)
	{
		longPreferences[key] = value;
	}

	void putStringPreference(const std::string& key, const std::string& value)
	{
		stringPreferences[key] = value;
	}
}
