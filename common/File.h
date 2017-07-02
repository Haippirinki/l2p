#pragma once

#include <cstddef>

#ifdef ANDROID
struct AAsset;
struct AAssetManager;
#endif

class File
{
public:
	File(const char* path);
	~File();

	size_t getSize() const;
	const void* getData() const;

#ifdef ANDROID
	static void init(AAssetManager* assetManager);
#endif

private:
#ifdef _WIN32
	void* m_file;
	void* m_fileMapping;
#else
#ifdef ANDROID
	static AAssetManager* s_assetManager;
	AAsset* m_asset;
#else
	int m_file;
#endif
#endif
	size_t m_size;
	const void* m_data;
};
