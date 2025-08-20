#pragma once

#include <vector>
#include <string>

#include "cocos2d.h"
#include "models/CardData.h"
#include "utils/FileNameLoadUtil.h"

/**
 * 卡牌视图类，负责卡牌的显示和交互
 */
class CardView : public cocos2d::Sprite {

public:
	/**
	 * 创建卡牌视图
	 * @param cardData 卡牌数据
	 * @return 卡牌视图实例
	 */
	static CardView* create(CardData cardData);

	/**
	 * 使用模型数据初始化卡牌
	 * @param cardData 卡牌数据
	 * @return 是否初始化成功
	 */
	bool initWithModel(CardData cardData);

	/**
	 * 设置是否为顶部牌
	 * @param isTop 是否为顶部牌
	 */
	void setIsTopCard(bool isTop) { _isTopCard = isTop; }

	/**
	* 获取是否为顶部牌
	* @return 是否为顶部牌
	*/
	bool isTopCard() const { return _isTopCard; }

	/**
	 * 获取卡牌X坐标
	 * @return X坐标
	 */
	int getPosX() const { return _cardData.posX; }

	/**
	 * 获取卡牌Y坐标
	 * @return Y坐标
	 */
	int getPosY() const { return _cardData.posY; }

	/**
	 * 获取卡牌面值
	 * @return 卡牌面值
	 */
	int getCardFace() const { return _cardData.cardFace; }

	/**
	* 获取卡牌花色
	* @return 卡牌花色
	*/
	int getCardSuit() const { return _cardData.cardSuit; }

	/**
	* 设置当前顶部牌
	* @param card 顶部牌指针
	*/
	void setCurrentTopCard(CardView* card) { _currentTopCard = card; }

private:
	CardView* _currentTopCard = nullptr; // 当前顶部牌
private:
	bool _isTopCard = false; // 是否为顶部牌
	CardData _cardData;//卡牌数据
	std::vector<std::string> _pixResList;//纹理资源列表
	
	/**
	 * 创建卡牌
	 */
	void createCard();
	std::vector<std::string> _getTextureNameForCard();
};