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
		if(level >= 0 && level < (int)levels.size())
		{
			return levels[level];
		}
		return "";
	}

	std::string getLevelDescription(int level)
	{
		File levelFile(getLevelName(level).c_str());
		std::vector<std::string> desc = getLines((const char*)levelFile.getData(), levelFile.getSize());
		return desc[0];
	}
}
