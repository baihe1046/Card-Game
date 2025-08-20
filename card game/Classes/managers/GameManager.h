#pragma once
#include "models/CardData.h"
#include <vector>
class GameManager {
public:
	GameManager(std::vector<CardData>* pf, std::vector<CardData>* st): _playfieldCards(pf), _stackCards(st) {}
	std::vector<CardData>* getPlayfieldCards();
	std::vector<CardData>* getStackCards();
private:
	std::vector<CardData>* _playfieldCards = nullptr;
	std::vector<CardData>* _stackCards = nullptr;
};