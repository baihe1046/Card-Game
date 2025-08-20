#include "GameManager.h"

std::vector<CardData>* GameManager::getPlayfieldCards() {
	return _playfieldCards;
}
std::vector<CardData>* GameManager::getStackCards() {
	return _stackCards;
}