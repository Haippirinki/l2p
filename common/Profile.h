#pragma once

#include <string>

namespace Profile
{
	int getCurrentLevel();
	void setCurrentLevel(int level);

	std::string getLevelName(int level);
}
