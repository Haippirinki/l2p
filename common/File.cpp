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
	m_file = open(path, O_RDONLY);
	if(m_file != -1)
	{
		struct stat buf;
		fstat(m_file, &buf);
		m_size = buf.st_size;
		m_data = mmap(nullptr, m_size, PROT_READ, MAP_PRIVATE, m_file, 0);
	}
	else
	{
		m_size = 0;
		m_data = nullptr;
	}
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
	if(m_data)
	{
		munmap(m_data, m_size);
	}

	if(m_file != -1)
	{
		close(m_file);
	}
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
