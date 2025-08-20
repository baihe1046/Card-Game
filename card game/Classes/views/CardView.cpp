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
	// ���ݿ�������������������
	// ʵ����Ŀ��Ӧ���ݿ������ͷ��ض�ӦͼƬ·��

	return FileNameLoadUtil::fileNameLoad(_cardData);
}
//��������
void CardView::createCard() {
	//������Ⱦ����
	const float CARD_WIDTH = 182.0f;
	const float CARD_HEIGHT = 282.0f;
	//���ÿ��Ƴߴ�
	this->setContentSize(Size(CARD_WIDTH, CARD_HEIGHT));
	//��������
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

	// ������ɫ
	if (_pixResList.size() > 1) {
		auto suitSprite = Sprite::create(_pixResList[1]);
		if (suitSprite) {
			suitSprite->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
			suitSprite->setPosition(15, 245); 
			this->addChild(suitSprite, 1);
		}
	}

	// ��������
	if (_pixResList.size() > 2) {
		auto bigFace = Sprite::create(_pixResList[2]);
		if (bigFace) {
			bigFace->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
			bigFace->setPosition(CARD_WIDTH / 2, CARD_HEIGHT / 2 - 30); 
			this->addChild(bigFace, 2);
		}
	}

	// ����С����
	if (_pixResList.size() > 3) {
		auto smallFace = Sprite::create(_pixResList[3]);
		if (smallFace) {
			smallFace->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
			smallFace->setPosition(CARD_WIDTH - 25, 245);
			this->addChild(smallFace, 3);
		}
	}
}