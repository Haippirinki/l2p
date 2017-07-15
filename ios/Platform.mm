#include "../common/Platform.h"

#import <Foundation/Foundation.h>

namespace Platform
{
	NSUserDefaults* defaults = nullptr;

	void init()
	{
		defaults = [NSUserDefaults standardUserDefaults];
	}

	bool getBooleanPreference(const std::string& key)
	{
		NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
		BOOL value = [defaults boolForKey:nsKey];
		return (value != NO);
	}

	float getFloatPreference(const std::string& key)
	{
		NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
		return [defaults floatForKey:nsKey];
	}

	int getIntPreference(const std::string& key)
	{
		NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
		return (int)[defaults integerForKey:nsKey];
	}

	long long getLongPreference(const std::string& key)
	{
		NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
		return (long long)[defaults integerForKey:nsKey];
	}

	std::string getStringPreference(const std::string& key)
	{
		NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
		NSString* value = [defaults stringForKey:nsKey];
		if(value)
		{
			return [value UTF8String];
		}
		return "";
	}

	void putBooleanPreference(const std::string& key, bool value)
	{
		NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
		[defaults setBool:value forKey:nsKey];
	}

	void putFloatPreference(const std::string& key, float value)
	{
		NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
		[defaults setFloat:value forKey:nsKey];
	}

	void putIntPreference(const std::string& key, int value)
	{
		NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
		[defaults setInteger:(NSInteger)value forKey:nsKey];
	}

	void putLongPreference(const std::string& key, long long value)
	{
		NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
		[defaults setInteger:(NSInteger)value forKey:nsKey];
	}

	void putStringPreference(const std::string& key, const std::string& value)
	{
		NSString* nsKey = [NSString stringWithUTF8String:key.c_str()];
		NSString* nsValue = [NSString stringWithUTF8String:value.c_str()];
		[defaults setObject:nsValue forKey:nsKey];
	}
}
