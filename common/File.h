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
	int m_file;
	size_t m_size;
	void* m_data;
};
