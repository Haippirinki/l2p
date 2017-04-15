#include "File.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

File::File(const char* path)
{
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
}

File::~File()
{
	if(m_data)
	{
		munmap(m_data, m_size);
	}

	if(m_file != -1)
	{
		close(m_file);
	}
}

size_t File::getSize() const
{
	return m_size;
}

const void* File::getData() const
{
	return m_data;
}
