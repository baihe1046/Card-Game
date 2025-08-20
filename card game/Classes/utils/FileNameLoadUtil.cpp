#include "FileNameLoadUtil.h"
#include <stdexcept>

std::vector<std::string> FileNameLoadUtil::fileNameLoad(const CardData cardData) {

	int face = cardData.cardFace;
	int suit = cardData.cardSuit;

	//花色字符加载
	std::string suitStr = "";
	if (suit == CST_CLUBS)
	{
		suitStr = "club";
	}
	else if (suit == CST_DIAMONDS)
	{
		suitStr = "diamond";
	}
	else if (suit == CST_HEARTS)
	{
		suitStr = "heart";
	}
	else if (suit == CST_SPADES)
	{
		suitStr = "spade";
	}

	//数字颜色字符加载
	std::string colorStr = "";
	if (suit == CST_HEARTS || suit == CST_DIAMONDS)
	{
		colorStr = "red";
	}
	else if (suit == CST_CLUBS || suit == CST_SPADES)
	{
		colorStr = "black";
	}

	//数字字符加载
	std::string number = "";

	if (face == 0) {
		//A
		number = "A";
	}
	else if (face == 10) {
		//J
		number = "J";
	}
	else if (face == 11) {
		//Q
		number = "Q";
	}
	else if (face == 12) {
		//K
		number = "K";
	}
	else {
		int value = static_cast<int>(face) + 1;
		number = std::to_string(value);
	}
	std::string backgroundPix = "res/card_general.png";
	std::string suitPix = "res/suits/" + suitStr + ".png";
	std::string bigFacePix = "res/number/big_" + colorStr + "_" + number + ".png";
	std::string smallFaceStr = "res/number/small_" + colorStr + "_" + number + ".png";
	std::vector<std::string> pixResList;
	pixResList.push_back(backgroundPix);
	pixResList.push_back(suitPix);
	pixResList.push_back(bigFacePix);
	pixResList.push_back(smallFaceStr);
	return pixResList;
}