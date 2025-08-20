#include "GameScene.h"
#include "ui/CocosGUI.h"
#include "LevelSelectScene.h"
#include "utils/JsonUtils.h"
#include "CardView.h"

USING_NS_CC;

Scene* GameScene::createScene(int levelNumber) {
    auto scene = Scene::create();
    auto playfieldCards = JsonUtils::parsePlayfield(levelNumber);
    auto stackCards = JsonUtils::parseStack(levelNumber);

    auto layer = GameScene::create();
    GameController* controller = new GameController(layer, playfieldCards, stackCards);

    layer->setController(controller);
    layer->_levelNumber = levelNumber;
    layer->createUI();

    scene->addChild(layer);
    return scene;
}

bool GameScene::init() {
    if (!Scene::init()) {
        return false;
    }
    return true;
}

void GameScene::createUI() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // ���㶥���͵ײ��߶�
    const float totalRatio = 150 + 58;
    float topHeight = visibleSize.height * (150 / totalRatio);
    float bottomHeight = visibleSize.height * (58 / totalRatio);

    // �����������򱳾�ͼƬ
    auto topBackground = Sprite::create("top_background.png");
    if (topBackground) {
        topBackground->setContentSize(Size(visibleSize.width, topHeight));
        topBackground->setPosition(Vec2(visibleSize.width / 2 + origin.x, origin.y + bottomHeight + topHeight / 2));
        this->addChild(topBackground, -1);
    }
    else {
        CCLOG("Error: Failed to load top background image!");
        // ����ʧ��ʱʹ��ԭ���Ļ�ɫ������Ϊ fallback
        auto fallbackTop = LayerColor::create(Color4B(255, 255, 0, 255), visibleSize.width, topHeight);
        fallbackTop->setPosition(origin.x, origin.y + bottomHeight);
        this->addChild(fallbackTop, -1);
    }

    // �����ײ����򱳾�ͼƬ
    auto bottomBackground = Sprite::create("bottom_background.png");
    if (bottomBackground) {
        bottomBackground->setContentSize(Size(visibleSize.width, bottomHeight));
        bottomBackground->setPosition(Vec2(visibleSize.width / 2 + origin.x, origin.y + bottomHeight / 2));
        this->addChild(bottomBackground, -1);
    }
    else {
        CCLOG("Error: Failed to load bottom background image!");
        // ����ʧ��ʱʹ��ԭ����ǳ��ɫ������Ϊ fallback
        auto fallbackBottom = LayerColor::create(Color4B(245, 245, 245, 255), visibleSize.width, bottomHeight);
        fallbackBottom->setPosition(origin.x, origin.y);
        this->addChild(fallbackBottom, -1);
    }

    // ��� Home ��ť
    auto homeButton = ui::Button::create("home_normal.png", "home_selected.png");
    homeButton->setPosition(Vec2(visibleSize.width - 120, visibleSize.height - 120));
    homeButton->addTouchEventListener([this](Ref*, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            this->onHomeButtonClicked();
        }
        });
    this->addChild(homeButton, 10);

    // ��ӳ��ذ�ť
    auto undoButton = ui::Button::create("back_normal.png", "back_selected.png");
    undoButton->setPosition(Vec2(visibleSize.width - 120, visibleSize.height - 1750));
    undoButton->addTouchEventListener([this](Ref*, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            CCLOG("Undo button clicked");
            // ������ӳ����߼�
            if (_controller) {
                _controller->handleUndo();
            }
        }
        });
    this->addChild(undoButton, 10);

    // ������Ƭ����
    this->createCardSprites();

    // ע�ᴥ���¼�������
    this->registerTouchEventHandlers();
}

// ������Ƭ����Ķ�������
void GameScene::createCardSprites() {
    // ��վ�������
    _topSprites.clear();
    _bottomSprites.clear();

    // Playfield ��Ƭ 
    auto deskcards = _controller->getPlayfieldCards();

    for (int i = 0; i < deskcards->size(); i++) {
        const auto& cardData = (*deskcards)[i];
        auto cardSprite = CardView::create(cardData);
        if (cardSprite) {
            cardSprite->setPosition(cardData.posX, cardData.posY + 500);
            this->addChild(cardSprite, 1); // ȷ����Ƭ��ͼ���Ϸ�
            // ���ж���ͼ�㾫�鶼��top card
            cardSprite->setIsTopCard(false);
            // �ײ�ͼ�������һ����������Ϊtop card

            _topSprites.push_back(cardSprite);
        }
        else {
            CCLOG("Error: Failed to create playfield card sprite at (%d, %d)", cardData.posX, cardData.posY);
        }
    }

    // Stack ��Ƭ
    auto handcards = _controller->getStackCards();
    for (int i = 0; i < handcards->size(); ++i) {
        const auto& cardData = (*handcards)[i];
        auto cardSprite = CardView::create(cardData);
        if (cardSprite) {
            cardSprite->setPosition(cardData.posX - 100, cardData.posY - 50);
            this->addChild(cardSprite, 2); // ȷ���ѵ���Ƭ�ڶ���ͼ���Ϸ�

            bool isTopCard = (i == handcards->size() - 1);
            log("12111111111111111111:   isTopCard: %d", isTopCard);
            cardSprite->setIsTopCard(isTopCard);


            // ����Ƕ����ƣ����õ�������
            if (isTopCard && _controller) {
                _controller->setCurrentTopCard(cardSprite);
            }
            _bottomSprites.push_back(cardSprite);
        }
        else {
            CCLOG("Error: Failed to create stack card sprite at (%d, %d)", cardData.posX, cardData.posY);
        }
    }
}

// ע�ᴥ���¼��������Ķ�������
void GameScene::registerTouchEventHandlers() {
    // ʹ��ȫ�ִ�����������������
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);

    touchListener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
        Vec2 touchLocation = touch->getLocation();

        // 1. ����Ƿ����˶������飨���ȴ���
        for (auto it = _topSprites.rbegin(); it != _topSprites.rend(); ++it) {
            auto sprite = *it;
            if (sprite && sprite->isVisible() && sprite->getBoundingBox().containsPoint(touchLocation)) {
                CCLOG("Touched top layer sprite");
                //this->handleTopLayerSpriteTouch(sprite);
                // ���ÿ����������鴥��
                if (_controller) {
                    _controller->handleDeskCardTouch(sprite);
                }
                return true; // �̵��¼�
            }
        }

        // 2. ����Ƿ����˵ײ�����
        for (auto it = _bottomSprites.rbegin(); it != _bottomSprites.rend(); ++it) {
            auto sprite = *it;
            if (sprite && sprite->isVisible() && sprite->getBoundingBox().containsPoint(touchLocation)) {
                CCLOG("Touched bottom layer sprite");
                //this->handleBottomLayerSpriteTouch(sprite);
                // ���ÿ����������鴥��
                if (_controller) {
                    _controller->handleHandCardTouch(sprite);
                }
                return true; // �̵��¼�
            }
        }
        return false;
    };

    // ʹ�ø����ȼ�ȷ����ť���ȴ���
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

// ����ͼ�㾫�鴥������
void GameScene::handleTopLayerSpriteTouch(CardView* sprite) {
    CCLOG("Top layer sprite touched: %p", sprite);
    // ������Ӷ�������ľ��崦���߼�
}

// �ײ�ͼ�㾫�鴥������
void GameScene::handleBottomLayerSpriteTouch(CardView* sprite) {
    CCLOG("Bottom layer sprite touched: %p", sprite);
    // ������ӵײ�����ľ��崦���߼�
}

// Home ��ť�������
void GameScene::onHomeButtonClicked() {
    CCLOG("Current Level Number: %d", this->_levelNumber);
    CCLOG("Back to menu");

    auto scene = LevelSelectScene::createScene();
    auto transition = TransitionFade::create(0.5f, scene);
    Director::getInstance()->replaceScene(transition);
}

GameScene::~GameScene() {
    CC_SAFE_DELETE(_controller);

    // ��վ����б�����Ҫɾ�����飬��Ϊ�����ǳ������ӽڵ㣩
    _topSprites.clear();
    _bottomSprites.clear();
}

void GameScene::setController(GameController* controller) {
    _controller = controller;
}

void GameScene::moveCardToTopLayer(CardView* card) {
    // �������������Ƴ�
    auto bottomIt = std::find(_bottomSprites.begin(), _bottomSprites.end(), card);
    if (bottomIt != _bottomSprites.end()) {
        _bottomSprites.erase(bottomIt);
    }

    // ��ӵ�����������
    auto topIt = std::find(_topSprites.begin(), _topSprites.end(), card);
    if (topIt == _topSprites.end()) {
        _topSprites.push_back(card);
    }

    card->setLocalZOrder(1);
}

void GameScene::moveCardToBottomLayer(CardView* card) {
    // ���������������Ƴ�
    auto topIt = std::find(_topSprites.begin(), _topSprites.end(), card);
    if (topIt != _topSprites.end()) {
        _topSprites.erase(topIt);
    }

    // ��ӵ���������
    auto bottomIt = std::find(_bottomSprites.begin(), _bottomSprites.end(), card);
    if (bottomIt == _bottomSprites.end()) {
        _bottomSprites.push_back(card);
    }

    card->setLocalZOrder(2);
}

