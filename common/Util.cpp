#include "Util.h"

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
