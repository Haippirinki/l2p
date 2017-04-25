#pragma once

#include <cstddef>

class File
{
public:
	File(const char* path);
	~File();

	size_t getSize() const;
	const void* getData() const;

private:
#if defined(_WIN32)
	void* m_file;
	void* m_fileMapping;
#else
	int m_file;
#endif
	size_t m_size;
	void* m_data;
};
