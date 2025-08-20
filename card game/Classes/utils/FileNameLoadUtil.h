#pragma once
#include "models/CardData.h"
#include "models/CardType.h"
#include <vector>
#include <string>

class FileNameLoadUtil {
public:
	static std::vector<std::string> fileNameLoad(const CardData cardData);
};
