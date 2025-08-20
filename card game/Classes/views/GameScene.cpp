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

    // 计算顶部和底部高度
    const float totalRatio = 150 + 58;
    float topHeight = visibleSize.height * (150 / totalRatio);
    float bottomHeight = visibleSize.height * (58 / totalRatio);

    // 创建顶部区域背景图片
    auto topBackground = Sprite::create("top_background.png");
    if (topBackground) {
        topBackground->setContentSize(Size(visibleSize.width, topHeight));
        topBackground->setPosition(Vec2(visibleSize.width / 2 + origin.x, origin.y + bottomHeight + topHeight / 2));
        this->addChild(topBackground, -1);
    }
    else {
        CCLOG("Error: Failed to load top background image!");
        // 加载失败时使用原来的黄色背景作为 fallback
        auto fallbackTop = LayerColor::create(Color4B(255, 255, 0, 255), visibleSize.width, topHeight);
        fallbackTop->setPosition(origin.x, origin.y + bottomHeight);
        this->addChild(fallbackTop, -1);
    }

    // 创建底部区域背景图片
    auto bottomBackground = Sprite::create("bottom_background.png");
    if (bottomBackground) {
        bottomBackground->setContentSize(Size(visibleSize.width, bottomHeight));
        bottomBackground->setPosition(Vec2(visibleSize.width / 2 + origin.x, origin.y + bottomHeight / 2));
        this->addChild(bottomBackground, -1);
    }
    else {
        CCLOG("Error: Failed to load bottom background image!");
        // 加载失败时使用原来的浅灰色背景作为 fallback
        auto fallbackBottom = LayerColor::create(Color4B(245, 245, 245, 255), visibleSize.width, bottomHeight);
        fallbackBottom->setPosition(origin.x, origin.y);
        this->addChild(fallbackBottom, -1);
    }

    // 添加 Home 按钮
    auto homeButton = ui::Button::create("home_normal.png", "home_selected.png");
    homeButton->setPosition(Vec2(visibleSize.width - 120, visibleSize.height - 120));
    homeButton->addTouchEventListener([this](Ref*, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            this->onHomeButtonClicked();
        }
        });
    this->addChild(homeButton, 10);

    // 添加撤回按钮
    auto undoButton = ui::Button::create("back_normal.png", "back_selected.png");
    undoButton->setPosition(Vec2(visibleSize.width - 120, visibleSize.height - 1750));
    undoButton->addTouchEventListener([this](Ref*, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            CCLOG("Undo button clicked");
            // 这里添加撤回逻辑
            if (_controller) {
                _controller->handleUndo();
            }
        }
        });
    this->addChild(undoButton, 10);

    // 创建卡片精灵
    this->createCardSprites();

    // 注册触摸事件监听器
    this->registerTouchEventHandlers();
}

// 创建卡片精灵的独立函数
void GameScene::createCardSprites() {
    // 清空精灵引用
    _topSprites.clear();
    _bottomSprites.clear();

    // Playfield 卡片 
    auto deskcards = _controller->getPlayfieldCards();

    for (int i = 0; i < deskcards->size(); i++) {
        const auto& cardData = (*deskcards)[i];
        auto cardSprite = CardView::create(cardData);
        if (cardSprite) {
            cardSprite->setPosition(cardData.posX, cardData.posY + 500);
            this->addChild(cardSprite, 1); // 确保卡片在图层上方
            // 所有顶部图层精灵都是top card
            cardSprite->setIsTopCard(false);
            // 底部图层中最后一个精灵设置为top card

            _topSprites.push_back(cardSprite);
        }
        else {
            CCLOG("Error: Failed to create playfield card sprite at (%d, %d)", cardData.posX, cardData.posY);
        }
    }

    // Stack 卡片
    auto handcards = _controller->getStackCards();
    for (int i = 0; i < handcards->size(); ++i) {
        const auto& cardData = (*handcards)[i];
        auto cardSprite = CardView::create(cardData);
        if (cardSprite) {
            cardSprite->setPosition(cardData.posX - 100, cardData.posY - 50);
            this->addChild(cardSprite, 2); // 确保堆叠卡片在顶部图层上方

            bool isTopCard = (i == handcards->size() - 1);
            log("12111111111111111111:   isTopCard: %d", isTopCard);
            cardSprite->setIsTopCard(isTopCard);


            // 如果是顶部牌，设置到控制器
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

// 注册触摸事件处理器的独立函数
void GameScene::registerTouchEventHandlers() {
    // 使用全局触摸监听器处理精灵点击
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);

    touchListener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
        Vec2 touchLocation = touch->getLocation();

        // 1. 检查是否点击了顶部精灵（优先处理）
        for (auto it = _topSprites.rbegin(); it != _topSprites.rend(); ++it) {
            auto sprite = *it;
            if (sprite && sprite->isVisible() && sprite->getBoundingBox().containsPoint(touchLocation)) {
                CCLOG("Touched top layer sprite");
                //this->handleTopLayerSpriteTouch(sprite);
                // 调用控制器处理精灵触摸
                if (_controller) {
                    _controller->handleDeskCardTouch(sprite);
                }
                return true; // 吞掉事件
            }
        }

        // 2. 检查是否点击了底部精灵
        for (auto it = _bottomSprites.rbegin(); it != _bottomSprites.rend(); ++it) {
            auto sprite = *it;
            if (sprite && sprite->isVisible() && sprite->getBoundingBox().containsPoint(touchLocation)) {
                CCLOG("Touched bottom layer sprite");
                //this->handleBottomLayerSpriteTouch(sprite);
                // 调用控制器处理精灵触摸
                if (_controller) {
                    _controller->handleHandCardTouch(sprite);
                }
                return true; // 吞掉事件
            }
        }
        return false;
    };

    // 使用高优先级确保按钮优先处理
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

// 顶部图层精灵触摸处理
void GameScene::handleTopLayerSpriteTouch(CardView* sprite) {
    CCLOG("Top layer sprite touched: %p", sprite);
    // 这里添加顶部精灵的具体处理逻辑
}

// 底部图层精灵触摸处理
void GameScene::handleBottomLayerSpriteTouch(CardView* sprite) {
    CCLOG("Bottom layer sprite touched: %p", sprite);
    // 这里添加底部精灵的具体处理逻辑
}

// Home 按钮点击处理
void GameScene::onHomeButtonClicked() {
    CCLOG("Current Level Number: %d", this->_levelNumber);
    CCLOG("Back to menu");

    auto scene = LevelSelectScene::createScene();
    auto transition = TransitionFade::create(0.5f, scene);
    Director::getInstance()->replaceScene(transition);
}

GameScene::~GameScene() {
    CC_SAFE_DELETE(_controller);

    // 清空精灵列表（不需要删除精灵，因为它们是场景的子节点）
    _topSprites.clear();
    _bottomSprites.clear();
}

void GameScene::setController(GameController* controller) {
    _controller = controller;
}

void GameScene::moveCardToTopLayer(CardView* card) {
    // 从手牌容器中移除
    auto bottomIt = std::find(_bottomSprites.begin(), _bottomSprites.end(), card);
    if (bottomIt != _bottomSprites.end()) {
        _bottomSprites.erase(bottomIt);
    }

    // 添加到桌面牌容器
    auto topIt = std::find(_topSprites.begin(), _topSprites.end(), card);
    if (topIt == _topSprites.end()) {
        _topSprites.push_back(card);
    }

    card->setLocalZOrder(1);
}

void GameScene::moveCardToBottomLayer(CardView* card) {
    // 从桌面牌容器中移除
    auto topIt = std::find(_topSprites.begin(), _topSprites.end(), card);
    if (topIt != _topSprites.end()) {
        _topSprites.erase(topIt);
    }

    // 添加到手牌容器
    auto bottomIt = std::find(_bottomSprites.begin(), _bottomSprites.end(), card);
    if (bottomIt == _bottomSprites.end()) {
        _bottomSprites.push_back(card);
    }

    card->setLocalZOrder(2);
}

