#include "File.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#endif

#ifdef ANDROID
#include <android/asset_manager.h>
AAssetManager* File::s_assetManager = nullptr;
#endif

File::File(const char* path)
{
#ifdef _WIN32
	m_file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if(m_file != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER size;
		GetFileSizeEx(m_file, &size);
		m_size = size.QuadPart;
		m_fileMapping = CreateFileMapping(m_file, nullptr, PAGE_READONLY, (DWORD)(m_size >> 32), (DWORD)m_size, nullptr);
		m_data = MapViewOfFile(m_fileMapping, FILE_MAP_READ, 0, 0, m_size);
	}
	else
	{
		m_fileMapping = INVALID_HANDLE_VALUE;
		m_size = 0;
		m_data = nullptr;
	}
#else
#ifdef ANDROID
	if(strncmp(path, "assets/", 7) == 0)
	{
		path += 7;
	}
	m_asset = AAssetManager_open(s_assetManager, path, AASSET_MODE_BUFFER);
	m_data = AAsset_getBuffer(m_asset);
	m_size = (size_t)AAsset_getLength(m_asset);
#else
	m_file = open(path, O_RDONLY);
	if(m_file != -1)
	{
		struct stat buf;
		fstat(m_file, &buf);
		m_size = (size_t)buf.st_size;
		m_data = mmap(nullptr, m_size, PROT_READ, MAP_PRIVATE, m_file, 0);
	}
	else
	{
		m_size = 0;
		m_data = nullptr;
	}
#endif
#endif
}

File::~File()
{
#ifdef _WIN32
	if(m_data)
	{
		UnmapViewOfFile(m_data);
	}

	if(m_fileMapping != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_fileMapping);
	}

	if(m_file != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_file);
	}
#else
#ifdef ANDROID
	AAsset_close(m_asset);
#else
	if(m_data)
	{
		munmap(const_cast<void*>(m_data), m_size);
	}

	if(m_file != -1)
	{
		close(m_file);
	}
#endif
#endif
}

size_t File::getSize() const
{
	return m_size;
}

const void* File::getData() const
{
	return m_data;
}

#ifdef ANDROID
void File::init(AAssetManager* assetManager)
{
	s_assetManager = assetManager;
}
#endif
