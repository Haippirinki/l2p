#include "Profile.h"
#include "File.h"
#include "Util.h"

namespace Profile
{
	int currentLevel = 0;

	int getCurrentLevel()
	{
		return currentLevel;
	}

	void setCurrentLevel(int level)
	{
		currentLevel = level;
	}

	std::string getLevelName(int level)
	{
		File indexFile("assets/levels/index");
		std::vector<std::string> levels = getLines((const char*)indexFile.getData(), indexFile.getSize());
		if(level >= 0 && level < levels.size())
		{
			return levels[level];
		}
		return "";
	}
}
