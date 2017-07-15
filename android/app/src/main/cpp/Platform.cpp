#include "../../../../../common/Platform.h"

#include <jni.h>
#include <android/native_activity.h>

namespace Platform
{
	JNIEnv* jniEnv = nullptr;

	jclass platformClass = nullptr;

	jmethodID getBooleanPreferenceMethod = nullptr;
	jmethodID getFloatPreferenceMethod = nullptr;
	jmethodID getIntPreferenceMethod = nullptr;
	jmethodID getLongPreferenceMethod = nullptr;
	jmethodID getStringPreferenceMethod = nullptr;
	jmethodID putBooleanPreferenceMethod = nullptr;
	jmethodID putFloatPreferenceMethod = nullptr;
	jmethodID putIntPreferenceMethod = nullptr;
	jmethodID putLongPreferenceMethod = nullptr;
	jmethodID putStringPreferenceMethod = nullptr;

	void init(JNIEnv* env, ANativeActivity* activity)
	{
		jniEnv = env;

		jclass activityClass = jniEnv->GetObjectClass(activity->clazz);
		jmethodID getClassLoaderMethod = jniEnv->GetMethodID(activityClass, "getClassLoader", "()Ljava/lang/ClassLoader;");

		jobject classLoader = jniEnv->CallObjectMethod(activity->clazz, getClassLoaderMethod);
		jclass classLoaderClass = jniEnv->GetObjectClass(classLoader);
		jmethodID findClassMethod = jniEnv->GetMethodID(classLoaderClass, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");

		platformClass = (jclass)jniEnv->CallObjectMethod(classLoader, findClassMethod, jniEnv->NewStringUTF("fi/haippirinki/l2p/Platform"));

		getBooleanPreferenceMethod = jniEnv->GetStaticMethodID(platformClass, "getBooleanPreference", "(Ljava/lang/String;)Z");
		getFloatPreferenceMethod = jniEnv->GetStaticMethodID(platformClass, "getFloatPreference", "(Ljava/lang/String;)F");
		getIntPreferenceMethod = jniEnv->GetStaticMethodID(platformClass, "getIntPreference", "(Ljava/lang/String;)I");
		getLongPreferenceMethod = jniEnv->GetStaticMethodID(platformClass, "getLongPreference", "(Ljava/lang/String;)J");
		getStringPreferenceMethod = jniEnv->GetStaticMethodID(platformClass, "getStringPreference", "(Ljava/lang/String;)Ljava/lang/String;");
		putBooleanPreferenceMethod = jniEnv->GetStaticMethodID(platformClass, "putBooleanPreference", "(Ljava/lang/String;Z)V");
		putFloatPreferenceMethod = jniEnv->GetStaticMethodID(platformClass, "putFloatPreference", "(Ljava/lang/String;F)V");
		putIntPreferenceMethod = jniEnv->GetStaticMethodID(platformClass, "putIntPreference", "(Ljava/lang/String;I)V");
		putLongPreferenceMethod = jniEnv->GetStaticMethodID(platformClass, "putLongPreference", "(Ljava/lang/String;J)V");
		putStringPreferenceMethod = jniEnv->GetStaticMethodID(platformClass, "putStringPreference", "(Ljava/lang/String;Ljava/lang/String;)V");
	}

	bool getBooleanPreference(const std::string& key)
	{
		jstring jkey = jniEnv->NewStringUTF(key.c_str());
		bool r = jniEnv->CallStaticBooleanMethod(platformClass, getBooleanPreferenceMethod, jkey);
		jniEnv->DeleteLocalRef(jkey);
		return r;
	}

	float getFloatPreference(const std::string& key)
	{
		jstring jkey = jniEnv->NewStringUTF(key.c_str());
		float r = jniEnv->CallStaticFloatMethod(platformClass, getFloatPreferenceMethod, jkey);
		jniEnv->DeleteLocalRef(jkey);
		return r;
	}

	int getIntPreference(const std::string& key)
	{
		jstring jkey = jniEnv->NewStringUTF(key.c_str());
		int r = jniEnv->CallStaticIntMethod(platformClass, getIntPreferenceMethod, jkey);
		jniEnv->DeleteLocalRef(jkey);
		return r;
	}

	long long getLongPreference(const std::string& key)
	{
		jstring jkey = jniEnv->NewStringUTF(key.c_str());
		long long r = jniEnv->CallStaticLongMethod(platformClass, getLongPreferenceMethod, jkey);
		jniEnv->DeleteLocalRef(jkey);
		return r;
	}

	std::string getStringPreference(const std::string& key)
	{
		jstring jkey = jniEnv->NewStringUTF(key.c_str());
		jstring s = (jstring)jniEnv->CallStaticObjectMethod(platformClass, getStringPreferenceMethod, jkey);
		jniEnv->DeleteLocalRef(jkey);
		const char* p = jniEnv->GetStringUTFChars(s, nullptr);
		size_t len = (size_t)jniEnv->GetStringUTFLength(s);
		std::string r(p, len);
		jniEnv->ReleaseStringUTFChars(s, p);
		jniEnv->DeleteLocalRef(s);
		return r;
	}

	void putBooleanPreference(const std::string& key, bool value)
	{
		jstring jkey = jniEnv->NewStringUTF(key.c_str());
		jniEnv->CallStaticVoidMethod(platformClass, putBooleanPreferenceMethod, jkey, value);
		jniEnv->DeleteLocalRef(jkey);
	}

	void putFloatPreference(const std::string& key, float value)
	{
		jstring jkey = jniEnv->NewStringUTF(key.c_str());
		jniEnv->CallStaticVoidMethod(platformClass, putFloatPreferenceMethod, jniEnv->NewStringUTF(key.c_str()), value);
		jniEnv->DeleteLocalRef(jkey);
	}

	void putIntPreference(const std::string& key, int value)
	{
		jstring jkey = jniEnv->NewStringUTF(key.c_str());
		jniEnv->CallStaticVoidMethod(platformClass, putIntPreferenceMethod, jniEnv->NewStringUTF(key.c_str()), value);
		jniEnv->DeleteLocalRef(jkey);
	}

	void putLongPreference(const std::string& key, long long value)
	{
		jstring jkey = jniEnv->NewStringUTF(key.c_str());
		jniEnv->CallStaticVoidMethod(platformClass, putLongPreferenceMethod, jniEnv->NewStringUTF(key.c_str()), value);
		jniEnv->DeleteLocalRef(jkey);
	}

	void putStringPreference(const std::string& key, const std::string& value)
	{
		jstring jkey = jniEnv->NewStringUTF(key.c_str());
		jstring jvalue = jniEnv->NewStringUTF(value.c_str());
		jniEnv->CallStaticVoidMethod(platformClass, putStringPreferenceMethod, jkey, jvalue);
		jniEnv->DeleteLocalRef(jkey);
		jniEnv->DeleteLocalRef(jvalue);
	}
}
