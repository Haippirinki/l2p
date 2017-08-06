#include "Util.h"

#include <cstdarg>
#include <cstdio>

std::vector<std::string> getLines(const char* data, size_t size)
{
	std::vector<std::string> lines;

	const char* p = nullptr;
	for(size_t i = 0; i < size; i++)
	{
		if(!p)
		{
			if(!isspace(data[i]))
			{
				p = data + i;
			}
		}
		else
		{
			if(data[i] == '\n')
			{
				lines.push_back(std::string(p, i - (p - data)));
				p = nullptr;
			}
		}
	}

	if(p)
	{
		lines.push_back(std::string(p, size - (p - data)));
	}

	return lines;
}

std::string format(const char* fmt, ...)
{
	char buf[1024];
	char* p = buf;
	size_t bufSize = sizeof(buf);
	size_t length = 0;

	while(true)
	{
		va_list ap;
		va_start(ap, fmt);

#if defined(_MSC_VER)
		int r = vsnprintf_s(p, bufSize, _TRUNCATE, fmt, ap);
		length = (r < 0) ? bufSize : r;
#else
		length = vsnprintf(p, bufSize, fmt, ap);
#endif

		va_end(ap);

		if(length < bufSize)
		{
			break;
		}
		else
		{
			if(p != buf)
			{
				delete[] p;
			}

			bufSize *= 2;
			p = new char[bufSize];
		}
	}

	std::string s(p, length);

	if(p != buf)
	{
		delete[] p;
	}

	return s;
}
