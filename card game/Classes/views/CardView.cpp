#include "views/CardView.h"
USING_NS_CC;

CardView* CardView::create(CardData cardData) {
    CardView* pRet = new(std::nothrow) CardView();
    if (pRet && pRet->initWithModel(cardData)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);

    return nullptr;
}
bool CardView::initWithModel(CardData cardData) {
    if (!Sprite::init()) {
        return false;
    }
	_cardData = cardData;
	_pixResList = _getTextureNameForCard();
    createCard();
    return true;
}
std::vector<std::string> CardView::_getTextureNameForCard() {
	// 根据卡牌类型生成纹理名称
	// 实际项目中应根据卡牌类型返回对应图片路径

	return FileNameLoadUtil::fileNameLoad(_cardData);
}
//制作卡牌
void CardView::createCard() {
	//创造渲染纹理
	const float CARD_WIDTH = 182.0f;
	const float CARD_HEIGHT = 282.0f;
	//设置卡牌尺寸
	this->setContentSize(Size(CARD_WIDTH, CARD_HEIGHT));
	//创建背景
	if (!_pixResList.empty()) {
		auto background = Sprite::create(_pixResList[0]);
		if (background) {
			background->setAnchorPoint(Vec2::ZERO);
			background->setPosition(0, 0); 
			background->setContentSize(Size(CARD_WIDTH, CARD_HEIGHT));
			this->addChild(background, 0);
		}
		else {
			CCLOG("Failed to load background texture: %s", _pixResList[0].c_str());
		}
	}

	// 创建花色
	if (_pixResList.size() > 1) {
		auto suitSprite = Sprite::create(_pixResList[1]);
		if (suitSprite) {
			suitSprite->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
			suitSprite->setPosition(15, 245); 
			this->addChild(suitSprite, 1);
		}
	}

	// 创建大卡面
	if (_pixResList.size() > 2) {
		auto bigFace = Sprite::create(_pixResList[2]);
		if (bigFace) {
			bigFace->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
			bigFace->setPosition(CARD_WIDTH / 2, CARD_HEIGHT / 2 - 30); 
			this->addChild(bigFace, 2);
		}
	}

	// 创建小卡面
	if (_pixResList.size() > 3) {
		auto smallFace = Sprite::create(_pixResList[3]);
		if (smallFace) {
			smallFace->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
			smallFace->setPosition(CARD_WIDTH - 25, 245);
			this->addChild(smallFace, 3);
		}
	}
}