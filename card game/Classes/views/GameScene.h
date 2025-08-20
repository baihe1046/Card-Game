#pragma once
#pragma once
#include "cocos2d.h"
#include "controllers/GameController.h"  // 引入 GameController
#include "CardView.h"

class GameController;
class GameScene : public cocos2d::Scene {
public:
    // 用这个方法创建场景，并传入 levelNumber
    static cocos2d::Scene* createScene(int levelNumber);
    // 使用 CREATE_FUNC 宏生成 create() 方法
    CREATE_FUNC(GameScene);
    virtual bool init() override;
    ~GameScene();//析构函数

    void setController(GameController* controller);

    void createUI();
    void createCardSprites();
    void registerTouchEventHandlers();

    // 精灵触摸处理
    void handleTopLayerSpriteTouch(CardView* sprite);
    void handleBottomLayerSpriteTouch(CardView* sprite);

    // 按钮点击处理
    void onHomeButtonClicked();
    void moveCardToTopLayer(CardView* card);
    void moveCardToBottomLayer(CardView* card);
private:
    int _levelNumber = 0;
    cocos2d::LayerColor* _topLayer = nullptr;
    cocos2d::LayerColor* _bottomLayer = nullptr;

    GameController* _controller = nullptr; // 控制器

    // 分开存储顶部和底部的精灵
    std::vector<CardView*> _topSprites;    // 顶部图层精灵
    std::vector<CardView*> _bottomSprites; // 底部图层精灵
};