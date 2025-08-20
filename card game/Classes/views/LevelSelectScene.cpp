#include "LevelSelectScene.h"
#include "ui/CocosGUI.h"
#include "utils/JsonUtils.h"
#include "GameScene.h"
#include <string>

USING_NS_CC;

// 常量定义
const int LEVEL_COUNT = 6;         // 关卡总数
const int COLUMNS = 3;             // 列数
const int ROWS = 2;                // 行数
const float BUTTON_WIDTH = 160.0f;  // 按钮宽度
const float BUTTON_HEIGHT = 80.0f; // 按钮高度
const float SPACING_X = 180.0f;    // 水平间距
const float SPACING_Y = 200.0f;    // 垂直间距
const float TITLE_FONT_SIZE = 200.0f; // 标题字体大小
const float BUTTON_FONT_SIZE = 36.0f; // 按钮字体大小
const Color4B BG_COLOR = Color4B(240, 240, 240, 255); // 背景颜色(浅灰色)

Scene* LevelSelectScene::createScene() {
    auto scene = Scene::create();
    auto layer = LevelSelectScene::create();
    scene->addChild(layer);
    return scene;
}

bool LevelSelectScene::init() {
    if (!Scene::init()) {
        return false;
    }

    // 设置屏幕尺寸为 1080x2080
    auto winSize = Director::getInstance()->getWinSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 设置背景颜色为浅色
    auto background = LayerColor::create(BG_COLOR);
    this->addChild(background);

    // 添加标题
    auto titleLabel = Label::createWithTTF("Level Select", "fonts/Marker Felt.ttf", TITLE_FONT_SIZE);
    titleLabel->setPosition(Vec2(origin.x + winSize.width / 2,
        origin.y + winSize.height - TITLE_FONT_SIZE));
    titleLabel->setColor(Color3B(0, 0, 0)); // 设置标题颜色为黑色
    this->addChild(titleLabel, 1);

    //添加卡牌花色装饰
    auto star = Sprite::create("car.png");
    if (star) {
        star->setPosition(Vec2(origin.x + winSize.width / 2, origin.y + winSize.height - 300));
        this->addChild(star, 1);
    }

    // 居中计算起始位置
    float totalWidth = COLUMNS * BUTTON_WIDTH + (COLUMNS - 1) * SPACING_X;
    float totalHeight = ROWS * BUTTON_HEIGHT + (ROWS - 1) * SPACING_Y;
    float startX = origin.x + (winSize.width - totalWidth) / 2+80;
    float startY = origin.y + (winSize.height - totalHeight) / 2 + totalHeight / 2; // 动态计算垂直位置

    // 预加载按钮纹理
    Director::getInstance()->getTextureCache()->addImage("button_normal.png");
    Director::getInstance()->getTextureCache()->addImage("button_pressed.png");

    // 创建关卡按钮
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLUMNS; ++col) {
            int levelNumber = row * COLUMNS + col + 1;

            auto button = ui::Button::create(
                "button_normal.png",
                "button_pressed.png"
            );
            button->setContentSize(Size(BUTTON_WIDTH, BUTTON_HEIGHT));
            button->setTitleText(std::to_string(levelNumber));
            button->setTitleFontSize(BUTTON_FONT_SIZE);
            button->setTitleColor(Color3B(0, 0, 0)); // 按钮文字颜色

      // 计算按钮位置
      float posX = startX + col * (BUTTON_WIDTH + SPACING_X);
      float posY = startY - row * (BUTTON_HEIGHT + SPACING_Y);
      button->setPosition(Vec2(posX, posY));

      // 添加触摸事件监听器
      button->addTouchEventListener([this, levelNumber](Ref* sender, ui::Widget::TouchEventType type) {
          if (type == ui::Widget::TouchEventType::ENDED) {
              CCLOG("Selected Level: %d", levelNumber);

      // 验证关卡数据是否存在
      auto playfieldCards = JsonUtils::parsePlayfield(levelNumber);
      auto stackCards = JsonUtils::parseStack(levelNumber);

      if (playfieldCards && stackCards) {
          CCLOG("Level %d data loaded successfully", levelNumber);
          // 切换到游戏场景
          Director::getInstance()->pushScene(
              TransitionFade::create(0.5f, GameScene::createScene(levelNumber))
          );
      }
      else {
          CCLOG("Error: Failed to load data for level %d", levelNumber);
          // 可以添加一个提示信息
          auto alert = Label::createWithTTF("Level data not available", "fonts/Marker Felt.ttf", 30);
          alert->setPosition(Director::getInstance()->getWinSize() / 2);
          alert->setColor(Color3B(255, 0, 0));
          this->addChild(alert, 10);
          // 2秒后移除提示
          alert->runAction(Sequence::create(
              DelayTime::create(2.0f),
              RemoveSelf::create(),
              nullptr
          ));
      }
          }
          });

      this->addChild(button);
        }
    }

    return true;
}